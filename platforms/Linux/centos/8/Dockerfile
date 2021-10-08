# This source file is part of the Swift.org open source project
#
# Copyright (c) 2021 Apple Inc. and the Swift project authors
# Licensed under Apache License v2.0 with Runtime Library Exception
#
# See http://swift.org/LICENSE.txt for license information
# See http://swift.org/CONTRIBUTORS.txt for Swift project authors

FROM centos:8
LABEL PURPOSE="This image is configured to build Swift for the version of CentOS listed above"

RUN yum -y update

# RPM and yum development tools
RUN yum install -y rpmdevtools yum-utils

# Configure powertools
RUN yum install -y https://dl.fedoraproject.org/pub/epel/epel-release-latest-8.noarch.rpm
RUN yum config-manager --set-enabled powertools

# Optimization: Install all the dependencies needed to build Swift from the spec file itself
ADD swift-lang.spec /tmp/swift-lang.spec
RUN yum-builddep -y /tmp/swift-lang.spec
