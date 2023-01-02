# This source file is part of the Swift.org open source project
#
# Copyright (c) 2022 Apple Inc. and the Swift project authors
# Licensed under Apache License v2.0 with Runtime Library Exception
#
# See http://swift.org/LICENSE.txt for license information
# See http://swift.org/CONTRIBUTORS.txt for Swift project authors

#!/bin/bash

set -eux

source_only_pkg=FALSE # build binary deb by default
while getopts ":s" option; do
  case $option in
    s)
      source_only_pkg=TRUE
      ;;
   esac
done

here="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# load version definitions
. ${here}/versions.sh

# working in /tmp since docker file sharing makes this very slow to do directly on the share
staging_dir=/tmp/swift-deb-builder
package_dir=${staging_dir}/swiftlang-${debversion}

# clean
rm -rf ${package_dir} && mkdir -p ${package_dir}

# use rsync to copy control, rules, patches files to package build directory,
# including relative symlink targets
rsync --archive --copy-links ${here}/debian ${package_dir}/
cp ${package_dir}/debian/control.in ${package_dir}/debian/control

# build the source package
${here}/build_source_package.sh ${staging_dir}

# install the build dependencies
cd ${staging_dir}

if [ -f /.dockerenv ]; then
  root_cmd="" # no --root-cmd assumed needed when executing in container
else
  root_cmd="--root-cmd=sudo"
fi

mk-build-deps --install ${package_dir}/debian/control ${root_cmd} --remove --tool 'apt-get -y -o Debug::pkgProblemResolver=yes --no-install-recommends'


# build the installable package
# TODO: add signing key information
cd ${package_dir}

if [ "$source_only_pkg" == "TRUE" ]; then
    DEB_BUILD_OPTIONS=parallel=64 debuild -uc -us -sa -S
else
    DEB_BUILD_OPTIONS=parallel=64 debuild
fi

# copy the final packages to /output
cd ${staging_dir}

if [ "$source_only_pkg" == "FALSE" ]; then
  cp *.deb /output/
  cp *.ddeb /output/
fi

cp *.dsc /output/
cp *.tar.* /output/
cp *.changes /output/
cp *.buildinfo /output/
