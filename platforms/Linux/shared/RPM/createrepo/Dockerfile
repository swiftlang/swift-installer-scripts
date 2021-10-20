# This source file is part of the Swift.org open source project
#
# Copyright (c) 2021 Apple Inc. and the Swift project authors
# Licensed under Apache License v2.0 with Runtime Library Exception
#
# See http://swift.org/LICENSE.txt for license information
# See http://swift.org/CONTRIBUTORS.txt for Swift project authors


FROM centos:8
LABEL PURPOSE="This image is configured to create repodata"

RUN yum -y update

# RPM and yum development tools
RUN yum install -y createrepo
