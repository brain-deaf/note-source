/*
  ==============================================================================

    WaveBin.cpp
    Created: 26 Jul 2014 2:08:53pm
    Author:  patrick

  ==============================================================================
*/

#include "WaveBin.h"
#include "InstrumentMappingEditor.h"
#include "Sampler.h"
#include "MainComponent.h"

WaveBin::WaveBin(MappingEditorBin* m): mapping_editor(m), 
                 waveform(new WaveformView(this)),
                 dragging(false),
                 z(nullptr),
                 filePlayer(nullptr),
                 playButton(new TextButton("Play"))
{
    mapping_editor->getMappingEditor()->group_editor;
    group_view = new GroupView(mapping_editor->getMappingEditor()->group_editor, this);
    
    Vport = new Viewport();
    Vport->setViewedComponent(group_view);
    Vport->removeMouseListener(this);
    addAndMakeVisible(Vport);
    
    playButton->addListener(this);
    playButton->setClickingTogglesState(true);
    addAndMakeVisible(playButton);
    
    WaveVport = new Viewport();
    WaveVport->setViewedComponent(waveform);
    WaveVport->removeMouseListener(this);
    addAndMakeVisible(WaveVport);
    
    sample_start = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
    sample_start->setRange(0, 96000, 1);
    sample_start->setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::green);
    sample_start->addListener(this);
    addAndMakeVisible(sample_start);

	releaseStart = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
	releaseStart->setRange(0, 96000, 1);
	releaseStart->setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::red);
	releaseStart->addListener(this);
	addAndMakeVisible(releaseStart);

	releaseTime = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
	releaseTime->setRange(0, 48000, 1);
	releaseTime->setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::red);
	releaseTime->addListener(this);
	addAndMakeVisible(releaseTime);

	releaseCurve = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
	releaseCurve->setRange(-0.5, 0.5);
	releaseCurve->setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::red);
	releaseCurve->addListener(this);
	addAndMakeVisible(releaseCurve);
    
    loopStart = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
    loopStart->setRange(0, 96000, 1);
    loopStart->setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::yellow);
    loopStart->addListener(this);
    addAndMakeVisible(loopStart);
    
    loopEnd= new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
    loopEnd->setRange(0, 96000, 1);
    loopEnd->setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::yellow);
    loopEnd->addListener(this);
    addAndMakeVisible(loopEnd);
    
    xfadeLength = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
    xfadeLength->setRange(0, 96000/2, 1);
    xfadeLength->setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::yellow);
    xfadeLength->addListener(this);
	xfadeLength->setValue(150.0);
    addAndMakeVisible(xfadeLength);

	xfadeCurve = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
	xfadeCurve->setRange(-0.5, 0.5);
	xfadeCurve->setValue(0.1);
	xfadeCurve->setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::orange);
	xfadeCurve->addListener(this);
	addAndMakeVisible(xfadeCurve);
    
    toggleLoop = new TextButton("Loop Mode");
    toggleLoop->setClickingTogglesState(true);
    toggleLoop->addListener(this);
    addAndMakeVisible(toggleLoop);

	toggleRelease = new TextButton("Release Mode");
	toggleRelease->setClickingTogglesState(true);
	toggleRelease->addListener(this);
	addAndMakeVisible(toggleRelease);
    
    tuneSlider= new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
    tuneSlider->setRange(-3, 3);
    tuneSlider->setValue(0.0);
    tuneSlider->setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::yellow);
    tuneSlider->addListener(this);
    addAndMakeVisible(tuneSlider);
    
    group_view_width = 800;
    vport_width = 300;
    lower_limit = 100;
    drag_threshold = 5;
    waveform_height = 280;
    waveform_width = 600;
    waveform_padding = 30;
    scaling_slider_height = 10;
    top_padding = 30;
    
    vScaling = new Slider(Slider::SliderStyle::LinearVertical, Slider::TextEntryBoxPosition::NoTextBox);
    hScaling = new Slider(Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::NoTextBox);
    vScaling->setRange(0.1, 2.0);
    vScaling->setValue(0.7);
    hScaling->setRange(100.0, 40000.0);
    vScaling->addListener(this);
    hScaling->addListener(this);
    addAndMakeVisible(vScaling);
    addAndMakeVisible(hScaling);
    
    waveform->setSize(waveform_width, waveform_height);
    waveform->setPlaying(false);
}

WaveBin::~WaveBin(){
    group_view = nullptr;
    Vport = nullptr;
    waveform = nullptr;
    sample_start = nullptr;
    WaveVport = nullptr;
    vScaling = nullptr;
    hScaling = nullptr;
    playButton = nullptr;
	filePlayer = nullptr;
	loopStart = nullptr;
	loopEnd = nullptr;
	xfadeLength = nullptr;
	tuneSlider = nullptr;
	toggleLoop = nullptr;
	releaseStart = nullptr;
	releaseTime = nullptr;
	releaseCurve = nullptr;
	toggleRelease = nullptr;
}

void WaveBin::updateZone(Zone* _zone){
    z = _zone;
    if (z != nullptr){
        sample_start->setValue(z->getPlaySettings()->getSampleStart(), sendNotification);
		releaseStart->setValue(z->getPlaySettings()->getReleaseStart(), sendNotification);
		releaseTime->setValue(z->getPlaySettings()->getReleaseTime(), sendNotification);
		releaseCurve->setValue(z->getPlaySettings()->getReleaseCurve(), sendNotification);
        toggleLoop->setToggleState(z->getPlaySettings()->getLoopMode(), dontSendNotification);
		toggleRelease->setToggleState(z->getPlaySettings()->getReleaseMode(), dontSendNotification);
        loopStart->setValue(z->getPlaySettings()->getLoopStart(), sendNotification);
        loopEnd->setValue(z->getPlaySettings()->getLoopEnd(), sendNotification);
        xfadeLength->setValue(z->getPlaySettings()->getXfadeLength(), sendNotification);
		xfadeCurve->setValue(z->getPlaySettings()->getXfadeCurve(), sendNotification);
        tuneSlider->setValue(z->getPlaySettings()->getTuning(), sendNotification);
        if (filePlayer != nullptr){
            filePlayer = nullptr;
        }
		try{
			filePlayer = new FilePlayer(z->getName());
		}
		catch (BadFormatException& ){
			filePlayer = nullptr;
			return;
		}
        filePlayer->getTransportSource().addChangeListener(this);
    }
}

void WaveBin::resized(){
    group_view->setBounds(0, 0, group_view_width, getHeight() - 20);
    Vport->setBounds(0, 0, vport_width, getHeight());
    WaveVport->setBounds(vport_width+waveform_padding, top_padding, getWidth()-vport_width-waveform_padding*2, waveform_height);
    hScaling->setBounds(vport_width + waveform_padding + WaveVport->getWidth()/2, 
                        top_padding+waveform_height,
                        WaveVport->getWidth()/2,
                        scaling_slider_height);
    vScaling->setBounds(vport_width + waveform_padding + WaveVport->getWidth(), 
                        top_padding,
                        scaling_slider_height,
                        WaveVport->getHeight()/2);
                        
    playButton->setBounds(getWidth() - waveform_padding*3, top_padding/5, waveform_padding*2, top_padding/3*2);
                        
    if (WaveVport->getWidth() > waveform->getWidth()){
        waveform->setSize(WaveVport->getWidth(), waveform_height);
    }
    
    sample_start->setBounds(vport_width + waveform_padding, waveform_height + 50, 50, 50);
    loopStart->setBounds(vport_width + waveform_padding+50, waveform_height + 50, 50, 50);
    loopEnd->setBounds(vport_width + waveform_padding+100, waveform_height + 50, 50, 50);
    xfadeLength->setBounds(vport_width + waveform_padding+150, waveform_height + 50, 50, 50);
	xfadeCurve->setBounds(vport_width + waveform_padding + 150, waveform_height + 100, 50, 50);
    toggleLoop->setBounds(vport_width + waveform_padding+200, waveform_height + 50, 80, 40);
    tuneSlider->setBounds(vport_width + waveform_padding+290, waveform_height + 50, 50, 50);
	releaseStart->setBounds(vport_width + waveform_padding, waveform_height + 150, 50, 50);
	releaseTime->setBounds(vport_width + waveform_padding+50, waveform_height + 150, 50, 50);
	releaseCurve->setBounds(vport_width + waveform_padding + 100, waveform_height + 150, 50, 50);
	toggleRelease->setBounds(vport_width + waveform_padding + 200, waveform_height + 150, 80, 40);
}

void WaveBin::paint(Graphics& g){
     g.fillAll(Colours::grey); 
}

void WaveBin::sliderValueChanged(Slider* s){
    if (s == hScaling){
        if (s->getValue() >= WaveVport->getWidth())
            waveform->setSize(s->getValue(), waveform_height);
    }
    if (s == vScaling){
        waveform->setVScale(s->getValue());
        waveform->repaint();
    }
    if (s == sample_start){
        if (z != nullptr){
            waveform->setSampleStart(sample_start->getValue());
            z->getPlaySettings()->setSampleStart(sample_start->getValue());
        
            MappingEditorGraph* m = mapping_editor->getMappingEditor()->graph;
            int zone_index = m->getZones().indexOf(z);
            static_cast<SampleSound*>(MainContentComponent::_static_sampler->getSynth()->getSound(zone_index))->setSampleStart(sample_start->getValue());
        }
    }
	if (s == releaseStart){
		if (z != nullptr){
			waveform->setReleaseStart(releaseStart->getValue());
			z->getPlaySettings()->setReleaseStart(releaseStart->getValue());
			waveform->repaint();
			MappingEditorGraph* m = mapping_editor->getMappingEditor()->graph;
			int zone_index = m->getZones().indexOf(z);
			static_cast<SampleSound*>(MainContentComponent::_static_sampler->getSynth()->getSound(zone_index))->setReleaseStart(releaseStart->getValue());
		}
	}
	if (s == releaseTime){
		if (z != nullptr){
			waveform->setReleaseTime(releaseTime->getValue());
			waveform->repaint();
			z->getPlaySettings()->setReleaseTime(releaseTime->getValue());
			MappingEditorGraph* m = mapping_editor->getMappingEditor()->graph;
			int zone_index = m->getZones().indexOf(z);
			static_cast<SampleSound*>(MainContentComponent::_static_sampler->getSynth()->getSound(zone_index))->setReleaseTime(releaseTime->getValue());
		}
	}
	if (s == releaseCurve){
		if (z != nullptr){
			waveform->setReleaseCurve(releaseCurve->getValue());
			waveform->repaint();
			z->getPlaySettings()->setReleaseCurve(releaseCurve->getValue());
			MappingEditorGraph* m = mapping_editor->getMappingEditor()->graph;
			int zone_index = m->getZones().indexOf(z);
			static_cast<SampleSound*>(MainContentComponent::_static_sampler->getSynth()->getSound(zone_index))->setReleaseCurve(releaseCurve->getValue());
		}
	}
    if (s == loopStart){
        if (z != nullptr){
            waveform->setLoopStart(loopStart->getValue());
            waveform->repaint();
            z->getPlaySettings()->setLoopStart(loopStart->getValue());
            MappingEditorGraph* m = mapping_editor->getMappingEditor()->graph;
            int zone_index = m->getZones().indexOf(z);
			static_cast<SampleSound*>(MainContentComponent::_static_sampler->getSynth()->getSound(zone_index))->setLoopStart(loopStart->getValue());
            xfadeLength->setRange(1, (loopEnd->getValue() - loopStart->getValue()) / 2 - 1, 1);
            sliderValueChanged(xfadeLength);
        }
    }
    if (s == loopEnd){
        if (z != nullptr){
            waveform->setLoopEnd(loopEnd->getValue());
            waveform->repaint();
            z->getPlaySettings()->setLoopEnd(loopEnd->getValue());
            MappingEditorGraph* m = mapping_editor->getMappingEditor()->graph;
            int zone_index = m->getZones().indexOf(z);
			static_cast<SampleSound*>(MainContentComponent::_static_sampler->getSynth()->getSound(zone_index))->setLoopEnd(loopEnd->getValue());
            xfadeLength->setRange(1, (loopEnd->getValue() - loopStart->getValue()) / 2 - 1, 1);
            sliderValueChanged(xfadeLength);
        }
    }
    if (s == xfadeLength){
        if (z != nullptr){
            waveform->setXfadeLength(xfadeLength->getValue());
            waveform->repaint();
            z->getPlaySettings()->setXfadeLength(xfadeLength->getValue());
            MappingEditorGraph* m = mapping_editor->getMappingEditor()->graph;
            int zone_index = m->getZones().indexOf(z);
			static_cast<SampleSound*>(MainContentComponent::_static_sampler->getSynth()->getSound(zone_index))->setXfadeLength(xfadeLength->getValue());
        }
    }
	if (s == xfadeCurve){
		if (z != nullptr){
			waveform->setXfadeCurve(xfadeCurve->getValue());
			waveform->repaint();
			z->getPlaySettings()->setXfadeCurve(xfadeCurve->getValue());
			MappingEditorGraph* m = mapping_editor->getMappingEditor()->graph;
			int zone_index = m->getZones().indexOf(z);
			static_cast<SampleSound*>(MainContentComponent::_static_sampler->getSynth()->getSound(zone_index))->setXfadeCurve(xfadeCurve->getValue());
		}
	}
    if (s == tuneSlider){
        if (z != nullptr){
            z->getPlaySettings()->setTuning(tuneSlider->getValue());
            MappingEditorGraph* m = mapping_editor->getMappingEditor()->graph;
            int zone_index = m->getZones().indexOf(z);
			static_cast<SampleSound*>(MainContentComponent::_static_sampler->getSynth()->getSound(zone_index))->setTuning(tuneSlider->getValue());
        }
    }
}

void WaveBin::buttonClicked(Button* b){
    if (b == playButton){
        if (filePlayer != nullptr){
            filePlayer->toggleState();
            if (b->getToggleState()){
                waveform->setLength(filePlayer->getTransportSource().getLengthInSeconds());
                waveform->setPlaying(true);
                startTimer(2);
            }else{
                waveform->setPlaying(false);
                stopTimer();
                waveform->repaint();
            }
        }
    }
    if (b == toggleLoop){
        if (z != nullptr){
            z->getPlaySettings()->setLoopMode(toggleLoop->getToggleState());
            MappingEditorGraph* m = mapping_editor->getMappingEditor()->graph;
            int zone_index = m->getZones().indexOf(z);
			static_cast<SampleSound*>(MainContentComponent::_static_sampler->getSynth()->getSound(zone_index))->setLoopMode(toggleLoop->getToggleState());
        }
    }
	if (b == toggleRelease){
		if (z != nullptr){
			z->getPlaySettings()->setReleaseMode(toggleRelease->getToggleState());
			MappingEditorGraph* m = mapping_editor->getMappingEditor()->graph;
			int zone_index = m->getZones().indexOf(z);
			static_cast<SampleSound*>(MainContentComponent::_static_sampler->getSynth()->getSound(zone_index))->setReleaseMode(toggleRelease->getToggleState());
		}
	}
}

void WaveBin::timerCallback(){
    waveform->setPlayPosition(filePlayer->getTransportSource().getCurrentPosition());
    waveform->repaint();
}
    

void WaveBin::changeListenerCallback(ChangeBroadcaster* source){
    if (source == &(filePlayer->getTransportSource())){
        if(filePlayer->getTransportSource().hasStreamFinished()){ 
            playButton->setToggleState(false, dontSendNotification);
            waveform->setPlaying(false);
            stopTimer();
            waveform->repaint();
        }
    }
}

void WaveBin::mouseMove(const MouseEvent& e){
    if (!dragging){
        if (std::abs(e.getPosition().getX() - vport_width) <= drag_threshold){
            setMouseCursor(MouseCursor::LeftRightResizeCursor);
        }else{
            if (getMouseCursor() != MouseCursor()){
                setMouseCursor(MouseCursor());
            }
        }
    }
}

void WaveBin::mouseDown(const MouseEvent& ){
    if (getMouseCursor() == MouseCursor::LeftRightResizeCursor){
        dragging = true;
    }
}

void WaveBin::mouseUp(const MouseEvent& ){
    dragging = false;
}

void WaveBin::mouseDrag(const MouseEvent& e){
    if (dragging){
        vport_width = e.getPosition().getX() <= group_view_width ? e.getPosition().getX() : group_view_width;
        vport_width = e.getPosition().getX() >= lower_limit ? vport_width : lower_limit;
        resized();
    }
}