# This source file is part of the Swift.org open source project
#
# Copyright (c) 2022 Apple Inc. and the Swift project authors
# Licensed under Apache License v2.0 with Runtime Library Exception
#
# See http://swift.org/LICENSE.txt for license information
# See http://swift.org/CONTRIBUTORS.txt for Swift project authors

#!/bin/bash

set -eux

here="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# load version definitions
. ${here}/versions.sh

# working in /tmp since docker file sharing makes this very slow to do dirctly on the share
staging_dir=/tmp/swift-deb-builder
package_dir=${staging_dir}/swiftlang-${debversion}

# clean
rm -rf ${package_dir} && mkdir -p ${package_dir}

# copy control files to pakcage build directory
cp -r ${here}/debian ${package_dir}/
cp -r ${package_dir}/debian/control.in ${package_dir}/debian/control

# build the source package
${here}/build_source_package.sh ${staging_dir}

# install the build dependencies
cd ${staging_dir}
mk-build-deps --install ${package_dir}/debian/control.in --tool 'apt-get -y -o Debug::pkgProblemResolver=yes --no-install-recommends'

# build the installable package
# TODO: add signing key information
cd ${package_dir}
DEB_BUILD_OPTIONS=parallel=64 debuild

# copy the final packages to /output
cd ${staging_dir}
cp *.deb /output/
cp *.ddeb /output/
cp *.dsc /output/
cp *.tar.* /output/
