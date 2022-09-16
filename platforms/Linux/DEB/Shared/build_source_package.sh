# This source file is part of the Swift.org open source project
#
# Copyright (c) 2022 Apple Inc. and the Swift project authors
# Licensed under Apache License v2.0 with Runtime Library Exception
#
# See http://swift.org/LICENSE.txt for license information
# See http://swift.org/CONTRIBUTORS.txt for Swift project authors

#!/bin/sh

# This helper script will fetch the Swift sources, save the
# tarballs following the appropriate naming convention, and
# build a Debian source package.

# You will need the devscripts and quilt packages installed
# to run this script and build the packages following these
# instructions.

# Create swiftlang-X.Y.Z, unpack the debian tarball within,
# proceed to adding the appropriate entry to the Debian
# changelog, edit debian/source-version.sh, then run this
# script from the directory containing swiftlang-X.Y.Z.
#   $ mkdir swiftlang-X.Y.Z
#   $ cd swiftlang-X.Y.Z
#   $ tar xvaf ../swiftlang_A.B.C-D.debian.tar.xz
#   $ dch -v X.Y.Z-1
#   $ cd ..
#   $ swiftlang-X.Y.Z/debian/getsource.sh
#
# This will download all the necessary sources, unpack them
# as required, refresh any Debian patches, then create the
# Debian source package.

# If all is well, proceed with building the packages.
#   $ cd swiftlang-X.Y.Z
#   $ DEB_BUILD_OPTIONS=parallel=64 debuild

set -eux

here="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# load version definitions
. ${here}/versions.sh

staging_dir=$1
package_dir=$staging_dir/swiftlang-${debversion}

get_component ()
{
    component=$1
    url="$2"

    dest=${staging_dir}/swiftlang_${debversion}.orig-${component}

    echo "Downloading ${component} from ${url}"

    case "${url}" in
	*.zip)
	    tmpdest=${component}.zip
	    dest=${dest}.tar.gz

	    curl -L -o ${tmpdest} "${url}"
	    mk-origtargz --package swiftlang -v ${debversion} -c ${component} --rename --compression gzip ${tmpdest}
	    ;;

	*.tar.gz)
	    dest=${dest}.tar.gz
	    curl -L -o ${dest} "${url}"
	    ;;

	*.tar.xz)
	    dest=${dest}.tar.xz
	    curl -L -o ${dest} "${url}"
	    ;;

	*.tar.bz2)
	    dest=${dest}.tar.bz2
	    curl -L -o ${dest} "${url}"
	    ;;

	*)
	    echo "Source archive not in a suitable format for Debian sources: ${url}" >&2
	    exit 1
    esac

    echo "Extracting ${component}"
    mkdir ${package_dir}/${component}
    tar -C ${package_dir}/${component} --strip-components=1 -axf ${dest}
}

get_component swift https://github.com/apple/swift/archive/swift-${swift_version}.tar.gz
get_component swift-corelibs-libdispatch https://github.com/apple/swift-corelibs-libdispatch/archive/swift-${swift_version}.tar.gz
get_component swift-corelibs-foundation https://github.com/apple/swift-corelibs-foundation/archive/swift-${swift_version}.tar.gz
get_component swift-integration-tests https://github.com/apple/swift-integration-tests/archive/swift-${swift_version}.tar.gz
get_component swift-corelibs-xctest https://github.com/apple/swift-corelibs-xctest/archive/swift-${swift_version}.tar.gz
get_component swiftpm https://github.com/apple/swift-package-manager/archive/swift-${swift_version}.tar.gz
get_component llbuild https://github.com/apple/swift-llbuild/archive/swift-${swift_version}.tar.gz
get_component cmark https://github.com/apple/swift-cmark/archive/swift-${swift_version}.tar.gz
get_component swift-xcode-playground-support https://github.com/apple/swift-xcode-playground-support/archive/swift-${swift_version}.tar.gz
get_component sourcekit-lsp https://github.com/apple/sourcekit-lsp/archive/swift-${swift_version}.tar.gz
get_component indexstore-db https://github.com/apple/indexstore-db/archive/swift-${swift_version}.tar.gz
get_component llvm-project https://github.com/apple/llvm-project/archive/swift-${swift_version}.tar.gz
get_component swift-tools-support-core https://github.com/apple/swift-tools-support-core/archive/swift-${swift_version}.tar.gz
get_component swift-argument-parser https://github.com/apple/swift-argument-parser/archive/${swift_argument_parser_version}.tar.gz
get_component swift-driver https://github.com/apple/swift-driver/archive/swift-${swift_version}.tar.gz
get_component icu https://github.com/unicode-org/icu/archive/release-${icu_version}.tar.gz
get_component swift-crypto https://github.com/apple/swift-crypto/archive/refs/tags/${swift_crypto_version}.tar.gz
get_component ninja https://github.com/ninja-build/ninja/archive/refs/tags/v${ninja_version}.tar.gz
get_component cmake https://github.com/KitWare/CMake/archive/refs/tags/v${cmake_version}.tar.gz
get_component swift-syntax https://github.com/apple/swift-syntax/archive/swift-${swift_version}.tar.gz
get_component yams https://github.com/jpsim/Yams/archive/${yams_version}.tar.gz
get_component swift-atomics https://github.com/apple/swift-atomics/archive/${swift_atomics_version}.tar.gz
get_component swift-cmark-gfm https://github.com/apple/swift-cmark/archive/swift-${swift_version}-gfm.tar.gz
get_component swift-docc https://github.com/apple/swift-docc/archive/swift-${swift_version}.tar.gz
get_component swift-docc-render-artifact https://github.com/apple/swift-docc-render-artifact/archive/swift-${swift_version}.tar.gz
get_component swift-docc-symbolkit https://github.com/apple/swift-docc-symbolkit/archive/swift-${swift_version}.tar.gz
get_component swift-collections https://github.com/apple/swift-collections/archive/${swift_collections_version}.tar.gz
get_component swift-numerics https://github.com/apple/swift-numerics/archive/${swift_numerics_version}.tar.gz
get_component swift-system https://github.com/apple/swift-system/archive/${swift_system_version}.tar.gz
get_component swift-nio https://github.com/apple/swift-nio/archive/${swift_nio_version}.tar.gz
get_component swift-nio-ssl https://github.com/apple/swift-nio-ssl/archive/${swift_nio_ssl_version}.tar.gz
get_component swift-format https://github.com/apple/swift-format/archive/swift-${swift_version}.tar.gz
get_component swift-lmdb https://github.com/apple/swift-lmdb/archive/swift-${swift_version}.tar.gz
get_component swift-markdown https://github.com/apple/swift-markdown/archive/swift-${swift_version}.tar.gz
get_component swift-experimental-string-processing https://github.com/apple/swift-experimental-string-processing/archive/swift-${swift_version}.tar.gz

# Refresh patches, if any
if [ -s swiftlang-${debversion}/debian/patches/series ]; then
    cd swiftlang-${debversion}

    export QUILT_PATCHES=debian/patches
    export QUILT_REFRESH_ARGS="-p ab --no-timestamps --no-index"

    while quilt push; do quilt refresh; done
    quilt pop -a

    cd -
fi

# create a source package
cd $staging_dir
dpkg-source --create-empty-orig -b ${package_dir}
