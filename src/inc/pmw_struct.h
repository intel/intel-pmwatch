/*
 * MIT License
 *
 * Copyright (C) 2019 Intel Corporation. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef _PMW_STRUCT_H_INC_
#define _PMW_STRUCT_H_INC_

#include <stdint.h>
#include <stdbool.h>

typedef struct PMWATCH_CONFIG_NODE_S  PMWATCH_CONFIG_NODE;
typedef        PMWATCH_CONFIG_NODE   *PMWATCH_CONFIG;

struct PMWATCH_CONFIG_NODE_S
{
    double interval;
    double health_interval;
    bool   collect_health;
    bool   collect_perf_metrics;
};

#define PMWATCH_CONFIG_INTERVAL(x)             (x)->interval
#define PMWATCH_CONFIG_HEALTH_INTERVAL(x)      (x)->health_interval
#define PMWATCH_CONFIG_COLLECT_HEALTH(x)       (x)->collect_health
#define PMWATCH_CONFIG_COLLECT_PERF_METRICS(x) (x)->collect_perf_metrics

typedef struct PMWATCH_OP_BUF_NODE_S  PMWATCH_OP_BUF_NODE;
typedef        PMWATCH_OP_BUF_NODE   *PMWATCH_OP_BUF;

struct PMWATCH_OP_BUF_NODE_S
{
    uint64_t      epoch;
    uint64_t      timestamp;
    uint64_t      total_bytes_read;
    uint64_t      total_bytes_written;
    double        read_hit_ratio;
    double        write_hit_ratio;
    uint64_t      media_read;
    uint64_t      media_write;
    uint64_t      bytes_read;
    uint64_t      bytes_written;
    uint64_t      host_reads;
    uint64_t      host_writes;
    unsigned char health_status;
    unsigned char percentage_remaining;
    unsigned char percentage_used;
    uint64_t      power_on_time;
    uint64_t      uptime;
    uint64_t      last_shutdown_time;
    double        media_temp;
    double        controller_temp;
    double        max_media_temp;
    double        max_controller_temp;
};

#define PMWATCH_OP_BUF_EPOCH(x)                (x)->epoch
#define PMWATCH_OP_BUF_TIMESTAMP(x)            (x)->timestamp
#define PMWATCH_OP_BUF_TOTAL_BYTES_READ(x)     (x)->total_bytes_read
#define PMWATCH_OP_BUF_TOTAL_BYTES_WRITTEN(x)  (x)->total_bytes_written
#define PMWATCH_OP_BUF_READ_HIT_RATIO(x)       (x)->read_hit_ratio
#define PMWATCH_OP_BUF_WRITE_HIT_RATIO(x)      (x)->write_hit_ratio
#define PMWATCH_OP_BUF_MEDIA_READ(x)           (x)->media_read
#define PMWATCH_OP_BUF_MEDIA_WRITE(x)          (x)->media_write
#define PMWATCH_OP_BUF_BYTES_READ(x)           (x)->bytes_read
#define PMWATCH_OP_BUF_BYTES_WRITTEN(x)        (x)->bytes_written
#define PMWATCH_OP_BUF_HOST_READS(x)           (x)->host_reads
#define PMWATCH_OP_BUF_HOST_WRITES(x)          (x)->host_writes
#define PMWATCH_OP_BUF_HEALTH_STATUS(x)        (x)->health_status
#define PMWATCH_OP_BUF_PERCENTAGE_REMAINING(x) (x)->percentage_remaining
#define PMWATCH_OP_BUF_PERCENTAGE_USED(x)      (x)->percentage_used
#define PMWATCH_OP_POWER_ON_TIME(x)            (x)->power_on_time
#define PMWATCH_OP_BUF_UPTIME(x)               (x)->uptime
#define PMWATCH_OP_BUF_LAST_SHUTDOWN_TIME(x)   (x)->last_shutdown_time
#define PMWATCH_OP_BUF_MEDIA_TEMP(x)           (x)->media_temp
#define PMWATCH_OP_BUF_CONTROLLER_TEMP(x)      (x)->controller_temp
#define PMWATCH_OP_BUF_MAX_MEDIA_TEMP(x)       (x)->max_media_temp
#define PMWATCH_OP_BUF_MAX_CONTROLLER_TEMP(x)  (x)->max_controller_temp

#endif
