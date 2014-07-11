/*
  ==============================================================================

    FxComponent.cpp
    Created: 6 Jul 2014 2:55:40pm
    Author:  patrick

  ==============================================================================
*/

#include "FxComponent.h"
#include "FxSelector.h"

FxComponent::FxComponent() : Component(), visibleFx(-1), content(nullptr){}

FxComponent::~FxComponent(){}

void FxComponent::paint(Graphics& g){
    g.fillAll(Colours::orange);
}

void FxComponent::loadFx(int x, Component* c){
    if (content != nullptr) removeChildComponent(content);
    if (x == FxChooser::FX::ADSR){
        visibleFx = FxChooser::FX::ADSR;
        content = c;
    }
    if (x == FxChooser::FX::NONE){
        visibleFx = FxChooser::FX::NONE;
    }
    showFx();
}

void FxComponent::showFx(){
    switch (visibleFx){
    case FxChooser::FX::ADSR:
        addAndMakeVisible(content);
        resized();
        break;
    
    case FxChooser::FX::NONE:
        break;
    }
}


void FxComponent::resized(){
    if (content != nullptr) content->setBounds(0, 0, getWidth(), getHeight());
}
            