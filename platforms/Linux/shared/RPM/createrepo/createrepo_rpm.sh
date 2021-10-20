# This source file is part of the Swift.org open source project
#
# Copyright (c) 2021 Apple Inc. and the Swift project authors
# Licensed under Apache License v2.0 with Runtime Library Exception
#
# See http://swift.org/LICENSE.txt for license information
# See http://swift.org/CONTRIBUTORS.txt for Swift project authors

#!/usr/bin/env bash

set -ex

INDIR=/input
OUTDIR=/output
if [[ ! -d "$INDIR" ]]; then
    echo "$INDIR does not exist, unable to copy rpm file!"
    exit 1
fi
if [[ ! -d "$OUTDIR" ]]; then
    echo "$OUTDIR does not exist, so no place to copy the artifacts!"
    exit 1
fi

# always make sure we're up to date
yum update -y

# prepare direcoties
mkdir -p $HOME/createrepo/

# Copy rpm file
cp $INDIR/*.rpm $HOME/createrepo/

# Create the repodata
createrepo $HOME/createrepo/ 2>&1 | tee /root/createrepo-output.txt


# Include the createrepo log which can be used to determine what went
# wrong if there are no artifacts
cp $HOME/createrepo-output.txt $OUTDIR
cp -r $HOME/createrepo/repodata $OUTDIR/repodata
