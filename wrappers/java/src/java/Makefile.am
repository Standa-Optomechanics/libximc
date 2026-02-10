jarfile = libjximc.jar
jarpath = $(top_builddir)/wrappers/java/$(jarfile)
# why?
jardir = $(datadir)/java
gendir = ${top_builddir}/wrappers/java/gen
javadocdir = ${top_builddir}/wrappers/java/doc-java

JAVAROOT = $(top_builddir)/wrappers/java

STAMPFILE = $(top_builddir)/wrappers/java/java-headers.stamp
JAVADOCSTAMPFILE = $(top_builddir)/wrappers/java/javadoc.stamp

# jar file

$(jarpath):
	$(JAR) cf $(JARFLAGS) $@ -C $(JAVAROOT) ru

jar_DATA = $(jarpath)

# Java sources and generated classes

STATIC_SOURCES = \
		 $(srcdir)/ru/ximc/libximc/XimcError.java \
		 $(srcdir)/ru/ximc/libximc/XimcNoDevice.java \
		 $(srcdir)/ru/ximc/libximc/XimcNotImplemented.java \
		 $(srcdir)/ru/ximc/libximc/XimcValueError.java \
		 $(srcdir)/ru/ximc/libximc/JXimc.java

$(STAMPFILE): $(jarpath)
	$(JAVAH) -classpath $(jarpath) -jni -d $(gendir) $(JAVAHFLAGS) ru.ximc.libximc.JXimc
	@echo "JNI headers generated" > $@

CLEANFILES = $(jarpath) $(JAVAROOT)/ru/ximc/libximc/*.class \
						 $(STAMPFILE) $(JAVADOCSTAMPFILE) $(srcdir)/classnoinst.stamp \
						 $(gendir)/*.h

noinst_JAVA = $(STATIC_SOURCES)

EXTRA_DIST = $(STATIC_SOURCES)

if HAVE_DOCS
# Generate documentation even if javadocstampfile is missing
# It allows reconfiguration to `--with-docs` back and forth
all-local: $(STAMPFILE)
	$(JAVADOC) -d $(javadocdir)/html ru.ximc.libximc
	@echo Generated > $(JAVADOCSTAMPFILE)

else

all-local: $(STAMPFILE)

endif

# javadoc stuff
if HAVE_DOCS

clean-local:
	-rm -rf $(javadocdir)

install-data-local: $(JAVADOCSTAMPFILE)
	mkdir -p "$(DESTDIR)$(docdir)/doc-java"
	(unset CDPATH; cd $(javadocdir) && tar cpf - html) \
		| (cd "$(DESTDIR)$(docdir)/doc-java" && tar xpf -)

uninstall-local:
	rm -rf "$(DESTDIR)$(docdir)/doc-java"

else

$(JAVADOCSTAMPFILE): $(STAMPFILE)
	@echo > $@
endif
