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
    MenuBar(InstrumentBin * p);
    
    StringArray getMenuBarNames();
    PopupMenu getMenuForIndex(int topLevelMenuIndex, const String& menuName);
    void menuItemSelected(int menuItemID, int topLevelMenuIndex);
    void resized();
    
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
    SharedResourcePointer<AudioDeviceManager> deviceManager;
    MenuBarComponent menuBar;
    PopupMenu file;
    PopupMenu view;
    PopupMenu edit; 
    ScopedPointer<AudioSettingsWindow> audioSettingsWindow;
    InstrumentBin* parent;
};



#endif  // MENUBAR_H_INCLUDED
