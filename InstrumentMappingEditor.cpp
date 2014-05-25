/*
  ==============================================================================

    InstrumentMappingEditor.cpp
    Created: 11 May 2014 11:31:53am
    Author:  patrick

  ==============================================================================
*/
#include <stdexcept>
#include "InstrumentMappingEditor.h"

InstrumentMappingEditor::InstrumentMappingEditor(const String& componentName, Component* Parent, AudioDeviceManager* audioManager)
: parent(Parent), audio_manager(audioManager), Viewport(componentName),
  graph(new MappingEditorGraph(1800.0f, 335.0f, 100.0f, 128)){
    setViewedComponent(graph);

    graph->setBounds(0, 0, graph->width(), graph->height() + graph->keyboard_height());
    graph->notes_held().addChangeListener(graph);
    graph->audio_manager(audio_manager);
    graph->audio_manager()->addMidiInputCallback("", graph->midi_callback());
}

InstrumentMappingEditor::~InstrumentMappingEditor(){
    graph = nullptr;
}

InstrumentMappingEditor::MappingEditorGraph::MappingEditorGraph(float w, float h, float kh, int nc)
: Component(), dragged_zone(nullptr), width_(w), height_(h), keyboard_height_(kh), num_columns_(nc){

    lasso = new LassoComponent<Zone*>;
    lasso_source = new MappingLasso<Zone*>;
    lasso_source->parent(this);
    lasso_source->set(new SelectedItemSet<Zone*>);

    keyboard_state = new MidiKeyboardState();
    keyboard_state->addListener(this);
    keyboard = new MidiKeyboardComponent(*keyboard_state, MidiKeyboardComponent::horizontalKeyboard);
    addAndMakeVisible(keyboard);

    midi_callback_ = new MidiDeviceCallback();
    midi_callback_->register_parent(this);
    zone_info_set_ = new SelectedItemSet<Zone*>;
}

void InstrumentMappingEditor::MappingEditorGraph::MidiDeviceCallback::handleIncomingMidiMessage
    (MidiInput* source, const MidiMessage& message) {
    if (message.isNoteOn()) {
        const Graphics g(Graphics(Image()));
        parent->notes_held().addToSelection(message.getNoteNumber());
    }
    if (message.isNoteOff()) {
        if (parent->notes_held().isSelected(message.getNoteNumber())){
            parent->notes_held().deselect(message.getNoteNumber());
        }
    }
}

void InstrumentMappingEditor::MappingEditorGraph::resized(){
    keyboard->setBounds(0, getHeight() - keyboard_height_, getWidth(), keyboard_height_);
    keyboard->setKeyWidth(getWidth() / num_columns_ * 1.7125f);
}

InstrumentMappingEditor::MappingEditorGraph::~MappingEditorGraph(){
    for (auto i : zones){
        delete i;
    }
    delete lasso_source;
    lasso_source = nullptr;
    delete keyboard;
    keyboard = nullptr;
    delete keyboard_state;
    keyboard_state = nullptr;
    delete midi_callback_;
    midi_callback_ = nullptr;
    delete zone_info_set_;
    zone_info_set_ = nullptr;
}

void InstrumentMappingEditor::MappingEditorGraph::handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity){
    notes_held_.addToSelection(midiNoteNumber);
}

void InstrumentMappingEditor::MappingEditorGraph::handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber){
    if (notes_held_.isSelected(midiNoteNumber)){
        notes_held_.deselect(midiNoteNumber);
    }
}

void InstrumentMappingEditor::MappingEditorGraph::paint(Graphics& g){
    g.fillAll(Colours::antiquewhite);
    float grid_outline = 1.0f;
    float grid_width = width_ / num_columns_;
    g.setColour(Colours::black);
    g.setOpacity(0.2f);
    for (auto i : notes_held_) {
        g.fillRect(Rectangle<int>(i*grid_width, 0, grid_width, height_));
    }

    g.setColour(Colours::black);
    g.setOpacity(1.0f);
    Path myPath;
    for (int i=0; i < num_columns_; i++){
        myPath.startNewSubPath (i*grid_width, 0.0f);
        myPath.lineTo (i*grid_width, height_);
    }

    g.strokePath (myPath, PathStrokeType (grid_outline));
}



void InstrumentMappingEditor::MappingEditorGraph::filesDropped(const StringArray& files, int x, int y){
    float grid_outline = 1.0f;
    float grid_width = width_ / num_columns_;
    for (int i=0; i<files.size(); i++){
        Zone* new_zone = new Zone(files[i],audio_manager());
        zones.add(new_zone);
        new_zone->changeState(Zone::Starting);

        new_zone->removeListener(this);
        new_zone->addListener(this);
        new_zone->removeMouseListener(new_zone);
        new_zone->addMouseListener(this, true);

        new_zone->x((x / grid_width) * grid_width + grid_outline + grid_width*i);
        new_zone->y(0);
        new_zone->height(getHeight());
        new_zone->register_parent(this);
        new_zone->setBounds(new_zone->x(), new_zone->y(), grid_width - grid_outline, new_zone->height());
        addAndMakeVisible(new_zone);
        lasso_source->zones().add(new_zone);
    }
}

void InstrumentMappingEditor::MappingEditorGraph::buttonClicked(Button* button){}

void InstrumentMappingEditor::MappingEditorGraph::mouseDown(const MouseEvent& e){
    addAndMakeVisible(lasso);
    lasso->beginLasso(e, lasso_source);
    start_drag_y = getMouseXYRelative().getY();
    dragging = true;
}

void InstrumentMappingEditor::MappingEditorGraph::set_bounds_for_component(Zone* c, MouseCursor cursor, float grid_outline, float grid_width, int grid_x_offset){
    if (cursor == MouseCursor()){
        int grid_x = static_cast<int>((c->x()) / grid_width);
        int new_grid_x = grid_x_offset + grid_x;
        int y = getMouseXYRelative().getY();
        int new_y;
        if (new_grid_x >= 0 && new_grid_x < num_columns_){
            c->x(new_grid_x * grid_width + grid_outline);
        }
        else if (new_grid_x < 0){
            c->x(0);
        }
        else if (new_grid_x > num_columns_){
            c->x(num_columns_ * grid_width + grid_outline);
        }
        if (c->y() + (y - start_drag_y) < 0){
            new_y = 0;
        }
        else if (c->y() + (y - start_drag_y) + c->height() > height_){
            new_y = height_ - c->height();
        }
        else if (c->y() + (y - start_drag_y) >= 0 && c->y() + (y - start_drag_y) + c->height() <= height_){
            new_y = c->y() + (y - start_drag_y);
        }
        c->setBounds(c->x(), new_y, grid_width - grid_outline, c->height());
    }

    if (cursor == MouseCursor(MouseCursor::TopEdgeResizeCursor)){
        int y = getMouseXYRelative().getY();
        int new_y = c->y() + (y - start_drag_y);
        int new_height = c->height() - (y - start_drag_y);

        if (new_y < 0){
            new_y = 0;
            new_height = c->height() + c->y();
        }

        c->setBounds(c->x(), new_y, grid_width - grid_outline, new_height);
    }
    if (cursor == MouseCursor(MouseCursor::BottomEdgeResizeCursor)){
        int y = getMouseXYRelative().getY();
        int new_height = c->height() + (y - start_drag_y);
        if (new_height + c->y() > height_){
            new_height = height_ - (c->y() + c->height()) + c->height();
        }
        c->setBounds(c->x(), c->y(), grid_width - grid_outline, new_height);
    }
}

void InstrumentMappingEditor::MappingEditorGraph::mouseDrag(const MouseEvent& e){
    if (dragged_zone != nullptr){
        lasso_source->dragging(true);
        float grid_outline = 1.0f;
        float grid_width = width_ / num_columns_;

        int grid_x_offset = e.x / grid_width;

        if (lasso_source->set()->getItemArray().contains(dragged_zone)){
            for (auto i : (*lasso_source->set())){
                set_bounds_for_component(i, dragged_zone->getMouseCursor(), grid_outline, grid_width, grid_x_offset);
            }
        }else{
            set_bounds_for_component(dragged_zone, dragged_zone->getMouseCursor(), grid_outline, grid_width, grid_x_offset);
        }
    }else{
        lasso->dragLasso(e);
    }
}

void InstrumentMappingEditor::MappingEditorGraph::mouseUp(const MouseEvent& e){
    if (dragged_zone != nullptr){
        auto cursor = dragged_zone -> getMouseCursor();
        if (cursor == MouseCursor::NormalCursor){
            if (lasso_source->set()->getItemArray().size() > 0){
                for (auto i : (*lasso_source->set())){
                    int new_y = getMouseXYRelative().getY() - start_drag_y;
                    if (i->y() + new_y + i->height() > height()){
                        new_y = height_ - i->height();
                        i->y(new_y);
                    }
                    else if (i->y() + new_y < 0){
                        new_y = 0;
                        i->y(new_y);
                    }
                    else if (i->y() + new_y >= 0 && i->y() + new_y + i->height() <= height()){
                        i->y(new_y+i->y());
                    }
                }
            }else{
                auto Y = getMouseXYRelative().getY();
                auto y = dragged_zone->y();
                int new_y = y + Y - start_drag_y;
                if (new_y + dragged_zone->height() > height()){
                    new_y = height() - dragged_zone->height();
                }
                else if (new_y < 0){
                    new_y = 0;
                }
                dragged_zone->y(new_y);
            }
        }
        else if (cursor == MouseCursor::TopEdgeResizeCursor){
            if (lasso_source->set()->getItemArray().size() > 0){
                for (auto i : *(lasso_source->set())){
                    int new_y = i->y() + (getMouseXYRelative().getY() - start_drag_y);
                    int new_height = i->height() - (getMouseXYRelative().getY() - start_drag_y);
                    if (new_y < 0){
                        new_y = 0;
                        new_height = i->height() + i->y();
                    }
                    i->height(new_height);
                    i->y(new_y);
                }
            } else {
                int new_y = dragged_zone->y() + (getMouseXYRelative().getY() - start_drag_y);
                int new_height = dragged_zone->height() - (getMouseXYRelative().getY() - start_drag_y);

                if (new_y < 0){
                    new_y = 0;
                    new_height = dragged_zone->height() +  dragged_zone->y();
                }
                dragged_zone->height(new_height);
                dragged_zone->y(new_y);
            }
        }
        else if (cursor == MouseCursor::BottomEdgeResizeCursor){
            if (lasso_source->set()->getItemArray().size() > 0){
                for (auto i : *(lasso_source->set())){
                    int new_height = i->height() + (getMouseXYRelative().getY() - start_drag_y);
                    if (new_height + i->y() > height_){
                        new_height = height_ - (i->y() + i->height()) + i->height();
                    }
                    i->height(new_height);
                }
            } else {
                int new_height = dragged_zone->height() + (getMouseXYRelative().getY() - start_drag_y);
                if (new_height + dragged_zone->y() > height_){
                    new_height = height_ - (dragged_zone->y() + dragged_zone->height()) + dragged_zone->height();
                }
                dragged_zone->height(new_height);
            }
        }
        dragged_zone = nullptr;
    }
    lasso->endLasso();
    if (!lasso_source->dragging()){
        for (auto i : (*lasso_source->set())){
            i->setToggleState(false, sendNotification);
            if (i->getMouseCursor() == MouseCursor::NormalCursor){
                i->y(i->y()+getMouseXYRelative().getY() - start_drag_y);
            }
        }
        lasso_source->set()->deselectAll();
    }
    lasso_source->dragging(false);
    dragging = false;
}

class BadFormatException : public std::runtime_error{
public: 
    BadFormatException(String s) : std::runtime_error(s.toStdString()){}
};

typedef InstrumentMappingEditor::MappingEditorGraph::Zone Zone;

Zone::Zone(const String& sample_name,AudioDeviceManager* am) : TextButton(""), name_(sample_name), audio_manager(am){
    setAlpha(0.5f);
    velocity.first = 0;
    velocity.second = 127;
    format_manager.registerBasicFormats();
    std::cout<<format_manager.getWildcardForAllFormats()<<std::endl;
    source_player.setSource(&transport_source);
    audio_manager->addAudioCallback(&source_player);
    audio_manager->initialise(0,2,nullptr,true);
    audio_manager->addChangeListener(this);
    transport_source.addChangeListener(this);
    state = Stopped;
    File f(sample_name);
    auto r = format_manager.createReaderFor(f);
    if( r == nullptr) {
        throw BadFormatException("cannot play "+sample_name);
    }
    reader_source = new AudioFormatReaderSource(r,true);
    transport_source.setSource(reader_source);
}

Zone::~Zone() { 
    reader_source = nullptr;
}   
void Zone::changeListenerCallback (ChangeBroadcaster* src)
{
    if (audio_manager == src) {
        AudioDeviceManager::AudioDeviceSetup setup;
        audio_manager->getAudioDeviceSetup (setup);
        if (setup.outputChannels.isZero()) {
            source_player.setSource (nullptr);
        } else {
            source_player.setSource (&transport_source);
        }
    } else if (&transport_source == src) {
        if (transport_source.isPlaying()) {
            changeState (Playing);
        } else {
            if ((Stopping == state) || (Playing == state)) {
                changeState (Stopped);
            }
            else if (Pausing == state) {
                changeState (Paused);
            }
        }
    }
}

void Zone::changeState (TransportState newState)
{
    if (state != newState) {
        state = newState;
        switch (state) {
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
void Zone::register_parent(InstrumentMappingEditor::MappingEditorGraph* c){parent=c;}

void Zone::mouseDown(const MouseEvent& e){
    parent->dragged_zone = this;
    parent->zone_info_set()->selectOnly(this);
}

void Zone::mouseMove(const MouseEvent& e){
    if (!this->parent->dragging){
        if (e.y < 5){
            setMouseCursor(MouseCursor::TopEdgeResizeCursor);
            return;
        }
        if (height_ - e.y < 5){
            setMouseCursor(MouseCursor::BottomEdgeResizeCursor);
            return;
        }
        setMouseCursor(MouseCursor());
    }
}

void Zone::mouseDoubleClick(const MouseEvent& e){
    if(state == Stopped) {
        changeState(Starting);
    }
    else {
        changeState(Stopping);
    }
}
