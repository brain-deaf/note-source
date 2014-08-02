/*
  ==============================================================================

    LuaScript.cpp
    Created: 30 Jul 2014 1:55:47pm
    Author:  patrick

  ==============================================================================
*/

#include "LuaScript.h"
#include "Sampler.h"
#include "MappingEditorBin.h"
#include <memory>

static LuaScript* luaScript = nullptr;
static Sampler* staticSampler = nullptr;

static int l_playNote(lua_State* L){
    double note = lua_tonumber(L, 1);
    double velocity = lua_tonumber(L, 2);
    double timestamp = lua_tonumber(L, 3);
    double triggered_note = lua_tonumber(L, 4);
    
    MidiMessage* m = new MidiMessage(MidiMessage::noteOn(1, (int)note, (float)velocity));
    m->setTimeStamp(timestamp);
    
    auto n = std::make_shared<NoteEvent>();
    n->setTriggerNote(triggered_note);
    n->setNoteNumber(note);
    n->setVelocity(velocity);
    n->setId(staticSampler->getIdCount());
    
    lua_pushnumber(L, staticSampler->getIdCount());
    
    staticSampler->incIdCount();
    
    staticSampler->getIncomingEvents().add(n);
    staticSampler->getMidiCollector().addMessageToQueue(*m);
    
    return 1;
}

LuaScript::LuaScript(MappingEditorBin* m) : L(nullptr), mapping_editor(m), lastPlayedNote(0){
    L = lua_open();
    luaL_openlibs(L);
    lua_pushcfunction(L, l_playNote);
    lua_setglobal(L, "playNote");
    luaScript = this;
}

void LuaScript::loadScript(String f){
    if (staticSampler == nullptr)
    staticSampler = mapping_editor->getMappingEditor()->graph->getSamplerP();
        
    if (luaL_loadfile(L, f.toRawUTF8()) || lua_pcall(L, 0, 0, 0)){
        std::cout<<"error: "<<lua_tostring(L, -1)<<std::endl;
    }
}

void LuaScript::onNote(int note, double velocity, double timestamp){
    lastPlayedNote = note;
    lua_getglobal(L, "onNote");
    lua_pushnumber(L, note);
    lua_pushnumber(L, velocity);
    lua_pushnumber(L, timestamp);
    if (lua_pcall(L, 3, 0, 0) != 0)
        std::cout<<"error running function `onNote' : "<<lua_tostring(L, -1)<<std::endl;
        
    //lua_pop(L, 1); no results to pop
}