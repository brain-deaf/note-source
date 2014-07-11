/*
  ==============================================================================

    FxBin.h
    Created: 6 Jul 2014 2:53:45pm
    Author:  patrick

  ==============================================================================
*/

#ifndef FXBIN_H_INCLUDED
#define FXBIN_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "FxComponent.h"
#include "FxSelector.h"
#include "GroupEditor.h"
#include "MappingEditorBin.h"

class FxBin : public Component {
public:
    FxBin(MappingEditorBin*);
    ~FxBin();
    void resized();
    void broughtToFront();
    FxComponent* getFxComponent(){return fx_component;}
    GroupEditor* getGroupEditor(){return group_editor;}
    FxSelector* getFxSelector(){return fx_selector;}
private:
    FxSelector* fx_selector;
    FxComponent* fx_component;
    int selector_height;
    GroupEditor* group_editor;
    MappingEditorBin* mapping_editor;
};



#endif  // FXBIN_H_INCLUDED
