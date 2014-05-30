@ECHO OFF
REM
REM Licensed to whatever license PeerBlock is using.
REM


SETLOCAL

PUSHD %~dp0

SET COVDIR=c:\Programe\cov-analysis-win32-7.0.2
SET "PB_DDK_DIR=H:\WinDDK\7600.16385.1"

CALL "%VS110COMNTOOLS%..\..\VC\vcvarsall.bat" x86

SET MSBUILD_SWITCHES=/nologo /consoleloggerparameters:Verbosity=minimal /maxcpucount /nodeReuse:true /target:Rebuild /property:Configuration="Release";Platform=Win32

PushD ../build
"%COVDIR%\bin\cov-build.exe" --dir cov-int MSBuild "serpents.sln" %MSBUILD_SWITCHES%

REM Built coverity report. Compress it.
zip -r gustysserpents.zip cov-int

PopD

REM :tar
REM IF EXIST "PeerBlock.tgz" DEL "PeerBlock.tgz"
REM tar --version 1>&2 2>NUL || (ECHO. & ECHO ERROR: tar not found & GOTO SevenZip)
REM tar czvf PeerBlock.tgz cov-int
REM GOTO End
REM  
REM :SevenZip
REM IF NOT EXIST "%PROGRAMFILES%\7za.exe" (ECHO. & ECHO ERROR: "%PROGRAMFILES%\7za.exe" not found & GOTO End)
REM IF EXIST "PeerBlock.tar" DEL "PeerBlock.tar"
REM IF EXIST "PeerBlock.tgz" DEL "PeerBlock.tgz"
REM "%PROGRAMFILES%\7za.exe" a -ttar PeerBlock.tar cov-int
REM "%PROGRAMFILES%\7za.exe" a -tgzip PeerBlock.tgz PeerBlock.tar
REM IF EXIST "PeerBlock.tar" DEL "PeerBlock.tar"


:End
POPD
ECHO. & ECHO Press any key to close this window...
PAUSE >NUL
ENDLOCAL
EXIT /B