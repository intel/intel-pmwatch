#!/bin/bash

 # MIT License
 #
 # Copyright (C) 2017 - 2019 Intel Corporation. All rights reserved.
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

PWD="pwd"
CWD=`${PWD}`

print_usage_and_exit()
{
    echo "Usage: ./package_pmwatch.sh M=<major_number> m=<minor_number> u=<update_num> os=<linux|esxi> [-r]"
    echo "Option:"
    echo "       -r  removes the package directory after tarball creation"
    echo
    echo "Always run this script at the base directory."

    exit $1
}

if [ $# = 0 ] ; then
    print_usage_and_exit 0
fi

OS=
remove_package_dir=
while [ $# -gt 0 ] ; do
  case "$1" in
    --help | -h)
        print_usage_and_exit 0
        ;;
    M=*)
        MAJOR_VERSION=`echo $1 | sed s?^M=??g`
        if [[ ! ${MAJOR_VERSION} =~ ^-?[0-9]+$ ]] ; then
            echo "Invalid argument $1! Use integer"
            exit
        fi
        ;;
    m=*)
        MINOR_VERSION=`echo $1 | sed s?^m=??g`
        if [[ ! ${MINOR_VERSION} =~ ^-?[0-9]+$ ]] ; then
            echo "Invalid argument $1! Use integer"
            exit
        fi
        ;;
    u=*)
        UPDATE_NUM=`echo $1 | sed s?^u=??g`
        if [[ ! ${UPDATE_NUM} =~ ^-?[0-9]+$ ]] ; then
            echo "Invalid argument $1! Use integer"
            exit
        fi
        ;;
    os=*)
        OS=`echo $1 | sed s?^os=??g`
        if [[ ${OS} != "esxi" && ${OS} != "linux" ]] ; then
            echo ""
            echo "Invalid OS!"
            print_usage_and_exit
        fi
        ;;
    -r)
        remove_package_dir=1
        ;;
    *)
       echo ""
       echo "Invalid option: \"$1\""
       print_usage_and_exit 1
       ;;
  esac
  shift
done


TOOL_NAME="pmwatch"
PACKAGE_NAME="${TOOL_NAME}_${MAJOR_VERSION}.${MINOR_VERSION}.${UPDATE_NUM}"
PKG="package"
OUTER_PACKAGE_NAME="${PACKAGE_NAME}_${OS}_${PKG}"
PACKAGE_DIR="${CWD}/${PACKAGE_NAME}"
OUTER_PACKAGE_DIR="${CWD}/${OUTER_PACKAGE_NAME}"

if [[ ${OS} == "esxi" ]] ; then
    DIR_EXT="esxi"
fi

# setting up path
PMW_SCRIPTS_DIR="scripts"
PMW_DOCS_DIR="docs"
PMW_SCRIPTS_PATH="${CWD}/${PMW_SCRIPTS_DIR}"
PMW_DOCS_PATH="${CWD}/${PMW_DOCS_DIR}"
BIN_DIR="${CWD}/bin64"
LIB_DIR="${CWD}/lib64"
INCLUDE_DIR="${CWD}/include"

# scripts
# pmw_vars
PMW_VARS="pmw_vars.sh"
if [[ ${OS} == "esxi" ]] ; then
    PMW_VARS="${DIR_EXT}/pmw_vars.sh"
fi
PMW_VARS_PATH="${PMW_SCRIPTS_PATH}/${PMW_VARS}"

# installer
INSTALL_SCRIPT="install-pmw"
if [[ ${OS} == "esxi" ]] ; then
    INSTALL_SCRIPT="${DIR_EXT}/install-pmw"
fi
INSTALL_SCRIPT_PATH="${PMW_SCRIPTS_PATH}/${INSTALL_SCRIPT}"

# docs
README="README"
if [[ ${OS} == "esxi" ]] ; then
    README="${DIR_EXT}/README"
fi
PMW_USER_GUIDE="PMWatch_User_Guide.pdf"
README_INSTALL="README_INSTALL"
if [[ ${OS} == "esxi" ]] ; then
    README_INSTALL="${DIR_EXT}/README_INSTALL"
fi
README_PATH="${PMW_DOCS_PATH}/${README}"
README_INSTALL_PATH="${PMW_DOCS_PATH}/${README_INSTALL}"
PMW_USER_GUIDE_PATH="${PMW_DOCS_PATH}/${PMW_USER_GUIDE}"

# bin
PMW_BIN1_PATH="${CWD}/pmwatch"
PMW_BIN2_PATH="${CWD}/src/pmwatch-stop"

# libs
PMW_LIB_BASE_PATH="${CWD}/.libs"
PMW_LIB1_NAME="libpmwapi.so*"
PMW_LIB2_NAME="libpmwcollect.so*"
PMW_LIB1_PATH="${PMW_LIB_BASE_PATH}/${PMW_LIB1_NAME}"
PMW_LIB2_PATH="${PMW_LIB_BASE_PATH}/${PMW_LIB2_NAME}"

# header files
PMW_INCLUDE_BASE_PATH="${CWD}/src/inc"
PMW_HEADER_FILE1="pmw_api.h"
PMW_HEADER_FILE2="pmw_struct.h"
PMW_INCLUDE1_PATH="${PMW_INCLUDE_BASE_PATH}/${PMW_HEADER_FILE1}"
PMW_INCLUDE2_PATH="${PMW_INCLUDE_BASE_PATH}/${PMW_HEADER_FILE2}"

# tar_name
TAR_EXT=".tar.bz2"
TAR_NAME="${PACKAGE_NAME}${TAR_EXT}"
TAR_PATH="${CWD}/${TAR_NAME}"
OUTER_TAR_NAME="${OUTER_PACKAGE_NAME}${TAR_EXT}"
OUTER_TAR_PATH="${CWD}/${OUTER_TAR_NAME}"


mkdir ${PACKAGE_DIR}
if [ $? -ne 0 ] ; then
    echo "Package directory not successfully created!"
    exit
fi

if [[ ! -d ${BIN_DIR} ]]; then
    mkdir ${BIN_DIR}
    if [[ $? -ne 0 ]] ; then
        echo "Package creation not successful; mkdir ${BIN_DIR}!"
        echo
        exit 1
    fi
fi

cp -a {${PMW_BIN1_PATH},${PMW_BIN2_PATH}} ${BIN_DIR}
if [ $? -ne 0 ] ; then
    echo "Package creation not successful; ${BIN_DIR} copy!"
    echo
    exit 1
fi

if [[ ! -d ${LIB_DIR} ]]; then
    mkdir ${LIB_DIR}
    if [[ $? -ne 0 ]] ; then
        echo "Package creation not successful; mkdir ${LIB_DIR}!"
        echo
        exit 1
    fi
fi

cp -a ${PMW_LIB1_PATH} ${LIB_DIR}
if [ $? -ne 0 ] ; then
    echo "Package creation not successful; ${LIB_DIR} copy!"
    echo
    exit 1
fi
cp -a ${PMW_LIB2_PATH} ${LIB_DIR}
if [ $? -ne 0 ] ; then
    echo "Package creation not successful; ${LIB_DIR} copy!"
    echo
    exit 1
fi

if [[ ! -d ${INCLUDE_DIR} ]]; then
    mkdir ${INCLUDE_DIR}
    if [[ $? -ne 0 ]] ; then
        echo "Package creation not successful; mkdir ${LIB_DIR}!"
        echo
        exit 1
    fi
fi

cp -a ${PMW_INCLUDE1_PATH} ${INCLUDE_DIR}
if [ $? -ne 0 ] ; then
    echo "Package creation not successful; ${INCLUDE_DIR} copy!"
    echo
    exit 1
fi
cp -a ${PMW_INCLUDE2_PATH} ${INCLUDE_DIR}
if [ $? -ne 0 ] ; then
    echo "Package creation not successful; ${INCLUDE_DIR} copy!"
    echo
    exit 1
fi

mv {${BIN_DIR},${LIB_DIR},${INCLUDE_DIR}} ${PACKAGE_DIR}
if [ $? -ne 0 ] ; then
    echo "Package creation not successful; intial copy!"
    echo
    if [ -d ${PACKAGE_DIR} ] ; then
        rm -rf ${PACKAGE_DIR}
    fi
    exit 1
fi
cp -r {${PMW_VARS_PATH},${README_PATH},${PMW_USER_GUIDE_PATH}} ${PACKAGE_DIR}
if [ $? -ne 0 ] ; then
    echo "Package creation not successful; intial copy!"
    echo
    if [ -d ${PACKAGE_DIR} ] ; then
        rm -rf ${PACKAGE_DIR}
    fi
    exit 1
fi

echo "Package directory created successfully."

res=`tar cvfj ${TAR_PATH} ${PACKAGE_NAME}`
if [ $? -ne 0 ] ; then
    echo "Core package tar ball cannot be created successfully."
    echo
    if [ -d ${PACKAGE_DIR} ] ; then
        rm -rf ${PACKAGE_DIR}
    fi
    exit 1
else
    echo "Core package tar ball created successfully."
fi

mkdir ${OUTER_PACKAGE_DIR}
if [ $? -ne 0 ] ; then
    echo "Unable to create ${OUTER_PACKAGE_DIR}! Exiting..."
    echo
    rm -rf ${PACKAGE_DIR} ${TAR_PATH}
    if [ -d ${OUTER_PACKAGE_DIR} ] ; then
        rm -rf ${OUTER_PACKAGE_DIR}
    fi
    exit 1
fi

cp {${TAR_PATH},${README_INSTALL_PATH},${INSTALL_SCRIPT_PATH}} ${OUTER_PACKAGE_DIR}
if [ $? -ne 0 ] ; then
    echo "Unable to package ${OUTER_PACKAGE_NAME}; copy failed! Exiting..."
    echo
    rm -rf ${PACKAGE_DIR} ${TAR_PATH}
    if [ -d ${OUTER_PACKAGE_DIR} ] ; then
        rm -rf ${OUTER_PACKAGE_DIR}
    fi
    exit 1
fi

res=`tar cvfj ${OUTER_TAR_PATH} ${OUTER_PACKAGE_NAME}`
if [ $? -ne 0 ] ; then
    echo "Final package tar ball cannot be created successfully."
    echo
    rm -rf ${PACKAGE_DIR} ${TAR_PATH} ${OUTER_PACKAGE_DIR}
    exit 1
else
    echo "Final package tar ball created successfully."
fi

if [[ -n "${remove_package_dir}" ]] ; then
    rm -rf ${PACKAGE_DIR} ${OUTER_PACKAGE_DIR} ${TAR_PATH}
    if [ $? -ne 0 ] ; then
        echo "${PACKAGE_NAME}, ${OUTER_PACKAGE_NAME} and ${TAR_NAME} were not removed."
        echo
        exit 1
    else
        echo "${PACKAGE_NAME}, ${OUTER_PACKAGE_NAME} and ${TAR_NAME} were removed."
        echo
    fi
fi
