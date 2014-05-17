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
//#include "/home/patrick/Juce/JUCE/modules/juce_audio_utils/gui/juce_MidiKeyboardComponent.h"

class instrumentMappingEditor : public Viewport
{
public:
    class mappingEditorGraph;
    
    instrumentMappingEditor(const String& componentName, Component* Parent);
    ~instrumentMappingEditor();
private:
    ScopedPointer<Component> parent; //cool
    ScopedPointer<mappingEditorGraph> graph;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (instrumentMappingEditor)
};
    
class instrumentMappingEditor::mappingEditorGraph : 
public Component, public FileDragAndDropTarget, public ButtonListener, public MidiKeyboardStateListener
{
public:
    float width;
    float height;
    float keyboard_height;
    int num_columns;
    
    bool dragging;
    int start_drag_y;
    
    MidiKeyboardComponent* keyboard;
    MidiKeyboardState* keyboard_state;
    
    SelectedItemSet<int> notes_held;
    
    class Zone : public TextButton
    {
    public:
        const String name;
        int x;
        int y;
        int height;
        std::pair<int, int> velocity;
        
        instrumentMappingEditor::mappingEditorGraph* parent;
    
        void register_parent(instrumentMappingEditor::mappingEditorGraph* parent);
        void mouseDown(const MouseEvent& event);
        void mouseMove(const MouseEvent& event);

        Zone(const String& sample_name) : TextButton(""), name(sample_name){
            setAlpha(0.5f);
            velocity.first = 0;
            velocity.second = 127;
        };
       
    
    private:
    };
    
    void set_bounds_for_component(Zone* z, MouseCursor cursor, float grid_outline, float grid_width, int grid_x_offset);
    
    mappingEditorGraph();
    ~mappingEditorGraph();
    
    void paint(Graphics& g);
    void resized();
    void buttonClicked(Button* button);
    
    void handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity);
    void handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber);
    
    void mouseDrag(const MouseEvent& event);
    void mouseUp(const MouseEvent& event);
    void mouseDown(const MouseEvent& event);
    
    void filesDropped(const StringArray& files, int x, int y);
    bool isInterestedInFileDrag(const StringArray& files){return true;};
    
    Array<Zone*> zones;
    ScopedPointer<LassoComponent<Zone*> > lasso;
    
    template <class SelectableItemType>
    class Lasso_Source : public LassoSource<SelectableItemType>, public ChangeListener
    {
    public:
        ~Lasso_Source() {set = nullptr;};
        Array<Zone*> zones_;
        instrumentMappingEditor::mappingEditorGraph* parent;
        SelectedItemSet<SelectableItemType>* set;
        bool dragging;
        
        void findLassoItemsInArea (Array <SelectableItemType>& itemsFound,
                                       const Rectangle<int>& area){
            dragging = true;
            int left  = area.getX();
            int right = area.getRight();
            
            for (int i=0; i<zones_.size(); i++){
                if ((zones_[i]->x >= left && zones_[i]->x <= right) || 
                    (zones_[i]->x + parent->width / parent->num_columns >= left 
                    && zones_[i]->x + parent->width / parent->num_columns <= right)){
                    itemsFound.add(zones_[i]);
                }else{
                    itemsFound.removeFirstMatchingValue(zones_[i]);
                    zones_[i]->setToggleState(false, 1);
                }
            }
        };
        SelectedItemSet<SelectableItemType>& getLassoSelection(){
            set->addChangeListener(this);
            return *set;
        };
        
        void changeListenerCallback(ChangeBroadcaster* source){
            if (source = set){
                for (Zone** i= (((SelectedItemSet<SelectableItemType>*)source)->begin()); i<(((SelectedItemSet<SelectableItemType>*)source)->end()); i++){
                    (*i)->setToggleState(true, 1);
                }
            }
        };
    };

    
    Lasso_Source<Zone*>* lasso_source;
private:
    Zone* dragged_zone;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (mappingEditorGraph)
};





#endif  // INSTRUMENTMAPPINGEDITOR_H_INCLUDED
