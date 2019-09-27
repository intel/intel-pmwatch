/*
 * MIT License
 *
 * Copyright (C) 2018 - 2019 Intel Corporation. All rights reserved.
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

#ifndef _PMW_COLLECT_H_INC_
#define _PMW_COLLECT_H_INC_

#ifdef PMW_DLL
#ifdef PMW_DLL_EXPORTS
#define PMW_API PMW_HELPER_DLL_EXPORT
#else
#define PMW_API PMW_HELPER_DLL_IMPORT
#endif // PMW_DLL_EXPORTS
#else // PMW_DLL
#define PMW_API
#endif // NVM_DLL

#include "pmw_struct.h"

extern PMW_API NVM_UINT32
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
);

extern PMW_API int
PMW_COLLECT_Print_Smart_Counters ();

extern PMW_API int
PMW_COLLECT_Print_Health_Info ();

extern PMW_API void
PMW_COLLECT_Print_Lifetime_Mem_Info_Counters ();

#ifndef _WIN32

extern PMW_API int
PMW_COLLECT_Get_DIMM_Count (
    int *num_nvmdimms
);

extern PMW_API int
PMW_COLLECT_Start_Collection (
    PMWATCH_CONFIG_NODE pmwatch_config
);

extern PMW_API int
PMW_COLLECT_Stop_Collection ();

extern PMW_API int
PMW_COLLECT_Read_Data (
    PMWATCH_OP_BUF* op_buf
);

#endif

#endif
