#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include "rps.h"
#include "thread.h"

// Замеряемый период
#define RPS_PERIOD_S 1

#define RPS_TIME_EVEN 0
#define RPS_TIME_ODD  1

unsigned char getRpsIndex(unsigned char protocol, unsigned char ipVersion);

void updateRps(struct rps *rps, unsigned char protocol, unsigned char ipVersion) {
    unsigned char index = getRpsIndex(protocol, ipVersion);

    if (rps->status == RPS_TIME_ODD) {
        unsigned int new_value = atomic_fetch_add(&rps->odd[index], 1) + 1;
        unsigned int current_max = atomic_load(&rps->max[index]);
        if (new_value > current_max) {
            atomic_compare_exchange_strong(&rps->max[index], &current_max, new_value);
        }
    } else {
        unsigned int new_value = atomic_fetch_add(&rps->even[index], 1) + 1;
        unsigned int current_max = atomic_load(&rps->max[index]);
        if (new_value > current_max) {
            atomic_compare_exchange_strong(&rps->max[index], &current_max, new_value);
        }
    }
}

float getRps(struct rps *rps, unsigned char protocol, unsigned char ipVersion) {
    unsigned char index = getRpsIndex(protocol, ipVersion);

    unsigned int value;
    if (atomic_load(&rps->status) == RPS_TIME_ODD) {
        value = atomic_load(&rps->even[index]);
    } else {
        value = atomic_load(&rps->odd[index]);
    }

    float rpm = (float) value;
    return rpm / RPS_PERIOD_S;
}

unsigned char getRpsIndex(unsigned char protocol, unsigned char ipVersion) {
    return protocol + ipVersion;
}

void resetMaxRps(struct rps *rps) {
    for (int index = 0; index < RPS_DIFFERENT_VALUES; ++index) {
        atomic_store(&rps->max[index], 0);
    }
}

void *rpsStatusHandler(struct rps *rps) {
    pthreadSetName(pthread_self(), "RPS Status");

    while (1) {
        if (rps->status == RPS_TIME_EVEN) {
            for (int index = 0; index < RPS_DIFFERENT_VALUES; ++index) {
                atomic_store(&rps->odd[index], 0);
            }
            atomic_store(&rps->status, RPS_TIME_ODD);
        } else {
            for (int index = 0; index < RPS_DIFFERENT_VALUES; ++index) {
                atomic_store(&rps->even[index], 0);
            }
            atomic_store(&rps->status, RPS_TIME_EVEN);
        }

        sleep(RPS_PERIOD_S);
        // Чтобы нормально работала подсветка кода в IDE
        if (rand() % 2 == 3) break;
    }

    return 0;
}

void runRpsStatusThread(struct rps *rps) {
    pthread_t tid;
    pthread_create(&tid, NULL, (void *(*)(void *)) rpsStatusHandler, rps);
}
