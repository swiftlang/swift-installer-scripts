start /min %TMP%
start /min %LOCALAPPDATA%

pushd %~dp0%

md dumps

start /wait builds\5.9.0\amd64\installer.exe /passive
dir /s %LOCALAPPDATA%\Programs > dumps\post.install.5.9.0.txt
start /wait builds\5.9.1\amd64\installer.exe /passive
dir /s %LOCALAPPDATA%\Programs > dumps\post.install.5.9.1.txt
start /wait builds\5.9.2\amd64\installer.exe /passive
dir /s %LOCALAPPDATA%\Programs > dumps\post.install.5.9.2.txt
start /wait builds\5.10.0\amd64\installer.exe /passive
dir /s %LOCALAPPDATA%\Programs > dumps\post.install.5.10.0.txt
start /wait builds\5.10.1\amd64\installer.exe /passive
dir /s %LOCALAPPDATA%\Programs > dumps\post.install.5.10.1.txt
start /wait builds\5.10.5\amd64\installer.exe /passive
dir /s %LOCALAPPDATA%\Programs > dumps\post.install.5.10.5.txt
start /wait builds\5.11.2\amd64\installer.exe /passive
dir /s %LOCALAPPDATA%\Programs > dumps\post.install.5.11.2.txt
start /wait builds\6.0.0\amd64\installer.exe /passive
dir /s %LOCALAPPDATA%\Programs > dumps\post.install.6.0.0.txt
start /wait builds\6.0.1\amd64\installer.exe /passive
dir /s %LOCALAPPDATA%\Programs > dumps\post.install.6.0.1.txt
start /wait builds\6.5.1\amd64\installer.exe /passive
dir /s %LOCALAPPDATA%\Programs > dumps\post.install.6.5.1.txt
start /wait builds\6.5.7\amd64\installer.exe /passive
dir /s %LOCALAPPDATA%\Programs > dumps\post.install.6.5.7.txt

robocopy %TMP% logs Swift*.log     /MIR /NP /NDL /XJ /DCOPY:T /FFT 