start /min %TMP%
start /min %LOCALAPPDATA%

pushd %~dp0%

md dumps

set SWIFTROOT=%LOCALAPPDATA%\Programs\Swift
set SWIFTROOT=C:\Swift

start /wait builds\5.9.0\amd64\installer.exe /passive InstallRoot=%SWIFTROOT%
dir /s %SWIFTROOT% > dumps\post.install.5.9.0.txt
start /wait builds\5.9.1\amd64\installer.exe /passive InstallRoot=%SWIFTROOT%
dir /s %SWIFTROOT% > dumps\post.install.5.9.1.txt
start /wait builds\5.9.2\amd64\installer.exe /passive InstallRoot=%SWIFTROOT%
dir /s %SWIFTROOT% > dumps\post.install.5.9.2.txt

:LExit

robocopy %TMP% logs Swift*.log     /MIR /NP /NDL /XJ /DCOPY:T /FFT 