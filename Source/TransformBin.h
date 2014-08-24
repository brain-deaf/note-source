/*
  ==============================================================================

    TransformBin.h
    Created: 11 Aug 2014 1:15:44pm
    Author:  patrick

  ==============================================================================
*/

#ifndef TRANSFORMBIN_H_INCLUDED
#define TRANSFORMBIN_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "TransformComponent.h"
#include "TransformSelector.h"
#include "GroupEditor.h"
#include "MappingEditorBin.h"

class TransformBin : public Component {
public:
    TransformBin(MappingEditorBin*);
    ~TransformBin();
    void resized();
    void broughtToFront();
    void quit();
    TransformComponent* getTransformComponent(){return fx_component;}
    GroupEditor* getGroupEditor(){return group_editor;}
    TransformSelector* getTransformSelector(){return fx_selector;}
    MappingEditorBin* getMappingEditor(){return mapping_editor;}
private:
    TransformSelector* fx_selector;
    TransformComponent* fx_component;
    int selector_height;
    GroupEditor* group_editor;
    MappingEditorBin* mapping_editor;
};



#endif  // TRANSFORMBIN_H_INCLUDED
