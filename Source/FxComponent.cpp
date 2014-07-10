/*
  ==============================================================================

    FxComponent.cpp
    Created: 6 Jul 2014 2:55:40pm
    Author:  patrick

  ==============================================================================
*/

#include "FxComponent.h"
#include "FxSelector.h"

FxComponent::FxComponent() : Component(), adsr(new Adsr()), visibleFx(-1){
    addChildComponent(adsr);
}

FxComponent::~FxComponent(){
    delete adsr;
}

void FxComponent::paint(Graphics& g){
    g.fillAll(Colours::orange);
}

void FxComponent::loadFx(int x){
    if (x == FxChooser::FX::ADSR){
        visibleFx = FxChooser::FX::ADSR;
    }
    if (x == FxChooser::FX::NONE){
        visibleFx = FxChooser::FX::NONE;
    }
    showFx();
}

void FxComponent::showFx(){
    switch (visibleFx){
    case FxChooser::FX::ADSR:
        std::cout<<adsr<<std::endl;
        if (!adsr->isVisible()) adsr->setVisible(true);
        break;
    
    case FxChooser::FX::NONE:
        if (adsr->isVisible()) adsr->setVisible(false);
        break;
    }
}


void FxComponent::resized(){
    adsr->setBounds(0, 0, getWidth(), getHeight());
}
            