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

## Contributing

Before contributing, please read [our main guide](https://www.swift.org/contributing)

## LICENSE

See [LICENSE](LICENSE.txt) for license information.

## Code of Conduct

See [Swift.org Code of Conduct](https://swift.org/code-of-conduct/) for Code of Conduct information.
