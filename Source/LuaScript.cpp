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
#include "MainPage.h"
#include <memory>
#include "MainPageComponents.h"
#include "InstrumentComponent.h"

static LuaScript* luaScript = nullptr;
static Sampler* staticSampler = nullptr;
static MainPage* staticMainPage = nullptr;

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

static int l_setGroup(lua_State* L){
    int id = lua_tonumber(L, 1);
    int group = lua_tonumber(L, 2);
    
    for (auto e : staticSampler->getIncomingEvents()){
        if (e->getId() == id){
            e->getGroups().add(group);
        }
    }
    
    return 0;
}

static int l_makeHorizontalSlider(lua_State* L){
    String name = lua_tostring(L, 1);
    double min = lua_tonumber(L, 2);
    double max = lua_tonumber(L, 3);
    double interval = lua_tonumber(L, 4);
    
    auto s = new MainHorizontalSlider(name, min, max, interval);
    s->setSize(250, 30);
    s->addListener(luaScript);
    
    staticMainPage->getComponents()[name] = s;
    staticMainPage->addNewComponent(name);
    
    lua_pushstring(L, name.toRawUTF8());
    
    return 1;
}

static int l_makeVerticalSlider(lua_State* L){
    String name = lua_tostring(L, 1);
    double min = lua_tonumber(L, 2);
    double max = lua_tonumber(L, 3);
    double interval = lua_tonumber(L, 4);
    
    auto s = new MainVerticalSlider(name, min, max, interval);
    s->setSize(30, 250);
    s->addListener(luaScript);
    
    staticMainPage->getComponents()[name] = s;
    staticMainPage->addNewComponent(name);
    
    lua_pushstring(L, name.toRawUTF8());
    
    return 1;
}

static int l_makeKnob(lua_State* L){
    String name = lua_tostring(L, 1);
    double min = lua_tonumber(L, 2);
    double max = lua_tonumber(L, 3);
    double interval = lua_tonumber(L, 4);
    
    auto s = new MainKnob(name, min, max, interval);
    s->setSize(30, 250);
    s->addListener(luaScript);
    
    staticMainPage->getComponents()[name] = s;
    staticMainPage->addNewComponent(name);
    
    lua_pushstring(L, name.toRawUTF8());
    
    return 1;
}

static int l_makeButton(lua_State* L){
    String name = lua_tostring(L, 1);
    bool toggle = lua_toboolean(L, 2);
    
    auto s = new MainButton(name);
    s->setSize(80, 50);
    s->addListener(luaScript);
    s->setClickingTogglesState(toggle);
    
    staticMainPage->getComponents()[name] = s;
    staticMainPage->addNewComponent(name);
    
    lua_pushstring(L, name.toRawUTF8());
    
    return 1;
}

static int l_makeLabel(lua_State* L){
    String name = lua_tostring(L, 1);
    String text = lua_tostring(L, 2);
    
    auto s = new MainLabel(name, text);
    s->setSize(150, 15);
    
    staticMainPage->getComponents()[name] = s;
    staticMainPage->addNewComponent(name);
    
    lua_pushstring(L, name.toRawUTF8());
    
    return 1;
}

static int l_setSize(lua_State* L){
    String name = lua_tostring(L, 1);
    double width = lua_tonumber(L, 2);
    double height = lua_tonumber(L, 3);
    
    Component* c = staticMainPage->getComponents()[name];
    if (c != nullptr)
        c->setSize(width, height);
    
    return 0;
}

static int l_setPosition(lua_State* L){
    String name = lua_tostring(L, 1);
    double x = lua_tonumber(L, 2);
    double y= lua_tonumber(L, 3);
    
    Component* c = staticMainPage->getComponents()[name];
    if (c != nullptr)
        c->setTopLeftPosition(x, y);
    
    return 0;
}

static int l_getPageWidth(lua_State* L){
    lua_pushnumber(L, staticMainPage->getWidth());
    return 1;
}

static int l_getWidth(lua_State* L){
    String name = lua_tostring(L, 1);
    Component* c = staticMainPage->getComponents()[name];
    
    if (c != nullptr){
        lua_pushnumber(L, staticMainPage->getComponents()[name]->getWidth());
    }else{
        lua_pushnumber(L, -1);
    }
    return 1;
}

static int l_getHeight(lua_State* L){
    String name = lua_tostring(L, 1);
    Component* c = staticMainPage->getComponents()[name];
    
    if (c != nullptr){
        lua_pushnumber(L, staticMainPage->getComponents()[name]->getHeight());
    }else{
        lua_pushnumber(L, -1);
    }
    return 1;
}

static int l_Hide(lua_State* L){
    String name = lua_tostring(L, 1);
    Component* c = staticMainPage->getComponents()[name];
    
    if (c != nullptr){
        c->setVisible(false);
    }
}

static int l_Show(lua_State* L){
    String name = lua_tostring(L, 1);
    Component* c = staticMainPage->getComponents()[name];
    
    if (c != nullptr){
        c->setVisible(true);
    }
}

LuaScript::LuaScript(MappingEditorBin* m) : L(nullptr), mapping_editor(m), lastPlayedNote(0){
    L = lua_open();
    luaL_openlibs(L);
    lua_pushcfunction(L, l_playNote);
    lua_setglobal(L, "playNote");
    lua_pushcfunction(L, l_setGroup);
    lua_setglobal(L, "setGroup");
    
    lua_pushcfunction(L, l_makeHorizontalSlider);
    lua_setglobal(L, "makeHorizontalSlider");
    lua_pushcfunction(L, l_makeVerticalSlider);
    lua_setglobal(L, "makeVerticalSlider");
    lua_pushcfunction(L, l_makeKnob);
    lua_setglobal(L, "makeKnob");
    lua_pushcfunction(L, l_makeButton);
    lua_setglobal(L, "makeButton");
    lua_pushcfunction(L, l_makeLabel);
    lua_setglobal(L, "makeLabel");
    
    lua_pushcfunction(L, l_setSize);
    lua_setglobal(L, "setSize");
    lua_pushcfunction(L, l_setPosition);
    lua_setglobal(L, "setPosition");
    lua_pushcfunction(L, l_getPageWidth);
    lua_setglobal(L, "getPageWidth");
    lua_pushcfunction(L, l_getWidth);
    lua_setglobal(L, "getWidth");
    lua_pushcfunction(L, l_getHeight);
    lua_setglobal(L, "getHeight");
    lua_pushcfunction(L, l_Hide);
    lua_setglobal(L, "Hide");
    lua_pushcfunction(L, l_Show);
    lua_setglobal(L, "Show");
    
    luaScript = this;
}

void LuaScript::loadScript(String f){
    if (staticSampler == nullptr)
        staticSampler = mapping_editor->getMappingEditor()->graph->getSamplerP();
        
    if (staticMainPage == nullptr)
        staticMainPage = mapping_editor->getMappingEditor()->graph->
                         getInstrument().getTabWindow().getMainPage();
    staticMainPage->resetComponents();
        
    if (luaL_loadfile(L, f.toRawUTF8()) || lua_pcall(L, 0, 0, 0)){
        std::cout<<"error: "<<lua_tostring(L, -1)<<std::endl;
        return;
    }
    
    lua_getglobal(L, "Initialize");
    if (lua_pcall(L, 0, 0, 0) != 0)
        std::cout<<"error running function `Initialize' : "<<lua_tostring(L, -1)<<std::endl;
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

void LuaScript::sliderValueChanged(Slider* s){
    lua_getglobal(L, String("on" + s->getName() + "Changed").toRawUTF8());
    lua_pushnumber(L, s->getValue());
    if (lua_pcall(L, 1, 0, 0) != 0)
        std::cout<<"error running function `on" + s->getName() + "Changed' : "<<lua_tostring(L, -1)<<std::endl;
}

void LuaScript::buttonClicked(Button* b){
    lua_getglobal(L, String("on" + b->getName() + "Clicked").toRawUTF8());
    lua_pushnumber(L, b->getToggleState());
    if (lua_pcall(L, 1, 0, 0) != 0)
        std::cout<<"error running function `on" + b->getName() + "Clicked' : "<<lua_tostring(L, -1)<<std::endl;
}
    