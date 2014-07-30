/*
  ==============================================================================

    LuaScript.h
    Created: 30 Jul 2014 1:55:59pm
    Author:  patrick

  ==============================================================================
*/

#ifndef LUASCRIPT_H_INCLUDED
#define LUASCRIPT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

class LuaScript{
public:
    LuaScript();
    lua_State* getLuaState(){return L;}
    void loadScript(String);
private:
    lua_State* L;
};


#endif  // LUASCRIPT_H_INCLUDED
