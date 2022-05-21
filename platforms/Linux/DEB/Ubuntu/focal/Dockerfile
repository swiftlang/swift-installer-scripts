# This source file is part of the Swift.org open source project
#
# Copyright (c) 2022 Apple Inc. and the Swift project authors
# Licensed under Apache License v2.0 with Runtime Library Exception
#
# See http://swift.org/LICENSE.txt for license information
# See http://swift.org/CONTRIBUTORS.txt for Swift project authors

FROM ubuntu:focal
LABEL PURPOSE="This image is configured to build Swift for the version of Ubuntu listed above"

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update

# Required deb packaging tools
RUN apt-get install -y curl devscripts equivs quilt tar

# Optimization: Install Swift build requirements listed in the control file
ADD debian/control.in /tmp/control.in
RUN mk-build-deps --install /tmp/control.in --tool 'apt-get -y -o Debug::pkgProblemResolver=yes --no-install-recommends'
