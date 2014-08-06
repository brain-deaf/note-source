/*
  ==============================================================================

    menuBar.cpp
    Created: 7 May 2014 6:14:56pm
    Author:  patrick

  ==============================================================================
*/
#include <stdexcept>
#include "MenuBar.h"
#include "InstrumentMappingEditor.h"
#include "FxSelector.h"
#include "Adsr.h"
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

MenuBar::MenuBar(InstrumentBin * i) : menuBar(this), scriptBin(nullptr), deviceManager(), file{}, view{}, edit{},
    audioSettingsWindow{nullptr}, parent(i) {
    addAndMakeVisible(menuBar);
    
    file.addItem(ID_New, "New Instrument");
    file.addItem(ID_Open, "Open Instrument");
    file.addItem(ID_Save, "Save Instrument");
    file.addItem(ID_AudioSettings, "Audio and MIDI Settings");
    file.addItem(ID_Quit, "Quit");
    view.addItem(ID_View1, "Nothing Here");
    edit.addItem(ID_Edit1, "Nothing Here");
    
    mapping_editor = nullptr;
    //instrumentBin, instrumentComponent, instrumentTabWindow, mappingeditorbin, instrumentmappingeditor, mappingeditorgraph
}

MenuBar::AudioSettingsWindow::AudioSettingsWindow(const String& name, Colour backgroundColour, int requiredButtons, bool addToDesktop=true, MenuBar* parent=nullptr)
: DocumentWindow{name, backgroundColour, requiredButtons, addToDesktop}, _parent(parent){}

void MenuBar::AudioSettingsWindow::closeButtonPressed(){
    XmlElement* state = _parent->getDeviceManager()->createStateXml();
    File p(File::getCurrentWorkingDirectory().getChildFile("audio_settings.xml"));
    state->writeToFile(p, StringRef(""));
    delete this;
}
 
StringArray MenuBar::getMenuBarNames(){
    StringArray s;
    s.add("File");
    s.add("View");
    s.add("Edit");
    
    return s;
}

class BadMenuException : public std::runtime_error {
public:
    BadMenuException(String menuName) : std::runtime_error(menuName.toStdString()){}
};

PopupMenu MenuBar::getMenuForIndex(int topLevelMenuIndex, const String& menuName){
    if (menuName == "File"){
        return file;
    }
    
    else if (menuName == "View"){
        return view;
    }
    
    else if (menuName == "Edit"){
        return edit;
    }
    else {
        throw BadMenuException(menuName);
    }
}

void MenuBar::menuItemSelected(int menuItemID, int topLevelMenuIndex){
    switch (menuItemID) {
        case ID_Quit: {
            for (auto instrument : parent->getInstruments()){
                scriptBin = instrument->getTabWindow().getScriptBin();
                lua_close(scriptBin->getLuaScript()->getLuaState());
            }
            JUCEApplication::quit();
            break;
        }

        case ID_New: {
            InstrumentComponent* i = new InstrumentComponent(parent);
            parent->addTab("New Instrument", Colour(100, 100, 100), i, false);
            parent->registerTab(i);
            break;
        }
        
        case ID_Open: {
            mapping_editor = parent->getInstruments()[parent->getCurrentTabIndex()]
            ->getTabWindow().getMappingEditorBin()->getMappingEditor();
            
            fx_bin = parent->getInstruments()[parent->getCurrentTabIndex()]
            ->getTabWindow().getFxBin();
            
            FileChooser patch_loader("Please select the patch you want to load",
                                     File::getCurrentWorkingDirectory(),
                                     "*.xml");
            
            if (patch_loader.browseForFileToOpen()){
                File xml = patch_loader.getResult();
                XmlDocument xml_doc(xml);
                XmlElement* instrument = xml_doc.getDocumentElement();
            
                mapping_editor->graph->loadPatch(instrument);
                fx_bin->getFxSelector()->loadPatch(instrument);
            }
            break;
        }
        
        case ID_Save: {
            mapping_editor = parent->getInstruments()[parent->getCurrentTabIndex()]
            ->getTabWindow().getMappingEditorBin()->getMappingEditor();
            
            fx_bin = parent->getInstruments()[parent->getCurrentTabIndex()]
            ->getTabWindow().getFxBin();
            
            Array<FxGroup*> fx_group_list = fx_bin->getFxSelector()->getGroups();
            
            XmlElement instrument("INSTRUMENT");
            for (int i=0; i<mapping_editor->graph->getGroups().size(); i++){
                XmlElement* group = new XmlElement("GROUP");
                group->setAttribute("name", mapping_editor->graph->getGroupEditor()->getModel()->getGroupNames()[i]);
                for (int j=0; j<mapping_editor->graph->getGroups()[i]->getZones()->size(); j++){
                    XmlElement* zone = new XmlElement("ZONE");
                    
                    String path(  (*( (mapping_editor->graph->getGroups()) [i] -> getZones() )) [j]->getName());
                    int    x(  (*( (mapping_editor->graph->getGroups()) [i] -> getZones() )) [j]->getX());
                    int    y(  (*( (mapping_editor->graph->getGroups()) [i] -> getZones() )) [j]->getY());
                    int    width(  (*( (mapping_editor->graph->getGroups()) [i] -> getZones() )) [j]->get_width());
                    int    height(  (*( (mapping_editor->graph->getGroups()) [i] -> getZones() )) [j]->getHeight());
                    int    note(  (*( (mapping_editor->graph->getGroups()) [i] -> getZones() )) [j]->getNote());
                    double sample_start(  (*( (mapping_editor->graph->getGroups()) [i] -> getZones() )) [j]->getPlaySettings().getSampleStart());

                    
                    zone->setAttribute("file", path);
                    zone->setAttribute("x", x);
                    zone->setAttribute("y", y);
                    zone->setAttribute("width", width);
                    zone->setAttribute("height", height);
                    zone->setAttribute("note", note);
                    zone->setAttribute("sample_start", sample_start);
                    group->addChildElement(zone);
                }
                for (int j=0; j<fx_group_list[i]->group_fx.size(); j++){
                    XmlElement* fx = new XmlElement("FX");
                    Fx* insert_fx = fx_group_list[i]->group_fx[j];
                    fx->setAttribute("type", insert_fx->getFxType());
                    
                    switch (insert_fx->getFxType()){
                        case (FxChooser::FX::ADSR):{
                            Adsr* adsr = (Adsr*) insert_fx->getContent();
                            fx->setAttribute("attack", adsr->getAttackTimeSlider()->getValue());
                            fx->setAttribute("attack_curve", adsr->getAttackCurveSlider()->getValue());
                            fx->setAttribute("decay", adsr->getDecayTimeSlider()->getValue());
                            fx->setAttribute("decay_curve", adsr->getDecayCurveSlider()->getValue());
                            fx->setAttribute("sustain", adsr->getSustainSlider()->getValue());
                            fx->setAttribute("release", adsr->getReleaseTimeSlider()->getValue());
                            fx->setAttribute("release_curve", adsr->getReleaseCurveSlider()->getValue());
                            break;
                        }
                        case (FxChooser::FX::NONE) :
                            break;
                    }
                    group->addChildElement(fx);
                }       
                instrument.addChildElement(group);
            }
            FileChooser patch_saver("Please select the patch destination.",
                                     File::getCurrentWorkingDirectory(),
                                     "*.xml");
            
            if (patch_saver.browseForFileToOpen()){
                File xml = patch_saver.getResult();
                instrument.writeToFile(xml, "");
            }
            break;
        }

        case ID_AudioSettings: {
            audioSettingsWindow = nullptr;
            audioSettingsWindow = new AudioSettingsWindow("Audio and MIDI Settings", Colours::grey,
            DocumentWindow::closeButton, true, this);
            auto audioSettings = new AudioDeviceSelectorComponent(*deviceManager, 0, 2, 0, 2, true, true, true, false); 
            audioSettings->setBounds(0, 0, 500, 400);
            audioSettingsWindow->setContentOwned(audioSettings, true);
            audioSettingsWindow->setVisible(true);
            break;
        }
    }
}

void MenuBar::resized(){
    menuBar.setBounds(0, 0, getWidth(), 20);
}

    


