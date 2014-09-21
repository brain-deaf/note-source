/*
  ==============================================================================

    This file was auto-generated by the Jucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#ifndef __PLUGINEDITOR_H_4ACCBAA__
#define __PLUGINEDITOR_H_4ACCBAA__

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "MainComponent.h"


//==============================================================================
/** This is the editor component that our filter will display.
*/
class JuceDemoPluginAudioProcessorEditor  : public AudioProcessorEditor,
                                            public SliderListener,
                                            public Timer
{
public:
    JuceDemoPluginAudioProcessorEditor (SamplerProcessor&);
    ~JuceDemoPluginAudioProcessorEditor();

    //==============================================================================
    void timerCallback() override;
    void paint (Graphics&) override;
    void resized() override;
    void sliderValueChanged (Slider*) override;

private:
    SamplerProcessor& getProcessor() const
    {
        return static_cast<SamplerProcessor&> (processor);
    }
	ScopedPointer<MainContentComponent> mainContent;
};


#endif  // __PLUGINEDITOR_H_4ACCBAA__
