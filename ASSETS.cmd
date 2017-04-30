@echo off

title Packing Assets
pushd Workspace
call AssetManager.exe.lnk --update --pack
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
