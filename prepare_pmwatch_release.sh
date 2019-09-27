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

print_usage_and_exit()
{
    echo "Usage: ./prepare_pmwatch_release.sh M=<major_version> m=<minor_version> u=<update_num> with-libipmctl=<path_to_libipmctl> with-libipmctl-inc=<path_to_ipmctl_header_files> os=<linux|esxi> [-r]"
    echo "Option:"
    echo "       -r  removes the package directory after tarball creation"
    echo

    exit $1
}

if [ $# = 0 ] ; then
    print_usage_and_exit 0
fi

OS=linux
remove_package_dir=
WITH_IPMCTL=
WITH_IPMCTL_INC=
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
    with-libipmctl=*)
        WITH_IPMCTL=`echo $1 | sed s?^with-libipmctl=??g`
        ;;
    with-libipmctl-inc=*)
        WITH_IPMCTL_INC=`echo $1 | sed s?^with-libipmctl-inc=??g`
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
        remove_package_dir=$1
        ;;
    *)
       echo ""
       echo "Invalid option: \"$1\""
       print_usage_and_exit 1
       ;;
  esac
  shift
done

./build_pmwatch.sh -all os=${OS} with-libipmctl=${WITH_IPMCTL} with-libipmctl-inc=${WITH_IPMCTL_INC} os=${OS}
if [[ $? -ne 0  ]] ; then
   exit 1
fi

./package_pmwatch.sh M=${MAJOR_VERSION} m=${MINOR_VERSION} u=${UPDATE_NUM} os=${OS} ${remove_package_dir}
if [[ $? -ne 0  ]] ; then
    exit 1
fi
