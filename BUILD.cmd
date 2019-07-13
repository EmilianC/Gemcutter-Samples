@echo off

if not "%1" == "-r" (
	call :BuildConfiguration Debug || goto :eof
)
if not "%1" == "-d" (
	call :BuildConfiguration Release || goto :eof
)

title Done!
echo Done!
goto :eof

:BuildConfiguration
title Building %1
echo ^>^>^>^>^>^> Building %1 ^<^<^<^<^<^<
"%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\MSbuild.exe" /v:m /t:Build /p:Configuration=%1 /property:Platform=Win32 "Jewel3D_Samples.sln"
if errorlevel 1 (
   title Build Failure %1
   echo ^>^>^>^>^>^> Build Failure %1 ^<^<^<^<^<^<
   pause
)
goto :eof
