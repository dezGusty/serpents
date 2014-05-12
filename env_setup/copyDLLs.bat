@echo off

SetLocal EnableDelayedExpansion
SetLocal EnableExtensions

REM Inform the user about the purpose of this script
Echo This script will copy ^(and replace^) the DLL files used by the Serpents project.

Set GameDir=..\Game\

Echo Searching for OGRE library
If Not "%OGRE_HOME%"=="" (
  Set _OgreHomeDir=%OGRE_HOME%
  Rem For consistency, remove the trailing slash in the name
  call :fixPathNoTrailing _OgreHomeDir
  Echo Ogre library found in [!_OgreHomeDir!]
  
  call :tryToCopySameName OgreMain.dll "!_OgreHomeDir!\bin\release\" "!GameDir!"
  call :tryToCopySameName OIS.dll "!_OgreHomeDir!\bin\release\" "!GameDir!"
  call :tryToCopySameName cg.dll "!_OgreHomeDir!\bin\release\" "!GameDir!"

  call :tryToCopySameName OgreMain_D.dll "!_OgreHomeDir!\bin\debug\" "!GameDir!"
  call :tryToCopySameName OIS_D.dll "!_OgreHomeDir!\bin\debug\" "!GameDir!"
  
) Else (
  Echo Could not find OGRE library. Make sure the env var. [OGRE_HOME] points to the installation folder.
)

Echo Searching for CEGUI library
If Not "%CEGUIDIR%"=="" (
  Set _CeguiHomeDir=%CEGUIDIR%
  Rem For consistency, remove the trailing slash in the name
  call :fixPathNoTrailing _CeguiHomeDir
  Echo Cegui library found in [!_CeguiHomeDir!]

  call :tryToCopySameName CEGUIBase.dll "!_CeguiHomeDir!\bin\" "!GameDir!"
  call :tryToCopySameName CEGUIBase_D.dll "!_CeguiHomeDir!\bin\" "!GameDir!"
  
  call :tryToCopySameName CEGUIOgreRenderer.dll "!_CeguiHomeDir!\bin\" "!GameDir!"
  call :tryToCopySameName CEGUIOgreRenderer_D.dll "!_CeguiHomeDir!\bin\" "!GameDir!"
  
  call :tryToCopySameName CEGUITinyXMLParser.dll "!_CeguiHomeDir!\bin\" "!GameDir!"
  call :tryToCopySameName CEGUITinyXMLParser_D.dll "!_CeguiHomeDir!\bin\" "!GameDir!"
  
  call :tryToCopySameName CEGUIFalagardWRBase.dll "!_CeguiHomeDir!\bin\" "!GameDir!"
  call :tryToCopySameName CEGUIFalagardWRBase_D.dll "!_CeguiHomeDir!\bin\" "!GameDir!"
  
) Else (
  Echo Could not find the CEGUI library. Make sure the env var. [CEGUIDIR] points to the installation folder.
)

:end
Echo Done
GoTo :eof


Rem 
Rem Try to copy a file to a given location.
Rem @param %1 The name of the file to copy
Rem @param %2 Copy from this folder
Rem @param %3 Copy to this folder
Rem 
:tryToCopySameName
  SetLocal
  Set _temp2=%2
  Set _temp3=%3
  call :fixPathNoTrailing _temp2
  call :fixPathNoTrailing _temp3

  If Exist !_temp2!\%1 (
    If Exist !_temp3!\%1 (
      Copy !_temp2!\%1 !_temp3!\%1
      Echo Overcopied [%1] from [!_temp2!] to [!_temp3!]
    ) Else (
      Copy !_temp2!\%1 !_temp3!\%1
      Echo Copied [%1] from [!_temp2!] to [!_temp3!]
    ) 
  ) Else (
    Echo ERROR: Could not find file to copy [!_temp2!\%1]
  )
  
  EndLocal
GoTo :eof


Rem 
Rem Fix the path sent as the first parameter add a "\" character at the end if one is not present.
Rem @param %1 The name of the variable to fix (don't add %% or !! characters around it)
Rem 
:fixPathEnforceTrailing
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


Rem 
Rem Fix the path sent as the first parameter add remove any "\" or "/" character at the end if one is present.
Rem @param %1 The name of the variable to fix (don't add %% or !! characters around it)
Rem 
:fixPathNoTrailing
  Set _lclVar=!%1!
  
  Set _needToRemoveChar=0
  If "!_lclVar:~-1!"=="\" (
    Set _needToRemoveChar=1
  )
  If "!_lclVar:~-1!"=="/" (
    Set _needToRemoveChar=1
  )
  
  If "!_needToRemoveChar!"=="1" (
    Set %1=!_lclVar:~0,-1!
  )
GoTo :eof
