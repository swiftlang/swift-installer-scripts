### Sample Package Build and Test
*Test build of a Ubuntu Focal .deb package for compliance testing.*
#### Swift Toolchain Source
```bash
https://download.swift.org/swift-5.5.1-release/ubuntu2004/swift-5.5.1-RELEASE/swift-5.5.1-RELEASE-ubuntu20.04.tar.gz
```
#### Control File

```yaml
# control file
Package: swiftlang
Version: 5.5.1
Iteration: 01-ubuntu-focal
License: Apache License Version 2.0
Architecture: amd64
Maintainer: Swift Infrastructure <swift-infrastructure@swift.org>
Depends: binutils, git, gnupg2, libc6-dev, libcurl4, libedit2, libgcc-9-dev, python3, libpython3.8, libsqlite3-0, libstdc++-9-dev, libxml2, libz3-dev, pkg-config, tzdata, zlib1g-dev
Section: devel
Priority: optional
Homepage: https://swift.org
Description: The Swift Programming Language.
 Swift is a general-purpose programming language built using a modern approach to safety, performance, and software design patterns.
```
#### Package Directory Structure
```bash
# download and unpack swift toolchain
wget https://download.swift.org/swift-5.5.1-release/ubuntu2004/swift-5.5.1-RELEASE/swift-5.5.1-RELEASE-ubuntu20.04.tar.gz
tar xzf swift-5.5.1-RELEASE-ubuntu20.04.tar.gz

# add the DEBIAN directory and control file
# the directory structure should be as follows
swift-5.5.1-RELEASE-ubuntu20.04
  ├ DEBIAN
  │  └ control
  └ usr
     └ ... swift toolchain
```
#### Build DEB Package
Build the test package is using `fakeroot` and `dpkg-deb` tools.
```bash
# build DEB package from toolchain directory
fakeroot dpkg-deb --build swift-5.5.1-RELEASE-ubuntu20.04/
 dpkg-deb: building package 'swiftlang' in 'swift-5.5.1-RELEASE-ubuntu20.04.deb'.

```
#### Testing Package With Lintian

```bash
lintian -i -v swift-5.5.1-RELEASE-ubuntu20.04.deb
```
```swift
// partial output of test results
N: Using profile ubuntu/main.
N: Starting on group swiftlang/5.5.1
N: Unpacking packages in group swiftlang/5.5.1
N: Finished processing group swiftlang/5.5.1
N: ----
N: Processing binary package swiftlang
N: (version 5.5.1, arch amd64) ...
E: swiftlang: binary-or-shlib-defines-rpath usr/bin/lldb /home/build-user/build/buildbot_linux/llvm-linux-x86_64/lib
N: 
N:    The binary or shared library sets RPATH or RUNPATH. This overrides the
N:    normal library search path, possibly interfering with local policy and
N:    causing problems for multilib, among other issues.
N:    
N:    The only time a binary or shared library in a Debian package should set
N:    RPATH or RUNPATH is if it is linked to private shared libraries in the
N:    same package. In that case, place those private shared libraries in
N:    /usr/lib/<package>. Libraries used by binaries in other packages should
N:    be placed in /lib or /usr/lib as appropriate, with a proper SONAME, in
N:    which case RPATH/RUNPATH is unnecessary.
N:    
N:    To fix this problem, look for link lines like:
N:        gcc test.o -o test -Wl,--rpath,/usr/local/lib
N:    or
N:        gcc test.o -o test -R/usr/local/lib
N:    and remove the -Wl,--rpath or -R argument. You can also use the chrpath
N:    utility to remove the RPATH.
N:    
N:    Refer to https://wiki.debian.org/RpathIssue for details.
N:    
N:    Severity: error
N:    
N:    Check: binaries
N: 
E: swiftlang: binary-or-shlib-defines-rpath usr/bin/lldb-argdumper /home/build-user/build/buildbot_linux/llvm-linux-x86_64/lib
E: swiftlang: binary-or-shlib-defines-rpath usr/bin/lldb-server /home/build-user/build/buildbot_linux/llvm-linux-x86_64/lib
E: swiftlang: binary-or-shlib-defines-rpath usr/bin/plutil /home/build-user/build/buildbot_linux/swift-linux-x86_64/lib/swift/linux
E: swiftlang: binary-or-shlib-defines-rpath usr/lib/clang/10.0.0/lib/linux/libclang_rt.memprof-x86_64.so /home/build-user/build/buildbot_linux/llvm-linux-x86_64/lib
E: swiftlang: binary-or-shlib-defines-rpath usr/lib/clang/10.0.0/lib/linux/libclang_rt.ubsan_minimal-x86_64.so /home/build-user/build/buildbot_linux/llvm-linux-x86_64/lib
E: swiftlang: binary-or-shlib-defines-rpath usr/lib/liblldb.so.10.0.0git /home/build-user/build/buildbot_linux/llvm-linux-x86_64/lib
E: swiftlang: changelog-file-missing-in-native-package
```
This initial testing shows there are a number errors and warnings that will need to be addressed before `swiftlang` packages can be accepted into the official Debian/Ubuntu repositories.

For full details of the test output please refer to this [Swift Bug Report SR-15515](https://bugs.swift.org/browse/SR-15515)