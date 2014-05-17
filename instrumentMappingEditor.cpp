/*
  ==============================================================================

    instrumentMappingEditor.cpp
    Created: 11 May 2014 11:31:53am
    Author:  patrick

  ==============================================================================
*/

#include "instrumentMappingEditor.h"

instrumentMappingEditor::instrumentMappingEditor(const String& componentName, Component* Parent)
: parent(Parent), Viewport(componentName){
    setViewedComponent(graph = new mappingEditorGraph());
    
    graph->width = 1800.0f;
    graph->height = 335.0f;
    graph->keyboard_height = 100.0f;
    
    graph->num_columns = 128;
    graph->setBounds(0, 0, graph->width, graph->height + graph->keyboard_height);
}

instrumentMappingEditor::~instrumentMappingEditor(){
    graph = nullptr;
}



instrumentMappingEditor::mappingEditorGraph::mappingEditorGraph()
: Component(), dragged_zone(nullptr){
    
    //lasso_source.set->addChangeListener(lasso_source);
    lasso = new LassoComponent<Zone*>;
    lasso_source = new Lasso_Source<Zone*>;
    lasso_source->parent = this;
    lasso_source->set = new SelectedItemSet<Zone*>;
    
    keyboard_state = new MidiKeyboardState();
    keyboard_state->addListener(this);
    keyboard = new MidiKeyboardComponent(*keyboard_state, MidiKeyboardComponent::horizontalKeyboard);
    
    addAndMakeVisible(keyboard);
}

void instrumentMappingEditor::mappingEditorGraph::resized(){
    keyboard->setBounds(0, height, width, keyboard_height);
    keyboard->setKeyWidth(width / num_columns * 1.7125f);
}

instrumentMappingEditor::mappingEditorGraph::~mappingEditorGraph(){
    for (int i=0; i<zones.size(); i++){
        delete zones[i];
    }
    lasso_source = nullptr;
    keyboard = nullptr;
}

void instrumentMappingEditor::mappingEditorGraph::handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity){
    std::cout<<"note on"<<std::endl;
    notes_held.addToSelection(midiNoteNumber);
    repaint();
}

void instrumentMappingEditor::mappingEditorGraph::handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber){
    std::cout<<"note off"<<std::endl;
    if (notes_held.isSelected(midiNoteNumber)){
        notes_held.deselect(midiNoteNumber);
        repaint();
    }
}

void instrumentMappingEditor::mappingEditorGraph::paint(Graphics& g){
    //g.setImageResamplingQuality(Graphics::highResamplingQuality);
    
    g.fillAll(Colours::antiquewhite);
    std::cout<<"paint called"<<std::endl;
    
    float grid_outline = 1.0f;
    float grid_width = width / num_columns;
    
    g.setColour(Colours::black);
    g.setOpacity(0.2f);
    for (int* i=notes_held.begin(); i<notes_held.end(); i++){
        g.fillRect(Rectangle<int>(*i*grid_width, 0, grid_width, height));
    }
    
    g.setColour(Colours::black);
    g.setOpacity(1.0f);
    Path myPath;
    for (int i=0; i<this->num_columns; i++){
        myPath.startNewSubPath (i*grid_width, 0.0f);       
        myPath.lineTo (i*grid_width, height);                
    }


    g.strokePath (myPath, PathStrokeType (grid_outline));
}



void instrumentMappingEditor::mappingEditorGraph::filesDropped(const StringArray& files, int x, int y){
    float grid_outline = 1.0f;
    float grid_width = width / num_columns;
    
    for (int i=0; i<files.size(); i++){
        Zone* new_zone;
        zones.add(new_zone = new Zone(files[i]));
        //std::cout<<new_zone->velocity.second<<std::endl;
        
        new_zone->removeListener(this);
        new_zone->addListener(this);
        new_zone->removeMouseListener(new_zone);
        new_zone->addMouseListener(this, true);

        new_zone->x = ((int)(x / grid_width)) * grid_width + grid_outline + grid_width*i;
        new_zone->y = 0;
        new_zone->height = height;
        new_zone->register_parent(this);
        
        new_zone->setBounds(new_zone->x, new_zone->y, grid_width - grid_outline, new_zone->height);
        addAndMakeVisible(new_zone);
        
        lasso_source->zones_.add(new_zone);
    }
}

void instrumentMappingEditor::mappingEditorGraph::buttonClicked(Button* button){}

void instrumentMappingEditor::mappingEditorGraph::mouseDown(const MouseEvent& e){
    addAndMakeVisible(lasso);
    lasso->beginLasso(e, lasso_source);
    start_drag_y = getMouseXYRelative().getY();
    dragging = true;
}

void instrumentMappingEditor::mappingEditorGraph::set_bounds_for_component(Zone* c, MouseCursor cursor, float grid_outline, float grid_width, int grid_x_offset){
    if (cursor == MouseCursor()){
        int grid_x = (int)(c->x / grid_width);
        int new_grid_x = grid_x_offset + grid_x;
        int y = getMouseXYRelative().getY();
    
        if (new_grid_x >= 0 and new_grid_x < num_columns){
            c->x = new_grid_x * grid_width + grid_outline;
            c->setBounds(c->x, c->y + (y - start_drag_y), grid_width - grid_outline, c->height);           
        }
    }
            
    if (cursor == MouseCursor(MouseCursor::TopEdgeResizeCursor)){
        int y = getMouseXYRelative().getY();
        c->setBounds(c->x, c->y + (y - start_drag_y), grid_width - grid_outline, c->height - (y - start_drag_y));
    }
    if (cursor == MouseCursor(MouseCursor::BottomEdgeResizeCursor)){
        int y = getMouseXYRelative().getY();
        c->setBounds(c->x, c->y, grid_width - grid_outline, c->height + (y - start_drag_y));
    }
}

void instrumentMappingEditor::mappingEditorGraph::mouseDrag(const MouseEvent& e){
    if (dragged_zone != nullptr){
        lasso_source->dragging = true;
        float grid_outline = 1.0f;
        float grid_width = width / num_columns;
        
        int grid_x_offset = (int)(e.x / grid_width);
        
        if (lasso_source->set->getItemArray().contains(dragged_zone)){
            for (Zone** i = lasso_source->set->begin(); i<lasso_source->set->end(); i++){
                set_bounds_for_component((*i), dragged_zone->getMouseCursor(), grid_outline, grid_width, grid_x_offset);
            }
        }else{
            set_bounds_for_component(dragged_zone, dragged_zone->getMouseCursor(), grid_outline, grid_width, grid_x_offset);
        }
    }else{
        lasso->dragLasso(e);
    }
}

void instrumentMappingEditor::mappingEditorGraph::mouseUp(const MouseEvent& e){
    if (dragged_zone != nullptr){
        if (dragged_zone->getMouseCursor() == MouseCursor()){
            if (lasso_source->set->getItemArray().size() > 0){
                for (Zone** i = lasso_source->set->begin(); i<lasso_source->set->end(); i++){
                    (*i)->y += getMouseXYRelative().getY() - start_drag_y;
                }
            }else{
                dragged_zone->y += getMouseXYRelative().getY() - start_drag_y;
            }
        }
        if (dragged_zone->getMouseCursor() == MouseCursor(MouseCursor::TopEdgeResizeCursor)){
            if (lasso_source->set->getItemArray().size() > 0){
                for (Zone** i = lasso_source->set->begin(); i<lasso_source->set->end(); i++){
                    (*i)->height -= getMouseXYRelative().getY() - start_drag_y;
                    (*i)->y += getMouseXYRelative().getY() - start_drag_y;
                }
            }else{
                dragged_zone->height -= getMouseXYRelative().getY() - dragged_zone->y;
                dragged_zone->y += getMouseXYRelative().getY() - start_drag_y;
            }
        }
        if (dragged_zone->getMouseCursor() == MouseCursor(MouseCursor::BottomEdgeResizeCursor)){
            if (lasso_source->set->getItemArray().size() > 0){
                for (Zone** i = lasso_source->set->begin(); i<lasso_source->set->end(); i++){
                    (*i)->height += getMouseXYRelative().getY() - start_drag_y;
                }
            }else{
                dragged_zone->height -= getMouseXYRelative().getY() - dragged_zone->y;
                dragged_zone->y += getMouseXYRelative().getY() - start_drag_y;
            }
        }
        dragged_zone = nullptr;
    }
    lasso->endLasso();
    
    if (!lasso_source->dragging){
        for (Zone** i = lasso_source->set->begin(); i<lasso_source->set->end(); i++){
            (*i)->setToggleState(false, 1);
            if ((*i)->getMouseCursor() == MouseCursor()){
                (*i)->y += getMouseXYRelative().getY() - start_drag_y;
            }
        }
        lasso_source->set->deselectAll();
    }
    lasso_source->dragging = false;
    dragging = false;
}

void instrumentMappingEditor::mappingEditorGraph::Zone::register_parent(instrumentMappingEditor::mappingEditorGraph* c){parent=c;}

void instrumentMappingEditor::mappingEditorGraph::Zone::mouseDown(const MouseEvent& e){
    parent->dragged_zone = this;
    this->clicked();
}

void instrumentMappingEditor::mappingEditorGraph::Zone::mouseMove(const MouseEvent& e){
    if (!this->parent->dragging){
        if (e.y < 5){
            setMouseCursor(MouseCursor::TopEdgeResizeCursor);
            return;
        }
        if (height - e.y < 5){
            setMouseCursor(MouseCursor::BottomEdgeResizeCursor);
            return;
        }
        setMouseCursor(MouseCursor());
    }
}

