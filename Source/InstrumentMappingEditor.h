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
#include "FilePlayer.h"
#include "GroupEditor.h"

class InstrumentComponent;

class InstrumentMappingEditor : public Component
{
public:
    class MappingEditorGraph : public Component,
                               public FileDragAndDropTarget,
                               public ButtonListener,
                               public MidiKeyboardStateListener,
                               public ChangeListener
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

        class Zone : public TextButton,
                     public ReferenceCountedObject
        {
        public:
            Zone(MappingEditorGraph * p, const String& s,
                    InstrumentComponent& i);
            const String& getName(){return name;}
            void setNote(int n){_note = n;}
            void setX(int x){_x = x; setNote((int)(x / (parent->getWidth() / parent->getNumColumns())));}
            void setY(int y){_y = y;}
            int getX(){return _x;}
            int getY(){return _y;}
            std::pair<int, int>& getVelocity(){return velocity;}
            int getNote(){return _note;}
            void setHeight(int h){_height = h;}
            int getHeight(){return _height;}
            void mouseDown(const MouseEvent& event);
            void mouseMove(const MouseEvent& event);
            void mouseDoubleClick(const MouseEvent& event);
            FilePlayer* getFilePlayer(){return filePlayer;}
            typedef ReferenceCountedObjectPtr<Zone> Ptr;
        private:
            MappingEditorGraph * parent;
            InstrumentComponent& instrument;
            FilePlayer* filePlayer;
            int _x;
            int _y;
            int _height;
            int _note;

            const String name;
            std::pair<int, int> velocity;
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Zone)
        };
        
        class Group
        {
            public:
                Group(){
                    name = "New Group"; 
                    zones = new Array<Zone::Ptr>();
                }
                Array<Zone::Ptr>* getZones(){return zones;}
                void setZones(Array<Zone::Ptr>* _zones){zones = _zones;}
                String getName(){return name;}
                void setName(String s){name = s;}
            private:
                Array<Zone::Ptr>* zones;
                String name;
        };
        
        //class GroupViewItem : public TreeViewItem

        template <class SelectableItemType>
        class MappingLasso: public LassoSource<SelectableItemType>, public ChangeListener
        {
            Array<Zone::Ptr> zones;
            MappingEditorGraph* parent;
            SelectedItemSet<SelectableItemType> set;
            bool _dragging;
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MappingLasso<SelectableItemType>)
        public:
            MappingLasso(MappingEditorGraph * p) : parent{p}, _dragging{false} {}
            bool Dragging(){return _dragging;}
            void setDragging(bool d){_dragging = d;}
            Array<Zone::Ptr>& getZones(){return zones;}
            void findLassoItemsInArea (Array <SelectableItemType>& itemsFound,
                const Rectangle<int>& area)
            {
                _dragging = true;

                for (auto i : zones){
                    Rectangle<int> r{i->getBounds()};
                    if (area.intersects(r)){
                        itemsFound.add(i);
                        i->setToggleState(true, sendNotification);
                    } else {
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
                    /*for (auto i : set)
                    {
                        i->setToggleState(true, sendNotification);
                    }*/
                }
            }
        };

        MappingEditorGraph(float,float,float,int,InstrumentComponent&, GroupEditor*);
        void changeListenerCallback(ChangeBroadcaster* source){repaint();};
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
        void updateZones();
        bool isInterestedInFileDrag(const StringArray& files){return true;}
        Array<Zone::Ptr> getZones(){return zones;}
        Array<Group*> getGroups(){return groups;}

        SelectedItemSet<Zone::Ptr>& getZoneInfoSet()
            { return zoneInfoSet;}
        void setWidth(float w){width=w;}
        float getWidth(){ return width;}
        void setHeight(float h){height=h;}
        float getHeight() { return height;}
        void setKeyboardHeight(float kh){keyboardHeight=kh;}
        float getKeyboardHeight(){return keyboardHeight;}
        void setNumColumns(int nc){numColumns=nc;}
        int getNumColumns(){return numColumns;}
        bool isDragging(){return dragging;}
        SelectedItemSet<int>& getNotesHeld() { return notesHeld;}
        MidiDeviceCallback& getMidiCallback() { return midiCallback;}
        GroupEditor* getGroupEditor(){return group_editor;}

    private:
        float width;
        float height;
        float keyboardHeight;
        int numColumns;
        bool dragging;
        int startDragY;
        InstrumentComponent& instrument;
        MidiDeviceCallback midiCallback;
        MidiKeyboardState keyboardState;
        MidiKeyboardComponent keyboard;
        SelectedItemSet<int> notesHeld;
        Array<Zone::Ptr> zones;
        Array<Group*> groups;
        LassoComponent<Zone::Ptr> lasso;
        MappingLasso<Zone::Ptr> lassoSource;
        Zone::Ptr draggedZone;
        SelectedItemSet<Zone::Ptr> zoneInfoSet;
        GroupEditor* group_editor;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MappingEditorGraph)
    };
    InstrumentMappingEditor(const String& componentName,
        InstrumentComponent& i);
    ~InstrumentMappingEditor();
    ScopedPointer<MappingEditorGraph> graph;
    Viewport* view_port;
    GroupEditor* group_editor;
private:
    InstrumentComponent& instrument;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentMappingEditor)
};

#endif  // INSTRUMENTMAPPINGEDITOR_H_INCLUDED

