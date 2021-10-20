 
# Building Swift on Fedora Linux

## Introduction
Fedora uses the [RPM package format](https://en.wikipedia.org/wiki/RPM_Package_Manager)
to install software packages. The Swift RPM package can be built either by creating
a Linux container image or manually on a computer running Fedora.

## Directories
There are separate directories for each version of Fedora. The
instructions below are applicable to all versions.
### Rawhide
Rawhide is always the very latest version of Fedora 
and is considered to be permanently in a beta, or even alpha, state.
Rawhide will typically have beta versions of other software (e.g., Clang),
so it is not unusual for there to be issues building Swift. 

## Files
### `build_rpm.sh`
Builds the Swift RPM within a running container.
### `Dockerfile`
Builds the container image
### `patches/*.patch`
Any post-release patches that have not yet been merged upstream that are 
temporarily necessary to build Swift. 
### `swiftlang.spec`
The "recipe" the RPM packaging tools use to build Swift. 

## Building Swift Via Linux Containers
To build swift using containers, there are two steps: preparing the container image,
and running the container.
### Preparing the Container Image
**Note**: These instructions assume you have either Docker or `podman` with
`podman-docker` and `podman-compose` installed. 
1. Run "`docker build -t swift-builder-fedora-`**version**`:5.5 .`", replacing 
**version** with the version of Fedora in the `Dockerfile`. This will create
a new image with the name `swift-builder-fedora-`**version** and the tag `5.5`. 
Note that if using Docker, it may be necessary to prepend the command with `sudo`. 
2. Run "`docker run -d -v$PWD:/out:Z swift-builder-fedora-`**version**`:5.5`". 
The container will be created and automatically run `build_rpm.sh`. After the 
container finishes building Swift, the artifacts will be placed in the current 
directory, along  with `build-output.txt` which can be used for troubleshooting 
any build issues.

## Building Swift Locally
Building Swift locally requires additional software to be installed in the local
Fedora environment, requiring `sudo` permissions.
### Installing the RPM tools on the local machine
The following commands must be run to install the necessary software to build Swift
locally
1. `sudo dnf -y update`
2. `sudo dnf install -y fedora-packager fedora-review`
### Preparing to build Swift
RPMs are built in the `$HOME/rpmbuild` directory, which has several subdirectories
for specific purposes.
1. `mkdir -p $HOME/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}`
2. `cp swiftlang.spec $HOME/rpmbuild/SPECS`
3. `cp *.patch $HOME/rpmbuild/SOURCES`
#### Install all the dependencies necessary to build Swift:
4. `sudo dnf builddep -y $HOME/rpmbuild/SPECS/swiftlang.spec`
#### Download the Swift source tarballs:
5. `spectool -g -R $HOME/rpmbuild/SPECS/swiftlang.spec`
### Building Swift
The RPM tools perform both building and packaging. _This process may take several
hours depending on the hardware Fedora is running on._

`rpmbuild -ba $HOME/rpmbuild/SPECS/swiftlang.spec 2>&1 | tee $HOME/build-output.txt`

If there were no errors, the resulting Swift RPM file will be located in 
`$HOME/rpmbuild/RPMS/` in a subdirectory for the current processor architecture 
(e.g., `x86_64`). 

## Installing Swift
### Name format
If Swift was built successfully there should
be a file with the name format similar to `swiftlang-5.5-1.fc34.x86_64.rpm` where
`5.5` is the version of Swift specified in `swiftlang.spec`, `fc34` is the version
of Fedora that was used to build Swift, and `x86_64` is the processor architecture.

### Note about Fedora versions
The Fedora version _must_ be the same as the one in the RPM file. In other words, 
a Swift RPM built for Fedora 34 can only be installed on Fedora 34.

### Installation
To install the Swift RPM, change to the directory that contains the RPM file and
run `sudo dnf install` _the Swift RPM file_ e.g.

`sudo dnf install ./swiftlang-5.5-1.fc34.x86_64.rpm`


