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

#ifndef _PMW_VERSION_H_INC_
#define _PMW_VERSION_H_INC_

#define _STRINGIFY(x)  #x
#define STRINGIFY(x)   _STRINGIFY(x)

#define PRODUCT_NAME   "Intel(R) PMWatch"

#define MAJOR_VERSION  3
#define MINOR_VERSION  2
#define UPDATE_VERSION 1
#if UPDATE_VERSION > 0
#define UPDATE_STRING  " Update "STRINGIFY(UPDATE_VERSION)
#else
#define UPDATE_STRING  ""
#endif

#define ENG_BUILD      ""

#define OUTPUT_FORMAT_MAJOR_VERSION 1
#define OUTPUT_FORMAT_MINOR_VERSION 0

#define PRODUCT_TYPE   "public"

#define PRODUCT_TYPE_STR " ("PRODUCT_TYPE")"

#endif
