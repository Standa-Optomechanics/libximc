if HAVE_XIWRAPPER
XIWRAPPER_LINK_EXPR = -lxiwrapper
endif
libximc_la_CPPFLAGS = -I$(top_builddir)/libximc/include -DLIBXIMC_EXPORTS -DDATADIR='"$(datadir)"'
libximc_la_CFLAGS = -fvisibility=hidden 
libximc_la_LDFLAGS = $(XIWRAPPER_LINK_EXPR) -version-info 7:0:0 $(extra_ldflags_iokit) -lminiupnpc

# the library name to build
lib_LTLIBRARIES = libximc.la

if HAVE_STATICDEPS
DEP_EXT = $(STATIC_EXT)
else
DEP_EXT = $(SHARED_EXT)
endif

install-exec-local:
	mkdir -p "$(DESTDIR)$(libdir)"
	cp -Ra $(XIWRAPPER_PATH)/libbindy$(DEP_EXT) "$(DESTDIR)$(libdir)"
	cp -Ra $(XIWRAPPER_PATH)/libxiwrapper$(DEP_EXT) "$(DESTDIR)$(libdir)"

uninstall-local:
	rm -f "$(DESTDIR)$(libdir)"/libbindy$(DEP_EXT)
	rm -f "$(DESTDIR)$(libdir)"/libxiwrapper$(DEP_EXT)

# the sources to add to the library and to add to the distribution
libximc_la_SOURCES = \
                        adapters.c \
                        adapters.h \
						common.h \
						devenum.c \
                        devenum.h \
						devvirt.c \
						loader.c \
						loader.h \
						metadata.h \
						platform-posix.c \
						udp-posix.c \
						tcp-posix.c \
						platform.h \
						protosup.c \
						protosup.h \
						util.c \
						util.h \
						types.h \
						sglib.h

nodist_libximc_la_SOURCES = ximc-gen.c ximc-gen.h fwprotocol.c fwprotocol.h

include_HEADERS = $(top_builddir)/libximc/include/ximc.h

BUILT_SOURCES = ximc-gen.c ximc-gen.h fwprotocol.c fwprotocol.h $(top_builddir)/libximc/include/ximc.h

EXTRA_DIST = protocol.xi ximc-gen-template.h ximc-gen-template.c ximc-template.h $(BUILT_SOURCES)

if HAVE_CHECK
# Unit test support
TESTS = check_ximc_ut
check_PROGRAMS = check_ximc_ut
check_ximc_ut_SOURCES = ximc-ut.c ${libximc_la_SOURCES} ximc-gen.c ximc-gen.h fwprotocol.c fwprotocol.h
check_ximc_ut_CFLAGS = @CHECK_CFLAGS@
check_ximc_ut_CPPFLAGS = ${libximc_la_CPPFLAGS}
check_ximc_ut_LDFLAGS = -lxiwrapper -lminiupnpc $(extra_ldflags_iokit)
check_ximc_ut_LDADD = @CHECK_LIBS@

TESTS_ENVIRONMENT = LD_LIBRARY_PATH=${XIWRAPPER_PATH}
endif

