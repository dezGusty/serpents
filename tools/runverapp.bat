@echo off

SETLOCAL EnableDelayedExpansion
SETLOCAL EnableExtensions

Echo.--------------------------------------------------------------------------
Echo.This script shall run vera for all source code files in the project.

Set g_log=%CD%\vera.log
Set g_vera_path="c:\Program Files (x86)\vera++\bin\vera++.exe"
Set g_vera_arg_prefix=--profile gus --vc-report - --warning --summary
Set g_vera_arg_suffix=-r %CD%\vera

Echo.Should use log file [!g_log!].
Echo.--------------------------------------------------------------------------

REM Identify the files that need to be parsed.
Call :doProcessing.ALL

GoTo :eof

REM ---------------------------------
REM  Internal processing of all files.
REM  ---------------------------------
:doProcessing.ALL
  REM Go to the parent folder.
  PushD ..
  Echo. ... Changed directory to [!CD!]
  REM Get the contents of the include subfolder
  Echo.Please stand by while the script analyses the header files...
  FOR /f "tokens=*" %%G IN ('dir /b /s include\*.h /O:N') DO (
    
    Call :doProcessing.SingleFile %%G
  )

  Echo.Please stand by while the script analyses the source files...
  FOR /f "tokens=*" %%G IN ('dir /b /s src\*.cpp /O:N') DO (
    Set lcl_baseFileName=%%G
    Call :doProcessing.SingleFile !lcl_baseFileName!
  )

  Echo.Please stand by while the script analyses the source files...
  FOR /f "tokens=*" %%G IN ('dir /b /s testfiles\*.cpp,testfiles\*.h /O:N') DO (
    Set lcl_baseFileName=%%G
    Call :doProcessing.SingleFile !lcl_baseFileName!
  )

  PopD
  Echo. ... Changed directory to [!CD!]
GoTo :eof

REM ---------------------------------------------
REM   Internal processing of a single file
REM   Can be called several times :-)
REM   @param 1 The name of the file to process.
REM ---------------------------------------------
:doProcessing.SingleFile paramFileName

  REM Build the command.
  Call !g_vera_path! !g_vera_arg_prefix! %1 !g_vera_arg_suffix! >>!g_log! 2>&1

GoTo :eof

Rem ------------------------------------------------------------------------------------
Rem Get the date in a nice format.
Rem Solution from: http://ss64.com/nt/syntax-getdate.html
:getDateYYYYMMDD _my_date
SetLocal
Rem This will return date into environment vars
Rem Works on any NT/2K/XP machine independent of regional date settings
Rem E.g. 20 March 2002
For /f "tokens=1-4 delims=/-. " %%G In ('Date /t') Do (Call :s_fixdate %%G %%H %%I %%J)
EndLocal&Set %1=%yy%.%mm%.%dd%

:s_fixdate
If "%1:~0,1%" GTR "9" Shift
For /f "skip=1 tokens=2-4 delims=(-)" %%G In ('Echo.^|date') DO (
   Set %%G=%1&Set %%H=%2&Set %%I=%3)
GoTo :eof
Rem ------------------------------------------------------------------------------------
