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

class ScriptBin : public Component{
public:
    ScriptBin();
    ~ScriptBin();
    void paint(Graphics&);
    void resized();
    LuaScript* getLuaScript(){return luaScript;}
private:
    CodeDocument* codeDocument;
    CodeEditorComponent* codeEditor;
    TextButton* compileButton;
    LuaScript* luaScript;
};





#endif  // SCRIPTBIN_H_INCLUDED
