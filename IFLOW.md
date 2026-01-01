# etracker - 开源 BitTorrent Tracker 上下文文档

## 项目概述

**etracker** 是一个开源的 BitTorrent tracker 实现，致力于在最小资源占用下实现最大稳定性。这是一个用纯 C 编写的高性能 tracker 服务器。

### 核心特性

- **协议支持**：
  - BitTorrent 协议规范 (BEP-0003)
  - UDP Tracker 协议 (BEP-0015)
  - 紧凑对等体列表 (BEP-0023)
  - IPv6 Tracker 扩展 (BEP-0007)

- **网络协议**：
  - 支持 IPv4 和 IPv6
  - 同时支持 TCP 和 UDP 协议
  - 可变间隔（4-30 分钟，根据负载平均值动态调整）

- **性能指标**：
  - 单核 CPU 即可运行
  - 1200 RPS 时约 30% CPU 占用
  - 2000 RPS 时约 60% CPU 占用
  - 最小内存需求 20.9MB
  - 每 1M peers + 1M torrents 需额外 224MB

### 项目架构

**主要组件**：
- `server.c` - 主服务器入口
- `socket_tcp.c` / `socket_udp.c` - TCP/UDP 网络通信
- `thread_client_tcp.c` / `thread_client_udp.c` - 客户端线程处理
- `data.c` / `data_structure.c` - 数据管理和结构
- `list.c` - 自定义链表实现（支持哈希索引）
- `stats.c` - 统计信息收集
- `geoip.c` - 地理位置数据库支持
- `websocket.c` - WebSocket 支持（用于实时地图）

**数据结构**：
- 使用自定义的链表结构（`list.h`）管理 torrents 和 peers
- 基于 SHA-1 哈希（20 字节）进行索引
- 支持多级哈希表（0-2 级）
- 使用信号量实现线程安全

## 构建和运行

### 编译

**使用 Makefile（推荐）**：
```bash
make server        # 构建生产版本（优化）
make debug         # 构建调试版本（带 AddressSanitizer）
make tidy          # 构建带严格警告的版本
make all           # 构建服务器和客户端
```

**使用 CMake**：
```bash
cmake .
make
```

### 运行

**基本运行**：
```bash
./etracker -p 6969
```

**查看帮助**：
```bash
./etracker --help
```

**使用启动脚本**：
```bash
./run 6969  # 示例启动脚本
```

**Chroot 运行（Debian 10 示例）**：
```bash
./run_chroot 6969
```

### 命令行选项

- `-p, --port` - 端口号（同时用于 TCP 和 UDP）
- `-i, --interval` - 间隔时间（秒）
- `--min-interval` - 最小间隔
- `--max-interval` - 最大间隔
- `-k, --keep-alive` - 启用 HTTP keep-alive
- `--no-tcp` - 禁用 TCP
- `--no-udp` - 禁用 UDP
- `--no-locations` - 禁用地理位置数据库
- `--charset` - 字符集（默认 utf-8）
- `--locale` - 区域设置（默认 en_US.UTF-8）
- `--nofile` - 文件描述符限制
- `--core` - 核心转储限制
- `-a, --max-load-avg` - 最大负载平均值
- `-e, --socket-timeout` - Socket 超时时间
- `-f, --failed` - 失败重启次数
- `-w, --workers` - 工作线程数
- `-m, --max-peers-per-response` - 每个响应的最大对等体数
- `--x-forwarded-for` - X-Forwarded-For 支持

## 测试

### 自动化测试

**运行单元测试**：
```bash
./aauto_tests_run
```

或手动运行：
```bash
make test && ./test.o
```

测试覆盖：
- 数据结构测试
- GeoIP 测试
- SHA-1 测试
- Base64 测试
- 信号量测试
- 链表测试
- 块测试

### 功能测试

**运行功能测试**：
```bash
./ffunctional_tests_run
```

功能测试包括：
- IPv4 TCP/UDP announce 和 scrape
- IPv6 TCP/UDP announce 和 scrape
- 混合协议测试
- 紧凑模式测试
- Peer 事件（started/stopped）测试

测试用例位于 `functional_tests/` 目录，包含 71 个测试场景。

## Web 界面

### 接口

- **Announce**: `http://host:port/announce` 或 `udp://host:port/announce`
- **Scrape**: `http://host:port/scrape` 或 `udp://host:port/scrape`

### Web 页面

1. **统计页面**: `http://host:port/stats.html`
   - 显示服务器统计信息、负载、RPS、内存使用等

2. **实时世界地图**: `http://host:port/map.html`
   - 需要下载 IP2LOCATION-LITE-DB5.CSV 数据库
   - 显示全球 peer 分布的实时地图

3. **设置页面**: `http://host:port/settings.html`
   - 需要先运行 `./initWeb` 下载 jQuery

### 初始化 Web 资源

```bash
./initWeb  # 下载 jQuery 到 web/js/
```

## 开发约定

### 代码风格

- **语言**: 纯 C（C 标准）
- **编译器**: GCC（支持 Apple LLVM）
- **编码风格**:
  - 使用小写字母和下划线命名函数和变量
  - 头文件使用宏保护（`#ifndef SC6_FILENAME_H`）
  - 使用自定义的内存管理（`alloc.c`）
  - 使用自定义的字符串处理（`string.c`）
  - 使用自定义的块缓冲（`block.c`）

### 内存管理

- 使用自定义的分配器（`alloc.c`）替代标准 `malloc/calloc`
- 使用信号量（`sem.c`）进行精确的内存块计数
- 实现垃圾回收机制（`data_garbage.c`, `socket_garbage.c`）

### 线程模型

- 使用 POSIX 线程（`pthread`）
- 每个工作线程处理一组 socket
- 使用信号量保护共享数据结构
- 支持多级哈希表以减少锁竞争

### 错误处理

- 使用自定义退出码系统（`exit_code.c`）
- 统计错误信息（`stats.c`）
- 详细的错误日志记录

### 配置选项

**编译时选项**：
- `DATA_FULL_SCRAPE_ENABLE` - 启用完整 scrape（在 `data.h` 中设置）

**运行时配置**：
- 通过命令行参数配置
- 支持动态调整间隔时间

## 项目结构

```
etracker/
├── server.c              # 主服务器入口
├── client.c              # 测试客户端
├── socket_tcp.c/h        # TCP socket 处理
├── socket_udp.c/h        # UDP socket 处理
├── thread_client_tcp.c/h # TCP 客户端线程
├── thread_client_udp.c/h # UDP 客户端线程
├── data.c/h              # 数据处理逻辑
├── data_structure.c/h    # 数据结构定义
├── data_garbage.c/h      # 垃圾回收
├── list.c/h              # 自定义链表
├── stats.c/h             # 统计信息
├── geoip.c/h             # 地理位置支持
├── websocket.c/h         # WebSocket 支持
├── alloc.c/h             # 内存分配
├── string.c/h            # 字符串处理
├── block.c/h             # 块缓冲
├── sem.c/h               # 信号量
├── thread.c/h            # 线程管理
├── interval.c/h          # 间隔管理
├── rps.c/h               # 请求/秒统计
├── uri.c/h               # URI 解析
├── base64.c/h            # Base64 编码
├── sha1.c/h              # SHA-1 哈希
├── math.c/h              # 数学工具
├── argument.c/h          # 命令行参数解析
├── exit_code.c/h         # 退出码
├── Makefile              # Make 构建配置
├── CMakeLists.txt        # CMake 构建配置
├── run                   # 启动脚本
├── run_chroot            # Chroot 启动脚本
├── initWeb               # Web 资源初始化
├── aauto_tests_run       # 自动化测试脚本
├── ffunctional_tests_run # 功能测试脚本
├── test.c                # 单元测试
├── web/                  # Web 界面资源
│   ├── map.html          # 世界地图
│   ├── stats.html        # 统计页面
│   ├── settings.html     # 设置页面
│   ├── css/              # 样式文件
│   ├── js/               # JavaScript 文件
│   └── images/           # 图片资源
├── functional_tests/     # 功能测试用例
├── origin-*/             # 原始实现参考
└── tests/                # 测试文件
```

## 平台支持

已测试平台：
- Debian 10.5, GCC 8.3.0
- macOS 10.11.6, Apple LLVM 8.0.0

## 依赖

- **编译时**: GCC 或 Clang, make 或 cmake
- **运行时**: 无外部依赖（纯 C 实现）
- **可选功能**:
  - IP2LOCATION-LITE-DB5.CSV（用于地理位置显示）
  - jQuery（通过 `./initWeb` 下载）

## 性能优化

- 使用 `-march=native -O2 -pipe` 优化标志
- 自定义内存分配器减少开销
- 多级哈希表减少锁竞争
- 紧凑的数据结构（torrent 116B, peer 108B）
- 支持紧凑 peer 列表减少网络传输

## 安全考虑

- 支持 chroot 运行
- 可配置文件描述符限制
- 可配置核心转储限制
- 支持 X-Forwarded-For 头处理
- Socket 超时机制

## 相关资源

- **GitHub**: https://github.com/truekenny/etracker
- **类似项目**: opentracker
- **BitTorrent 协议规范**: https://www.bittorrent.org/beps/

## 注意事项

1. **Locale 配置**: 如需使用特定语言环境，请参考 `HOW_TO_ADD_LOCALE.md`
2. **地理位置数据库**: 世界地图功能需要下载 IP2LOCATION-LITE-DB5.CSV
3. **文件描述符限制**: 高并发场景下需要增加 `nofile` 限制（建议 64000）
4. **内存规划**: 根据 peers 和 torrents 数量规划内存使用
5. **负载监控**: 根据负载平均值动态调整间隔时间

## Git 提交规范

- 使用英文提交信息
- 保持提交信息简洁明确
- 参考 `.iflow/IFLOW.md` 中的指导