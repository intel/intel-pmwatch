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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#include <time.h>
#pragma intrinsic(__rdtsc)
#else
#include <unistd.h>
#include <sys/time.h>
#endif

#if defined(_WIN32)
#ifndef PMW_DLL_EXPORTS
#define PMW_DLL_EXPORTS 1
#endif
#define DLL_FILE_NAME L"libipmctl.dll"
#endif

#include "pmw_utils.h"

NVM_UINT32
PMW_UTILS_Strncpy_Safe (
    char       *dst,
    size_t      dst_size,
    const char *src
)
{
    if (!dst || !src) {
        fprintf(stderr, "ERROR: NULL value for strncpy operation\n");

        return 1;
    }

    size_t to_copy = strlen(src);

    if (to_copy >= dst_size) {
        fprintf(stderr, "ERROR: (strncpy) buffer too small\n");

        return 1;
    }

    memcpy(dst, src, to_copy);
    dst[to_copy] = '\0';

    return NVM_SUCCESS;
}

NVM_UINT32
PMW_UTILS_Strncat_Safe(
    char       *dst,
    size_t      dst_size,
    const char *src
)
{
    size_t to_copy, copy_at;

    if (!dst || !src) {
        fprintf(stderr, "ERROR: NULL value for strncat operation\n");

        return 1;
    }

    to_copy = strlen(src);
    copy_at = strlen(dst);

    if (to_copy + copy_at >= dst_size) {
        fprintf(stderr, "ERROR: (strncat) buffer too small\n");

        return 1;
    }

    memcpy(dst + copy_at, src, to_copy);
    dst[to_copy + copy_at] = '\0';

    return NVM_SUCCESS;
}

void
PMW_UTILS_Cal_Diff (
    const LIFETIME_INFO_COUNTER diff,
    const LIFETIME_INFO_COUNTER pre,
    const LIFETIME_INFO_COUNTER cur
)
{
    uint64_t overflow_val = 0xFFFFFFFFFFFFFFFFULL;

    // bytes_read
    CALCULATE_DIFFERENCE(cur->bytes_read, pre->bytes_read, diff->bytes_read, overflow_val);

    // bytes_written
    CALCULATE_DIFFERENCE(cur->bytes_written, pre->bytes_written, diff->bytes_written, overflow_val);

    // host_reads
    CALCULATE_DIFFERENCE(cur->host_reads, pre->host_reads, diff->host_reads, overflow_val);

    // host_writes
    CALCULATE_DIFFERENCE(cur->host_writes, pre->host_writes, diff->host_writes, overflow_val);

    return ;
}

#ifdef _WIN32
// return in millisec
uint64_t
PMW_UTILS_Get_Curr_Time_For_Sleep ()
{
    time_t epoch_time;

    time(&epoch_time);

   return epoch_time * MILL_SEC;
}

uint64_t
PMW_UTILS_Get_Curr_Time_In_Sec ()
{
    time_t epoch_time;

    time(&epoch_time);

   return epoch_time;
}
#else
// returns in microsec
uint64_t
PMW_UTILS_Get_Curr_Time_For_Sleep ()
{
   struct timeval time;

   gettimeofday(&time, NULL);

   return (time.tv_sec * MICRO_SEC) + time.tv_usec;
}

uint64_t
PMW_UTILS_Get_Curr_Time_In_Sec ()
{
   struct timeval time;
   gettimeofday(&time, NULL);

   return time.tv_sec + (time.tv_usec / MICRO_SEC);
}
#endif

/* ------------------------------------------------------------------------- */
/*!
 * @fn          PMW_UTILS_Print_Key
 *
 * @brief       Print the key of a key-value pair (key, value)
 *
 * @param       const char* format - format of the key string
 *
 * @return      void
 * <I>Special Notes:</I>
 *            < None >
 */
void PMW_UTILS_Print_Key (const char* format, ... )
{
    va_list args;
    va_start (args, format);
    fprintf(stdout, "(\"");
    vfprintf (stdout, format, args);
    fprintf(stdout, "\", ");
    va_end (args);
}

/* ------------------------------------------------------------------------- */
/*!
 * @fn          PMW_UTILS_Print_Value
 *
 * @brief       Print the value of a key-value pair (key, value)
 *
 * @param       const char* format - format of the value string
 *
 * @return      void
 * <I>Special Notes:</I>
 *            < None >
 */
void PMW_UTILS_Print_Value (const char* format, ... )
{
    va_list args;
    va_start (args, format);
    vfprintf (stdout, format, args);
    fprintf(stdout, ")\n");
    va_end (args);
    fflush(stdout);
}

#if !defined(_WIN32)
#if defined(__i386__)

static __inline__ unsigned long long rdtsc(void)
{
  unsigned long long int x;
     __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
     return x;
}

#else

static __inline__ unsigned long long rdtsc(void)
{
  unsigned hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

#endif
#endif

/* ------------------------------------------------------------------------- */
/*!
 * @fn          PMW_UTILS_Read_TSC
 *
 * @brief       Read TSC and return
 *
 * @return      TSC
 *
 * <I>Special Notes:</I>
 *            < None >
 */
uint64_t PMW_UTILS_Read_TSC ()
{
#if _WIN32
    return __rdtsc();
#else
    return rdtsc();
#endif
}

#if defined(_WIN32)
/* ------------------------------------------------------------------------- */
/*!
 * @fn          pmw_api_DLL_Error (
 *                                 wchar_t   *out_buffer,
 *                                 NVM_UINT32 max_buffer_len
 *                                )
 *
 * @brief       Convert error code to corresponding error message
 *
 * @return      return code
 *
 * <I>Special Notes:</I>
 *            < None >
 */
NVM_UINT32
pmw_api_DLL_Error (
    wchar_t   *out_buffer,
    NVM_UINT32 max_buffer_len
)
{
    if(!out_buffer) {
        return NVM_ERR_UNKNOWN;
    }

    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM,
                   NULL,
                   GetLastError(),
                   0,
                   out_buffer,
                   max_buffer_len,
                   NULL);

    return NVM_SUCCESS;
}

/* ------------------------------------------------------------------------- */
/*!
 * @fn          HANDLE PMW_UTILS_DLL_Open ()
 *
 * @brief       Convert error code to corresponding error message
 *
 * @return      DLL handle
 *
 * <I>Special Notes:</I>
 *            < None >
 */
HANDLE
PMW_UTILS_DLL_Open ()
{
    void   *p_mem   = NULL;
    wchar_t vt_buffer[NVM_EVENT_MSG_LEN];

    p_mem = LoadLibraryExW(DLL_FILE_NAME, NULL, 0);
    if(p_mem == NULL) {
        fwprintf(stderr, L"Unable to open shared library: %s\n", DLL_FILE_NAME);
        pmw_api_DLL_Error(vt_buffer, NVM_EVENT_MSG_LEN);
        fwprintf(stderr, L"Error: %s\n", vt_buffer);
        fflush(stderr);
        p_mem = (void*) INVALID_HANDLE_VALUE;
    }

    return p_mem;
}

/* ------------------------------------------------------------------------- */
/*!
 * @fn          PMW_UTILS_DLL_Sym (
 *                                 HANDLE      handle,
 *                                 const char *symbol
 *                                )
 *
 * @brief       Get symbol from DLL
 *
 * @return      pointer to symbol
 *
 * <I>Special Notes:</I>
 *            < None >
 */
void*
PMW_UTILS_DLL_Sym (
    HANDLE      handle,
    const char *symbol
)
{
    void   *p_mem = NULL;
    wchar_t vt_buffer[NVM_EVENT_MSG_LEN];

#if defined(UNICODE)
    char a_symbol[NVM_EVENT_MSG_LEN];
    w2a(a_symbol, symbol, NVM_EVENT_MSG_LEN);
    p_mem = GetProcAddress((HINSTANCE)handle, a_symbol);
#else
    p_mem = GetProcAddress((HINSTANCE)handle, symbol);
#endif
    if(p_mem == NULL) {
        pmw_api_DLL_Error(vt_buffer, NVM_EVENT_MSG_LEN);
        fprintf(stderr, "Unable to find symbol in shared library: %s\n", symbol);
        fwprintf(stderr, L"Error: %s\n", vt_buffer);
        fflush(stderr);
    }

    return p_mem;
}

/* ------------------------------------------------------------------------- */
/*!
 * @fn          PMW_UTILS_DLL_Close (
 *                                   HANDLE handle
 *                                  )
 *
 * @brief       Close DLL handle
 *
 * @return      return code
 *
 * <I>Special Notes:</I>
 *            < None >
 */
NVM_UINT32
PMW_UTILS_DLL_Close (
    HANDLE handle
)
{
    NVM_UINT32 ret_val = NVM_SUCCESS;
    wchar_t    vt_buffer[NVM_EVENT_MSG_LEN];

    if (handle != INVALID_HANDLE_VALUE && (void*) handle != NULL) {
        ret_val = FreeLibrary((HINSTANCE) handle);
        if(ret_val != 0) {
            ret_val = NVM_SUCCESS;
        }
        else {
            ret_val = NVM_ERR_UNKNOWN;
        }
    }

    if (ret_val != NVM_SUCCESS) {
        pmw_api_DLL_Error(vt_buffer, NVM_EVENT_MSG_LEN);
        fprintf(stderr, "Unable to close handle: %p\n", handle);
        fwprintf(stderr, L"Error: %s\n", vt_buffer);
        fflush(stderr);
    }

    return ret_val;
}

#endif

/* ------------------------------------------------------------------------- */
/*!
 * @fn          PMW_COLLECTION_Sleep (
 *                                   uint64_t sleep_time
 *                                  )
 *
 * @brief       Sleep the flow for the given time
 *
 * @return      No Return
 *
 * <I>Special Notes:</I>
 *            < None >
 */
void
PMW_UTILS_Sleep(uint64_t sleep_time)
{
#ifdef _WIN32
    Sleep(sleep_time);
#else
    usleep(sleep_time);
#endif
}