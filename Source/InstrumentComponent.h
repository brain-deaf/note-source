/*
  ==============================================================================

    InstrumentComponent.h
    Created: 9 May 2014 6:12:43pm
    Author:  patrick

  ==============================================================================
*/

#ifndef INSTRUMENTCOMPONENT_H_INCLUDED
#define INSTRUMENTCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "InstrumentTabWindow.h"
#include "InstrumentMasterComponent.h"
#include "FilePlayer.h"
class InstrumentBin;

class InstrumentComponent : public Component
{
public:
    InstrumentComponent(InstrumentBin*);
    void resized();
    void addFilePlayer(FilePlayer*);
    StringArray& getSampleNames() {return sampleNames;}
    InstrumentTabWindow& getTabWindow(){return tabs;}
    InstrumentBin* getParent(){return parent;}
private:
    SharedResourcePointer<AudioDeviceManager> deviceManager;
    AudioSourcePlayer sourcePlayer;
    MixerAudioSource mixer;
    Array<FilePlayer*> sources;
    StringArray sampleNames;
    InstrumentBin* parent;
    InstrumentTabWindow tabs;
    InstrumentMasterComponent master;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentComponent)
};



#endif  // INSTRUMENTCOMPONENT_H_INCLUDED
