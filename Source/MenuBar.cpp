/*
  ==============================================================================

    menuBar.cpp
    Created: 7 May 2014 6:14:56pm
    Author:  patrick

  ==============================================================================
*/
#include <stdexcept>
#include "MenuBar.h"

MenuBar::MenuBar(InstrumentBin * i) : menuBar{this}, deviceManager(), file{}, view{}, edit{},
    audioSettingsWindow{nullptr}, parent{i} {
    addAndMakeVisible(menuBar);
    
    file.addItem(ID_New, "New Instrument");
    file.addItem(ID_Save, "Save Instrument");
    file.addItem(ID_AudioSettings, "Audio and MIDI Settings");
    file.addItem(ID_Quit, "Quit");
    view.addItem(ID_View1, "Nothing Here");
    edit.addItem(ID_Edit1, "Nothing Here");
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
            JUCEApplication::quit();
            break;
        }

        case ID_New: {
            InstrumentComponent* i = new InstrumentComponent(parent);
            parent->addTab("New Instrument", Colour(100, 100, 100), i, false);
            parent->registerTab(i);
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

    


