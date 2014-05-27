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
#include "InstrumentBin.h"

class MenuBar : public MenuBarModel, public Component
{
public:
    MenuBar();
    ~MenuBar();
    
    StringArray getMenuBarNames();
    PopupMenu getMenuForIndex(int topLevelMenuIndex, const String& menuName);
    void menuItemSelected(int menuItemID, int topLevelMenuIndex);
    void resized();
    void set_parent_instrument_bin(InstrumentBin* parent_instrument_bin);
    
    class AudioSettingsWindow : public DocumentWindow
    {
    public:
        AudioSettingsWindow(const String& name, Colour backgroundColour, int requiredButtons, bool addToDesktop);
        void closeButtonPressed(){delete this;}
    };

    enum menuIDs{
        ID_New = 1000,
        ID_Save,  
        ID_Quit,  
        ID_View1, 
        ID_Edit1,
        ID_AudioSettings,
    };

private:
    SharedResourcePointer<AudioDeviceManager> device_manager_;
    MenuBarComponent menu_bar_component;
    PopupMenu menu_file;
    PopupMenu menu_view;
    PopupMenu menu_edit;
    AudioSettingsWindow* audio_settings_window;
    InstrumentBin* parent_instrument_bin;
};



#endif  // MENUBAR_H_INCLUDED
