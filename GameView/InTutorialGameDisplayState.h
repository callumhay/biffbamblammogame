/**
 * InTutorialGameDisplayState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __INTUTORIALGAMEDISPLAYSTATE_H__
#define __INTUTORIALGAMEDISPLAYSTATE_H__

#include "DisplayState.h"
#include "InGameRenderPipeline.h"
#include "BoostCountdownHUD.h"

class TutorialHint;
class TutorialEventsListener;

/**
 * This class is the game state where the game is in play but the current
 * level is the tutorial level and therefore has various graphical hints
 * and other ease-of-play characteristics activated.
 */
class InTutorialGameDisplayState : public DisplayState {
public:
	InTutorialGameDisplayState(GameDisplay* display);
	~InTutorialGameDisplayState();

    bool AllowsGameModelUpdates() const { return true; }
	void RenderFrame(double dT);

	void ButtonPressed(const GameControl::ActionButton& pressedButton);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
    void MousePressed(const GameControl::MouseButton& pressedButton);
    void MouseReleased(const GameControl::MouseButton& releasedButton);

	void DisplaySizeChanged(int width, int height);

	DisplayState::DisplayStateType GetType() const;

private:
	InGameRenderPipeline renderPipeline;
    TutorialEventsListener* tutorialListener;
    std::vector<TutorialHint*> tutorialHints;
    BoostCountdownHUD boostCountdownHUD;

    void InitTutorialHints();

    DISALLOW_COPY_AND_ASSIGN(InTutorialGameDisplayState);
};


inline DisplayState::DisplayStateType InTutorialGameDisplayState::GetType() const {
    return DisplayState::InTutorialGame;
}

#endif // __INTUTORIALGAMEDISPLAYSTATE_H__