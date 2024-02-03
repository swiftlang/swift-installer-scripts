# This source file is part of the Swift.org open source project
#
# Copyright (c) 2022 Apple Inc. and the Swift project authors
# Licensed under Apache License v2.0 with Runtime Library Exception
#
# See http://swift.org/LICENSE.txt for license information
# See http://swift.org/CONTRIBUTORS.txt for Swift project authors

FROM fedora:35
LABEL PURPOSE="This image is configured to build Swift for the version of Fedora listed above"

RUN dnf -y update

# RPM and dnf development tools
RUN dnf install -y rpmdevtools dnf-utils createrepo

# Optimization: Install Swift build requirements listed in the spec file
COPY swiftlang.spec /tmp/swiftlang.spec
# Rewrite a minimal spec with the build requirements
RUN echo -e "Name: optimization\nVersion: optimization\nRelease: optimization\nSummary: optimization\nLicense: optimization\n" > /tmp/optimization.spec
RUN cat /tmp/swiftlang.spec | grep BuildRequires >> /tmp/optimization.spec
RUN echo -e "\n%description" >> /tmp/optimization.spec
# Install the build requirements
RUN cd /tmp && dnf builddep -y optimization.spec
