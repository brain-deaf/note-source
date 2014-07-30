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

class ScriptBin : public Component{
public:
    ScriptBin();
    ~ScriptBin();
    void paint(Graphics&);
    void resized();
private:
    CodeDocument* codeDocument;
    CodeEditorComponent* codeEditor;
    TextButton* compileButton;
};





#endif  // SCRIPTBIN_H_INCLUDED
