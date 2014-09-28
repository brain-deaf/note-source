/*
  ==============================================================================

    This file was auto-generated by the Jucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JuceDemoPluginAudioProcessorEditor::JuceDemoPluginAudioProcessorEditor (SamplerProcessor& owner)
	: AudioProcessorEditor(owner), mainContent(new MainContentComponent(&owner))
{
    // set our component's initial size to be the last one that was stored in the filter's settings
    setSize (1000,
              829);

    startTimer (50);

	addAndMakeVisible(mainContent.get());
	owner.prepareToPlay(44100.0, 0);
}

JuceDemoPluginAudioProcessorEditor::~JuceDemoPluginAudioProcessorEditor()
{
}

//==============================================================================
void JuceDemoPluginAudioProcessorEditor::paint (Graphics& g)
{
    g.setGradientFill (ColourGradient (Colours::white, 0, 0,
                                       Colours::grey, 0, (float) getHeight(), false));
    g.fillAll();
}

void JuceDemoPluginAudioProcessorEditor::resized()
{
    getProcessor().lastUIWidth = getWidth();
    getProcessor().lastUIHeight = getHeight();
	mainContent->setBounds(0, 0, getWidth(), getHeight());
}

//==============================================================================
// This timer periodically checks whether any of the filter's parameters have changed...
void JuceDemoPluginAudioProcessorEditor::timerCallback()
{
    SamplerProcessor& ourProcessor = getProcessor();

    /*AudioPlayHead::CurrentPositionInfo newPos (ourProcessor.lastPosInfo);

    if (lastDisplayedPosition != newPos)
        displayPositionInfo (newPos);

    gainSlider.setValue (ourProcessor.gain, dontSendNotification);
    delaySlider.setValue (ourProcessor.delay, dontSendNotification);*/
}

// This is our Slider::Listener callback, when the user drags a slider.
void JuceDemoPluginAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    /*if (slider == &gainSlider)
    {
        // It's vital to use setParameterNotifyingHost to change any parameters that are automatable
        // by the host, rather than just modifying them directly, otherwise the host won't know
        // that they've changed.
        getProcessor().setParameterNotifyingHost (JuceDemoPluginAudioProcessor::gainParam,
                                                  (float) gainSlider.getValue());
    }
    else if (slider == &delaySlider)
    {
        getProcessor().setParameterNotifyingHost (JuceDemoPluginAudioProcessor::delayParam,
                                                  (float) delaySlider.getValue());
    }*/
}


