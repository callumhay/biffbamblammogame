/**
 * InTutorialGameDisplayState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __INTUTORIALGAMEDISPLAYSTATE_H__
#define __INTUTORIALGAMEDISPLAYSTATE_H__

#include "../BlammoEngine/TextLabel.h"
#include "../ESPEngine/ESP.h"
#include "../GameModel/GameModel.h"

#include "DisplayState.h"
#include "TutorialInGameRenderPipeline.h"
#include "CountdownHUD.h"
#include "GameDisplay.h"
#include "MouseRenderer.h"

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

	void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
    void MousePressed(const GameControl::MouseButton& pressedButton);
    void MouseReleased(const GameControl::MouseButton& releasedButton);
    void MouseMoved(int oglWindowCoordX, int oglWindowCoordY);
    void WindowFocus(bool windowHasFocus);
	void DisplaySizeChanged(int width, int height);

	DisplayState::DisplayStateType GetType() const;

private:
    GameModel::Difficulty beforeTutorialDifficulty;

	TutorialInGameRenderPipeline renderPipeline;
    TutorialEventsListener* tutorialListener;
    std::vector<TutorialHint*> noDepthTutorialHints;

    CountdownHUD boostCountdownHUD;

    void InitTutorialHints();

    DISALLOW_COPY_AND_ASSIGN(InTutorialGameDisplayState);
};

inline void InTutorialGameDisplayState::MouseMoved(int oglWindowCoordX, int oglWindowCoordY) {
    this->display->GetMouse()->ShowMouse(oglWindowCoordX, oglWindowCoordY);
}

inline void InTutorialGameDisplayState::WindowFocus(bool windowHasFocus) {
    this->display->GetMouse()->SetWindowHasFocus(windowHasFocus);
}

inline DisplayState::DisplayStateType InTutorialGameDisplayState::GetType() const {
    return DisplayState::InTutorialGame;
}

#endif // __INTUTORIALGAMEDISPLAYSTATE_H__