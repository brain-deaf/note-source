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
    void addNewComponent(int);
    void resetComponents();
    std::map<int, Component*>& getComponents(){return components;}
private:
    std::map<int, Component*> components;
    int idCount;
};

#endif /* MAINPAGE_H */
