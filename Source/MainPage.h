#ifndef MAINPAGE_H
#define MAINPAGE_H 

#include "../JuceLibraryCode/JuceHeader.h"
#include <map>
#include "MainPageComponents.h"

class InstrumentComponent;

class MainPage : public Component {
    Array<ScopedPointer<Label>> labels;
    InstrumentComponent& instrument;
public:
    MainPage(InstrumentComponent& i);
    ~MainPage();
    void changeListenerCallback(ChangeBroadcaster * src);
    void broughtToFront() override;
    void resized();
    void addNewComponent(String);
    void resetComponents();
    std::map<String, Component*>& getComponents(){return components;}
private:
    std::map<String, Component*> components;
    int idCount;
};

#endif /* MAINPAGE_H */
