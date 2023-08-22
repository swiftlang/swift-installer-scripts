rd /s/q X:\sandbox\Swift\builds

msbuild -Restore -m -p:ProductVersion=5.9.0  -p:BundleUpgradeCode={710F1827-DA4A-4BF4-BDCE-D5F2D7C0DEF2} -p:BaseOutputPath=X:\sandbox\Swift\builds\5.9.0\  bundle\installer.wixproj
msbuild -Restore -m -p:ProductVersion=5.9.1  -p:BundleUpgradeCode={710F1827-DA4A-4BF4-BDCE-D5F2D7C0DEF2} -p:BaseOutputPath=X:\sandbox\Swift\builds\5.9.1\  bundle\installer.wixproj
msbuild -Restore -m -p:ProductVersion=5.9.2  -p:BundleUpgradeCode={710F1827-DA4A-4BF4-BDCE-D5F2D7C0DEF2} -p:BaseOutputPath=X:\sandbox\Swift\builds\5.9.2\  bundle\installer.wixproj

*copy %~dp0\Run*.cmd X:\sandbox\Swift\
start %~dp0\MiniSxSTesting.wsb
