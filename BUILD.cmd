@echo off

call ASSETS.cmd || goto :eof
call :BuildConfiguration Debug || goto :eof
call :BuildConfiguration Release || goto :eof

title Done!
echo Done!
goto :eof

:BuildConfiguration
title Building %1
echo ^>^>^>^>^>^> Building %1 ^<^<^<^<^<^<
"%ProgramFiles(x86)%\Microsoft Visual Studio\2017\BuildTools\MSBuild\15.0\Bin\MSbuild.exe" /t:Build /p:Configuration=%1 /property:Platform=Win32 "Jewel3D_Samples.sln"
if errorlevel 1 (
   title Build Failure %1
   echo ^>^>^>^>^>^> Build Failure %1 ^<^<^<^<^<^<
   pause
)
goto :eof
