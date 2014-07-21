/*
  ==============================================================================

    IIR_Filter.h
    Created: 19 Jul 2014 11:48:21am
    Author:  patrick

  ==============================================================================
*/

#ifndef IIR_FILTER_H_INCLUDED
#define IIR_FILTER_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"
#include <complex>
#include <fftw3.h>

class IIR_Filter : public IIRFilter{
public:
    void setCoefficients(const IIRCoefficients&);
    Array<double> getImpulseResponse(){return impulseResponse;}
    Array<double> getInterpolatedImpulseResponse(){return interpolatedImpulseResponse;}
    Array<double> getFrequencyResponse(){return frequencyResponse;}
    int getZeroHeadingSize(){return zero_heading_size;}
private:
    Array<double> impulseResponse;
    Array<double> interpolatedImpulseResponse;
    Array<double> frequencyResponse;
    int zero_heading_size;
};



#endif  // IIR_FILTER_H_INCLUDED
