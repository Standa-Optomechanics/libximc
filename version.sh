#!/bin/sh

set -e

trap " echo Cleaning up && rm -f *bak libximc/*bak libximc/src/*bak rpm/*bak libximc/libximc.xcodeproj/*bak debian/*bak debian/*bak pkg/freebsd/*bak wrappers/java/src/c/*bak wrappers/csharp/Properties/*bak examples/testapp_C/*bak examples/test_Java/*bak " EXIT

VERSION_FILE=`dirname $0`/version
if [ ! -f "$VERSION_FILE" ] ; then
	echo No version file
fi
VER=`sed 'q' "$VERSION_FILE"`
SOVER=`sed '2q;d' "$VERSION_FILE"`

echo Setting version to $VER
echo Setting SO version to $SOVER
echo "Remember, if you want to change interfaces and break ABI, change major version"

if [ -z "`echo $VER | sed 's/^\([0-9]*\)\.\([0-9]*\)\.\([0-9]*\)$//'`" ] ; then
	PRERELEASE=
elif [ -z "`echo $VER | sed 's/^\([0-9]*\)\.\([0-9]*\)\.\([0-9]*\)b\([0-9]*\)$//'`" ] ; then
	PRERELEASE=`echo $VER | sed 's/^\([0-9]*\)\.\([0-9]*\)\.\([0-9]*\)b\([0-9]*\)$/\4/'`
else
	echo Wrong version specified: $VER
	exit 1
fi

if [ -n "$PRERELEASE" ] ; then
	echo Prerelease is b$PRERELEASE
fi
VERMAJOR=`echo $VER | sed 's/^\([0-9]*\)\.\([0-9]*\)\.\([0-9]*\).*/\1/'`
VERMINOR=`echo $VER | sed 's/^\([0-9]*\)\.\([0-9]*\)\.\([0-9]*\).*/\2/'`
VERBUILD=`echo $VER | sed 's/^\([0-9]*\)\.\([0-9]*\)\.\([0-9]*\).*/\3/'`

echo Ver $VERMAJOR.$VERMINOR.$VERBUILD

SOVERCURRENT=`echo $SOVER | sed 's/^\([0-9]*\)\.\([0-9]*\)\.\([0-9]*\)$/\1/'`
SOVERREVISION=`echo $SOVER | sed 's/^\([0-9]*\)\.\([0-9]*\)\.\([0-9]*\)$/\2/'`
SOVERAGE=`echo $SOVER | sed 's/^\([0-9]*\)\.\([0-9]*\)\.\([0-9]*\)$/\3/'`
SOVERPREV=`expr $SOVERCURRENT - 1`
if [ -z "$SOVERCURRENT" ] || [ -z "$SOVERREVISION" ] || [ -z "$SOVERAGE" ] ; then
	 echo Wrong so version specified: $SOVER
	exit 1
fi
echo Previous SO version to $SOVERPREV

if [ -z "$PRERELEASE" ] ; then
	RCVERCOMMA=$VERMAJOR,$VERMINOR,$VERBUILD,100
	RCVERFULL=$VERMAJOR.$VERMINOR.$VERBUILD
	ASSEMBLYVER=$VERMAJOR.$VERMINOR.$VERBUILD.100
	VERSTR=$VERMAJOR.$VERMINOR.$VERBUILD
	VERDEB=$VERMAJOR.$VERMINOR.$VERBUILD
else
	RCVERCOMMA=$VERMAJOR,$VERMINOR,$VERBUILD,$PRERELEASE
	RCVERFULL=$VERMAJOR.$VERMINOR.$VERBUILD"b"$PRERELEASE
	ASSEMBLYVER=$VERMAJOR.$VERMINOR.$VERBUILD.$PRERELEASE
	VERSTR=$VERMAJOR.$VERMINOR.$VERBUILD"b"$PRERELEASE
	VERDEB=$VERMAJOR.$VERMINOR.$VERBUILD"~b"$PRERELEASE
fi

DEBDATE=`LC_ALL=C date "+%a, %d %b %Y %H:%M:%S %z"`
RPMDATE=`LC_ALL=C date "+%a %b %d %Y"`

# now set them all!

echo Autotools files
sed -ibak "s/^AC_INIT(\[libximc.*\], \[[^\]*\])$/AC_INIT(\[libximc$SOVERCURRENT\], \[$VERSTR\])/" configure.ac
# remove old clutter
sed -ibak "s/-release [0-9\.]*//" libximc/src/Makefile.am
# it's critical to have zero as a third number - there is a mess with ages on debian/sonames
sed -ibak "s/-version-info [0-9\.:]*/-version-info $SOVERCURRENT:$SOVERREVISION:$SOVERAGE/" libximc/src/Makefile.am
sed -ibak "s/-version-info [0-9\.:]*/-version-info $SOVERCURRENT:$SOVERREVISION:$SOVERAGE/" wrappers/java/src/c/Makefile.am

echo Debian files
for file in `ls -1 debian/libximc* debian/shlibs.libximc* debian/copyright` ; do
	file=`basename $file`
	newfile=`echo $file | sed -n "s/\(.*\)libximc[0-9]*\(.*\)$/\1libximc$SOVERCURRENT\2/p" | sed -n "s/\(.*\)libximc[0-9]*\(.*\)$/\1libximc$SOVERCURRENT\2/p"`
	if [ -n "$newfile" ] && [ "$newfile" != "$file" ] ; then
		mv -v "debian/$file" "debian/$newfile"
	fi
done
for file in `ls -1 debian/libximc*.install` ; do
	echo Patching $file
	sed -E -ibak "/^usr\/share\/doc/s/libximc[^\/\-\ ]/libximc$SOVERCURRENT/g" $file
done
for file in `ls -1 debian/libximc*.doc-base debian/libximc*.lintian-overrides` ; do
	echo Patching $file
	sed -E -ibak "s/libximc[0-9]+/libximc$SOVERCURRENT/g" $file
done
if [ ! -f "debian/shlibs.libximc$SOVERCURRENT" ] ; then
	echo No shlibs file
	echo FAIL
	exit 1
fi
#sed -ibak "s/ [0-9]* libximc[0-9]*/ $SOVERCURRENT libximc$SOVERCURRENT/" debian/shlibs.libximc$SOVERCURRENT
sed -E -ibak "s/^(Upstream-Name: libximc)[0-9]+/\1$SOVERCURRENT/g" debian/copyright
echo "libximc $SOVERCURRENT libximc$SOVERCURRENT" > debian/shlibs.libximc$SOVERCURRENT
echo "libjximc $SOVERCURRENT libjximc$SOVERCURRENT" >> debian/shlibs.libximc$SOVERCURRENT
sed -ibak -E "s/^(Source|Package|Depends): libximc[0-9]+/\1: libximc$SOVERCURRENT/g;s/^(Breaks|Replaces): libximc[0-9]+/\1: libximc$SOVERPREV/g" debian/control
echo Please carefully check versions inside a debian files if you changed SONAME

# file and product version strings should be 1.2.3.4 or 1.2.3b4 but numeric arrays are only 1.2.3
echo DLL files
sed -ibak "s/FILEVERSION .*/FILEVERSION $RCVERCOMMA/" libximc/src/libximc.rc
sed -ibak "s/PRODUCTVERSION .*/PRODUCTVERSION $RCVERCOMMA/" libximc/src/libximc.rc
sed -ibak "s/VALUE \"FileVersion\", \".*/VALUE \"FileVersion\", \"$RCVERFULL\\\\0\"/" libximc/src/libximc.rc
sed -ibak "s/VALUE \"ProductVersion\", \".*/VALUE \"ProductVersion\", \"$RCVERFULL\\\\0\"/" libximc/src/libximc.rc
if [ -z "$PRERELEASE" ] ; then
	sed -ibak "s/#define LIBXIMC_RELEASE_FLAG .*/#define LIBXIMC_RELEASE_FLAG 0/" libximc/src/libximc.rc
else
	sed -ibak "s/#define LIBXIMC_RELEASE_FLAG .*/#define LIBXIMC_RELEASE_FLAG VER_PRERELEASE/" libximc/src/libximc.rc
fi

echo .NET files
sed -ibak "s/^\[assembly: AssemblyVersion(\"[^\"]*\")\]/\[assembly: AssemblyVersion(\"$ASSEMBLYVER\")\]/" wrappers/csharp/Properties/AssemblyInfo.cs
sed -ibak "s/^\[assembly: AssemblyFileVersion(\"[^\"]*\")\]/\[assembly: AssemblyFileVersion(\"$ASSEMBLYVER\")\]/" wrappers/csharp/Properties/AssemblyInfo.cs

echo Java example files
sed -ibak "s/^SOVER =.*$/SOVER = $SOVERCURRENT/" examples/test_C/testapp_C/Makefile
sed -ibak "s/^SOVER =.*$/SOVER = $SOVERCURRENT/" examples/test_Java/Makefile

echo RPM files
sed -ibak "s/^Name: libximc.*$/Name: libximc$SOVERCURRENT/" rpm/libximc.spec
sed -ibak "s/^Version: .*/Version: $VERMAJOR.$VERMINOR.$VERBUILD/" rpm/libximc.spec
if [ -z "$PRERELEASE" ] ; then
	sed -ibak "s/^Release: .*/Release: 1/" rpm/libximc.spec
else
	sed -ibak "s/^Release: .*/Release: 0.1.b$PRERELEASE/" rpm/libximc.spec
fi

echo XCode file
echo "DYLIB_COMPATIBILITY_VERSION = $SOVERAGE;" > libximc/version.xcconfig
echo "DYLIB_CURRENT_VERSION = $SOVERREVISION;" >> libximc/version.xcconfig
echo "FRAMEWORK_VERSION = $SOVERCURRENT;" >> libximc/version.xcconfig

echo Adding empty changelogs
# TODO fix version strings

file=debian/changelog
awk "NR==1{print \"libximc$SOVERCURRENT ($VERDEB-1) unstable; urgency=low\n\n  * Version bump\n\n -- XIMC <info@ximc.ru>  $DEBDATE\n\"};1" $file > $file.bak && mv $file.bak $file

file=rpm/libximc.spec
awk "/%changelog/{print; print \"\n* $RPMDATE XIMC <info@ximc.ru> - $VERSTR\n- version bump\"; next}1" $file > $file.bak && mv $file.bak $file

file=ChangeLog
awk "NR==1{print \"libximc ($VERDEB-1)\n  * Released: $DEBDATE\n  * Version bump\n\"};1" $file > $file.bak && mv $file.bak $file

mkdir -p dist

echo "Please add changelogs:"
echo "   vim rpm/libximc.spec debian/changelog ChangeLog"

echo Ok
