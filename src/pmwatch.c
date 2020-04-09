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
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#ifdef _WIN32
#include <windows.h>
#include <time.h>
#pragma intrinsic(__rdtsc)
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#endif

#include "nvm_types.h"
#include "nvm_management.h"
#include "inc/pmw_utils.h"
#include "inc/pmw_comm.h"
#include "inc/pmw_version.h"
#include "inc/pmw_collect.h"

FILE     *fout_m;
int       loops_m = 0, infinite_run_m = 0;
uint64_t  usec_sample_time_m, usec_sample_time_prev_m;
int       collect_lifetime_m   = 0;
char      SEMICOLON_M[]        = ";";
char      TAB_M[]              = "\t";
char      DELIMITER_M[]        = ";";
int       output_format_one_m  = 1;
int       output_format_two_m  = 0;
int       collect_health_m     = 0;

static char output_version[] = PRODUCT_NAME" "STRINGIFY(MAJOR_VERSION)"."STRINGIFY(MINOR_VERSION)""UPDATE_STRING""PRODUCT_TYPE_STR""ENG_BUILD;
static char build_date[] = __DATE__" at "__TIME__;

void
pmwatch_print_help()
{
    fprintf(stdout, "Usage:\npmwatch <collection_time> <loops>\n");
    fprintf(stdout, "pmwatch 1 (to run indefinitely; use \"pmwatch-stop\" to stop the collection)\n");
    fprintf(stdout, "pmwatch <collection_time> 0 (to run indefinitely with user specified collection time)\n");
    fprintf(stdout, "pmwatch -a | --avail (tool version and Optane DCPMM DIMM topology information)\n");
    fprintf(stdout, "pmwatch -l (collect lifetime data snapshot of memory performance)\n");
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "\t-hi, --health-info\tcollects health information\n");
    fprintf(stdout, "\t-f <outputfile.csv>\twrite the result to a csv file\n");
    fprintf(stdout, "\t-F <outputfile.csv>\tappend the result to a csv file\n");
    fprintf(stdout, "\t-g, --group-metrics-format\tdisplay the output in metric grouping format\n");
    fprintf(stdout, "\t-td, --tab-delimited\tuse tab as delimiter. Default is comma\n");
    fprintf(stdout, "\n");
    fflush(stdout);
}

int
pmwatch_parse_cli (
    int   argc,
    char* argv[]
)
{
    int       itr, valid_cli = 0, file_logging = 0, ret_val;
    double    sample_time       = 1;
    char     *file_name;
    char      extension[] = ".csv";
    char      file_mode[] = "w";

    ret_val = STRNCPY_SAFE(DELIMITER_M, sizeof(DELIMITER_M), SEMICOLON_M);
    if (ret_val != NVM_SUCCESS) {
        return ret_val;
    }

    // check if sufficient args are available
    if(argc < 3){
        // check if indefinite run or single snapshot
        if (argc == 2) {
            if (strcmp(argv[1], "1") == 0) {
                infinite_run_m = 1;
                valid_cli = 1;
            }
            else if (strcmp(argv[1], "-l") == 0) {
                collect_lifetime_m = 1;
                valid_cli = 1;
            }
        }

        if (!valid_cli) {
            pmwatch_print_help();
            return NVM_ERR_INVALIDPARAMETER;
        }
    }

    // check if indefinite run or single snapshot (cli: pmwatch 1 -f or pmwatch -l -f)
    if (!infinite_run_m && !collect_lifetime_m) {
        if (strcmp(argv[2], "-f") == 0 || strcmp(argv[2], "-F") == 0                     || \
            strcmp(argv[2], "-td") == 0 || strcmp(argv[2], "--tab-delimited") == 0       || \
            strcmp(argv[2], "-g") == 0 || strcmp(argv[2], "--group-metrics-format") == 0 || \
            strcmp(argv[2], "-hi") == 0 || strcmp(argv[2], "--health-info") == 0) {
            if (strcmp(argv[1], "1") == 0) {
                infinite_run_m = 1;
                valid_cli = 1;
            }
            else if (strcmp(argv[1], "-l") == 0) {
                collect_lifetime_m = 1;
                valid_cli = 1;
            }
        }
    }

    // if not indefinite run or single snapshot, parse cli args for sampling time and loops
    if (!infinite_run_m && !collect_lifetime_m) {
        sample_time      = (double) atof(argv[1]);
        loops_m            = (int) atof(argv[2]);
        if (sample_time <= 0 || loops_m < 0) {
            fprintf(stderr, "<collection_time>/<loops> should be a positive value.\n");
            return NVM_ERR_INVALIDPARAMETER;
        }
        if (loops_m == 0) {
            infinite_run_m = 1;
        }
        valid_cli = 1;
    }

    if (!valid_cli) {
        pmwatch_print_help();
        return NVM_ERR_INVALIDPARAMETER;
    }

#ifdef _WIN32
    usec_sample_time_m = (uint64_t) (sample_time * MILL_SEC);
#else
    usec_sample_time_m = (uint64_t) (sample_time * MICRO_SEC);
#endif

    // needed during overhead adjustment
    usec_sample_time_prev_m = usec_sample_time_m;

    // parse for other options
    itr = 2;
    while(itr < argc) {
        if (strcmp(argv[itr], "-f") == 0) {
            file_name = (char*) malloc (SMALL_STR_LEN * sizeof(char));
            if (itr + 1 >= argc) {
                fprintf(stderr, "Error: provide an output file name.\n");
                return NVM_ERR_INVALIDPARAMETER;
            }

            ret_val = STRNCPY_SAFE(file_name, SMALL_STR_LEN, argv[itr + 1]);
            if (ret_val != NVM_SUCCESS) {
                return ret_val;
            }
            file_logging = 1;
        }
        else if (strcmp(argv[itr], "-F") == 0) {
            file_name = (char*) malloc (SMALL_STR_LEN * sizeof(char));
            if (itr + 1 >= argc) {
                fprintf(stderr, "Error: provide an output file name.\n");
                return NVM_ERR_INVALIDPARAMETER;
            }

            ret_val = STRNCPY_SAFE(file_name, SMALL_STR_LEN, argv[itr + 1]);
            if (ret_val != NVM_SUCCESS) {
                return ret_val;
            }
            ret_val = STRNCPY_SAFE(file_mode, sizeof(file_mode), "a");
            if (ret_val != NVM_SUCCESS) {
                return ret_val;
            }
            file_logging = 1;
        }
        else if (strcmp(argv[itr], "-td") == 0 || strcmp(argv[itr], "--tab-delimited") == 0) {
            ret_val = STRNCPY_SAFE(extension, sizeof(extension), ".tsv");
            if (ret_val != NVM_SUCCESS) {
                return ret_val;
            }
            ret_val = STRNCPY_SAFE(DELIMITER_M, sizeof(DELIMITER_M), TAB_M);
            if (ret_val != NVM_SUCCESS) {
                return ret_val;
            }
        }
        else if (strcmp(argv[itr], "-g") == 0 || strcmp(argv[itr], "--group-metrics-format") == 0) {
            output_format_two_m = 1;
            output_format_one_m = 0;
        }
        else if (strcmp(argv[itr], "-hi") == 0 || strcmp(argv[itr], "--health-info") == 0) {
            collect_health_m = 1;
        }
        ++itr;
    }

    if (output_format_two_m && collect_lifetime_m) {
        fprintf(stderr, "WARNING: -g option cannot be used with -l option. Continuing without -g option ...\n");
        output_format_one_m = 1;
        output_format_two_m = 0;
    }

    // set stdout or fout
    if (file_logging) {
        if (strstr(file_name, ".csv") == NULL) {
            ret_val = STRNCAT_SAFE(file_name, SMALL_STR_LEN, extension);
            if (ret_val != NVM_SUCCESS) {
                return ret_val;
            }
        }

        fout_m = fopen(file_name, file_mode);
        if (!fout_m) {
            fprintf(stderr, "ERROR: Unable to open output file %s. Output will be sent to the console.\n", file_name);
            fout_m = stdout;
        }
    }
    else {
        fout_m = stdout;
    }

    return NVM_SUCCESS;
}

void
pmwatch_Close_File ()
{
    if (fout_m != stdout && fout_m != NULL) {
        fclose(fout_m);
    }

    return;
}

int main (int argc, char* argv[]){
    int ret_code = NVM_SUCCESS;
    int detailed = 0;

    // check for "-a"/"--avail" option and print in machine-readable format
    if ((argc == 2 || argc == 3) && (strcmp(argv[1], "-a") == 0 || strcmp(argv[1], "--avail") == 0)) {
        if (argc == 3 && (strcmp(argv[2], "-d") == 0 || strcmp(argv[2], "--detailed") == 0)) {
            detailed = 1;
        }
        PMW_UTILS_Print_Key("%s", "version");
        PMW_UTILS_Print_Value("\"%s\"", output_version);
        PMW_UTILS_Print_Key("%s","build_date");
        PMW_UTILS_Print_Value("\"%s\"", build_date);

        ret_code = PMW_COMM_Print_Topology(1, detailed);
        if (ret_code != NVM_SUCCESS) {
            if (ret_code != NVM_ERR_INVALID_PERMISSIONS) {
                PMW_COMM_Print_Warning_Message();
            }
            return ret_code;
        }
        return NVM_SUCCESS;
    }

    // parse cli option
    if (pmwatch_parse_cli(argc, argv) != NVM_SUCCESS) {
        return 1;
    }

    ret_code = PMW_COLLECT_Set_Global_Vars (fout_m,
                                            loops_m,
                                            infinite_run_m,
                                            usec_sample_time_m,
                                            usec_sample_time_prev_m,
                                            collect_lifetime_m,
                                            output_format_one_m,
                                            output_format_two_m,
                                            collect_health_m,
                                            DELIMITER_M);
    if (ret_code != NVM_SUCCESS) {
        return 1;
    }

    // initialize: check the availability
    // get the DIMM topology
    ret_code = PMW_COMM_Init();
    if (ret_code != NVM_SUCCESS) {
        if (ret_code != NVM_ERR_INVALID_PERMISSIONS) {
            PMW_COMM_Print_Warning_Message();
        }
        if (ret_code != PMW_ERR_DLLLOAD && ret_code != PMW_ERR_DLLSYM) {
            PMW_COMM_Cleanup();
        }
        pmwatch_Close_File();
        return ret_code;
    }

    if (collect_lifetime_m) {
        PMW_COLLECT_Print_Lifetime_Mem_Info_Counters();
    }
    else if (collect_health_m) {
        PMW_COLLECT_Print_Health_Info();
    }
    else {
        // get the counts from 6 SMART counters and output
        PMW_COLLECT_Print_Smart_Counters();
    }

    PMW_COMM_Cleanup();

    pmwatch_Close_File();

    return 0;
}
