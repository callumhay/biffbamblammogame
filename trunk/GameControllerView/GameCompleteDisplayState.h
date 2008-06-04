#ifndef __GAMECOMPLETEDISPLAYSTATE_H__
#define __GAMECOMPLETEDISPLAYSTATE_H__

#include "DisplayState.h"
#include "TextLabel.h"

#include <string>

/**
 * State for displaying the completion of the game animation and
 * end credits.
 */
class GameCompleteDisplayState : public DisplayState {

private:
	static const std::string GAME_COMPLETE_TEXT;
	TextLabel2D gameCompleteLabel;
	float gameCompleteLabelRasterWidth;

public:
	GameCompleteDisplayState(GameDisplay* display);
	virtual ~GameCompleteDisplayState();

	virtual void RenderFrame(double dT);
	virtual void KeyPressed(unsigned char key);

};

#endif