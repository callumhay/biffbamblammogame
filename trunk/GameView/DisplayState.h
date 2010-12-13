#ifndef __DISPLAYSTATE_H__
#define __DISPLAYSTATE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../GameControl/GameControl.h"

class GameDisplay;

class DisplayState {

public:
	enum DisplayStateType { MainMenu, Blammopedia, LevelStart, WorldStart, InGame, InGameMenu, LevelEnd, GameComplete, GameOver };
	static DisplayState* BuildDisplayStateFromType(const DisplayStateType& type, GameDisplay* display);

	DisplayState(GameDisplay* display) : display(display) {}
	virtual ~DisplayState() {};

	virtual void RenderFrame(double dT) = 0;

	virtual void ButtonPressed(const GameControl::ActionButton& pressedButton)   = 0;
	virtual void ButtonReleased(const GameControl::ActionButton& releasedButton) = 0;

	virtual void DisplaySizeChanged(int width, int height) = 0;

	virtual DisplayState::DisplayStateType GetType() const = 0;

protected:
	GameDisplay* display;

private:
	DISALLOW_COPY_AND_ASSIGN(DisplayState);

};

#endif