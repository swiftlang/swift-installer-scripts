# Windows installers

## Online and offline bundles

The Swift SDK bundle is now available in two flavors:

- **Online**: A small .exe that downloads the packages the user selected at install time. Can download fewer total bytes but must be connected to the Internet. The work to stage the payloads required for the online bundle is left as an exercise for the reader.
- **Offline**: A big .exe that contains all the packages and are extracted on-demand at install time. Downloads the most bytes but doesn't require live Internet connection.

A third pseudo-flavor is using the online bundle with the `/layout` switch to create a layout that can be placed on a network or USB drive to install offline.

### Download URLs

For the online bundle, we need to provide a download URL for each package and its cabinet files (`*.cab`). (The `SourceFile` attribute is also required, both for the offline bundle and for Burn to get the hash of the packages. Note that the packages and .cabs available for download must exactly match the ones used when building the bundle. Burn validates downloads by their hashes.)

(For the offline bundle, the download URLs are ignored so we can provide any string. That's easier than conditionalizing the `Chain` of packages and omitting the `DownloadUrl` attributes.)

`BaseReleaseDownloadUrl` is a preprocessor variable that provides the URL to the directory containing the packages for that bundle. So, for example, for the bundle at `https://download.swift.org/swift-5.8.1-release/windows10/swift-5.8.1-RELEASE/swift-5.8.1-RELEASE-windows10.exe`, `BaseReleaseDownloadUrl` would be `https://download.swift.org/swift-5.8.1-release/windows10/swift-5.8.1-RELEASE`.

Burn supports some [special syntax](https://wixtoolset.org/docs/schema/wxs/msipackage/) to simplify authoring download URLs. The one we use is `{2}`, which is replaced by the file name of the payload.

Burn automatically expands payload authoring to handle .cab files that are external to their .msi files so `DownloadUrl="$(BaseReleaseDownloadUrl)/{2}"` turns into, for example:

- `$(BaseReleaseDownloadUrl)/bld.msi`
- `$(BaseReleaseDownloadUrl)/bld.cab`


### Install directory and feature selection

The bundle authoring (in `installer.wxs`) drives optional install directory and feature selection via `Variable`s that can be specified by the user at the command line and in the bundle UI on the Options page.

| Variable | Description |
| -------- | ----------- |
| InstallRoot | A formatted string variable that specifies the installation root directory. The default value specified in `installer.wxs` should match the equivalent `INSTALLROOT` authoring in `shared.wxs`. The bundle variable is passed to each `MsiPackage` so overwrites the default directory authored in the MSI packages -- but keeping them in sync avoids the confusion if the default directory should change. |
| OptionsInstallCLI | Controls whether command-line tools will be installed. |
| OptionsInstallDBG | Controls whether debugging tools will be installed. |
| OptionsInstallIDE | Controls whether IDE integration tools will be installed. |
| OptionsInstallUtilties | Controls whether additional utilities will be installed. |
| OptionsInstallAndroidPlatform | Controls whether the Android platform will be installed. |
| OptionsInstallAndroidSDKAMD64 | Controls whether the Android AMD64 SDK will be installed. |
| OptionsInstallAndroidSDKARM | Controls whether the Android ARM SDK will be installed. |
| OptionsInstallAndroidSDKARM64 | Controls whether the Android ARM64 SDK will be installed. |
| OptionsInstallAndroidSDKX86 | Controls whether the Android X86 SDK will be installed. |
| OptionsInstallWindowsPlatform | Controls whether the Windows platform will be installed. |
| OptionsInstallWindowsSDKX86 | Controls whether the Windows X86 SDK will be installed. |
| OptionsInstallWindowsRedistAMD64 | Controls whether the Windows AMD64 Redistributable MSM will be installed. |
| OptionsInstallWindowsSDKAMD64 | Controls whether the Windows AMD64 SDK will be installed. |
| OptionsInstallWindowsRedistARM64 | Controls whether the Windows ARM64 Redistributable MSM will be installed. |
| OptionsInstallWindowsSDKX86 | Controls whether the Windows X86 SDK will be installed. |
| OptionsInstallWindowsRedistX86 | Controls whether the Windows X86 Redistributable MSM will be installed. |

Those variables are tied to controls in the bundle theme (`installer\theme.xml`) on the Options page. For example, the install directory is an `Editbox` control that takes the `InstallRoot` name to tie itself to the `InstallRoot` variable in `installer.wxs`:

```xml
<Editbox Name="InstallRoot" X="185" Y="46" Width="-91" Height="21" TabStop="yes" FontId="3" FileSystemAutoComplete="yes" />
```

Likewise, the feature selection controls are all checkboxes tied to the variables that control feature selection:

```xml
<Checkbox Name="OptionsInstallIDE" X="185" Y="170" Width="-11" Height="17" TabStop="yes" FontId="3">#(loc.OptionsInstallIDE)</Checkbox>
```

`Checkbox` controls set variables to `1` when checked and `0` when unchecked.

The variables are used in `installer.wxs` bundle authoring to control the installation of whole packages using the package `InstallCondition` attribute:

```xml
<MsiPackage
  SourceFile="!(bindpath.ide)\ide.msi"
  InstallCondition="OptionsInstallIDE = 1"
  DownloadUrl="$(BaseReleaseDownloadUrl)/{2}">
  <MsiProperty Name="INSTALLROOT" Value="[InstallRoot]" />
</MsiPackage>
```


### Bundle command line

All Burn bundles support a common set of command-line switches that, unsurprisingly, mimic [those of Windows Installer](https://learn.microsoft.com/en-us/windows/win32/msi/standard-installer-command-line-options). The Swift bundle theme also implements a standard Help dialog when you run the bundle with a `/?` switch.

For example, to uninstall the bundle showing UI but not requiring user interaction:

```sh
installer.exe /passive /uninstall
```

The same variables that drive install directory and feature selection are available from the command line too:


```sh
installer.exe /passive InstallRoot=X:\Swift OptionsInstallIDE=0 OptionsInstallWindowsSDKX86=0 OptionsInstallWindowsSDKARM64=0
```

To create a layout from a online bundle to allow for offline install:

```sh
installer.exe /layout path\to\layout\directory
```


## MSBuild projects

### Directory.Build.props

MSBuild automatically imports Directory.Build.props files in your tree. We use Directory.Build.props to centralize MSBuild properties that are shared among the .wixproj projects for the installers. Here are some of the interesting ones:

| Property | Description |
| -------- | ----------- |
| MajorMinorProductVersion | Gets the `major.minor` fields of `ProductVersion`. Used for the side-by-side upgrade strategy. |
| BaseIntermediateOutputPath, OutputPath | Sets the intermediate and build output directories to handle MSBuild/NuGet restore functionality and support multiplatform output. |
| SuppressIces | Suppress the ICE validation messages that are erroneously emitted for per-user packages. **ICE38** is about mixing per-user and per-machine resources (not a thing for us). **ICE61** is warning about allowing "same-version" major upgrades, something we want. **ICE64** is documented as not being an issue when packages are always per-user. **ICE91** is about "roaming scenarios," which doesn't apply to our use of `LocalAppDataFolder`. |
| PackageCompressionLevel, BundleCompressionLevel | Always set to `high` for the smallest downloads (and painfully-long build times). It's a property so it can be overridden for dev builds. See _<user>.props_. |
| ArePackageCabsEmbedded | Always set to false to keep the .cab files external to the .msi files. This save user disk space: Burn caches packages so it can always uninstall and repair. MSI also caches packages for uninstall. If the cab is embedded, you have two copies and MSI doesn't always use its cached copy as a source for repair. With an external .cab, MSI caches only the tiny .msi file and not the (relatively huge) .cab. |
| BundleFlavor, IsBundleCompressed | BundleFlavor defaults to `online` to build an online bundle. Set by the invocation of MSBuild to build an online or offline bundle. Controls IsBundleCompressed. |
| DefineConstants | Passes a subset of MSBuild properties into the WiX build as preprocessor variables. |
| Platforms | A semi-colon delimited list of platforms which are bundled. Set it to `android;windows` to package both platforms. |
| AndroidArchitectures | A semi-colon delimited list of architectures which the Android platform supports. Set it to `aarch64;armv7;i686;x86_64` to package all architectures. |
| WindowsArchitectures | A semi-colon delimited list of architectures which the Windows platform supports. Set it to `aarch64;i686;x86_64` to package all architectures. |
| INCLUDE_SWIFT_DOCC | swift-docc is currently conditionalized out. Set it to `True` to include it. The property `SWIFT_DOCC_BUILD` defines the directory to find the artifacts. |


## User.props

Also imported by Directory.Build.props is a user-specific .props file in same directory as Directory.Build.props:

```xml
<Import Project="$(USERNAME).props" Condition="Exists('$(USERNAME).props')" />
```

That lets you override settings in Directory.Build.props for local dev builds. For example, you can dramatically increase build speed with a couple of property tweaks:

```xml
<Project>
  <PropertyGroup>
    <PackageCompressionLevel>none</PackageCompressionLevel>
    <BundleCompressionLevel>none</BundleCompressionLevel>
  </PropertyGroup>
</Project>
```


## Building the installers

`ProjectReference`s between the various .wixproj projects establish the correct dependencies and build order, so while you can still build individual .wixproj projects, you can build the bundle and have it automatically build all the MSI packages.

To support the three architecture flavors of the SDK and RTL MSI packages, you need to pass in architecture-specific paths in MSBuild properties. Directory.Build.props translates them to "generic" preprocessor variables in the SDK .wxs authoring:

| MSBuild property | Description |
| ---------------- | ----------- |
| ImageRoot | Path to the root of the installed Swift image to package |
| Platforms | Semicolon delimited list of platforms to package (android;windows) |
| AndroidArchitectures | Semicolon delimited list of architectures the Android platform supports (aarch54;armv7;i686;x86_64) |
| WindowsArchitectures | Semicolon delimited list of architectures the Windows platform supports (aarch64;i686;x86_64) |
| WindowsRuntimeARM64 | Path to the staged Windows ARM64 runtime |
| WindowsRuntimeX64 | Path to the staged Windows AMD64 runtime |
| WindowsRuntimeX86 | Path to the staged Windows x86 runtime |

```sh
msbuild %SourceRoot%\swift-installer-scripts\platforms\Windows\bundle\installer.wixproj ^
  -m ^
  -restore ^
  -p:BundleFlavor=online ^
  -p:BaseReleaseDownloadUrl=todo://base/release/download/url ^
  -p:Configuration=Release ^
  -p:BaseOutputPath=%PackageRoot%\online\ ^
  -p:ImageRoot=%ImageRoot%\Program Files\Swift ^
  -p:Platforms="android;windows" ^
  -p:AndroidArchitectures="aarch64;armv7;i686;x86_64" ^
  -p:WindowsArchitectures="aarch64;i686;x86_64" ^
  -p:WindowsRuntimeARM64=%ImageRoot%\Prograam Files (Arm64)\Swift\Runtimes\0.0.0 ^
  -p:WindowsRuntimeX64=%ImageRoot%\Program Files\Swift\Runtimes\0.0.0 ^
  -p:WindowsRuntimeX86=%ImageRoot%\Program Files (x86)\Swift\Runtimes\0.0.0
```


## Side-by-side upgrade strategy

The Swift side-by-side upgrade strategy lets you retain the latest patch release of each major.minor release of the Swift SDK. So, for example, if you were to install these (hypothetical) versions of the Swift SDK:

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

you'd be left with these versions:

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

SideBySideUpgradeStrategy.props is imported by Directory.Build.props to bring in the GUIDs used to handle side-by-side upgrades.

Note that these GUIDs are substituted at bind time so they skip the normal validation and cleanup that the compiler does and therefore must be "proper" GUIDs:

- All uppercase
- Surrounded by curly braces

| Property | Description |
| -------- | ----------- |
| BldAssertsUpgradeCode, CliAssertsUpgradeCode, DbgUpgradeCode, IdeUpgradeCode, RtlUpgradeCode, WindowsSDKUpgradeCode, AndroidSDKUpgradeCode | Upgrade codes for individual packages. Packages keep the same upgrade codes "forever" because MSI lets you specify version ranges for upgrades, which you can find in `shared/shared.wxs`. |
| BundleUpgradeCode | Upgrade codes for the bundle. Bundles don't support upgrade version ranges, so the bundle upgrade code must change for every minor version _and_ stay the same for the entire lifetime of that minor version (e.g., v5.10.0 through v5.10.9999). You can keep the history of upgrade codes using a condition like `Condition="'$(MajorMinorProductVersion)' == '5.10'` or just replace BundleUpgradeCode when forking to a new minor version. |


### shared\shared.wxs

To support side-by-side installation for each minor release (the latest point release of each minor release), we need to use "old-school" `Upgrade`/`UpgradeVersion` authoring to get the upgrade version ranges, which also requires manually scheduling `RemoveExistingProducts`. (We can no longer use WiX's `MajorUpgrade` element because it's intended to support the way-more-common case of upgrading every version.) To avoid duplication, the upgrade logic is authored in `shared\shared.wxs` and referenced from the `Package` element of each package:

```xml
<WixVariable Id="SideBySidePackageUpgradeCode" Value="$(WindowsSDKUpgradeCode)" />
<FeatureGroupRef Id="SideBySideUpgradeStrategy" />
```

`SideBySidePackageUpgradeCode` is a bind-time variable, used here essentially to pass the upgrade code as an argument to the fragment in `shared\shared.wxs`.


### Cleaning up

Windows Installer can, under conditions that are both mysterious and undocumented, leave behind empty folders. (The files are removed.) The ICE validator ICE64 says this is a problem for roaming profiles so our use of local AppData shouldn't apply, but, sadly, sometimes it does. ICE64 suggests the user-hostile workaround of manually authoring to remove every directory. Instead, Directory.Build.props suppresses ICE64 and in `shared\shared.wxs`, the `VersionedDirectoryCleanup` component group handles cleaning up any orphaned, empty directories. Every package has a `<ComponentGroupRef Id="VersionedDirectoryCleanup" />` to pull in that component group.


## Compression levels, build time, and download size

As expected, the `high` compression level builds the smallest payloads with the benefit that you have enough time to not only go get a coffee after kicking off the build, you can drink it _and_ get a refill with time to spare.

Because both MSI and Burn use cabinets for containers and compression, the results of both packages and bundles are consistent. MSI is almost certainly using cabinets forever but Burn could adopt a different mechanism in the future (e.g., LZMA) that could improve these results.

Until that point, however, the authoring uses the older `Media` element to ensure a single .cab for maximal compression of each .msi file's content.

I tried a few combinations to show the tradeoffs of size and build time.


### Offline bundle

| Package compression | Bundle compression | Build time | Bundle size |
| ------------------- | ------------------ | ---------- | ----------- |
| High | High | 12m51s | ~470MB |
| High | None | 9m47s | ~472MB |
| None | High | 10m0s | ~479MB |
| None | Medium | 5m46s | ~513MB |
| None | Low | 4m21s | ~552MB |
| None | None | 43s | ~1.6GB |


### Online bundle

The bundle .exe itself gets a decent amount of compression:

| Compression level | Bundle size |
| ----------------- | ----------- |
| High | ~950K |
| None | ~1.2MB |

The payloads show similar sizes and build times as for the offline bundle, except that building the online bundle compresses only the .exe itself, whereas the offline bundle compresses the payloads and then (tries and mostly fails to) compress them again into the .exe:

| Compression level | Build time | Download size |
| ----------------- | ---------- | ------------- |
| High | 9m47s | ~470MB |
| Medium | 5m37s | ~504MB |
| Low | 4m10s | ~544MB |
| None | 36s | ~1.6GB |


## Getting files without installing

Windows Installer has what it calls [administrative installations](https://learn.microsoft.com/en-us/windows/win32/msi/administrative-installation) that let you "unzip" an .msi package and get its files without actually installing the package.

```sh
msiexec /qb /a build\amd64\bld.msi TARGETDIR=X:\swift.bld.admin
```

| Argument | Description |
| -------- | ----------- |
| /qb | Run a "passive" UI with a progress bar but no user interaction. |
| /a | Run an administrative installation. |
| TARGETDIR= | Specify the output path for the package's files. All the Swift packages can specify the same TARGETDIR. |

Administrative installations don't touch the machine (including the registry) outside the directory you specify for `TARGETDIR`.


## Testing in Windows Sandbox

Windows Sandbox is a Windows 10-and-later virtualization feature that is convenient and fairly easy to (mostly) automate. The Swift per-user bundles are reliable and low-impact so you don't always need to use virtual machines for testing, if you feel daring. But Sandbox's automatability makes it handy for "bulkier" testing, like the side-by-side feature. The files in samples\tests\SxS are what I used for that testing.

> Note that these files assume that there is a directory `X:\sandbox` directory that holds the test collateral and is shared with the Sandbox VM. This is a convention I use to avoid over-exposing the host machine to Sandbox VMs, which is more important when you're using Sandbox for riskier testing.

| File | Description |
| ---- | ----------- |
| BuildTests.cmd | Batch file that builds many different versions of the online bundle to `X:\sandbox\Swift\builds`. |
| RunSxSTests.cmd | Batch file that runs in the Sandbox VM when it boots (as specified in SxSTesting.wsb). It opens a couple of useful Explorer windows then executes the side-by-side test bundles. This file is expected to live in the `X:\sandbox\Swift` directory and BuildTests.cmd copies it there. |
| SxSTesting.wsb | Windows Sandbox configuration that maps `X:\sandbox\Swift` to `C:\sandbox` inside the Sandbox VM. The directory is read/write from the Sandbox VM so the host can build the test collateral into that tree and the Sandbox VM can write logs and test results. That means you can inspect results using your favorite tools on the host machine, rather than just Notepad in the basic Windows image running in the Sandbox VM. This is the file you launch to open the Sandbox VM and can live in any handy directory. |

A cut-down version with a smaller number of bundles being tested is available in the samples\tests\MiniSxS.


## Samples

`HelloMergeModule` is a sample WiX project that installs a Swift-built app and the appropriate RTL merge module. To build it, you need to set the `RedistributablesDirectory` property to the redistributables directory for a particular version of Swift:

```sh
msbuild -Restore -p:RedistributablesDirectory=X:\Swift\Redistributables\0.0.0 -p:Platform=Arm64 hellomm.wixproj
```
