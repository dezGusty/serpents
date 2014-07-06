@Echo off

SETLOCAL EnableDelayedExpansion
SETLOCAL EnableExtensions

Rem Go to the directory where the batch is located... you know... in case we use "Run As Administrator".
PushD %~dp0

SET COVDIR=c:\Programe\cov-analysis-win32-7.0.2

Echo.Current directory is !CD!
Echo.Calling VSExpress variable setting batch file...
CALL "%VS110COMNTOOLS%..\..\VC\vcvarsall.bat" x86

Echo.Preparing build switches... 
SET MSBUILD_SWITCHES=/nologo /consoleloggerparameters:Verbosity=minimal /maxcpucount /nodeReuse:true /target:Rebuild /property:Configuration="Release";Platform=Win32

Echo.Navigating to build folder...
PushD ..\build
Echo.Current directory is !CD!
"%COVDIR%\bin\cov-build.exe" --dir cov-int MSBuild "serpents.sln" %MSBUILD_SWITCHES%

Echo.Built project serpents.sln

Echo.Compressing output...

REM Built coverity report. Compress it.
zip -r gustysserpents.zip cov-int
PopD
Echo.Compressed output...
If Exist "submitcoveritybuild.bat" (
  Echo.A batch for auto-submitting the coverity build has been detected and will be called.
  Call "submitcoveritybuild.bat"
)

:End
PopD

Echo. & ECHO Press any key to close this window...
PAUSE >NUL
ENDLOCAL
EXIT /B