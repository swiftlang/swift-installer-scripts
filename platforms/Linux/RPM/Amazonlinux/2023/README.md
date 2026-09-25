# Building Swift on Amazon Linux 2023

## Install docker 

```sh
sudo dnf install docker -y
sudo usermod -a -G docker ec2-user
sudo newgrp docker
sudo systemctl enable docker.service
sudo systemctl start docker.service
```

## Install docker compose v2 

```sh
DOCKER_CONFIG=${DOCKER_CONFIG:-$HOME/.docker}
mkdir -p $DOCKER_CONFIG/cli-plugins
curl -SL https://github.com/docker/compose/releases/download/v2.24.5/docker-compose-linux-x86_64 -o $DOCKER_CONFIG/cli-plugins/docker-compose
chmod +x $DOCKER_CONFIG/cli-plugins/docker-compose
```

## Select the swift version to build

Create a symlink `swiftlang.spec -> swiftlang-VERSION.spec`

For example, to build Swift 5.9 :

```sh
SWIFT_VERSION=5.9
rm swiftlang.spec
ln -s swiftlang-${SWIFT)_VERSION}.spec swiftlang.spec
```

## Building with docker-compose

### run the build end-to-end

```sh
docker compose -f docker-compose-${SWIFT_VERSION}.yaml run build
```

### enter the docker env in shell mode

```sh
docker compose -f docker-compose-${SWIFT_VERSION}.yaml run shell
```

then you can run `./build_rpm.sh` to run the build manually inside the docker

### rebuild the base image

```sh
docker compose -f docker-compose-${SWIFT_VERSION}.yaml build --pull
```

note this still uses the docker cache, so will rebuild only if the version of the underlying base image changed upstream

### Open Issues / TODO
* the list of build requirements (BuildRequires) and especially requirements (Requires) should come from an external file, likely one per swift release version (which we can use it to also drive documentation)
