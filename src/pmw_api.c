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

#include "nvm_types.h"
#include "pmw_collect.h"

#ifndef _WIN32

/* ------------------------------------------------------------------------- */
/*!
 * @fn          int PMWAPIGetDIMMCount (
 *                                      int *num_nvdimms
 *                                     )
 *
 * @brief       function to obtain the number of DCPMM DIMMs
 *
 * @param       OUT num_nvdimms: number of DCPMM DIMMs
 *
 * @return      0 if successful
 *
 * <I>Special Notes:</I>
 *              <NONE>
 */
int
PMWAPIGetDIMMCount (
    int *num_nvdimms
)
{
    return PMW_COLLECT_Get_DIMM_Count(num_nvdimms);
}

/* ------------------------------------------------------------------------- */
/*!
 * @fn          int PMWAPIStart (
 *                               PMWATCH_CONFIG_NODE pmwatch_config
 *                              )
 *
 * @brief       function to start the collection
 *
 * @param       IN pmwatch_config: struct that provides collection configuration
 *
 * @return      0 if successful
 *
 * <I>Special Notes:</I>
 *              <NONE>
 */
int
PMWAPIStart (
    PMWATCH_CONFIG_NODE pmwatch_config
)
{
    return PMW_COLLECT_Start_Collection(pmwatch_config);
}

/* ------------------------------------------------------------------------- */
/*!
 * @fn          int PMWAPIStop ()
 *
 * @brief       function to stop the collection
 *
 * @param       None
 *
 * @return      0 if successful
 *
 * <I>Special Notes:</I>
 *              <NONE>
 */
int
PMWAPIStop ()
{
    return PMW_COLLECT_Stop_Collection();
}

/* ------------------------------------------------------------------------- */
/*!
 * @fn          int PMWAPIRead (
 *                              PMWATCH_OP_BUF* op_buf
 *                             )
 *
 * @brief       function to obtain the metrics count
 *
 * @param       OUT op_buf: output buffer
 *
 * @return      0 if successful
 *
 * <I>Special Notes:</I>
 *              <NONE>
 */
int
PMWAPIRead (
    PMWATCH_OP_BUF* op_buf
)
{
    return PMW_COLLECT_Read_Data(op_buf);
}

#endif
