/*
  ==============================================================================

    LuaScript.cpp
    Created: 30 Jul 2014 1:55:47pm
    Author:  patrick

  ==============================================================================
*/

#include "LuaScript.h"

LuaScript::LuaScript() : L(nullptr){
    L = lua_open();
    luaL_openlibs(L);
}