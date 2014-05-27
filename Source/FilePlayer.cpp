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

FilePlayer::FilePlayer(const String& sample_name): ChangeListener{},sample_name_{sample_name},
    device_manager{}, format_manager{}, transport_source{}, state_{Stopped} {

    format_manager.registerBasicFormats();
    source_player.setSource(&transport_source);
    device_manager->addAudioCallback(&source_player);
    device_manager->addChangeListener(this);
    transport_source.addChangeListener(this);
    File f(sample_name);
    auto r = format_manager.createReaderFor(f);
    if( r == nullptr) {
        throw BadFormatException("cannot play "+sample_name);
    }
    reader_source = new AudioFormatReaderSource(r,true);
    transport_source.setSource(reader_source);
    changeState(Starting);
}
FilePlayer::~FilePlayer() {
    changeState(Stopping);
    device_manager->removeAudioCallback(&source_player);
}

void FilePlayer::changeListenerCallback (ChangeBroadcaster* src) {
    if (&*(device_manager) == src) {
        AudioDeviceManager::AudioDeviceSetup setup;
        device_manager->getAudioDeviceSetup (setup);
        if (setup.outputChannels.isZero()) {
            source_player.setSource (nullptr);
        } else {
            source_player.setSource (&transport_source);
        }
    } else if (&transport_source == src) {
        if (transport_source.isPlaying()) {
            changeState (Playing);
        } else {
            if ((Stopping == state_) || (Playing == state_)) {
                changeState (Stopped);
            }
            else if (Pausing == state_) {
                changeState (Paused);
            }
        }
    }
}

void FilePlayer::changeState (TransportState newState) {
    if (state_ != newState) {
        state_ = newState;
        switch (state_) {
        case Stopped:
            transport_source.setPosition (0.0);
            break;
        case Starting:
            transport_source.start();
            break;
        case Playing:
            break;
        case Pausing:
            transport_source.stop();
            break;
        case Paused:
            break;
        case Stopping:
            transport_source.stop();
            break;
        }
    }
}

void FilePlayer::toggleState() {
    if (state_ == Stopped) {
        changeState(Starting);
    }
    else {
        changeState(Stopping);
    }
}
