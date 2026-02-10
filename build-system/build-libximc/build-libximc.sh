#!/bin/sh
set -e
[ -z "$BASEDIR" ] && export BASEDIR="$(pwd)"
[ -z "$DEPSDIR" ] && export DEPSDIR="$BASEDIR/deps"
[ -z "$DISTDIR" ] && export DISTDIR="$BASEDIR/dist"
export LIBXIMC_SRC_DIR="$BASEDIR/libximc/src"
export LIBXIMC_INC_DIR="$BASEDIR/libximc/include"
export LOCAL="$BASEDIR/dist/local"

if [ "$1" = "clean" ] ; then
    echo '****************************************************'
    echo '*                  CLEAN LIBXIMC                   *'
    echo '****************************************************'
    ./build-system/build-libximc/build-libximc/build-libximc.sh clean
    echo '****************************************************'
    echo '*                       DONE                       *'
    echo '****************************************************'
    exit 0
fi

case $1 in
    "")
        echo '****************************************************'
        echo '*                                                  *'
        echo '*                 BUILDING LIBXIMC                 *'
        echo '*                                                  *'
        echo '****************************************************'

        ./build-system/build-libximc/build-libximc/build-libximc.sh

        echo '****************************************************'
        echo '*           BUILDING LIBXIMC IS COMPLETED          *'
        echo '****************************************************'
        ;;
    "--help")
        echo "Builds libximc.\n"
        ;;
    *)
        echo "Unknown option \"$1\"."
        ;;
esac
