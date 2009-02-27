#!/bin/sh

cp -p ../../src/MZ3/mz3_lua_api.cpp mz3_lua_api.luadoc

export LUA_PATH="./bin/?.lua;./bin/?/init.lua;;"
lua bin/luadoc_start.lua mz3_lua_api.luadoc
