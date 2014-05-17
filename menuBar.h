/*
  ==============================================================================

    menuBar.h
    Created: 7 May 2014 6:15:08pm
    Author:  patrick

  ==============================================================================
*/

#ifndef MENUBAR_H_INCLUDED
#define MENUBAR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "instrumentBin.h"

class menuBar : public MenuBarModel, public Component
{
public:
    menuBar();
    ~menuBar();
    
    StringArray getMenuBarNames();
    PopupMenu getMenuForIndex(int topLevelMenuIndex, const String& menuName);
    void menuItemSelected(int menuItemID, int topLevelMenuIndex);
    void resized();
    void set_parent_instrument_bin(instrumentBin* parent_instrument_bin);
    
    AudioDeviceSelectorComponent* audio_settings;
    AudioDeviceManager* device_manager;
      
    class midiDeviceCallback : public MidiInputCallback
    {
    public:
        void handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message){
            //std::cout<<"midi event!!"<<std::endl;
        };
    };
    
    class audioSettingsWindow : public DocumentWindow
    {
    public:
        audioSettingsWindow(const String& name, Colour backgroundColour, int requiredButtons, bool addToDesktop);
        void closeButtonPressed(){delete this;};
    };
    
    midiDeviceCallback* midi_callback;
    
    audioSettingsWindow* audio_settings_window;
    
    enum menuIDs{
        ID_New = 1000,
        ID_Save,  
        ID_Quit,  
        ID_View1, 
        ID_Edit1,
        ID_AudioSettings,
    };
private:
    MenuBarComponent menu_bar_component;
    PopupMenu menu_file;
    PopupMenu menu_view;
    PopupMenu menu_edit;
    instrumentBin* parent_instrument_bin;
};



#endif  // MENUBAR_H_INCLUDED
