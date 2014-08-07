/*
  ==============================================================================

    InstrumentMappingEditor.cpp
    Created: 11 May 2014 11:31:53am
    Author:  patrick

  ==============================================================================
*/
#include <memory>
#include "InstrumentComponent.h"
#include "InstrumentMappingEditor.h"
#include "InstrumentBin.h"
#include "MainComponent.h"


class BadFormatException : public std::runtime_error{
public:
    BadFormatException(String s) : std::runtime_error(s.toStdString()){}
};

InstrumentMappingEditor::InstrumentMappingEditor(const String& componentName, InstrumentComponent& i)
:   Component(), instrument(i), view_port(new Viewport(componentName)),
    group_editor(new GroupEditor(200, 333 + 100, this)),
    graph(new MappingEditorGraph(1800.0f, 315.0f, 100.0f, 128, i, group_editor))
{
    
    graph->getGroupEditor() = group_editor;
    view_port->setViewedComponent(graph);
    int group_editor_width = 200;
    view_port->setBounds(group_editor_width, 0, 1000 - group_editor_width, 333 + 100);
    
    group_editor->getCreateGroupButton()->addListener(graph);
    group_editor->getDeleteGroupButton()->addListener(graph);
    group_editor->setBounds(0, 0, group_editor_width, 333 + 100);
    group_editor->getModel();
    addAndMakeVisible(view_port);
    addAndMakeVisible(group_editor);
}

/*void InstrumentMappingEditor::MappingEditorGraph::broughtToFront(){
    int group_editor_width = 200;
    addAndMakeVisible(group_editor);
    group_editor->setBounds(0, 0, group_editor_width, 333 + 100);
    std::cout<<"group editor resized mapping page"<<std::endl;
}*/

void InstrumentMappingEditor::resized() 
{
    auto group_editor_width = group_editor->getWidth();
    view_port->setBounds(group_editor_width,0,getWidth()-group_editor_width, 333+100);
    group_editor->setBounds(0, 0, group_editor_width, 333 + 100);
}

InstrumentMappingEditor::~InstrumentMappingEditor(){
    delete view_port;
    view_port = nullptr;
    delete group_editor;
    group_editor = nullptr;
}


typedef InstrumentMappingEditor::MappingEditorGraph MappingEditorGraph;

MappingEditorGraph::MappingEditorGraph(float w, float h,
    float kh, int nc, InstrumentComponent& i, GroupEditor* g)
: Component(), width(w), height(h), keyboardHeight(kh), group_editor(g),
    numColumns(nc), draggedZone(nullptr), dragging(false), groupEditorY(0),
    lasso(), lassoSource(this), instrument(i), midiCallback(this), 
    keyboardState(), zones(), sampler(&(getNotesHeld())), metronome(), metronome_player(),
    source_player(),
    keyboard(keyboardState, MidiKeyboardComponent::horizontalKeyboard),
    luaScript(nullptr)
{
    keyboardState.addListener(this);
    addAndMakeVisible(&keyboard);
    SharedResourcePointer<AudioDeviceManager> dm;
    dm->addAudioCallback(&source_player);
    dm->addAudioCallback(&metronome_player);
    dm->addMidiInputCallback("",&midiCallback);

    setBounds(0, 0, getWidth(), getHeight() + getKeyboardHeight());
    notesHeld.addChangeListener(this);
    
    addKeyListener(this);
    
    groups.add(new Group());
    
    source_player.setSource(&sampler);
    metronome_player.setSource(&metronome);
    sampler.prepareToPlay(0, 44100.0);
    metronome.prepareToPlay(0, 44100.0);
    MainContentComponent* m = static_cast<MainContentComponent*>((&instrument)->getParent()->getParent());
    //std::cout<<"post parent: "<<m<<std::endl;
    //std::cout<<"asdf: "<< m->getMetronome()<<" asdd"<<std::endl;
    (&instrument)->getParent()->getParent()->getMetronome()->setMetronome(&metronome);
    (&instrument)->getParent()->getParent()->getTransport()->setMetronome(&metronome);
    
    groupEditorY = getHeight();
}

void MappingEditorGraph::buttonClicked(Button* source){
    if (source == group_editor->getCreateGroupButton()){
        groups.add(new Group());
    }
    else if (source == group_editor->getDeleteGroupButton()){
        SparseSet<int> s = getGroupEditor()->getListBox()->getSelectedRows();
        for (int i=s.size()-1; i>=0; i--){
            delete groups[s[i]];
            groups.remove(s[i]);
        }
    }
}

void MappingEditorGraph::updateZones(){
    if (zones.size() > 0){
        for (int i=0; i<groups.size(); i++){
            for (int j=0; j<zones.size(); j++){
                zones[j]->setVisible(false);
            }
        }
        lassoSource.getZones().clear();
        SparseSet<int> s = getGroupEditor()->getListBox()->getSelectedRows();
        for (int i=0; i<s.size(); i++){
            if (groups[s[i]]->getZones() != nullptr && groups[s[i]]->getZones()->size() > 0){
                for (int j=0; j<groups[s[i]]->getZones()->size(); j++){
                    (*(groups[s[i]]->getZones()))[j]->setVisible(true);
                    lassoSource.getZones().add((*(groups[s[i]]->getZones()))[j]);
                }
            }
        }
    }
}
    
void MappingEditorGraph::MidiDeviceCallback::handleIncomingMidiMessage
    (MidiInput* source, const MidiMessage& message) 
{
    if (message.getChannel() == midi_input_id || midi_input_id== -1){
        if (message.isNoteOn()) 
        {
            if (luaScript == nullptr)
                luaScript = parent->getInstrument().getTabWindow().getScriptBin()->getLuaScript();
            
            parent->getNotesHeld().addToSelection(std::pair<int, int>(message.getNoteNumber(), message.getVelocity()));
            for (auto zone : parent->zones){
                if (zone->getNote() == message.getNoteNumber()){}
            }
            luaScript->onNote(message.getNoteNumber(), message.getVelocity(), message.getTimeStamp());
        }
        if (message.isNoteOff()) 
        {
            for (auto pair : parent->getNotesHeld()){
                if (pair.first == message.getNoteNumber()){
                    parent->getNotesHeld().deselect(pair);
                }
            }
            parent->getSampler().getMidiCollector().addMessageToQueue(message);
        }
        
        
    }
    //SharedResourcePointer<AudioDeviceManager> dm;
    //std::cout<<dm->getCpuUsage()<<std::endl;
}

void MappingEditorGraph::resized()
{
    keyboard.setBounds(
        0, getHeight(), getWidth(), getKeyboardHeight());
    keyboard.setKeyWidth(getWidth() / numColumns * 1.715); 
}

void MappingEditorGraph::handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity){
    notesHeld.addToSelection(std::pair<int, int>(midiNoteNumber, velocity*128));
    //sampler.getSynth()->noteOn(midiChannel, midiNoteNumber, velocity/**128*/);
    if (luaScript == nullptr)
        luaScript = instrument.getTabWindow().getScriptBin()->getLuaScript();
        
    MidiMessage* m = new MidiMessage(MidiMessage::noteOn(1, midiNoteNumber, velocity));
    m->setTimeStamp(10);
    
    auto n = std::make_shared<NoteEvent>();
    n->setTriggerNote(-1);
    n->setNoteNumber(midiNoteNumber);
    n->setVelocity(velocity);
    n->setId(sampler.getIdCount());
    
    for (int i=0; i<groups.size(); i++){
        n->getGroups().add(i);
    }
    
    sampler.incIdCount();
    sampler.getIncomingEvents().add(n);
    sampler.getMidiCollector().addMessageToQueue(*m);
}

void MappingEditorGraph::handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber){
    for (auto pair : notesHeld){
        if (pair.first == midiNoteNumber){
            notesHeld.deselect(pair);
        }
    }
}

void MappingEditorGraph::paint(Graphics& g)
{
    g.fillAll(Colours::antiquewhite);
    float gridOutline = 1.0f;
    float gridWidth = width / numColumns;
    g.setColour(Colours::black);
    g.setOpacity(0.2f);
    
    for (auto i : notesHeld) {
        g.fillRect(Rectangle<int>((i.first)*gridWidth, 0, gridWidth, height));
    }
    
    g.setColour(Colours::yellow);
    g.setOpacity(0.8f);
    for (auto i : notesHeld) {
        g.fillRect(Rectangle<int>((i.first)*gridWidth, (127 - i.second) * height/128, gridWidth, height/128));
    }

    g.setColour(Colours::black);
    g.setOpacity(1.0f);
    Path myPath;
    for (int i=0; i < numColumns; i++){
        myPath.startNewSubPath (i*gridWidth, 0.0f);
        myPath.lineTo (i*gridWidth, height);
    }
    myPath.startNewSubPath(0.0f, 0.0f);
    myPath.lineTo(numColumns*gridWidth, 0.0f);

    g.strokePath (myPath, PathStrokeType (gridOutline));
}

void MappingEditorGraph::loadPatch(XmlElement* i){
    
   group_editor->removeGroups();    
   groups.clear();
   zones.clear();
   getSampler().getSynth()->clearSounds();
            
   int j=0;
   forEachXmlChildElement(*i, group){
     if (group->hasTagName("GROUP")){
        Group* new_group = new Group();
        new_group->setName(group->getStringAttribute("name"));
        
        group_editor->addGroup();
        group_editor->getModel()->getGroupNames().set(j, new_group->getName());
        groups.add(new_group);
        
        j++;
                                
        forEachXmlChildElement(*group, zone){
            if (zone->hasTagName("ZONE")){
                Zone* new_zone;
                new_zone = new Zone(this, zone->getStringAttribute("file"), instrument);
                zones.add(new_zone);
                new_group->getZones()->add(new_zone);

                new_zone->removeListener(this);
                new_zone->addListener(this);
                new_zone->removeMouseListener(new_zone);
                new_zone->addMouseListener(this, true);

                new_zone->setX(zone->getIntAttribute("x"));
                new_zone->setNote(zone->getIntAttribute("note"));
                new_zone->setY(zone->getIntAttribute("y"));
                new_zone->setHeight(zone->getIntAttribute("height"));
                new_zone->set_width(zone->getIntAttribute("width"));
                new_zone->getPlaySettings().setSampleStart(zone->getDoubleAttribute("sample_start"));

                float gridWidth = get_width() / getNumColumns();
                new_zone->setBounds(new_zone->getX(), new_zone->getY(), 
                new_zone->get_width() * gridWidth - 1, new_zone->getHeight());
                                                
                addAndMakeVisible(new_zone);
                
                Array<int> groups_for_zone;
                groups_for_zone.add(j-1);
        
                getSampler().addSample(new_zone->getName(), new_zone->getNote(), round(new_zone->getX()/gridWidth), round(new_zone->getX()/gridWidth) + new_zone->get_width(), groups_for_zone, new_zone->getPlaySettings().getSampleStart());
            }
        }
     }
   } 
   group_editor->getListBox()->selectRow(0);
   getGroupEditor()->getGroupView()->refreshRows();
}

void MappingEditorGraph::filesDropped(const StringArray& files, int x, int y){
    float gridOutline = 1.0f;
    float gridWidth = width / numColumns;
    Zone* newZone;
    for (int i=0; i<files.size(); i++){
        
        if (File(files[i]).isDirectory()){continue;}
        
        try{
            newZone = new Zone(this, files[i], instrument);
        }catch (BadFormatException const & e){
            //there's a crash here...why?
            continue;
        }  
        zones.add(newZone);
        
        Array<int> groups_for_zone;
        SparseSet<int> s = getGroupEditor()->getListBox()->getSelectedRows();
        for (int i=0; i<s.size(); i++){
            groups[s[i]]->getZones()->add(newZone);
            groups_for_zone.add(s[i]);
        }

        newZone->removeListener(this);
        newZone->addListener(this);
        newZone->removeMouseListener(newZone);
        newZone->addMouseListener(this, true);

        newZone->setX(roundToInt(x / gridWidth) * gridWidth +
            gridOutline + gridWidth*i);
        newZone->setNote(round(newZone->getX()/gridWidth));
        newZone->setY(0);
        newZone->setHeight(getHeight());
        newZone->setRangeLow(newZone->getNote());
        newZone->setRangeHigh(newZone->getNote());
        newZone->setBounds(newZone->getX(), newZone->getY(), 
            gridWidth - gridOutline, newZone->getHeight());
        addAndMakeVisible(newZone);
        newZone->set_width(1);
        lassoSource.getZones().add(newZone);
        
        sampler.addSample(newZone->getName(), newZone->getNote(), newZone->getNote(), newZone->getNote()+1, groups_for_zone, newZone->getPlaySettings().getSampleStart());
        
    }
    getZoneInfoSet().selectOnly(newZone);
    getGroupEditor()->getGroupView()->refreshRows();
}

bool MappingEditorGraph::keyPressed(const KeyPress& key, Component* c){
    const int KEY_C = 99;
    const int KEY_V = 118;
    const int KEY_DELETE = 268435711;
    float gridOutline = 1.0f;
    float gridWidth = width / numColumns;
    ModifierKeys modifier_keys(key.getModifiers());
    
    if (c == this){
        //copy zones
        if (modifier_keys.isCtrlDown() && key.getKeyCode() == KEY_C){
            copied_zones.clear();
            for (auto zone : lassoSource.getLassoSelection()){
                Zone* new_zone = new Zone(this, zone->getName(), instrument);
                new_zone->removeListener(this);
                new_zone->addListener(this);
                new_zone->removeMouseListener(new_zone);
                new_zone->addMouseListener(this, true);
                new_zone->setX(zone->getX());
                new_zone->setNote(zone->getNote());
                new_zone->setY(zone->getY());
                new_zone->setHeight(zone->getHeight());
                new_zone->set_width(zone->get_width());
                new_zone->setBounds(new_zone->getX(), new_zone->getY(), 
                                    gridWidth * new_zone->get_width() - gridOutline, new_zone->getHeight());
                new_zone->getPlaySettings().setSampleStart(zone->getPlaySettings().getSampleStart());
                
                copied_zones.add(new_zone);
            }
            return true;
        }
        
        //paste zones
        if (modifier_keys.isCtrlDown() && key.getKeyCode() == KEY_V){
            for (auto zone : copied_zones){
                addAndMakeVisible(zone);
                
                Array<int> groups_for_zone;
                SparseSet<int> s = getGroupEditor()->getListBox()->getSelectedRows();
                for (int i=0; i<s.size(); i++){
                    groups_for_zone.add(s[i]);
                }
                
                sampler.addSample(zone->getName(), 
                                  zone->getNote(), 
                                  round(zone->getX()/gridWidth), 
                                  round(zone->getX()/gridWidth) + zone->get_width(),
                                  groups_for_zone,
                                  zone->getPlaySettings().getSampleStart());

                                   
                zones.add(zone);
                
                for (int i=0; i<s.size(); i++){
                    groups[s[i]]->getZones()->add(zone);
                }
                lassoSource.getZones().add(zone);
            }
            getGroupEditor()->getGroupView()->refreshRows();
            return true;
        }
        
        if (key.getKeyCode() == KEY_DELETE){
            for (auto zone : lassoSource.getLassoSelection()){
                int zone_index = zones.indexOf(zone);
                zones.removeFirstMatchingValue(zone);
                sampler.getSynth()->removeSound(zone_index);
                lassoSource.getZones().removeFirstMatchingValue(zone);
                SparseSet<int> s = getGroupEditor()->getListBox()->getSelectedRows();
                for (int i=0; i<s.size(); i++){
                    groups[s[i]]->getZones()->removeFirstMatchingValue(zone);
                }
                removeChildComponent(zone);
                if (zoneInfoSet.isSelected(zone)) zoneInfoSet.deselect(zone);
            }
            getGroupEditor()->getGroupView()->refreshRows();
            return true;
        }       
    }
    return false;
}

void MappingEditorGraph::mouseDown(const MouseEvent& e) {
    addAndMakeVisible(lasso);
    lasso.beginLasso(e, &lassoSource);
    startDragY = getMouseXYRelative().getY();
    startDragX = getMouseXYRelative().getX();
    
}

void MappingEditorGraph::setBoundsForComponent(Zone& c, MouseCursor cursor,
    float gridOutline, float gridWidth, int gridXOffset){
    int y = getMouseXYRelative().getY();
    int delY = y - startDragY;
    int x = getMouseXYRelative().getX();
    int delX = x - startDragX;
    if (cursor == MouseCursor::NormalCursor){
        int gridX = roundToInt((c.getX()) / gridWidth);
        Range<int> r(0,numColumns-1);
        int newGridX = r.clipValue(gridXOffset + gridX); 
        int delX_grid_units = (int)(delX / gridWidth);
        
        int newY = 0;
        if (c.getY() + delY < 0){
            newY = 0;
        }
        else if (c.getY() + delY + c.getHeight() > height){
            newY = height - c.getHeight();
        }
        else if (c.getY() + delY >= 0 &&
                c.getY() + delY + c.getHeight() <= height){
            newY = c.getY() + delY;
        }
        
        if ((c.getX()/gridWidth + delX_grid_units) * gridWidth + gridOutline <= 128*gridWidth && (c.getX()/gridWidth + delX_grid_units) * gridWidth + gridOutline >=0){
            c.setBounds((round(c.getX()/gridWidth) + delX_grid_units) * gridWidth + gridOutline, newY, gridWidth * c.get_width() - gridOutline, c.getHeight());
            c.getVelocity().first = (int)((getHeight() - (newY + c.getHeight())) / (getHeight() / 128));
            c.getVelocity().second = (int)((getHeight() - newY) / (getHeight() / 128));
            //c.setNote(c.getX()/gridWidth + delX_grid_units);
        }
        if (c.getVelocity().second > 127){c.getVelocity().second = 127;}
    }

    else if (cursor == MouseCursor::TopEdgeResizeCursor) {
        int newY = c.getY() + delY;
        int newHeight = c.getHeight() - delY;

        if (newY < 0){
            newY = 0;
            newHeight = c.getHeight() + c.getY();
        }
        c.setBounds(c.getX(), newY, c.get_width() * gridWidth - gridOutline, newHeight);
        c.getVelocity().second = (int)((getHeight() - newY) / (getHeight() / 128));
        if (c.getVelocity().second > 127){c.getVelocity().second = 127;}
    }
    else if (cursor == MouseCursor::BottomEdgeResizeCursor){
        int y = getMouseXYRelative().getY();
        int newHeight = c.getHeight() + delY;
        if (newHeight + c.getY() > height){
            newHeight = height - (c.getY() + c.getHeight()) + c.getHeight();
        }
        c.setBounds(c.getX(), c.getY(), 
            c.get_width() * gridWidth - gridOutline, newHeight);
        c.getVelocity().first = (int)((getHeight() - (c.getY() + newHeight)) / (getHeight() / 128));
    }
    else if (cursor == MouseCursor::RightEdgeResizeCursor){
        int new_grid_width = (int)((c.get_width() + delX) / gridWidth);
        if (new_grid_width + c.get_width() >= 1 && c.getX() + c.get_width() + new_grid_width * gridWidth <= 128*gridWidth){
            c.setSize(c.get_width() * gridWidth + (new_grid_width * gridWidth) - gridOutline, c.getHeight());
        }
    }
    else if (cursor == MouseCursor::LeftEdgeResizeCursor){
        int added_grid_width = (int)((c.get_width() + (-1*delX)) / gridWidth);
        int new_grid_x = (int)(c.getX() / gridWidth) - added_grid_width;
        if (added_grid_width + c.get_width() >= 1 && new_grid_x*gridWidth + gridOutline >=0){
            c.setBounds(new_grid_x * gridWidth + gridOutline, c.getY(), c.get_width() * gridWidth + (added_grid_width * gridWidth) - gridOutline, c.getHeight());
        }
    }
}

void InstrumentMappingEditor::MappingEditorGraph::mouseDrag(const MouseEvent& e){
    if (!lassoSource.Dragging()){lassoSource.setDragging(true);}
    if (draggedZone != nullptr){
        float gridOutline = 1.0f;
        float gridWidth = width / numColumns;

        int gridXOffset = e.x / gridWidth;

        if (lassoSource.getLassoSelection().getItemArray().contains(draggedZone)){
            for (auto i : lassoSource.getLassoSelection()) {
                setBoundsForComponent(*i, draggedZone->getMouseCursor(), 
                    gridOutline, gridWidth, gridXOffset);
            }
        }else{
            setBoundsForComponent(*draggedZone, draggedZone->getMouseCursor(), gridOutline, gridWidth, gridXOffset);
        }
        getZoneInfoSet().selectOnly(draggedZone);
        getZoneInfoSet().changed();
    }else {
        lasso.dragLasso(e);
    }
}

void InstrumentMappingEditor::MappingEditorGraph::mouseUp(const MouseEvent& e){
    getZoneInfoSet().changed();
    auto set = lassoSource.getLassoSelection();
    int x = getMouseXYRelative().getX();
    int gridWidth = round(width/128);
    int delX = x - startDragX;
    
    if (draggedZone != nullptr){
        
        auto cursor = draggedZone->getMouseCursor();
        if (cursor == MouseCursor::NormalCursor){
            if (lassoSource.getLassoSelection().getItemArray().contains(draggedZone)){
                for (auto i : lassoSource.getLassoSelection()){
                    int newY = getMouseXYRelative().getY() - startDragY;
                    int old_x = i->getX();
                    i->setX(i->getBounds().getX());
                    i->setNote(i->getNote() + round((i->getX()-old_x) / gridWidth));
                    if (i->getY() + newY + i->getHeight() > height){
                        newY = height - i->getHeight();
                        i->setY(newY);
                    }
                    else if (i->getY() + newY < 0){
                        newY = 0;
                        i->setY(newY);
                    }
                    else if (i->getY() + newY >= 0 && 
                            i->getY() + newY + i->getHeight() <= height){
                        i->setY(newY+i->getY());
                    }
                    int zone_index = zones.indexOf(i);
                    zones.remove(zone_index);
                    sampler.getSynth()->removeSound(zone_index);
                    zones.add(i);
                    
                    Array<int> groups_for_zone;
                    SparseSet<int> s = getGroupEditor()->getListBox()->getSelectedRows();
                    for (int j=0; j<s.size(); j++){
                        if (groups[s[j]]->getZones()->contains(i)){
                            groups_for_zone.add(s[j]);
                        }
                    }
                    
                    sampler.addSample(i->getName(), 
                                      i->getNote(), 
                                      round(i->getX()/gridWidth), 
                                      round(i->getX()/gridWidth) + i->get_width(),
                                      groups_for_zone,
                                      i->getPlaySettings().getSampleStart());
                }
            }else{
                auto Y = getMouseXYRelative().getY();
                auto y = draggedZone->getY();
                int newY = y + Y - startDragY;
                if (newY + draggedZone->getHeight() > height){
                    newY = height - draggedZone->getHeight();
                }
                else if (newY < 0){
                    newY = 0;
                }
                
                int old_x = draggedZone->getX();
                draggedZone->setX(draggedZone->getBounds().getX());
                draggedZone->setNote(draggedZone->getNote() + round((draggedZone->getX()-old_x) / gridWidth));
                draggedZone->setY(newY);
                int zone_index = zones.indexOf(draggedZone);
                zones.remove(zone_index);
                sampler.getSynth()->removeSound(zone_index);
                zones.add(draggedZone);
                
                Array<int> groups_for_zone;
                SparseSet<int> s = getGroupEditor()->getListBox()->getSelectedRows();
                for (int i=0; i<s.size(); i++){
                    if (groups[s[i]]->getZones()->contains(draggedZone)){
                        groups_for_zone.add(s[i]);
                    }
                }
                sampler.addSample(draggedZone->getName(), 
                                  draggedZone->getNote(), 
                                  round(draggedZone->getX()/gridWidth), 
                                  round(draggedZone->getX()/gridWidth) + draggedZone->get_width(),
                                  groups_for_zone,
                                  draggedZone->getPlaySettings().getSampleStart());
            }
        }
        else if (cursor == MouseCursor::TopEdgeResizeCursor){
            if (lassoSource.getLassoSelection().getItemArray().contains(draggedZone)){
                for (auto i : lassoSource.getLassoSelection()){
                    int newY = i->getY() + (getMouseXYRelative().getY() - 
                        startDragY);
                    int newHeight = i->getHeight() - 
                        (getMouseXYRelative().getY() - startDragY);
                    if (newY < 0){
                        newY = 0;
                        newHeight = i->getHeight() + i->getY();
                    }
                    i->setHeight(newHeight);
                    i->setY(newY);
                }
            } else {
                int newY = draggedZone->getY() + 
                    (getMouseXYRelative().getY() - startDragY);
                int newHeight = draggedZone->getHeight() -
                    (getMouseXYRelative().getY() - startDragY);

                if (newY < 0){
                    newY = 0;
                    newHeight = draggedZone->getHeight() + 
                        draggedZone->getY();
                }
                draggedZone->setHeight(newHeight);
                draggedZone->setY(newY);
            }
        }
        else if (cursor == MouseCursor::BottomEdgeResizeCursor){
            if (lassoSource.getLassoSelection().getItemArray().contains(draggedZone)){
                for (auto i : lassoSource.getLassoSelection()){
                    int newHeight = i->getHeight() + 
                        (getMouseXYRelative().getY() - startDragY);
                    if (newHeight + i->getY() > height){
                        newHeight = height - (i->getY() + i->getHeight()) + 
                            i->getHeight();
                    }
                    i->setHeight(newHeight);
                }
            } else {
                int newHeight = draggedZone->getHeight() +
                    (getMouseXYRelative().getY() - startDragY);
                if (newHeight + draggedZone->getY() > height){
                    newHeight = height - (draggedZone->getY() + 
                        draggedZone->getHeight()) + draggedZone->getHeight();
                }
                draggedZone->setHeight(newHeight);
            }
        }
        else if (cursor == MouseCursor::RightEdgeResizeCursor){
            float gridWidth = width / numColumns;
            if (lassoSource.getLassoSelection().getItemArray().contains(draggedZone)){
                for (auto i : lassoSource.getLassoSelection()){
                    int newWidth= round(i->getWidth() / gridWidth);
                    if (newWidth >= 1){
                        i->set_width(newWidth);
                        int zone_index = zones.indexOf(i);
                        zones.remove(zone_index);
                        sampler.getSynth()->removeSound(zone_index);
                        zones.add(i);
                        
                        Array<int> groups_for_zone;
                        SparseSet<int> s = getGroupEditor()->getListBox()->getSelectedRows();
                        for (int j=0; j<s.size(); j++){
                            if (groups[s[j]]->getZones()->contains(i)){
                                groups_for_zone.add(s[j]);
                            }
                        }
                        sampler.addSample(i->getName(), 
                                          i->getNote(), 
                                          round(i->getX()/gridWidth), 
                                          round(i->getX()/gridWidth) + i->get_width(),
                                          groups_for_zone,
                                          i->getPlaySettings().getSampleStart());
                    }
                }
            } else {
                int newWidth= round(draggedZone->getWidth() / gridWidth);
                if (newWidth >= 1){
                    draggedZone->set_width(newWidth);
                    int zone_index = zones.indexOf(draggedZone);
                    zones.remove(zone_index);
                    sampler.getSynth()->removeSound(zone_index);
                    zones.add(draggedZone);
                    
                    Array<int> groups_for_zone;
                    SparseSet<int> s = getGroupEditor()->getListBox()->getSelectedRows();
                    for (int i=0; i<s.size(); i++){
                        if (groups[s[i]]->getZones()->contains(draggedZone)){
                            groups_for_zone.add(s[i]);
                        }
                    }
                    sampler.addSample(draggedZone->getName(), 
                                      draggedZone->getNote(), 
                                      round(draggedZone->getX()/gridWidth), 
                                      round(draggedZone->getX()/gridWidth) + draggedZone->get_width(),
                                      groups_for_zone,
                                      draggedZone->getPlaySettings().getSampleStart());
                }
            }
        }
        else if (cursor == MouseCursor::LeftEdgeResizeCursor){
            float gridWidth = width / numColumns;
            if (lassoSource.getLassoSelection().getItemArray().contains(draggedZone)){
                for (auto i : lassoSource.getLassoSelection()){
                    int newWidth= round(i->getWidth() / gridWidth);
                    if (newWidth >= 1){
                        i->set_width(newWidth);
                        i->setX(i->getBounds().getX());
                        if (round(i->getX()/gridWidth) > i->getNote()){
                            i->setNote(round(i->getX()/gridWidth));
                            int zone_index = zones.indexOf(i);
                            zones.remove(zone_index);
                            sampler.getSynth()->removeSound(zone_index);
                            zones.add(i);
                            
                            Array<int> groups_for_zone;
                            SparseSet<int> s = getGroupEditor()->getListBox()->getSelectedRows();
                            for (int j=0; j<s.size(); j++){
                                if (groups[s[j]]->getZones()->contains(i)){
                                    groups_for_zone.add(s[j]);
                                }
                            }
                            
                            sampler.addSample(i->getName(), 
                                              i->getNote(), 
                                              round(i->getX()/gridWidth), 
                                              round(i->getX()/gridWidth) + i->get_width(),
                                              groups_for_zone,
                                              i->getPlaySettings().getSampleStart());
                        }
                    }
                }
            } else {
                int newWidth= round(draggedZone->getWidth() / gridWidth);
                if (newWidth >= 1){
                    draggedZone->set_width(newWidth);
                    draggedZone->setX(draggedZone->getBounds().getX());
                    if (round(draggedZone->getX()/gridWidth) > draggedZone->getNote()){
                        draggedZone->setNote(round(draggedZone->getX()/gridWidth));
                    }
                    int zone_index = zones.indexOf(draggedZone);
                    zones.remove(zone_index);
                    sampler.getSynth()->removeSound(zone_index);
                    zones.add(draggedZone);
                    
                    Array<int> groups_for_zone;
                    SparseSet<int> s = getGroupEditor()->getListBox()->getSelectedRows();
                    for (int i=0; i<s.size(); i++){
                        if (groups[s[i]]->getZones()->contains(draggedZone)){
                            groups_for_zone.add(s[i]);
                        }
                    }
                    sampler.addSample(draggedZone->getName(), 
                                      draggedZone->getNote(), 
                                      round(draggedZone->getX()/gridWidth), 
                                      round(draggedZone->getX()/gridWidth) + draggedZone->get_width(),
                                      groups_for_zone,
                                      draggedZone->getPlaySettings().getSampleStart());
                }
            }
        }
        draggedZone = nullptr;
    }
    lasso.endLasso();
    
    if (! lassoSource.Dragging()){
        for (auto i : lassoSource.getLassoSelection()){
            i->setToggleState(false, sendNotification);
            if (i->getMouseCursor() == MouseCursor::NormalCursor){
                i->setY(i->getY()+getMouseXYRelative().getY() - startDragY);
            }
        }
        lassoSource.getLassoSelection().deselectAll();
    }
    lassoSource.setDragging(false);
}


typedef InstrumentMappingEditor::MappingEditorGraph::Zone Zone;

Zone::Zone(MappingEditorGraph* p, const String& sampleName, InstrumentComponent& i) 
    : TextButton{""}, parent{p}, instrument(i), playSettings(),
    name(sampleName)  {
    setAlpha(0.5f);
    velocity.first = 0;
    velocity.second = 127;
    playSettings.setSampleStart(0.0);
}
    

void Zone::mouseDown(const MouseEvent& e) {
    parent->draggedZone = this;
    parent->getZoneInfoSet().selectOnly(this);
}

void Zone::mouseMove(const MouseEvent& e) {
    if (! parent->isDragging()){
        if (e.y < 5){
            setMouseCursor(MouseCursor::TopEdgeResizeCursor);
            return;
        }
        else if (getHeight() - e.y < 5){
            setMouseCursor(MouseCursor::BottomEdgeResizeCursor);
            return;
        }
        else if (e.x < 5){
            setMouseCursor(MouseCursor::LeftEdgeResizeCursor);
            return;
        }
        else if (getWidth() - e.x < 5){
            setMouseCursor(MouseCursor::RightEdgeResizeCursor);
            return;
        }
        setMouseCursor(MouseCursor());
    }
}

void Zone::mouseDoubleClick(const MouseEvent& e){}
