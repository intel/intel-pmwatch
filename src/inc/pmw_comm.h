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

#ifndef _PMW_COMM_H_INC_
#define _PMW_COMM_H_INC_

#ifdef _WIN32
#define PMW_DLL 1
#define PMW_HELPER_DLL_IMPORT __declspec(dllimport)
#define PMW_HELPER_DLL_EXPORT __declspec(dllexport)
#endif

#ifdef PMW_DLL
#ifdef PMW_DLL_EXPORTS
#define PMW_API PMW_HELPER_DLL_EXPORT
#else
#define PMW_API PMW_HELPER_DLL_IMPORT
#endif // PMW_DLL_EXPORTS
#else // PMW_DLL
#define PMW_API
#endif // NVM_DLL

#define _STRINGIFY(x)     #x
#define STRINGIFY(x)      _STRINGIFY(x)

// defining ipmctl API macros
#ifdef _WIN32
#define NVM_CREATE_CONTEXT              p_nvm_create_context
#define NVM_FREE_CONTEXT                p_nvm_free_context
#define NVM_GET_BUILD_NUMBER            p_nvm_get_build_number
#define NVM_GET_NUMBER_OF_DEVICES       p_nvm_get_number_of_devices
#define NVM_GET_DEVICES                 p_nvm_get_devices
#define NVM_GET_NVM_CAPACITIES          p_nvm_get_nvm_capacities
#define NVM_SEND_DEVICE_PASSTHROUGH_CMD p_nvm_send_device_passthrough_cmd
#define NVM_GET_MAJOR_VERSION           p_nvm_get_major_version
#define NVM_GET_MINOR_VERSION           p_nvm_get_minor_version
#define NVM_GET_HOTFIX_NUMBER           p_nvm_get_hotfix_number
#define NVM_GET_BUILD_NUMBER            p_nvm_get_build_number
#else
#define NVM_CREATE_CONTEXT              nvm_create_context
#define NVM_FREE_CONTEXT                nvm_free_context
#define NVM_GET_BUILD_NUMBER            nvm_get_build_number
#define NVM_GET_NUMBER_OF_DEVICES       nvm_get_number_of_devices
#define NVM_GET_DEVICES                 nvm_get_devices
#define NVM_GET_NVM_CAPACITIES          nvm_get_nvm_capacities
#define NVM_SEND_DEVICE_PASSTHROUGH_CMD nvm_send_device_passthrough_cmd
#define NVM_GET_MAJOR_VERSION           nvm_get_major_version
#define NVM_GET_MINOR_VERSION           nvm_get_minor_version
#define NVM_GET_HOTFIX_NUMBER           nvm_get_hotfix_number
#define NVM_GET_BUILD_NUMBER            nvm_get_build_number
#endif

#define PMWATCH_SUPPORT_IPMCTL_VERSION_MIN   "01.00.00.3262"

// memory info struct
typedef struct MEMORY_INFO_PAGE_NODE_S  MEMORY_INFO_PAGE_NODE;
typedef        MEMORY_INFO_PAGE_NODE   *MEMORY_INFO_PAGE;

#if defined(_WIN32)
#pragma pack(push,1)
#endif
struct MEMORY_INFO_PAGE_NODE_S {              // output
    unsigned char total_bytes_read[16];       // Number of 64 byte reads from the DIMM
    unsigned char total_bytes_written[16];    // Number of 64 byte writes from the DIMM
    unsigned char total_read_reqs[16];        // Number of DDRT read transactions the DIMM has serviced
    unsigned char total_write_reqs[16];       // Number of DDRT write transactions the DIMM has serviced
    unsigned char rsvd[64];
#if defined(_WIN32)
};
#pragma pack(pop)
#else
} __attribute__((packed));
#endif

typedef struct HEALTH_INFO_PAGE_NODE_S  HEALTH_INFO_PAGE_NODE;
typedef        HEALTH_INFO_PAGE_NODE   *HEALTH_INFO_PAGE;

#if defined(_WIN32)
#pragma pack(push,1)
#endif
struct HEALTH_INFO_PAGE_NODE_S {
    unsigned char validation_flags[4];
    unsigned char rsvd1[4];
    unsigned char health_status;
    unsigned char percentage_remaining;
    unsigned char percentage_used;
    unsigned char rsvd2;
    unsigned char media_temp[2];
    unsigned char controller_temp[2];
    unsigned char rsvd3[16];
    unsigned char vendor_data_size[4];
    unsigned char rsvd4[8];
    unsigned char power_on_time[8];
    unsigned char uptime[8];
    unsigned char rsvd5[5];
    unsigned char last_shutdown_time[8];
    unsigned char rsvd6[9];
    unsigned char max_media_temp[2];
    unsigned char max_controller_temp[2];
    unsigned char rsvd7[42];
#if defined(_WIN32)
};
#pragma pack(pop)
#else
} __attribute__((packed));
#endif

#if defined(_WIN32)
#pragma pack(push, 1)
#endif
struct pt_payload_input_memory_info { //input
    unsigned char memory_page; // The page of memory information you want to retrieve
    unsigned char rsvd[127];
#if defined(_WIN32)
};
#pragma pack(pop)
#else
} __attribute__((packed));
#endif

/*
 * Strcuture to hold the SMART counter values
 */
typedef struct LIFETIME_INFO_COUNTER_NODE_S  LIFETIME_INFO_COUNTER_NODE;
typedef        LIFETIME_INFO_COUNTER_NODE   *LIFETIME_INFO_COUNTER;

struct LIFETIME_INFO_COUNTER_NODE_S
{
    uint64_t bytes_read;    // Lifetime number of 64 bytes of data read from the DIMM.
    uint64_t bytes_written; // Lifetime number of 64 bytes of data written to the DIMM.
    uint64_t host_reads;    // Lifetime number of DDRT read requests the DIMM has serviced.
    uint64_t host_writes;   // Lifetime number of DDRT write requests the DIMM has serviced.
};

typedef struct HEALTH_INFO_COUNTER_NODE_S  HEALTH_INFO_COUNTER_NODE;
typedef        HEALTH_INFO_COUNTER_NODE   *HEALTH_INFO_COUNTER;

struct HEALTH_INFO_COUNTER_NODE_S
{
    uint32_t                        validation_flags;
    unsigned char                   health_status;
    unsigned char                   percentage_remaining;
    unsigned char                   percentage_used;
    uint16_t                        media_temp;
    uint16_t                        controller_temp;
    uint32_t                        vendor_data_size;
    uint64_t                        power_on_time;
    uint64_t                        uptime;
    uint64_t                        last_shutdown_time;
    uint16_t                        max_media_temp;
    uint16_t                        max_controller_temp;
};


extern PMW_API struct device_discovery*
PMW_COMM_Get_DIMM_Topology ();

extern PMW_API NVM_INT32
PMW_COMM_Get_Memory_Info_Page (
    NVM_INT32           dimm_index,
    void               *counters,
    const unsigned char page_num
);

extern PMW_API NVM_INT32
PMW_COMM_Get_Health_Info_Page (
    NVM_INT32           dimm_index,
    void               *counters
);

extern PMW_API void
PMW_COMM_Print_Warning_Message ();

extern PMW_API NVM_INT32
PMW_COMM_Is_DIMM_Available ();

extern PMW_API NVM_INT32
PMW_COMM_Read_DIMM_Topology ();

extern PMW_API NVM_INT32
PMW_COMM_Print_Topology (
    NVM_INT32 get_topo,
    NVM_INT32 detailed
);

extern PMW_API NVM_INT32
PMW_COMM_Get_Number_of_DIMM (
    NVM_UINT32 *dimm_count
);

extern PMW_API NVM_INT32
PMW_COMM_Get_Count_by_Topology (
    NVM_INT32             socket_num,
    NVM_INT32             mc_num,
    NVM_INT32             channel_num,
    NVM_INT32             dimm_num,
    LIFETIME_INFO_COUNTER smt_counters
);

extern PMW_API int
PMW_COMM_Check_User_Permission ();

extern PMW_API NVM_INT32
PMW_COMM_Init ();

extern PMW_API NVM_INT32
PMW_COMM_Cleanup ();

#endif
