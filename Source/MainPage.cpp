#include "MainPage.h"
#include "InstrumentComponent.h"

MainPage::MainPage(InstrumentComponent& i) : Component(), idCount(0), instrument(i){}

MainPage::~MainPage() {
	for (int i = 0; i < idCount; i++){
		delete components[i];
	}
}

void MainPage::resized(){}

void MainPage::broughtToFront() {}

void MainPage::addNewComponent(int i){
    addAndMakeVisible(components[i]);
    components[i]->setTopLeftPosition(0, 0);
    components[i]->setSize(components[i]->getWidth(), components[i]->getHeight());
}

void MainPage::resetComponents(){
	for (int i = 0; i < idCount; i++){
		delete components[i];
	}
	idCount = 0;
}
