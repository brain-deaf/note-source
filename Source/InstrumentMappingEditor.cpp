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
#include "SamplerEventProcessor.h"
#include "FilePlayer.h"
#include "MainComponent.h"
#include "InstrumentTabWindow.h"
#include "RoundRobin.h"

InstrumentMappingEditor::InstrumentMappingEditor(const String& componentName, InstrumentComponent& i)
:   Component(), instrument(i), view_port(new Viewport(componentName)),
    group_editor(new GroupEditor(200, 333 + 100, this)),
    graph(new MappingEditorGraph(1800.0f, 315.0f, 100.0f, 128, i, group_editor.get()))
{
    
    graph->getGroupEditor() = group_editor;
    view_port->setViewedComponent(graph);
    int group_editor_width = 200;
    view_port->setBounds(group_editor_width, 0, 1000 - group_editor_width, 333 + 100);
    
    group_editor->getCreateGroupButton()->addListener(graph);
    group_editor->getDeleteGroupButton()->addListener(graph);
    group_editor->getGroupDownButton()->addListener(graph);
    group_editor->getGroupUpButton()->addListener(graph);
    
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
	
	graph = nullptr;
    view_port = nullptr;
    group_editor = nullptr;
}


MappingEditorGraph::MappingEditorGraph(float w, float h,
    float kh, int nc, InstrumentComponent& i, GroupEditor* g)
: Component(), width(w), height(h), keyboardHeight(kh), group_editor(g),
    numColumns(nc), draggedZone(nullptr), dragging(false), groupEditorY(0),
    lasso(), lassoSource(this), instrument(i), midiCallback(this), 
    keyboardState(), zones(), metronome(), metronome_player(),
    source_player(), zoneDown(false), samplerProcessor(/*MainContentComponent::_static_sampler->getSamplerProcessor()*/ nullptr),
    keyboard(keyboardState, MidiKeyboardComponent::horizontalKeyboard),
    luaScript(nullptr), renderEventsButton(new TextButton("Render Events")),
	zoneCount(0), patchProgress(0.0), progressWindow(nullptr)
{
    keyboardState.addListener(this);
    addAndMakeVisible(&keyboard);
    SharedResourcePointer<AudioDeviceManager> dm;
    //dm->addAudioCallback(&source_player);
    //dm->addAudioCallback(&metronome_player);
    dm->addMidiInputCallback("",&midiCallback);
	
	SamplerProcessor* samplerProcessor = MainContentComponent::_static_sampler;
	if (samplerProcessor != nullptr){
		samplerProcessor->setMidiCallback(&midiCallback);
		samplerProcessor->setNotesHeld(&(getNotesHeld()));
	}

	//static Sampler* s(MainContentComponent::_static_sampler);
	//s->setNotesHeld(&(getNotesHeld()));
	//s->prepareToPlay(0, 44100.0);
	
    setBounds(0, 0, getWidth(), getHeight() + getKeyboardHeight());
    notesHeld.addChangeListener(this);
    
    addKeyListener(this);
    
    groups.add(new Group());
  
	//source_player.setSource(s);
    //metronome_player.setSource(&metronome);
    
    //metronome.prepareToPlay(0, 44100.0);
    //MainContentComponent* m = static_cast<MainContentComponent*>((&instrument)->getParent()->getParent());
    //std::cout<<"post parent: "<<m<<std::endl;
    //std::cout<<"asdf: "<< m->getMetronome()<<" asdd"<<std::endl;
    (&instrument)->getParent()->getParent()->getMetronome()->setMetronome(&metronome);
    (&instrument)->getParent()->getParent()->getTransport()->setMetronome(&metronome);
    
    groupEditorY = getHeight();
    
    
    //SamplerEventProcessor test...
    /*SamplerEvent s;
    s.setNoteNumber(60);
    s.setVelocity(60.0);
    s.getGroups().add(0);
    samplerProcessor.addSamplerEvent(s);
    SamplerEvent s2;
    s.setNoteNumber(63);
    s.setVelocity(60.0);
    s.setStart(44100);
    s.getGroups().add(0);
    samplerProcessor.addSamplerEvent(s);*/
    
    //addAndMakeVisible(renderEventsButton);
    //renderEventsButton->addListener(this);
}

MappingEditorGraph::~MappingEditorGraph()
{
	//SharedResourcePointer<AudioDeviceManager> dm;
	//dm->removeAudioCallback(&source_player);
	//dm->removeAudioCallback(&metronome_player);
	//dm->removeMidiInputCallback("", &midiCallback);
	for (int i = 0; i < zones.size(); i++){
		zones[i].get()->decReferenceCount();
	}
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
    else if (source == group_editor->getGroupDownButton()){
        SparseSet<int> s = getGroupEditor()->getListBox()->getSelectedRows();
        int size = group_editor->getModel()->getGroupNames().size();
        for (int i=s.size()-1; i>=0; i--){
            int new_index = s[i] + 1 < size ? s[i]+1 : size-1; 
            groups.move(s[i], new_index);
        }
    }
    else if (source == group_editor->getGroupUpButton()){
        SparseSet<int> s = getGroupEditor()->getListBox()->getSelectedRows();
        for (int i=0; i<s.size(); i++){
            int new_index = s[i] - 1 >= 0 ? s[i] - 1 : 0;
            groups.move(s[i], new_index);
        }
        
                
            
    }
    /*for (int i=0; i<groups.size(); i++){
        for (int j=0; j<sampler.getSynth()->getNumSounds(); j++){
            SampleSound* sound = sampler.getSynth()->getSound(j);
            sound->setGroup(0, */
                
    /*if (source == renderEventsButton){
        sampler.setupRendering();
        samplerProcessor.renderAllEvents();
    }*/
}

MidiDeviceCallback& MappingEditorGraph::getMidiCallback() { return midiCallback; }

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

void MappingEditorGraph::updateZone(Zone* i){
    int zone_index = zones.indexOf(i);
    zones.remove(zone_index);
    MainContentComponent::_static_sampler->getSynth()->removeSound(zone_index);
    zones.add(i);
    
    float gridWidth = get_width() / getNumColumns();
                    
    Array<int> groups_for_zone;
    SparseSet<int> s = getGroupEditor()->getListBox()->getSelectedRows();
    for (int j=0; j<s.size(); j++){
        if (groups[s[j]]->getZones()->contains(i)){
            groups_for_zone.add(s[j]);
        }
    }
                    
    MainContentComponent::_static_sampler->addSample(i->getName(), 
                      i->getNote(), 
                      round(i->getX()/gridWidth), 
                      round(i->getX()/gridWidth) + i->get_width(),
                      groups_for_zone,
                      i->getPlaySettings(),
                      i->getVelocity(),
					  i->getRRGroup()->getProcessor());
}
    
    
void MidiDeviceCallback::handleIncomingMidiMessage
    (MidiInput* , const MidiMessage& message) 
{
    if (message.getChannel() == midi_input_id || midi_input_id== -1){
        if (message.isNoteOn()) 
        {
            if (luaScript == nullptr)
                luaScript = parent->getInstrument().getTabWindow()->getScriptBin()->getLuaScript();
            
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
            MainContentComponent::_static_sampler->getMidiCollector().addMessageToQueue(message);
        }
    }
    //SharedResourcePointer<AudioDeviceManager> dm;
    //std::cout<<dm->getCpuUsage()<<std::endl;
}

void MidiDeviceCallback::handleMidiBuffer(MidiBuffer& buffer)
{
	MidiBuffer buffer_copy = MidiBuffer(buffer);
	buffer.clear();
	MidiMessage message;
	int samplePosition;
	MidiBuffer::Iterator i(buffer_copy);
	while (i.getNextEvent(message, samplePosition)){
		if (message.getChannel() == midi_input_id || midi_input_id == -1){
			int numEvents = buffer.getNumEvents();
			if (message.isNoteOn())
			{
				if (luaScript == nullptr)
					luaScript = parent->getInstrument().getTabWindow()->getScriptBin()->getLuaScript();

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
				//MainContentComponent::_static_sampler->getMidiCollector().addMessageToQueue(message);
			}
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
    renderEventsButton->setBounds(100, 100, 100, 30);
}

void MappingEditorGraph::handleNoteOn(MidiKeyboardState* /*source*/, int /*midiChannel*/, int midiNoteNumber, float velocity){
    notesHeld.addToSelection(std::pair<int, int>(midiNoteNumber, velocity*128));
    //sampler.getSynth()->noteOn(midiChannel, midiNoteNumber, velocity/**128*/);
    if (luaScript == nullptr)
        luaScript = instrument.getTabWindow()->getScriptBin()->getLuaScript();
        
    MidiMessage* m = new MidiMessage(MidiMessage::noteOn(1, midiNoteNumber, velocity));
    m->setTimeStamp(10);
    
    auto n = std::make_shared<NoteEvent>();
    n->setTriggerNote(-1);
    n->setNoteNumber(midiNoteNumber);
    n->setVelocity(velocity*128);
    n->setId(MainContentComponent::_static_sampler->getIdCount());
    
    for (int i=0; i<groups.size(); i++){
        n->getGroups().add(i);
    }
    
    MainContentComponent::_static_sampler->incIdCount();
    for (int i=0; i<MainContentComponent::_static_sampler->getSynth()->getNumSounds(); i++){
        if (MainContentComponent::_static_sampler->getSynth()->getSound(i)->appliesToNote(midiNoteNumber)){
			MainContentComponent::_static_sampler->getIncomingEvents().add(n);
        }
    }
    MainContentComponent::_static_sampler->getMidiCollector().addMessageToQueue(*m);
}

void MappingEditorGraph::handleNoteOff(MidiKeyboardState* /*source*/, int /*midiChannel*/, int midiNoteNumber){
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
   MainContentComponent::_static_sampler->getSynth()->clearSounds();
            
   int j=0;
   
   //double percent(0.0);
   //ProgressBar p(percent);
   //instrument.addAndMakeVisible(&p);
   //p.setBounds(400, 300, 300, 80);
   
   new_zones.clear();
   
   forEachXmlChildElement(*i, group){
     if (group->hasTagName("GROUP")){
         const MessageManagerLock l;
        Group* new_group = new Group();
        new_group->setName(group->getStringAttribute("name"));
        
        group_editor->addGroup();
        group_editor->getModel()->getGroupNames().set(j, new_group->getName());
        groups.add(new_group);
        
        j++;
         
        zoneCount = 0;
        forEachXmlChildElement(*group, zone){
            zoneCount++;
        }
        
        int count = 0;
        forEachXmlChildElement(*group, zone){
            patchProgress = ((double)(count))/((double)(zoneCount-1));
            //patchProgress = 0.5;
            if (zone->hasTagName("ZONE")){
                ReferenceCountedObjectPtr<Zone>  new_zone;
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
                
                float px_per_velocity = getHeight() / 128;
                int velocity_low = (getHeight() - (new_zone->getHeight()+new_zone->getY())) / px_per_velocity;
                int velocity_high = (getHeight() - new_zone->getY()) / px_per_velocity;
                std::pair<int, int> velocity;
                velocity.first = velocity_low;
                velocity.second = velocity_high;
                new_zone->setVelocity(velocity);
                
                new_zone->set_width(zone->getIntAttribute("width"));
                new_zone->getPlaySettings()->setSampleStart(zone->getDoubleAttribute("sample_start"));
				new_zone->getPlaySettings()->setReleaseStart(zone->getDoubleAttribute("release_start"));
				new_zone->getPlaySettings()->setReleaseTime(zone->getDoubleAttribute("release_time"));
				new_zone->getPlaySettings()->setReleaseCurve(zone->getDoubleAttribute("release_curve"));
                new_zone->getPlaySettings()->setLoopMode(zone->getIntAttribute("loop_mode")!=0);
				new_zone->getPlaySettings()->setReleaseMode(zone->getIntAttribute("release_mode") != 0);
                new_zone->getPlaySettings()->setLoopStart(zone->getDoubleAttribute("loop_start"));
                new_zone->getPlaySettings()->setLoopEnd(zone->getDoubleAttribute("loop_end"));
                new_zone->getPlaySettings()->setXfadeLength(zone->getDoubleAttribute("xfade_length"));
				new_zone->getPlaySettings()->setXfadeCurve(zone->getDoubleAttribute("xfade_curve"));

                float gridWidth = get_width() / getNumColumns();
                new_zone->setBounds(new_zone->getX(), new_zone->getY(), 
                new_zone->get_width() * gridWidth - 1, new_zone->getHeight());
                                                
                addAndMakeVisible(new_zone);
                
                Array<int> groups_for_zone;
                groups_for_zone.add(j-1);
                new_zone->setGroup(j-1);
                                       
                new_zones.add(new_zone);
                                       
                instrument.getTabWindow()->getWaveBin()->updateZone(new_zone);

				forEachXmlChildElement(*zone, round_robin){
					if (round_robin->hasTagName("RR")){
						new_zone->getRRGroup()->getRoundRobin(round_robin->getIntAttribute("index"))->setData(round_robin->getStringAttribute("file"));
						new_zone->getRRGroup()->getRoundRobin(round_robin->getIntAttribute("index"))->setState(round_robin->getBoolAttribute("state"));
					}
				}
            }
            count++;
        }
     }
   } 
   float gridWidth = get_width() / getNumColumns();
   progressWindow = new ProgressWindow(new_zones, MainContentComponent::_static_sampler, gridWidth);
   progressWindow->launchThread();
   
   group_editor->getListBox()->selectRow(0);
   getGroupEditor()->getGroupView()->refreshRows();
}

void MappingEditorGraph::itemDropped(const SourceDetails& details){
    std::cout<<details.description.toString()<<std::endl;
    
    if (details.description.toString() == "file name"){
        StringArray s;
        DragButton* d = static_cast<DragButton*>(details.sourceComponent.get());
        for (int i=0; i<d->getFileList().size(); i++){
            std::cout<<d->getFileList()[i]<<std::endl;
            if (File(d->getFileList()[i]).getFileExtension() != ".lua")
                s.add(d->getFileList()[i]);
        }
        
        if (s.size() > 0){
            filesDropped(s, details.localPosition.getX(), details.localPosition.getY());
        }
    }
}

void MappingEditorGraph::filesDropped(const StringArray& files, int x, int /*y*/){
    float gridOutline = 1.0f;
    float gridWidth = width / numColumns;
    ReferenceCountedObjectPtr<Zone>  newZone;
    for (int i=0; i<files.size(); i++){
        if (File(files[i]).isDirectory()){continue;}
        newZone = new Zone(this, files[i], instrument);
        zones.add(newZone);
        
        Array<int> groups_for_zone;
        SparseSet<int> s = getGroupEditor()->getListBox()->getSelectedRows();
        for (int j=0; j<s.size(); j++){
            groups[s[j]]->getZones()->add(newZone);
            groups_for_zone.add(s[j]);
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
        
        std::pair<int, int> velocity;
        velocity.first = 0;
        velocity.second = 127;
        newZone->setVelocity(velocity);
        
        lassoSource.getZones().add(newZone);
        
		if (!MainContentComponent::_static_sampler->addSample(newZone->getName(), newZone->getNote(), newZone->getNote(), newZone->getNote() + 1, groups_for_zone,
			newZone->getPlaySettings(), newZone->getVelocity(), newZone->getRRGroup()->getProcessor()))
		{
			newZone->removeListener(this);
			zones.removeFirstMatchingValue(newZone);
			for (int j = 0; j<s.size(); j++){
				groups[s[j]]->getZones()->removeFirstMatchingValue(newZone);
			}
			removeChildComponent(newZone);
			newZone = nullptr;
			return;
		}
        
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
                ReferenceCountedObjectPtr<Zone>  new_zone = new Zone(this, zone->getName(), instrument);
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
                new_zone->getPlaySettings()->setSampleStart(zone->getPlaySettings()->getSampleStart());
                new_zone->setVelocity(zone->getVelocity());
                
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
                
                MainContentComponent::_static_sampler->addSample(zone->getName(), 
                                  zone->getNote(), 
                                  round(zone->getX()/gridWidth), 
                                  round(zone->getX()/gridWidth) + zone->get_width(),
                                  groups_for_zone,
                                  zone->getPlaySettings(),
                                  zone->getVelocity(),
								  zone->getRRGroup()->getProcessor());

                                   
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
                MainContentComponent::_static_sampler->getSynth()->removeSound(zone_index);
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
    float gridOutline, float gridWidth, int /*gridXOffset*/){
    int y = getMouseXYRelative().getY();
    int delY = y - startDragY;
    int x = getMouseXYRelative().getX();
    int delX = x - startDragX;
    if (cursor == MouseCursor::NormalCursor){
        //int gridX = roundToInt((c.getX()) / gridWidth);
        Range<int> r(0,numColumns-1);
        //int newGridX = r.clipValue(gridXOffset + gridX); 
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
        //int y = getMouseXYRelative().getY();
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

void MappingEditorGraph::mouseDrag(const MouseEvent& e){
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

void MappingEditorGraph::mouseUp(const MouseEvent& ){
    //getZoneInfoSet().changed();
    auto set = lassoSource.getLassoSelection();
    //int x = getMouseXYRelative().getX();
    int gridWidth = round(width/128);
    //int delX = x - startDragX;
    
    
    
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
                    MainContentComponent::_static_sampler->getSynth()->removeSound(zone_index);
                    zones.add(i);
                    
                    Array<int> groups_for_zone;
                    SparseSet<int> s = getGroupEditor()->getListBox()->getSelectedRows();
                    for (int j=0; j<s.size(); j++){
                        if (groups[s[j]]->getZones()->contains(i)){
                            groups_for_zone.add(s[j]);
                        }
                    }
                    
                    float px_per_velocity = getHeight() / 128;
                    int velocity_low = (getHeight() - (i->getHeight()+i->getY())) / px_per_velocity;
                    int velocity_high = (getHeight() - i->getY()) / px_per_velocity;
                    std::pair<int, int> velocity;
                    velocity.first = velocity_low;
                    velocity.second = velocity_high;
                    
                    i->setVelocity(velocity);
                    
                    MainContentComponent::_static_sampler->addSample(i->getName(), 
                                      i->getNote(), 
                                      round(i->getX()/gridWidth), 
                                      round(i->getX()/gridWidth) + i->get_width(),
                                      groups_for_zone,
                                      i->getPlaySettings(),
                                      i->getVelocity(),
									  i->getRRGroup()->getProcessor());
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
                MainContentComponent::_static_sampler->getSynth()->removeSound(zone_index);
                zones.add(draggedZone);
                
                Array<int> groups_for_zone;
                SparseSet<int> s = getGroupEditor()->getListBox()->getSelectedRows();
                for (int i=0; i<s.size(); i++){
                    if (groups[s[i]]->getZones()->contains(draggedZone)){
                        groups_for_zone.add(s[i]);
                    }
                }
                
                float px_per_velocity = getHeight() / 128;
                int velocity_low = (getHeight() - (draggedZone->getHeight()+draggedZone->getY())) / px_per_velocity;
                int velocity_high = (getHeight() - draggedZone->getY()) / px_per_velocity;
                std::pair<int, int> velocity;
                velocity.first = velocity_low;
                velocity.second = velocity_high;
                draggedZone->setVelocity(velocity);
                
                MainContentComponent::_static_sampler->addSample(draggedZone->getName(), 
                                  draggedZone->getNote(), 
                                  round(draggedZone->getX()/gridWidth), 
                                  round(draggedZone->getX()/gridWidth) + draggedZone->get_width(),
                                  groups_for_zone,
                                  draggedZone->getPlaySettings(),
                                  draggedZone->getVelocity(),
								  draggedZone->getRRGroup()->getProcessor());
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
                    
                    int zone_index = zones.indexOf(i);
                    zones.remove(zone_index);
                    MainContentComponent::_static_sampler->getSynth()->removeSound(zone_index);
                    zones.add(i);
                    
                    Array<int> groups_for_zone;
                    SparseSet<int> s = getGroupEditor()->getListBox()->getSelectedRows();
                    for (int j=0; j<s.size(); j++){
                        if (groups[s[j]]->getZones()->contains(i)){
                            groups_for_zone.add(s[j]);
                        }
                    }
                    
                    float px_per_velocity = getHeight() / 128;
                    int velocity_low = (getHeight() - (i->getHeight()+i->getY())) / px_per_velocity;
                    int velocity_high = (getHeight() - i->getY()) / px_per_velocity;
                    std::pair<int, int> velocity;
                    velocity.first = velocity_low;
                    velocity.second = velocity_high;
                    i->setVelocity(velocity);
                    
                    MainContentComponent::_static_sampler->addSample(i->getName(), 
                                      i->getNote(), 
                                      round(i->getX()/gridWidth), 
                                      round(i->getX()/gridWidth) + i->get_width(),
                                      groups_for_zone,
                                      i->getPlaySettings(),
                                      i->getVelocity(),
									  i->getRRGroup()->getProcessor());
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
                
                int zone_index = zones.indexOf(draggedZone);
                zones.remove(zone_index);
                MainContentComponent::_static_sampler->getSynth()->removeSound(zone_index);
                zones.add(draggedZone);
                
                Array<int> groups_for_zone;
                SparseSet<int> s = getGroupEditor()->getListBox()->getSelectedRows();
                for (int i=0; i<s.size(); i++){
                    if (groups[s[i]]->getZones()->contains(draggedZone)){
                        groups_for_zone.add(s[i]);
                    }
                }
                
                float px_per_velocity = getHeight() / 128;
                int velocity_low = (getHeight() - (draggedZone->getHeight()+draggedZone->getY())) / px_per_velocity;
                int velocity_high = (getHeight() - draggedZone->getY()) / px_per_velocity;
                std::pair<int, int> velocity;
                velocity.first = velocity_low;
                velocity.second = velocity_high;
                draggedZone->setVelocity(velocity);
                
                MainContentComponent::_static_sampler->addSample(draggedZone->getName(), 
                                  draggedZone->getNote(), 
                                  round(draggedZone->getX()/gridWidth), 
                                  round(draggedZone->getX()/gridWidth) + draggedZone->get_width(),
                                  groups_for_zone,
                                  draggedZone->getPlaySettings(),
                                  draggedZone->getVelocity(),
								  draggedZone->getRRGroup()->getProcessor());
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
                    
                    int zone_index = zones.indexOf(i);
                    zones.remove(zone_index);
                    MainContentComponent::_static_sampler->getSynth()->removeSound(zone_index);
                    zones.add(i);
                    
                    Array<int> groups_for_zone;
                    SparseSet<int> s = getGroupEditor()->getListBox()->getSelectedRows();
                    for (int j=0; j<s.size(); j++){
                        if (groups[s[j]]->getZones()->contains(i)){
                            groups_for_zone.add(s[j]);
                        }
                    }
                    
                    float px_per_velocity = getHeight() / 128;
                    int velocity_low = (getHeight() - (i->getHeight()+i->getY())) / px_per_velocity;
                    int velocity_high = (getHeight() - i->getY()) / px_per_velocity;
                    std::pair<int, int> velocity;
                    velocity.first = velocity_low;
                    velocity.second = velocity_high;
                    i->setVelocity(velocity);
                    
                    MainContentComponent::_static_sampler->addSample(i->getName(), 
                                      i->getNote(), 
                                      round(i->getX()/gridWidth), 
                                      round(i->getX()/gridWidth) + i->get_width(),
                                      groups_for_zone,
                                      i->getPlaySettings(),
                                      i->getVelocity(),
									  i->getRRGroup()->getProcessor());
                }
            } else {
                int newHeight = draggedZone->getHeight() +
                    (getMouseXYRelative().getY() - startDragY);
                if (newHeight + draggedZone->getY() > height){
                    newHeight = height - (draggedZone->getY() + 
                        draggedZone->getHeight()) + draggedZone->getHeight();
                }
                draggedZone->setHeight(newHeight);
                
                int zone_index = zones.indexOf(draggedZone);
                zones.remove(zone_index);
                MainContentComponent::_static_sampler->getSynth()->removeSound(zone_index);
                zones.add(draggedZone);
                
                Array<int> groups_for_zone;
                SparseSet<int> s = getGroupEditor()->getListBox()->getSelectedRows();
                for (int i=0; i<s.size(); i++){
                    if (groups[s[i]]->getZones()->contains(draggedZone)){
                        groups_for_zone.add(s[i]);
                    }
                }
                
                float px_per_velocity = getHeight() / 128;
                int velocity_low = (getHeight() - (draggedZone->getHeight()+draggedZone->getY())) / px_per_velocity;
                int velocity_high = (getHeight() - draggedZone->getY()) / px_per_velocity;
                std::pair<int, int> velocity;
                velocity.first = velocity_low;
                velocity.second = velocity_high;
                draggedZone->setVelocity(velocity);
                
                MainContentComponent::_static_sampler->addSample(draggedZone->getName(), 
                                  draggedZone->getNote(), 
                                  round(draggedZone->getX()/gridWidth), 
                                  round(draggedZone->getX()/gridWidth) + draggedZone->get_width(),
                                  groups_for_zone,
                                  draggedZone->getPlaySettings(),
                                  draggedZone->getVelocity(),
								  draggedZone->getRRGroup()->getProcessor());
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
                        MainContentComponent::_static_sampler->getSynth()->removeSound(zone_index);
                        zones.add(i);
                        
                        Array<int> groups_for_zone;
                        SparseSet<int> s = getGroupEditor()->getListBox()->getSelectedRows();
                        for (int j=0; j<s.size(); j++){
                            if (groups[s[j]]->getZones()->contains(i)){
                                groups_for_zone.add(s[j]);
                            }
                        }
                        float px_per_velocity = getHeight() / 128;
                        int velocity_low = (getHeight() - (i->getHeight()+i->getY())) / px_per_velocity;
                        int velocity_high = (getHeight() - i->getY()) / px_per_velocity;
                        std::pair<int, int> velocity;
                        velocity.first = velocity_low;
                        velocity.second = velocity_high;
                        i->setVelocity(velocity);
                        
                        MainContentComponent::_static_sampler->addSample(i->getName(), 
                                          i->getNote(), 
                                          round(i->getX()/gridWidth), 
                                          round(i->getX()/gridWidth) + i->get_width(),
                                          groups_for_zone,
                                          i->getPlaySettings(),
                                          i->getVelocity(),
										  i->getRRGroup()->getProcessor());
                    }
                }
            } else {
                int newWidth= round(draggedZone->getWidth() / gridWidth);
                if (newWidth >= 1){
                    draggedZone->set_width(newWidth);
                    int zone_index = zones.indexOf(draggedZone);
                    zones.remove(zone_index);
                    MainContentComponent::_static_sampler->getSynth()->removeSound(zone_index);
                    zones.add(draggedZone);
                    
                    Array<int> groups_for_zone;
                    SparseSet<int> s = getGroupEditor()->getListBox()->getSelectedRows();
                    for (int i=0; i<s.size(); i++){
                        if (groups[s[i]]->getZones()->contains(draggedZone)){
                            groups_for_zone.add(s[i]);
                        }
                    }
                    
                    float px_per_velocity = getHeight() / 128;
                    int velocity_low = (getHeight() - (draggedZone->getHeight()+draggedZone->getY())) / px_per_velocity;
                    int velocity_high = (getHeight() - draggedZone->getY()) / px_per_velocity;
                    std::pair<int, int> velocity;
                    velocity.first = velocity_low;
                    velocity.second = velocity_high;
                    draggedZone->setVelocity(velocity);
                    
                    MainContentComponent::_static_sampler->addSample(draggedZone->getName(), 
                                      draggedZone->getNote(), 
                                      round(draggedZone->getX()/gridWidth), 
                                      round(draggedZone->getX()/gridWidth) + draggedZone->get_width(),
                                      groups_for_zone,
                                      draggedZone->getPlaySettings(),
                                      draggedZone->getVelocity(),
									  draggedZone->getRRGroup()->getProcessor());
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
                            MainContentComponent::_static_sampler->getSynth()->removeSound(zone_index);
                            zones.add(i);
                            
                            Array<int> groups_for_zone;
                            SparseSet<int> s = getGroupEditor()->getListBox()->getSelectedRows();
                            for (int j=0; j<s.size(); j++){
                                if (groups[s[j]]->getZones()->contains(i)){
                                    groups_for_zone.add(s[j]);
                                }
                            }
                            
                            float px_per_velocity = getHeight() / 128;
                            int velocity_low = (getHeight() - (i->getHeight()+i->getY())) / px_per_velocity;
                            int velocity_high = (getHeight() - i->getY()) / px_per_velocity;
                            std::pair<int, int> velocity;
                            velocity.first = velocity_low;
                            velocity.second = velocity_high;
                            i->setVelocity(velocity);
                            
                            MainContentComponent::_static_sampler->addSample(i->getName(), 
                                              i->getNote(), 
                                              round(i->getX()/gridWidth), 
                                              round(i->getX()/gridWidth) + i->get_width(),
                                              groups_for_zone,
                                              i->getPlaySettings(),
                                              i->getVelocity(),
											  i->getRRGroup()->getProcessor());
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
                    MainContentComponent::_static_sampler->getSynth()->removeSound(zone_index);
                    zones.add(draggedZone);
                    
                    Array<int> groups_for_zone;
                    SparseSet<int> s = getGroupEditor()->getListBox()->getSelectedRows();
                    for (int i=0; i<s.size(); i++){
                        if (groups[s[i]]->getZones()->contains(draggedZone)){
                            groups_for_zone.add(s[i]);
                        }
                    }
                    
                    float px_per_velocity = getHeight() / 128;
                    int velocity_low = (getHeight() - (draggedZone->getHeight()+draggedZone->getY())) / px_per_velocity;
                    int velocity_high = (getHeight() - draggedZone->getY()) / px_per_velocity;
                    std::pair<int, int> velocity;
                    velocity.first = velocity_low;
                    velocity.second = velocity_high;
                    draggedZone->setVelocity(velocity);
                    
                    MainContentComponent::_static_sampler->addSample(draggedZone->getName(), 
                                      draggedZone->getNote(), 
                                      round(draggedZone->getX()/gridWidth), 
                                      round(draggedZone->getX()/gridWidth) + draggedZone->get_width(),
                                      groups_for_zone,
                                      draggedZone->getPlaySettings(),
                                      draggedZone->getVelocity(),
									  draggedZone->getRRGroup()->getProcessor());
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
    
    if (!zoneDown)
        zoneInfoSet.deselectAll();
        
    zoneDown = false;
    auto set2 = lassoSource.getLassoSelection();
    for (auto i : set2){
        zoneInfoSet.addToSelection(i);
    }
}


Zone::Zone(MappingEditorGraph* p, const String& sampleName, InstrumentComponent& i) 
	: TextButton{ "" }, parent{ p }, instrument(i), playSettings(), rrGroup(new RoundRobinGroup()),
    name(sampleName)  {
    setAlpha(0.5f);
    velocity.first = 0;
    velocity.second = 127;
    playSettings.setSampleStart(0.0);
}
    

void Zone::mouseDown(const MouseEvent& ){
    parent->draggedZone = this;
    parent->getZoneInfoSet().deselectAll();
    parent->getZoneInfoSet().selectOnly(this);
    parent->setZoneDown(true);
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

void Zone::mouseDoubleClick(const MouseEvent& ){}

void ProgressWindow::run(){
	double progress(0.0);
	for (int i = 0; i<zones.size(); i++){
		if (threadShouldExit())
			break;
		Zone* z = zones[i];

		Array<int> groups_for_zone;
		groups_for_zone.add(z->getGroup());


		sampler->addSample(z->getName(), z->getNote(), round(z->getX() / gridWidth),
			round(z->getX() / gridWidth) + z->get_width(), groups_for_zone,
			z->getPlaySettings(), z->getVelocity(), z->getRRGroup()->getProcessor());
		progress = i / (double)zones.size();
		setProgress(progress);
	}
}
