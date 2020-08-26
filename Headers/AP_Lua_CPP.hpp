// Lic:
// ***********************************************************
// Headers/AP_Lua_CPP.hpp
// This particular file has been released in the public domain
// and is therefore free of any restriction. You are allowed
// to credit me as the original author, but this is not
// required.
// This file was setup/modified in:
// 2020
// If the law of your country does not support the concept
// of a product being released in the public domain, while
// the original author is still alive, or if his death was
// not longer than 70 years ago, you can deem this file
// "(c) Jeroen Broks - licensed under the CC0 License",
// with basically comes down to the same lack of
// restriction the public domain offers. (YAY!)
// ***********************************************************
// Version 20.08.26
// EndLic
#pragma once

// This include not only has to include all Lua stuff
// It also must take care of all unwanted conflicts within C and C++

// I must owe my thanks in this question
// https://stackoverflow.com/questions/22813642/lua-compile-error-with-visual-2010-external-symbol-struct-lua-state-cdecl
// to https://stackoverflow.com/users/451007/010110110101 for explaining this.




extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}