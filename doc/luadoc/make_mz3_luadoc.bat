@echo off
:del /S /Q scripts
:xcopy /S /E /Y ..\..\src\MZ3\scripts scripts\

xcopy /Y ..\..\src\MZ3\mz3_lua_api.cpp mz3_lua_api.luadoc

set PATH=%PATH%;bin
lua5.1dll bin\luadoc_start.lua mz3_lua_api.luadoc
:pause
