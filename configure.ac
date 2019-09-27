AC_INIT(Intel PMWatch, 3.0)
PROJECT_CFLAGS="-std=gnu99 -I./src/inc"

AC_SUBST(PROJECT_CFLAGS)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(src/pmwatch.c)
: ${CXXFLAGS="-O2 -D_FORTIFY_SOURCE=2 -s"}
AC_PROG_CXX
: ${CFLAGS="-O2 -D_FORTIFY_SOURCE=2 -s"}
AC_PROG_CC
AM_INIT_AUTOMAKE
LT_INIT
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_FILES([Makefile])
AM_CONDITIONAL(BUILD_ESXI, [test x${BUILD_ESXI} = xBUILD_ESXI])
if test x${BUILD_ESXI} != xBUILD_ESXI ; then
	PKG_CHECK_MODULES([KMOD], [libkmod])
	PKG_CHECK_MODULES([UDEV], [libudev])
	PKG_CHECK_MODULES([UUID], [uuid])
fi

m4_divert_once([HELP_WITH], [
PMWatch Dependencies:])

# --with-libipmctl BEGIN
AC_ARG_WITH([libipmctl],
  [AS_HELP_STRING([--with-libipmctl@<:@=PREFIX@:>@], [Path to libipmctl.])],
  [
    if test "x$withval" = "xyes" || test "x$withval" = "xno" ; then
      with_libipmctl="$withval"
    else
      with_libipmctl_ldflags="-L$withval"
      with_libipmctl="yes"
    fi
  ],
  [with_libipmctl="yes"]
)

if test "x$with_libipmctl" = "xyes" ; then
  SAVE_LDFLAGS="$LDFLAGS"
  LDFLAGS="$LDFLAGS $with_libipmctl_ldflags"

  AC_CHECK_LIB([ipmctl], [nvm_create_context],
    [with_libipmctls="yes"],
    [with_libipmctl="no (symbol 'nvm_create_context' not found)"]
  )

  AC_CHECK_LIB([ipmctl], [nvm_free_context],
    [with_libipmctls="yes"],
    [with_libipmctl="no (symbol 'nvm_free_context' not found)"]
  )

  AC_CHECK_LIB([ipmctl], [nvm_get_build_number],
    [with_libipmctls="yes"],
    [with_libipmctl="no (symbol 'nvm_get_build_number' not found)"]
  )

  AC_CHECK_LIB([ipmctl], [nvm_get_nvm_capacities],
    [with_libipmctls="yes"],
    [with_libipmctl="no (symbol 'nvm_get_nvm_capacities' not found)"]
  )

  AC_CHECK_LIB([ipmctl], [nvm_get_number_of_devices],
    [with_libipmctls="yes"],
    [with_libipmctl="no (symbol 'nvm_get_number_of_devices' not found)"]
  )

  AC_CHECK_LIB([ipmctl], [nvm_get_devices],
    [with_libipmctls="yes"],
    [with_libipmctl="no (symbol 'nvm_get_devices' not found)"]
  )

  AC_CHECK_LIB([ipmctl], [nvm_send_device_passthrough_cmd],
    [with_libipmctls="yes"],
    [with_libipmctl="no (symbol 'nvm_send_device_passthrough_cmd' not found)"]
  )

  AC_CHECK_LIB([ipmctl], [nvm_get_major_version],
    [with_libipmctls="yes"],
    [with_libipmctl="no (symbol 'nvm_get_major_version' not found)"]
  )

  AC_CHECK_LIB([ipmctl], [nvm_get_minor_version],
    [with_libipmctls="yes"],
    [with_libipmctl="no (symbol 'nvm_get_minor_version' not found)"]
  )

  AC_CHECK_LIB([ipmctl], [nvm_get_hotfix_number],
    [with_libipmctls="yes"],
    [with_libipmctl="no (symbol 'nvm_get_hotfix_number' not found)"]
  )

  AC_CHECK_LIB([ipmctl], [nvm_get_build_number],
    [with_libipmctls="yes"],
    [with_libipmctl="no (symbol 'nvm_get_build_number' not found)"]
  )

  LDFLAGS="$SAVE_LDFLAGS"
fi

if test "x$with_libipmctl" = "xyes" ; then
  BUILD_WITH_LIBIPMCTL_LDFLAGS="$with_libipmctl_ldflags"
fi

AC_SUBST([BUILD_WITH_LIBIPMCTL_LDFLAGS])
# END

# --with-libipmctl-inc BEGIN
AC_ARG_WITH([libipmctl-inc],
  [AS_HELP_STRING([--with-libipmctl-inc@<:@=PREFIX@:>@], [Path to libipmctl header files.])],
  [
    if test "x$withval" = "xyes" || test "x$withval" = "xno" ; then
      with_libipmctlinc="$withval"
    else
      with_libipmctl_cflags="-I$withval"
      with_libipmctlinc="yes"
    fi
  ],
  [with_libipmctlinc="yes"]
)

if test "x$with_libipmctlinc" = "xyes" ; then
  SAVE_CFLAGS="$CFLAGS"
  CFLAGS="$CFLAGS $with_libipmctl_cflags"

  AC_CHECK_HEADERS([NvmSharedDefs.h],
    [with_libipmctlinc="yes"],
    [with_libipmctlinc="no (NvmSharedDefs.h not found)"]
  )

  AC_CHECK_HEADERS([nvm_types.h],
    [with_libipmctlinc="yes"],
    [with_libipmctlinc="no (nvm_types.h not found)"]
  )

  AC_CHECK_HEADERS([export_api.h],
    [with_libipmctlinc="yes"],
    [with_libipmctlinc="no (export_api.h not found)"]
  )

  AC_CHECK_HEADERS([nvm_management.h],
    [with_libipmctlinc="yes"],
    [with_libipmctlinc="no (nvm_management.h not found)"]
  )

  CFLAGS="$SAVE_CFLAGS"
fi

if test "x$with_libipmctlinc" = "xyes" ; then
  BUILD_WITH_LIBIPMCTL_CFLAGS="$with_libipmctl_cflags"
fi

AC_SUBST([BUILD_WITH_LIBIPMCTL_CFLAGS])
# END

AM_CONDITIONAL(BUILD_PMWATCH, test "x$with_libipmctl" = "xyes")
if test "x$with_libipmctlinc" != "xyes" ; then
    AM_CONDITIONAL(BUILD_PMWATCH, test "x$with_libipmctlinc" = "xyes")
fi

AC_OUTPUT

AC_MSG_RESULT()
AC_MSG_RESULT([Library availability:])
AC_MSG_RESULT([    libipmctl  . . . . . .  . $with_libipmctl])
AC_MSG_RESULT([Header files availability:])
AC_MSG_RESULT([    libipmctl-inc . . . . . . $with_libipmctlinc])
AC_MSG_RESULT()
