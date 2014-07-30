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
#include "ScriptBin.h"

class InstrumentMappingEditor;

class MenuBar : public MenuBarModel, public Component
{
public:
    MenuBar(InstrumentBin * p);
    
    StringArray getMenuBarNames();
    PopupMenu getMenuForIndex(int topLevelMenuIndex, const String& menuName);
    void menuItemSelected(int menuItemID, int topLevelMenuIndex);
    void resized();
    AudioDeviceManager* getDeviceManager(){return deviceManager;}
    
    class AudioSettingsWindow : public DocumentWindow
    {
    public:
        AudioSettingsWindow(const String& name, Colour backgroundColour, int requiredButtons, bool addToDesktop, MenuBar* parent);
        void closeButtonPressed();
    private:
        MenuBar* _parent;
    };

    enum menuIDs{
        ID_New = 1000,
        ID_Save = 1001,  
        ID_Quit = 1002,
        ID_Open = 1003,
        ID_View1 = 1004, 
        ID_Edit1 = 1005,
        ID_AudioSettings = 1006,
    };

private:
    SharedResourcePointer<AudioDeviceManager> deviceManager;
    MenuBarComponent menuBar;
    PopupMenu file;
    PopupMenu view;
    PopupMenu edit; 
    AudioSettingsWindow* audioSettingsWindow;
    InstrumentBin* parent;
    std::shared_ptr<InstrumentMappingEditor> mapping_editor;
    ScriptBin* scriptBin;
    FxBin* fx_bin;
};



#endif  // MENUBAR_H_INCLUDED
