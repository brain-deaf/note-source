/*
  ==============================================================================

    FilePlayer.h
    Created: 27 May 2014 1:45:45pm
    Author:  allie

  ==============================================================================
*/

#ifndef FILEPLAYER_H_INCLUDED
#define FILEPLAYER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class FilePlayer : public ChangeListener {
public:
    FilePlayer(const String& sample_name);
    ~FilePlayer();
    enum TransportState {
        Stopped,
        Starting,
        Playing,
        Pausing,
        Paused,
        Stopping
    };
    virtual void changeListenerCallback (ChangeBroadcaster* source) override; 
    void changeState (TransportState newState);
    TransportState getState() {return state;}
    AudioTransportSource& getTransportSource() {return transportSource;}
    String& getSampleName() { return sampleName;}
    void toggleState();
private:
    String sampleName;
    SharedResourcePointer<AudioDeviceManager> deviceManager;
    ScopedPointer<AudioFormatReaderSource> readerSource;
    AudioFormatManager formatManager;
    AudioTransportSource transportSource;
    IIRFilterAudioSource filter;
    AudioSourcePlayer sourcePlayer;
    TransportState state;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilePlayer)
};

#endif  // FILEPLAYER_H_INCLUDED
