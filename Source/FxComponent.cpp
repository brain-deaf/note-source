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
    std::cout<<"desructor..."<<std::endl;
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
    std::cout<<"show fx"<<std::endl;
    switch (visibleFx){
    case FxChooser::FX::ADSR:
        std::cout<<"pre adsr"<<std::endl;
        std::cout<<adsr<<std::endl;
        if (!adsr->isVisible()) adsr->setVisible(true);
        std::cout<<"adsr"<<std::endl;
        break;
    
    case FxChooser::FX::NONE:
        std::cout<<"none"<<std::endl;
        if (adsr->isVisible()) adsr->setVisible(false);
        break;
    }
}


void FxComponent::resized(){
    std::cout<<"resized"<<std::endl;
    adsr->setBounds(0, 0, getWidth(), getHeight());
}
            