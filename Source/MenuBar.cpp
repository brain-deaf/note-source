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
            
            tf_bin = parent->getInstruments()[parent->getCurrentTabIndex()]
            ->getTabWindow().getTransformBin();
            
            scriptBin = parent->getInstruments()[parent->getCurrentTabIndex()]
            ->getTabWindow().getScriptBin();
            
            FileChooser patch_loader("Please select the patch you want to load",
                                     File::getCurrentWorkingDirectory(),
                                     "*.xml");
            
            if (patch_loader.browseForFileToOpen()){
                File xml = patch_loader.getResult();
                XmlDocument xml_doc(xml);
                XmlElement* instrument = xml_doc.getDocumentElement();
            
                mapping_editor->graph->loadPatch(instrument);
                fx_bin->getFxSelector()->loadPatch(instrument);
                tf_bin->getTransformSelector()->loadPatch(instrument);
                scriptBin->loadPatch(instrument);
            }
            break;
        }
        
        case ID_Save: {
            mapping_editor = parent->getInstruments()[parent->getCurrentTabIndex()]
            ->getTabWindow().getMappingEditorBin()->getMappingEditor();
            
            fx_bin = parent->getInstruments()[parent->getCurrentTabIndex()]
            ->getTabWindow().getFxBin();
            
            tf_bin = parent->getInstruments()[parent->getCurrentTabIndex()]
            ->getTabWindow().getTransformBin();
            
            scriptBin = parent->getInstruments()[parent->getCurrentTabIndex()]
            ->getTabWindow().getScriptBin();
            
            Array<FxGroup*> fx_group_list = fx_bin->getFxSelector()->getGroups();
            Array<TransformGroup*> tf_group_list = tf_bin->getTransformSelector()->getGroups();
            
            XmlElement instrument("INSTRUMENT");
            
            XmlElement* script_element = new XmlElement("SCRIPT");
            script_element->setAttribute("path", scriptBin->getScriptPath());
            instrument.addChildElement(script_element);
            
            for (int i=0; i<mapping_editor->graph->getGroups().size(); i++){
                XmlElement* group = new XmlElement("GROUP");
                group->setAttribute("name", mapping_editor->graph->getGroupEditor()->getModel()->getGroupNames()[i]);
                for (int j=0; j<mapping_editor->graph->getGroups()[i]->getZones()->size(); j++){
                    XmlElement* zone = new XmlElement("ZONE");
                    Zone* z = (*( (mapping_editor->graph->getGroups()) [i] -> getZones() )) [j];
                    String path(z->getName());
                    int    x(z->getX());
                    int    y(z->getY());
                    int    width(z->get_width());
                    int    height(z->getHeight());
                    int    note(z->getNote());
                    double sample_start(z->getPlaySettings()->getSampleStart());

                    zone->setAttribute("file", path);
                    zone->setAttribute("x", x);
                    zone->setAttribute("y", y);
                    zone->setAttribute("width", width);
                    zone->setAttribute("height", height);
                    zone->setAttribute("note", note);
                    zone->setAttribute("sample_start", sample_start);
                    zone->setAttribute("loop_mode", z->getPlaySettings()->getLoopMode());
                    zone->setAttribute("loop_start", z->getPlaySettings()->getLoopStart());
                    zone->setAttribute("loop_end", z->getPlaySettings()->getLoopEnd());
                    zone->setAttribute("xfade_length", z->getPlaySettings()->getXfadeLength());
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
                        case (FxChooser::FX::RINGMOD):{
                            RingModulator* ringmod = static_cast<RingModulator*>(insert_fx->getContent());
                            fx->setAttribute("frequency", ringmod->getFrequencySlider()->getValue());
                            fx->setAttribute("mix", ringmod->getAmplitudeSlider()->getValue());
                            break;
                        }
                        case (FxChooser::FX::NONE) :
                            break;
                    }
                    group->addChildElement(fx);
                }
                for (int j=0; j<fx_group_list[i]->group_fx.size(); j++){
                    XmlElement* tf = new XmlElement("TRANSFORM");
                    Transform* insert_fx = tf_group_list[i]->group_fx[j];
                    tf->setAttribute("type", insert_fx->getFxType());
                    
                    switch (insert_fx->getFxType()){
                        case (TransformChooser::FX::LINEAR):{
                            LinearTransform* linear = static_cast<LinearTransform*>(insert_fx->getContent());
                            tf->setAttribute("start", linear->getStartSlider()->getValue());
                            tf->setAttribute("end", linear->getEndSlider()->getValue());
                            tf->setAttribute("source", linear->getSourceBox()->getSelectedId());
                            tf->setAttribute("target", linear->getTargetBox()->getSelectedId());
                            Array<Point<int> > points = linear->getGraph()->getPoints();
                            for (int i=0; i<points.size(); i++){
                                XmlElement* tfa = new XmlElement("POINT");
                                tfa->setAttribute("x", points[i].getX());
                                tfa->setAttribute("y", points[i].getY());
                                tf->addChildElement(tfa);
                            }
                                
                            break;
                        }
                        case (TransformChooser::FX::EXPONENTIAL):{
                            ExponentialTransform* exponential = static_cast<ExponentialTransform*>(insert_fx->getContent());
                            tf->setAttribute("start", exponential->getStartSlider()->getValue());
                            tf->setAttribute("end", exponential->getEndSlider()->getValue());
                            tf->setAttribute("source", exponential->getSourceBox()->getSelectedId());
                            tf->setAttribute("target", exponential->getTargetBox()->getSelectedId());
                            Array<Point<int> > points = exponential->getGraph()->getPoints();
                            Array<double> curves = exponential->getGraph()->getCurves();
                            for (int i=0; i<points.size(); i++){
                                XmlElement* tfa = new XmlElement("POINT");
                                tfa->setAttribute("x", points[i].getX());
                                tfa->setAttribute("y", points[i].getY());
                                tfa->setAttribute("curve", curves[i]);
                                tf->addChildElement(tfa);
                            }
                                
                            break;
                        }
                        case (FxChooser::FX::NONE) :
                            break;
                    }
                    group->addChildElement(tf);
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

    


