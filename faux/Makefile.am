lib_LTLIBRARIES += libfaux.la
libfaux_la_SOURCES =
libfaux_la_LIBADD = $(PTHREAD_LIBS)
libfaux_la_CFLAGS = $(PTHREAD_CFLAGS)
libfaux_la_LDFLAGS = $(AM_LDFLAGS) $(VERSION_INFO)

if HAVE_LD_VERSION_SCRIPT
libfaux_la_LDFLAGS += -Wl,--version-script=faux/faux.map
endif

if DEBUG
libfaux_la_CFLAGS += -DDEBUG
endif

if TESTC
libfaux_la_CFLAGS += -DTESTC
endif

nobase_include_HEADERS += \
	faux/faux.h \
	faux/ctype.h \
	faux/str.h \
	faux/sysdb.h \
	faux/conv.h \
	faux/log.h \
	faux/list.h \
	faux/vec.h \
	faux/ini.h \
	faux/file.h \
	faux/argv.h \
	faux/time.h \
	faux/sched.h \
	faux/net.h \
	faux/msg.h \
	faux/eloop.h \
	faux/async.h \
	faux/error.h \
	faux/testc_helpers.h \
	faux/buf.h

EXTRA_DIST += \
	faux/faux.map \
	faux/base/Makefile.am \
	faux/ctype/Makefile.am \
	faux/str/Makefile.am \
	faux/sysdb/Makefile.am \
	faux/conv/Makefile.am \
	faux/log/Makefile.am \
	faux/list/Makefile.am \
	faux/vec/Makefile.am \
	faux/ini/Makefile.am \
	faux/file/Makefile.am \
	faux/argv/Makefile.am \
	faux/time/Makefile.am \
	faux/sched/Makefile.am \
	faux/net/Makefile.am \
	faux/msg/Makefile.am \
	faux/eloop/Makefile.am \
	faux/async/Makefile.am \
	faux/error/Makefile.am \
	faux/testc_helpers/Makefile.am
	faux/buf/Makefile.am

include $(top_srcdir)/faux/base/Makefile.am
include $(top_srcdir)/faux/ctype/Makefile.am
include $(top_srcdir)/faux/str/Makefile.am
include $(top_srcdir)/faux/sysdb/Makefile.am
include $(top_srcdir)/faux/conv/Makefile.am
include $(top_srcdir)/faux/log/Makefile.am
include $(top_srcdir)/faux/list/Makefile.am
include $(top_srcdir)/faux/vec/Makefile.am
include $(top_srcdir)/faux/ini/Makefile.am
include $(top_srcdir)/faux/file/Makefile.am
include $(top_srcdir)/faux/argv/Makefile.am
include $(top_srcdir)/faux/time/Makefile.am
include $(top_srcdir)/faux/sched/Makefile.am
include $(top_srcdir)/faux/net/Makefile.am
include $(top_srcdir)/faux/msg/Makefile.am
include $(top_srcdir)/faux/eloop/Makefile.am
include $(top_srcdir)/faux/async/Makefile.am
include $(top_srcdir)/faux/error/Makefile.am
include $(top_srcdir)/faux/testc_helpers/Makefile.am
include $(top_srcdir)/faux/buf/Makefile.am

if TESTC
include $(top_srcdir)/faux/testc_module/Makefile.am
endif
