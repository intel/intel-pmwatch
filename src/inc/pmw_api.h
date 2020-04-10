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

#ifndef _PMW_API_H_INC_
#define _PMW_API_H_INC_

#include "pmw_struct.h"

#ifndef PMW_API
#define PMW_API
#endif

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
extern PMW_API int
PMWAPIGetDIMMCount (
    int *num_nvdimms
);

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
extern PMW_API int
PMWAPIStart (
    PMWATCH_CONFIG_NODE pmwatch_config
);

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
extern PMW_API int
PMWAPIStop ();

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
extern PMW_API int
PMWAPIRead (
    PMWATCH_OP_BUF* op_buf
);

#endif
