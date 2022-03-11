# Building Swift on Fedora Linux


### building with docker-compose

* to run the build end-to-end

```
docker-compose run build
```

* to enter the docker env in shell mode

```
docker-compose run shell
```

then you can run `./build_rpm.sh` to run the build manually inside the docker


* to rebuild the base image

```
docker-compose build --pull
```

note this still uses the docker cache, so will rebuild only if the version of the underlying base image changed upstream


### Open Issues / TODO
* the list of build requirements (BuildRequires) and especially requirements (Requires) should come from an external file, likely one per swift release version (which we can use it to also drive documentation)
