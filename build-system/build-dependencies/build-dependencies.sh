#!/bin/sh
set -e
[ -z "$BASEDIR" ] && export BASEDIR="$(pwd)"
[ -z "$DEPSDIR" ] && export DEPSDIR="$BASEDIR/deps"
[ -z "$DISTDIR" ] && export DISTDIR="$BASEDIR/dist"
[ -z "$MAKE" ] && export MAKE=make


if [ "$1" = "clean" ] ; then
    echo '****************************************************'
    echo '*                CLEAN DEPENDENCIES                *'
    echo '****************************************************'
    ./build-system/build-dependencies/build-xigen/build-xigen.sh         clean
    ./build-system/build-dependencies/build-bindy/build-bindy.sh         clean
    ./build-system/build-dependencies/build-xiwrapper/build-xiwrapper.sh clean
    ./build-system/build-dependencies/build-miniupnpc/build-miniupnpc.sh clean
    [ -d "$DEPSDIR" ] && rm -rf "$DEPSDIR"
    echo '****************************************************'
    echo '*                       DONE                       *'
    echo '****************************************************'
    exit 0
fi

echo '****************************************************'
echo '*                                                  *'
echo '*              BUILDING DEPENDENCIES               *'
echo '*                                                  *'
echo '****************************************************'


STAGE_NAME=$0
ACTION=$1
[ $# -ge 1 ] && shift

case $ACTION in
"xigen")
    ./build-system/build-dependencies/build-xigen/build-xigen.sh ;;
"bindy")
    ./build-system/build-dependencies/build-bindy/build-bindy.sh ;;
"xiwrapper")
    ./build-system/build-dependencies/build-xiwrapper/build-xiwrapper.sh ;;
"miniupnpc")
    ./build-system/build-dependencies/build-miniupnpc/build-miniupnpc.sh ;;
"")
    ./build-system/build-dependencies/build-xigen/build-xigen.sh
    ./build-system/build-dependencies/build-bindy/build-bindy.sh
    ./build-system/build-dependencies/build-xiwrapper/build-xiwrapper.sh
    ./build-system/build-dependencies/build-miniupnpc/build-miniupnpc.sh
    ;;
"--help")
    echo "Usage: $SCRIPT_NAME $STAGE_NAME [ xigen | bindy | xiwrapper | miniupnpc | clean ]"
    echo "To build all dependencies just use $SCRIPT_NAME $STAGE_NAME"
    echo ""
    echo "OPTIONS:"
    echo "  xigen     - build xigen"
    echo "  bindy     - build Bindy"
    echo "  xiwrapper - build xiwrapper"
    echo "  miniupnpc - build miniupnpc"
    echo "  clean     - remove all dependencies and related files"
    echo ""
    ;;
*)
    echo "\nUnknown command \"$ACTION\"\n"
    echo "Usage: $SCRIPT_NAME $STAGE_NAME [ xigen | bindy | xiwrapper | miniupnpc | clean ]"
    echo "To build all dependencies just use $SCRIPT_NAME $STAGE_NAME"
    echo ""
    echo "OPTIONS:"
    echo "  xigen     - build xigen"
    echo "  bindy     - build Bindy"
    echo "  xiwrapper - build xiwrapper"
    echo "  miniupnpc - build miniupnpc"
    echo "  clean     - remove all dependencies and related files"
    echo ""
    exit 1
esac

echo '****************************************************'
echo '*        BUILDING DEPENDENCIES IS COMPLETED        *'
echo '****************************************************'
