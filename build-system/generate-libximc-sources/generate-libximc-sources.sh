#!/bin/sh
set -e
[ -z "$BASEDIR" ] && export BASEDIR="$(pwd)"
[ -z "$DEPSDIR" ] && export DEPSDIR="$BASEDIR/deps"
[ -z "$DISTDIR" ] && export DISTDIR="$BASEDIR/dist"
export LIBXIMC_SRC_DIR="$BASEDIR/libximc/src"
export LIBXIMC_INC_DIR="$BASEDIR/libximc/include"
export LIBXIMC_JAVA_DIR="$BASEDIR/wrappers/java/src"
export XIMC_VERSION_FILE="$BASEDIR/version"

if [ "$1" = "clean" ] ; then
    echo '****************************************************'
    echo "*         CLEAN GENERATED LIBXIMC's SOURCES        *"
    echo '****************************************************'
    ./build-system/generate-libximc-sources/generate-libximc-sources/generate-libximc-sources.sh clean
    echo '****************************************************'
    echo '*                       DONE                       *'
    echo '****************************************************'
    exit 0
fi

case $1 in
    "")
        echo '****************************************************'
        echo '*                                                  *'
        echo "*           GENERATING LIBXIMC's SOURCES           *"
        echo '*                                                  *'
        echo '****************************************************'
        ./build-system/generate-libximc-sources/generate-libximc-sources/generate-libximc-sources.sh
        echo '****************************************************'
        echo "*    GENERATING LIBXIMC's SOURCES IS COMPLETED     *"
        echo '****************************************************'
        ;;
    *)
        echo "Generates libximc's sources.\n"
        ;;
esac
