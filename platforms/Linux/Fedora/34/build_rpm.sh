#!/usr/bin/env bash

# This script assumes it's running in a container as root
# and that /out is mounted to a directory on the local
# filesystem so the build output and artifacts can be
# copied out and used

OUTDIR=/out
if [[ ! -d "$OUTDIR" ]]
then
    echo "$OUTDIR does not exist, so no place to copy the artifacts!"
    exit 1
fi

# Always make sure we're up to date
yum -y update

# Now we proceed to build Swift. If this is successful, we
# will have two files: a SRPM file which contains the source files
# as well as a regular RPM file that can be installed via `dnf' or `yum'
pushd $HOME/rpmbuild/SPECS
rpmbuild -ba ./swift-lang.spec 2>&1 | tee $HOME/build-output.txt
popd

# Include the build log which can be used to determine what went
# wrong if there are no artifacts
cp $HOME/build-output.txt $OUTDIR
cp $HOME/rpmbuild/SRPMS/* $OUTDIR
cp $HOME/rpmbuild/RPMS/`uname -i`/* $OUTDIR
