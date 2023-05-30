#!/bin/bash
set -euo pipefail
IFS=$'\n\t'

# this build script for older versions of cmake that do not support cmake-presets (<3.19)

thisfile=$(readlink -m $0)
basedir=$(dirname $thisfile)

if [ ! -d "${basedir}/build" ]; then
	mkdir "${basedir}/build"
fi

pushd "${basedir}/build"

	cmake -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_TOOLCHAIN_FILE="${basedir}/vcpkg/scripts/buildsystems/vcpkg.cmake" \
    -DVCPKG_TARGET_TRIPLET=x64-linux-dynamic \
    -GNinja \
    "${basedir}"

	cmake --build .

popd
