/*
  ==============================================================================

    instrumentMappingEditor.h
    Created: 11 May 2014 11:32:11am
    Author:  patrick

  ==============================================================================
*/

#ifndef INSTRUMENTMAPPINGEDITOR_H_INCLUDED
#define INSTRUMENTMAPPINGEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "GroupEditor.h"
//#include "Sampler.h"
#include "LuaScript.h"
#include "Metronome.h"
#include "PluginProcessor.h"

class Sampler;
class InstrumentComponent;
class ProgressWindow;
class Zone;
class MappingEditorGraph;
class MidiDeviceCallback;
class Group;
class RoundRobinGroup;

class PlaySettings
{
public:
    PlaySettings() : sampleStart(0.0), loopStart(0.0), loopEnd(0.0),
		xfadeLength(150.0), xfadeCurve(0.1), releaseStart(0.0), 
		releaseTime(44100.0 / 8.0), releaseCurve(0.01),
		loopMode(false), releaseMode(false), tuning(0.0){}
    double getSampleStart(){return sampleStart;}
    void setSampleStart(double d){sampleStart=d;}
    void setLoopStart(double d){loopStart=d;}
    void setLoopEnd(double d){loopEnd=d;}
    void setLoopMode(bool b){loopMode=b;}
	void setReleaseMode(bool b){ releaseMode = b; }
    void setXfadeLength(double d){xfadeLength = d;}
	void setXfadeCurve(double d){ xfadeCurve = d; }
	void setReleaseStart(double d){ releaseStart = d; }
	void setReleaseTime(double d){ releaseTime = d; }
	void setReleaseCurve(double d){ releaseCurve = d; }
    bool getLoopMode(){return loopMode;}
	bool getReleaseMode(){ return releaseMode; }
    double getLoopStart(){return loopStart;}
    double getLoopEnd(){return loopEnd;}
    double getXfadeLength(){return xfadeLength;}
	double getXfadeCurve(){ return xfadeCurve; }
    double getTuning(){return tuning;}
	double getReleaseStart(){ return releaseStart; }
	double getReleaseTime(){ return releaseTime; }
	double getReleaseCurve(){ return releaseCurve; }
    void setTuning(double d){tuning=d;}
private:
    double sampleStart;
	double releaseStart;
	double releaseTime;
	double releaseCurve;
    double loopStart;
    double loopEnd;
    bool loopMode;
	bool releaseMode;
    double xfadeLength;
	double xfadeCurve;
    double tuning;
};

class Zone : public TextButton,
	public ReferenceCountedObject
{
public:
	Zone(MappingEditorGraph * p, const String& s,
		InstrumentComponent& i);
	const String& getName(){ return name; }
	void setNote(int n){ _note = n; }
	void setX(int x){ _x = x; /*setNote((int)(x / (parent->getWidth() / parent->getNumColumns())))*/; }
	void setY(int y){ _y = y; }
	int getX(){ return _x; }
	int getY(){ return _y; }
	std::pair<int, int>& getVelocity(){ return velocity; }
	void setVelocity(std::pair<float, float> v){ velocity = v; }
	int getNote(){ return _note; }
	void setHeight(int h){ _height = h; }
	int getHeight(){ return _height; }
	int getRangeLow(){ return range_low; }
	int getRangeHigh(){ return range_high; }
	int get_width(){ return width; }
	void set_width(int w){ width = w; }
	void setRangeLow(int n){ range_low = n; }
	void setRangeHigh(int n){ range_high = n; }
	void mouseDown(const MouseEvent&);
	void mouseMove(const MouseEvent& event);
	void mouseDoubleClick(const MouseEvent& event);
	void setGroup(int i){ group = i; }
	int getGroup(){ return group; }
	RoundRobinGroup* getRRGroup(){ return rrGroup.get(); }
	
	PlaySettings* getPlaySettings(){ return &playSettings; }
private:
	MappingEditorGraph * parent;
	InstrumentComponent& instrument;
	int _x;
	int _y;
	int _height;
	int _note;
	int range_low;
	int range_high;
	int width;
	int group;
	PlaySettings playSettings;
	ScopedPointer<RoundRobinGroup> rrGroup;

	const String name;
	std::pair<int, int> velocity;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Zone)
};

class InstrumentMappingEditor : public Component
{
public:
    
    InstrumentMappingEditor(const String& componentName,
        InstrumentComponent& i);
    ~InstrumentMappingEditor();
    virtual void resized() override;
    void refreshGroupEditor(){addAndMakeVisible(group_editor);resized();}
    ScopedPointer<MappingEditorGraph> graph;
    ScopedPointer<Viewport> view_port;
    ScopedPointer<GroupEditor> group_editor;
    InstrumentComponent& getInstrument(){return instrument;}
    
private:
    InstrumentComponent& instrument;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentMappingEditor)
};

class MidiDeviceCallback : public MidiInputCallback
{
	MappingEditorGraph* parent;
	int midi_input_id;
public:
	MidiDeviceCallback(MappingEditorGraph * p) : luaScript(nullptr),
		parent{ p },
		midi_input_id(-1){}

	void handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message);
	void handleMidiBuffer(MidiBuffer&);
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiDeviceCallback)
		void setMidiChannel(int i){ midi_input_id = i; }
	LuaScript* luaScript;
};


template <class SelectableItemType>
class MappingLasso : public LassoSource<SelectableItemType>, public ChangeListener
{
	Array<ReferenceCountedObjectPtr<Zone> > zones;
	MappingEditorGraph* parent;
	SelectedItemSet<SelectableItemType> set;
	bool _dragging;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MappingLasso<SelectableItemType>)
public:
	MappingLasso(MappingEditorGraph * p) : parent{ p }, _dragging{ false } {}
	bool Dragging(){ return _dragging; }
	void setDragging(bool d){ _dragging = d; }
	Array<ReferenceCountedObjectPtr<Zone> >& getZones(){ return zones; }
	void findLassoItemsInArea(Array <SelectableItemType>& itemsFound,
		const Rectangle<int>& area)
	{
		_dragging = true;

		for (auto i : zones){
			Rectangle<int> r{ i->getBounds() };
			if (area.intersects(r)){
				itemsFound.add(i);
				i->setToggleState(true, sendNotification);
			}
			else {
				itemsFound.removeFirstMatchingValue(i);
				i->setToggleState(false, sendNotification);
			}
		}
	};

	SelectedItemSet<SelectableItemType>& getLassoSelection()
	{
		return set;
	};

	void changeListenerCallback(ChangeBroadcaster* source)
	{
		if (source == &set)
		{
			for (auto z : getZones())
			{
				if (set.isSelected(z))
				{
					z->setToggleState(true, sendNotification);
				}
				else
				{
					z->setToggleState(false, sendNotification);
				}
			}
		}
	}
};


class MappingEditorGraph : public Component,
	public FileDragAndDropTarget,
	public DragAndDropTarget,
	public ButtonListener,
	public MidiKeyboardStateListener,
	public ChangeListener,
	public KeyListener
{
public:
	//class GroupViewItem : public TreeViewItem
	MappingEditorGraph(float, float, float, int, InstrumentComponent&, GroupEditor*);
	~MappingEditorGraph();
	void changeListenerCallback(ChangeBroadcaster*){ repaint(); };
	void setBoundsForComponent(Zone& z, MouseCursor cursor,
		float grid_outline, float gridWidth, int gridXOffset);
	void paint(Graphics& g);
	void resized();
	virtual void buttonClicked(Button* button) override;
	void handleNoteOn(MidiKeyboardState* source, int midiChannel,
		int midiNoteNumber, float velocity);
	void handleNoteOff(MidiKeyboardState* source, int midiChannel,
		int midiNoteNumber);
	void mouseDrag(const MouseEvent& event);
	void mouseUp(const MouseEvent& event);
	void mouseDown(const MouseEvent& event);
	void filesDropped(const StringArray& files, int x, int y);
	bool isInterestedInDragSource(const SourceDetails&){ return true; }
	void itemDropped(const SourceDetails&);
	void updateZones();
	void updateZone(Zone*);
	bool isInterestedInFileDrag(const StringArray&){ return true; }
	void loadPatch(XmlElement* i);
	bool keyPressed(const KeyPress&, Component*);
	Array<ReferenceCountedObjectPtr<Zone> > getZones(){ return zones; }
	Array<ReferenceCountedObjectPtr<Zone> > getCopiedZones(){ return copied_zones; }
	Array<Group*> getGroups(){ return groups; }

	SelectedItemSet<ReferenceCountedObjectPtr<Zone> >& getZoneInfoSet()
	{
		return zoneInfoSet;
	}
	void setWidth(float w){ width = w; }
	float getWidth(){ return width; }
	void setHeight(float h){ height = h; }
	float getHeight() { return height; }
	void setKeyboardHeight(float kh){ keyboardHeight = kh; }
	float getKeyboardHeight(){ return keyboardHeight; }
	void setNumColumns(int nc){ numColumns = nc; }
	int getNumColumns(){ return numColumns; }
	int get_width(){ return (int)width; }
	bool isDragging(){ return dragging; }
	InstrumentComponent& getInstrument(){ return instrument; }
	SelectedItemSet<std::pair<int, int> >& getNotesHeld() { return notesHeld; }
	MidiDeviceCallback& getMidiCallback();
	GroupEditor*& getGroupEditor(){ return group_editor; }
	Metronome& getMetronome(){ return metronome; }
	void setZoneDown(bool b){ zoneDown = b; }
	double* getPatchProgress(){ return &patchProgress; }
	Array<ReferenceCountedObjectPtr<Zone> > zones;
	ReferenceCountedObjectPtr<Zone> draggedZone;
private:
	float width;
	float height;
	float keyboardHeight;
	int numColumns;
	bool dragging;
	int startDragY;
	int startDragX;
	int groupEditorY;
	bool zoneDown;
	Array<Zone*> new_zones;
	ScopedPointer<ProgressWindow> progressWindow;

	InstrumentComponent& instrument;
	MidiDeviceCallback midiCallback;
	MidiKeyboardState keyboardState;
	MidiKeyboardComponent keyboard;

	int zoneCount;
	double patchProgress;

	SelectedItemSet<std::pair<int, int> > notesHeld;
	
	Array<ReferenceCountedObjectPtr<Zone> > copied_zones;
	Array<Group*> groups;
	LassoComponent<ReferenceCountedObjectPtr<Zone> > lasso;
	MappingLasso<ReferenceCountedObjectPtr<Zone> > lassoSource;
	
	SelectedItemSet<ReferenceCountedObjectPtr<Zone> > zoneInfoSet;
	GroupEditor* group_editor;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MappingEditorGraph)

	SamplerEventProcessor* samplerProcessor;
	ScopedPointer<TextButton> renderEventsButton;

	Metronome metronome;
	AudioSourcePlayer source_player;
	AudioSourcePlayer metronome_player;
	LuaScript* luaScript;
};

class Group
{
public:
	Group(){
		name = "New Group";
		zones = new Array<ReferenceCountedObjectPtr<Zone> >();
	}
	Array<ReferenceCountedObjectPtr<Zone> >* getZones(){ return zones; }
	//void setZones(Array<ReferenceCountedObjectPtr<Zone> >* _zones){zones = _zones;}
	String getName(){ return name; }
	void setName(String s){ name = s; }
private:
	Array<ReferenceCountedObjectPtr<Zone> >* zones;
	String name;
};

class ProgressWindow  : public ThreadWithProgressWindow
{
public:
    ProgressWindow(Array<Zone*> a, SamplerProcessor* s, float f)    
        : ThreadWithProgressWindow ("loading...", true, false), zones(a), sampler(s), gridWidth(f)
    {
    }
	void run();
    void threadComplete(bool){
        std::cout<<"thread complete!"<<std::endl;
    }
private:
    Array<Zone*> zones;
    SamplerProcessor* sampler;
    float gridWidth;
};

#endif  // INSTRUMENTMAPPINGEDITOR_H_INCLUDED

