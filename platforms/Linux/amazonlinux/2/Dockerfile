# This source file is part of the Swift.org open source project
#
# Copyright (c) 2021 Apple Inc. and the Swift project authors
# Licensed under Apache License v2.0 with Runtime Library Exception
#
# See http://swift.org/LICENSE.txt for license information
# See http://swift.org/CONTRIBUTORS.txt for Swift project authors

FROM amazonlinux:2
LABEL PURPOSE="This image is configured to build Swift for the version of Amazon Linux listed above"

RUN yum -y update

# RPM and yum development tools
RUN yum install -y rpmdevtools yum-utils

# Configure epel
RUN yum install -y https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm

# Optimization: Install all the dependencies needed to build Swift from the spec file itself
ADD swiftlang.spec /tmp/swiftlang.spec
RUN touch /tmp/metadata.inc # fake metadata is okay for this optimization
RUN cd /tmp && yum-builddep -y swiftlang.spec
