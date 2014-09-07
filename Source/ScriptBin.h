/*
  ==============================================================================

    ScriptBin.h
    Created: 30 Jul 2014 1:09:06pm
    Author:  patrick

  ==============================================================================
*/

#ifndef SCRIPTBIN_H_INCLUDED
#define SCRIPTBIN_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "LuaScript.h"

class ScriptBin : public Component, Button::Listener{
public:
    ScriptBin(MappingEditorBin*);
    ~ScriptBin();
    void paint(Graphics&);
    void resized();
    LuaScript* getLuaScript(){return luaScript.get();}
    void buttonClicked(Button*);
    void loadPatch(XmlElement*);
    String getScriptPath(){return scriptPath;}
private:
    ScopedPointer<CodeDocument> codeDocument;
    ScopedPointer<CodeEditorComponent> codeEditor;
    ScopedPointer<TextButton> compileButton;
    ScopedPointer<TextButton> saveAsButton;
    ScopedPointer<LuaScript> luaScript;
    MappingEditorBin* mapping_editor;
    String scriptPath;
};





#endif  // SCRIPTBIN_H_INCLUDED
