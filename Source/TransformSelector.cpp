/*
  ==============================================================================

    TransformSelector.cpp
    Created: 11 Aug 2014 1:16:31pm
    Author:  patrick

  ==============================================================================
*/

#include "TransformSelector.h"
#include "TransformBin.h"
#include "Sampler.h"

TransformSelector::TransformSelector(int rows, int columns) : Component(),
    num_rows(rows), num_columns(columns), groups(), group_editor(nullptr)
{
    groups.add(new TransformGroup());
    for (int i=0; i<num_columns*rows; i++){
        fx_boxes.add(new TransformBox(new TransformButton(this), this));
        fx_boxes[i]->getButton()->setButtonText("");
        groups[0]->group_fx.add(new Transform());
    }
    for (auto box : fx_boxes){
        addAndMakeVisible(box);
    }
    chooser = new TransformChooser(2, 4, this);
    
    
}

TransformSelector::~TransformSelector(){
    for (auto box : fx_boxes){
        delete box;
    }
    for (auto group : groups){
        delete group;
    }
    delete chooser;
}

void TransformSelector::loadPatch(XmlElement* xml){
    registerGroupEditor();
    for (auto group : groups){
        delete group;
    }
    groups.clear();
    
    forEachXmlChildElement(*xml, e){
        if (e->hasTagName("GROUP")){
            groups.add(new TransformGroup());
            forEachXmlChildElement(*e, tf_element){
                if (tf_element->hasTagName("TRANSFORM")){
                    Transform* tf = new Transform();
                    tf->getFxType() = tf_element->getIntAttribute("type");
                
                    switch (tf->getFxType()){
                        case (TransformChooser::FX::LINEAR):{
                            TransformBin* t = static_cast<TransformBin*>(getParentComponent());
                            LinearTransform* linear = new LinearTransform(t);
                            linear->getStartSlider()->setValue(tf_element->getDoubleAttribute("start"), dontSendNotification);
                            linear->getEndSlider()->setValue(tf_element->getDoubleAttribute("end"), dontSendNotification);
                            linear->getSourceBox()->setSelectedId(tf_element->getIntAttribute("source"), dontSendNotification);
                            linear->getTargetBox()->setSelectedId(tf_element->getIntAttribute("target"), dontSendNotification);
                            forEachXmlChildElement(*tf_element, point_element){
                                if (point_element->hasTagName("POINT")){
                                    linear->addPoint(Point<int>(point_element->getIntAttribute("x"), point_element->getIntAttribute("y")));
                                }
                            }
                            tf->setContent(linear);
                            TransformBin* bin = static_cast<TransformBin*>(getParentComponent());
                            bin->getTransformComponent()->loadFx(TransformChooser::FX::LINEAR, linear);
                            break;
                        }
                        case (TransformChooser::FX::EXPONENTIAL):{
                            ExponentialTransform* exponential = new ExponentialTransform();
                            exponential->getStartSlider()->setValue(tf_element->getDoubleAttribute("start"), dontSendNotification);
                            exponential->getEndSlider()->setValue(tf_element->getDoubleAttribute("end"), dontSendNotification);
                            exponential->getSourceBox()->setSelectedId(tf_element->getIntAttribute("source"), dontSendNotification);
                            exponential->getTargetBox()->setSelectedId(tf_element->getIntAttribute("target"), dontSendNotification);
                            forEachXmlChildElement(*tf_element, point_element){
                                if (point_element->hasTagName("POINT")){
                                    exponential->addPoint(Point<int>(point_element->getIntAttribute("x"), point_element->getIntAttribute("y")));
                                    exponential->getGraph()->addCurve(point_element->getDoubleAttribute("curve"));
                                }
                            }
                            tf->setContent(exponential);
                            TransformBin* bin = static_cast<TransformBin*>(getParentComponent());
                            bin->getTransformComponent()->loadFx(TransformChooser::FX::EXPONENTIAL, exponential);
                            break;
                        }
                        case (TransformChooser::FX::NONE):
                            tf->setContent(nullptr);
                            break;
                    }
                    groups[groups.size()-1]->group_fx.add(tf);
                }
            }
        }        
    }
    updateFx();
}

void TransformSelector::registerGroupEditor(){
    if (group_editor == nullptr){
        TransformBin* parent = (TransformBin*)getParentComponent();
        group_editor = parent->getGroupEditor();
        group_editor->getCreateGroupButton()->addListener(this);
        group_editor->getDeleteGroupButton()->addListener(this);
    }
}

void TransformSelector::resized(){
    registerGroupEditor();
    int box_width = getWidth() / num_columns;
    int box_height = getHeight() / num_rows;
    for (int row=0; row<num_rows; row++){
        for (int i=0; i<num_columns; i++){
            fx_boxes[i+row*num_columns]->setBounds(i*box_width, box_height*row, box_width, box_height);
        }
    }
}

void TransformSelector::paint(Graphics& g){
    
    if (isDragAndDropActive()){
    }else{
    }
}

void TransformSelector::buttonClicked(Button* b){
    registerGroupEditor();
    
    if (b == group_editor->getCreateGroupButton()){
        groups.add(new TransformGroup());
        for (int i=0; i<num_columns*num_rows; i++){
            groups[groups.size()-1]->group_fx.add(new Transform());
        }
    }
    else if (b == group_editor->getDeleteGroupButton()){
        SparseSet<int> s = group_editor->getListBox()->getSelectedRows();
        for (int i=s.size()-1; i>=0; i--){
            groups.remove(s[i]);
        }
    }
}

void TransformSelector::updateButtonText(String buttonText){
    for (auto box : fx_boxes){
        if (box->getButton() == fxButtonChoice){
            box->getButton()->setButtonText(buttonText);
        }
    }
}

void TransformSelector::updateFx(){
    Array<int> used_index;
    SparseSet<int> s = group_editor->getListBox()->getSelectedRows();
    for (int i=0; i<s.size(); i++){
        for (int j=0; j<groups[s[i]]->group_fx.size(); j++){
            Transform* fx = groups[s[i]]->group_fx[j];
            if (fx->getFxType() == TransformChooser::FX::LINEAR){
                fx_boxes[j]->getButton()->setButtonText("LINEAR");
                used_index.add(j);
            }
            else if (fx->getFxType() == TransformChooser::FX::EXPONENTIAL){
                fx_boxes[j]->getButton()->setButtonText("EXPONENTIAL");
            }
            else{
                if (used_index.indexOf(j) == -1) fx_boxes[j]->getButton()->setButtonText("");
            }
        }
    }
}


TransformButton::TransformButton(TransformSelector* _parent) : 
    TextButton("fx"), parent(_parent), fx(TransformChooser::FX::NONE), content(nullptr){}

void TransformBox::resized(){
    _button->setSize(getWidth(), getHeight());
}

void TransformButton::mouseDrag(const MouseEvent& e){
    DragAndDropContainer::findParentDragContainerFor(this)->startDragging("fx", this);
    parent->setDragObject(this);
}

void TransformButton::mouseDown(const MouseEvent& e){
    ModifierKeys keys = e.mods;
    parent->fxButtonChoice = this;
    
    if (keys.isRightButtonDown()){
        parent->addAndMakeVisible(parent->getChooser());
        int x_margin = 50;
        int y_margin = 10;
        parent->getChooser()->setBounds(x_margin, y_margin, parent->getWidth() - x_margin*2, parent->getHeight() - y_margin*2);
    }else{
        typedef TransformChooser::FX FX;
        TransformBox* box = (TransformBox*)getParentComponent();
        TransformSelector* selector = box->getParent();
        TransformBin* bin = (TransformBin*)selector->getParentComponent();
        
        SparseSet<int> s = selector->getGroupEditor()->getListBox()->getSelectedRows();
        
        for (int i=0; i<s.size(); i++){
            Transform* fx = selector->getGroups()[s[i]]->group_fx[selector->getBoxes().indexOf(box)];
            
            switch (fx->getFxType()){
            case FX::LINEAR:{
                if (fx->getFxType() == FX::LINEAR){
                    bin->getTransformComponent()->loadFx(FX::LINEAR, fx->getContent());
                    return;
                }
            }
            case FX::EXPONENTIAL:{
                if (fx->getFxType() == FX::EXPONENTIAL){
                    bin->getTransformComponent()->loadFx(FX::EXPONENTIAL, fx->getContent());
                    return;
                }
            }
            
            case FX::NONE:{
                bin->getTransformComponent()->loadFx(FX::NONE, nullptr);
                return;}
            }
        }
        bin->getTransformComponent()->loadFx(FX::NONE, nullptr);
    }
}

TransformBox::TransformBox(TransformButton* button, TransformSelector* _parent): 
    _button(button), parent(_parent), item_entered(false)
{
    addAndMakeVisible(_button);
}

void TransformBox::itemDropped(const SourceDetails& dragSourceDetails){
    String details = dragSourceDetails.description.toString();
    if (details == "file name") return;
    
    Array<TransformBox*> boxes = parent->getBoxes();
    
    TransformBox* dragged_box;
    int dragged_index;
    for (auto box : boxes){
        if (box->getButton() == parent->getDragObject()){
            dragged_box = box;
            dragged_index = boxes.indexOf(box);
            boxes.removeFirstMatchingValue(dragged_box);
            break;
        }
    }
    
    int new_index = boxes.indexOf(this);
    SparseSet<int> s = parent->getGroupEditor()->getListBox()->getSelectedRows();
    
    if (new_index == -1){
        boxes.insert(dragged_index, dragged_box);
    }
    else if (new_index < dragged_index){
        boxes.insert(new_index, dragged_box);
        for (int i=0; i<s.size(); i++){
            parent->getGroups()[s[i]]->group_fx.insert(new_index, parent->getGroups()[s[i]]->group_fx[dragged_index]);
            parent->getGroups()[s[i]]->group_fx.remove(dragged_index+1);
        }
    }
    else{
        boxes.insert(new_index+1, dragged_box);
        for (int i=0; i<s.size(); i++){
            parent->getGroups()[s[i]]->group_fx.insert(new_index+2, parent->getGroups()[s[i]]->group_fx[dragged_index]);
            parent->getGroups()[s[i]]->group_fx.remove(dragged_index);
        }
    }

    for (auto box : boxes){
        box->setEntered(false);
        box->repaint();
    }
    parent->setBoxes(boxes);
    parent->resized();
}

void TransformBox::itemDragEnter(const SourceDetails& dragSourceDetails){
    item_entered = true;
    repaint();
}

void TransformBox::itemDragExit(const SourceDetails& dragSourceDetails){
    item_entered = false;
    repaint();
}

void TransformBox::paint(Graphics& g){
    if (DragAndDropContainer::findParentDragContainerFor(this)->isDragAndDropActive()){
        if (item_entered){
            g.fillAll(Colours::blue);
        }
        else if (!item_entered){
            g.fillAll(Colours::grey);
        }
    }else{
        g.fillAll(Colours::grey);
    }
}

void TransformChoiceButton::mouseDown(const MouseEvent& e){
    ((TransformSelector*)((TransformChooser*)getParentComponent())->getParentComponent())->updateButtonText(getButtonText());
    getParentComponent()->setVisible(false);
    ((TransformChooser*)getParentComponent())->callButtonClick(this);
}

void TransformChoiceButton::mouseEnter(const MouseEvent& e){
    ((TransformChooser*)(getParentComponent()))->focusButton(this);
}

void TransformChoiceButton::mouseExit(const MouseEvent& e){
    ((TransformChooser*)(getParentComponent()))->unfocusButton(this);
}

TransformChooser::TransformChooser(int rows, int columns, TransformSelector * f) : Component(), numRows(rows), numColumns(columns), parent(f)
{
    for (int i=0; i<numRows; i++){
        for (int j=0; j<numColumns; j++){
            buttons.add(new TransformChoiceButton());
        }
    }
        
    for (int i=0; i<numRows; i++){
        for (int j=0; j<numColumns; j++){
            addAndMakeVisible(buttons[i*numColumns + j]);
        }
    }
    buttons[0]->setButtonText("LINEAR");
    buttons[1]->setButtonText("EXPONENTIAL");
}

TransformChooser::~TransformChooser(){
    for (auto i : buttons){
        delete i;
    }
}

void TransformChooser::paint(Graphics& g){
    g.setColour(Colours::black);
    g.setOpacity(0.9f);
    g.fillRoundedRectangle(0.0, 0.0, getWidth(), getHeight(), 30.0);
}

void TransformChooser::resized(){
    float button_width = getWidth() / numColumns;
    float button_height = getHeight() / numRows;
    for (int i=0; i<numRows; i++){
        for (int j=0; j<numColumns; j++){
            buttons[i*numColumns+j]->setBounds(button_width*j + button_width/4, button_height*i + button_height/4, button_width/2, button_height/2);
        }
    }
}

void TransformChooser::callButtonClick(TransformChoiceButton* b){
    TransformSelector* selector = (TransformSelector*)getParentComponent();
    TransformBin* bin = ((TransformBin*)(selector->getParentComponent()));
    SparseSet<int> s = selector->getGroupEditor()->getListBox()->getSelectedRows();
    if (buttons.indexOf(b) == LINEAR){
        int fx_index = selector->getBoxes().indexOf((TransformBox*)(selector->fxButtonChoice->getParentComponent()));
        LinearTransform* new_linear = new LinearTransform(bin);
        for (int i=s.size()-1; i>=0; i--){
            selector->getGroups()[s[i]]->group_fx[fx_index]->getFxType() = LINEAR;
            selector->getGroups()[s[i]]->group_fx[fx_index]->setContent(static_cast<Component*>(new_linear));
            
        }
        selector->fxButtonChoice->setFx(LINEAR);
        bin->getTransformComponent()->loadFx(LINEAR, new_linear);
    }
    else if (buttons.indexOf(b) == EXPONENTIAL){
        int fx_index = selector->getBoxes().indexOf((TransformBox*)(selector->fxButtonChoice->getParentComponent()));
        ExponentialTransform* new_exponential= new ExponentialTransform();
        for (int i=s.size()-1; i>=0; i--){
            selector->getGroups()[s[i]]->group_fx[fx_index]->getFxType() = EXPONENTIAL;
            selector->getGroups()[s[i]]->group_fx[fx_index]->setContent(static_cast<Component*>(new_exponential));
            
        }
        selector->fxButtonChoice->setFx(EXPONENTIAL);
        bin->getTransformComponent()->loadFx(EXPONENTIAL, new_exponential);
    }
    else{ 
            
        int fx_index = selector->getBoxes().indexOf((TransformBox*)(selector->fxButtonChoice->getParentComponent()));
        for (int i=s.size()-1; i>=0; i--){
            selector->getGroups()[s[i]]->group_fx[fx_index]->getFxType() = NONE;
            selector->getGroups()[s[i]]->group_fx[fx_index]->setContent(nullptr);
        }
            
        selector->fxButtonChoice->setFx(NONE);
        bin->getTransformComponent()->loadFx(NONE, nullptr);
    }
}

void TransformChooser::focusButton(TransformChoiceButton* button){
    float button_width = getWidth() / numColumns;
    float button_height = getHeight() / numRows;
    int x_offset = 20;
    int y_offset = 7;
    for (int i=0; i<numRows; i++){
        for (int j=0; j<numColumns; j++){
            if (buttons[i*numColumns+j] == button){
                button->setBounds(button_width*j+x_offset, button_height*i+y_offset, button_width-x_offset*2, button_height-y_offset*2);
            }
        }
    }
}

void TransformChooser::unfocusButton(TransformChoiceButton* button){
    float button_width = getWidth() / numColumns;
    float button_height = getHeight() / numRows;
    for (int i=0; i<numRows; i++){
        for (int j=0; j<numColumns; j++){
            if (buttons[i*numColumns+j] == button){
                button->setBounds(button_width*j + button_width/4, button_height*i + button_height/4, button_width/2, button_height/2);
            }
        }
    }
}