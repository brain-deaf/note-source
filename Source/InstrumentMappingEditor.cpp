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

class BadFormatException : public std::runtime_error{
public:
    BadFormatException(String s) : std::runtime_error(s.toStdString()){}
};

InstrumentMappingEditor::InstrumentMappingEditor(const String& componentName, InstrumentComponent& i)
:   Viewport{componentName},graph{new MappingEditorGraph(1800.0f, 315.0f, 100.0f, 128, i)},
    instrument(i)
{
    setViewedComponent(graph);
}


typedef InstrumentMappingEditor::MappingEditorGraph MappingEditorGraph;

MappingEditorGraph::MappingEditorGraph(float w, float h,
    float kh, int nc, InstrumentComponent& i)
: Component(), width(w), height(h), keyboardHeight(kh),
    numColumns(nc), draggedZone(nullptr), dragging(false), 
    lasso(),lassoSource(this), instrument(i), midiCallback(this), 
    keyboardState(), 
    keyboard(keyboardState, MidiKeyboardComponent::horizontalKeyboard) 
{
    keyboardState.addListener(this);
    addAndMakeVisible(&keyboard);
    SharedResourcePointer<AudioDeviceManager> dm;
    dm->addMidiInputCallback("",&midiCallback);
    setBounds(0, 0, getWidth(), getHeight() + getKeyboardHeight());
    notesHeld.addChangeListener(this);
}

void MappingEditorGraph::buttonClicked(Button *){}
void MappingEditorGraph::MidiDeviceCallback::handleIncomingMidiMessage
    (MidiInput* source, const MidiMessage& message) 
{
    if (message.isNoteOn()) 
    {
        parent->getNotesHeld().addToSelection(message.getNoteNumber());
    }
    if (message.isNoteOff()) 
    {
        if (parent->getNotesHeld().isSelected(message.getNoteNumber()))
        {
            parent->getNotesHeld().deselect(message.getNoteNumber());
        }
    }
}

void MappingEditorGraph::resized()
{
    keyboard.setBounds(
        0, getHeight(), getWidth(), getKeyboardHeight());
    keyboard.setKeyWidth(getWidth() / numColumns * 1.715);
}

void MappingEditorGraph::handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity){
    notesHeld.addToSelection(midiNoteNumber);
}

void MappingEditorGraph::handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber){
    if (notesHeld.isSelected(midiNoteNumber)){
        notesHeld.deselect(midiNoteNumber);
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
        g.fillRect(Rectangle<int>(i*gridWidth, 0, gridWidth, height));
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

        newZone->removeListener(this);
        newZone->addListener(this);
        newZone->removeMouseListener(newZone);
        newZone->addMouseListener(this, true);

        newZone->setX(roundToInt(x / gridWidth) * gridWidth +
            gridOutline + gridWidth*i);
        newZone->setY(0);
        newZone->setHeight(getHeight());
        newZone->setBounds(newZone->getX(), newZone->getY(), 
            gridWidth - gridOutline, newZone->getHeight());
        addAndMakeVisible(newZone);
        lassoSource.getZones().add(newZone);
        
    }
    getZoneInfoSet().selectOnly(newZone);
}
void MappingEditorGraph::mouseDown(const MouseEvent& e) {
    addAndMakeVisible(lasso);
    lasso.beginLasso(e, &lassoSource);
    startDragY = getMouseXYRelative().getY();
    
}

void MappingEditorGraph::setBoundsForComponent(Zone& c, MouseCursor cursor,
    float gridOutline, float gridWidth, int gridXOffset){
    int y = getMouseXYRelative().getY();
    int delY = y - startDragY;
    if (cursor == MouseCursor::NormalCursor){
        int gridX = roundToInt((c.getX()) / gridWidth);
        Range<int> r(0,numColumns-1);
        int newGridX = r.clipValue(gridXOffset + gridX);
        c.setX(newGridX * gridWidth + gridOutline);
         
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
        c.setBounds(c.getX(), newY, gridWidth - gridOutline, c.getHeight());
        c.getVelocity().first = (int)((getHeight() - (newY + c.getHeight())) / (getHeight() / 128));
        c.getVelocity().second = (int)((getHeight() - newY) / (getHeight() / 128));
    }

    else if (cursor == MouseCursor::TopEdgeResizeCursor) {
        int newY = c.getY() + delY;
        int newHeight = c.getHeight() - delY;

        if (newY < 0){
            newY = 0;
            newHeight = c.getHeight() + c.getY();
        }
        c.setBounds(c.getX(), newY, gridWidth - gridOutline, newHeight);
        c.getVelocity().second = (int)((getHeight() - newY) / (getHeight() / 128));
    }
    else if (cursor == MouseCursor::BottomEdgeResizeCursor){
        int y = getMouseXYRelative().getY();
        int newHeight = c.getHeight() + delY;
        if (newHeight + c.getY() > height){
            newHeight = height - (c.getY() + c.getHeight()) + c.getHeight();
        }
        c.setBounds(c.getX(), c.getY(), 
            gridWidth - gridOutline, newHeight);
        c.getVelocity().first = (int)((getHeight() - (c.getY() + newHeight)) / (getHeight() / 128));
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
    auto set = lassoSource.getLassoSelection();
    if (draggedZone != nullptr){
        auto cursor = draggedZone->getMouseCursor();
        if (cursor == MouseCursor::NormalCursor){
            if (lassoSource.getLassoSelection().getItemArray().contains(draggedZone)){
                for (auto i : lassoSource.getLassoSelection()){
                    int newY = getMouseXYRelative().getY() - startDragY;
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
                draggedZone->setY(newY);
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
    : TextButton{""}, parent{p}, instrument(i),
    filePlayer(std::make_shared<FilePlayer>(sampleName)),
    name(sampleName)  {
    instrument.addFilePlayer(filePlayer);
    setAlpha(0.5f);
    velocity.first = 0;
    velocity.second = 127;
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
        setMouseCursor(MouseCursor());
    }
}

void Zone::mouseDoubleClick(const MouseEvent& e){
    filePlayer->toggleState();
}
