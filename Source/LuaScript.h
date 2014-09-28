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

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#include "Metronome.h"
#include "SamplerEventProcessor.h"


class MappingEditorBin;
class ScriptBin;

class LuaScript : public Slider::Listener, public Button::Listener,
                  public ComboBox::Listener
{
public:

    enum LUA_TYPES{
        VSLIDER = 1,
        HSLIDER = 2,
        MENU    = 3,
        BUTTON  = 4,
        LABEL   = 5,
        KNOB    = 6
    };

    LuaScript(MappingEditorBin*, ScriptBin*);
    lua_State* getLuaState(){return L;}
    void loadScript(String);
	void clearScript();
    void onNote(int, double, double);
    void onBeat();
    int getLastPlayedNote(){return lastPlayedNote;}
    void sliderValueChanged(Slider*) override;
    void buttonClicked(Button*) override;
    void setBeatListener(bool b){beatListening=b;}
    void comboBoxChanged(ComboBox*) override;
    Array<Font>& getFonts(){return fonts;}
    int& getGuiId(){return guiId;}
    int getfield(const char*);
private:
	ScriptBin* scriptBin;
    lua_State* L;
    MappingEditorBin* mapping_editor;
    Metronome* metronome;
    bool beatListening;
    int lastPlayedNote;
    int guiId;
    Array<Font> fonts;
};


#endif  // LUASCRIPT_H_INCLUDED
