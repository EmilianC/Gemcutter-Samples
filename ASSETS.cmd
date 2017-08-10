@echo off

title Packing Assets
pushd Workspace
call "%JEWEL3D_PATH%\Tools\AssetManager\bin\Release\AssetManager.exe" --pack
if errorlevel 1 (
   title Assets Failure
   popd
   pause
   goto :eof
)
popd

title Done!
echo Done!
goto :eof
