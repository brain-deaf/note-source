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

class MappingEditorBin;

class LuaScript : public Slider::Listener, public Button::Listener,
                  public ComboBox::Listener
{
public:
    LuaScript(MappingEditorBin*);
    lua_State* getLuaState(){return L;}
    void loadScript(String);
    void onNote(int, double, double);
    int getLastPlayedNote(){return lastPlayedNote;}
    void sliderValueChanged(Slider*) override;
    void buttonClicked(Button*) override;
    void comboBoxChanged(ComboBox*) override;
    Array<Font>& getFonts(){return fonts;}
private:
    lua_State* L;
    MappingEditorBin* mapping_editor;
    int lastPlayedNote;
    Array<Font> fonts;
};


#endif  // LUASCRIPT_H_INCLUDED
