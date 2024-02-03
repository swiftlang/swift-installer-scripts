#!/usr/bin/env bash

set -eux

OUTDIR=/output

# Ensure the output directory exists
if [[ ! -d "$OUTDIR" ]]; then
    echo "$OUTDIR does not exist, so no place to copy the artifacts!"
    exit 1
fi

# Always make sure we're up to date
dnf update -y

# Prepare directories
mkdir -p "$HOME/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}"

# Add the spec
cp swiftlang.spec "$HOME/rpmbuild/SPECS/"
# Add the metadata files
cp *.inc "$HOME/rpmbuild/SPECS/"
# Add any patches
cp patches/*.patch "$HOME/rpmbuild/SOURCES/"

pushd "$HOME/rpmbuild/SPECS"
# Install all the dependencies needed to build Swift from the spec file itself
dnf builddep -y ./swiftlang.spec
# Get the sources for Swift as defined in the spec file
spectool -g -R ./swiftlang.spec
# Now we proceed to build Swift. If this is successful, we
# will have two files: a SRPM file which contains the source files
# as well as a regular RPM file that can be installed via `dnf' or `yum'
rpmbuild -ba ./swiftlang.spec 2>&1 | tee "$HOME/build-output.txt"
popd

# Include the build log which can be used to determine what went
# wrong if there are no artifacts
cp "$HOME/build-output.txt" "$OUTDIR"
cp "$HOME/rpmbuild/SRPMS/"* "$OUTDIR"
cp "$HOME/rpmbuild/RPMS/$(arch)"/* "$OUTDIR"
