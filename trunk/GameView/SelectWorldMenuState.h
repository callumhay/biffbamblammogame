/**
 * SelectWorldMenuState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __SELECTWORLDMENUSTATE_H__
#define __SELECTWORLDMENUSTATE_H__

#include "DisplayState.h"
#include "../BlammoEngine/Animation.h"

class TextLabel2D;
class FBObj;
class CgFxBloom;
class KeyboardHelperLabel;

class SelectWorldMenuState : public DisplayState {
public:
    SelectWorldMenuState(GameDisplay* display);
    ~SelectWorldMenuState();

	void RenderFrame(double dT);
	void ButtonPressed(const GameControl::ActionButton& pressedButton);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
	void DisplaySizeChanged(int width, int height);
	DisplayState::DisplayStateType GetType() const;

private:
    static const char* WORLD_SELECT_TITLE;

    TextLabel2D* worldSelectTitleLbl;
    KeyboardHelperLabel* keyEscLabel;
    AnimationLerp<float> fadeAnimation;
    AnimationMultiLerp<float> pressEscAlphaAnim;

	CgFxBloom* bloomEffect;
	FBObj* menuFBO;

    bool goBackToMainMenu;
    void GoBackToMainMenu();


	DISALLOW_COPY_AND_ASSIGN(SelectWorldMenuState);
};

inline void SelectWorldMenuState::DisplaySizeChanged(int width, int height) {
	UNUSED_PARAMETER(width);
	UNUSED_PARAMETER(height);
}

inline DisplayState::DisplayStateType SelectWorldMenuState::GetType() const {
	return DisplayState::SelectWorldMenu;
}

#endif // __SELECTWORLDMENUSTATE_H__