#!/bin/sh
set -e
[ -z "$BASEDIR" ] && export BASEDIR="$(pwd)"
[ -z "$DEPSDIR" ] && export DEPSDIR="$BASEDIR/deps"
[ -z "$DISTDIR" ] && export DISTDIR="$BASEDIR/dist"
[ -z "$MAKE" ] && export MAKE=make

if [ "$1" = "clean" ] ; then
    echo '****************************************************'
    echo '*                    CLEAN DEBS                    *'
    echo '****************************************************'
    ./build-system/build-debs/build-debs/build-debs.sh clean
    echo '****************************************************'
    echo '*                       DONE                       *'
    echo '****************************************************'
    exit 0
fi

echo '****************************************************'
echo '*                                                  *'
echo '*                  BUILDING DEBS                   *'
echo '*                                                  *'
echo '****************************************************'

ACTION=$1
[ $# -ge 1 ] && shift

case $ACTION in
    "")
        ./build-system/build-debs/build-debs/build-debs.sh $*
        ;;
    "--help")
        echo "Builds deb-packages.\n"
        ;;
    *)
        echo "Unknown option \"$ACTION\"."
        exit 1
    ;;
esac