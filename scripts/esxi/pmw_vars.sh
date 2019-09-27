#!/bin/sh

# =============================================================================
# Filename: pmw_vars.sh
# Version: 1.0
# Purpose: PMWatch Runtime environment setup script
# Description: This script should be used to set up the run-time environment
#              for PMWatch. It requires sh. Run this script before running
#              PMWatch executable.
#
# Usage: source pmw_vars.sh
#
# MIT License
#
# Copyright (C) 2018 - 2019 Intel Corporation. All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
#
# =============================================================================

echo "Please source this script under pmwatch install directory"

GREP=grep

#
# Note: Busybox has a restricted shell environment, and
#       conventional system utilities may not be present;
#       so need to account for this ...
#
# busybox binary check
BUSYBOX_SHELL=` ${GREP} --help 2>&1 | ${GREP} BusyBox`

if [ -n "${BUSYBOX_SHELL}" ] ; then
    echo "Running on Busybox!"
fi

SCRIPT_DIR="$( cd "$( dirname "sh" )" && pwd )"

export PMW_LOC_PATH="${SCRIPT_DIR}"
export PMW_INSTALL_PATH="${PMW_LOC_PATH}/bin64"
export LD_LIBRARY_PATH="${PMW_LOC_PATH}/lib64"
export PATH="${PMW_INSTALL_PATH}:${PATH}"

# show settings of various environment variables
echo "LD_LIBRARY_PATH=${LD_LIBRARY_PATH}"
echo "PATH=${PATH}"
echo ""
echo "pmwatch is currently installed under ${PMW_INSTALL_PATH}"
echo ""
echo "pmwatch works only when ESXi is in maintenance mode."
echo ""
