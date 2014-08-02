#include "MainPage.h"
#include "InstrumentComponent.h"

MainPage::MainPage(InstrumentComponent& i) : Component(), idCount(0), instrument(i){}

MainPage::~MainPage() {
    for (auto c : components){
        delete c.second;
    }
}

void MainPage::resized(){}

void MainPage::broughtToFront() {}

void MainPage::addNewComponent(String s){
    addAndMakeVisible(components[s]);
    components[s]->setTopLeftPosition(0, 0);
    components[s]->setSize(components[s]->getWidth(), components[s]->getHeight());
}

void MainPage::resetComponents(){
    for (auto c : components){
        delete c.second;
    }
    components.clear();
}
