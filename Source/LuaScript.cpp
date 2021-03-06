/*
  ==============================================================================

    LuaScript.cpp
    Created: 30 Jul 2014 1:55:47pm
    Author:  patrick

  ==============================================================================
*/

#include "LuaScript.h"
#include "Sampler.h"
#include "SamplerEventProcessor.h"
#include "MappingEditorBin.h"
#include "MainPage.h"
#include <memory>
#include "MainPageComponents.h"
#include "InstrumentComponent.h"
#include "InstrumentBin.h"
#include "MainComponent.h"
#include "ScriptBin.h"


static LuaScript* luaScript = nullptr;
static SamplerProcessor* staticSampler = nullptr;
static MainPage* staticMainPage = nullptr;

static int l_playNote(lua_State* L){
	if (staticSampler == nullptr)
		staticSampler = MainContentComponent::_static_sampler;

	if (staticSampler == nullptr)
		return 0;

    double note = lua_tonumber(L, 1);
    double velocity = lua_tonumber(L, 2);
    double timestamp = lua_tonumber(L, 3);
    double triggered_note = lua_tonumber(L, 4);
    lua_pop(L, 4);
    
    MidiMessage m = MidiMessage(MidiMessage::noteOn(1, (int)note, (float)velocity));
    m.setTimeStamp(timestamp);
    
    auto n = std::make_shared<NoteEvent>();
    n->setTriggerNote(triggered_note);
    n->setNoteNumber(note);
    n->setVelocity(velocity);
    n->setId(staticSampler->getIdCount());
    
    lua_pushnumber(L, staticSampler->getIdCount());
    
    staticSampler->incIdCount();
    
    for (int i=0; i<staticSampler->getSynth()->getNumSounds(); i++){
        if (staticSampler->getSynth()->getSound(i)->appliesToNote(note)){
            staticSampler->getIncomingEvents().add(n);
        }
    }
    //staticSampler->getMidiCollector().addMessageToQueue(*m);
	staticSampler->getMidiBuffer().addEvent(m, (int)m.getTimeStamp());
    
    return 1;
}

static int l_makeSamplerEvent(lua_State* L){
    double note = lua_tonumber(L, 1);
    double velocity = lua_tonumber(L, 2);
    double start_time = lua_tonumber(L, 3);
    int group = lua_tonumber(L, 4);
    lua_pop(L, 4);
    
    SamplerEvent s;
    s.setNoteNumber(note);
    s.setVelocity(velocity);
    s.setStart(start_time);
    s.getGroups().add(group);
    
    staticSampler->getSamplerProcessor()->addSamplerEvent(s);
    int id_count = staticSampler->getSamplerProcessor()->getNumEvents() - 1;
    lua_pushnumber(L, id_count);
    
    return 1;
}

static int l_renderAllEvents(lua_State* ){
    staticSampler->setupRendering();
    staticSampler->getSamplerProcessor()->renderAllEvents();
    
    return 0;
}

static int l_setGroup(lua_State* L){
    int id = lua_tonumber(L, 1);
    int group = lua_tonumber(L, 2);
    lua_pop(L, 2);
    
    for (auto e : staticSampler->getIncomingEvents()){
        if (e->getId() == id){
            e->getGroups().add(group);
        }
    }
    
    return 0;
}

static int l_setEventVolume(lua_State* L){
    int id = lua_tonumber(L, 1);
    lua_pop(L, 1);
    double volume = lua_tonumber(L, 2);
    if (volume < 0.0) volume = 0.0;
    
    for (auto e : staticSampler->getIncomingEvents()){
        if (e != nullptr){
            if (e->getId() == id){
                e->setVolume(volume);
                return 0;
            }
        }
    }
    for (auto e : staticSampler->getEvents()){
        if (e != nullptr){
            if (e->getId() == id){
                e->setVolume(volume);
                return 0;
            }
        }
    }
    
    return 0;
}

static int l_makeTable(lua_State* L){
    double min = lua_tonumber(L, 1);
    const char* name = lua_tostring(L, 2);
    
    lua_newtable(L);
    
    lua_pushstring(L, "id");
    lua_pushnumber(L, min);
    lua_settable(L, -3);
    
    lua_pushstring(L, "name");
    lua_pushstring(L, name);
    lua_settable(L, -3);
    
    return 1;
}

static int l_getTable(lua_State* L){
    //int guiId;
    if (!lua_istable(L, -1)){
        std::cout<<"first parameter must be a table!"<<std::endl;
        return 0;
    }else{
        lua_pushstring(L, "id");
        lua_gettable(L, -2);
        //int id = lua_tonumber(L, -1);
        
        lua_pop(L, 1);
    }
    
    return 0;
}

static int l_makeHorizontalSlider(lua_State* L){
    const char* name = lua_tostring(L, 1);
    double min = lua_tonumber(L, 2);
    double max = lua_tonumber(L, 3);
    double interval = lua_tonumber(L, 4);
    lua_pop(L, 4);
    
    auto s = new MainHorizontalSlider(name, luaScript->getGuiId(), min, max, interval);
    s->setSize(250, 30);
    s->addListener(luaScript);
    
    staticMainPage->getComponents()[luaScript->getGuiId()] = s;
    staticMainPage->addNewComponent(luaScript->getGuiId());
    
    lua_newtable(L);
    
    lua_pushstring(L, "id");
    lua_pushnumber(L, luaScript->getGuiId());
    lua_settable(L, -3);
    
    lua_pushstring(L, "type");
    lua_pushnumber(L, LuaScript::LUA_TYPES::HSLIDER);
    lua_settable(L, -3);
    luaScript->getGuiId()++;
    
    return 1;
}

static int l_makeVerticalSlider(lua_State* L){
    const char* name = lua_tostring(L, 1);
    double min = lua_tonumber(L, 2);
    double max = lua_tonumber(L, 3);
    double interval = lua_tonumber(L, 4);
    lua_pop(L, 4);
    
    auto s = new MainVerticalSlider(name, luaScript->getGuiId(), min, max, interval);
    s->setSize(30, 250);
    s->addListener(luaScript);
    
    staticMainPage->getComponents()[luaScript->getGuiId()] = s;
    staticMainPage->addNewComponent(luaScript->getGuiId());
    
    lua_newtable(L);
    
    lua_pushstring(L, "id");
    lua_pushnumber(L, luaScript->getGuiId());
    lua_settable(L, -3);
    
    lua_pushstring(L, "type");
    lua_pushnumber(L, LuaScript::LUA_TYPES::VSLIDER);
    lua_settable(L, -3);
    luaScript->getGuiId()++;
    
    return 1;
}



static int l_makeKnob(lua_State* L){
    const char* name = lua_tostring(L, 1);
    double min = lua_tonumber(L, 2);
    double max = lua_tonumber(L, 3);
    double interval = lua_tonumber(L, 4);
    lua_pop(L, 4);
    
    auto s = new MainKnob(name, luaScript->getGuiId(), min, max, interval);
    s->setSize(50, 50);
    s->addListener(luaScript);
    
    staticMainPage->getComponents()[luaScript->getGuiId()] = s;
    staticMainPage->addNewComponent(luaScript->getGuiId());
    
    lua_newtable(L);
    
    lua_pushstring(L, "id");
    lua_pushnumber(L, luaScript->getGuiId());
    lua_settable(L, -3);
    
    lua_pushstring(L, "type");
    lua_pushnumber(L, LuaScript::LUA_TYPES::KNOB);
    lua_settable(L, -3);
    
    luaScript->getGuiId()++;
    
    return 1;
}

static int l_makeButton(lua_State* L){
    const char* name = lua_tostring(L, 1);
    bool toggle = lua_toboolean(L, 2)!=0;
    lua_pop(L, 2);
    
    auto s = new MainButton(name, luaScript->getGuiId());
    s->setSize(80, 50);
    s->addListener(luaScript);
    s->setClickingTogglesState(toggle);
    
    staticMainPage->getComponents()[luaScript->getGuiId()] = s;
    staticMainPage->addNewComponent(luaScript->getGuiId());
    
    lua_newtable(L);
    
    lua_pushstring(L, "id");
    lua_pushnumber(L, luaScript->getGuiId());
    lua_settable(L, -3);
    
    lua_pushstring(L, "type");
    lua_pushnumber(L, LuaScript::LUA_TYPES::BUTTON);
    lua_settable(L, -3);
    
    luaScript->getGuiId()++;
    
    return 1;
}

static int l_makeLabel(lua_State* L){
    String text = lua_tostring(L, 1);
    lua_pop(L, 1);
    
    auto s = new MainLabel(luaScript->getGuiId(), text);
    s->setSize(150, 15);
    
    staticMainPage->getComponents()[luaScript->getGuiId()] = s;
    staticMainPage->addNewComponent(luaScript->getGuiId());
    
    lua_newtable(L);
    
    lua_pushstring(L, "id");
    lua_pushnumber(L, luaScript->getGuiId());
    lua_settable(L, -3);
    
    lua_pushstring(L, "type");
    lua_pushnumber(L, LuaScript::LUA_TYPES::LABEL);
    lua_settable(L, -3);
    
    luaScript->getGuiId()++;
    
    return 1;
}

static int l_makeComboBox(lua_State* L){
    const char* name = lua_tostring(L, 1);
    lua_pop(L, 1);
    
    auto s = new MainComboBox(name, luaScript->getGuiId());
    s->addListener(luaScript);
    s->setSize(160, 25);
    
    staticMainPage->getComponents()[luaScript->getGuiId()] = s;
    staticMainPage->addNewComponent(luaScript->getGuiId());
    
    lua_newtable(L);
    
    lua_pushstring(L, "id");
    lua_pushnumber(L, luaScript->getGuiId());
    lua_settable(L, -3);
    
    lua_pushstring(L, "type");
    lua_pushnumber(L, LuaScript::LUA_TYPES::MENU);
    lua_settable(L, -3);
    
    luaScript->getGuiId()++;
    
    return 1;
}

static int l_addComboBoxItem(lua_State* L){
    String itemstr = lua_tostring(L, 2);
    int id         = lua_tonumber(L, 3);
    int guiId;
    if (!lua_istable(L, 1)){
        std::cout<<"first parameter must be a table!"<<std::endl;
        return 0;
    }else{
        lua_pushstring(L, "id");
        lua_gettable(L, -4);
        guiId = lua_tonumber(L, -1);
    }
    
    MainComboBox* c = (MainComboBox*)(staticMainPage->getComponents()[guiId]);
    c->addItem(itemstr, id);
    
    lua_pop(L, 4);
    return 0;
}
    

static int l_setFont(lua_State* L){
    int font = lua_tonumber(L, 2);
    if (font < 0) font = 0;
    if (font > luaScript->getFonts().size())
        font = luaScript->getFonts().size() - 1;
    int guiId;
    if (!lua_istable(L, 1)){
        std::cout<<"first parameter must be a table!"<<std::endl;
        return 0;
    }else{
        lua_pushstring(L, "id");
        lua_gettable(L, -3);
        guiId = lua_tonumber(L, -1);
    }
    
    MainLabel* l = (MainLabel*)(staticMainPage->getComponents()[guiId]);
    if (l != nullptr)
        l->setFont(luaScript->getFonts()[font]);
    
    lua_pop(L, 3);
    return 0;
}

static int l_setSize(lua_State* L){
    double width = lua_tonumber(L, 2);
    double height = lua_tonumber(L, 3);
    
    int guiId;
    if (!lua_istable(L, 1)){
        std::cout<<"first parameter must be a table!"<<std::endl;
        return 0;
    }else{
        lua_pushstring(L, "id");
        lua_gettable(L, -4);
        guiId = lua_tonumber(L, -1);
    }
    
    Component* c = staticMainPage->getComponents()[guiId];
    if (c != nullptr)
        c->setSize(width, height);
    
    lua_pop(L, 4);
    return 0;
}

static int l_setPosition(lua_State* L){
    double x = lua_tonumber(L, 2);
    double y= lua_tonumber(L, 3);
    
    int guiId;
    if (!lua_istable(L, 1)){
        std::cout<<"first parameter must be a table!"<<std::endl;
        return 0;
    }else{
        lua_pushstring(L, "id");
        lua_gettable(L, -4);
        guiId = lua_tonumber(L, -1);
    }
    
    Component* c = staticMainPage->getComponents()[guiId];
    if (c != nullptr)
        c->setTopLeftPosition(x, y);
    
    lua_pop(L, 4);
    return 0;
}

static int l_getPageWidth(lua_State* L){
    lua_pushnumber(L, staticMainPage->getWidth());
    return 1;
}

static int l_getWidth(lua_State* L){
    int guiId;
    if (!lua_istable(L, 1)){
        std::cout<<"first parameter must be a table!"<<std::endl;
        return 0;
    }else{
        lua_pushstring(L, "id");
        lua_gettable(L, -2);
        guiId = lua_tonumber(L, -1);
    }
    
    Component* c = staticMainPage->getComponents()[guiId];
    lua_pop(L, 1);
    if (c != nullptr){
        lua_pushnumber(L, staticMainPage->getComponents()[guiId]->getWidth());
    }else{
        lua_pushnumber(L, -1);
    }
    
    return 1;
}

static int l_getHeight(lua_State* L){
    int guiId;
    if (!lua_istable(L, 1)){
        std::cout<<"first parameter must be a table!"<<std::endl;
        return 0;
    }else{
        lua_pushstring(L, "id");
        lua_gettable(L, -2);
        guiId = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    Component* c = staticMainPage->getComponents()[guiId];
    
    if (c != nullptr){
        lua_pushnumber(L, staticMainPage->getComponents()[guiId]->getHeight());
    }else{
        lua_pushnumber(L, -1);
    }
    return 1;
}

static int l_Hide(lua_State* L){
    int guiId;
    if (!lua_istable(L, 1)){
        std::cout<<"first parameter must be a table!"<<std::endl;
        return 0;
    }else{
        lua_pushstring(L, "id");
        lua_gettable(L, -2);
        guiId = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    Component* c = staticMainPage->getComponents()[guiId];
    
    if (c != nullptr){
        c->setVisible(false);
    }
    return 0;
}

static int l_Show(lua_State* L){
    int guiId;
    if (!lua_istable(L, 1)){
        std::cout<<"first parameter must be a table!"<<std::endl;
        return 0;
    }else{
        lua_pushstring(L, "id");
        lua_gettable(L, -2);
        guiId = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    Component* c = staticMainPage->getComponents()[guiId];
    
    if (c != nullptr){
        c->setVisible(true);
    }
    return 0;
}

static int l_setBeatListener(lua_State* L){
    bool b = lua_toboolean(L, 1)!=0;
    lua_pop(L, 1);

    luaScript->setBeatListener(b);
    
    return 0;
}

static int l_setImage(lua_State* L){
    String imageName = lua_tostring(L, 2);
    int numFrames = lua_tonumber(L, 3);
    
    int guiId;
    int type;
    if (!lua_istable(L, 1)){
        std::cout<<"first parameter must be a table!"<<std::endl;
        return 0;
    }else{
        lua_pushstring(L, "id");
        lua_gettable(L, -4);
        guiId = lua_tonumber(L, -1);
        lua_pushstring(L, "type");
        lua_gettable(L, -5);
        type = lua_tonumber(L, -1);
    }
    lua_pop(L, 4);

    Component* c = staticMainPage->getComponents()[guiId];
    
    switch (type){
        case LuaScript::LUA_TYPES::VSLIDER :{
            MainVerticalSlider* m = static_cast<MainVerticalSlider*>(c);
            m->setImage(imageName, numFrames);
        }
        case LuaScript::LUA_TYPES::HSLIDER :{
            MainHorizontalSlider* m = static_cast<MainHorizontalSlider*>(c);
            m->setImage(imageName, numFrames);
        }
    }
    
    return 0;
}

static int l_sleep(lua_State* L){
    int n = lua_tonumber(L, 1);
    lua_pop(L, 1);

    Thread::sleep(n);
    
    return 0;
}

LuaScript::LuaScript(MappingEditorBin* m, ScriptBin* s) : L(nullptr), mapping_editor(m), metronome(nullptr),
                                            guiId(1), lastPlayedNote(0), beatListening(false), scriptBin(s){
    L = luaL_newstate();
    luaL_openlibs(L);
    
    lua_pushcfunction(L, l_makeTable);
    lua_setglobal(L, "makeTable");
    lua_pushcfunction(L, l_getTable);
    lua_setglobal(L, "getTable");
    
    lua_pushcfunction(L, l_makeSamplerEvent);
    lua_setglobal(L, "makeSamplerEvent");
    lua_pushcfunction(L, l_renderAllEvents);
    lua_setglobal(L, "renderAllEvents");
    
    lua_pushcfunction(L, l_setBeatListener);
    lua_setglobal(L, "setBeatListener");
    
    lua_pushcfunction(L, l_sleep);
    lua_setglobal(L, "Sleep");
    
    lua_pushcfunction(L, l_playNote);
    lua_setglobal(L, "playNote");
    lua_pushcfunction(L, l_setGroup);
    lua_setglobal(L, "setGroup");
    lua_pushcfunction(L, l_setEventVolume);
    lua_setglobal(L, "setEventVolume");
    
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
    lua_pushcfunction(L, l_makeComboBox);
    lua_setglobal(L, "makeComboBox");
    lua_pushcfunction(L, l_addComboBoxItem);
    lua_setglobal(L, "addComboBoxItem");
    lua_pushcfunction(L, l_setFont);
    lua_setglobal(L, "setFont");
    lua_pushcfunction(L, l_setImage);
    lua_setglobal(L, "setImage");
    
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
    
    Font::findFonts(fonts);
    
    metronome = mapping_editor->getMappingEditor()->graph
                ->getInstrument().getParent()->getParent()
                ->getMetronome()->getMetronome();
                
    MetronomeVoice* metrovoice = static_cast<MetronomeVoice*>(metronome->getSynth()->getVoice(0));
    metrovoice->setLuaScript(this);
    
    
    luaScript = this;
}

void LuaScript::clearScript(){
	if (staticSampler == nullptr)
		staticSampler = MainContentComponent::_static_sampler;

	if (staticMainPage == nullptr)
		staticMainPage = scriptBin->getTabWindow()->getMainPage();

	if (staticMainPage != nullptr)
		staticMainPage->resetComponents();

	static SamplerProcessor* s(MainContentComponent::_static_sampler);
	if (s != nullptr && s->getSamplerProcessor() != nullptr){
		if (s->getSamplerProcessor()->getNumEvents() > 0)
			s->getSamplerProcessor()->clearAllSamplerEvents();
	}
}

void LuaScript::loadScript(String f){
    if (luaL_loadfile(L, f.toRawUTF8()) || lua_pcall(L, 0, 0, 0)){
        std::cout<<"error: "<<lua_tostring(L, -1)<<std::endl;
        return;
    }
    
    lua_getglobal(L, "Initialize");
    if (lua_pcall(L, 0, 0, 0) != 0)
        std::cout<<"error running function `Initialize' : "<<lua_tostring(L, -1)<<std::endl;
}

int LuaScript::getfield(const char* key){
    int result;
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    if (!lua_isnumber(L, -1))
        std::cout<<"field value isn't a number"<<std::endl;
    result = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    return result;
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

void LuaScript::onBeat(){
    
    if (beatListening){
        lua_getglobal(L, "onBeat");
        if (lua_pcall(L, 0, 0, 0) != 0)
            std::cout<<"error running function `onBeat' : "<<lua_tostring(L, -1)<<std::endl;
    }
}

void LuaScript::sliderValueChanged(Slider* s){
    lua_getglobal(L, String("on" + s->getName() + "Changed").toRawUTF8());
    lua_pushnumber(L, s->getValue());
    if (lua_pcall(L, 1, 0, 0) != 0){
        std::cout<<"error running function `on" + s->getName() + "Changed' : "<<lua_tostring(L, -1)<<std::endl;
        lua_pop(L, 1);
    }
}

void LuaScript::buttonClicked(Button* b){
    lua_getglobal(L, String("on" + b->getName() + "Clicked").toRawUTF8());
    lua_pushnumber(L, b->getToggleState());
    if (lua_pcall(L, 1, 0, 0) != 0){
        std::cout<<"error running function `on" + b->getName() + "Clicked' : "<<lua_tostring(L, -1)<<std::endl;
        lua_pop(L, 1);
    }
        
}

void LuaScript::comboBoxChanged(ComboBox* c){
    lua_getglobal(L, String("on" + c->getName() + "Changed").toRawUTF8());
    lua_pushnumber(L, c->getSelectedId());
    if (lua_pcall(L, 1, 0, 0) != 0){
        std::cout<<"error running function `on" + c->getName() + "Changed' : "<<lua_tostring(L, -1)<<std::endl;
        lua_pop(L, 1);
    }
}
    