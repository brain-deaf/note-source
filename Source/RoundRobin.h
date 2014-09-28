

#include "../JuceLibraryCode/JuceHeader.h"


class RoundRobinDropTarget;
class RoundRobinComponent : public Component, public Button::Listener
{
public:
	RoundRobinComponent(int, RoundRobinDropTarget*);
	~RoundRobinComponent();
	void resized();
	void buttonClicked(Button*);
	int getSelectedRR(){ return selectedRR; }
	void updateRoundRobinComponentForGroup();
private:
	ScopedPointer<Label> header;
	OwnedArray<TextButton> buttons;
	RoundRobinDropTarget* rrTarget;
	int selectedRR;
};

class RoundRobinDropTarget : public Component, public DragAndDropTarget
{
public:
	RoundRobinDropTarget();
	~RoundRobinDropTarget();
	bool isInterestedInDragSource(const SourceDetails&){ return true; }
	void itemDropped(const SourceDetails&) override;
	void itemDragEnter(const SourceDetails&) override;
	void itemDragExit(const SourceDetails&) override;
	void paint(Graphics&);
	void setRRComponent(RoundRobinComponent* r){ rrComponent = r; }
private:
	RoundRobinComponent* rrComponent;
	bool hovered;
};

class RoundRobin
{
public:
	RoundRobin();
	~RoundRobin(){ data = nullptr; }
	void setData(const String& filename);
	AudioSampleBuffer* getData(){ return data.get(); }
	const String& getFileName(){ return fileName; }
	void setState(bool b){ state = b; }
	bool getState(){ return state; }
private:
	ScopedPointer<AudioSampleBuffer> data;
	String fileName;
	int sampleIndex;
	bool state;
};

class RoundRobinGroup;
class RoundRobinPlayback
{
public:
	enum RRMode{
		RANDOM = 1,
		CYCLE = 2
	};
	RoundRobinPlayback(RoundRobinGroup* g) : currentRR(-1), mode(RANDOM), group(g){}
	~RoundRobinPlayback(){}
	void setMode(int m){ mode = m; }
	int getNextRoundRobin();
	int getMode(){ return mode; }
	int getCurrentRoundRobin(){ return currentRR; }
	RoundRobinGroup* getGroup(){ return group; }
private:
	int mode;
	int currentRR;
	RoundRobinGroup* group;
};

class RoundRobinGroup
{
public:
	RoundRobinGroup();
	~RoundRobinGroup();
	RoundRobin* getRoundRobin(int index){ return roundRobins[index]; }
	RoundRobinPlayback* getProcessor(){ return & processor; }
	int getNumRoundRobins();
	int getActiveRoundRobin(int);
private:
	OwnedArray<RoundRobin> roundRobins;
	RoundRobinPlayback processor;
};

