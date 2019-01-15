SOURCE_DIR=$(pwd)/$(dirname "$0")/../..
BUILD_DIR=$SOURCE_DIR/../openglnoel-build
INSTALL_DIR=$SOURCE_DIR/../openglnoel-install
mkdir -p $BUILD_DIR
pushd $BUILD_DIR
cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR -DASSIMP_BUILD_TESTS=0 $SOURCE_DIR