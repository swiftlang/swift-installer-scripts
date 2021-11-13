# Swift Installer Scripts

This repository contains all the supporting files required for building
toolchain packages for the Swift toolchain for distribution.

This repository does not contain the actual contents of the toolchain. These
files are used to construct the packaged forms of the toolchain to layout the
toolchain properly on the destination system.

## Organization

Because the repository hosts the packaging support content for multiple
platforms, the following structure allows all the platforms to colocate
in the same repository without colliding with each other:

~~~
swift-installer-scripts
  └ platforms
        ├ Linux
        │   ├ README
        │   ├ DEB
        │   │   ├ README
        │   │   ├ Docs - contains information specific to .deb packages
        │   │   ├ Debian - contains information specific to the distribution
        │   │   │   ├ buster - contains information specific to the version
        │   │   │   ├ bullseye
        │   │   │   └ ...
        │   │   ├ Ubuntu
        │   │   │   ├ bionic
        │   │   │   ├ focal
        │   │   │   ├ hirsute
        │   │   │   └ ...
        │   │   └ ...
        │   │
        │   ├ RPM
        │   │   ├ README
        │   │   ├ Docs - contains information specific to .rpm packages
        │   │   ├ Amazonlinux
        │   │   │   ├ 2
        │   │   │   └ ...
        │   │   ├ Centos
        │   │   │   ├ 7
        │   │   │   ├ 8
        │   │   │   └ ...
        │   │   ├ Fedora
        │   │   │   ├ 33
        │   │   │   ├ 34
        │   │   │   ├ rawhide
        │   │   │   └ ...
        │   │   └ ...
        │   └ ...
        └ Windows
            └ ...
~~~

## Linux Packages (RPM/Deb)

Currently Swift on Linux is distributed via tarball and Docker, and
we would like to start supporting RPM and Debs officially on swift.org.
The goal is to provide a seamless install process for Swift on Linux by
utilizing the platform’s native package manager (RPM/Deb).


* Step 1. Develop native packages / installers for the distributions
* Step 2. Offer the native packages / installers through swift.org
  * Support all officially supported Linux platforms
  * Code signed by swift.org certificate
  * Repository hosted on swift.org
* Step 3. Offer the native packages / installer through official repositories
for the various platforms
  * Work with official repositories to accept package specs
  * Deprecate swift.org packages / installer repository
* Step 4. Deprecate swift.org Linux tarballs

### Package Info

* Package name: swiftlang
* License: Apache 2.0
* Maintainer: swift-infrastructure@forums.swift.org
* URL: https://swift.org
* Description:
```
Swift is a general-purpose programming language built using
a modern approach to safety, performance, and software design
patterns.

The goal of the Swift project is to create the best available
language for uses ranging from systems programming, to mobile
and desktop apps, scaling up to cloud services. Most
importantly, Swift is designed to make writing and maintaining
correct programs easier for the developer.
```

### RPM Naming Convention:

Package naming convention: `swiftlang-<VERSION>-<RELEASE>.<DIST>.<ARCH>.rpm`  
Package structure: `/repo/<OS>/releases/<OS_VERSION>/<ARCH>/`  
Repository configuration: `/repo/<OS>/releases/<OS_VERSION>/swiftlang.repo`  


#### Example

* **Package structure:**  
```
/repo/centos/releases/8/x86_64/swiftlang-5.5.0-1.el8.x86_64.rpm
/repo/centos/releases/8/aarch64/swiftlang-5.5.0-1.el8.aarch64.rpm
```

* **Package URL:**  
https://download.swift.org/repo/centos/releases/8/aarch64/swiftlang-5.5.0-1.el8.aarch64.rpm

* **Repository configuration file URL:**  
https://download.swift.org/repo/centos/releases/8/swiftlang.repo

## Tasks

### RPM Package Manager (RPM)*

- [ ] [SR-15325](https://bugs.swift.org/browse/SR-15325) Create RPM spec file
- [ ] [SR-15326](https://bugs.swift.org/browse/SR-15326) Setup CI job to build the rpm package
- [ ] [SR-15327](https://bugs.swift.org/browse/SR-15327) Code sign rpm package with swift.org certificate
- [ ] [SR-15328](https://bugs.swift.org/browse/SR-15328) Host the rpm package on swift.org
- [ ] [SR-15329](https://bugs.swift.org/browse/SR-15329) Host the rpm repository on swift.org
- [ ] [SR-15330](https://bugs.swift.org/browse/SR-15330) Verify the rpm package and repository
- [ ] [SR-15331](https://bugs.swift.org/browse/SR-15331) Update swift.org download / install page
- [ ] [SR-15332](https://bugs.swift.org/browse/SR-15332) Work with official repositories to accept package specs

*For each platform, we will start with CentOS 8.

### Debian Package (Deb)*

- [ ] [SR-15334](https://bugs.swift.org/browse/SR-15334) Create Debs control file
- [ ] [SR-15335](https://bugs.swift.org/browse/SR-15335) Setup CI to build the deb package
- [ ] [SR-15336](https://bugs.swift.org/browse/SR-15336) Code sign package with swift.org certificate
- [ ] [SR-15337](https://bugs.swift.org/browse/SR-15337) Host the deb package on swift.org
- [ ] [SR-15338](https://bugs.swift.org/browse/SR-15338) Host the deb repository on swift.org
- [ ] [SR-15339](https://bugs.swift.org/browse/SR-15339) Verify the deb package and repository
- [ ] [SR-15340](https://bugs.swift.org/browse/SR-15340) Update swift.org download / install page
- [ ] [SR-15341](https://bugs.swift.org/browse/SR-15341) Work with official repositories to accept package control files

*For each platform, we will start with Ubuntu 20.04

## Open Questions

* Where should swiftlang be installed on the system?
	* Option 1: Diverge the install location between platform to best fit the platform requirements.
		* symlink the toolchain into /usr/ to avoid conflicting with llvm.org binaries.
	* Option 2: Install in /usr and rename llvm-project binaries (example: swift-lldb/lldb-swift ...)
	* [GitHub discussion](https://github.com/apple/swift-installer-scripts/pull/37#discussion_r726707320)
* Should we support multiple swiftlang versions on the system?
* Multiple packages:
	* swiftlang
	* swiftlang-runtime

## Contributing

Before contributing, please read [CONTRIBUTING.md](CONTRIBUTING.md).

## LICENSE

See [LICENSE](LICENSE.txt) for license information.

## Code of Conduct

See [Swift.org Code of Conduct](https://swift.org/code-of-conduct/) for Code of Conduct information.
