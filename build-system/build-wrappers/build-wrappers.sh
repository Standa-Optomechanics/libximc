#!/bin/sh
set -e
[ -z "$BASEDIR" ] && export BASEDIR="$(pwd)"
[ -z "$DEPSDIR" ] && export DEPSDIR="$BASEDIR/deps"
[ -z "$DISTDIR" ] && export DISTDIR="$BASEDIR/dist"
[ -z "$MAKE" ] && export MAKE=make

if [ "$1" = "clean" ] ; then
    echo '****************************************************'
    echo '*                  CLEAN WRAPPERS                  *'
    echo '****************************************************'
    ./build-system/build-wrappers/build-wrappers/build-java-wrapper.sh  clean
    ./build-system/build-wrappers/build-wrappers/pack-python-wrapper.sh clean
    rm -rf "$DEPSDIR"
    echo '****************************************************'
    echo '*                       DONE                       *'
    echo '****************************************************'
    exit 0
fi

echo '****************************************************'
echo '*                                                  *'
echo '*                BUILDING WRAPPERS                 *'
echo '*                                                  *'
echo '****************************************************'

STAGE_NAME=$0
ACTION=$1
[ $# -ge 1 ] && shift

case $ACTION in
    "java")
        ./build-system/build-wrappers/build-wrappers/build-java-wrapper.sh $* ;;
    "python")
        ./build-system/build-wrappers/build-wrappers/pack-python-wrapper.sh $* ;;
    "")
        ./build-system/build-wrappers/build-wrappers/build-java-wrapper.sh $*
        ./build-system/build-wrappers/build-wrappers/pack-python-wrapper.sh $*
        ;;
    "--help")
        echo "Usage: $SCRIPT_NAME $STAGE_NAME [ python | java ]"
        echo "To build all wrappers just use $SCRIPT_NAME $STAGE_NAME"
        echo ""
        echo "OPTIONS:"
        echo "  java      - build java wrapper"
        echo "  python    - build python wrapper"
        echo "  clean     - remove all wrappers and related files"
        echo ""
        ;;
    *)
        echo "\nUnlnown option $ACTION\n"echo "Usage: $SCRIPT_NAME $STAGE_NAME [ python | java ]"
        echo "To build all wrappers just use $SCRIPT_NAME $STAGE_NAME"
        echo ""
        echo "OPTIONS:"
        echo "  java      - build java wrapper"
        echo "  python    - build python wrapper"
        echo "  clean     - remove all wrappers and related files"
        echo ""
        exit 1
    ;;
esac
