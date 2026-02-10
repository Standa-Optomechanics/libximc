gendir = ${top_builddir}/wrappers/java/gen

dist_libjximc_la_SOURCES = ru_ximc_libximc_JXimc.c

libjximc_la_CPPFLAGS = $(CPPJNIFLAGS) -I$(gendir) -I${top_builddir}/libximc/include

lib_LTLIBRARIES = libjximc.la

libjximc_la_LIBADD = $(top_builddir)/libximc/src/libximc.la
libjximc_la_LDFLAGS = -version-info 7:0:0

framework-build:
	$(CC) -dynamiclib $(top_builddir)/wrappers/java/src/c/.libs/libjximc_la-ru_ximc_libximc_JXimc.o \
	-F $(DISTDIR)/ximc/macosx $(extra_ldflags_iokit) -framework libximc -rpath '@loader_path/' \
	-install_name libjximc.dylib -o $(DISTDIR)/ximc/macosx/libjximc.dylib

EXTRA_DIST = ru_ximc_libximc_JXimc-template.c ru_ximc_libximc_JXimc-gen.c
