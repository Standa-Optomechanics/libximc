#!/bin/bash

# Please be extra careful with modifying autotools files. For example, DESTDIR is an extremely important variable and it must not be omitted from builds. Also, debian installation layout is not related to `dist` layout at all.
# Additional packages installed: dh-helper texlive-fonts-extra lintian


set -e
if [ "$1" = "clean" ] ; then
    echo Cleaning deb-packages...
    rm -rf "$DISTDIR/ximc/debs"
    exit 0
fi

echo '----------------------------------------------------'
echo '                                                    '
echo '                    BUILDING DEBS                   '
echo '                                                    '
echo '----------------------------------------------------'

echo '----------------------------------------------------'
echo '              Checking dependencies...              '
echo '----------------------------------------------------'

set -e
set -o pipefail

VERSION_FILE=version
if [ ! -f "$VERSION_FILE" ] ; then
  echo ERROR: No version file
  exit 1
fi
VER=`sed 'q' "$VERSION_FILE"`
SOVER=`sed '2q;d' "$VERSION_FILE"`
SOVERMAJOR=`echo $SOVER | sed 's/\..*//'`
VNAME=libximc$SOVERMAJOR-$VER

DISTNAME=${VNAME}.tar.gz
if [ ! -f $DISTNAME ] ; then
  echo ERROR: dist file $DISTNAME is missing
  exit 1
fi

set -e
echo '------- Dependencies checking has been done --------'

export PACKAGE_EXTRA_CONFIGURE="--with-xiwrapper=$DEPSDIR/xiwrapper --with-miniupnpc=$DEPSDIR/miniupnpc"

PKGROOT=$BASEDIR/pkgroot
rm -rf $PKGROOT
mkdir -p $PKGROOT
tar -C $PKGROOT -xf $DISTNAME
cd $PKGROOT/$VNAME
dpkg-buildpackage -D -tc -us -uc -rfakeroot

echo Checking dist
ls -1 $PKGROOT/*.deb | xargs -t -L1 lintian -v

echo Copying dist...
DEBDISTDIR=$DISTDIR/ximc/deb
rm -rf $DEBDISTDIR
mkdir -p $DEBDISTDIR

cp $PKGROOT/libximc*.deb $DEBDISTDIR/


rm -rf $PKGROOT $BASEROOT/$VNAME.tar.gz
cd $BASEROOT

echo '----------------------------------------------------'
echo '              BUILDING DEPS IS COMPLETED            '
echo '----------------------------------------------------'
echo 'Output:                                             '
echo "* $DISTDIR/ximc/deb                                 "
echo '----------------------------------------------------'
