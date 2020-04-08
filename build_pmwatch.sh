#!/bin/bash

print_usage_and_exit()
{
    echo "Usage: ./build_pmwatch.sh <-all|-cm|-m> with-libipmctl=<path_to_libipmctl> with-libipmctl-inc=<path_to_ipmctl_header_files> os=[esxi]"

    exit $1
}

if [ $# = 0 ] ; then
    print_usage_and_exit 0
fi

RUN_CONFIG=
RUN_MAKE=
RUN_CLEANMAKE=
BUILD_ESXI=
WITH_IPMCTL=
WITH_IPMCTL_INC=
while [ $# -gt 0 ] ; do
  case "$1" in
    --help | -h)
        print_usage_and_exit 0
        ;;
    -all)
        RUN_CONFIG=1
        RUN_CLEANMAKE=1
        RUN_MAKE=1
        ;;
    -cm)
        RUN_CLEANMAKE=1
        RUN_MAKE=1
        ;;
    -m)
        RUN_MAKE=1
        ;;
    with-libipmctl=*)
        WITH_IPMCTL=`echo $1 | sed s?^with-libipmctl=??g`
        ;;
    with-libipmctl-inc=*)
        WITH_IPMCTL_INC=`echo $1 | sed s?^with-libipmctl-inc=??g`
        ;;
    os=*)
        OS=`echo $1 | sed s?^os=??g`
        if [[ ${OS} = "esxi" ]] ; then
            BUILD_ESXI="BUILD_ESXI"
        fi
        ;;
    *)
       echo ""
       echo "Invalid option: \"$1\""
       print_usage_and_exit 1
       ;;
  esac
  shift
done

if [[ -n "${RUN_CONFIG}" ]] ; then
    echo "***************************************"
    echo "Executing ./autogen.sh..."
    echo "***************************************"
    ./autogen.sh
    if [[ $? -eq 0 ]] ; then
        echo "***************************************"
        echo "Execution of autogen.sh successful."
        echo "***************************************"
        echo
    else
        echo "***************************************"
        echo "Execution of autogen.sh failed."
        echo "***************************************"
        echo
        exit 1
    fi

    echo "***************************************"
    echo "Executing ./configure..."
    echo "***************************************"
    ./configure BUILD_ESXI=${BUILD_ESXI} --with-libipmctl=${WITH_IPMCTL} --with-libipmctl-inc=${WITH_IPMCTL_INC}
    if [[ $? -eq 0 ]] ; then
        echo "***************************************"
            echo "Execution of configure successful."
        echo "***************************************"
        echo
    else
        echo "***************************************"
            echo "Execution of configure failed."
        echo "***************************************"
        echo
        exit 1
    fi
fi

if [[ -n "${RUN_CLEANMAKE}" ]] ; then
    echo "***************************************"
    echo "Executing make clean..."
    echo "***************************************"
    make clean
    if [[ $? -eq 0 ]] ; then
        echo "***************************************"
            echo "Execution of make clean successful."
        echo "***************************************"
        echo
    else
        echo "***************************************"
            echo "Execution of make clean failed."
        echo "***************************************"
        echo
        exit 1
    fi
fi

if [[ -n "${RUN_MAKE}" ]] ; then
    echo "***************************************"
    echo "Executing make..."
    echo "***************************************"
    make -j 10 | tee makelog.log
    res=$?
    cat makelog.log | grep "Nothing to be done for 'all'" >> /dev/null 2>&1
    res_grep=$?
    rm -rf makelog.log
    if [[ ${res} -eq 0 && ${res_grep} -ne 0 ]] ; then
        echo "***************************************"
            echo "Execution of make successful."
        echo "***************************************"
        echo
    else
        echo "***************************************"
            echo "Execution of make failed."
        echo "***************************************"
        echo
        exit 1
    fi
fi
