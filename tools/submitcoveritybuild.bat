@Echo off

SetLocal EnableDelayedExpansion
SetLocal EnableExtensions

Set path_to_build="..\build"
Set coverity_zip=gustysserpents.zip
If Exist !path_to_build! (
  PushD !path_to_build!
  Echo.Using curl to submit the coverity build [!coverity_zip!].
  Echo.Please stand by...
  curl --form project=dezGusty%%2Fserpents --form token=KRaOF-kSeKSMDHXTJ3-ILA --form email=thegusty999@gmail.com --form file=@!coverity_zip! --form version="abs" --form description="auto-build-submit" https://scan.coverity.com/builds?project=dezGusty%%2Fserpents
  Echo.Coverity build submitted.
) Else (
  Echo.Could not navigate to folder [!path_to_build!].
  GoTo :eof
)

PopD
