# This source file is part of the Swift.org open source project
#
# Copyright (c) 2022 Apple Inc. and the Swift project authors
# Licensed under Apache License v2.0 with Runtime Library Exception
#
# See http://swift.org/LICENSE.txt for license information
# See http://swift.org/CONTRIBUTORS.txt for Swift project authors

FROM oraclelinux:8
LABEL PURPOSE="This image is configured to build Swift for the version of Oracle Linux listed above"

RUN yum -y update

# RPM and yum development tools
RUN yum install -y rpmdevtools yum-utils createrepo

# Configure epel and codeready
RUN yum install -y oracle-epel-release-el8
RUN yum config-manager --set-enabled ol8_codeready_builder

# Optimization: Install Swift build requirements listed in the spec file
ADD swiftlang.spec /tmp/swiftlang.spec
# rewrite a minimal spec with the build requirements
RUN echo -e "Name: optimization\nVersion: optimization\nRelease: optimization\nSummary: optimization\nLicense: optimization\n" > /tmp/optimization.spec
RUN cat /tmp/swiftlang.spec | grep BuildRequires >> /tmp/optimization.spec
RUN echo -e "\n%description" >> /tmp/optimization.spec
# install the build requirements
RUN cd /tmp && yum-builddep -y optimization.spec
