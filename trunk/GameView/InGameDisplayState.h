#ifndef __INGAMEDISPLAYSTATE_H__
#define __INGAMEDISPLAYSTATE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "DisplayState.h"
#include "InGameRenderPipeline.h"

class Texture2D;

/**
 * State for representing and rendering the display of the game as it is
 * being played by the user.
 */
class InGameDisplayState : public DisplayState {

public:
	InGameDisplayState(GameDisplay* display);
	~InGameDisplayState();

	void RenderFrame(double dT);
	void KeyPressed(SDLKey key, SDLMod modifier);
	void KeyReleased(SDLKey key, SDLMod modifier);
	void DisplaySizeChanged(int width, int height);

private:
	InGameRenderPipeline renderPipeline;

#ifdef _DEBUG
	void DebugDrawBounds();
#endif
};

inline void InGameDisplayState::KeyReleased(SDLKey key, SDLMod modifier) {
}

#endif