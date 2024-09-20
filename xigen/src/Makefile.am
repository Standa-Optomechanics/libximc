AM_YFLAGS = -d -v -ra

CXX = @CXX_FOR_BUILD@
CXXCPP = @CXXCPP_FOR_BUILD@
CXX_FLAGS = @CXXFLAGS_FOR_BUILD@
LD_FLAGS = @LDFLAGS_FOR_BUILD@

xigen_GENERATED_YACC = parser.cc parser.hh
xigen_GENERATED_LEX = scanner.cc 
xigen_GENERATED_YACC_HH = location.hh position.hh stack.hh

xigen_GENERATED = $(xigen_GENERATED_YACC) $(xigen_GENERATED_LEX) $(xigen_GENERATED_YACC_HH)

STAMPFILE = $(top_builddir)/xigen/src/xigen.stamp

nodist_xigen_SOURCES = $(xigen_GENERATED)

xigen_SOURCES = driver.cc generator.cc locale.cc xigen.cc \
	basegenerator.hh modegenerator.hh common.hh csharpgenerator.hh pythongenerator.hh defgenerator.hh driver.hh fwheadergenerator.hh fwlibgenerator.hh \
	javagenerator.hh jnigenerator.hh \
	generator.hh generatorhelper.hh headergenerator.hh libgenerator.hh locale.hh model.hh parsercontext.hh \
	pascalgenerator.hh postprocess.hh qsdefinegenerator.hh qtscriptgenerator_fromscript.hh qtscriptgenerator_getsetfunc.hh \
	qtscriptgenerator_registermt.hh qtscriptgenerator_toscript.hh scanner.hh sizecheck.hh visitor.hh wikigenerator.hh \
	qtscriptgenerator_comparison.hh qtscriptgenerator_getsethead.hh qtscriptgenerator_highlights.hh qtscriptgenerator_toscript_calb.hh qtscriptgenerator_fromscript_calb.hh

$(xigen_GENERATED_YACC_HH):

parser.cc: parser.yy driver.hh model.hh $(xigen_GENERATED_YACC_HH)
	$(YACC) $(YFLAGS) $(AM_YFLAGS) -S lalr1.cc -o $@ $<

scanner.cc: scanner.ll scanner.hh
	$(LEX) $(LFLAGS) $(AM_LFLAGS) -i -+ -o $@ $<

$(STAMPFILE): xigen
	echo > $@

all-local: $(STAMPFILE)

# a little hack. timestamp does not exist before actual make run
dist-hook:
	touch $(STAMPFILE)

noinst_PROGRAMS = xigen

EXTRA_DIST = parser.yy scanner.ll fwprotocol-template.c fwprotocol-template.h

BUILT_SOURCES = $(xigen_GENERATED)

CLEANFILES = *.output $(xigen_GENERATED) $(STAMPFILE)

