/*
  ==============================================================================

    TransformBin.cpp
    Created: 11 Aug 2014 1:15:34pm
    Author:  patrick

  ==============================================================================
*/

#include "TransformBin.h"
#include "InstrumentBin.h"
#include "MainComponent.h"

TransformBin::TransformBin(MappingEditorBin* bin) : Component(), selector_height(150),
    fx_component(new TransformComponent()),
    mapping_editor{bin},
    group_editor(bin->getMappingEditor()->graph->getGroupEditor()),
    fx_selector(new TransformSelector(2, 6))
{
    addAndMakeVisible(fx_selector);
    addAndMakeVisible(fx_component);
    addAndMakeVisible(group_editor);
    group_editor->setTransformBin(this);
    metronome = mapping_editor->getInstrument()->getParent()->getParent()->getMetronome()->getMetronome();
}

TransformBin::~TransformBin(){
	for (auto i : fx_selector->getGroups()){
		TransformGroup* tf_group = i;
		for (auto fx : tf_group->group_fx){
			if (fx->getFxType() == TransformChooser::FX::LINEAR){
				LinearTransform* ltf = (LinearTransform*)fx->getContent();
				ltf->quit();
			}
		}
	}
    fx_selector = nullptr;
    fx_component = nullptr;
}

void TransformBin::resized(){
    fx_selector->setBounds(200, 0, getWidth() - 200, selector_height);
    fx_component->setBounds(200, selector_height + 25, getWidth(), getHeight() - (selector_height + 25)); 
    group_editor->setBounds(0, 0, 200, getHeight());
    group_editor->repaint();
}

void TransformBin::broughtToFront(){
    addAndMakeVisible(group_editor);
    resized();
}

void TransformBin::quit(){
    
}