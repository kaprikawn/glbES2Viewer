#!/bin/sh

SCRIPT_DIR="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
ROOT_DIR="$( cd $SCRIPT_DIR/.. ; pwd -P )"
BUILD_DIR=$ROOT_DIR/build
SRC_DIR=$ROOT_DIR/src

# echo script is $SCRIPT_DIR
# echo root is $ROOT_DIR
# echo build is $BUILD_DIR
# echo src is $SRC_DIR

DEBUG_BUILD=1

BUILD_CALL="g++ $SRC_DIR/main_linux.cpp -o glbES2Viewer"
if [[ $DEBUG_BUILD -eq 1 ]]; then
  BUILD_CALL="$BUILD_CALL -g -DDEBUG"
fi

mkdir -pv $BUILD_DIR

BUILD_CALL="$BUILD_CALL `sdl2-config --cflags --libs` -lGL"

pushd $BUILD_DIR

eval $BUILD_CALL

echo $BUILD_CALL


popd

exit 0