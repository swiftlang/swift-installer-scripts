FROM fedora:34
LABEL PURPOSE="This image is configured to build Swift for the version of Fedora listed above"

WORKDIR /root

RUN yum -y update

# RPM and yum development tools
RUN yum install -y rpmdevtools yum-utils

# Add the spec
RUN mkdir -p /root/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}
ADD swift-lang.spec /root/rpmbuild/SPECS/swift-lang.spec


# Install all the dependencies needed to build Swift from the spec file itself
RUN yum-builddep -y /root/rpmbuild/SPECS/swift-lang.spec

# Get the sources for Swift as defined in the spec file
RUN spectool -g -R /root/rpmbuild/SPECS/swift-lang.spec

# Add the patches
ADD patches/*.patch /root/rpmbuild/SOURCES/

# Add the driver script
ADD build_rpm.sh /root/build_rpm.sh
RUN chmod +x /root/build_rpm.sh


CMD ["/root/build_rpm.sh"]
