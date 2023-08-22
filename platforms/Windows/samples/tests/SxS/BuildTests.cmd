rd /s/q X:\sandbox\Swift\builds

msbuild -Restore -m -p:ProductVersion=5.9.0  -p:BundleUpgradeCode={710F1827-DA4A-4BF4-BDCE-D5F2D7C0DEF2} -p:BaseOutputPath=X:\sandbox\Swift\builds\5.9.0\  bundle\installer.wixproj
msbuild -Restore -m -p:ProductVersion=5.9.1  -p:BundleUpgradeCode={710F1827-DA4A-4BF4-BDCE-D5F2D7C0DEF2} -p:BaseOutputPath=X:\sandbox\Swift\builds\5.9.1\  bundle\installer.wixproj
msbuild -Restore -m -p:ProductVersion=5.9.2  -p:BundleUpgradeCode={710F1827-DA4A-4BF4-BDCE-D5F2D7C0DEF2} -p:BaseOutputPath=X:\sandbox\Swift\builds\5.9.2\  bundle\installer.wixproj
msbuild -Restore -m -p:ProductVersion=5.10.0 -p:BundleUpgradeCode={F3764C05-9568-4DE9-98FE-AF64C0B30706} -p:BaseOutputPath=X:\sandbox\Swift\builds\5.10.0\ bundle\installer.wixproj
msbuild -Restore -m -p:ProductVersion=5.10.1 -p:BundleUpgradeCode={F3764C05-9568-4DE9-98FE-AF64C0B30706} -p:BaseOutputPath=X:\sandbox\Swift\builds\5.10.1\ bundle\installer.wixproj
msbuild -Restore -m -p:ProductVersion=5.10.5 -p:BundleUpgradeCode={F3764C05-9568-4DE9-98FE-AF64C0B30706} -p:BaseOutputPath=X:\sandbox\Swift\builds\5.10.5\ bundle\installer.wixproj
msbuild -Restore -m -p:ProductVersion=5.11.2 -p:BundleUpgradeCode={239DD2D6-3F1B-42AE-8412-4640BD48284D} -p:BaseOutputPath=X:\sandbox\Swift\builds\5.11.2\ bundle\installer.wixproj
msbuild -Restore -m -p:ProductVersion=6.0.0  -p:BundleUpgradeCode={71CCD6EF-F3CF-492D-A1DA-D76E50A26EBF} -p:BaseOutputPath=X:\sandbox\Swift\builds\6.0.0\  bundle\installer.wixproj
msbuild -Restore -m -p:ProductVersion=6.0.1  -p:BundleUpgradeCode={71CCD6EF-F3CF-492D-A1DA-D76E50A26EBF} -p:BaseOutputPath=X:\sandbox\Swift\builds\6.0.1\  bundle\installer.wixproj
msbuild -Restore -m -p:ProductVersion=6.5.1  -p:BundleUpgradeCode={44D57C54-D8F2-4374-8C52-8856EE663D32} -p:BaseOutputPath=X:\sandbox\Swift\builds\6.5.1\  bundle\installer.wixproj
msbuild -Restore -m -p:ProductVersion=6.5.7  -p:BundleUpgradeCode={44D57C54-D8F2-4374-8C52-8856EE663D32} -p:BaseOutputPath=X:\sandbox\Swift\builds\6.5.7\  bundle\installer.wixproj

copy %~dp0\RunSxSTests.cmd X:\sandbox\Swift\
start %~dp0\SxSTesting.wsb
