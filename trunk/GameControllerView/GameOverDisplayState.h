#ifndef __GAMEOVERDISPLAYSTATE_H__
#define __GAMEOVERDISPLAYSTATE_H__

#include "DisplayState.h"
#include "TextLabel.h"

#include <string>

class GameOverDisplayState : public DisplayState {

private:
	static const std::string GAME_OVER_TEXT;
	TextLabel2D gameOverLabel;

public:
	GameOverDisplayState(GameDisplay* display);
	virtual ~GameOverDisplayState();

	virtual void RenderFrame(double dT);
	virtual void KeyPressed(unsigned char key);

};
#endif
