## etracker - Open-Source BitTorrent Tracker

**etracker** aims for maximum stability and minimal resource usage.

### Protocol Support

etracker implements the following BitTorrent Enhancement Proposals (BEPs):

- [BEP-0003: The BitTorrent Protocol Specification](https://www.bittorrent.org/beps/bep_0003.html)
- [BEP-0015: UDP Tracker Protocol for BitTorrent](https://www.bittorrent.org/beps/bep_0015.html)
- [BEP-0023: Tracker Returns Compact Peer Lists](https://www.bittorrent.org/beps/bep_0023.html)
- [BEP-0007: IPv6 Tracker Extension](https://www.bittorrent.org/beps/bep_0007.html)

### Features

| Protocol | IPv4 | IPv6 |
|----------|------|------|
| **TCP**  | ✓    | ✓    |
| **UDP**  | ✓    | ✓    |

- Announce and Scrape support
- Variable interval (4-30 minutes, dynamically adjusted based on load average)
- Compact peer lists
- HTTP keep-alive support

## Installation

### Building from Source

```bash
git clone https://github.com/truekenny/etracker
cd etracker
cmake -B build
cmake --build build
```

This will build the following executables in the `build/` directory:
- `etracker` - Main tracker server
- `client` - Test client
- `test` - Unit tests

### Build Options

- **Debug mode**: `cmake -DCMAKE_BUILD_TYPE=Debug -B build`
- **Release mode**: `cmake -DCMAKE_BUILD_TYPE=Release -B build` (includes optimization flags)

## Usage

### Basic Usage

```bash
./build/etracker -p 6969
```

The port is used simultaneously for both TCP and UDP protocols.

### Command Line Options

```bash
./build/etracker --help
```

Available options:
- `-p, --port PORT` - Port number (default: 3000)
- `-i, --interval SECONDS` - Announce interval in seconds (default: 1799)
- `--min-interval SECONDS` - Minimum interval (default: 239)
- `--max-interval SECONDS` - Maximum interval (default: 1799)
- `-k, --keep-alive` - Enable HTTP keep-alive
- `--no-tcp` - Disable TCP
- `--no-udp` - Disable UDP
- `--charset CHARSET` - Character set (default: utf-8)
- `--locale LOCALE` - Locale setting (default: en_US.UTF-8)
- `-a, --max-load-avg LOAD` - Maximum load average
- `-e, --socket-timeout SECONDS` - Socket timeout (default: 3)
- `-f, --failed COUNT` - Number of restart attempts on failure
- `-w, --workers NUM` - Number of worker threads (default: CPU cores)
- `-m, --max-peers-per-response NUM` - Maximum peers per response (default: 60)
- `--x-forwarded-for` - Enable X-Forwarded-For header support

### Startup Scripts

#### Standard Startup

File: [scripts/run](scripts/run)  
Example: `./scripts/run 6969`

#### Chroot Startup (Debian 10)

File: [scripts/run_chroot](scripts/run_chroot)  
Example: `./scripts/run_chroot 6969`

## Tracker Interfaces

### Announce

```
http://host:port/announce
udp://host:port/announce
```

### Scrape

```
http://host:port/scrape
udp://host:port/scrape
```

## Configuration

### Optional Full Scrape

Enable full scrape support by modifying `include/data.h`:
```c
#define DATA_FULL_SCRAPE_ENABLE 1
```

## Testing

### Unit Tests

```bash
./scripts/aauto_tests_run
```

Or manually:
```bash
cd build && ./test
```

### Functional Tests

```bash
./scripts/ffunctional_tests_run
```

Includes 71 test scenarios covering:
- IPv4/IPv6 TCP/UDP announce and scrape
- Mixed protocol tests
- Compact mode tests
- Peer event tests (started/stopped)

## Performance

### CPU Usage

All you need is a single CPU core.

| Requests per Second | Load Avg (Single Core Xeon 2100MHz) |
|---------------------|--------------------------------------|
| 1200                | ~30%                                 |
| 2000                | ~60%                                 |

### Memory Usage

**Minimum requirement**: 20.9MB  
**Additional**: 224MB per 1M peers + 1M torrents

**Example**: Rutracker.org (7M peers + 2M torrents) requires 988MB.

#### Memory Breakdown

| Component | Size |
|-----------|------|
| list structure | 32B |
| item structure | 40B |
| dataTorrent structure | 24B |
| dataPeer structure | 48B |
| hash | 20B |
| torrent (total) | 116B |
| peer (total) | 108B |

#### Memory Usage Examples

| Scenario | Memory |
|----------|--------|
| Startup | 20.9MB |
| 1M peers + 1M torrents | 224MB |
| 7M peers + 2M torrents | 988MB |

## Platforms Tested

- **Debian 10.5**, GCC version 8.3.0
- **macOS 10.11.6**, Apple LLVM version 8.0.0

## Comparison with Similar Projects

### opentracker

![opentracker comparison](https://raw.githubusercontent.com/truekenny/etracker/master/Pictures/opentracker.png)

[opentracker](https://github.com/masroore/opentracker) is a similar BitTorrent tracker implementation.

## Example Output

### Terminal Output

```
Revision: cd6bc1d
Starting configuration:
  port = 80
  interval = 239
  workers = 1
  maxPeersPerResponse = 300
  socketTimeout = 3
  keepAlive = 1
  minInterval = 239
  maxInterval = 1799
  noTcp = 0
  noUdp = 0
  charset = utf-8
  locale = en_US.UTF-8
This system has 1 processors available.
Current 7 -> soft=1024, hard=1048576
New 7 -> soft=64000, hard=1048576
Current 4 -> soft=0, hard=18446744073709551615
New 4 -> soft=18446744073709551615, hard=18446744073709551615
Starting UDP worker 0/0
Waiting UDP for incoming packets...
Starting TCP worker 0/0
Waiting TCP for incoming connections...
Join TCP Thread
Sat Oct 17 15:51:09    2969 TP    2792 TT       0 TL      12 MPT      12 MPL       3 MTL       0 RP       0 RT    4581 µs  LA: 0.95 0.56 0.47  ML: 0.50  I: 239 (239->239) s  RPS: 2741/0/0/13           
Sat Oct 17 15:55:08  333225 TP  193966 TT      13 TL     500 MPT     245 MPL      25 MTL       0 RP     649 RT   97638 µs  LA: 0.55 0.52 0.46  ML: 0.50  I: 239 (239->239) s  RPS: 2071/8/0/4            
Sat Oct 17 15:59:07  428935 TP  256066 TT      19 TL     647 MPT     252 MPL      33 MTL       0 RP     728 RT   99353 µs  LA: 0.48 0.56 0.50  ML: 0.50  I: 239 (239->239) s  RPS: 1859/9/0/17           
Sat Oct 17 16:03:06  485953 TP  299767 TT      22 TL     715 MPT     254 MPL     100 MTL       0 RP     851 RT  106837 µs  LA: 0.95 0.72 0.57  ML: 0.50  I: 239 (239->299) s  RPS: 1869/5/0/24           
Sat Oct 17 16:08:05  553297 TP  351550 TT      26 TL     793 MPT     254 MPL     100 MTL       0 RP     910 RT  117971 µs  LA: 0.53 0.59 0.55  ML: 0.50  I: 299 (299->359) s  RPS: 1574/4/0/30           
Sat Oct 17 16:14:04  631267 TP  411342 TT      29 TL     885 MPT     249 MPL     100 MTL       0 RP    1114 RT  128539 µs  LA: 0.40 0.53 0.54  ML: 0.50  I: 359 (359->359) s  RPS: 1240/5/1/10           
```

**Legend**:
- `TP` – Total Peers
- `TT` – Total Torrents
- `TL` – Torrents with expanded peer's list
- `MPT` – Max Peers for single torrent
- `MPL` – Max Peers for single list
- `MTL` – Max Torrents for single list
- `RP` – Removed Peers by garbage collector
- `RT` – Removed Torrents by garbage collector
- `µs` – Time spent on garbage collect
- `LA` – Load Average
- `ML` – Max Allowed Load Average
- `I` – Interval
- `RPS` – Requests per second (IPv4-TCP/IPv4-UDP/IPv6-TCP/IPv6-UDP)

### Stats Page Output

```
start_time = Sat Oct 17 15:51:05 2020 (0d) (0f)
thread_number = 0

load_avg = 0.63 0.59 0.56
interval = 359 (299->359)
active_sockets = 1,555 (rlimit 64,000/1,048,576)

requests_per_second ipv4 = tcp: 1707, udp: 1
requests_per_second ipv6 = tcp: 0, udp: 19

rusage.ru_maxrss =      195,556

malloc        =       6,339,693
calloc        =      10,358,931
*alloc        =      16,698,624
free          =      13,152,206
*alloc - free =       3,546,418

stats.http_200 =    2,360,400
stats.http_400 =       25,951
stats.http_403 =            3 (Full Scrape)
stats.http_404 =           10
stats.http_405 =            3 (Not GET)
stats.http_408 =      294,753 (Timeout)
stats.http_413 =            9 (Oversize)

stats.close_pass  =    2,465,274
stats.send_pass   =    2,681,114
stats.recv_pass   =    2,386,368
stats.accept_pass =    2,466,829

stats.close_failed  =            0
stats.send_failed   =           15
stats.recv_failed   =        6,900
stats.accept_failed =            1

stats.recv_failed_read_0         =    1,376,203
stats.recv_failed_read_sub_0     =        6,891
stats.recv_failed_read_not_equal =            0

stats.send_pass_udp =       40,572
stats.recv_pass_udp =       40,572

stats.send_failed_udp =            0
stats.recv_failed_udp =            0

stats.keep_alive    =    1,598,949
stats.no_keep_alive =      787,415

stats.sent_bytes =     811,669,852
stats.recv_bytes =     844,073,045

stats.sent_bytes_udp =       1,294,000
stats.recv_bytes_udp =       3,259,887

stats.announce =    2,240,429
stats.scrape   =      145,909

stats.connect_udp  =        9,935
stats.announce_udp =       30,290
stats.scrape_udp   =          347

close_errno:
send_errno:
  errno =  32 count =         3 name = 'Broken pipe'
  errno = 104 count =        12 name = 'Connection reset by peer'
recv_errno:
  errno = 104 count =     6,891 name = 'Connection reset by peer'
accept_errno:
  errno = 103 count =         1 name = 'Software caused connection abort'
send_errno_udp:
recv_errno_udp:
```

## Project Files

### Scripts

- [`scripts/aauto_tests_run`](scripts/aauto_tests_run) – Run automated unit tests
- [`scripts/ffunctional_tests_run`](scripts/ffunctional_tests_run) – Run functional tests
- [`scripts/run`](scripts/run) – Example server startup script
- [`scripts/run_chroot`](scripts/run_chroot) – Example chroot startup script

### Documentation

- [`CMakeLists.txt`](CMakeLists.txt) – CMake build configuration
- [`LICENSE`](LICENSE) – License information
- [`README.md`](README.md) – This file

## Author

I am not a C developer. This application is my hobby and my first program in C.

## License

See [`LICENSE`](LICENSE) file for details.