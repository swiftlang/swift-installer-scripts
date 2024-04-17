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

# Optimization: Install Swift build requirements listed in the spec file
ADD swiftlang.spec /tmp/swiftlang.spec
# rewrite a minimal spec with the build requirements
RUN echo -e "Name: optimization\nVersion: optimization\nRelease: optimization\nSummary: optimization\nLicense: optimization\n" > /tmp/optimization.spec
RUN cat /tmp/swiftlang.spec | grep BuildRequires >> /tmp/optimization.spec
RUN echo -e "\n%description" >> /tmp/optimization.spec
# install the build requirements
RUN cd /tmp && yum-builddep --enablerepo=centosplus -y optimization.spec

# Workaround to support clang-3.5 or a later version
RUN echo -e ". /opt/rh/sclo-git25/enable\n. /opt/rh/devtoolset-8/enable\n" >> $HOME/.bashrc
RUN source $HOME/.bashrc
RUN sed -i -e 's/\*__block/\*__libc_block/g' /usr/include/unistd.h

ARG SWIFT_PLATFORM=centos7
ARG SWIFT_VERSION=5.8.1
ARG SWIFT_BRANCH=swift-${SWIFT_VERSION}-release
ARG SWIFT_TAG=swift-${SWIFT_VERSION}-RELEASE
ARG SWIFT_WEBROOT=https://download.swift.org
ARG SWIFT_PREFIX=/opt/swift/${SWIFT_VERSION}

ENV SWIFT_PLATFORM=$SWIFT_PLATFORM \
    SWIFT_VERSION=$SWIFT_VERSION \
    SWIFT_BRANCH=$SWIFT_BRANCH \
    SWIFT_TAG=$SWIFT_TAG \
    SWIFT_WEBROOT=$SWIFT_WEBROOT \
    SWIFT_PREFIX=$SWIFT_PREFIX

RUN set -e; \
    SWIFT_WEBDIR="$SWIFT_WEBROOT/$SWIFT_BRANCH/$(echo $SWIFT_PLATFORM | tr -d .)" \
    && SWIFT_BIN_URL="$SWIFT_WEBDIR/$SWIFT_TAG/$SWIFT_TAG-$SWIFT_PLATFORM.tar.gz" \
    && SWIFT_SIG_URL="$SWIFT_BIN_URL.sig" \
    # - Download the GPG keys, Swift toolchain, and toolchain signature, and verify.
    && export GNUPGHOME="$(mktemp -d)" \
    && curl -fsSL "$SWIFT_BIN_URL" -o swift.tar.gz "$SWIFT_SIG_URL" -o swift.tar.gz.sig \
    && curl -fSsL https://swift.org/keys/all-keys.asc | gpg --import -  \
    && gpg --batch --verify swift.tar.gz.sig swift.tar.gz \
    # - Unpack the toolchain, set libs permissions, and clean up.
    && mkdir -p $SWIFT_PREFIX \
    && tar -xzf swift.tar.gz --directory $SWIFT_PREFIX --strip-components=1 \
    && chmod -R o+r $SWIFT_PREFIX/usr/lib/swift \
    && rm -rf "$GNUPGHOME" swift.tar.gz.sig swift.tar.gz

ENV PATH="${SWIFT_PREFIX}/usr/bin:${PATH}"
