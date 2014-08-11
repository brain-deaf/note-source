/*
  ==============================================================================

    TransformComponent.h
    Created: 11 Aug 2014 1:16:14pm
    Author:  patrick

  ==============================================================================
*/

#ifndef TRANSFORMCOMPONENT_H_INCLUDED
#define TRANSFORMCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Adsr.h"

class TransformComponent : public Component{
public:
    TransformComponent();
    ~TransformComponent();
    void paint(Graphics&);
    void loadFx(int, Component*);
    void showFx();
    void resized();
    void setVisibleFx(int i){visibleFx=i; showFx();}
private:
    int visibleFx;
    Component* content;
};



#endif  // TRANSFORMCOMPONENT_H_INCLUDED
