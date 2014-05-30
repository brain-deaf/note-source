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

class InstrumentComponent;

class InstrumentMappingEditor : public Viewport
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
            void setX(int x){setTopLeftPosition(x,getY());}
            void setY(int y){setTopLeftPosition(getX(),y);}
            void setHeight(int h){setSize(getWidth(),h);}
            void mouseDown(const MouseEvent& event);
            void mouseMove(const MouseEvent& event);
            void mouseDoubleClick(const MouseEvent& event);
            typedef ReferenceCountedObjectPtr<Zone> Ptr;
        private:
            MappingEditorGraph * parent;
            InstrumentComponent& instrument;
            std::shared_ptr<FilePlayer> filePlayer;

            const String name;
            std::pair<int, int> velocity;
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Zone)
        };

        template <class SelectableItemType>
        class MappingLasso: public LassoSource<SelectableItemType>, public ChangeListener
        {
            Array<Zone::Ptr> zones;
            MappingEditorGraph* parent;
            SelectedItemSet<SelectableItemType> set;
            bool dragging;
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MappingLasso<SelectableItemType>)
        public:
            MappingLasso(MappingEditorGraph * p) : parent{p}, dragging{false} {}
            bool isDragging(){return dragging;}
            Array<Zone::Ptr>& getZones(){return zones;}
            void findLassoItemsInArea (Array <SelectableItemType>& itemsFound,
                const Rectangle<int>& area)
            {
                dragging = true;

                for (auto i : zones){
                    Rectangle<int> r{i->getX(),i->getY(),
                        roundToInt(parent->getWidth()),
                        roundToInt(parent->getHeight())};
                    if (area.intersects(r)){
                        itemsFound.add(i);
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
                    for (auto i : set)
                    {
                        i->setToggleState(true, sendNotification);
                    }
                }
            }
        };

        MappingEditorGraph(float,float,float,int,InstrumentComponent&);
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
        bool isInterestedInFileDrag(const StringArray& files){return true;}

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
        LassoComponent<Zone::Ptr> lasso;
        MappingLasso<Zone::Ptr> lassoSource;
        Zone::Ptr draggedZone;
        SelectedItemSet<Zone::Ptr> zoneInfoSet;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MappingEditorGraph)
    };
    InstrumentMappingEditor(const String& componentName,
        InstrumentComponent& i);
    ScopedPointer<MappingEditorGraph> graph;
private:
    InstrumentComponent& instrument;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentMappingEditor)
};

#endif  // INSTRUMENTMAPPINGEDITOR_H_INCLUDED

