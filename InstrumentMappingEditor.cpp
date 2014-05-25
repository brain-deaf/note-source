/*
  ==============================================================================

    InstrumentMappingEditor.cpp
    Created: 11 May 2014 11:31:53am
    Author:  patrick

  ==============================================================================
*/

#include "InstrumentMappingEditor.h"

InstrumentMappingEditor::InstrumentMappingEditor(const String& componentName, Component* Parent, AudioDeviceManager* audioManager)
: parent(Parent), audio_manager(audioManager), Viewport(componentName){
    setViewedComponent(graph = new MappingEditorGraph());

    graph->width(1800.0f);
    graph->height(335.0f);
    graph->keyboard_height(100.0f);

    graph->num_columns(128);
    graph->setBounds(0, 0, graph->width(), graph->height() + graph->keyboard_height());

    graph->notes_held().addChangeListener(graph);

    graph->audio_manager(audio_manager);
    graph->audio_manager()->addMidiInputCallback("", graph->midi_callback());
}

InstrumentMappingEditor::~InstrumentMappingEditor(){
    graph = nullptr;
}

InstrumentMappingEditor::MappingEditorGraph::MappingEditorGraph()
: Component(), dragged_zone(nullptr){

    lasso = new LassoComponent<Zone*>;
    lasso_source = new MappingLasso<Zone*>;
    lasso_source->parent(this);
    lasso_source->set(new SelectedItemSet<Zone*>);

    keyboard_state = new MidiKeyboardState();
    keyboard_state->addListener(this);
    keyboard = new MidiKeyboardComponent(*keyboard_state, MidiKeyboardComponent::horizontalKeyboard);

    midi_callback_ = new MidiDeviceCallback();
    midi_callback_->register_parent(this);

    zone_info_set_ = new SelectedItemSet<Zone*>;

    addAndMakeVisible(keyboard);
}

void InstrumentMappingEditor::MappingEditorGraph::MidiDeviceCallback::handleIncomingMidiMessage
    (MidiInput* source, const MidiMessage& message){
    if (message.isNoteOn()){
        const Graphics g(Graphics(Image()));
        parent->notes_held().addToSelection(message.getNoteNumber());
    }
    if (message.isNoteOff()){
        if (parent->notes_held().isSelected(message.getNoteNumber())){
            parent->notes_held().deselect(message.getNoteNumber());
        }
    }

}

void InstrumentMappingEditor::MappingEditorGraph::resized(){
    keyboard->setBounds(0, height_, width_, keyboard_height_);
    keyboard->setKeyWidth(width_ / num_columns_ * 1.7125f);
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
    std::cout<<"note on"<<std::endl;
    //keyboard_state->noteOn(0, midiNoteNumber, 1);
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
        Zone* new_zone;
        zones.add(new_zone = new Zone(files[i]));
        new_zone->removeListener(this);
        new_zone->addListener(this);
        new_zone->removeMouseListener(new_zone);
        new_zone->addMouseListener(this, true);

        new_zone->x(static_cast<int>(x / grid_width) * grid_width + grid_outline + grid_width*i);
        new_zone->y(0);
        new_zone->height(height_);
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
        if (new_grid_x < 0){
            c->x(0);
        }
        if (new_grid_x > num_columns_){
            c->x(num_columns_ * grid_width + grid_outline);
        }
        if (c->y() + (y - start_drag_y) < 0){
            new_y = 0;
        }
        if (c->y() + (y - start_drag_y) + c->height() > height_){
            new_y = height_ - c->height();
        }
        if (c->y() + (y - start_drag_y) >= 0 && c->y() + (y - start_drag_y) + c->height() <= height_){
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

        int grid_x_offset = static_cast<int>((e.x / grid_width));

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
        if (dragged_zone->getMouseCursor() == MouseCursor()){
            if (lasso_source->set()->getItemArray().size() > 0){
                for (auto i : (*lasso_source->set())){
                    int new_y = getMouseXYRelative().getY() - start_drag_y;
                    if (i->y() + new_y + i->height() > height_){
                        new_y = height_ - i->height();
                        i->y(new_y);
                    }
                    if (i->y() + new_y < 0){
                        new_y = 0;
                        i->y(new_y);
                    }
                    if (i->y() + new_y >= 0 && i->y() + new_y + i->height() <= height_){
                        i->y(new_y+i->y());
                    }
                }
            }else{
                int new_y = dragged_zone->y() + (getMouseXYRelative().getY() - start_drag_y);
                if (dragged_zone->y() + (getMouseXYRelative().getY() - start_drag_y) + dragged_zone->height() > height()){
                    new_y = height_ - dragged_zone->height();
                }
                if (dragged_zone->y() + (getMouseXYRelative().getY() - start_drag_y) < 0){
                    new_y = 0;
                }
                dragged_zone->y(new_y);
            }
        }
        if (dragged_zone->getMouseCursor() == MouseCursor(MouseCursor::TopEdgeResizeCursor)){
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
        if (dragged_zone->getMouseCursor() == MouseCursor(MouseCursor::BottomEdgeResizeCursor)){
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
            if (i->getMouseCursor() == MouseCursor()){
                i->y(i->y()+getMouseXYRelative().getY() - start_drag_y);
            }
        }
        lasso_source->set()->deselectAll();
    }
    lasso_source->dragging(false);
    dragging = false;
}

InstrumentMappingEditor::MappingEditorGraph::Zone::Zone(const String& sample_name) : TextButton(""), name_(sample_name){
    setAlpha(0.5f);
    velocity.first = 0;
    velocity.second = 127;
}

void InstrumentMappingEditor::MappingEditorGraph::Zone::register_parent(InstrumentMappingEditor::MappingEditorGraph* c){parent=c;}

void InstrumentMappingEditor::MappingEditorGraph::Zone::mouseDown(const MouseEvent& e){
    parent->dragged_zone = this;
    parent->zone_info_set()->selectOnly(this);
    this->clicked();
}

void InstrumentMappingEditor::MappingEditorGraph::Zone::mouseMove(const MouseEvent& e){
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

