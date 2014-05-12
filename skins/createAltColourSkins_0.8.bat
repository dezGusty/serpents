@echo off

SetLocal EnableDelayedExpansion
SetLocal EnableExtensions


Rem
Rem -------------------------------------------- Create the Default theme ----------------------------------------------
Rem

Set _inputFile=..\Game\data\gui_0.8\looknfeel\GlossySerpent.looknfeel
Set _outputFile=..\Game\data\gui_0.8\looknfeel\GlossySerpent.Default.looknfeel

Echo.Creating the alternate colour for !_outputFile!.

If Exist "!_outputFile!" (
  Del "!_outputFile!"
)
Copy "!_inputFile!" "!_outputFile!"


Rem
Rem --------------------------------------------- Create the Amber theme -----------------------------------------------
Rem

Rem
Rem Initialize the contents for the Amber color scheme.
Rem
Set _inputFile=..\Game\data\gui_0.8\looknfeel\GlossySerpent.Default.looknfeel
Set _outputFile=..\Game\data\gui_0.8\looknfeel\GlossySerpent.Amber.looknfeel

Rem Copy the source file to use as a base.
Call :createTemporaryForFile !_inputFile! !_outputFile!

Echo.Creating the alternate colour for !_outputFile!.

Rem Replace the darker theme colour
Call :replaceSingleWordInFile !_outputFile! "FF940000" "FF7F4800"

Rem Replace the lighter theme colour
Call :replaceSingleWordInFile !_outputFile! "FFFF0101" "FFFF9A1A"

Rem Replace the main theme colour
Call :replaceSingleWordInFile !_outputFile! "FFDC5F5F" "FFCD6417"

Rem Replace the desaturated theme colour
Call :replaceSingleWordInFile !_outputFile! "FFAD6E6E" "FFA17127"

Rem Replace the selected text colour
Call :replaceSingleWordInFile !_outputFile! "FFFFDDDD" "FFFFFFDD"

Rem Replace the active selection colour
Call :replaceSingleWordInFile !_outputFile! "FFFF8080" "FFFFB080"

Rem Replace the inactive selection colour
Call :replaceSingleWordInFile !_outputFile! "FFB15050" "FFB18A50"

Rem Replace the gradient
Call :replaceQuadWordInFile !_outputFile! "tl:00FFFFFF tr:FFFF0101 bl:00FFFFFF br:FFFF0101" "tl:00FFFFFF tr:FFFF9A1A bl:00FFFFFF br:FFFF9A1A"

Rem
Rem --------------------------------------------- Create the Jade theme ------------------------------------------------
Rem

Rem
Rem Initialize the contents for the Jade color scheme.
Rem
Set _inputFile=..\Game\data\gui_0.8\looknfeel\GlossySerpent.Default.looknfeel
Set _outputFile=..\Game\data\gui_0.8\looknfeel\GlossySerpent.Jade.looknfeel

Rem Copy the source file to use as a base.
Call :createTemporaryForFile !_inputFile! !_outputFile!

Echo.Creating the alternate colour for !_outputFile!.

Rem Replace the darker theme colour
Call :replaceSingleWordInFile !_outputFile! "FF940000" "FF009400"

Rem Replace the lighter theme colour
Call :replaceSingleWordInFile !_outputFile! "FFFF0101" "FF01FF01"

Rem Replace the main theme colour
Call :replaceSingleWordInFile !_outputFile! "FFDC5F5F" "FF57BD57"
 
Rem Replace the desaturated theme colour
Call :replaceSingleWordInFile !_outputFile! "FFAD6E6E" "FF6FA16F"

Rem Replace the selected text colour
Call :replaceSingleWordInFile !_outputFile! "FFFFDDDD" "FFDDFFDD"
 
Rem Replace the active selection colour
Call :replaceSingleWordInFile !_outputFile! "FFFF8080" "FF80FF80"
 
Rem Replace the inactive selection colour
Call :replaceSingleWordInFile !_outputFile! "FFB15050" "FF50B150"
 
Rem Replace the gradient
Call :replaceQuadWordInFile !_outputFile! "tl:00FFFFFF tr:FFFF0101 bl:00FFFFFF br:FFFF0101" "tl:00FFFFFF tr:FF01FF01 bl:00FFFFFF br:FF01FF01"


Rem
Rem --------------------------------------------- Create the Blue theme ------------------------------------------------
Rem

Rem
Rem Initialize the contents for the Blue color scheme.
Rem
Set _inputFile=..\Game\data\gui_0.8\looknfeel\GlossySerpent.Default.looknfeel
Set _outputFile=..\Game\data\gui_0.8\looknfeel\GlossySerpent.Blue.looknfeel

Rem Copy the source file to use as a base.
Call :createTemporaryForFile !_inputFile! !_outputFile!

Echo.Creating the alternate colour for !_outputFile!.

Rem Replace the darker theme colour
Call :replaceSingleWordInFile !_outputFile! "FF940000" "FF009400"

Rem Replace the lighter theme colour
Call :replaceSingleWordInFile !_outputFile! "FFFF0101" "FF01FF01"

Rem Replace the main theme colour
Call :replaceSingleWordInFile !_outputFile! "FFDC5F5F" "FF57BD57"
 
Rem Replace the desaturated theme colour
Call :replaceSingleWordInFile !_outputFile! "FFAD6E6E" "FF6FA16F"

Rem Replace the selected text colour
Call :replaceSingleWordInFile !_outputFile! "FFFFDDDD" "FFDDFFDD"
 
Rem Replace the active selection colour
Call :replaceSingleWordInFile !_outputFile! "FFFF8080" "FF80FF80"
 
Rem Replace the inactive selection colour
Call :replaceSingleWordInFile !_outputFile! "FFB15050" "FF50B150"
 
Rem Replace the gradient
Call :replaceQuadWordInFile !_outputFile! "tl:00FFFFFF tr:FFFF0101 bl:00FFFFFF br:FFFF0101" "tl:00FFFFFF tr:FF01FF01 bl:00FFFFFF br:FF01FF01"


Rem
Rem ------------------------------------------ Create the Default FHD theme --------------------------------------------
Rem

Set _inputFile=..\Game\data\gui_0.8\looknfeel\GlossySerpentFHD.looknfeel
Set _outputFile=..\Game\data\gui_0.8\looknfeel\GlossySerpentFHD.Default.looknfeel

Echo.Creating the alternate colour for !_outputFile!.

If Exist "!_outputFile!" (
  Del "!_outputFile!"
)
Copy "!_inputFile!" "!_outputFile!"


Rem
Rem ------------------------------------------- Create the FHD Amber theme ---------------------------------------------
Rem

Rem
Rem Initialize the contents for the Amber color scheme.
Rem
Set _inputFile=..\Game\data\gui_0.8\looknfeel\GlossySerpentFHD.Default.looknfeel
Set _outputFile=..\Game\data\gui_0.8\looknfeel\GlossySerpentFHD.Amber.looknfeel

Rem Copy the source file to use as a base.
Call :createTemporaryForFile !_inputFile! !_outputFile!

Echo.Creating the alternate colour for !_outputFile!.

Rem Replace the darker theme colour
Call :replaceSingleWordInFile !_outputFile! "FF940000" "FF7F4800"

Rem Replace the lighter theme colour
Call :replaceSingleWordInFile !_outputFile! "FFFF0101" "FFFF9A1A"

Rem Replace the main theme colour
Call :replaceSingleWordInFile !_outputFile! "FFDC5F5F" "FFCD6417"

Rem Replace the desaturated theme colour
Call :replaceSingleWordInFile !_outputFile! "FFAD6E6E" "FFA17127"

Rem Replace the selected text colour
Call :replaceSingleWordInFile !_outputFile! "FFFFDDDD" "FFFFFFDD"

Rem Replace the active selection colour
Call :replaceSingleWordInFile !_outputFile! "FFFF8080" "FFFFB080"

Rem Replace the inactive selection colour
Call :replaceSingleWordInFile !_outputFile! "FFB15050" "FFB18A50"

Rem Replace the gradient
Call :replaceQuadWordInFile !_outputFile! "tl:00FFFFFF tr:FFFF0101 bl:00FFFFFF br:FFFF0101" "tl:00FFFFFF tr:FFFF9A1A bl:00FFFFFF br:FFFF9A1A"

Rem
Rem ------------------------------------------- Create the FHD Jade theme ----------------------------------------------
Rem

Rem
Rem Initialize the contents for the Jade color scheme.
Rem
Set _inputFile=..\Game\data\gui_0.8\looknfeel\GlossySerpentFHD.Default.looknfeel
Set _outputFile=..\Game\data\gui_0.8\looknfeel\GlossySerpentFHD.Jade.looknfeel

Rem Copy the source file to use as a base.
Call :createTemporaryForFile !_inputFile! !_outputFile!

Echo.Creating the alternate colour for !_outputFile!.

Rem Replace the darker theme colour
Call :replaceSingleWordInFile !_outputFile! "FF940000" "FF009400"

Rem Replace the lighter theme colour
Call :replaceSingleWordInFile !_outputFile! "FFFF0101" "FF01FF01"

Rem Replace the main theme colour
Call :replaceSingleWordInFile !_outputFile! "FFDC5F5F" "FF57BD57"
 
Rem Replace the desaturated theme colour
Call :replaceSingleWordInFile !_outputFile! "FFAD6E6E" "FF6FA16F"

Rem Replace the selected text colour
Call :replaceSingleWordInFile !_outputFile! "FFFFDDDD" "FFDDFFDD"
 
Rem Replace the active selection colour
Call :replaceSingleWordInFile !_outputFile! "FFFF8080" "FF80FF80"
 
Rem Replace the inactive selection colour
Call :replaceSingleWordInFile !_outputFile! "FFB15050" "FF50B150"
 
Rem Replace the gradient
Call :replaceQuadWordInFile !_outputFile! "tl:00FFFFFF tr:FFFF0101 bl:00FFFFFF br:FFFF0101" "tl:00FFFFFF tr:FF01FF01 bl:00FFFFFF br:FF01FF01"


Rem
Rem ------------------------------------------- Create the FHD Blue theme ----------------------------------------------
Rem

Rem
Rem Initialize the contents for the Blue color scheme.
Rem
Set _inputFile=..\Game\data\gui_0.8\looknfeel\GlossySerpentFHD.Default.looknfeel
Set _outputFile=..\Game\data\gui_0.8\looknfeel\GlossySerpentFHD.Blue.looknfeel

Rem Copy the source file to use as a base.
Call :createTemporaryForFile !_inputFile! !_outputFile!

Echo.Creating the alternate colour for !_outputFile!.

Rem Replace the darker theme colour
Call :replaceSingleWordInFile !_outputFile! "FF940000" "FF000094"

Rem Replace the lighter theme colour
Call :replaceSingleWordInFile !_outputFile! "FFFF0101" "FF0101FF"

Rem Replace the main theme colour
Call :replaceSingleWordInFile !_outputFile! "FFDC5F5F" "FF5F5FDC"
 
Rem Replace the desaturated theme colour
Call :replaceSingleWordInFile !_outputFile! "FFAD6E6E" "FF6E6EAD"

Rem Replace the selected text colour
Call :replaceSingleWordInFile !_outputFile! "FFFFDDDD" "FFDDDDFF"
 
Rem Replace the active selection colour
Call :replaceSingleWordInFile !_outputFile! "FFFF8080" "FF8080FF"
 
Rem Replace the inactive selection colour
Call :replaceSingleWordInFile !_outputFile! "FFB15050" "FF5050B1"
 
Rem Replace the gradient
Call :replaceQuadWordInFile !_outputFile! "tl:00FFFFFF tr:FFFF0101 bl:00FFFFFF br:FFFF0101" "tl:00FFFFFF tr:FF0101FF bl:00FFFFFF br:FF0101FF"


Echo. Finished processing.

GoTo :eof


Rem
Rem Make a replacement of a single simple word (no blanks)
Rem
:replaceSingleWordInFile sourceFileName sourceWord destWord
  Set sourceFileName=%1
  Set sourceWord=%2
  Set destWord=%3
  
  Set auxFileName=!sourceFileName!_aux
  If Exist "!auxFileName!" (
    Del "!auxFileName!"
  )

  Rem Call the replacement utility
  sed -e s/!sourceWord!/!destWord!/g "!sourceFileName!" > "!auxFileName!"

  Rem Delete the source file
  If Exist "!sourceFileName!" (
    Del "!sourceFileName!"
  )
  Rem Rename the destination file to the source file.
  Copy "!auxFileName!" "!sourceFileName!"
  If Exist "!auxFileName!" (
    Del "!auxFileName!"
  )
GoTo :eof



Rem
Rem Make a replacement of a word consisting of 4 words (so it should contain 3 space chars)
Rem
:replaceQuadWordInFile sourceFileName src1 src2 src3 src4 dst1 dst2 dst3 dst4
  Set sourceFileName=%1
  Set src1=%2
  Set dst1=%3

  Rem Remove the quotation marks from the input params.
  set src1=%src1:"=%
  set dst1=%dst1:"=%
  
  Set auxFileName=!sourceFileName!_aux
  If Exist "!auxFileName!" (
    Del "!auxFileName!"
  )

  Rem Call the replacement utility
  sed -e "s/!src1!/!dst1!/g" "!sourceFileName!" > "!auxFileName!"
REM   Echo sed -e "s/!src1!/!dst1!/g" "!sourceFileName!" ^> "!auxFileName!"

  Rem Delete the source file
  If Exist "!sourceFileName!" (
    Del "!sourceFileName!"
  )
  Rem Rename the destination file to the source file.
  Copy "!auxFileName!" "!sourceFileName!"
  If Exist "!auxFileName!" (
    Del "!auxFileName!"
  )
GoTo :eof



Rem
Rem Create a copy for the source file name.
Rem
:createTemporaryForFile sourceFileName destFileName
  Set sourceFileName=%1
  Set destFileName=%2
  If Exist "!destFileName!" (
    Del "!destFileName!"
  )
  Copy "!sourceFileName!" "!destFileName!"
GoTo :eof
