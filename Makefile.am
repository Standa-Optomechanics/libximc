ACLOCAL_AMFLAGS = -I m4 -I config
AM_DISTCHECK_CONFIGURE_FLAGS = --with-docs

SUBDIRS = libximc wrappers

EXTRA_DIST = version debian rpm


if HAVE_XCODE_BUILD
all-local: build-xcode

build-xcode:
	$(MAKE) -C libximc $(AM_MAKEFLAGS) $@ 

.PHONY: build-xcode
endif

