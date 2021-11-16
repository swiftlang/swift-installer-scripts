%include metadata.inc

%global debug_package %{nil}

Name:           %{package_name}
Version:        %{package_version}
Release:        1%{?dist}
Summary:        %{package_summary}
License:        %{package_license}
URL:            %{package_url}

Source0:        https://github.com/apple/swift/archive/swift-%{swift_version}.tar.gz#/swift.tar.gz
Source1:        https://github.com/apple/swift-corelibs-libdispatch/archive/swift-%{swift_version}.tar.gz#/corelibs-libdispatch.tar.gz
Source2:        https://github.com/apple/swift-corelibs-foundation/archive/swift-%{swift_version}.tar.gz#/corelibs-foundation.tar.gz
Source3:        https://github.com/apple/swift-integration-tests/archive/swift-%{swift_version}.tar.gz#/swift-integration-tests.tar.gz
Source4:        https://github.com/apple/swift-corelibs-xctest/archive/swift-%{swift_version}.tar.gz#/corelibs-xctest.tar.gz
Source5:        https://github.com/apple/swift-package-manager/archive/swift-%{swift_version}.tar.gz#/package-manager.tar.gz
Source6:        https://github.com/apple/swift-llbuild/archive/swift-%{swift_version}.tar.gz#/llbuild.tar.gz
Source7:        https://github.com/apple/swift-cmark/archive/swift-%{swift_version}.tar.gz#/cmark.tar.gz
Source8:        https://github.com/apple/swift-xcode-playground-support/archive/swift-%{swift_version}.tar.gz#/swift-xcode-playground-support.tar.gz
Source9:        https://github.com/apple/sourcekit-lsp/archive/swift-%{swift_version}.tar.gz#/sourcekit-lsp.tar.gz
Source10:       https://github.com/apple/indexstore-db/archive/swift-%{swift_version}.tar.gz#/indexstore-db.tar.gz
Source11:       https://github.com/apple/llvm-project/archive/swift-%{swift_version}.tar.gz#/llvm-project.tar.gz
Source12:       https://github.com/apple/swift-tools-support-core/archive/swift-%{swift_version}.tar.gz#/swift-tools-support-core.tar.gz
Source13:       https://github.com/apple/swift-argument-parser/archive/%{swift_argument_parser_version}.tar.gz#/swift-argument-parser.tar.gz
Source14:       https://github.com/apple/swift-driver/archive/swift-%{swift_version}.tar.gz#/swift-driver.tar.gz
Source15:       https://github.com/unicode-org/icu/archive/release-%{icu_version}.tar.gz#/icu.tar.gz
Source16:       https://github.com/apple/swift-syntax/archive/swift-%{swift_version}.zip#/swift-syntax.tar.gz
Source17:       https://github.com/jpsim/Yams/archive/%{yams_version}.zip#/yams.tar.gz
Source18:       https://github.com/apple/swift-crypto/archive/refs/tags/%{swift_crypto_version}.tar.gz#/swift-crypto.tar.gz
Source19:       https://github.com/ninja-build/ninja/archive/refs/tags/v%{ninja_version}.tar.gz#/ninja.tar.gz
Source20:       https://github.com/KitWare/CMake/archive/refs/tags/v%{cmake_version}.tar.gz#/cmake.tar.gz

Patch0:         patches/swift-api-checker.patch
Patch1:         patches/hwasan_symbolize.patch

BuildRequires:  clang
BuildRequires:  curl-devel
BuildRequires:  gcc-c++
BuildRequires:  git
BuildRequires:  glibc-static
BuildRequires:  libbsd-devel
BuildRequires:  libedit-devel
BuildRequires:  libicu-devel
BuildRequires:  libuuid-devel
BuildRequires:  libxml2-devel
BuildRequires:  make
BuildRequires:  ncurses-devel
BuildRequires:  pexpect
BuildRequires:  pkgconfig
BuildRequires:  procps-ng
BuildRequires:  python
BuildRequires:  python-devel
BuildRequires:  python-pkgconfig
BuildRequires:  python-six
BuildRequires:  python3-devel
BuildRequires:  rsync
BuildRequires:  sqlite-devel
BuildRequires:  swig
BuildRequires:  tzdata
BuildRequires:  uuid-devel
BuildRequires:  wget
BuildRequires:  which

Requires:       binutils
Requires:       gcc
Requires:       git
Requires:       glibc-static
Requires:       gzip
Requires:       libbsd
Requires:       libcurl
Requires:       libedit
Requires:       libicu
Requires:       libstdc++-static
Requires:       libuuid
Requires:       libxml2
Requires:       tar
Requires:       tzdata

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
%setup -q -c -n %{swift_source_location} -a 0 -a 1 -a 2 -a 3 -a 4 -a 5 -a 6 -a 7 -a 8 -a 9 -a 10 -a 11 -a 12 -a 13 -a 14 -a 15 -a 16 -a 17 -a 18 -a 19 -a 20
# The Swift build script requires directories to be named
# in a specific way so renaming the source directories is
# necessary
mv swift-cmark-swift-%{swift_version} cmark
mv swift-corelibs-foundation-swift-%{swift_version} swift-corelibs-foundation
mv swift-corelibs-libdispatch-swift-%{swift_version} swift-corelibs-libdispatch
mv swift-corelibs-xctest-swift-%{swift_version} swift-corelibs-xctest
mv swift-integration-tests-swift-%{swift_version} swift-integration-tests
mv swift-llbuild-swift-%{swift_version} llbuild
mv swift-package-manager-swift-%{swift_version} swiftpm
mv swift-swift-%{swift_version} swift
mv swift-xcode-playground-support-swift-%{swift_version} swift-xcode-playground-support
mv sourcekit-lsp-swift-%{swift_version} sourcekit-lsp
mv indexstore-db-swift-%{swift_version} indexstore-db
mv llvm-project-swift-%{swift_version} llvm-project
mv swift-syntax-swift-%{swift_version} swift-syntax
mv swift-tools-support-core-swift-%{swift_version} swift-tools-support-core
mv swift-argument-parser-%{swift_argument_parser_version} swift-argument-parser
mv swift-driver-swift-%{swift_version} swift-driver
mv swift-crypto-%{swift_crypto_version} swift-crypto
mv ninja-%{ninja_version} ninja
mv CMake-%{cmake_version} cmake

# ICU
mv icu-release-%{icu_version} icu

# Yams
mv Yams-%{yams_version} yams

# Adjust python version swift-api-checker
%patch0 -p1

# Adjust python version hwasan_symbolize
%patch1 -p1

%build
export VERBOSE=1

# Run the build
swift/utils/build-script --preset=buildbot_linux,no_test install_destdir=%{_builddir} installable_package=%{_builddir}/swift-%{version}-amazonlinux2.tar.gz

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
