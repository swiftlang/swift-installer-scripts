%include metadata.inc

%global debug_package %{nil}

Name:           %{package_name}
Version:        %{package_version}
Release:        1%{?dist}
Summary:        %{package_summary}
License:        %{package_license}
URL:            %{package_url}

Source0:        https://github.com/apple/swift/archive/8c610f7f19b9c4e0651f95fa7c4852e5d3b7a03e.tar.gz#/swift.tar.gz
Source1:        https://github.com/apple/swift-corelibs-libdispatch/archive/880bf655b65595862bed8fc5cbd922f60765e8b0.tar.gz#/corelibs-libdispatch.tar.gz
Source2:        https://github.com/apple/swift-corelibs-foundation/archive/061007026b6a12039bede2c6419753c8630741d4.tar.gz#/corelibs-foundation.tar.gz
Source3:        https://github.com/apple/swift-integration-tests/archive/3670ed08b57bd1c1b6dffc677cd93ffcfd1ddd48.tar.gz#/swift-integration-tests.tar.gz
Source4:        https://github.com/apple/swift-corelibs-xctest/archive/2f64db1fdcc600d26566fe67ffceca7d76226b8b.tar.gz#/corelibs-xctest.tar.gz
Source5:        https://github.com/apple/swift-package-manager/archive/6647fa09d6042b29cbb115eecf6f292beb7f6837.tar.gz#/package-manager.tar.gz
Source6:        https://github.com/apple/swift-llbuild/archive/acd686530e56122d916acd49a166beb9198e9b87.tar.gz#/llbuild.tar.gz
Source7:        https://github.com/apple/swift-cmark/archive/9c8096a23f44794bde297452d87c455fc4f76d42.tar.gz#/cmark.tar.gz
Source8:        https://github.com/apple/swift-xcode-playground-support/archive/dd0d8c8d121d2f20664e4779a3d29482a55908bb.tar.gz#/swift-xcode-playground-support.tar.gz
Source9:        https://github.com/apple/sourcekit-lsp/archive/489da496c46e01746ea5158ea0b8ec6e4a8a1f97.tar.gz#/sourcekit-lsp.tar.gz
Source10:       https://github.com/apple/indexstore-db/archive/47aa228fa4d47ab90e8a3b8d9468d2ca99434463.tar.gz#/indexstore-db.tar.gz
Source11:       https://github.com/apple/llvm-project/archive/900c3b6b832d1d0e7d6e1220f6ba001802cbe0cc.tar.gz#/llvm-project.tar.gz
Source12:       https://github.com/apple/swift-tools-support-core/archive/107e570e3565920174d5a25bc3a0340b32d16042.tar.gz#/swift-tools-support-core.tar.gz
Source13:       https://github.com/apple/swift-argument-parser/archive/%{swift_argument_parser_version}.tar.gz#/swift-argument-parser.tar.gz
Source14:       https://github.com/apple/swift-driver/archive/9982f32f96a2e0e597d1b4a0af4a7e997dc471be.tar.gz#/swift-driver.tar.gz
Source15:       https://github.com/unicode-org/icu/archive/release-%{icu_version}.tar.gz#/icu.tar.gz
Source16:       https://github.com/apple/swift-syntax/archive/87d6d0af3d2db26ce0d6014cd953e546d45f63c6.tar.gz#/swift-syntax.tar.gz
Source17:       https://github.com/jpsim/Yams/archive/%{yams_version}.zip#/yams.tar.gz
Source18:       https://github.com/apple/swift-crypto/archive/refs/tags/%{swift_crypto_version}.tar.gz#/swift-crypto.tar.gz
Source19:       https://github.com/ninja-build/ninja/archive/refs/tags/v%{ninja_version}.tar.gz#/ninja.tar.gz
Source20:       https://github.com/KitWare/CMake/archive/refs/tags/v%{cmake_version}.tar.gz#/cmake.tar.gz
Source21:       https://github.com/apple/swift-atomics/archive/%{swift_atomics_version}.tar.gz#/swift-atomics.tar.gz
Source22:       https://github.com/apple/swift-cmark/archive/7fc530e5f12432db9768326ff74dc46de72a24e2.tar.gz#/swift-cmark-gfm.tar.gz
Source23:       https://github.com/apple/swift-docc/archive/ed41ca7bdd3adb59702285e7ca94b60d6ba6f3c4.tar.gz#/swift-docc.tar.gz
Source24:       https://github.com/apple/swift-docc-render-artifact/archive/a6507a5a9b35c4f29178807dec35ba9437449089.tar.gz#/swift-docc-render-artifact.tar.gz
Source25:       https://github.com/apple/swift-docc-symbolkit/archive/aab31e5bfe39775e1b8555fe6d91255559bf5a5f.tar.gz#/swift-docc-symbolkit.tar.gz
Source26:       https://github.com/apple/swift-collections/archive/%{swift_collections_version}.tar.gz#/swift-collections.tar.gz
Source27:       https://github.com/apple/swift-numerics/archive/%{swift_numerics_version}.tar.gz#/swift-numerics.tar.gz
Source28:       https://github.com/apple/swift-system/archive/%{swift_system_version}.tar.gz#/swift-system.tar.gz
Source29:       https://github.com/apple/swift-nio/archive/%{swift_nio_version}.tar.gz#/swift-nio.tar.gz
Source30:       https://github.com/apple/swift-nio-ssl/archive/%{swift_nio_ssl_version}.tar.gz#/swift-nio-ssl.tar.gz
Source31:       https://github.com/apple/swift-format/archive/3e924a83fc24e81c5d7fdd5f93da3fedb49872b0.tar.gz#/swift-format.tar.gz
Source32:       https://github.com/apple/swift-lmdb/archive/6ea45a7ebf6d8f72bd299dfcc3299e284bbb92ee.tar.gz#/swift-lmdb.tar.gz
Source33:       https://github.com/apple/swift-markdown/archive/16ebb0ccea68c0009f550bd48cca1df8675685dc.tar.gz#/swift-markdown.tar.gz

Patch0:         patches/hwasan_symbolize.patch

BuildRequires:  autoconf
BuildRequires:  clang
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
BuildRequires:  platform-python-devel
BuildRequires:  python2
BuildRequires:  python2-devel
BuildRequires:  python2-six
BuildRequires:  python3
BuildRequires:  python3-pexpect
BuildRequires:  python3-six
BuildRequires:  rsync
BuildRequires:  sqlite-devel
BuildRequires:  swig
BuildRequires:  tar
BuildRequires:  which

Requires:       binutils
Requires:       gcc
Requires:       git
Requires:       glibc-static
Requires:       libbsd-devel
Requires:       libcurl-devel
Requires:       libedit
Requires:       libedit-devel
Requires:       libicu-devel
Requires:       libstdc++-static
Requires:       libxml2-devel
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
%setup -q -c -n %{swift_source_location} -a 0 -a 1 -a 2 -a 3 -a 4 -a 5 -a 6 -a 7 -a 8 -a 9 -a 10 -a 11 -a 12 -a 13 -a 14 -a 15 -a 16 -a 17 -a 18 -a 19 -a 20 -a 21 -a 22 -a 23 -a 24 -a 25 -a 26 -a 27 -a 28 -a 29 -a 30 -a 31 -a 32 -a 33

# The Swift build script requires directories to be named
# in a specific way so renaming the source directories is
# necessary
mv CMake-%{cmake_version} cmake
mv icu-release-%{icu_version} icu
mv indexstore-db-47aa228fa4d47ab90e8a3b8d9468d2ca99434463 indexstore-db
mv llvm-project-900c3b6b832d1d0e7d6e1220f6ba001802cbe0cc llvm-project
mv ninja-%{ninja_version} ninja
mv sourcekit-lsp-489da496c46e01746ea5158ea0b8ec6e4a8a1f97 sourcekit-lsp
mv swift-8c610f7f19b9c4e0651f95fa7c4852e5d3b7a03e swift
mv swift-argument-parser-%{swift_argument_parser_version} swift-argument-parser
mv swift-atomics-%{swift_atomics_version} swift-atomics
mv swift-cmark-7fc530e5f12432db9768326ff74dc46de72a24e2 swift-cmark-gfm
mv swift-cmark-9c8096a23f44794bde297452d87c455fc4f76d42 cmark
mv swift-collections-%{swift_collections_version} swift-collections
mv swift-corelibs-foundation-061007026b6a12039bede2c6419753c8630741d4 swift-corelibs-foundation
mv swift-corelibs-libdispatch-880bf655b65595862bed8fc5cbd922f60765e8b0 swift-corelibs-libdispatch
mv swift-corelibs-xctest-2f64db1fdcc600d26566fe67ffceca7d76226b8b swift-corelibs-xctest
mv swift-crypto-%{swift_crypto_version} swift-crypto
mv swift-docc-ed41ca7bdd3adb59702285e7ca94b60d6ba6f3c4 swift-docc
mv swift-docc-render-artifact-a6507a5a9b35c4f29178807dec35ba9437449089 swift-docc-render-artifact
mv swift-docc-symbolkit-aab31e5bfe39775e1b8555fe6d91255559bf5a5f swift-docc-symbolkit
mv swift-driver-9982f32f96a2e0e597d1b4a0af4a7e997dc471be swift-driver
mv swift-format-3e924a83fc24e81c5d7fdd5f93da3fedb49872b0 swift-format
mv swift-integration-tests-3670ed08b57bd1c1b6dffc677cd93ffcfd1ddd48 swift-integration-tests
mv swift-llbuild-acd686530e56122d916acd49a166beb9198e9b87 llbuild
mv swift-lmdb-6ea45a7ebf6d8f72bd299dfcc3299e284bbb92ee swift-lmdb
mv swift-markdown-16ebb0ccea68c0009f550bd48cca1df8675685dc swift-markdown
mv swift-nio-%{swift_nio_version} swift-nio
mv swift-nio-ssl-%{swift_nio_ssl_version} swift-nio-ssl
mv swift-numerics-%{swift_numerics_version} swift-numerics
mv swift-package-manager-6647fa09d6042b29cbb115eecf6f292beb7f6837 swiftpm
mv swift-syntax-87d6d0af3d2db26ce0d6014cd953e546d45f63c6 swift-syntax
mv swift-system-%{swift_system_version} swift-system
mv swift-tools-support-core-107e570e3565920174d5a25bc3a0340b32d16042 swift-tools-support-core
mv swift-xcode-playground-support-dd0d8c8d121d2f20664e4779a3d29482a55908bb swift-xcode-playground-support
mv Yams-%{yams_version} yams

# Adjust python version hwasan_symbolize
%patch0 -p1

# Fix python to python3
ln -s /usr/bin/python3 /usr/bin/python

%build
export VERBOSE=1

# Run the build
swift/utils/build-script --preset=buildbot_linux,no_assertions,no_test install_destdir=%{_builddir} installable_package=%{_builddir}/swift-%{version}-centos8.tar.gz

%install
mkdir -p %{buildroot}%{_libexecdir}/swift/%{package_version}
cp -r %{_builddir}/usr/* %{buildroot}%{_libexecdir}/swift/%{package_version}
mkdir -p %{buildroot}%{_bindir}
ln -fs %{_libexecdir}/swift/%{package_version}/bin/swift %{buildroot}%{_bindir}/swift
ln -fs %{_libexecdir}/swift/%{package_version}/bin/swiftc %{buildroot}%{_bindir}/swiftc
ln -fs %{_libexecdir}/swift/%{package_version}/bin/sourcekit-lsp %{buildroot}%{_bindir}/sourcekit-lsp
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
