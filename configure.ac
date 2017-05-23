#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([libsystem], [4.1], [walyong.cho@samsung.com])

dnl AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AC_USE_SYSTEM_EXTENSIONS
AC_SYS_LARGEFILE
AC_PREFIX_DEFAULT([/usr])
AM_INIT_AUTOMAKE([foreign subdir-objects])

LT_PREREQ(2.2)
LT_INIT([disable-static])

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h inttypes.h limits.h mntent.h stddef.h stdint.h stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_GETMNTENT
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_CHECK_FUNCS([dup2 getmntent gettimeofday localtime_r memset mkdir rmdir strchr strcspn strdup strndup strrchr strspn])

AC_CHECK_TOOL(GPERF, gperf)
if test -z "$GPERF" ; then
        AC_MSG_ERROR([*** gperf not found])
fi

# ------------------------------------------------------------------------------
our_cflags=" \
        -g -O2 \
        -Werror \
        -fpie"

our_ldflags=" \
        -Wl,--as-needed \
        -Wl,--no-undefined \
        -Wl,--gc-sections \
        -Wl,-z,relro \
        -Wl,-z,now \
        -pie"

our_cflags=$(echo $our_cflags |sed 's/\t/ /g' | sed 's/  / /g')
our_ldflags=$(echo $our_ldflags | sed 's/\t/ /g' | sed 's/  / /g')

AC_SUBST([OUR_CFLAGS], "$our_cflags")
AC_SUBST([OUR_LDFLAGS], "$our_ldflags")

# ------------------------------------------------------------------------------
PKG_CHECK_MODULES(DBUS, [dbus-1 >= 1.3.2])
PKG_CHECK_MODULES(GLIB, [glib-2.0])
PKG_CHECK_MODULES(GIO, [gio-2.0])

# ------------------------------------------------------------------------------
AC_SUBST([LIBSYSTEM_PC_REQUIRES], "")
AC_SUBST([LIBSYSTEM_PC_CFLAGS], "-D_GNU_SOURCE")
AC_SUBST([LIBSYSTEM_PC_LIBS], "-L${libdir} -lsystem")

AC_SUBST([LIBSYSTEM_PC_REQUIRES], "${LIBSYSTEM_PC_REQUIRES} ${GLIB_REQUIRES}")
AC_SUBST([LIBSYSTEM_PC_CFLAGS], "${LIBSYSTEM_PC_CFLAGS} ${GLIB_CFLAGS}")
AC_SUBST([LIBSYSTEM_PC_LIBS], "${LIBSYSTEM_PC_LIBS} ${GLIB_LIBS}")

# ------------------------------------------------------------------------------
AC_SUBST([LIBSYSTEM_SD_PC_REQUIRES], "libsystem")
AC_SUBST([LIBSYSTEM_SD_PC_CFLAGS], "")
AC_SUBST([LIBSYSTEM_SD_PC_LIBS], "-L${libdir} -lsystem-sd")

AC_SUBST([LIBSYSTEM_SD_PC_REQUIRES], "${LIBSYSTEM_SD_PC_REQUIRES} ${DBUS_REQUIRES}")
AC_SUBST([LIBSYSTEM_SD_PC_CFLAGS], "${LIBSYSTEM_SD_PC_CFLAGS} ${DBUS_CFLAGS}")
AC_SUBST([LIBSYSTEM_SD_PC_LIBS], "${LIBSYSTEM_SD_PC_LIBS} ${DBUS_LIBS}")

AC_SUBST([LIBSYSTEM_SD_PC_REQUIRES], "${LIBSYSTEM_SD_PC_REQUIRES} ${GIO_REQUIRES}")
AC_SUBST([LIBSYSTEM_SD_PC_CFLAGS], "${LIBSYSTEM_SD_PC_CFLAGS} ${GIO_CFLAGS}")
AC_SUBST([LIBSYSTEM_SD_PC_LIBS], "${LIBSYSTEM_SD_PC_LIBS} ${GIO_LIBS}")

# ------------------------------------------------------------------------------
AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
AC_MSG_RESULT([
        $PACKAGE_NAME $VERSION

        prefix:                  ${prefix}
        rootprefix:              ${with_rootprefix}
        datarootdir:             ${datarootdir}
        lib dir:                 ${libdir}
        rootlib dir:             ${with_rootlibdir}

        OUR CFLAGS:              ${OUR_CFLAGS}
        CFLAGS:                  ${CFLAGS}
        OUR LDFLAGS:             ${OUR_LDFLAGS}
        LDFLAGS:                 ${LDFLAGS}
])
