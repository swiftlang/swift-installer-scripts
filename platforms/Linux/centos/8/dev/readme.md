# Building Swift on CentOS Linux


### building with docker


Build the builder docker image, this will download the sources

```
docker build . -t rpm-builder
```

Run the builder, this will run the build

```
docker run -v `pwd`/.out:/out rpm-builder
```


Open Issues / Introduction
* the swift release version should be an argument
* the versions of source packages are no pinned to the swift release version (eg yams) should come from an external file, likely one per swift release version
* the list of build requirements (BuildRequires) and especially requirements (Requires) should come from an external file, likely one per swift release version (which we can use it to also drive documentation)
