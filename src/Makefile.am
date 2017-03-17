AUTOMAKE_OPTIONS = color-tests parallel-tests

pkgconfiglibdir=$(libdir)/pkgconfig

CLEANFILES =
EXTRA_DIST =

lib_LTLIBRARIES =
noinst_LTLIBRARIES =
noinst_DATA =
pkgconfiglib_DATA =

check_PROGRAMS =
check_DATA =
tests=
noinst_PROGRAMS = $(tests)
check_PROGRAMS += $(tests)
TESTS = $(tests)

DEFAULT_CFLAGS = \
	$(OUR_CFLAGS)

DEFAULT_LDFLAGS = \
	$(OUR_LDFLAGS)

AM_CPPFLAGS = \
	-include $(top_builddir)/config.h \
	$(DEFAULT_CFLAGS)

AM_CFLAGS = $(DEFAULT_CFLAGS)
AM_LDFLAGS = $(DEFAULT_LDFLAGS)

INSTALL_EXEC_HOOKS =
UNINSTALL_EXEC_HOOKS =

# ------------------------------------------------------------------------------
pkgconfiglib_DATA += \
	libsystem/libsystem.pc

EXTRA_DIST += \
	libsystem/libsystem.pc.in

CLEANFILES += \
	libsystem/libsystem.pc

libsystem_pkgincludedir=$(includedir)/libsystem
libsystem_pkginclude_HEADERS =

libsystem_pkginclude_HEADERS += \
	libsystem/config-parser.h \
	libsystem/dbus-util.h \
	libsystem/glib-util.h \
	libsystem/libsystem.h \
	libsystem/proc.h

lib_LTLIBRARIES += \
	libsystem.la

libsystem_la_SOURCES = \
	libsystem/config-parser.c \
	libsystem/config-parser.h \
	libsystem/dbus-util.h\
	libsystem/exec.c \
	libsystem/glib-util.c \
	libsystem/glib-util.h \
	libsystem/libsystem.c \
	libsystem/libsystem.h \
	libsystem/proc.c \
	libsystem/proc-meminfo-lookup.c \
	libsystem/proc-smaps-lookup.c \
	libsystem/time-util.c

EXTRA_DIST += \
	libsystem/proc-meminfo-lookup.gperf \
	libsystem/proc-smaps-lookup.gperf

CLEANFILES += \
	libsystem/proc-meminfo-lookup.c \
	libsystem/proc-smaps-lookup.c

libsystem_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS)

libsystem_la_LIBADD = \
	-lrt

# ------------------------------------------------------------------------------
test_truncate_nl_SOURCES = \
	test/test-truncate_nl.c

test_truncate_nl_LDADD = \
	libsystem.la

tests += test-truncate_nl

# ------------------------------------------------------------------------------
test_exec_SOURCES = \
	test/test-exec.c

test_exec_LDADD = \
	libsystem.la

tests += test-exec

# ------------------------------------------------------------------------------
test_cp_SOURCES = \
	test/test-cp.c

test_cp_LDADD = \
	libsystem.la

tests += test-cp

# ------------------------------------------------------------------------------
test_read_write_SOURCES = \
	test/test-read-write.c

test_read_write_LDADD = \
	libsystem.la

tests += test-read-write

# ------------------------------------------------------------------------------
test_foreach_glist_SOURCES = \
	test/test-foreach-glist.c

test_foreach_glist_CFLAGS = \
	$(GIO_CFLAGS)

test_foreach_glist_LDADD = \
	$(GIO_LIBS) \
	libsystem.la

tests += test-foreach-glist

# ------------------------------------------------------------------------------
test_proc_smaps_SOURCES = \
	test/test-proc-smaps.c

test_proc_smaps_LDADD = \
	libsystem.la

tests += test-proc-smaps

# ------------------------------------------------------------------------------
pkgconfiglib_DATA += \
	libsystem-sd/libsystem-sd.pc

EXTRA_DIST += \
	libsystem-sd/libsystem-sd.pc.in

CLEANFILES += \
	libsystem-sd/libsystem-sd.pc

libsystem_sd_pkgincludedir=$(includedir)/libsystem-sd
libsystem_sd_pkginclude_HEADERS =

libsystem_sd_pkginclude_HEADERS += \
	libsystem-sd/systemd.h

lib_LTLIBRARIES += \
	libsystem-sd.la

libsystem_sd_la_SOURCES = \
	libsystem-sd/systemd.c \
	libsystem-sd/systemd.h

libsystem_sd_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(GIO_CFLAGS)

libsystem_sd_la_LIBADD = \
	-lrt \
	$(DBUS_LIBS) \
	$(GIO_LIBS) \
	libsystem.la

# ------------------------------------------------------------------------------
substitutions = \
       '|PACKAGE_VERSION=$(PACKAGE_VERSION)|' \
       '|PACKAGE_NAME=$(PACKAGE_NAME)|' \
       '|PACKAGE_URL=$(PACKAGE_URL)|' \
       '|LIBSYSTEM_PC_REQUIRES=$(LIBSYSTEM_PC_REQUIRES)|' \
       '|LIBSYSTEM_PC_CFLAGS=$(LIBSYSTEM_PC_CFLAGS)|' \
       '|LIBSYSTEM_PC_LIBS=$(LIBSYSTEM_PC_LIBS)|' \
       '|LIBSYSTEM_SD_PC_REQUIRES=$(LIBSYSTEM_SD_PC_REQUIRES)|' \
       '|LIBSYSTEM_SD_PC_CFLAGS=$(LIBSYSTEM_SD_PC_CFLAGS)|' \
       '|LIBSYSTEM_SD_PC_LIBS=$(LIBSYSTEM_SD_PC_LIBS)|' \
       '|includedir=$(includedir)|' \
       '|VERSION=$(VERSION)|'

SED_PROCESS = \
	$(AM_V_GEN)$(MKDIR_P) $(dir $@) && \
	$(SED) $(subst '|,-e 's|@,$(subst =,\@|,$(subst |',|g',$(substitutions)))) \
		< $< > $@

%.pc: %.pc.in
	$(SED_PROCESS)

%.c: %.gperf
	$(AM_V_at)$(MKDIR_P) $(dir $@)
	$(AM_V_GPERF)$(GPERF) < $< > $@

install-exec-hook: $(INSTALL_EXEC_HOOKS)
