# the subdirectories of the project to go into
SUBDIRS = src .

EXTRA_DIST = libximc.xcodeproj Info.plist 

if HAVE_DOCS
SUBDIRS += docs
endif

if HAVE_XCODE_BUILD
all-local: build-xcode

build-xcode:
	xcodebuild GCC_VERSION=$(XCODE_COMPILER)

install-exec-local:
	rm -rf "$(DESTDIR)$(libdir)/libximc.framework"
	cp -Ra build/Release/libximc.framework "$(DESTDIR)$(libdir)/libximc.framework"

uninstall-local:
	rm -rf "$(DESTDIR)$(libdir)/libximc.framework"

clean-local:
	-rm -rf build DerivedData *.xcodeproj/xcuserdata *.xcodeproj/project.xcworkspace *.xcodeproj/*.pbxuser *.xcodeproj/*.mode1v3 

.PHONY: build-xcode
endif


