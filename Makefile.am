ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = $(PROJECT_CFLAGS) \
    $(KMOD_CFLAGS) \
    $(UDEV_CFLAGS) \
    $(UUID_CFLAGS)
EXTRA_DIST = autogen.sh
AUTOMAKE_OPTIONS = subdir-objects foreign

if BUILD_PMWATCH
lib_LTLIBRARIES = libpmwcollect.la libpmwapi.la
libpmwcollect_la_SOURCES = src/pmw_collect.c src/pmw_comm.c src/pmw_utils.c
libpmwcollect_la_LDFLAGS = $(BUILD_WITH_LIBIPMCTL_LDFLAGS)
if BUILD_ESXI
libpmwcollect_la_LIBADD = -lipmctl -lvmkuser -lpthread
libpmwcollect_la_CFLAGS = ${NEW_CFLAGS} $(AM_CFLAGS) -DBUILD_ESXI $(BUILD_WITH_LIBIPMCTL_CFLAGS)
else
libpmwcollect_la_LIBADD = -lipmctl -lpthread
libpmwcollect_la_CFLAGS = ${NEW_CFLAGS} $(AM_CFLAGS) $(BUILD_WITH_LIBIPMCTL_CFLAGS)
endif
libpmwapi_la_SOURCES = src/pmw_api.c
libpmwapi_la_LIBADD = -L./.libs libpmwcollect.la
libpmwapi_la_CFLAGS = ${NEW_CFLAGS} $(AM_CFLAGS) $(BUILD_WITH_LIBIPMCTL_CFLAGS)

include_HEADERS =  src/inc/pmw_api.h src/inc/pmw_struct.h

dist_bin_SCRIPTS = src/pmwatch-stop

bin_PROGRAMS = pmwatch
pmwatch_SOURCES = src/pmwatch.c
pmwatch_LDFLAGS = -static
pmwatch_CFLAGS = ${NEW_CFLAGS} $(AM_CFLAGS) $(BUILD_WITH_LIBIPMCTL_CFLAGS)
if BUILD_ESXI
pmwatch_LDADD = $(lib_LTLIBRARIES)
else
pmwatch_LDADD = $(lib_LTLIBRARIES) $(UUID_LIBS) $(KMOD_LIBS) $(UDEV_LIBS)
endif
endif