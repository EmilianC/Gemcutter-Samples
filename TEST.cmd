@echo off

call :TestSample Components
call :TestSample GodRays
call :TestSample Input
call :TestSample Lighting
call :TestSample Particles
call :TestSample Shadows
call :TestSample SimpleScene
call :TestSample Sounds
call :TestSample Sprites
call :TestSample Text

title Done testing!
echo Done Testing!
goto :eof

:TestSample
pushd %1
title %1 Debug
echo %1 Debug
call bin\Debug_Win32\%1.exe
title %1 Debug
echo %1 Release
call bin\Release_Win32\%1.exe
popd
goto :eof
