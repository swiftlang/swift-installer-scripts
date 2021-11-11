# This source file is part of the Swift.org open source project
#
# Copyright (c) 2021 Apple Inc. and the Swift project authors
# Licensed under Apache License v2.0 with Runtime Library Exception
#
# See http://swift.org/LICENSE.txt for license information
# See http://swift.org/CONTRIBUTORS.txt for Swift project authors

FROM centos:7
LABEL PURPOSE="This image is configured to build Swift for the version of CentOS listed above"

RUN yum -y update

# RPM and yum development tools
RUN yum install -y rpmdevtools yum-utils createrepo

# Configure epel
RUN yum install -y epel-release centos-release-scl

# Optimization: Install all the dependencies needed to build Swift from the spec file itself
ADD swiftlang.spec /tmp/swiftlang.spec
RUN touch /tmp/metadata.inc # fake metadata is okay for this optimization
RUN cd /tmp && yum-builddep -y swiftlang.spec

# Workaround to support clang-3.5 or a later version
RUN echo -e ". /opt/rh/sclo-git25/enable\n. /opt/rh/llvm-toolset-7/enable\n. /opt/rh/devtoolset-8/enable\n" >> $HOME/.bashrc
RUN source $HOME/.bashrc
RUN sed -i -e 's/\*__block/\*__libc_block/g' /usr/include/unistd.h
