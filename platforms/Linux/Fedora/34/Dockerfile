FROM fedora:34
LABEL PURPOSE="This image is configured to build Swift for the version of Fedora listed above"

WORKDIR /root

RUN mkdir -p /root/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}
ADD swift-lang.spec /root/rpmbuild/SPECS/swift-lang.spec
ADD *.patch /root/rpmbuild/SOURCES/

ADD build_swift_rpm.sh /root/build_swift_rpm.sh
RUN chmod +x /root/build_swift_rpm.sh

RUN dnf -y update

# Required for the "rpmbuild" command
RUN dnf install -y fedora-packager fedora-review

# Install all the dependencies needed to build Swift from the
# spec file itself
RUN dnf builddep -y /root/rpmbuild/SPECS/swift-lang.spec
# And now get the sources for Swift as defined in the spec file
RUN spectool -g -R /root/rpmbuild/SPECS/swift-lang.spec

CMD ["/root/build_swift_rpm.sh"]
