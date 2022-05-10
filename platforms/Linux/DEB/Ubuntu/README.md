## Ubuntu

Ubuntu uses the [Deb package format]() to install software packages.
The Swift Deb package can be built either by creating a Linux container image or manually on a computer running Ubuntu.

There are separate directories for each version of Ubuntu.
The instructions below are applicable to all versions.

## Important file and directories

**build_deb.sh**
Driver file to build the deb package

**build_source.sh**
Driver file to build the deb source package (which is a step in `build_deb`)

**patches/*.patch**
Any post-release patches that have not yet been merged upstream that are temporarily necessary to build Swift.

**control.in**
Debian package metadata, including the `BuildDepends` and `Depends` definitions.

**changelog**
Debian package changelog

**rules**
Debian package recipe

**Dockerfile**
Defines the base docker image to run the install scripts in.

**docker-compose**
Defines docker compose tasks to drive the pacakge build in Docker.

## Importand file and directories

**Shared/version.sh**
Shell fragment versions.sh containing version information for all source components, and the Debian package upstream version (debversion).

**Shared/copyright**
Copyright information

### Building with docker-compose

* to run the build end-to-end

```
docker-compose run build
```

* to enter the docker env in shell mode

```
docker-compose run shell
```

then you can run `./build_deb.sh` to run the build manually inside the docker


* to rebuild the base image

```
docker-compose build --pull
```

note this still uses the docker cache, so will rebuild only if the version of the underlying base image changed upstream

### Building locally on an Ubuntu machine

1. Install required development tools (see Dockerfile)
2. Run `./build_deb.sh`
