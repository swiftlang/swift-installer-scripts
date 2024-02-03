# Swift Installer Scripts 🚀

This repository houses essential files for crafting toolchain packages for the Swift toolchain, ensuring seamless distribution.

📁 **Organization**
```
swift-installer-scripts
  └ platforms
        ├ Linux
        │   ├ README
        │   ├ DEB
        │   │   ├ README
        │   │   ├ Docs - contains info specific to .deb packages
        │   │   ├ Debian - contains info specific to the distribution
        │   │   │   ├ buster - info specific to the version
        │   │   │   ├ bullseye
        │   │   │   └ ...
        │   │   ├ Ubuntu
        │   │   │   ├ bionic
        │   │   │   ├ focal
        │   │   │   ├ hirsute
        │   │   │   └ ...
        │   │   └ ...
        │   ├ RPM
        │   │   ├ README
        │   │   ├ Docs - contains info specific to .rpm packages
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
```

## Linux Packages (RPM/Deb) 🐧

Swift on Linux is currently distributed via tarball and Docker. The aim is to officially support RPM and Deb on swift.org.

1. **Develop Native Packages / Installers**
2. **Offer Native Packages / Installers on swift.org**
   - Support all officially supported Linux platforms
   - Code signed by swift.org certificate
   - Repository hosted on swift.org
3. **Offer Packages / Installers Through Official Repositories**
   - Work with official repositories to accept package specs
   - Deprecate swift.org packages / installer repository
4. **Deprecate swift.org Linux Tarballs**

### Package Info 📦

- **Package name:** swiftlang
- **License:** Apache 2.0
- **Maintainer:** swift-infrastructure@forums.swift.org
- **URL:** [Swift.org](https://swift.org)
- **Description:**
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

### RPM Naming Convention 📛

- Package naming convention: `swiftlang-<VERSION>-<RELEASE>.<DIST>.<ARCH>.rpm`
- Package structure: `/repo/<OS>/releases/<OS_VERSION>/<ARCH>/`
- Repository configuration: `/repo/<OS>/releases/<OS_VERSION>/swiftlang.repo`

#### Example 🌐

- **Package structure:**
  ```
  /repo/centos/releases/8/x86_64/swiftlang-5.5.0-1.el8.x86_64.rpm
  /repo/centos/releases/8/aarch64/swiftlang-5.5.0-1.el8.aarch64.rpm
  ```

- **Package URL:**
  [Download Swiftlang RPM](https://download.swift.org/repo/centos/releases/8/aarch64/swiftlang-5.5.0-1.el8.aarch64.rpm)

- **Repository configuration file URL:**
  [Swiftlang Repo Config](https://download.swift.org/repo/centos/releases/8/swiftlang.repo)

## Tasks 📋

### RPM Package Manager (RPM) 🧰

- [ ] [SR-15325](https://bugs.swift.org/browse/SR-15325) Create RPM spec file
- [ ] [SR-15326](https://bugs.swift.org/browse/SR-15326) Setup CI job to build the RPM package
- [ ] [SR-15327](https://bugs.swift.org/browse/SR-15327) Code sign RPM package with swift.org certificate
- [ ] [SR-15328](https://bugs.swift.org/browse/SR-15328) Host the RPM package on swift.org
- [ ] [SR-15329](https://bugs.swift.org/browse/SR-15329) Host the RPM repository on swift.org
- [ ] [SR-15330](https://bugs.swift.org/browse/SR-15330) Verify the RPM package and repository
- [ ] [SR-15331](https://bugs.swift.org/browse/SR-15331) Update swift.org download / install page
- [ ] [SR-15332](https://bugs.swift.org/browse/SR-15332) Work with official repositories to accept package specs

*Starting with CentOS 8 for each platform.*

### Debian Package (Deb) 📦

- [ ] [SR-15334](https://bugs.swift.org/browse/SR-15334) Create Deb control file
- [ ] [SR-15335](https://bugs.swift.org/browse/SR-15335) Setup CI to build the Deb package
- [ ] [SR-15336](https://bugs.swift.org/browse/SR-15336) Code sign package with swift.org certificate
- [ ] [SR-15337](https://bugs.swift.org/browse/SR-15337) Host the Deb package on swift.org
- [ ] [SR-15338](https://bugs.swift.org/browse/SR-15338) Host the Deb repository on swift.org
- [ ] [SR-15339](https://bugs.swift.org/browse/SR-15339) Verify the Deb package

 and repository
- [ ] [SR-15340](https://bugs.swift.org/browse/SR-15340) Update swift.org download / install page
- [ ] [SR-15341](https://bugs.swift.org/browse/SR-15341) Work with official repositories to accept package control files

*Starting with Ubuntu 20.04*

## Open Questions ❓

- Where should swiftlang be installed on the system?
  - Option 1: Diverge the install location between platforms to best fit the platform requirements.
  - Option 2: Install in /usr and rename llvm-project binaries (example: swift-lldb/lldb-swift...)
  - [GitHub discussion](https://github.com/apple/swift-installer-scripts/pull/37#discussion_r726707320)
- Should we support multiple swiftlang versions on the system?
- Multiple packages:
  - swiftlang
  - swiftlang-runtime

## Contributing 🤝

Before contributing, please read [CONTRIBUTING.md](CONTRIBUTING.md).

## LICENSE 📄

See [LICENSE](LICENSE.txt) for license information.

## Code of Conduct 📝

See [Swift.org Code of Conduct](https://swift.org/code-of-conduct/) for Code of Conduct information.
