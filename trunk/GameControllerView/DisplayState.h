#ifndef __DISPLAYSTATE_H__
#define __DISPLAYSTATE_H__

class GameDisplay;

class DisplayState {

protected:
	GameDisplay* display;

public:
	DisplayState(GameDisplay* display) : display(display) {}
	virtual ~DisplayState() {};

	virtual void RenderFrame(double dT) = 0;
	virtual void KeyPressed(unsigned char key) = 0;

};

#endif