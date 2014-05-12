@echo off

SetLocal EnableDelayedExpansion
SetLocal EnableExtensions

REM Inform the user about the purpose of this script
Echo This script will check and register the specific environment variables used within the Serpents projects.

REM 
REM REM Check to see whether it's already registered.
REM If "%SERPENT_PATH%"=="" (
REM   REM The environment variable has not been set.
REM   Echo The environment variable [SERPENT_PATH] is not set yet.
REM   PushD..
REM   Echo E.g. !CD!
REM   PopD
REM   Call :getStringInput SERPENT_PATH
REM ) Else (
REM   Echo The environment variable [SERPENT_PATH] is already registered to [%SERPENT_PATH%].
REM   Call :reregisterVarConfirm SERPENT_PATH
REM )



Rem Newline
Echo.

REM Check to see whether it's already registered.
If "%OGRE_HOME%"=="" (
  REM The environment variable has not been set.
  Echo The environment variable [OGRE_HOME] is not set yet.

  Call :getStringInput OGRE_HOME
) Else (
  Echo The environment variable [OGRE_HOME] is already registered to [%OGRE_HOME%].
  Call :reregisterVarConfirm OGRE_HOME
)

Rem Newline
Echo.

REM Check to see whether it's already registered.
If "%CEGUI_HOME%"=="" (
  REM The environment variable has not been set.
  Echo The environment variable [CEGUI_HOME] is not set yet.

  Call :getStringInput CEGUI_HOME
) Else (
  Echo The environment variable [CEGUI_HOME] is already registered to [%CEGUI_HOME%].
  Call :reregisterVarConfirm CEGUI_HOME
)

Rem Newline
Echo.

Rem Check to see whether CEGUI has the premake.exe
Set _expectedPath="%CEGUI_HOME%projects\premake\premake.exe"
If Not Exist !_expectedPath! (
  Echo The [premake.exe] was not found^!
  Echo Expected location: !_expectedPath!
)


REM Check to see whether it's already registered.
If "%BOOST_ROOT%"=="" (
  REM The environment variable has not been set.
  Echo The environment variable [BOOST_ROOT] is not set yet.
  Echo Please make sure it matches the version expected by the OGRE SDK.

  Call :getStringInput BOOST_ROOT
) Else (
  Echo The environment variable [BOOST_ROOT] is already registered to [%BOOST_ROOT%].
  Echo If you want to change it, please make sure it matches the version expected by the OGRE SDK.
  Call :reregisterVarConfirm BOOST_ROOT
)


REM Check to see whether it's already registered.
If "%OIS_HOME%"=="" (
  REM The environment variable has not been set.
  Echo The environment variable [OIS_HOME] is not set yet.
  Echo Please make sure it points to the directory where [includes/ois.h] is located. 
  Echo.E.g. [d:\wrkdir\OgreDependencies\src\ois]
  Call :getStringInput OIS_HOME
) Else (
  Echo The environment variable [OIS_HOME] is already registered to [%OIS_HOME%].
  Call :reregisterVarConfirm OIS_HOME
)



Rem Newline
Echo.
Echo Creating required folders if not existing.
PushD..

If Not Exist "Debug" (
  Md Debug
)
If Not Exist "Release" (
  Md Release
)
If Not Exist "ReleaseNoTrace" (
  Md ReleaseNoTrace
)

If Not Exist "Debug\audiere.dll" (
  If Exist "bin\audiere.dll" (
    Copy bin\audiere.dll Debug\audiere.dll
  )
)
If Not Exist "Release\audiere.dll" (
  If Exist "bin\audiere.dll" (
    Copy bin\audiere.dll Release\audiere.dll
  )
)
If Not Exist "ReleaseNoTrace\audiere.dll" (
  If Exist "bin\audiere.dll" (
    Copy bin\audiere.dll ReleaseNoTrace\audiere.dll
  )
)

If Not Exist "lib\libboost_regex-vc100-mt-1_44.lib" (
  If Exist "lib\libboost_regex-vc100-mt-1_44.7z" (
    Echo Please unarc [lib\libboost_regex-vc100-mt-1_44.7z]
  )
)

If Not Exist "lib\libboost_regex-vc100-mt-gd-1_44.lib" (
  If Exist "lib\libboost_regex-vc100-mt-gd-1_44.7z" (
    Echo Please unarc [lib\libboost_regex-vc100-mt-gd-1_44.7z]
  )
)

PopD

Echo.
Echo Setting folder read-only flags to enable Desktop.ini usage...
Rem Set the read-only flag for the "Game" folder (it has an INI file that will be used in this case).
PushD..
If Exist "Game" (
  Attrib +R Game
)
PopD

Pause
GoTo :eof

:getStringInput
  Set /P _answer=Please enter the path: 
  If "!_answer!"=="" GoTo :getStringInput
  
  Rem Fix the variable path. 
  Call :fixPath _answer

  Call :doStringRegistration %1 !_answer!
GoTo :eof

:reregisterVarConfirm
  Set /P _answer=Do you want to change it? [Y / N] 
  Set _continue=2
  If "!_answer!"=="Y" Set _continue=1
  If "!_answer!"=="y" Set _continue=1
  If "!_answer!"=="N" Set _continue=0
  If "!_answer!"=="n" Set _continue=0
  If !_continue!==2 GoTo :reregisterVarConfirm
  If !_continue!==1 Call :getStringInput %1
GoTo :eof

:doStringRegistration
  wscript.exe self_register_hlp.vbs -type USER -name %1 -value %2
  Set %1=%2
  Echo Registered [%1] with [%2]
GoTo :eof

Rem and add a "\" character at the end if one is not present.
:fixPath
  Set _lclVar=!%1!
  
  Set _needToAddChar=1
  If "!_lclVar:~-1!"=="\" (
    Set _needToAddChar=0
  )
  If "!_lclVar:~-1!"=="/" (
    Set _needToAddChar=0
  )
  
  If "!_needToAddChar!"=="1" (
    Set %1=!_lclVar!\
  )
GoTo :eof