'------------------------------------------------------------------------------ 
' Utility script to register an user or system environment variable.
' @author Augustin Preda
' @date   2010.11.02
'------------------------------------------------------------------------------ 

' The type; can be one of the 2:
' USER
' SYSTEM
const ARG_VAR_TYPE = "-type"
const ARG_VAR_NAME = "-name"
const ARG_VAR_VAL = "-value"


Dim sVarType
Dim sVarName
Dim sVarValue


' Call the main function.

main


'
' Read the optional parameters.
'
Sub readParams
  tempVarType = ""
  tempVarName = ""
  tempVarValue = ""

  nStatus=0
  nCount=0
  Do While nCount < Wscript.Arguments.Count
    arg = Wscript.Arguments(nCount)
    Select Case arg
      Case ARG_VAR_TYPE
        nStatus=1
      Case ARG_VAR_NAME
        nStatus=2
      Case ARG_VAR_VAL
        nStatus=3
      Case Else
        Select Case nStatus
          Case 1
            tempVarType=tempVarType + arg
            nStatus = 1.1
          Case 1.1
            tempVarType=tempVarType + " " + arg
          Case 2
            tempVarName=tempVarName + arg
            nStatus = 2.1
          Case 2.1
            tempVarName=tempVarName + " " + arg
          Case 3
            tempVarValue=tempVarValue + arg
            nStatus = 2.1
          Case 3.1
            tempVarValue=tempVarValue + " " + arg
        End Select
    End Select
    
    nCount = nCount+1 
  Loop

  sVarType = tempVarType
  sVarName = tempVarName
  sVarValue = tempVarValue

  If sVarType = "" Then
    Wscript.Echo "No var type"
    Wscript.Quit
  End If
  If sVarName = "" Then
    Wscript.Echo "No var name"
    Wscript.Quit
  End If

End Sub


'------------------------------------------------------------------------------ 
' The main entry point in the script.
'------------------------------------------------------------------------------ 
Sub main

  ' Read the parameters.
  readParams
  
  Set wshShell = CreateObject( "WScript.Shell" )
  Set wshUserEnv = wshShell.Environment( sVarType )
  wshUserEnv(sVarName) = sVarValue
  Set wshUserEnv = Nothing
  Set wshShell   = Nothing
End Sub