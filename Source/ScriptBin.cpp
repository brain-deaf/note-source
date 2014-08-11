/*
  ==============================================================================

    ScriptBin.cpp
    Created: 30 Jul 2014 1:08:49pm
    Author:  patrick

  ==============================================================================
*/

#include "ScriptBin.h"

ScriptBin::ScriptBin(MappingEditorBin* m) : codeDocument(new CodeDocument()), 
                       codeEditor(new CodeEditorComponent(*codeDocument, new LuaTokeniser())),
                       compileButton(new TextButton("compile")),
                       saveAsButton(new TextButton("save as...")),
                       luaScript(new LuaScript(m)),
                       mapping_editor(m),
                       scriptPath(File::getCurrentWorkingDirectory().getFullPathName() + "/script.lua")
{
    addAndMakeVisible(codeEditor);
    addAndMakeVisible(compileButton);
    compileButton->addListener(this);
    addAndMakeVisible(saveAsButton);
    saveAsButton->addListener(this);
    
    File f(scriptPath);
    FileInputStream* stream = new FileInputStream(f);
    codeDocument->loadFromStream(*stream);
    buttonClicked(compileButton);
    
    delete stream;
    stream = nullptr;
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
    delete saveAsButton;
    saveAsButton = nullptr;
}

void ScriptBin::paint(Graphics& g){
    g.fillAll(Colours::white);
}

void ScriptBin::resized(){
    codeEditor->setBounds(0, 20, getWidth(), getHeight());
    compileButton->setBounds(0, 0, 50, 20);
    saveAsButton->setBounds(53, 0, 70, 20);
}

void ScriptBin::buttonClicked(Button* source){
    if (source == compileButton){
        File outFile(scriptPath);
        outFile.replaceWithText(codeDocument->getAllContent());
        luaScript->loadScript(outFile.getFullPathName());
    }
    
    if (source == saveAsButton){
        FileChooser script_saver("Please select the patch destination.",
                                File::getCurrentWorkingDirectory(),
                                "*.lua");
            
        if (script_saver.browseForFileToOpen()){
            File script = script_saver.getResult();
            scriptPath = script.getFullPathName();
            File outFile(scriptPath);
            outFile.replaceWithText(codeDocument->getAllContent());
            luaScript->loadScript(outFile.getFullPathName());
        }
    }
}

void ScriptBin::loadPatch(XmlElement* xml){
   forEachXmlChildElement(*xml, element){
     if (element->hasTagName("SCRIPT")){
        scriptPath = element->getStringAttribute("path");
        
        File f(scriptPath);
        FileInputStream* stream = new FileInputStream(f);
        codeDocument->loadFromStream(*stream);
        buttonClicked(compileButton);
    
        delete stream;
        stream = nullptr;
     }
   }
}    