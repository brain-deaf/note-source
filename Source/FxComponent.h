/*
  ==============================================================================

    FxComponent.h
    Created: 6 Jul 2014 2:55:54pm
    Author:  patrick

  ==============================================================================
*/

#ifndef FXCOMPONENT_H_INCLUDED
#define FXCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Adsr.h"

class FxComponent : public Component{
public:
    FxComponent();
    ~FxComponent();
    void paint(Graphics&);
    void loadFx(int, Component*);
    void showFx();
    void resized();
    void setVisibleFx(int i){visibleFx=i; showFx();}
    //void parentHierarchyChanged(){std::cout<<"parent changed"<<std::endl;}
private:
    int visibleFx;
    Component* content;
};


#endif  // FXCOMPONENT_H_INCLUDED
