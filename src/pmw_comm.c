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
#define __NVM_DLL__     1
#else
#include <unistd.h>
#endif

#include "nvm_types.h"
#include "nvm_management.h"

#include "inc/pmw_comm.h"
#include "inc/pmw_utils.h"

#define LIFETIME_INFO_PAGE_NUM     1

#define IS_LIFETIME_INFO(x) (x == LIFETIME_INFO_PAGE_NUM ? 1 : 0)

/*
 * Defines the Firmware Command Table opcodes
 */
enum passthrough_opcode {
    PT_GET_LOG            = 0x08
};

/*
 * Defines the Sub-Opcodes for PT_GET_LOG
 */
enum get_log_subop {
    SUBOP_SMART_HEALTH        = 0x00,
    SUBOP_MEM_INFO            = 0x03
};


#ifdef _WIN32
    HANDLE handle;
    int (*p_nvm_create_context)();
    int (*p_nvm_free_context)(const NVM_BOOL);
    int (*p_nvm_get_build_number)();
    int (*p_nvm_get_number_of_devices)(int*);
    int (*p_nvm_get_devices)(struct device_discovery*, const NVM_UINT8);
    int (*p_nvm_get_nvm_capacities)(struct device_capacities*);
    int (*p_nvm_send_device_passthrough_cmd)(const NVM_UID, struct device_pt_cmd*);
    int (*p_nvm_get_major_version)();
    int (*p_nvm_get_minor_version)();
    int (*p_nvm_get_hotfix_number)();
    int (*p_nvm_get_build_number)();
#endif

NVM_UINT32 count = 0;
char  ipmctl_version_str[15];
struct device_discovery *gp_DIMM_devices = NULL;
int ipmctl_major_ver, ipmctl_minor_ver, ipmctl_hot_fix_num, ipmctl_build;

/*!
 * @fn          struct device_discovery* PMW_COMM_Get_DIMM_Topology
 *
 * @brief       Get the the DIMM information
 *
 * @param       None
 *
 * @return      struct device_discovery*
 *
 */
struct device_discovery*
PMW_COMM_Get_DIMM_Topology ()
{
    return gp_DIMM_devices;
}

/*!
 * @fn          NVM_INT32 PMW_COMM_Get_Smart_Counters
 *
 * @brief       Get the the smart couters in the DIMM dimm_index in the default page number
 *
 * @param       NVM_INT32           dimm_index   - index of the dimm
 * @param       LIFETIME_INFO_COUNTER smt_counters - structure of smart counter
 *
 * @return      error code
 *
 */
NVM_INT32
PMW_COMM_Get_Memory_Info (
    NVM_INT32     dimm_index,
    LIFETIME_INFO_COUNTER smt_counters
)
{
    return PMW_COMM_Get_Memory_Info_Page(dimm_index, smt_counters, LIFETIME_INFO_PAGE_NUM);
}

/*!
 * @fn          NVM_INT32 PMW_COMM_Get_Count_by_Topology
 *
 * @brief       Get the the smart couters based on user provided DIMM topology
 *
 * @param       NVM_INT32     socket_num,
 * @param       NVM_INT32     mc_num,
 * @param       NVM_INT32     channel_num,
 * @param       NVM_INT32     dimm_num,
 * @param       LIFETIME_INFO_COUNTER smt_counters
 *
 * @return      error code
 *
 */
NVM_INT32
PMW_COMM_Get_Count_by_Topology (
    NVM_INT32     socket_num,
    NVM_INT32     mc_num,
    NVM_INT32     channel_num,
    NVM_INT32     dimm_num,
    LIFETIME_INFO_COUNTER smt_counters
)
{
    int itr, dimm_found = 0;

    for (itr = 0; itr < count; itr++) {
        if (gp_DIMM_devices[itr].socket_id != socket_num) {
            continue;
        }

        if (gp_DIMM_devices[itr].memory_controller_id != mc_num) {
            continue;
        }

        if (gp_DIMM_devices[itr].channel_id != channel_num) {
            continue;
        }

        if (gp_DIMM_devices[itr].channel_pos != dimm_num) {
            continue;
        }

        dimm_found = 1;
        PMW_COMM_Get_Memory_Info_Page(itr, smt_counters, LIFETIME_INFO_PAGE_NUM);
        break;
    }

    if (dimm_found) {
        return NVM_SUCCESS;
    }
    else {
        return 1;
    }
}

/*!
 * @fn          NVM_INT32 pmw_comm_Init_DLL()
 *
 * @brief       Get process address for Windows APIs
 *
 * @param       None
 *
 * @return      NVM_SUCCESS on success
 *              PMW_ERR_DLLLOAD or PMW_ERR_DLLSYM on error
 *
 */
#ifdef _WIN32
NVM_INT32
pmw_comm_Init_DLL ()
{
    handle = PMW_UTILS_DLL_Open();
    if (handle == INVALID_HANDLE_VALUE) {
        return PMW_ERR_DLLLOAD;
    }

    p_nvm_create_context = (int (*)()) PMW_UTILS_DLL_Sym(handle, "nvm_create_context");
    if (p_nvm_create_context == NULL) {
        return PMW_ERR_DLLSYM;
    }

    p_nvm_free_context = (int (*)(const NVM_BOOL)) PMW_UTILS_DLL_Sym(handle, "nvm_free_context");
    if (p_nvm_free_context == NULL) {
        return PMW_ERR_DLLSYM;
    }

    p_nvm_get_build_number = (int (*)()) PMW_UTILS_DLL_Sym(handle, "nvm_get_build_number");
    if (p_nvm_get_build_number == NULL) {
        return PMW_ERR_DLLSYM;
    }

    p_nvm_get_nvm_capacities = (int (*)(struct device_capacities*)) PMW_UTILS_DLL_Sym(handle, "nvm_get_nvm_capacities");
    if (p_nvm_get_nvm_capacities == NULL) {
        return PMW_ERR_DLLSYM;
    }

    p_nvm_get_number_of_devices = (int (*)(int*)) PMW_UTILS_DLL_Sym(handle, "nvm_get_number_of_devices");
    if (p_nvm_get_number_of_devices == NULL) {
        return PMW_ERR_DLLSYM;
    }

    p_nvm_get_devices = (int (*)(struct device_discovery*, const NVM_UINT8)) PMW_UTILS_DLL_Sym(handle, "nvm_get_devices");
    if (p_nvm_get_devices == NULL) {
        return PMW_ERR_DLLSYM;
    }

    p_nvm_send_device_passthrough_cmd = (int (*)(const NVM_UID, struct device_pt_cmd*)) PMW_UTILS_DLL_Sym(handle, "nvm_send_device_passthrough_cmd");
    if (p_nvm_send_device_passthrough_cmd == NULL) {
        return PMW_ERR_DLLSYM;
    }

    p_nvm_get_major_version = (int (*)()) PMW_UTILS_DLL_Sym(handle, "nvm_get_major_version");
    if (p_nvm_get_major_version == NULL) {
        return PMW_ERR_DLLSYM;
    }

    p_nvm_get_minor_version = (int (*)()) PMW_UTILS_DLL_Sym(handle, "nvm_get_minor_version");
    if (p_nvm_get_minor_version == NULL) {
        return PMW_ERR_DLLSYM;
    }

    p_nvm_get_hotfix_number = (int (*)()) PMW_UTILS_DLL_Sym(handle, "nvm_get_hotfix_number");
    if (p_nvm_get_hotfix_number == NULL) {
        return PMW_ERR_DLLSYM;
    }

    p_nvm_get_build_number = (int (*)()) PMW_UTILS_DLL_Sym(handle, "nvm_get_build_number");
    if (p_nvm_get_build_number == NULL) {
        return PMW_ERR_DLLSYM;
    }

    return NVM_SUCCESS;
}
#endif

/*!
 * @fn          NVM_INT32 pmw_comm_compare_topology
 *
 * @brief       compare function for the quick sort
 *
 * @param       const void *a - NVDIMM_TOPOLOGY struct
 * @param       const void *b - NVDIMM_TOPOLOGY struct
 *
 * @return      NVM_INT32
 *
 */
NVM_INT32
pmw_comm_Compare_Topology (
    const void *a,
    const void *b
)
{
    struct device_discovery *p_a = (struct device_discovery*) a;
    struct device_discovery *p_b = (struct device_discovery*) b;

    return p_a->device_handle.handle - p_b->device_handle.handle;
}

/*!
 * @fn          void pmw_comm_Sort_DIMM_Topology
 *
 * @brief       Sort the topology struct array based on socket number
 *
 * @param       const NVM_UINT8 count       - number of dimms
 * @param       NVDIMM_TOPOLOGY p_dimm_topo - dimm topology information
 *
 * @return      void
 *
 */
void
pmw_comm_Sort_DIMM_Topology ()
{
    qsort(gp_DIMM_devices, count, sizeof(struct device_discovery), pmw_comm_Compare_Topology);
}

/*!
 * @fn          NVM_INT32 pmw_comm_Get_Mode
 *
 * @brief       Check  and print the mode of DCPMM DIMM (App Direct, memory mode or both)
 *
 * @return      NVM_SUCCESS on success
 *
 */
NVM_INT32
pmw_comm_Get_Mode ()
{
    NVM_INT32 is_ad = 0, is_2lm = 0;

    // check to see if DCPMM has memory mode or AD capability.
    struct device_capacities *p_dcap;
    p_dcap = (struct device_capacities*) calloc(1, sizeof(struct device_capacities));
    if (p_dcap == NULL) {
        fprintf(stderr, "ERROR: memory allocation failed!\n");
        return 1;
    }

    NVM_GET_NVM_CAPACITIES(p_dcap);

    if (p_dcap->memory_capacity != 0) {
        is_2lm = 1;
    }
    if (p_dcap->app_direct_capacity != 0) {
        is_ad = 1;
    }

    free(p_dcap);

    PMW_UTILS_Print_Key("%s", "system.nvdimm_mode");
    if (is_ad && is_2lm) {
        PMW_UTILS_Print_Value("\"%s\"", "app_direct+memory_mode");
    }
    else if (is_ad) {
        PMW_UTILS_Print_Value("\"%s\"", "app_direct");
    }
    else if (is_2lm) {
        PMW_UTILS_Print_Value("\"%s\"", "memory_mode");
    }
    else {
        PMW_UTILS_Print_Value("\"%s\"", "unknown");
    }

#if 0
    pmw_comm_Get_Device_Mapping();
#endif

    return NVM_SUCCESS;
}

NVM_INT32
pmw_comm_Create_Context()
{
    NVM_INT32 ret_code = 0;

    ret_code = NVM_CREATE_CONTEXT();
    if (ret_code == NVM_ERR_INVALID_PERMISSIONS || ret_code == NVM_ERR_NO_MEM) {
        fprintf(stdout, "Failed to create context. Tool performace may be impacted. Continuing...\n");
        return ret_code;
    }

    return NVM_SUCCESS;
}

NVM_INT32
pmw_comm_Free_Context()
{
    NVM_INT32 ret_code;

    ret_code = NVM_FREE_CONTEXT(1);
    if (ret_code == NVM_ERR_INVALID_PERMISSIONS || ret_code == NVM_ERR_NO_MEM) {
        fprintf(stderr, "Failed to free context\n");
        return ret_code;
    }

    return NVM_SUCCESS;
}

#ifdef BUILD_DEBUG
void
pmw_comm_Print_Payload (
    void *mb_payload,
    size_t size
)
{
    const unsigned char * const bytes = mb_payload;
    size_t i;

    printf("[ ");
    for(i = 0; i < size; i++)
    {
        printf("%02x ", bytes[i]);
        if((i+1)%8 == 0) {
            printf(",\n  ");
        }
    }
    printf("]\n");
}
#endif

void
pmw_comm_Prepare_Command (
    struct device_pt_cmd *cmd,
    NVM_UINT8      opcode,
    NVM_UINT8      sub_opcode,
    NVM_UINT32     input_payload_size,
    void          *input_payload,
    NVM_UINT32     output_payload_size,
    void          *output_payload
)
{
    cmd->opcode = opcode;
    cmd->sub_opcode = sub_opcode;
    cmd->input_payload_size = input_payload_size;
    cmd->input_payload = input_payload;
    cmd->output_payload_size = output_payload_size;
    cmd->output_payload = output_payload;
}

NVM_INT32
PMW_COMM_Get_Memory_Info_Page (
    NVM_INT32           dimm_index,
    void               *counters,
    const unsigned char page_num
)
{
    NVM_INT32             ret_code = NVM_SUCCESS;
    struct device_pt_cmd  cmd;
    void                 *p_out_payload;
    NVM_UINT32            out_payload_size;
    MEMORY_INFO_PAGE_NODE mem_info_page;   //output mailbox payload
    LIFETIME_INFO_COUNTER smt_counters;

    if (IS_LIFETIME_INFO(page_num)) {
        memset (&mem_info_page, 0, sizeof(MEMORY_INFO_PAGE_NODE));
        p_out_payload = &mem_info_page;
        out_payload_size = sizeof(MEMORY_INFO_PAGE_NODE);

        smt_counters = (LIFETIME_INFO_COUNTER) counters;
    }
    else {
        fprintf(stderr, "ERROR: Incorrect config information. Exiting...\n");
        return 1;
    }

    struct pt_payload_input_memory_info mem_input; //input mailbox payload
    memset(&cmd, 0, sizeof(struct device_pt_cmd));
    memset(&mem_input, 0, sizeof(struct pt_payload_input_memory_info));

    mem_input.memory_page = page_num;

    pmw_comm_Prepare_Command(&cmd,                                        \
                            PT_GET_LOG,                                  \
                            SUBOP_MEM_INFO,                              \
                            sizeof(struct pt_payload_input_memory_info), \
                            &mem_input,                                  \
                            out_payload_size,                            \
                            p_out_payload);

    ret_code = NVM_SEND_DEVICE_PASSTHROUGH_CMD(gp_DIMM_devices[dimm_index].uid, &cmd);
    if (ret_code != NVM_SUCCESS) {
        fprintf(stderr, "Failed to obtain count\n");
        return ret_code;
    }

    //read data from mem_info_page and copy to smt_counters strcuture
    if (IS_LIFETIME_INFO(page_num)) {
        PMW_8_BYTE_ARRAY_TO_64_BIT_VALUE(mem_info_page.total_bytes_read, smt_counters->bytes_read);
        PMW_8_BYTE_ARRAY_TO_64_BIT_VALUE(mem_info_page.total_bytes_written, smt_counters->bytes_written);
        PMW_8_BYTE_ARRAY_TO_64_BIT_VALUE(mem_info_page.total_read_reqs, smt_counters->host_reads);
        PMW_8_BYTE_ARRAY_TO_64_BIT_VALUE(mem_info_page.total_write_reqs, smt_counters->host_writes);
    }

    return ret_code;
}

NVM_INT32
PMW_COMM_Get_Health_Info_Page (
    NVM_INT32           dimm_index,
    void               *counters
)
{
    NVM_INT32             ret_code = NVM_SUCCESS;
    struct device_pt_cmd  cmd;
    void                 *p_out_payload;
    NVM_UINT32            out_payload_size;
    HEALTH_INFO_PAGE_NODE health_info_page;   //output mailbox payload
    HEALTH_INFO_COUNTER   smt_counters;

    memset (&health_info_page, 0, sizeof(HEALTH_INFO_PAGE_NODE));
    p_out_payload = &health_info_page;
    out_payload_size = sizeof(HEALTH_INFO_PAGE_NODE);

    smt_counters = (HEALTH_INFO_COUNTER) counters;

    struct pt_payload_input_memory_info mem_input; //input mailbox payload
    memset(&cmd, 0, sizeof(struct device_pt_cmd));
    memset(&mem_input, 0, sizeof(struct pt_payload_input_memory_info));

    pmw_comm_Prepare_Command(&cmd,                                        \
                            PT_GET_LOG,                                  \
                            SUBOP_SMART_HEALTH,                          \
                            sizeof(struct pt_payload_input_memory_info), \
                            &mem_input,                                  \
                            out_payload_size,                            \
                            p_out_payload);

    ret_code = NVM_SEND_DEVICE_PASSTHROUGH_CMD(gp_DIMM_devices[dimm_index].uid, &cmd);
    if (ret_code != NVM_SUCCESS) {
        fprintf(stderr, "Failed to obtain count\n");
        return ret_code;
    }

    PMW_4_BYTE_ARRAY_TO_32_BIT_VALUE(health_info_page.validation_flags, smt_counters->validation_flags);
    PMW_2_BYTE_ARRAY_TO_16_BIT_VALUE(health_info_page.media_temp, smt_counters->media_temp);
    PMW_2_BYTE_ARRAY_TO_16_BIT_VALUE(health_info_page.controller_temp, smt_counters->controller_temp);
    PMW_4_BYTE_ARRAY_TO_32_BIT_VALUE(health_info_page.vendor_data_size, smt_counters->vendor_data_size);
    PMW_8_BYTE_ARRAY_TO_64_BIT_VALUE(health_info_page.power_on_time, smt_counters->power_on_time);
    PMW_8_BYTE_ARRAY_TO_64_BIT_VALUE(health_info_page.uptime, smt_counters->uptime);
    PMW_8_BYTE_ARRAY_TO_64_BIT_VALUE(health_info_page.last_shutdown_time, smt_counters->last_shutdown_time);
    PMW_2_BYTE_ARRAY_TO_16_BIT_VALUE(health_info_page.max_media_temp, smt_counters->max_media_temp);
    PMW_2_BYTE_ARRAY_TO_16_BIT_VALUE(health_info_page.max_controller_temp, smt_counters->max_controller_temp);

    smt_counters->health_status        = health_info_page.health_status;
    smt_counters->percentage_remaining = health_info_page.percentage_remaining;

    return ret_code;
}

#if !defined(_WIN32) && !defined(BULID_ESXI)

/*!
 * @fn          int PMW_COMM_Check_Permission
 *
 * @brief       Check if user has root permission
 *
 * @return      0 on success
 *
 */
int
PMW_COMM_Check_User_Permission ()
{
    if (getuid() != ROOT_UID) {
        fprintf(stderr, "\nNon-root user!\n");
        fprintf(stderr, "Requires root access. Please run as root.\n\n");

        return 204;
    }

    return 0;
}

#endif

/*!
 * @fn          NVM_INT32 pmw_comm_Check_PreR_FW
 *
 * @brief       Check if this is a supported firmware
 *
 * @return      error code
 *
 */
NVM_INT32
pmw_comm_Check_PreR_FW (
    NVM_UINT32 *is_prer_fw
)
{
    NVM_UINT32 itr, i_fw_api_minor_ver, i_fw_api_major_ver, prer_fw = 0, status = NVM_SUCCESS;
    char *fw_api_version, *token;
    char  fw_api_minor_ver[10];
    char  fw_api_major_ver[10];

    *is_prer_fw = prer_fw;

    for(itr = 0; itr < count; itr++) {
        fw_api_version = strndup(gp_DIMM_devices[itr].fw_api_version, SMALL_STR_LEN);
        token = strtok(fw_api_version, ".");
        if (!token) {
            fprintf(stderr, "ERROR: Failure to parse version information. Exiting...\n");
            free(fw_api_version);
            return 1;
        }
        status = STRNCPY_SAFE(fw_api_major_ver, sizeof(fw_api_major_ver), token);
        if (status != NVM_SUCCESS) {
            free(fw_api_version);
            return status;
        }
        while(token) {
            status = STRNCPY_SAFE(fw_api_minor_ver, sizeof(fw_api_minor_ver), token);
            if (status != NVM_SUCCESS) {
                free(fw_api_version);
                return status;
            }
            token = strtok(NULL, ".");
        }

        i_fw_api_major_ver = atoi(fw_api_major_ver);
        i_fw_api_minor_ver = atoi(fw_api_minor_ver);

        // check for unsupported firmware
        prer_fw += (i_fw_api_major_ver > 1) ? 1 : 0;

        free(fw_api_version);
    }

    if (prer_fw) {
        *is_prer_fw = 1;
    }

    return status;
}

NVM_INT32
PMW_COMM_Print_Topology (
    NVM_INT32 get_topo,
    NVM_INT32 detailed
)
{
    NVM_INT32 itr, dimm_avail = 1, ret_code = NVM_SUCCESS;
    NVM_UINT32 dummy_count, is_prer_fw = 0;

#if !defined(_WIN32) && !defined(BULID_ESXI)
    ret_code = PMW_COMM_Check_User_Permission();
    if (ret_code != NVM_SUCCESS) {
        return ret_code;
    }
#endif

#ifdef _WIN32
    ret_code = pmw_comm_Init_DLL();
    if (ret_code != NVM_SUCCESS) {
        return 200;
    }
#endif

    if (get_topo) {
        pmw_comm_Create_Context();

        ipmctl_major_ver = NVM_GET_MAJOR_VERSION();
        ipmctl_minor_ver = NVM_GET_MINOR_VERSION();
        ipmctl_hot_fix_num = NVM_GET_HOTFIX_NUMBER();
        ipmctl_build = NVM_GET_BUILD_NUMBER();

        snprintf(ipmctl_version_str, sizeof(ipmctl_version_str), "%02d.%02d.%02d.%04d", ipmctl_major_ver, ipmctl_minor_ver, ipmctl_hot_fix_num, ipmctl_build);

        ret_code = PMW_COMM_Is_DIMM_Available();
        if (ret_code != NVM_SUCCESS) {
            dimm_avail = 0;
        }
        if(!dimm_avail) {
            PMW_UTILS_Print_Key("%s", "system.nvdimm_availability");
            PMW_UTILS_Print_Value("%d", dimm_avail);
            return 200;
        }

        ret_code = PMW_COMM_Get_Number_of_DIMM(&dummy_count);
        if (ret_code != NVM_SUCCESS) {
            count = 0;
        }
        if(!count) {
            PMW_UTILS_Print_Key("%s", "system.num_nvdimm");
            PMW_UTILS_Print_Value("%d", count);
            return 200;
        }

        ret_code = PMW_COMM_Read_DIMM_Topology();
        if (ret_code != NVM_SUCCESS) {
            fprintf(stderr, "Unable to obtain topology\n");
            PMW_COMM_Cleanup();
            return 200;
        }
    }

    pmw_comm_Check_PreR_FW(&is_prer_fw);
    if (is_prer_fw) {
        fprintf(stderr, "\nUnsupported firmware.\n\n");
        PMW_COMM_Cleanup();
        return NVM_ERR_INVALID_PERMISSIONS;
    }

    if (get_topo) {
        PMW_UTILS_Print_Key("%s", "system.nvdimm_availability");
        PMW_UTILS_Print_Value("%d", dimm_avail);
        PMW_UTILS_Print_Key("%s", "system.num_nvdimm");
        PMW_UTILS_Print_Value("%d", count);
    }

    for(itr = 0; itr < count; itr++) {
        PMW_UTILS_Print_Key("system.package[%u].memory_controller[%u].channel[%hu].dimm[%d].name", \
                             gp_DIMM_devices[itr].socket_id, \
                             gp_DIMM_devices[itr].memory_controller_id, \
                             gp_DIMM_devices[itr].channel_id, \
                             gp_DIMM_devices[itr].channel_pos
                           );
        // using channel position for DIMM ID. Not sure if it is the right way.
        PMW_UTILS_Print_Value("\"DIMM%d\"", itr);
        if (detailed) {
            PMW_UTILS_Print_Key("system.package[%u].memory_controller[%u].channel[%hu].dimm[%d].fw_version", \
                                 gp_DIMM_devices[itr].socket_id, \
                                 gp_DIMM_devices[itr].memory_controller_id, \
                                 gp_DIMM_devices[itr].channel_id, \
                                 gp_DIMM_devices[itr].channel_pos
                               );
            PMW_UTILS_Print_Value("\"%s\"", gp_DIMM_devices[itr].fw_revision);
            PMW_UTILS_Print_Key("system.package[%u].memory_controller[%u].channel[%hu].dimm[%d].fw_api_version", \
                                 gp_DIMM_devices[itr].socket_id, \
                                 gp_DIMM_devices[itr].memory_controller_id, \
                                 gp_DIMM_devices[itr].channel_id, \
                                 gp_DIMM_devices[itr].channel_pos
                               );
            PMW_UTILS_Print_Value("\"%s\"", gp_DIMM_devices[itr].fw_api_version);
        }
    }

    pmw_comm_Get_Mode();

    if (detailed) {
        PMW_UTILS_Print_Key("system.ipmctl_version");
        PMW_UTILS_Print_Value("%s", ipmctl_version_str);
    }

    PMW_COMM_Cleanup();

    return ret_code;
}

/*!
 * @fn          NVM_INT32 PMW_COMM_Read_DIMM_Topology
 *
 * @brief       Get the topology info of each DCPMM DIMMs
 *
 * @param       const NVM_UINT8 count       - number of dimms
 * @param       NVDIMM_TOPOLOGY p_dimm_topo - dimm topology information
 *
 * @return      error code
 *
 */
NVM_INT32
PMW_COMM_Read_DIMM_Topology ()
{
    NVM_INT32 ret_code;

    gp_DIMM_devices = (struct device_discovery*) calloc(count, sizeof(struct device_discovery));
    if (gp_DIMM_devices == NULL){
        fprintf(stderr, "Memory allocation failed!\n");
        ret_code = NVM_ERR_NO_MEM;
        return ret_code;
    }

    ret_code = NVM_GET_DEVICES(gp_DIMM_devices, count);
    if (ret_code != NVM_SUCCESS) {
        fprintf(stderr, "Obtaining DIMM details failed!\n");
        return NVM_ERR_UNKNOWN;
    }

    pmw_comm_Sort_DIMM_Topology();

    return NVM_SUCCESS;
}

/*!
 * @fn          NVM_INT32 PMW_COMM_Get_Number_of_DIMM
 *
 * @brief       Get the number of DCPMM DIMMs
 *
 * @return      error code
 *
 */
NVM_INT32
PMW_COMM_Get_Number_of_DIMM (
    NVM_UINT32 *dimm_count
)
{
    NVM_INT32 ret_val = NVM_SUCCESS;

    ret_val = NVM_GET_NUMBER_OF_DEVICES(&count);

    *dimm_count = count;

    if (count == 0) {
        fprintf(stderr, "Intel Optane DIMMs are not available in this system.\n");
        return NVM_ERR_DIMM_NOT_FOUND;
    }

    if (ret_val != NVM_SUCCESS) {
        fprintf(stderr, "Obtaining the number of Intel Optane DIMMs failed!\n");
    }

    return ret_val;
}

/*!
 * @fn          NVM_INT32 PMW_COMM_Is_DIMM_Available
 *
 * @brief       Check if DCPMM DIMM is available in the machine or not
 *              Print DCPMM DIMM details if print_info is enabled
 *              Check the DCPMM DIMM mode if print_info is enabled
 *
 * @return      error code
 *
 */
NVM_INT32
PMW_COMM_Is_DIMM_Available ()
{
    NVM_INT32 count, ret_val = NVM_SUCCESS;

    ret_val = NVM_GET_NUMBER_OF_DEVICES(&count);
    if (count == 0) {
        return NVM_ERR_DIMM_NOT_FOUND;
    }

    if (ret_val != NVM_SUCCESS) {
        fprintf(stderr, "Obtaining the number of Intel Optane DIMMs failed!\n");
    }

    return ret_val;
}

/*!
 * @fn          void PMW_COMM_Print_Warning_Message
 *
 * @brief       Print PMWatch pre-requisites as warning
 *
 * @return      void
 *
 */
void
PMW_COMM_Print_Warning_Message ()
{
    fprintf(stderr, "\nWARNING: pmwatch pre-requisites:\n");
    fprintf(stderr, "         Firmware version may be unsupported.\n");

    if (count != 0 && gp_DIMM_devices != NULL) {
        fprintf(stderr, "         Firmware version found on the system: %s\n", gp_DIMM_devices[0].fw_revision);
    }

    fprintf(stderr, "\n         ipmctl >= ");
    fprintf(stderr, "%s\n", PMWATCH_SUPPORT_IPMCTL_VERSION_MIN);
    fprintf(stderr, "         ipmctl found on the system: %02d.%02d.%02d.%04d\n", ipmctl_major_ver, ipmctl_minor_ver, ipmctl_hot_fix_num, ipmctl_build);
    fprintf(stderr, "\npmwatch is not guaranteed to work with versions less than the ones mentioned above.\n\n");

    return;
}

/*!
 * @fn          NVM_INT32 PMW_COMM_Init
 *
 * @brief       Intialize APIs
 *
 * @param       None
 *
 * @return      error code
 *
 */
NVM_INT32
PMW_COMM_Init ()
{
    NVM_INT32 ret_code = NVM_SUCCESS;
    NVM_UINT32 dummy_count, is_prer_fw = 0;
    gp_DIMM_devices = NULL;

#ifdef _WIN32
    ret_code = pmw_comm_Init_DLL();
    if (ret_code != NVM_SUCCESS) {
        return 201;
    }
#endif

    pmw_comm_Create_Context();

#if !defined(_WIN32) && !defined(BULID_ESXI)
    ret_code = PMW_COMM_Check_User_Permission();
    if (ret_code != NVM_SUCCESS) {
        return ret_code;
    }
#endif

    ipmctl_major_ver = NVM_GET_MAJOR_VERSION();
    ipmctl_minor_ver = NVM_GET_MINOR_VERSION();
    ipmctl_hot_fix_num = NVM_GET_HOTFIX_NUMBER();
    ipmctl_build = NVM_GET_BUILD_NUMBER();

    snprintf(ipmctl_version_str, sizeof(ipmctl_version_str), "%02d.%02d.%02d.%04d", ipmctl_major_ver, ipmctl_minor_ver, ipmctl_hot_fix_num, ipmctl_build);

    ret_code = PMW_COMM_Is_DIMM_Available();
    if (ret_code != NVM_SUCCESS) {
        fprintf(stderr, "Intel Optane DIMMS are not available on this system.\n");
        return 201;
    }

    ret_code = PMW_COMM_Get_Number_of_DIMM(&dummy_count);
    if (ret_code != NVM_SUCCESS) {
        return 201;
    }

    ret_code = PMW_COMM_Read_DIMM_Topology();
    if (ret_code != NVM_SUCCESS) {
        fprintf(stderr, "Failed to initialize.\n");
        return 201;
    }

    pmw_comm_Check_PreR_FW(&is_prer_fw);
    if (is_prer_fw) {
        fprintf(stderr, "\nUnsupported firmware.\n\n");
        return NVM_ERR_INVALID_PERMISSIONS;
    }

    return ret_code;
}

NVM_INT32
PMW_COMM_Cleanup ()
{
    free(gp_DIMM_devices);

    pmw_comm_Free_Context();

#ifdef _WIN32
    PMW_UTILS_DLL_Close(handle);
#endif

    return NVM_SUCCESS;
}
