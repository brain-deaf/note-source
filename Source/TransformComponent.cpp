/*
  ==============================================================================

    TransformComponent.cpp
    Created: 11 Aug 2014 1:16:05pm
    Author:  patrick

  ==============================================================================
*/

#include "TransformComponent.h"
#include "TransformSelector.h"

TransformComponent::TransformComponent() : Component(), visibleFx(-1), content(nullptr){}

TransformComponent::~TransformComponent(){}

void TransformComponent::paint(Graphics& g){
    g.fillAll(Colours::orange);
}

void TransformComponent::loadFx(int x, Component* c){
    if (content != nullptr) removeChildComponent(content);
    if (x == TransformChooser::FX::LINEAR){
        visibleFx = TransformChooser::FX::LINEAR;
        content = c;
    }
    else if (x == TransformChooser::FX::FILTER){
        visibleFx = TransformChooser::FX::FILTER;
        content = c;
    }
    else if (x == TransformChooser::FX::NONE){
        visibleFx = TransformChooser::FX::NONE;
    }
    showFx();
}

void TransformComponent::showFx(){
    switch (visibleFx){
    case TransformChooser::FX::LINEAR:
        addAndMakeVisible(content);
        resized();
        break;
    
    case TransformChooser::FX::FILTER:
        addAndMakeVisible(content);
        resized();
        break;
    
    case TransformChooser::FX::NONE:
        break;
    }
}


void TransformComponent::resized(){
    if (content != nullptr) content->setBounds(0, 0, getWidth()-200, getHeight());
}