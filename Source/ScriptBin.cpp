/*
  ==============================================================================

    ScriptBin.cpp
    Created: 30 Jul 2014 1:08:49pm
    Author:  patrick

  ==============================================================================
*/

#include "ScriptBin.h"

ScriptBin::ScriptBin() : codeDocument(new CodeDocument()), 
                       codeEditor(new CodeEditorComponent(*codeDocument, new LuaTokeniser())),
                       compileButton(new TextButton("compile")),
                       luaScript(new LuaScript())
{
    addAndMakeVisible(codeEditor);
    addAndMakeVisible(compileButton);
    compileButton->addListener(this);
}
                       
ScriptBin::~ScriptBin(){
    delete codeEditor;
    codeEditor = nullptr;
    delete compileButton;
    compileButton = nullptr;
    delete codeDocument;
    codeDocument = nullptr;
    delete luaScript;
    luaScript = nullptr;
    
}

void ScriptBin::paint(Graphics& g){
    g.fillAll(Colours::white);
}

void ScriptBin::resized(){
    codeEditor->setBounds(0, 20, getWidth(), getHeight());
    compileButton->setBounds(0, 0, 50, 20);
}

void ScriptBin::buttonClicked(Button* source){
    if (source == compileButton){
        File outFile(File::getCurrentWorkingDirectory().getFullPathName() + "/script.lua");
        outFile.replaceWithText(codeDocument->getAllContent());
        luaScript->loadScript(outFile.getFullPathName());
    }
}