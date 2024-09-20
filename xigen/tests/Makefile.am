TESTS = \
	example1.test example2.test example3.test example4.test \
	full.test large.test \
	code1x.test code2x.test code3.test code5x.test \
	def1x.test wiki.test wikiimg.test \
	header1.test header2.test header3.test header4.test header5.test header6.test \
	header5cs.test header5pas.test header5py.test jni1.test jni2.test \
	doc1.test \
	fwheader4.test

TESTS_EXPECTED = \
	code1x.expected code2x.expected code3.expected code5x.expected \
	def1x.expected wiki.expected wikiimg.expected \
	header1.expected header2.expected header3.expected header4.expected header5.expected header6.expected \
	header5cs.expected header5pas.expected header5py.expected jni1.expected jni2.expected \
	doc1.expected \
	fwheader4.expected

TESTS_XI = \
	example1.xi example2.xi example3.xi example4.xi \
	full.xi large.xi \
	code3.xi jni2.xi \
	wiki.xi wikiimg.xi \
	header1.xi header2.xi header3.xi header4.xi header5.xi header6.xi \
	doc1.xi

TESTS_ENVIRONMENT = XIGEN=${top_builddir}/xigen/src/xigen

EXTRA_DIST = $(TESTS) $(TESTS_XI) $(TESTS_EXPECTED) defs

CLEANFILES = *.out *.err *.diff *.ok
