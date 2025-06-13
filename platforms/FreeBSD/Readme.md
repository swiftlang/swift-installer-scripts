# FreeBSD Swift Package Builder Script

## Description

This shell script automates the creation of FreeBSD packages (.pkg files) for Swift programming language toolchains. It takes a compiled Swift toolchain directory as input and produces a properly structured FreeBSD package that can be installed using the `pkg` package manager.

## Summary

The script is specifically designed for FreeBSD systems and requires a properly structured Swift toolchain with a `usr/` subdirectory containing the Swift binaries and libraries.

The script performs the following key operations:

- Validation: Ensures the current platform is FreeBSD and validates command-line arguments
- Directory Setup: Creates temporary staging areas for package files and metadata
- Toolchain Processing: Copies the Swift toolchain to the appropriate directory structure (`/usr/local/swift`)
- Symlink Creation: Creates symbolic links in `/usr/local/bin` for `swift` and `swiftc` commands
- Package Manifest Generation: Creates a FreeBSD package manifest with metadata including version, description, and maintainer information
- File List Generation: Builds a complete list of files and symlinks to include in the package
- Package Creation: Uses FreeBSD's `pkg create` command to build the final package with Zstandard compression
- Cleanup: Removes temporary staging directories

The resulting package installs Swift to `/usr/local/swift` with convenient symlinks in `/usr/local/bin`, making Swift commands available in the standard path.

## Usage Examples

### Basic Usage

```
# Package a Swift 6.2 toolchain
./makePackage /home/user/swift-6.2-RELEASE-freebsd /usr/local/packages/swift-6.2.pkg
```

### Complete Workflow Example

```
# 1. Create the package
./makePackage ./swift-6.2-RELEASE-freebsd ./swift-6.2.pkg

# 2. Install the package (as root)
pkg install ./swift-6.2.pkg

# 3. Verify installation
swift --version
swiftc --help
```
