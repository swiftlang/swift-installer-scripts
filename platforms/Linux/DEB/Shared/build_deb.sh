#!/bin/bash

set -eux

# Set the working directory
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Load version definitions
. "${here}/versions.sh"

# Working in /tmp since Docker file sharing makes this very slow to do directly on the share
staging_dir="/tmp/swift-deb-builder"
package_dir="${staging_dir}/swiftlang-${debversion}"

# Clean
rm -rf "${package_dir}" && mkdir -p "${package_dir}"

# Copy control files to package build directory
cp -r "${here}/debian" "${package_dir}/"
cp -r "${package_dir}/debian/control.in" "${package_dir}/debian/control"

# Build the source package
"${here}/build_source_package.sh" "${staging_dir}"

# Install the build dependencies
cd "${staging_dir}"
mk-build-deps --install "${package_dir}/debian/control.in" --tool 'apt-get -y -o Debug::pkgProblemResolver=yes --no-install-recommends'

# Build the installable package
# TODO: Add signing key information
cd "${package_dir}"
DEB_BUILD_OPTIONS=parallel=64 debuild

# Copy the final packages to /output
cd "${staging_dir}"
cp *.deb /output/
cp *.ddeb /output/
cp *.dsc /output/
cp *.tar.* /output/
