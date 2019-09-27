@echo off
rem ===============================================================================================

rem Purpose: PMWatch Runtime environment setup script

rem Description: This script should be used to set up the run-time environment for PMWatch.

rem MIT License
rem
rem Copyright (C) 2017 - 2019 Intel Corporation. All rights reserved.
rem
rem Permission is hereby granted, free of charge, to any person obtaining a
rem copy of this software and associated documentation files (the "Software"),
rem to deal in the Software without restriction, including without limitation
rem the rights to use, copy, modify, merge, publish, distribute, sublicense,
rem and/or sell copies of the Software, and to permit persons to whom the
rem Software is furnished to do so, subject to the following conditions:
rem
rem The above copyright notice and this permission notice shall be included in
rem all copies or substantial portions of the Software.
rem
rem THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
rem IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
rem FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
rem AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
rem LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
rem FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
rem DEALINGS IN THE SOFTWARE.

rem ===============================================================================================


set PMW_PKG_DIR=%~dp0%

if "%PROCESSOR_ARCHITECTURE%"=="x86" (
    set PMW_BIN_DIR=bin32
)
if '%PROCESSOR_ARCHITECTURE%x'=='AMD64x' (
    set PMW_BIN_DIR=bin64
)

set PMW_LOC_PATH=%PMW_PKG_DIR%
set PMW_INSTALL_PATH=%PMW_LOC_PATH%%PMW_BIN_DIR%
set PATH=%PMW_INSTALL_PATH%;%PATH%

rem echo all the variables
rem echo PATH: %PATH%
echo pmwatch is currently installed under %PMW_INSTALL_PATH%

goto noerr

:err
echo Error occured during environment setup.
goto :eof

:noerr
echo pmwatch run time environment successfully set.
