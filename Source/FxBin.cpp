/*
  ==============================================================================

    FxBin.cpp
    Created: 6 Jul 2014 2:53:34pm
    Author:  patrick

  ==============================================================================
*/

#include "FxBin.h"

FxBin::FxBin(MappingEditorBin* bin) : Component(), selector_height(150),  
    fx_selector(new FxSelector(2, 6)), 
    fx_component(new FxComponent()),
    mapping_editor{bin},
    group_editor(bin->getMappingEditor()->graph->getGroupEditor())
{
    addAndMakeVisible(fx_selector);
    addAndMakeVisible(fx_component);
    addAndMakeVisible(group_editor);
}

FxBin::~FxBin(){
    delete fx_selector;
    delete fx_component;
}

void FxBin::resized(){
    fx_selector->setBounds(200, 0, getWidth() - 200, selector_height);
    fx_component->setBounds(200, selector_height + 30, getWidth(), getHeight() - (selector_height + 30)); 
    group_editor->setBounds(0, 0, 200, getHeight());
    group_editor->repaint();
}

void FxBin::broughtToFront(){
    addAndMakeVisible(group_editor);
    resized();
}