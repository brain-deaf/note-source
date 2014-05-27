/*
  ==============================================================================

    menuBar.cpp
    Created: 7 May 2014 6:14:56pm
    Author:  patrick

  ==============================================================================
*/
#include <stdexcept>
#include "MenuBar.h"

MenuBar::MenuBar() : menu_bar_component{this}, menu_file{}, menu_view{}, menu_edit{},
    audio_settings_window{nullptr},parent_instrument_bin{nullptr} {
    addAndMakeVisible(menu_bar_component);
    
    menu_file.addItem(ID_New, "New Instrument");
    menu_file.addItem(ID_Save, "Save Instrument");
    menu_file.addItem(ID_AudioSettings, "Audio and MIDI Settings");
    menu_file.addItem(ID_Quit, "Quit");
    menu_view.addItem(ID_View1, "Nothing Here");
    menu_edit.addItem(ID_Edit1, "Nothing Here");
}

MenuBar::~MenuBar() {
    delete audio_settings_window; // in case main window closes while it's up
}

MenuBar::AudioSettingsWindow::AudioSettingsWindow(const String& name, Colour backgroundColour, int requiredButtons, bool addToDesktop=true)
: DocumentWindow{name, backgroundColour, requiredButtons, addToDesktop}{}
    
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
        return menu_file;
    }
    
    else if (menuName == "View"){
        return menu_view;
    }
    
    else if (menuName == "Edit"){
        return menu_edit;
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
            InstrumentComponent* i = new InstrumentComponent(parent_instrument_bin);
            parent_instrument_bin->addTab("New Instrument", Colour(100, 100, 100), i, false);
            parent_instrument_bin->register_tab(i);
            break;
        }

        case ID_AudioSettings: {
            audio_settings_window = new AudioSettingsWindow("Audio and MIDI Settings", Colours::grey,
            DocumentWindow::closeButton, true);
            auto audio_settings = new AudioDeviceSelectorComponent(*device_manager_, 0, 2, 0, 2, true, true, true, false); 
            audio_settings->setBounds(0, 0, 500, 400);
            audio_settings_window->setContentOwned(audio_settings, true);
            audio_settings_window->setVisible(true);
            break;
        }
    }
}

void MenuBar::resized(){
    menu_bar_component.setBounds(0, 0, getWidth(), 20);
}

void MenuBar::set_parent_instrument_bin(InstrumentBin* bin){
    parent_instrument_bin = bin;
}
