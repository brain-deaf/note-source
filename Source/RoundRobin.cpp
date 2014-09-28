#include "RoundRobin.h"
#include "ZoneInfo.h"
#include "SampleBrowserBin.h"
#include <stdlib.h> 

RoundRobinComponent::RoundRobinComponent(int n, RoundRobinDropTarget* target) : Component(), selectedRR(0), rrTarget(target)
{
	for (int i = 0; i < n; i++){
		buttons.add(new TextButton(""));
	}
	for (auto button : buttons){
		button->addListener(this);
		addAndMakeVisible(button);
	}
	buttons[0]->setToggleState(true, false);
	target->setRRComponent(this);

	header = new Label("");
	header->setText("Round Robins", dontSendNotification);
	header->setJustificationType(Justification::horizontallyCentred);
	addAndMakeVisible(header);
}

RoundRobinComponent::~RoundRobinComponent(){
	for (auto button : buttons){
		button = nullptr;
	}
	header = nullptr;
}

void RoundRobinComponent::buttonClicked(Button* source){
	for (auto button : buttons){
		button->setToggleState(false, false);
	}
	source->setToggleState(true, false);

	selectedRR = buttons.indexOf((TextButton*)source);

	ZoneInfo* parent = static_cast<ZoneInfo*>(getParentComponent());
	String file_name = parent->getZone()->getRRGroup()->getRoundRobin(selectedRR)->getFileName();
	parent->getFileNameLabel()->setText(file_name, dontSendNotification);
	parent->getAudioThumbnail()->updateWaveformForFilePlayer(parent->getZone(), selectedRR);
}

void RoundRobinComponent::resized(){
	header->setBounds(0, 0, getWidth(), 20);
	float button_width = getWidth() / (buttons.size() / 2.0);
	float button_height = (getHeight() - 20) / 2.0;
	for (int i = 0; i < buttons.size(); i++){
		buttons[i]->setBounds(button_width*(i % (buttons.size() / 2)), button_height * (int)(i / (buttons.size() / 2)) + 20, button_width, button_height);
	}
}

RoundRobinDropTarget::RoundRobinDropTarget() : Component(), rrComponent(nullptr)
{

}

RoundRobinDropTarget::~RoundRobinDropTarget(){}

void RoundRobinDropTarget::itemDropped(const SourceDetails& details){
	if (details.description.toString() == "file name"){
		StringArray s;
		DragButton* d = static_cast<DragButton*>(details.sourceComponent.get());
		String filePath(d->getFileList()[0]);
		String extension(File(filePath).getFileExtension());
		if (extension == ".wav" || extension == ".aiff" || extension == ".ogg"){
			ZoneInfo* parent = static_cast<ZoneInfo*>(getParentComponent());
			if (parent->getZone() != nullptr){
				RoundRobinGroup* r = parent->getZone()->getRRGroup();
				r->getRoundRobin(rrComponent->getSelectedRR())->setData(filePath);
				parent->getAudioThumbnail()->updateWaveformForFilePlayer(parent->getZone(), rrComponent->getSelectedRR());
				parent->getFileNameLabel()->setText(filePath, dontSendNotification);
			}
		}
	}

}

void RoundRobinDropTarget::paint(Graphics& g){
	g.setColour(Colours::black);

	Path borderPath;
	borderPath.startNewSubPath(0.0f, 0.0f);
	borderPath.lineTo(0.0f, getHeight());
	borderPath.lineTo(getWidth(), getHeight());
	borderPath.lineTo(getWidth(), 0.0f);
	borderPath.lineTo(0.0f, 0.0f);

	g.strokePath(borderPath, PathStrokeType(2.0f));
}

RoundRobin::RoundRobin() : data(nullptr), fileName(""), sampleIndex(-1){}

void RoundRobin::setData(const String& filename){
	if (File(filename).exists()){
		const File f(filename);
		FileInputStream* stream(new FileInputStream(f));
		AudioFormatReader* reader;
		if (f.getFileExtension() == ".wav"){
			WavAudioFormat w;
			reader = w.createReaderFor(stream, true);
		}
		if (f.getFileExtension() == ".ogg"){
			OggVorbisAudioFormat w;
			reader = w.createReaderFor(stream, true);
		}
		if (f.getFileExtension() == ".aiff"){
			AiffAudioFormat w;
			reader = w.createReaderFor(stream, true);
			
		}
		
		if (stream != nullptr){
			fileName = filename;
			data = new AudioSampleBuffer(reader->numChannels, reader->lengthInSamples);
			reader->read(data, 0, reader->lengthInSamples, 0, true, true);
			delete reader;
		}
		else{
			data = nullptr;
		}
	}
}

RoundRobinGroup::RoundRobinGroup() : processor(this){
	for (int i = 0; i < 10; i++)
		roundRobins.add(new RoundRobin());
}

RoundRobinGroup::~RoundRobinGroup(){
	for (auto rr : roundRobins)
		rr = nullptr;
}

int RoundRobinGroup::getNumRoundRobins(){
	int rr_count(0);
	for (auto rr : roundRobins){
		if (rr->getData() == nullptr){
			return rr_count;
		}
		else{
			rr_count++;
		}
	}
	return rr_count;
}

int RoundRobinPlayback::getNextRoundRobin(){
	int num_round_robins = group->getNumRoundRobins();

	switch (mode){
	case RANDOM:
		currentRR = rand() % (num_round_robins + 1) - 1;
	case CYCLE:
		currentRR = ++currentRR % (num_round_robins + 1) - 1;
	}
	return currentRR;
}
