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
    removeAllChildren();
    if (x == FxChooser::FX::ADSR){
        visibleFx = FxChooser::FX::ADSR;
        content = c;
    }
    else if (x == FxChooser::FX::FILTER){
        visibleFx = FxChooser::FX::FILTER;
        content = c;
    }
    else if (x == FxChooser::FX::RINGMOD){
        visibleFx = FxChooser::FX::RINGMOD;
        content = c;
    }
    else if (x == FxChooser::FX::NONE){
        visibleFx = FxChooser::FX::NONE;
        content = nullptr;
    }
    if (content != nullptr){
        addAndMakeVisible(content);
        resized();
    }
}

/*void FxComponent::showFx(){
    switch (visibleFx){
    case FxChooser::FX::ADSR:
        addAndMakeVisible(content);
        resized();
        break;
    
    case FxChooser::FX::FILTER:
        addAndMakeVisible(content);
        resized();
        break;
        
    case FxChooser::FX::RINGMOD:
        addAndMakeVisible(content);
        resized();
        break;
    
    case FxChooser::FX::NONE:
        break;
    }
}*/


void FxComponent::resized(){
    if (content != nullptr){content->setBounds(0, 0, getWidth(), getHeight());}
}
            