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

#ifndef _PMW_UTILS_H_INC_
#define _PMW_UTILS_H_INC_

#include "nvm_types.h"
#include "pmw_comm.h"

#define STRNCPY_SAFE PMW_UTILS_Strncpy_Safe
#define STRNCAT_SAFE PMW_UTILS_Strncat_Safe

#define CALCULATE_DIFFERENCE(cur,pre,diff,overflow_val) {   \
        if (cur < pre) {                                    \
            diff = (overflow_val - pre) + 1 + cur;          \
        }                                                   \
        else {                                              \
            diff = cur - pre;                               \
        }                                                   \
    }

#define PMWATCH_GET_BIT_VALUE(val, idx)   (val >> idx) & 0x1

#define MICRO_SEC                       1000000
#define MILL_SEC                        1000
#define SMALL_STR_LEN                   128
#define MEDIUM_STR_LEN                  512
#define LARGE_STR_LEN                   1024
#define CELCIUS_CONV_VAL                0.0625
#define TEMP_SIGN_BIT_INDEX             15
#define TEMP_VALUE_MASK                 0x7FFF
#define ROOT_UID                        0

#define HEALTH_INFO_VENDOR_SPECIFIC_DATA_SIZE           11
#define HEALTH_STATUS_FATAL                             2
#define HEALTH_REASON_CRITICAL_INTERNAL_STATE           7

#define NEGATIVE_POSITIVE_BIT_INDEX         15

// error codes
#define PMW_ERR_DLLLOAD 300
#define PMW_ERR_DLLSYM  301

/**
 * Convert an array of 8 unsigned chars into an unsigned 64 bit value
 * @remarks While it doesn't seem right to be casting 8 bit chars to unsigned long
 * long, this is an issue with gcc - see http:// gcc.gnu.org/bugzilla/show_bug.cgi?id=47821.
 */
#define PMW_8_BYTE_ARRAY_TO_64_BIT_VALUE(arr, val) \
  val = ((unsigned long long)(arr[7] & 0xFF) << 56) + \
        ((unsigned long long)(arr[6] & 0xFF) << 48) + \
        ((unsigned long long)(arr[5] & 0xFF) << 40) + \
        ((unsigned long long)(arr[4] & 0xFF) << 32) + \
        ((unsigned long long)(arr[3] & 0xFF) << 24) + \
        ((unsigned long long)(arr[2] & 0xFF) << 16) + \
        ((unsigned long long)(arr[1] & 0xFF) << 8) + \
        (unsigned long long)(arr[0] & 0xFF);

#define PMW_4_BYTE_ARRAY_TO_32_BIT_VALUE(arr, val)      \
    val = ((unsigned long long)(arr[3] & 0xFF) << 24) + \
          ((unsigned long long)(arr[2] & 0xFF) << 16) + \
          ((unsigned long long)(arr[1] & 0xFF) << 8)  + \
           (unsigned long long)(arr[0] & 0xFF);

#define PMW_3_BYTE_ARRAY_TO_32_BIT_VALUE(arr, val)      \
    val = ((unsigned long long)(arr[2] & 0xFF) << 16) + \
          ((unsigned long long)(arr[1] & 0xFF) << 8)  + \
           (unsigned long long)(arr[0] & 0xFF);

#define PMW_2_BYTE_ARRAY_TO_16_BIT_VALUE(arr, val)      \
    val = ((unsigned long long)(arr[1] & 0xFF) << 8)  + \
           (unsigned long long)(arr[0] & 0xFF);

/**
 * Convert an unsigned 64 bit integer to an array of 8 unsigned chars
 */
#define PMW_64_BIT_VALUE_TO_8_BYTE_ARRAY(val, arr) \
  arr[7] = (unsigned char)((val >> 56) & 0xFF); \
  arr[6] = (unsigned char)((val >> 48) & 0xFF); \
  arr[5] = (unsigned char)((val >> 40) & 0xFF); \
  arr[4] = (unsigned char)((val >> 32) & 0xFF); \
  arr[3] = (unsigned char)((val >> 24) & 0xFF); \
  arr[2] = (unsigned char)((val >> 16) & 0xFF); \
  arr[1] = (unsigned char)((val >> 8) & 0xFF); \
  arr[0] = (unsigned char)(val & 0xFF);

#if defined(_WIN32)

#include <windows.h>
#include <wchar.h>

PMW_API uint64_t
PMW_UTILS_Read_TSC();

PMW_API HANDLE
PMW_UTILS_DLL_Open ();

PMW_API void*
PMW_UTILS_DLL_Sym (
    HANDLE      handle,
    const char *symbol
);

PMW_API NVM_UINT32
PMW_UTILS_DLL_Close (
    HANDLE handle
);

#else

extern PMW_API uint64_t
PMW_UTILS_Read_TSC();

#endif

PMW_API NVM_UINT32
PMW_UTILS_Strncpy_Safe (
    char       *dst,
    size_t      dst_size,
    const char *src
);

PMW_API NVM_UINT32
PMW_UTILS_Strncat_Safe(
    char       *dst,
    size_t      dst_size,
    const char *src
);

PMW_API void
PMW_UTILS_Cal_Diff (
    const LIFETIME_INFO_COUNTER diff,
    const LIFETIME_INFO_COUNTER pre,
    const LIFETIME_INFO_COUNTER cur
);

PMW_API uint64_t
PMW_UTILS_Get_Curr_Time_For_Sleep ();

PMW_API uint64_t
PMW_UTILS_Get_Curr_Time_In_Sec ();

PMW_API void
PMW_UTILS_Print_Key (const char* format, ... );

PMW_API void
PMW_UTILS_Print_Value (const char* format, ... );

void
PMW_UTILS_Sleep (uint64_t sleep_time);

#endif