# This source file is part of the Swift.org open source project
#
# Copyright (c) 2021 Apple Inc. and the Swift project authors
# Licensed under Apache License v2.0 with Runtime Library Exception
#
# See http://swift.org/LICENSE.txt for license information
# See http://swift.org/CONTRIBUTORS.txt for Swift project authors

#!/usr/bin/env bash

set -ex

OUTDIR=/output
if [[ ! -d "$OUTDIR" ]]; then
    echo "$OUTDIR does not exist, so no place to copy the artifacts!"
    exit 1
fi

# always make sure we're up to date
yum update -y

# prepare direcoties
mkdir -p $HOME/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}

# Add the spec
cp swiftlang.spec $HOME/rpmbuild/SPECS/
# Add the metadata files
cp *.inc $HOME/rpmbuild/SPECS/
# Add any patches
cp patches/*.patch $HOME/rpmbuild/SOURCES/

pushd $HOME/rpmbuild/SPECS
# install all the dependencies needed to build Swift from the spec file itself
yum-builddep -y ./swiftlang.spec
# get the sources for Swift as defined in the spec file
spectool -g -R ./swiftlang.spec
# Now we proceed to build Swift. If this is successful, we
# will have two files: a SRPM file which contains the source files
# as well as a regular RPM file that can be installed via `dnf' or `yum'
rpmbuild -ba ./swiftlang.spec 2>&1 | tee /root/build-output.txt
popd

# Include the build log which can be used to determine what went
# wrong if there are no artifacts
cp $HOME/build-output.txt $OUTDIR
cp $HOME/rpmbuild/SRPMS/* $OUTDIR
cp $HOME/rpmbuild/RPMS/`uname -i`/* $OUTDIR
