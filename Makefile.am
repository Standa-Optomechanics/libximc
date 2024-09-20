ACLOCAL_AMFLAGS = -I m4 -I config
AM_DISTCHECK_CONFIGURE_FLAGS = --with-docs

SUBDIRS = xigen libximc wrappers

EXTRA_DIST = aminclude.am version debian rpm


if HAVE_XCODE_BUILD
all-local: build-xcode

build-xcode:
	$(MAKE) -C libximc $(AM_MAKEFLAGS) $@ 

.PHONY: build-xcode
endif

