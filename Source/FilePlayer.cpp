/*
  ==============================================================================

    FilePlayer.cpp
    Created: 27 May 2014 1:45:45pm
    Author:  allie

  ==============================================================================
*/
#include <stdexcept>
#include "FilePlayer.h"
class BadFormatException : public std::runtime_error{
public:
    BadFormatException(String s) : std::runtime_error(s.toStdString()){}
};

FilePlayer::FilePlayer(const String& s): ChangeListener{},
    sampleName{s}, deviceManager{}, formatManager{}, transportSource{},
    filter{&transportSource,false}, sourcePlayer{}, state{Stopped} {
    filter.setCoefficients (IIRCoefficients::makeHighPass (44100,1300));

    formatManager.registerBasicFormats();
    sourcePlayer.setSource (&filter);
    deviceManager->addAudioCallback (&sourcePlayer);
    deviceManager->addChangeListener (this);
    transportSource.addChangeListener (this);
    File f (sampleName);
    auto r = formatManager.createReaderFor(f);
    if( r == nullptr) {
        throw BadFormatException("cannot play "+sampleName);
        delete this;
    }else{
        readerSource = new AudioFormatReaderSource (r,true);
        transportSource.setSource (readerSource);
        //changeState (Starting);
    }
}
FilePlayer::~FilePlayer() {
    changeState(Stopping);
    deviceManager->removeAudioCallback(&sourcePlayer);
}

void FilePlayer::changeListenerCallback (ChangeBroadcaster* src) {
    if (&*(deviceManager) == src) {
        AudioDeviceManager::AudioDeviceSetup setup;
        deviceManager->getAudioDeviceSetup (setup);
        if (setup.outputChannels.isZero()) {
            sourcePlayer.setSource (nullptr);
        } else {
            sourcePlayer.setSource (&transportSource);
        }
    } else if (&transportSource == src) {
        if (transportSource.isPlaying()) {
            changeState (Playing);
        } else {
            if ((Stopping == state) || (Playing == state)) {
                changeState (Stopped);
                delete this;
            }
            else if (Pausing == state) {
                changeState (Paused);
            }
        }
    }
}

void FilePlayer::changeState (TransportState newState) {
    if (state != newState) {
        state = newState;
        switch (state) {
        case Stopped:
            transportSource.setPosition (0.0);
            break;
        case Starting:
            transportSource.start();
            break;
        case Playing:
            break;
        case Pausing:
            transportSource.stop();
            break;
        case Paused:
            break;
        case Stopping:
            transportSource.stop();
            break;
        }
    }
}

void FilePlayer::toggleState() {
    if (state == Stopped) {
        changeState(Starting);
    }
    else {
        changeState(Stopping);
    }
}

