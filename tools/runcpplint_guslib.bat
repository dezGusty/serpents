@echo off

SETLOCAL EnableDelayedExpansion
SETLOCAL EnableExtensions

Echo.--------------------------------------------------------------------------
Echo.This script shall run cpplint for all source code files in the project.

Set g_log=%CD%\cpplint_guslib.log
Set g_python_path=python.exe
Set g_cpplint_arg=%CD%\cpplint\cpplint.py --output=vs7

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
  PushD ..\guslib\guslib
  Echo. ... Changed directory to [!CD!]
  
  Echo.Please stand by while the script analyses the header files...

  REM Start processing each desired subfolder.
  Call :doProcessing.Folder guslib\common
  Call :doProcessing.Folder guslib\system
  Call :doProcessing.Folder guslib\trace
  Call :doProcessing.Folder guslib\util
  Call :doProcessing.Folder guslib
  
  PopD
  Echo. ... Changed directory to [!CD!]
GoTo :eof


REM ---------------------------------------------
REM   Internal processing of an entire folder. 
REM   Does not enter subfolders.
REM   Can be called several times :-)
REM   @param 1 The name of the file to process.
REM ---------------------------------------------
:doProcessing.Folder paramFolderName
  Set _folder_name=%1
  FOR /f "tokens=*" %%G IN ('dir /b /r !_folder_name!\*.cpp !_folder_name!\*.hpp  !_folder_name!\*.h /O:N') DO (
    REM If no subfolders flag is used (/s), the file name does not include the folder.
    Set _single_file=%CD%\!_folder_name!\%%G
    Call :doProcessing.SingleFile !_single_file!
REM     Echo.!_single_file!
  )

GoTo :eof

REM ---------------------------------------------
REM   Internal processing of a single file
REM   Can be called several times :-)
REM   @param 1 The name of the file to process.
REM ---------------------------------------------
:doProcessing.SingleFile paramFileName

  REM Build the command.
  Call !g_python_path! !g_cpplint_arg! %1 >>!g_log! 2>&1

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
