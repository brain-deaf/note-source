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
    TransportState state() {return state_;}
    AudioTransportSource& transportSource() {return transport_source;}
    String& sampleName() { return sample_name_;}
    void toggleState();
private:
    String sample_name_;
    SharedResourcePointer<AudioDeviceManager> device_manager;
    ScopedPointer<AudioFormatReaderSource> reader_source;
    AudioFormatManager format_manager;
    AudioTransportSource transport_source;
    AudioSourcePlayer source_player;
    TransportState state_;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilePlayer)
};




#endif  // FILEPLAYER_H_INCLUDED
