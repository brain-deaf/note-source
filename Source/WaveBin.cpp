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
    delete group_view;
    group_view = nullptr;
    delete Vport;
    Vport = nullptr;
    delete waveform;
    waveform = nullptr;
    delete sample_start;
    sample_start = nullptr;
    delete WaveVport;
    WaveVport = nullptr;
    delete vScaling;
    vScaling = nullptr;
    delete hScaling;
    hScaling = nullptr;
    delete playButton;
    playButton = nullptr;
}

void WaveBin::updateZone(Zone* _zone){
    z = _zone;
    if (z != nullptr){
        sample_start->setValue(z->getPlaySettings().getSampleStart());
        if (filePlayer != nullptr){
            delete filePlayer;
            filePlayer = nullptr;
        }
        filePlayer = new FilePlayer(z->getName());
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
        waveform->setSampleStart(sample_start->getValue());
        z->getPlaySettings().setSampleStart(sample_start->getValue());
        
        InstrumentMappingEditor::MappingEditorGraph* m = mapping_editor->getMappingEditor()->graph;
        int zone_index = m->getZones().indexOf(z);
        static_cast<SampleSound*>(m->getSampler().getSynth()->getSound(zone_index))->setSampleStart(sample_start->getValue());
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
        if (fabs(e.getPosition().getX() - vport_width) <= drag_threshold){
            setMouseCursor(MouseCursor::LeftRightResizeCursor);
        }else{
            if (getMouseCursor() != MouseCursor()){
                setMouseCursor(MouseCursor());
            }
        }
    }
}

void WaveBin::mouseDown(const MouseEvent& e){
    if (getMouseCursor() == MouseCursor::LeftRightResizeCursor){
        dragging = true;
    }
}

void WaveBin::mouseUp(const MouseEvent& e){
    dragging = false;
}

void WaveBin::mouseDrag(const MouseEvent& e){
    if (dragging){
        vport_width = e.getPosition().getX() <= group_view_width ? e.getPosition().getX() : group_view_width;
        vport_width = e.getPosition().getX() >= lower_limit ? vport_width : lower_limit;
        resized();
    }
}