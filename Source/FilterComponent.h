/*
  ==============================================================================

    FilterComponent.h
    Created: 19 Jul 2014 12:32:41pm
    Author:  patrick

  ==============================================================================
*/

#ifndef FILTERCOMPONENT_H_INCLUDED
#define FILTERCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "IIR_Filter.h"

class FilterComponent : public Component{
public:
    FilterComponent();
    void paint(Graphics&);
    void setIIRFilter(IIR_Filter* i){filter=i; repaint();std::cout<<"set filter for filter component"<<std::endl;}
private:
    IIR_Filter* filter;
};




#endif  // FILTERCOMPONENT_H_INCLUDED
