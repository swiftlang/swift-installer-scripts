# Swift Installer Scripts

This repository contains all the supporting files required for building
toolchain packages for the Swift toolchain for distribution.

This repository does not contain the actual contents of the toolchain. These
files are used to contruct the packaged forms of the toolchain to layout the
toolchain properly on the destination system.

## Organization

Because the repository hosts the packaging support content for multiple
platforms, the following structure allows all the platforms to colocate
in the same repository without colliding with each other:

~~~
swift-installer-scripts
  └ platforms
      ├ Linux
      │   ├ Ubuntu
      │   │  └ ...
      │   └ CentOS
      │      └ ...
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

## Tasks

### RPM Package Manager (RPM)*

- [ ] SR-### Create RPM spec file
- [ ] SR-### Setup CI job to build the rpm package
- [ ] SR-### Code sign rpm package with swift.org certificate
- [ ] SR-### Host the rpm package on swift.org
- [ ] SR-### Host the rpm repository on swift.org
- [ ] SR-### Verify the rpm package and repository
- [ ] SR-### Update swift.org download / install page
- [ ] SR-### Work with official repositories to accept package specs

*For each platform, we will start with CentOS 8.

### Debian Package (Deb)*

- [ ] SR-### Create Debs control file
- [ ] SR-### Setup CI to build the deb package
- [ ] SR-### Code sign package with swift.org certificate
- [ ] SR-### Host the deb package on swift.org
- [ ] SR-### Host the deb repository on swift.org
- [ ] SR-### Verify the deb package and repository
- [ ] SR-### Update swift.org download / install page
- [ ] SR-### Work with official repositories to accept package specs

*For each platform, we will start with Ubuntu 20.04

## Contributing

Before contributing, please read [CONTRIBUTING.md](CONTRIBUTING.md).

## LICENSE

See [LICENSE](LICENSE.txt) for license information.

## Code of Conduct

See [Swift.org Code of Conduct](https://swift.org/code-of-conduct/) for Code of Conduct information.
