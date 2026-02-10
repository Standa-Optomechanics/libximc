#!/bin/sh
set -e

BASEDIR=$(pwd)
DISTDIR="$BASEDIR/dist"
DEPSDIR="$BASEDIR/deps"

# TODO: remove that dirty kludge
VERSION_FILE="$BASEDIR/version"
[ ! -f "$VERSION_FILE" ] && echo No version file && exit 1
export VER=$(sed 'q' "$VERSION_FILE")
SOVER=`sed '2q;d' "$VERSION_FILE"`
SOVERMAJOR=$(echo $SOVER | sed 's/\..*//')
if [ -z "$SOVERMAJOR" ] ; then
	echo Version error. Check version file - first line must be x.x.x
	exit 1
fi

export VNAME=libximc$SOVERMAJOR-$VER

set_environment()
{
	echo "Setting environment variables for $(uname -s)"
	# Variables that should be exported to sub-shells
	export SPECIAL_ENV=
	export DEPENDENCIES_CMAKE_OPTIONS=
	export PACKAGE_EXTRA_CONFIGURE="--with-xiwrapper=$DEPSDIR/xiwrapper --with-miniupnpc=$DEPSDIR/miniupnpc"

	# Variables that should to be local
	CONFIGURE_FLAGS=
	if [ -z "$USE_CFLAGS" ] ; then
		USE_CFLAGS=
		echo Using default external CFLAGS
	fi
	if [ -z "$USE_CXXFLAGS" ] ; then
		export USE_CXXFLAGS=
		echo Using default external CXXFLAGS
	fi

	case "$(uname -s)" in
		Darwin)
			# Variables that should be exported to sub-shells
			export OS_NAME=Darwin
			export MAKE=make
			export DEPENDENCIES_CMAKE_OPTIONS="-DCMAKE_CXX_COMPILER=clang++"
			export SPECIAL_ENV="CC=clang CXX=clang++ CC_FOR_BUILD=clang CXX_FOR_BUILD=clang++"
			if [ -z "$JAVA_HOME" ] ; then
				export SPECIAL_ENV="$SPECIAL_ENV JAVA_HOME=$(/usr/libexec/java_home -v 1.7)"
			fi
			export LIBSDIR="$DISTDIR/ximc/macosx"

			USE_XCODE=--with-xcode-build
			# it is about deprecated sem_getvalue
			USE_CFLAGS="-Wno-deprecated-declarations -Wall -Werror -Wextra -Wshadow -Wno-switch"
			USE_CXXFLAGS="-Wno-tautological-compare"
			;;
		Linux)
			export OS_NAME=Linux
			export MAKE=make
			case "$(uname -m)" in
				"x86_64")
					export LIBSDIR="$DISTDIR/ximc/debian-amd64" ;;
				"i"*"86")
					export LIBSDIR="$DISTDIR/ximc/debian-i386" ;;
				"arm"*)
					# TODO: fix armhf creating for every arm* arch.
					export LIBSDIR="$DISTDIR/ximc/debian-armhf" ;;
				*)
					echo "ERROR: Linxu $(uname -m) is not supported."
					exit 1
			esac
			LSB_RELEASE=$(lsb_release -i -s)
			if [ "$LSB_RELEASE" = "SUSE LINUX" ] || [ "$LSB_RELEASE" = "openSUSE project" ] ||
				  [ "$LSB_RELEASE" = "ScientificCERNSLC" ] ; then
				export PACKAGE_EXTRA_CONFIGURE="$PACKAGE_EXTRA_CONFIGURE --disable-static"
			fi

			DISTNAME=
			if [ -f "/etc/debian_version" ] ; then
				DISTNAME=deb
			fi
			# readdir_r is now deprecated. ignore
			USE_CFLAGS="-Wno-deprecated-declarations -Wall -Werror -Wextra -Wshadow -Wno-switch"
			;;
		*)
			echo "Wrong distribution"
			exit 1
			;;
	esac
	export COPYFILE_DISABLE=true  # Prevent copying OS X extended attributes
}

configure_build()
{
	if [ -d "$VNAME" ] ; then
		chmod -R 777 "$VNAME"
		rm -rf $VNAME
	fi

	DISTLATEST="$DISTDIR/$DISTNAME"
	rm -rf $DISTLATEST
	mkdir -p $DISTLATEST
	mkdir -p m4 config

	echo Using env $SPECIAL_ENV

	# The configuration!
	autoreconf --force --install
	echo Invoke ./configure CFLAGS="$USE_CFLAGS" CXXFLAGS="$USE_CXXFLAGS" \
		CPPFLAGS="$USE_CPPFLAGS" LDFLAGS="$USE_LDFLAGS" \
		$CONFIGURE_FLAGS --with-xiwrapper=$DEPSDIR/xiwrapper --with-miniupnpc=$DEPSDIR/miniupnpc \
		$USE_XCODE --prefix=$DISTDIR/local $*

	env $SPECIAL_ENV ./configure CFLAGS="$USE_CFLAGS" CXXFLAGS="$USE_CXXFLAGS" \
		CPPFLAGS="$USE_CPPFLAGS" LDFLAGS="$USE_LDFLAGS" \
		$CONFIGURE_FLAGS --with-xiwrapper=$DEPSDIR/xiwrapper --with-miniupnpc=$DEPSDIR/miniupnpc \
		$USE_XCODE --prefix=$DISTDIR/local $*

	# --with-doxygen=PATH_TO_DOXYGEN can be also used
}

print_help () {
	# Run me only after $SCRIPT_NAME is set
	echo "Usage: $SCRIPT_NAME [ clean | deps | gen-libximc-sources | libximc | wrappers | examples | docs | debs | --help ]"
	echo ""
	echo "ATTENTION: Before building you must run configure step. Use '$SCRIPT_NAME configure'."
	echo "For full build just use $SCRIPT_NAME."
	echo "Use $SCRIPT_NAME COMMAND --help to see detailed info."
	echo ""
	echo ""
	echo "COMMAND:"
	echo "  configure           - configure build: set appropriate env. variables,"
	echo "                        configure autotools."
	echo "  clean               - full cleaning"
	echo "  deps                - build dependencies"
	echo "  gen-libximc-sources - generate libximc source code"
	echo "  libximc             - build the library"
	echo "  wrappers            - build wrappers"
	echo "  examples            - build examples"
	echo "  docs                - build documentation"
	echo "  debs                - build debian packages"
	echo "  --help              - show this help message"
	echo ""
}

export SCRIPT_NAME=$0
ACTION=$1
[ $# -ge 1 ] && shift

case $ACTION in
	"configure")
		set_environment
		configure_build --with-docs
		;;
	"clean")
		echo '$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$'
		echo '$                     CLEAN ALL                    $'
		echo '$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$'
		git --version 1>/dev/null 2>/dev/null
		if [ ! $? -eq 0 ] ; then 
			echo "WARNING: git cannot be found! Cleaning won't be complete"
		else
			git clean -xdf .
		fi
		./build-system/build-dependencies/build-dependencies.sh             clean
		./build-system/generate-libximc-sources/generate-libximc-sources.sh clean
		./build-system/build-libximc/build-libximc.sh                       clean
		./build-system/build-wrappers/build-wrappers.sh                     clean
		./build-system/build-examples/build-examples.sh                    	clean || true
		echo '$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$'
		echo '$                       DONE                       $'
		echo '$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$'
		;;
	"deps")
		set_environment
		./build-system/build-dependencies/build-dependencies.sh $*	
		;;
	"gen-libximc-sources")
		set_environment
		./build-system/generate-libximc-sources/generate-libximc-sources.sh $*
		;;
	"libximc")
		set_environment
		[ ! -n "$DEBUG" ] && configure_build
		./build-system/build-libximc/build-libximc.sh $*
		;;
	"wrappers")
		set_environment
		[ ! -n "$DEBUG" ] && configure_build --with-docs
		[ ! -n "$DEBUG" ] && ./build-system/build-libximc/build-libximc.sh
		./build-system/build-wrappers/build-wrappers.sh $*
		;;
	"examples")
		set_environment
		./build-system/build-examples/build-examples.sh $* # no need to || true. If it's been called explicitly we cannot omit fails.
		;;
	"docs")
		set_environment
		[ ! -n "$DEBUG" ] && configure_build --with-docs
		[ ! -n "$DEBUG" ] && ./build-system/build-libximc/build-libximc.sh
		./build-system/build-docs/build-docs.sh $*
		;;
	"debs")
		set_environment
		[ ! -n "$DEBUG" ] && ./build-system/build-libximc/build-libximc.sh
		./build-system/build-debs/build-debs.sh $*
		;;
	"")
		set_environment
		configure_build --with-docs  # By default build is without docs, so we need to explicitly set it.
		echo '$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$'
		echo '$                     BUILD ALL                    $'
		echo '$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$'

		./build-system/build-dependencies/build-dependencies.sh
		./build-system/generate-libximc-sources/generate-libximc-sources.sh
		./build-system/build-libximc/build-libximc.sh
		./build-system/build-wrappers/build-wrappers.sh
		./build-system/build-docs/build-docs.sh
		./build-system/build-examples/build-examples.sh || true
		;;
	"--help")
		echo "\nLibximc build script.\n"
		print_help
		;;
	*)
		echo "\nUnknown command $ACTION.\n"
		print_help
		exit 1
esac
# vim: noet
