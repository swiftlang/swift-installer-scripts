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
RUN yum install -y rpmdevtools yum-utils createrepo

# Configure epel
RUN yum install -y https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm

# Optimization: Install Swift build requirements listed in the spec file
ADD swiftlang.spec /tmp/swiftlang.spec
# rewrite a minimal spec with the build requirements
RUN echo -e "Name: optimization\nVersion: optimization\nRelease: optimization\nSummary: optimization\nLicense: optimization\n" > /tmp/optimization.spec
RUN cat /tmp/swiftlang.spec | grep BuildRequires >> /tmp/optimization.spec
RUN echo -e "\n%description" >> /tmp/optimization.spec
# install the build requirements
RUN cd /tmp && yum-builddep -y optimization.spec

# fix python six
RUN mkdir -p /usr/local/lib/python3.7/site-packages/
RUN easy_install-3.7 six
