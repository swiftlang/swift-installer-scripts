%include global.inc
%include metadata.inc

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
Source17:       https://github.com/jpsim/Yams/archive/%{yams_version}.tar.gz#/yams.tar.gz
Source18:       https://github.com/apple/swift-crypto/archive/refs/tags/%{swift_crypto_version}.tar.gz#/swift-crypto.tar.gz
Source19:       https://github.com/ninja-build/ninja/archive/refs/tags/v%{ninja_version}.tar.gz#/ninja.tar.gz
Source20:       https://github.com/KitWare/CMake/archive/refs/tags/v%{cmake_version}.tar.gz#/cmake.tar.gz
Source21:       https://github.com/apple/swift-atomics/archive/%{swift_atomics_version}.tar.gz#/swift-atomics.tar.gz
Source22:       https://github.com/apple/swift-cmark/archive/swift-%{swift_version}-gfm.tar.gz#/swift-cmark-gfm.tar.gz
Source23:       https://github.com/apple/swift-docc/archive/swift-%{swift_version}.tar.gz#/swift-docc.tar.gz
Source24:       https://github.com/apple/swift-docc-render-artifact/archive/swift-%{swift_version}.tar.gz#/swift-docc-render-artifact.tar.gz
Source25:       https://github.com/apple/swift-docc-symbolkit/archive/swift-%{swift_version}.tar.gz#/swift-docc-symbolkit.tar.gz
Source26:       https://github.com/apple/swift-collections/archive/%{swift_collections_version}.tar.gz#/swift-collections.tar.gz
Source27:       https://github.com/apple/swift-numerics/archive/%{swift_numerics_version}.tar.gz#/swift-numerics.tar.gz
Source28:       https://github.com/apple/swift-system/archive/%{swift_system_version}.tar.gz#/swift-system.tar.gz
Source29:       https://github.com/apple/swift-nio/archive/%{swift_nio_version}.tar.gz#/swift-nio.tar.gz
Source30:       https://github.com/apple/swift-nio-ssl/archive/%{swift_nio_ssl_version}.tar.gz#/swift-nio-ssl.tar.gz
Source31:       https://github.com/apple/swift-format/archive/swift-%{swift_version}.tar.gz#/swift-format.tar.gz
Source32:       https://github.com/apple/swift-lmdb/archive/swift-%{swift_version}.tar.gz#/swift-lmdb.tar.gz
Source33:       https://github.com/apple/swift-markdown/archive/swift-%{swift_version}.tar.gz#/swift-markdown.tar.gz
Source34:       https://github.com/apple/swift-experimental-string-processing/archive/swift-%{swift_version}.tar.gz#/swift-experimental-string-processing.tar.gz

Patch0:         nocyclades.patch
Patch1:         unusedvariable.patch

BuildRequires:  clang
BuildRequires:  libbsd-devel
BuildRequires:  libcurl-devel
BuildRequires:  libdispatch-devel
BuildRequires:  libedit-devel
BuildRequires:  libicu-devel
BuildRequires:  libsqlite3x-devel
BuildRequires:  libuuid-devel
BuildRequires:  libxml2-devel
BuildRequires:  perl-podlators
BuildRequires:  python3
BuildRequires:  python3-devel
BuildRequires:  python3-distro
BuildRequires:  python3-setuptools
BuildRequires:  python3-six
BuildRequires:  rsync
BuildRequires:  swig
%if ! 0%{?el8}
BuildRequires:	python-unversioned-command
%endif
BuildRequires:  /usr/bin/pathfix.py

Requires:       binutils-gold
Requires:       gcc
Requires:       glibc-devel
Requires:       ncurses-compat-libs
Requires:       ncurses-devel

ExclusiveArch:  x86_64 aarch64

%description
%include description.inc

%prep
%setup -q -c -n %{swift_source_location} -a 0 -a 1 -a 2 -a 3 -a 4 -a 5 -a 6 -a 7 -a 8 -a 9 -a 10 -a 11 -a 12 -a 13 -a 14 -a 15 -a 16 -a 17 -a 18 -a 19 -a 20 -a 21 -a 22 -a 23 -a 24 -a 25 -a 26 -a 27 -a 28 -a 29 -a 30 -a 31 -a 32 -a 33 -a 34

# The Swift build script requires directories to be named
# in a specific way so renaming the source directories is
# necessary
mv CMake-%{cmake_version} cmake
mv icu-release-%{icu_version} icu
mv indexstore-db-swift-%{swift_version} indexstore-db
mv llvm-project-swift-%{swift_version} llvm-project
mv ninja-%{ninja_version} ninja
mv sourcekit-lsp-swift-%{swift_version} sourcekit-lsp
mv swift-argument-parser-%{swift_argument_parser_version} swift-argument-parser
mv swift-atomics-%{swift_atomics_version} swift-atomics
mv swift-cmark-swift-%{swift_version} cmark
mv swift-cmark-swift-%{swift_version}-gfm swift-cmark-gfm
mv swift-collections-%{swift_collections_version} swift-collections
mv swift-corelibs-foundation-swift-%{swift_version} swift-corelibs-foundation
mv swift-corelibs-libdispatch-swift-%{swift_version} swift-corelibs-libdispatch
mv swift-corelibs-xctest-swift-%{swift_version} swift-corelibs-xctest
mv swift-crypto-%{swift_crypto_version} swift-crypto
mv swift-docc-render-artifact-swift-%{swift_version} swift-docc-render-artifact
mv swift-docc-swift-%{swift_version} swift-docc
mv swift-docc-symbolkit-swift-%{swift_version} swift-docc-symbolkit
mv swift-driver-swift-%{swift_version} swift-driver
mv swift-format-swift-%{swift_version} swift-format
mv swift-integration-tests-swift-%{swift_version} swift-integration-tests
mv swift-llbuild-swift-%{swift_version} llbuild
mv swift-lmdb-swift-%{swift_version} swift-lmdb
mv swift-markdown-swift-%{swift_version} swift-markdown
mv swift-nio-%{swift_nio_version} swift-nio
mv swift-nio-ssl-%{swift_nio_ssl_version} swift-nio-ssl
mv swift-numerics-%{swift_numerics_version} swift-numerics
mv swift-package-manager-swift-%{swift_version} swiftpm
mv swift-swift-%{swift_version} swift
mv swift-syntax-swift-%{swift_version} swift-syntax
mv swift-system-%{swift_system_version} swift-system
mv swift-tools-support-core-swift-%{swift_version} swift-tools-support-core
mv swift-xcode-playground-support-swift-%{swift_version} swift-xcode-playground-support
mv Yams-%{yams_version} yams
mv swift-experimental-string-processing-swift-%{swift_version} swift-experimental-string-processing

# Remove Cyclades as it has been removed from the Linux kernel
%patch0 -p0

# Temp patch to test libdispatch issue with clang 13
%patch1 -p0

# Fix python to python3
pathfix.py -pni "%{__python3} %{py3_shbang_opts}" swift/utils/api_checker/swift-api-checker.py
pathfix.py -pni "%{__python3} %{py3_shbang_opts}" llvm-project/compiler-rt/lib/hwasan/scripts/hwasan_symbolize

%build
export VERBOSE=1
# Before Fedora 34, we may not have /usr/bin/python, so we
# roll our own because the build script expects there to be one.
%if 0%{?fedora} < 34 || 0%{?el8}
mkdir $PWD/binforpython
ln -s /usr/bin/python3 $PWD/binforpython/python
export PATH=$PWD/binforpython:$PATH
%endif

# Run the build
swift/utils/build-script --preset=buildbot_linux,no_assertions,no_test install_destdir=%{_builddir} installable_package=%{_builddir}/swift-%{version}-fedora-rawhide.tar.gz

%install
mkdir -p %{buildroot}%{_libexecdir}/swift/%{package_version}
cp -r %{_builddir}/usr/* %{buildroot}%{_libexecdir}/swift/%{package_version}
mkdir -p %{buildroot}%{_bindir}
ln -fs %{_libexecdir}/swift/%{package_version}/bin/swift %{buildroot}%{_bindir}/swift
ln -fs %{_libexecdir}/swift/%{package_version}/bin/swiftc %{buildroot}%{_bindir}/swiftc
ln -fs %{_libexecdir}/swift/%{package_version}/bin/sourcekit-lsp %{buildroot}%{_bindir}/sourcekit-lsp
mkdir -p %{buildroot}%{_mandir}/man1
cp %{_builddir}/usr/share/man/man1/swift.1 %{buildroot}%{_mandir}/man1/swift.1

# This is to fix an issue with check-rpaths complaining about
# how the Swift binaries use RPATH
export QA_SKIP_RPATHS=1

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
