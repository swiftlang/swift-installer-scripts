%global debug_package %{nil}
%global swifttag 5.5-RELEASE
%global swiftbuild swift-source
%global icu_version 65-1
%global yams_version 4.0.2
%global sap_version 0.4.3
%global swift_crypto_version 1.1.5
%global ninja_version 1.10.2

Name:           swift-lang
Version:        5.5
Release:        1%{?dist}
Summary:        Apple's Swift programming language
License:        ASL 2.0 and Unicode
URL:            https://swift.org

Source0:        https://github.com/apple/swift/archive/swift-%{swifttag}.tar.gz#/swift.tar.gz
Source1:        https://github.com/apple/swift-corelibs-libdispatch/archive/swift-%{swifttag}.tar.gz#/corelibs-libdispatch.tar.gz
Source2:        https://github.com/apple/swift-corelibs-foundation/archive/swift-%{swifttag}.tar.gz#/corelibs-foundation.tar.gz
Source3:        https://github.com/apple/swift-integration-tests/archive/swift-%{swifttag}.tar.gz#/swift-integration-tests.tar.gz
Source4:        https://github.com/apple/swift-corelibs-xctest/archive/swift-%{swifttag}.tar.gz#/corelibs-xctest.tar.gz
Source5:        https://github.com/apple/swift-package-manager/archive/swift-%{swifttag}.tar.gz#/package-manager.tar.gz
Source6:        https://github.com/apple/swift-llbuild/archive/swift-%{swifttag}.tar.gz#/llbuild.tar.gz
Source7:        https://github.com/apple/swift-cmark/archive/swift-%{swifttag}.tar.gz#/cmark.tar.gz
Source8:        https://github.com/apple/swift-xcode-playground-support/archive/swift-%{swifttag}.tar.gz#/swift-xcode-playground-support.tar.gz
Source9:        https://github.com/apple/sourcekit-lsp/archive/swift-%{swifttag}.tar.gz#/sourcekit-lsp.tar.gz
Source10:       https://github.com/apple/indexstore-db/archive/swift-%{swifttag}.tar.gz#/indexstore-db.tar.gz
Source11:       https://github.com/apple/llvm-project/archive/swift-%{swifttag}.tar.gz#/llvm-project.tar.gz
Source12:       https://github.com/apple/swift-tools-support-core/archive/swift-%{swifttag}.tar.gz#/swift-tools-support-core.tar.gz
Source13:       https://github.com/apple/swift-argument-parser/archive/%{sap_version}.tar.gz
Source14:       https://github.com/apple/swift-driver/archive/swift-%{swifttag}.tar.gz#/swift-driver.tar.gz
Source15:       https://github.com/unicode-org/icu/archive/release-%{icu_version}.tar.gz
Source16:       https://github.com/apple/swift-syntax/archive/swift-%{swifttag}.zip#/swift-syntax.tar.gz
Source17:       https://github.com/jpsim/Yams/archive/%{yams_version}.zip
Source18:       https://github.com/apple/swift-crypto/archive/refs/tags/%{swift_crypto_version}.tar.gz
Source19:       https://github.com/ninja-build/ninja/archive/refs/tags/v%{ninja_version}.tar.gz

BuildRequires:  autoconf
BuildRequires:  clang
BuildRequires:  cmake
BuildRequires:  diffutils
BuildRequires:  git
BuildRequires:  glibc-static
BuildRequires:  libbsd-devel
BuildRequires:  libcurl-devel
BuildRequires:  libedit-devel
BuildRequires:  libicu-devel
BuildRequires:  libstdc++-static
BuildRequires:  libtool
BuildRequires:  libuuid-devel
BuildRequires:  libxml2-devel
BuildRequires:  make
BuildRequires:  ncurses-devel
BuildRequires:  pcre-devel
BuildRequires:  python2
BuildRequires:  python2-devel
BuildRequires:  python2-six
BuildRequires:  python3
BuildRequires:  python3-six
BuildRequires:  python3-pexpect
BuildRequires:  platform-python-devel
BuildRequires:  sqlite-devel
BuildRequires:  swig
BuildRequires:  rsync
BuildRequires:  tar
BuildRequires:  which

Requires:       binutils
Requires:       gcc
Requires:       git
Requires:       glibc-static
Requires:       libbsd-devel
Requires:       libedit
Requires:       libedit-devel
Requires:       libicu-devel
Requires:       libstdc++-static
Requires:       pkg-config
Requires:       python3
Requires:       sqlite
Requires:       zlib-devel

ExclusiveArch:  x86_64 aarch64

%description
Swift is a general-purpose programming language built using
a modern approach to safety, performance, and software design
patterns.

The goal of the Swift project is to create the best available
language for uses ranging from systems programming, to mobile
and desktop apps, scaling up to cloud services. Most
importantly, Swift is designed to make writing and maintaining
correct programs easier for the developer.

%prep
%setup -q -c -n %{swiftbuild} -a 0 -a 1 -a 2 -a 3 -a 4 -a 5 -a 6 -a 7 -a 8 -a 9 -a 10 -a 11 -a 12 -a 13 -a 14 -a 15 -a 16 -a 17 -a 18 -a 19
# The Swift build script requires directories to be named
# in a specific way so renaming the source directories is
# necessary
mv swift-cmark-swift-%{swifttag} cmark
mv swift-corelibs-foundation-swift-%{swifttag} swift-corelibs-foundation
mv swift-corelibs-libdispatch-swift-%{swifttag} swift-corelibs-libdispatch
mv swift-corelibs-xctest-swift-%{swifttag} swift-corelibs-xctest
mv swift-integration-tests-swift-%{swifttag} swift-integration-tests
mv swift-llbuild-swift-%{swifttag} llbuild
mv swift-package-manager-swift-%{swifttag} swiftpm
mv swift-swift-%{swifttag} swift
mv swift-xcode-playground-support-swift-%{swifttag} swift-xcode-playground-support
mv sourcekit-lsp-swift-%{swifttag} sourcekit-lsp
mv indexstore-db-swift-%{swifttag} indexstore-db
mv llvm-project-swift-%{swifttag} llvm-project
mv swift-syntax-swift-%{swifttag} swift-syntax
mv swift-tools-support-core-swift-%{swifttag} swift-tools-support-core
mv swift-argument-parser-%{sap_version} swift-argument-parser
mv swift-driver-swift-%{swifttag} swift-driver
mv swift-crypto-%{swift_crypto_version} swift-crypto

# ICU
mv icu-release-%{icu_version} icu

# Yams
mv Yams-%{yams_version} yams

# Ninja
mv ninja-%{ninja_version} ninja

# Fix python to python3
ln -s /usr/bin/python3 /usr/bin/python

%build
export VERBOSE=1

# Run the build
swift/utils/build-script --preset=buildbot_linux,no_test install_destdir=%{_builddir} installable_package=%{_builddir}/swift-%{version}-centos8.tar.gz

%install
mkdir -p %{buildroot}%{_libexecdir}/swift/
cp -r %{_builddir}/usr/* %{buildroot}%{_libexecdir}/swift
mkdir -p %{buildroot}%{_bindir}
ln -fs %{_libexecdir}/swift/bin/swift %{buildroot}%{_bindir}/swift
ln -fs %{_libexecdir}/swift/bin/swiftc %{buildroot}%{_bindir}/swiftc
ln -fs %{_libexecdir}/swift/bin/sourcekit-lsp %{buildroot}%{_bindir}/sourcekit-lsp
mkdir -p %{buildroot}%{_mandir}/man1
cp %{_builddir}/usr/share/man/man1/swift.1 %{buildroot}%{_mandir}/man1/swift.1

%files
%license swift/LICENSE.txt
%{_bindir}/swift
%{_bindir}/swiftc
%{_bindir}/sourcekit-lsp
%{_mandir}/man1/swift.1.gz
%{_libexecdir}/swift/

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig


%changelog
