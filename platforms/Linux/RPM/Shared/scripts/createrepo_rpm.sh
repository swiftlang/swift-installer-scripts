#!/usr/bin/env bash

set -eux

output_dir=/output

# Ensure the output directory exists
if [[ ! -d "$output_dir" ]]; then
    echo "$output_dir does not exist, so no place to copy the artifacts!"
    exit 1
fi

# Always make sure we're up to date
dnf update -y

# Prepare directories
mkdir -p "$HOME/createrepo"

# Copy rpm file
cp "$output_dir"/*.rpm "$HOME/createrepo/"

# Create the repodata
createrepo "$HOME/createrepo/" 2>&1 | tee "$HOME/createrepo-output.txt"

# Include the createrepo log which can be used to determine what went
# wrong if there are no artifacts
cp "$HOME/createrepo-output.txt" "$output_dir"
cp -r "$HOME/createrepo/repodata" "$output_dir/repodata"
