/*
 * MIT License
 *
 * Copyright (C) 2017 - 2019 Intel Corporation. All rights reserved.
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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#ifdef _WIN32
#define PMW_DLL_EXPORTS 1
#else
#include <unistd.h>
#include <pthread.h>
#endif

#include "nvm_types.h"
#include "nvm_management.h"

#include "inc/pmw_utils.h"
#include "inc/pmw_version.h"
#include "inc/pmw_comm.h"
#include "inc/pmw_collect.h"

FILE     *fout;
int       loops = 0, infinite_run = 0;
uint64_t  usec_sample_time, usec_sample_time_prev;
int       collect_lifetime   = 0;
char      DELIMITER[]        = ";";
int       output_format_one  = 1;
int       output_format_two  = 0;
int       collect_health     = 0;
char     *filename;
#ifndef _WIN32
pthread_t thread_id;
int       using_api          = 0;
static int write_count       = 0;
static int collection_done   = 0;
static int thread_status     = 0;
PMWATCH_OP_BUF output_buf    = NULL;
pthread_mutex_t buffer_lock;
#endif

NVM_UINT32
PMW_COLLECT_Set_Global_Vars (
    FILE     *l_fout,
    int       l_loops,
    int       l_infinite_run,
    uint64_t  l_usec_sample_time,
    uint64_t  l_usec_sample_time_prev,
    int       l_collect_lifetime,
    int       l_output_format_one,
    int       l_output_format_two,
    int       l_collect_health,
    char     *l_delim
)
{
    NVM_UINT32 ret_val = NVM_SUCCESS;

    fout                  = l_fout;
    loops                 = l_loops;
    infinite_run          = l_infinite_run;
    usec_sample_time      = l_usec_sample_time;
    usec_sample_time_prev = l_usec_sample_time_prev;
    collect_lifetime      = l_collect_lifetime;
    output_format_one     = l_output_format_one;
    output_format_two     = l_output_format_two;
    collect_health        = l_collect_health;

    ret_val = STRNCPY_SAFE(DELIMITER, sizeof(DELIMITER), l_delim);

    return ret_val;
}

void print_header_format_one (
    int  count
)
{
    int i;

    // 1st header
    fprintf(fout, "timestamp%s%s", DELIMITER, DELIMITER);
    for (i = 0; i < count; i++){
        fprintf(fout, "DIMM%d%s%s%s%s%s%s%s%s%s%s", i, DELIMITER, DELIMITER, \
            DELIMITER, DELIMITER, DELIMITER, DELIMITER, DELIMITER, DELIMITER, DELIMITER, DELIMITER);
    }
    fprintf(fout, "\n");
    fflush(fout);

    // 2nd header
    fprintf(fout, "epoch%stimestamp%s", DELIMITER, DELIMITER);
    for (i = 0; i < count; i++){
        fprintf(fout, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
            "bytes_read (derived)",
            DELIMITER,
            "bytes_written (derived)",
            DELIMITER,
            "read_hit_ratio (derived)",
            DELIMITER,
            "write_hit_ratio (derived)",
            DELIMITER,
            "media_read_ops (derived)",
            DELIMITER,
            "media_write_ops (derived)",
            DELIMITER,
            "read_64B_ops_received",
            DELIMITER,
            "write_64B_ops_received",
            DELIMITER,
            "cpu_read_ops",
            DELIMITER,
            "cpu_write_ops",
            DELIMITER
        );
    }
    fprintf(fout, "\n");
    fflush(fout);
}

void print_header_format_two (
    int  count
)
{
    struct device_discovery *lp_DIMM_devices;
    int num_packages, itr_p, itr_d;

    lp_DIMM_devices = PMW_COMM_Get_DIMM_Topology();

    // get the total number of packages
    num_packages = lp_DIMM_devices[count - 1].socket_id;
    ++num_packages;

    // header level 1
    fprintf(fout, "timestamp%s%s", DELIMITER, DELIMITER);
    for (itr_p = 0; itr_p < num_packages; itr_p++) {
        for (itr_d = 0; itr_d < count; itr_d++) {
            if (itr_p == lp_DIMM_devices[itr_d].socket_id) {
                fprintf(fout, "Package%d%sPackage%d%sPackage%d%sPackage%d%s", itr_p, DELIMITER, \
                               itr_p, DELIMITER, itr_p, DELIMITER, itr_p, DELIMITER);
            }
        }
        for (itr_d = 0; itr_d < count; itr_d++) {
            if (itr_p == lp_DIMM_devices[itr_d].socket_id) {
                fprintf(fout, "Package%d%sPackage%d%s", itr_p, DELIMITER, \
                               itr_p, DELIMITER);
            }
            
        }
        for (itr_d = 0; itr_d < count; itr_d++) {
            if (itr_p == lp_DIMM_devices[itr_d].socket_id) {
                fprintf(fout, "Package%d%sPackage%d%s", itr_p, DELIMITER, \
                               itr_p, DELIMITER);
            }
        }
        for (itr_d = 0; itr_d < count; itr_d++) {
            if (itr_p == lp_DIMM_devices[itr_d].socket_id) {
                fprintf(fout, "Package%d%sPackage%d%s", itr_p, DELIMITER, itr_p, DELIMITER);
            }
        }
    }
    fprintf(fout, "\n");
    fflush(fout);

    // header level 2
    fprintf(fout, "%s%s", DELIMITER, DELIMITER);
    for (itr_p = 0; itr_p < num_packages; itr_p++) {
        for (itr_d = 0; itr_d < count; itr_d++) {
            if (itr_p == lp_DIMM_devices[itr_d].socket_id) {
                fprintf(fout, "Media%sMedia%sMedia%sMedia%s", DELIMITER, \
                               DELIMITER, DELIMITER, DELIMITER);
            }
        }
        for (itr_d = 0; itr_d < count; itr_d++) {
            if (itr_p == lp_DIMM_devices[itr_d].socket_id) {
                fprintf(fout, "Buffer%sBuffer%s", DELIMITER, \
                               DELIMITER);
            }
        }
        for (itr_d = 0; itr_d < count; itr_d++) {
            if (itr_p == lp_DIMM_devices[itr_d].socket_id) {
                fprintf(fout, "Controller%sController%s", DELIMITER, DELIMITER);
            }
        }
        for (itr_d = 0; itr_d < count; itr_d++) {
            if (itr_p == lp_DIMM_devices[itr_d].socket_id) {
                fprintf(fout, "CPU%sCPU%s", DELIMITER, DELIMITER);
            }
        }
    }
    fprintf(fout, "\n");
    fflush(fout);

    //header level 3
    fprintf(fout, "epoch%stimestamp%s", DELIMITER, DELIMITER);
    for (itr_p = 0; itr_p < num_packages; itr_p++) {
        for (itr_d = 0; itr_d < count; itr_d++) {
            if (itr_p == lp_DIMM_devices[itr_d].socket_id) {
                fprintf(fout, "DIMM%d%sDIMM%d%sDIMM%d%sDIMM%d%s", itr_d, DELIMITER, itr_d, DELIMITER, \
                               itr_d, DELIMITER, itr_d, DELIMITER);
            }
        }
        for (itr_d = 0; itr_d < count; itr_d++) {
            if (itr_p == lp_DIMM_devices[itr_d].socket_id) {
                fprintf(fout, "DIMM%d%sDIMM%d%s", itr_d, DELIMITER, itr_d, DELIMITER);
            }
        }
        for (itr_d = 0; itr_d < count; itr_d++) {
            if (itr_p == lp_DIMM_devices[itr_d].socket_id) {
                fprintf(fout, "DIMM%d%sDIMM%d%s", itr_d, DELIMITER, itr_d, DELIMITER);
            }
        }
        for (itr_d = 0; itr_d < count; itr_d++) {
            if (itr_p == lp_DIMM_devices[itr_d].socket_id) {
                fprintf(fout, "DIMM%d%sDIMM%d%s", itr_d, DELIMITER, itr_d, DELIMITER);
            }
        }
    }
    fprintf(fout, "\n");
    fflush(fout);

    //header level 4
    fprintf(fout, "%s%s", DELIMITER, DELIMITER);
    for (itr_p = 0; itr_p < num_packages; itr_p++) {
        for (itr_d = 0; itr_d < count; itr_d++) {
            if (itr_p == lp_DIMM_devices[itr_d].socket_id) {
                fprintf(fout, "bytes_read (derived)%sbytes_written (derived)%smedia_read_ops (derived)%smedia_write_ops (derived)%s", \
                               DELIMITER, DELIMITER, DELIMITER, DELIMITER);
            }
        }
        for (itr_d = 0; itr_d < count; itr_d++) {
            if (itr_p == lp_DIMM_devices[itr_d].socket_id) {
                fprintf(fout, "read_hit_ratio (derived)%swrite_hit_ratio (derived)%s", DELIMITER, DELIMITER);
            }
        }
        for (itr_d = 0; itr_d < count; itr_d++) {
            if (itr_p == lp_DIMM_devices[itr_d].socket_id) {
                fprintf(fout, "read_64B_ops_received%swrite_64B_ops_received%s", \
                               DELIMITER, DELIMITER);
            }
        }
        for (itr_d = 0; itr_d < count; itr_d++) {
            if (itr_p == lp_DIMM_devices[itr_d].socket_id) {
                fprintf(fout, "cpu_read_ops%scpu_write_ops%s", DELIMITER, DELIMITER);
            }
        }
    }
    fprintf(fout, "\n");
    fflush(fout);
}

void print_header_health_info (
    int  count
)
{
    int i;

    // 1st header
    fprintf(fout, "timestamp%s%s", DELIMITER, DELIMITER);
    for (i = 0; i < count; i++){
        fprintf(fout, "DIMM%d%s%s%s%s%s%s%s%s%s%s", i, DELIMITER, DELIMITER,      \
            DELIMITER, DELIMITER, DELIMITER, DELIMITER, DELIMITER, DELIMITER, DELIMITER, DELIMITER);
    }
    fprintf(fout, "\n");
    fflush(fout);

    // 2nd header
    fprintf(fout, "epoch%stimestamp%s", DELIMITER, DELIMITER);
    for (i = 0; i < count; i++){
        fprintf(fout, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
            "health_status",
            DELIMITER,
            "lifespan_used",
            DELIMITER,
            "lifespan_remaining",
            DELIMITER,
            "power_on_time",
            DELIMITER,
            "uptime",
            DELIMITER,
            "last_shutdown_time",
            DELIMITER,
            "media_temp",
            DELIMITER,
            "controller_temp",
            DELIMITER,
            "max_media_temp",
            DELIMITER,
            "max_controller_temp",
            DELIMITER
        );
    }
    fprintf(fout, "\n");
    fflush(fout);
}

void print_info_header ()
{
    time_t                   t;
    struct tm               *local_time;

    // print version and collection time
    fprintf(fout, "# Collector: %s v%d.%d.%d\n", PRODUCT_NAME, MAJOR_VERSION, MINOR_VERSION, UPDATE_VERSION);
    fprintf(fout, "# Output Format: v%d.%d\n", OUTPUT_FORMAT_MAJOR_VERSION, OUTPUT_FORMAT_MINOR_VERSION);
    t = time(NULL);
    local_time = localtime(&t);
    if (local_time != NULL) {
        fprintf(fout, "# Collected on %s", asctime(local_time));
    }
    if (output_format_one) {
        fprintf(fout, "# Default Format\n");
    }
    else if (output_format_two) {
        fprintf(fout, "# Metric Grouping Format\n");
    }

    // configuration to calculate avergae value in csv viewer
    if (collect_health) {
        fprintf(fout, "%s: %s, %s, %s, %s, %s, %s, %s, %s, %s, %s", \
                        "# Avg column",                         \
                        "health_status",                        \
                        "lifespan_used",                        \
                        "lifespan_remaining",                   \
                        "power_on_time",                        \
                        "uptime",                               \
                        "last_shutdown_time",                   \
                        "media_temp",                           \
                        "controller_temp",                      \
                        "max_media_temp",                       \
                        "max_controller_temp"                   \
                );
    }
    else {
        fprintf(fout, "# Avg column: read_hit_ratio (derived), write_hit_ratio (derived)");
    }
    fprintf(fout, "\n");
}

int adjust_for_overhead (
    int      i,
    uint64_t elapsed_time,
    int     *num_skips
)
{
    int itr = 0;
    int sample_time_sec_prev, sample_time_sec, elapsed_time_sec, temp_operand, loops_left;

#ifdef _WIN32
    temp_operand = (int) MILL_SEC;
#else
    temp_operand = (int) MICRO_SEC;
#endif

    while (elapsed_time > usec_sample_time) {
        usec_sample_time = usec_sample_time * 2;
        ++itr;

        elapsed_time_sec     = (float) elapsed_time / temp_operand;
        sample_time_sec_prev = (float) usec_sample_time_prev / temp_operand;

        if (i + itr > loops && !infinite_run) {
            fprintf(stderr, "\nERROR: The collection overhead is more than the remaining time for the run.\n");
            fprintf(stderr, "       This system has substantial overhead during collection. Use a minimum sampling time of %d sec. Exiting...\n", \
                             elapsed_time_sec + 1);
            return 1;
        }
    }

    sample_time_sec = (int) usec_sample_time / temp_operand;

    if (usec_sample_time != usec_sample_time_prev) {
        // calculate skipped loops and loops left
        if (!infinite_run) {
            // loops left with new sampling time: (number of loops left times previous sampling time) / new sampling time
            loops_left = ((loops - i) * sample_time_sec_prev) / sample_time_sec;
            // [ number of loops left] - loops_left gives the num_skips
            *num_skips += (loops - i) - loops_left;
            // total number of loops
            loops = i + loops_left;
        }
        if (!infinite_run && loops_left == 0) {
            fprintf(stderr, "\nERROR: The collection overhead is more than the remaining time for the run.\n");
            fprintf(stderr, "       This system has substantial overhead during collection. Use a minimum sampling time of %d sec. Exiting...\n", \
                             elapsed_time_sec + 1);
        }
        else {
            fprintf(stderr, "\nWARNING: The collection overhead (%d sec) is more than the sampling time.\n", elapsed_time_sec);
            fprintf(stderr, "         Updating the collection time from %d sec to %d sec\n", sample_time_sec_prev, sample_time_sec);
        }
        usec_sample_time_prev = usec_sample_time;
    }

    return 0;
}

int
PMW_COLLECT_Print_Smart_Counters ()
{
    int                      i, j, itr_p, itr_m, itr_d, num_packages, num_skips = 0, ret_code = 0;
    unsigned int             count;
    unsigned char            page_num;
    uint64_t                 pre_tsc, cur_tsc, delta_tsc, start_time, cur_time, elapsed_time, sleep_time, epoch;
    uint64_t                 total_bytes_read, total_bytes_written, media_read, media_write, media_write_adjusted;
    double                   read_hit_ratio, write_hit_ratio;
    struct device_discovery *lp_DIMM_devices;

    uint64_t overflow_val = 0xFFFFFFFFFFFFFFFFULL;

    lp_DIMM_devices = PMW_COMM_Get_DIMM_Topology();

    i              = 0;
    j              = 0;
    pre_tsc        = 0ULL;
    cur_tsc        = 0ULL;
    delta_tsc      = 0ULL;
    page_num       = 1;
    PMW_COMM_Get_Number_of_DIMM(&count);

#ifndef _WIN32
    if (using_api) {
        output_buf = (PMWATCH_OP_BUF) calloc (count, sizeof(PMWATCH_OP_BUF_NODE));
        if (output_buf == NULL) {
            fprintf(stderr, "ERROR: memory allocation failed!\n");

            return 1;
        }
    }
#endif

    // get the total number of packages
    num_packages = lp_DIMM_devices[count - 1].socket_id;
    ++num_packages;

    LIFETIME_INFO_COUNTER sample1 = (LIFETIME_INFO_COUNTER)calloc(count, sizeof(LIFETIME_INFO_COUNTER_NODE));
    LIFETIME_INFO_COUNTER sample2 = (LIFETIME_INFO_COUNTER)calloc(count, sizeof(LIFETIME_INFO_COUNTER_NODE));
    LIFETIME_INFO_COUNTER diff    = (LIFETIME_INFO_COUNTER)calloc(count, sizeof(LIFETIME_INFO_COUNTER_NODE));
    if (!sample1 || !sample2 || !diff) {
        fprintf(stderr, "ERROR: memory allocation failed!\n");
        free(sample1);
        free(sample2);
        free(diff);
#ifndef _WIN32
        if (using_api) {
            free(output_buf);
        }
#endif

        return 1;
    }
    LIFETIME_INFO_COUNTER pre_sample[count];
    LIFETIME_INFO_COUNTER cur_sample[count];

    print_info_header();

    // print header
    if (output_format_one) {
        print_header_format_one(count);
    }
    else {
        print_header_format_two(count);
    }

    for (j = 0; j < count; j++){
        pre_sample[j] = &sample1[j];
        cur_sample[j] = &sample2[j];
    }
    for (i = 0; i <= loops || infinite_run; i++){
        if (i == 0) {
            start_time = PMW_UTILS_Get_Curr_Time_For_Sleep();

            pre_tsc = PMW_UTILS_Read_TSC();

            // get count from all SMART counters
            for (j = 0; j < count; j++){
                ret_code = PMW_COMM_Get_Memory_Info_Page(j, pre_sample[j], page_num);
                if ( ret_code != NVM_SUCCESS) {
#ifndef _WIN32
                    if (!using_api) {
#endif
                        fprintf(stderr, "\nSomething went wrong while collecting metrics.\n");
#ifndef _WIN32
                    }
#endif
                    goto cleanup;
                }
            }
            // sleep for sample time
            cur_time = PMW_UTILS_Get_Curr_Time_For_Sleep();
            elapsed_time = cur_time - start_time;

            ret_code = adjust_for_overhead(i, elapsed_time, &num_skips);
            if (ret_code != 0) {
                goto cleanup;
            }

            sleep_time = usec_sample_time - elapsed_time;

            PMW_UTILS_Sleep(sleep_time);

            continue;
        }

        cur_tsc = PMW_UTILS_Read_TSC();

        CALCULATE_DIFFERENCE(cur_tsc, pre_tsc, delta_tsc, overflow_val);

        epoch = PMW_UTILS_Get_Curr_Time_In_Sec();
        fprintf(fout,"%" PRIu64 "%s%" PRIu64 "%s", epoch, DELIMITER, delta_tsc, DELIMITER);

        start_time = PMW_UTILS_Get_Curr_Time_For_Sleep();

#ifndef _WIN32
        if (using_api) {
            pthread_mutex_lock(&buffer_lock);
        }
#endif

        for (j = 0; j < count; j++){
#ifndef _WIN32
            if (using_api) {
                output_buf[j].timestamp = delta_tsc;
                output_buf[j].epoch = epoch;
            }
#endif

            // get count from all SMART counters after sleep
            ret_code = PMW_COMM_Get_Memory_Info_Page(j, cur_sample[j], page_num);
            if ( ret_code != NVM_SUCCESS) {
#ifndef _WIN32
                if (using_api) {
                    pthread_mutex_unlock(&buffer_lock);
                }
                else {
#endif
                    fprintf(stderr, "\nSomething went wrong while collecting metrics.\n");
#ifndef _WIN32
                }
#endif
                goto cleanup;
            }

            // get the difference of before and after sleep count
            PMW_UTILS_Cal_Diff(&diff[j], pre_sample[j], cur_sample[j]);

            if (output_format_one) {
                // calculate derived output metrics

                // bytes_read less than bytes_written results in negative value for total_bytes_read/media_read
                // providing "0" value to avoid displaying bogus negative value in this scenario
                if (diff[j].bytes_read > diff[j].bytes_written) {
                    total_bytes_read = (diff[j].bytes_read - diff[j].bytes_written) * 64;
                    media_read         = (diff[j].bytes_read - diff[j].bytes_written) / 4;
                }
                else {
                    total_bytes_read = 0ULL;
                    media_read       = 0ULL;
                }
                total_bytes_written = diff[j].bytes_written * 64;
                media_write           = diff[j].bytes_written / 4;

                // calculating read and write hit ratio
                if (diff[j].host_reads > media_read) {
                    double opr_01 = (double) diff[j].host_reads - media_read;
                    read_hit_ratio = (double) opr_01 / diff[j].host_reads;
                }
                else {
                    read_hit_ratio = 0;
                }
                if (diff[j].host_writes > media_write) {
                    double opr_02 = (double) diff[j].host_writes - media_write;
                    write_hit_ratio = (double) opr_02 / diff[j].host_writes;
                }
                else {
                    write_hit_ratio = 0;
                }

                // print counts
                fprintf(fout, "%" PRIu64 "%s%" PRIu64 "%s%.2lf%s%.2lf%s%" PRIu64 "%s%" PRIu64 "%s%" \
                    PRIu64 "%s%" PRIu64 "%s%" PRIu64 "%s%" PRIu64 "%s",
                    total_bytes_read,
                    DELIMITER,
                    total_bytes_written,
                    DELIMITER,
                    read_hit_ratio,
                    DELIMITER,
                    write_hit_ratio,
                    DELIMITER,
                    media_read,
                    DELIMITER,
                    media_write,
                    DELIMITER,
                    diff[j].bytes_read,
                    DELIMITER,
                    diff[j].bytes_written,
                    DELIMITER,
                    diff[j].host_reads,
                    DELIMITER,
                    diff[j].host_writes,
                    DELIMITER
                );

#ifndef _WIN32
                if (using_api) {
                    output_buf[j].total_bytes_read    = total_bytes_read;
                    output_buf[j].total_bytes_written = total_bytes_written;
                    output_buf[j].read_hit_ratio      = read_hit_ratio;
                    output_buf[j].write_hit_ratio     = write_hit_ratio;
                    output_buf[j].media_read          = media_read;
                    output_buf[j].media_write         = media_write;
                    output_buf[j].bytes_read          = diff[j].bytes_read;
                    output_buf[j].bytes_written       = diff[j].bytes_written;
                    output_buf[j].host_reads          = diff[j].host_reads;
                    output_buf[j].host_writes         = diff[j].host_writes;
                }
#endif

#ifdef BUILD_DEBUG
#ifndef _WIN32
                fprintf(fout, "%" PRIu64 "%s%.2lf%s%.2lf%s%.2lf%s%" PRIu64 "%s%" PRIu64 "%s%" \
                    PRIu64 "%s%" PRIu64 "%s%" PRIu64 "%s%" PRIu64 "%s",
                    output_buf[j].total_bytes_read,
                    DELIMITER,
                    output_buf[j].total_bytes_written,
                    DELIMITER,
                    output_buf[j].read_hit_ratio,
                    DELIMITER,
                    output_buf[j].write_hit_ratio,
                    DELIMITER,
                    output_buf[j].media_read,
                    DELIMITER,
                    output_buf[j].media_write,
                    DELIMITER,
                    output_buf[j].bytes_read,
                    DELIMITER,
                    output_buf[j].bytes_written,
                    DELIMITER,
                    output_buf[j].host_reads,
                    DELIMITER,
                    output_buf[j].host_writes,
                    DELIMITER
                );
#endif
#endif
            }

            //switch pointers for pre and cur smt_counter samples
            if (pre_sample[j] == &sample1[j]) {
                pre_sample[j] = &sample2[j];
                cur_sample[j] = &sample1[j];
            }
            else {
                pre_sample[j] = &sample1[j];
                cur_sample[j] = &sample2[j];
            }
        }

#ifndef _WIN32
        if (using_api) {
            ++write_count;
            pthread_mutex_unlock(&buffer_lock);
        }
#endif

        if (output_format_two) {
            for (itr_p = 0; itr_p < num_packages; itr_p++) {
                for (itr_m = 0; itr_m < 4; itr_m++) {
                    for (itr_d = 0; itr_d < count; itr_d++) {
                        // calculate derived output metrics

                        if (itr_p != lp_DIMM_devices[itr_d].socket_id) {
                            continue;
                        }

                        if (itr_m == 0) {
                            // bytes_read less than bytes_written results in negative value for total_bytes_read/media_read
                            // providing "0" value to avoid displaying bogus negative value in this scenario
                            if (diff[itr_d].bytes_read > diff[itr_d].bytes_written) {
                                total_bytes_read = (diff[itr_d].bytes_read - diff[itr_d].bytes_written) * 64;
                                media_read         = (diff[itr_d].bytes_read - diff[itr_d].bytes_written) / 4;
                            }
                            else {
                                total_bytes_read = 0ULL;
                                media_read         = 0ULL;
                            }
                            total_bytes_written = diff[itr_d].bytes_written * 64;
                            media_write           = diff[itr_d].bytes_written / 4;

                            // print media metrics' counts
                            fprintf(fout, "%" PRIu64 "%s%" PRIu64 "%s%" PRIu64 "%s%" PRIu64 "%s",
                                total_bytes_read,
                                DELIMITER,
                                total_bytes_written,
                                DELIMITER,
                                media_read,
                                DELIMITER,
                                media_write,
                                DELIMITER
                            );
                        }
                        else if (itr_m == 1) {
                            // calculate media_read and media_write again
                            if (diff[itr_d].bytes_read > diff[itr_d].bytes_written) {
                                media_read         = (diff[itr_d].bytes_read - diff[itr_d].bytes_written) / 4;
                            }
                            else {
                                media_read         = 0ULL;
                            }
                            media_write           = diff[itr_d].bytes_written / 4;

                            // calculating read and write hit ratio
                            if (diff[itr_d].host_reads > media_read) {
                                double opr_01 = (double) diff[itr_d].host_reads - media_read;
                                read_hit_ratio = (double) opr_01 / diff[itr_d].host_reads;
                            }
                            else {
                                read_hit_ratio = 0;
                            }
                            if (diff[itr_d].host_writes > media_write) {
                                double opr_02 = (double) diff[itr_d].host_writes - media_write;
                                write_hit_ratio = (double) opr_02 / diff[itr_d].host_writes;
                            }
                            else {
                                write_hit_ratio = 0;
                            }

                            // print buffer metrics' counts
                            fprintf(fout, "%.2lf%s%.2lf%s",
                                read_hit_ratio,
                                DELIMITER,
                                write_hit_ratio,
                                DELIMITER
                            );
                        }
                        else if (itr_m == 2) {
                            // print controller metrics' counts
                            fprintf(fout, "%" PRIu64 "%s%" PRIu64 "%s",
                                diff[itr_d].bytes_read,
                                DELIMITER,
                                diff[itr_d].bytes_written,
                                DELIMITER
                            );
                        }
                        else if (itr_m == 3) {
                            // print CPU metrics' counts
                            fprintf(fout, "%" PRIu64 "%s%" PRIu64 "%s",
                                diff[itr_d].host_reads,
                                DELIMITER,
                                diff[itr_d].host_writes,
                                DELIMITER
                            );
                        }
                    }
                }
            }
        }

        fprintf(fout,"\n");
        fflush(fout);

        pre_tsc = cur_tsc;
        cur_time = PMW_UTILS_Get_Curr_Time_For_Sleep();
        elapsed_time = cur_time - start_time;

        ret_code = adjust_for_overhead(i, elapsed_time, &num_skips);
        if (ret_code != 0) {
            goto cleanup;
        }

        sleep_time = usec_sample_time - elapsed_time;

        PMW_UTILS_Sleep(sleep_time);
    }

cleanup:
    free(sample1);
    free(sample2);
    free(diff);

#ifndef _WIN32
    if (using_api) {
        pthread_mutex_lock(&buffer_lock);
        collection_done = 1;
        pthread_mutex_unlock(&buffer_lock);
    }
#endif

    if (num_skips) {
        fprintf(fout, "%d interval(s) skipped.\n", num_skips);
    }

    return ret_code;
}

int
PMW_COLLECT_Print_Health_Info (
)
{
    int                      i, j, k, t, num_packages, num_skips = 0, ret_code = 0;
    unsigned int             count;
    uint64_t                 pre_tsc, cur_tsc, delta_tsc, start_time, cur_time, elapsed_time, sleep_time, epoch;
    struct device_discovery *lp_DIMM_devices;
    int                      temp_val = 0;
    char                     media_temp[SMALL_STR_LEN], controller_temp[SMALL_STR_LEN], max_media_temp[SMALL_STR_LEN], max_controller_temp[SMALL_STR_LEN];
    double                   media_temp_d, controller_temp_d, max_media_temp_d, max_controller_temp_d;

    uint64_t overflow_val = 0xFFFFFFFFFFFFFFFFULL;

    lp_DIMM_devices = PMW_COMM_Get_DIMM_Topology();

    i              = 0;
    j              = 0;
    pre_tsc        = 0ULL;
    cur_tsc        = 0ULL;
    delta_tsc      = 0ULL;
    PMW_COMM_Get_Number_of_DIMM(&count);

#ifndef _WIN32
    if (using_api) {
        output_buf = (PMWATCH_OP_BUF) calloc (count, sizeof(PMWATCH_OP_BUF_NODE));
        if (output_buf == NULL) {
            fprintf(stderr, "ERROR: memory allocation failed!\n");

            return 1;
        }
    }
#endif

    // get the total number of packages
    num_packages = lp_DIMM_devices[count - 1].socket_id;
    ++num_packages;

    HEALTH_INFO_COUNTER sample = (HEALTH_INFO_COUNTER)calloc(count, sizeof(HEALTH_INFO_COUNTER_NODE));
    if (sample == NULL) {
        fprintf(stderr, "ERROR: memory allocation failed!\n");
#ifndef _WIN32
        if (using_api) {
            free(output_buf);
        }
#endif

        return 1;
    }
    HEALTH_INFO_COUNTER health_info[count];

    print_info_header();

    print_header_health_info(count);

    for (i = 0; i < count; i++) {
        health_info[i] = &sample[i];
    }

    for (i = 0; i <= loops || infinite_run; i++){
        if (i == 0) {
            start_time = PMW_UTILS_Get_Curr_Time_For_Sleep();

            pre_tsc = PMW_UTILS_Read_TSC();

            cur_time = start_time;
            elapsed_time = cur_time - start_time;

            ret_code = adjust_for_overhead(i, elapsed_time, &num_skips);
            if (ret_code != 0) {
                goto cleanup;
            }

            sleep_time = usec_sample_time - elapsed_time;

            PMW_UTILS_Sleep(sleep_time);

            continue;
        }

        cur_tsc = PMW_UTILS_Read_TSC();

        CALCULATE_DIFFERENCE(cur_tsc, pre_tsc, delta_tsc, overflow_val);

        epoch = PMW_UTILS_Get_Curr_Time_In_Sec();
        fprintf(fout,"%" PRIu64 "%s%" PRIu64 "%s", epoch, DELIMITER, delta_tsc, DELIMITER);

        start_time = PMW_UTILS_Get_Curr_Time_For_Sleep();

#ifndef _WIN32
        if (using_api) {
            pthread_mutex_lock(&buffer_lock);
        }
#endif

        for (j = 0; j < count; j++) {
#ifndef _WIN32
            if (using_api) {
                output_buf[j].timestamp = delta_tsc;
                output_buf[j].epoch = epoch;
            }
#endif

            ret_code = PMW_COMM_Get_Health_Info_Page(j, health_info[j]);
            if ( ret_code != NVM_SUCCESS) {
#ifndef _WIN32
                if (using_api) {
                    pthread_mutex_unlock(&buffer_lock);
                }
                else {
#endif
                    fprintf(stderr, "\nSomething went wrong while collecting health info.\n");
#ifndef _WIN32
                }
#endif
                goto cleanup;
            }

            for (k = 0; k <= HEALTH_INFO_VENDOR_SPECIFIC_DATA_SIZE; k++) {
                if (PMWATCH_GET_BIT_VALUE(health_info[j]->validation_flags, k)) {
                    switch (k) {

                        case 0:         // Health Status
                            temp_val = 0;
                            for (t = 0; t <= HEALTH_STATUS_FATAL; t++) {
                                if (PMWATCH_GET_BIT_VALUE(health_info[j]->health_status, t)) {
                                    temp_val = t + 1;
                                }
                            }
                            health_info[j]->health_status = temp_val;
                            break;

                        case 1:         // Percentage Remaining
                            health_info[j]->percentage_used = 100 - health_info[j]->percentage_remaining;
                            break;

                        case 3:         // Media Temperature
                            media_temp_d = (health_info[j]->media_temp & TEMP_VALUE_MASK) * CELCIUS_CONV_VAL;
                            memset(media_temp, 0, sizeof(char) * SMALL_STR_LEN);
                            snprintf(media_temp, sizeof(media_temp) - 1, "%s%.2f",                                                       \
                                                PMWATCH_GET_BIT_VALUE(health_info[j]->media_temp, TEMP_SIGN_BIT_INDEX) ? "-" : "",      \
                                                media_temp_d);
                            break;

                        case 4:         // Controller Temperature
                            controller_temp_d = (health_info[j]->controller_temp & TEMP_VALUE_MASK) * CELCIUS_CONV_VAL;
                            memset(controller_temp, 0, sizeof(char) * SMALL_STR_LEN);
                            snprintf(controller_temp, sizeof(controller_temp) - 1, "%s%.2f",                                             \
                                                PMWATCH_GET_BIT_VALUE(health_info[j]->controller_temp, TEMP_SIGN_BIT_INDEX) ? "-" : "", \
                                                controller_temp_d);
                            break;

                        case 11:
                            max_media_temp_d = 0;
                            max_controller_temp_d = 0;
                            memset(max_media_temp, 0, sizeof(char) * SMALL_STR_LEN);
                            memset(max_controller_temp, 0, sizeof(char) * SMALL_STR_LEN);

                            if (health_info[j]->vendor_data_size > 0) {
                                max_media_temp_d      = (health_info[j]->max_media_temp & TEMP_VALUE_MASK) * CELCIUS_CONV_VAL;
                                max_controller_temp_d = (health_info[j]->max_controller_temp & TEMP_VALUE_MASK) * CELCIUS_CONV_VAL;
                                snprintf(max_media_temp, sizeof(max_media_temp), "%s%.2f",                                                   \
                                                PMWATCH_GET_BIT_VALUE(health_info[j]->max_media_temp, TEMP_SIGN_BIT_INDEX) ? "-" : "",      \
                                                max_media_temp_d);

                                snprintf(max_controller_temp, sizeof(max_controller_temp), "%s%.2f",                                         \
                                                PMWATCH_GET_BIT_VALUE(health_info[j]->max_controller_temp, TEMP_SIGN_BIT_INDEX) ? "-" : "", \
                                                max_controller_temp_d);
                            }
                            else {
                                health_info[j]->power_on_time                               = 0;
                                health_info[j]->uptime                                      = 0;
                                health_info[j]->last_shutdown_time                          = 0;
                            }
                            break;

                        default:
                            break;
                    }
                }
            }

            // Add to this fprintf when want to enable more fields. Data is already obtained
            fprintf(fout, "%d%s%d%s%d%s%" PRIu64 "%s%" PRIu64 "%s%" PRIu64 "%s%s%s%s%s%s%s%s%s",                        \
                            health_info[j]->health_status,                                                              \
                            DELIMITER,                                                                                  \
                            health_info[j]->percentage_used,                                                            \
                            DELIMITER,                                                                                  \
                            health_info[j]->percentage_remaining,                                                       \
                            DELIMITER,                                                                                  \
                            health_info[j]->power_on_time,                                                              \
                            DELIMITER,                                                                                  \
                            health_info[j]->uptime,                                                                     \
                            DELIMITER,                                                                                  \
                            health_info[j]->last_shutdown_time,                                                         \
                            DELIMITER,                                                                                  \
                            media_temp,                                                                                 \
                            DELIMITER,                                                                                  \
                            controller_temp,                                                                            \
                            DELIMITER,                                                                                  \
                            max_media_temp,                                                                             \
                            DELIMITER,                                                                                  \
                            max_controller_temp,                                                                        \
                            DELIMITER                                                                                   \
                            );
#ifndef _WIN32
            if (using_api) {
                output_buf[j].health_status        = health_info[j]->health_status;
                output_buf[j].percentage_remaining = health_info[j]->percentage_remaining;
                output_buf[j].percentage_used      = health_info[j]->percentage_used;
                output_buf[j].power_on_time        = health_info[j]->power_on_time;
                output_buf[j].uptime               = health_info[j]->uptime;
                output_buf[j].last_shutdown_time   = health_info[j]->last_shutdown_time;
                output_buf[j].media_temp           = media_temp_d;
                output_buf[j].controller_temp      = controller_temp_d;
                output_buf[j].max_media_temp       = max_media_temp_d;
                output_buf[j].max_controller_temp  = max_controller_temp_d;
            }
#endif

#ifdef BUILD_DEBUG
#ifndef _WIN32
            if (using_api) {
                fprintf(fout, "|%d%s%d%s%d%s%" PRIu64 "%s%" PRIu64 "%s%" PRIu64 "%s%.2f%s%.2f%s%.2f%s%.2f%s|",
                    output_buf[j].health_status,
                    DELIMITER,
                    output_buf[j].percentage_used,
                    DELIMITER,
                    output_buf[j].percentage_remaining,
                    DELIMITER,
                    output_buf[j].power_on_time,
                    DELIMITER,
                    output_buf[j].uptime,
                    DELIMITER,
                    output_buf[j].last_shutdown_time,
                    DELIMITER,
                    output_buf[j].media_temp,
                    DELIMITER,
                    output_buf[j].controller_temp,
                    DELIMITER,
                    output_buf[j].max_media_temp,
                    DELIMITER,
                    output_buf[j].max_controller_temp,
                    DELIMITER
                );
            }
#endif
#endif

        }

#ifndef _WIN32
        if (using_api) {
            ++write_count;
            pthread_mutex_unlock(&buffer_lock);
        }
#endif
        fprintf(fout, "\n");
        fflush(fout);

        pre_tsc = cur_tsc;
        cur_time = PMW_UTILS_Get_Curr_Time_For_Sleep();
        elapsed_time = cur_time - start_time;

        ret_code = adjust_for_overhead(i, elapsed_time, &num_skips);
        if (ret_code != 0) {
            goto cleanup;
        }

        sleep_time = usec_sample_time - elapsed_time;

        PMW_UTILS_Sleep(sleep_time);
    }

cleanup:
    free(sample);

#ifndef _WIN32
    if (using_api) {
        pthread_mutex_lock(&buffer_lock);
        collection_done = 1;
        pthread_mutex_unlock(&buffer_lock);
    }
#endif

    if (num_skips) {
        fprintf(fout, "%d interval(s) skipped.\n", num_skips);
    }

    return ret_code;
}

void
PMW_COLLECT_Print_Lifetime_Mem_Info_Counters ()
{
    int            i, j, ret_code;
    unsigned char  page_num;
    unsigned int   count;

    i        = 0;
    j        = 0;
    page_num = 1;
    PMW_COMM_Get_Number_of_DIMM(&count);

    LIFETIME_INFO_COUNTER_NODE cur_sample[count];

    print_info_header();

    fprintf(fout, "%s%s%s%s%s%s%s%s%s%s\n",
            "DIMM",
            DELIMITER,
            "read_64B_ops_received",
            DELIMITER,
            "write_64B_ops_received",
            DELIMITER,
            "cpu_read_ops",
            DELIMITER,
            "cpu_write_ops",
            DELIMITER);

    for (i = 0; i < count; i++) {
        ret_code = PMW_COMM_Get_Memory_Info_Page(i, &cur_sample[i], page_num);
        if ( ret_code != NVM_SUCCESS) {
            fprintf(stderr, "\nSomething went wrong while collecting metrics.\n");
            return;
        }
        fprintf(fout,
                "%s%d%s%" PRIu64 "%s%" PRIu64 "%s%" PRIu64 "%s%" PRIu64 "%s\n",
                "DIMM",
                i,
                DELIMITER,
                cur_sample[i].bytes_read,
                DELIMITER,
                cur_sample[i].bytes_written,
                DELIMITER,
                cur_sample[i].host_reads,
                DELIMITER,
                cur_sample[i].host_writes,
                DELIMITER
        );
    }

    return;
}

#ifndef _WIN32
void* threadable_func (
    void *arg
)
{
    if (collect_lifetime) {
        thread_status = PMW_COLLECT_Print_Smart_Counters();
    }
    if (collect_health) {
        thread_status = PMW_COLLECT_Print_Health_Info();
    }

    pthread_exit((void *) &thread_status);
}

int
PMW_COLLECT_Get_DIMM_Count (
    int *num_nvmdimms
)
{
    return PMW_COMM_Get_Number_of_DIMM(num_nvmdimms);
}

int
PMW_COLLECT_Start_Collection (
    PMWATCH_CONFIG_NODE pmwatch_config
)
{
    int ret_code;

    loops        = 1;
    using_api    = 1;
    infinite_run = 1;

    collect_health   = pmwatch_config.collect_health;
    collect_lifetime = pmwatch_config.collect_perf_metrics;

    if (collect_lifetime && collect_health) {
        fprintf(stderr, "WARNING: Health info and performance metrics cannot be collected simultaneously. Continuing with performance metrics collection...\n");
        collect_health = 0;
    }
    else if (!collect_lifetime && !collect_health) {
        fprintf(stderr, "ERROR: health info and performance metrics collection disabled. Enable atleast one.\n");

        pthread_mutex_lock(&buffer_lock);
        collection_done = 1;
        pthread_mutex_unlock(&buffer_lock);

        return 1;
    }

    usec_sample_time = (uint64_t) (pmwatch_config.interval * MICRO_SEC);
    // needed during overhead adjustment
    usec_sample_time_prev = usec_sample_time;

    filename = (char *) calloc(SMALL_STR_LEN, sizeof(char));
    if (filename == NULL) {
        fprintf(stderr, "ERROR: memory allocation failed!\n");

        pthread_mutex_lock(&buffer_lock);
        collection_done = 1;
        pthread_mutex_unlock(&buffer_lock);

        return 1;
    }

    ret_code = STRNCPY_SAFE(filename, SMALL_STR_LEN, "/dev/null");
    if (ret_code != NVM_SUCCESS) {
        pthread_mutex_lock(&buffer_lock);
        collection_done = 1;
        pthread_mutex_unlock(&buffer_lock);

        return ret_code;
    }
    fout = fopen(filename, "w");
    if (!fout) {
        fprintf(stderr, "ERROR: Unable to create output file %s. Exiting...\n", filename);
        free(filename);

        pthread_mutex_lock(&buffer_lock);
        collection_done = 1;
        pthread_mutex_unlock(&buffer_lock);

        return 1;
    }

    ret_code = PMW_COMM_Init();
    if (ret_code != 0) {
        PMW_COMM_Cleanup();
        fclose(fout);
        free(filename);

        pthread_mutex_lock(&buffer_lock);
        collection_done = 1;
        pthread_mutex_unlock(&buffer_lock);

        return 1;
    }

    pthread_create(&thread_id, NULL, threadable_func, NULL);

    return 0;
}

int
PMW_COLLECT_Stop_Collection ()
{
    infinite_run = 0;

    // wait for collection thread to exit
    while (1) {
        pthread_mutex_lock(&buffer_lock);
        if (collection_done || thread_status != 0) {
            break;
        }
        pthread_mutex_unlock(&buffer_lock);
    };
    pthread_mutex_unlock(&buffer_lock);

    loops            = 0;
    using_api        = 0;
    collect_health   = 0;
    collection_done  = 0;
    collect_lifetime = 0;

    free(output_buf);
    fclose(fout);
    free(filename);

    return 0;
}

int
PMW_COLLECT_Read_Data (
    PMWATCH_OP_BUF* op_buf
)
{
    unsigned int count;

    // wait to finish the first write to metrics count buffer
    while (1) {
        pthread_mutex_lock(&buffer_lock);
        if (write_count != 0 || thread_status != 0) {
            break;
        }
        pthread_mutex_unlock(&buffer_lock);
    };

    if (thread_status != 0) {
        fprintf(stderr, "%s:%d Collection thread exited with an error!\n", __FUNCTION__, __LINE__);
        pthread_mutex_unlock(&buffer_lock);

        return 1;
    }

    PMW_COMM_Get_Number_of_DIMM(&count);

    memset(*op_buf, 0, count * sizeof(PMWATCH_OP_BUF_NODE));
    memcpy(*op_buf, output_buf, count * sizeof(PMWATCH_OP_BUF_NODE));

    pthread_mutex_unlock(&buffer_lock);

    return 0;
}
#endif
