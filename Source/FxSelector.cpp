/*
  ==============================================================================

    FxSelector.cpp
    Created: 6 Jul 2014 2:54:41pm
    Author:  patrick

  ==============================================================================
*/

#include "FxSelector.h"
#include "FxBin.h"
#include "Sampler.h"

FxSelector::FxSelector(int rows, int columns) : Component(),
    num_rows(rows), num_columns(columns), groups(), group_editor(nullptr)
{
    groups.add(new FxGroup());
    for (int i=0; i<num_columns*rows; i++){
        fx_boxes.add(new FxBox(new FxButton(this), this));
        fx_boxes[i]->getButton()->setButtonText("");
        groups[0]->group_fx.add(new Fx());
    }
    for (auto box : fx_boxes){
        addAndMakeVisible(box);
    }
    chooser = new FxChooser(2, 4, this);
    
    
}

FxSelector::~FxSelector(){
    for (auto box : fx_boxes){
        delete box;
    }
    for (auto group : groups){
        delete group;
    }
    delete chooser;
}

void FxSelector::loadPatch(XmlElement* xml){
    registerGroupEditor();
    for (auto group : groups){
        delete group;
    }
    groups.clear();
    
    forEachXmlChildElement(*xml, e){
        if (e->hasTagName("GROUP")){
            groups.add(new FxGroup());
            forEachXmlChildElement(*e, fx_element){
                if (fx_element->hasTagName("FX")){
                    Fx* fx = new Fx();
                    fx->getFxType() = fx_element->getIntAttribute("type");
                
                    switch (fx->getFxType()){
                        case (FxChooser::FX::ADSR):{
                            Adsr* adsr = new Adsr();
                            adsr->getAttackTimeSlider()->setValue(fx_element->getIntAttribute("attack"));
                            adsr->getAttackCurveSlider()->setValue(fx_element->getDoubleAttribute("attack_curve"));
                            adsr->getDecayTimeSlider()->setValue(fx_element->getIntAttribute("decay"));
                            adsr->getDecayCurveSlider()->setValue(fx_element->getDoubleAttribute("decay_curve"));
                            adsr->getSustainSlider()->setValue(fx_element->getIntAttribute("sustain"));
                            adsr->getReleaseTimeSlider()->setValue(fx_element->getIntAttribute("release"));
                            adsr->getReleaseCurveSlider()->setValue(fx_element->getDoubleAttribute("release_curve"));
                            fx->setContent(adsr);
                            break;
                        }
                        case (FxChooser::FX::RINGMOD):{
                            RingModulator* ringmod= new RingModulator();
                            ringmod->getFrequencySlider()->setValue(fx_element->getDoubleAttribute("frequency"));
                            ringmod->getAmplitudeSlider()->setValue(fx_element->getDoubleAttribute("mix"));
                            fx->setContent(ringmod);
                            break;
                        }
                        case (FxChooser::FX::NONE):
                            fx->setContent(nullptr);
                            break;
                    }
                    groups[groups.size()-1]->group_fx.add(fx);
                }
            }
        }        
    }
    updateFx();
}

void FxSelector::registerGroupEditor(){
    if (group_editor == nullptr){
        FxBin* parent = (FxBin*)getParentComponent();
        group_editor = parent->getGroupEditor();
        group_editor->getCreateGroupButton()->addListener(this);
        group_editor->getDeleteGroupButton()->addListener(this);
        group_editor->getGroupDownButton()->addListener(this);
        group_editor->getGroupUpButton()->addListener(this);
    }
}

void FxSelector::resized(){
    registerGroupEditor();
    int box_width = getWidth() / num_columns;
    int box_height = getHeight() / num_rows;
    for (int row=0; row<num_rows; row++){
        for (int i=0; i<num_columns; i++){
            fx_boxes[i+row*num_columns]->setBounds(i*box_width, box_height*row, box_width, box_height);
        }
    }
}

void FxSelector::paint(Graphics&){
    
    if (isDragAndDropActive()){
    }else{
    }
}

void FxSelector::buttonClicked(Button* b){
    registerGroupEditor();
    
    if (b == group_editor->getCreateGroupButton()){
        groups.add(new FxGroup());
        for (int i=0; i<num_columns*num_rows; i++){
            groups[groups.size()-1]->group_fx.add(new Fx());
        }
    }
    else if (b == group_editor->getDeleteGroupButton()){
        SparseSet<int> s = group_editor->getListBox()->getSelectedRows();
        for (int i=s.size()-1; i>=0; i--){
            groups.remove(s[i]);
        }
    }
    else if (b == group_editor->getGroupDownButton()){
        SparseSet<int> s = group_editor->getListBox()->getSelectedRows();
        int size = group_editor->getModel()->getGroupNames().size();
        for (int i=s.size()-1; i>=0; i--){
            int new_index = s[i] + 1 < size ? s[i]+1 : size-1; 
            groups.move(s[i], new_index);
        }
    }
    else if (b == group_editor->getGroupUpButton()){
        SparseSet<int> s = group_editor->getListBox()->getSelectedRows();
        for (int i=0; i<s.size(); i++){
            int new_index = s[i] - 1 >= 0 ? s[i]-1 : 0; 
            groups.move(s[i], new_index);
        }
    }
}

void FxSelector::updateButtonText(String buttonText){
    for (auto box : fx_boxes){
        if (box->getButton() == fxButtonChoice){
            box->getButton()->setButtonText(buttonText);
        }
    }
}

void FxSelector::updateFx(){
    Array<int> used_index;
    SparseSet<int> s = group_editor->getListBox()->getSelectedRows();
    for (int i=0; i<s.size(); i++){
        for (int j=0; j<groups[s[i]]->group_fx.size(); j++){
            Fx* fx = groups[s[i]]->group_fx[j];
            if (fx->getFxType() == FxChooser::FX::ADSR){
                fx_boxes[j]->getButton()->setButtonText("ADSR");
                used_index.add(j);
            }
            else if (fx->getFxType() == FxChooser::FX::FILTER){
                fx_boxes[j]->getButton()->setButtonText("FILTER");
            }
            else if (fx->getFxType() == FxChooser::FX::RINGMOD){
                fx_boxes[j]->getButton()->setButtonText("RING MOD");
            }
            else{
                if (used_index.indexOf(j) == -1) fx_boxes[j]->getButton()->setButtonText("");
            }
        }
    }
}


FxButton::FxButton(FxSelector* _parent) : TextButton("fx"), parent(_parent), fx(FxChooser::FX::NONE), content(nullptr){}

void FxBox::resized(){
    _button->setSize(getWidth(), getHeight());
}

void FxButton::mouseDrag(const MouseEvent& ){
    DragAndDropContainer::findParentDragContainerFor(this)->startDragging("fx", this);
    parent->setDragObject(this);
}

void FxButton::mouseDown(const MouseEvent& e){
    ModifierKeys keys = e.mods;
    parent->fxButtonChoice = this;
    
    if (keys.isRightButtonDown()){
        parent->addAndMakeVisible(parent->getChooser());
        int x_margin = 50;
        int y_margin = 10;
        parent->getChooser()->setBounds(x_margin, y_margin, parent->getWidth() - x_margin*2, parent->getHeight() - y_margin*2);
    }else{
        typedef FxChooser::FX FX;
        FxBox* box = static_cast<FxBox*>(getParentComponent());
        FxSelector* selector = box->getParent();
        FxBin* bin = static_cast<FxBin*>(selector->getParentComponent());
        
        SparseSet<int> s = selector->getGroupEditor()->getListBox()->getSelectedRows();
        
        for (int i=s.size()-1; i>=0; i--){
            Fx* fx = selector->getGroups()[s[i]]->group_fx[selector->getBoxes().indexOf(box)];
            std::cout<<fx<<std::endl;
            std::cout<<selector->getGroups()[s[i]]<<std::endl;
            
            switch (fx->getFxType()){
            case FX::ADSR:{
                if (fx->getFxType() == FX::ADSR){
                    std::cout<<"show adsr"<<std::endl;
                    bin->getFxComponent()->loadFx(FX::ADSR, fx->getContent());
                    std::cout<<"done showing fx"<<std::endl;
                    return;
                }
            }
            case FX::FILTER:{
                if (fx->getFxType() == FX::FILTER){
                    std::cout<<"show filter"<<std::endl;
                    bin->getFxComponent()->loadFx(FX::FILTER, fx->getContent());
                    std::cout<<"done showing fx"<<std::endl;
                    return;
                }
            }
            case FX::RINGMOD:{
                if (fx->getFxType() == FX::RINGMOD){
                    bin->getFxComponent()->loadFx(FX::RINGMOD, fx->getContent());
                    std::cout<<"done showing fx"<<std::endl;
                    return;
                }
            }
            
            case FX::NONE:{
                bin->getFxComponent()->loadFx(FX::NONE, nullptr);
                return;}
            }
        }
        //bin->getFxComponent()->loadFx(FX::NONE, nullptr);
    }
}

FxBox::FxBox(FxButton* button, FxSelector* _parent): _button(button), parent(_parent), item_entered(false){
    addAndMakeVisible(_button);
}

void FxBox::itemDropped(const SourceDetails& dragSourceDetails){
    String details = dragSourceDetails.description.toString();
    if (details == "file name") return;
    
    Array<FxBox*> boxes = parent->getBoxes();
    
    FxBox* dragged_box;
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

void FxBox::itemDragEnter(const SourceDetails& ){
    item_entered = true;
    repaint();
}

void FxBox::itemDragExit(const SourceDetails& ){
    item_entered = false;
    repaint();
}

void FxBox::paint(Graphics& g){
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

void FxChoiceButton::mouseDown(const MouseEvent& ){
    ((FxSelector*)((FxChooser*)getParentComponent())->getParentComponent())->updateButtonText(getButtonText());
    getParentComponent()->setVisible(false);
    ((FxChooser*)getParentComponent())->callButtonClick(this);
}

void FxChoiceButton::mouseEnter(const MouseEvent& ){
    ((FxChooser*)(getParentComponent()))->focusButton(this);
}

void FxChoiceButton::mouseExit(const MouseEvent& ){
    ((FxChooser*)(getParentComponent()))->unfocusButton(this);
}

FxChooser::FxChooser(int rows, int columns, FxSelector * f) : Component(), numRows(rows), numColumns(columns), parent(f)
{
    for (int i=0; i<numRows; i++){
        for (int j=0; j<numColumns; j++){
            buttons.add(new FxChoiceButton());
        }
    }
        
    for (int i=0; i<numRows; i++){
        for (int j=0; j<numColumns; j++){
            addAndMakeVisible(buttons[i*numColumns + j]);
        }
    }
    buttons[0]->setButtonText("ADSR");
    buttons[1]->setButtonText("FILTER");
    buttons[2]->setButtonText("RING MOD");
}

FxChooser::~FxChooser(){
    for (auto i : buttons){
        delete i;
    }
}

void FxChooser::paint(Graphics& g){
    g.setColour(Colours::black);
    g.setOpacity(0.9f);
    g.fillRoundedRectangle(0.0, 0.0, getWidth(), getHeight(), 30.0);
}

void FxChooser::resized(){
    float button_width = getWidth() / numColumns;
    float button_height = getHeight() / numRows;
    for (int i=0; i<numRows; i++){
        for (int j=0; j<numColumns; j++){
            buttons[i*numColumns+j]->setBounds(button_width*j + button_width/4, button_height*i + button_height/4, button_width/2, button_height/2);
        }
    }
}

void FxChooser::callButtonClick(FxChoiceButton* b){
    FxSelector* selector = (FxSelector*)getParentComponent();
    FxBin* bin = ((FxBin*)(selector->getParentComponent()));
    SparseSet<int> s = selector->getGroupEditor()->getListBox()->getSelectedRows();
    if (buttons.indexOf(b) == ADSR){
        int fx_index = selector->getBoxes().indexOf((FxBox*)(selector->fxButtonChoice->getParentComponent()));
        Adsr* new_adsr = new Adsr();
        for (int i=s.size()-1; i>=0; i--){
            selector->getGroups()[s[i]]->group_fx[fx_index]->getFxType() = ADSR;
            selector->getGroups()[s[i]]->group_fx[fx_index]->setContent(static_cast<Component*>(new_adsr));
            
        }
        selector->fxButtonChoice->setFx(ADSR);
        bin->getFxComponent()->loadFx(ADSR, new_adsr);
    }
    else if (buttons.indexOf(b) == FILTER){
        selector->fxButtonChoice->setFx(FILTER);
        FilterComponent* f = new FilterComponent();
        
		f->setIIRFilter(MainContentComponent::_static_sampler->getFilter());
        
        int fx_index = selector->getBoxes().indexOf((FxBox*)(selector->fxButtonChoice->getParentComponent()));
        for (int i=s.size()-1; i>=0; i--){
            selector->getGroups()[s[i]]->group_fx[fx_index]->getFxType() = FILTER;
            selector->getGroups()[s[i]]->group_fx[fx_index]->setContent(static_cast<Component*>(f));
            
        }
        selector->fxButtonChoice->setFx(FILTER);
        bin->getFxComponent()->loadFx(FILTER, f);
    }
    else if (buttons.indexOf(b) == RINGMOD){
        int fx_index = selector->getBoxes().indexOf((FxBox*)(selector->fxButtonChoice->getParentComponent()));
        RingModulator* ring_mod = new RingModulator();
        for (int i=s.size()-1; i>=0; i--){
            selector->getGroups()[s[i]]->group_fx[fx_index]->getFxType() = RINGMOD;
            selector->getGroups()[s[i]]->group_fx[fx_index]->setContent(static_cast<Component*>(ring_mod));
            
        }
        selector->fxButtonChoice->setFx(RINGMOD);
        bin->getFxComponent()->loadFx(RINGMOD, ring_mod);
    }
    else{ 
            
        int fx_index = selector->getBoxes().indexOf((FxBox*)(selector->fxButtonChoice->getParentComponent()));
        for (int i=s.size()-1; i>=0; i--){
            selector->getGroups()[s[i]]->group_fx[fx_index]->getFxType() = NONE;
            selector->getGroups()[s[i]]->group_fx[fx_index]->setContent(nullptr);
        }
            
        selector->fxButtonChoice->setFx(NONE);
        bin->getFxComponent()->loadFx(NONE, nullptr);
    }
}

void FxChooser::focusButton(FxChoiceButton* button){
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

void FxChooser::unfocusButton(FxChoiceButton* button){
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