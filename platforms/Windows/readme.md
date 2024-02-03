# 🚀 Swift Windows Installers

## 📦 Online and Offline Bundles

The Swift SDK bundle is now available in two flavors:

- **Online**: A small .exe that downloads the selected packages at install time. Requires an Internet connection but downloads fewer bytes.
- **Offline**: A larger .exe containing all packages, extracted on-demand at install time. Downloads more bytes but works without an Internet connection.

A third pseudo-flavor is using the online bundle with the `/layout` switch to create an offline-install-ready layout on a network or USB drive.

### 🌐 Download URLs

For the online bundle, provide a download URL for each package and its cabinet files (`*.cab`). `BaseReleaseDownloadUrl` is a preprocessor variable providing the URL to the package directory.

Burn supports `{2}` as a placeholder in download URLs, replaced by the payload's file name.

### 📁 Install Directory and Feature Selection

User-configurable variables control install directory and feature selection. These are set through the command line or bundle UI:

| Variable | Description |
| -------- | ----------- |
| InstallRoot | Installation root directory. |
| OptionsInstallBld | Install bld.msi. |
| OptionsInstallCli | Install cli.msi. |
| OptionsInstallDbg | Install dbg.msi. |
| OptionsInstallIde | Install ide.msi. |
| OptionsInstallRtl | Install rtl.msi. |
| OptionsInstallSdkX86 | Install x86 SDK. |
| OptionsInstallSdkAMD64 | Install AMD64 SDK. |
| OptionsInstallSdkArm64 | Install Arm64 SDK. |

These variables are linked to controls in the bundle theme (`installer\theme.xml`). For example:

```xml
<Editbox Name="InstallRoot" X="185" Y="46" Width="-91" Height="21" TabStop="yes" FontId="3" FileSystemAutoComplete="yes" />
```

Feature selection controls are checkboxes tied to the corresponding variables.

Variables control package installation using the `InstallCondition` attribute:

```xml
<MsiPackage
  SourceFile="!(bindpath.ide)\ide.msi"
  InstallCondition="OptionsInstallIde"
  DownloadUrl="$(BaseReleaseDownloadUrl)/{2}">
```

### 📦 Bundle Command Line

Burn bundles support a common set of command-line switches, mirroring [Windows Installer options](https://learn.microsoft.com/en-us/windows/win32/msi/standard-installer-command-line-options). The Swift bundle theme also implements a standard Help dialog when run with a `/?` switch.

For example, to uninstall the bundle showing UI but not requiring user interaction:

```sh
installer.exe /passive /uninstall
```

The same variables for install directory and feature selection are available from the command line too:

```sh
installer.exe /passive InstallRoot=X:\Swift OptionsInstallIde=0 OptionsInstallSdkX86=0 OptionsInstallSdkArm64=0
```

To create an offline install layout from an online bundle:

```sh
installer.exe /layout path\to\layout\directory
```

## 🛠️ MSBuild Projects

### Directory.Build.props

MSBuild automatically imports Directory.Build.props files. It centralizes shared MSBuild properties for .wixproj projects. Key properties include:

| Property | Description |
| -------- | ----------- |
| MajorMinorProductVersion | `major.minor` fields of `ProductVersion`. Used for side-by-side upgrade strategy. |
| BaseIntermediateOutputPath, OutputPath | Intermediate and build output directories. Supports MSBuild/NuGet restore and multiplatform output. |
| SuppressIces | Suppresses ICE validation messages for per-user packages. |
| PackageCompressionLevel, BundleCompressionLevel | Sets compression levels for packages and bundles. |
| ArePackageCabsEmbedded | Keeps .cab files external to .msi files. |
| BundleFlavor, IsBundleCompressed | Controls bundle flavor (online/offline) and compression. |
| DefineConstants | Passes MSBuild properties into WiX build as preprocessor variables. |
| INCLUDE_SWIFT_INSPECT | Conditional inclusion of swift-inspect. |
| INCLUDE_X86_SDK, INCLUDE_ARM64_SDK | Conditional inclusion of x86 and Arm64 SDKs. |

### User.props

Also imported is a user-specific .props file:

```xml
<Import Project="$(USERNAME).props" Condition="Exists('$(USERNAME).props')" />
```

Override settings in Directory.Build.props for local dev builds. For example, to increase build speed:

```xml
<Project>
  <PropertyGroup>
    <PackageCompressionLevel>none</PackageCompressionLevel>
    <BundleCompressionLevel>none</BundleCompressionLevel>
  </PropertyGroup>
</Project>
```

## 🛠️ Building the Installers

Use `ProjectReference`s between .wixproj projects for correct dependencies and build order. Build the bundle, and it automatically builds all MSI packages.

To support SDK and RTL MSI packages' three architecture flavors, pass in architecture-specific paths as MSBuild properties.

```sh
msbuild %SourceRoot%\swift-installer-scripts\platforms\Windows\bundle\installer.wixproj ^
  -m ^
  -restore ^
  -p:BundleFlavor=online ^
  -p:BaseReleaseDownloadUrl=todo://base/release/download/url ^
  -p:Configuration=Release ^
  -p:BaseOutputPath=%Package

Root%\online\ ^
  -p:DEVTOOLS_ROOT=%BuildRoot%\Library\Developer\Toolchains\unknown-Asserts-development.xctoolchain ^
  -p:TOOLCHAIN_ROOT=%BuildRoot%\Library\Developer\Toolchains\unknown-Asserts-development.xctoolchain ^
  -p:PLATFORM_ROOT_X86=path\to\x86\platform ^
  -p:PLATFORM_ROOT_AMD64=path\to\amd64\platform ^
  -p:PLATFORM_ROOT_ARM64=path\to\arm64\platform ^
  -p:SDK_ROOT_X86=path\to\x86\sdk ^
  -p:SDK_ROOT_AMD64=path\to\amd64\sdk ^
  -p:SDK_ROOT_ARM64=path\to\arm64\sdk
```

## 🔄 Side-by-Side Upgrade Strategy

Swift's side-by-side upgrade strategy retains the latest patch release of each major.minor release. For example:

- 5.9.0
- 5.9.1
- 5.9.2
- 5.10.0
- 5.10.1
- 5.10.5
- 5.11.2
- 6.0.0
- 6.0.1
- 6.5.1
- 6.5.7

Results in:

- ~~5.9.0~~
- ~~5.9.1~~
- **5.9.2**
- ~~5.10.0~~
- ~~5.10.1~~
- **5.10.5**
- **5.11.2**
- ~~6.0.0~~
- **6.0.1**
- ~~6.5.1~~
- **6.5.7**

### SideBySideUpgradeStrategy.props

Imported by Directory.Build.props to bring in the GUIDs used for side-by-side upgrades.

GUIDs are substituted at bind time to skip validation and cleanup done by the compiler:

- All uppercase
- Surrounded by curly braces

| Property | Description |
| -------- | ----------- |
| BldUpgradeCode, CliUpgradeCode, DbgUpgradeCode, IdeUpgradeCode, RtlUpgradeCode, SdkUpgradeCode | Upgrade codes for individual packages. |
| BundleUpgradeCode | Upgrade codes for the bundle. Bundles don't support upgrade version ranges, so the code must change for every minor version _and_ stay the same for the entire lifetime of that minor version.

### shared\shared.wxs

To support side-by-side installation, use "old-school" `Upgrade`/`UpgradeVersion` authoring. The upgrade logic is authored in `shared\shared.wxs` and referenced from the `Package` element of each package:

```xml
<WixVariable Id="SideBySidePackageUpgradeCode" Value="$(SdkUpgradeCode)" />
<FeatureGroupRef Id="SideBySideUpgradeStrategy" />
```

`SideBySidePackageUpgradeCode` is a bind-time variable used to pass the upgrade code as an argument to the fragment in `shared\shared.wxs`.

### Cleaning up

Windows Installer may leave behind empty folders. Directory.Build.props suppresses ICE validation messages, and `shared\shared.wxs` handles cleaning up orphaned, empty directories.

### Compression Levels, Build Time, and Download Size

As expected, `high` compression builds the smallest payloads but with longer build times. The authoring uses the older `Media` element to ensure a single .cab for maximal compression.

### Getting Files without Installing

Windows Installer supports [administrative installations](https://learn.microsoft.com/en-us/windows/win32/msi/administrative-installation) to retrieve files without installing:

```sh
msiexec /qb /a build\amd64\bld.msi TARGETDIR=X:\swift.bld.admin
```

### Testing in Windows Sandbox

Windows Sandbox is a convenient virtualization feature for testing. The files in `samples\tests\SxS` are used for testing. The `RunSxSTests.cmd` script runs side-by-side test bundles in the Sandbox VM.

## 🧪 Samples

`HelloMergeModule` is a sample WiX project that installs a Swift-built app and the appropriate RTL merge module. To build it:

```sh
msbuild -Restore -p:RedistributablesDirectory=X:\Swift\Redistributables\0.0.0 -p:Platform=Arm64 hellomm.wixproj
```
