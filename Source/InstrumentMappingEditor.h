/*
  ==============================================================================

    instrumentMappingEditor.h
    Created: 11 May 2014 11:32:11am
    Author:  patrick

  ==============================================================================
*/

#ifndef INSTRUMENTMAPPINGEDITOR_H_INCLUDED
#define INSTRUMENTMAPPINGEDITOR_H_INCLUDED

#include <memory>
#include "../JuceLibraryCode/JuceHeader.h"

class InstrumentMappingEditor : public Viewport
{
public:
    class MappingEditorGraph;
    InstrumentMappingEditor(const String& componentName, Component* Parent, std::shared_ptr<AudioDeviceManager>& audio);
    ScopedPointer<MappingEditorGraph> graph;
    std::shared_ptr<AudioDeviceManager> audio_manager;
private:
    Component* parent; 
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentMappingEditor)
};
    
class InstrumentMappingEditor::MappingEditorGraph : 
public Component, public FileDragAndDropTarget, public ButtonListener, public MidiKeyboardStateListener, public ChangeListener
{
public:
    class MidiDeviceCallback : public MidiInputCallback
    {
        MappingEditorGraph* parent;
    public:
        MidiDeviceCallback(MappingEditorGraph * p): parent{p}{}
        void handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message);
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiDeviceCallback)
    };

    class Zone : public TextButton, public ChangeListener {
    public:
        Zone(MappingEditorGraph * p,const String& sample_name,std::shared_ptr<AudioDeviceManager>& am);
        ~Zone();
        enum TransportState {
            Stopped,
            Starting,
            Playing,
            Pausing,
            Paused,
            Stopping
        };

        void changeState (TransportState newState);
        virtual void changeListenerCallback (ChangeBroadcaster* source) override; 
        const String& name(){return name_;}
        void x(int xx){x_ = xx;}
        int x() { return x_;}
        void y(int yy){y_ = yy;}
        int y() { return y_;}
        void height(int h){height_=h;}
        int height(){return height_;}
        void mouseDown(const MouseEvent& event);
        void mouseMove(const MouseEvent& event);
        void mouseDoubleClick(const MouseEvent& event);
    private:
        const String name_;
        int x_;
        int y_;
        int height_;
        std::pair<int, int> velocity;
        MappingEditorGraph* parent;
        std::shared_ptr<AudioDeviceManager> audio_manager;
        ScopedPointer<AudioFormatReaderSource> reader_source;
        AudioFormatManager format_manager;
        AudioTransportSource transport_source;
        AudioSourcePlayer source_player;
        TransportState state;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Zone)
    };

    template <class SelectableItemType>
    class MappingLasso: public LassoSource<SelectableItemType>, public ChangeListener
    {
        Array<Zone*> zones_;
        MappingEditorGraph* parent;
        SelectedItemSet<SelectableItemType> set; 
        bool dragging_;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MappingLasso<SelectableItemType>)
    public:
        MappingLasso(MappingEditorGraph * p) : parent{p}, dragging_{false} {}
        bool dragging(){return dragging_;}
        void dragging(bool d) {dragging_=d;}
        Array<Zone*>& zones(){return zones_;}
        void findLassoItemsInArea (Array <SelectableItemType>& itemsFound,
                                       const Rectangle<int>& area){
            dragging(true);
            int left  = area.getX();
            int right = area.getRight();
            int top   = area.getY();
            int bottom= area.getBottom();
            
            ModifierKeys modifier_keys(0);
            
            for (auto i : zones_){
                if (((i->x() >= left && i->x() <= right) || 
                    (i->x() + parent->width() / parent->num_columns() >= left && 
                     i->x() + parent->width() / parent->num_columns() <= right)) && 
                   ((i->y() >= top && i->y() <= bottom) || ((top >= i->y() &&
                     top <= i->y() + i->height())))){
                    itemsFound.add(i);
                } else {
                    itemsFound.removeFirstMatchingValue(i);
                    i->setToggleState(false, sendNotification);
                }
            }
        };
        SelectedItemSet<SelectableItemType>& getLassoSelection(){
            return set;
        };
        
        void changeListenerCallback(ChangeBroadcaster* source){
            if (source == &set){
                for (auto i : set){
                    i->setToggleState(true, sendNotification);
                }
            }
        }
    };

    MappingEditorGraph(float,float,float,int);
    ~MappingEditorGraph();
    void changeListenerCallback(ChangeBroadcaster* source){repaint();};
    void set_bounds_for_component(Zone* z, MouseCursor cursor, float grid_outline, float grid_width, int grid_x_offset);
    void paint(Graphics& g);
    void resized();
    virtual void buttonClicked(Button* button) override;
    void handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity);
    void handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber);
    void mouseDrag(const MouseEvent& event);
    void mouseUp(const MouseEvent& event);
    void mouseDown(const MouseEvent& event);
    void filesDropped(const StringArray& files, int x, int y);
    bool isInterestedInFileDrag(const StringArray& files){return true;}

    SelectedItemSet<Zone*>* zone_info_set(){ return zone_info_set_;}
    void width(float w){width_=w;}
    float width(){ return width_;}
    void height(float h){height_=h;}
    float height() { return height_;}
    void keyboard_height(float kh){keyboard_height_=kh;}
    float keyboard_height(){return keyboard_height_;}
    void num_columns(int nc){num_columns_=nc;}
    int num_columns(){return num_columns_;}
    bool dragging(){return dragging_;}
    void audio_manager(std::shared_ptr<AudioDeviceManager> am) {audio_manager_ = am;}
    std::shared_ptr<AudioDeviceManager>& audio_manager() { return audio_manager_;}
    SelectedItemSet<int>& notes_held() { return notes_held_;}
    MidiDeviceCallback* midi_callback() { return midi_callback_;}

private:
    float width_;
    float height_;
    float keyboard_height_;
    int num_columns_;
    bool dragging_;
    int start_drag_y;
    std::shared_ptr<AudioDeviceManager> audio_manager_;
    MidiDeviceCallback* midi_callback_;
    MidiKeyboardComponent* keyboard;
    MidiKeyboardState* keyboard_state;
    SelectedItemSet<int> notes_held_;
    Array<Zone*> zones;
    ScopedPointer<LassoComponent<Zone*>> lasso;
    MappingLasso<Zone*>* lasso_source;
    Zone* dragged_zone;
    SelectedItemSet<Zone*>* zone_info_set_;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MappingEditorGraph)
};
#endif  // INSTRUMENTMAPPINGEDITOR_H_INCLUDED

