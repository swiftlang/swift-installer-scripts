# Official Debian Packaging Policy
*The purpose of this document is to outline what is required to have the `Swift Toolchain` packages accepted into the official Debian Repositories.*

#### Note: About Official Ubuntu Repositories.
For a package to be accepted into the official `Ubuntu` repositories first the package must be accepted and approved for Debian. Packages are then synced from Debian to the Ubuntu repositories.

### Intent To Package
The first step in getting a package approved is to file an `ITP` (Intent To Package).
An `ITP` is a special bug report saying that you want to package some product.
You file an `ITP` by using the reportbug tool and specifying “wnpp” as the package you want to report a bug against.

*NOTE:- an `ITP` for `swiftlang` was filed in 2015 [#788327](https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=788327). It doesn't appear anything other than filing the `ITP` was done.*

### Sponsorship
Unless you are an accepted Debian Developer you cannot upload to the Debian Repository directly. You have to package your product and then ask for a `sponsor` on the `debian-mentors mailing list`. The `sponser` will check your packages and decide whether they are ready to be accepted into Debian. The `sponser` will then upload the packages and add them to Debian.
Once it is proven that you are able to create proper new packages and that you are willing to maintain them, you can then become a Debian developer.

### Debian Developer
For more information about being a Debian Developer please read the [Debian Developer's Reference](http://www.debian.org/doc/developers-reference/)

### Debian Distributions Timeline
Debian is divided into three distributions: `stable, testing and unstable`. Whenever a new package is added, or an existing package is updated, it goes into `unstable`. Once it has been in `unstable` for ten days without revealing serious bugs, it automatically moves into `testing`. When the release manager decides it's time for a new release, he declares the `testing` distribution as `frozen`. This means that no new packages can be added, and no existing ones may be updated. Only outstanding bugs may be fixed. Once he thinks that `testing` is ready to be released, it becomes `stable` and a new `testing` distribution is added.

### Package Testing
New packages need to be thoroughly tested for installation and errors before they can be uploaded for inclusion.

This should be done with sequences such as the following:
* install the previous version (if needed).
* upgrade it from the previous version.
* downgrade it back to the previous version (optional).
* purge it.
* install the new package.
* remove it.
* install it again.
* purge it.

#### Testing With Lintian
`lintian` is one of the main tools used to test DEB packages for errors.

*Lintian dissects Debian packages and reports bugs and policy violations.  It contains automated checks for many aspects of Debian policy as well as some checks for common errors.*

See here for [Testing a Sample Swift DEB Package with Lintian](sample_package.md)

### Reference Documents

* [Debian Policy Manual](https://www.debian.org/doc/debian-policy/)
* [Debian New Maintainers' Guide](https://www.debian.org/doc/manuals/maint-guide/)
* [How Software Producers Can Distribute Their Products Directly in DEB Format](https://www.debian.org/doc/manuals/distribute-deb/distribute-deb.html)
* [Debian Mailing Lists](https://www.debian.org/MailingLists/)
* [Debian New Members Corner](https://www.debian.org/devel/join/newmaint)
* [Debian bug tracking system](https://www.debian.org/Bugs/)
