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
class GameWorld;
class Texture;
class TextLabel2DFixedWidth;

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

    // Inner class for representing a selectable world item in the world select menu
    class WorldSelectItem {
    public:
        WorldSelectItem(const GameWorld* world, size_t worldNumber, float size);
        ~WorldSelectItem();

        void SetTopLeftCorner(float x, float y) {
            this->topLeftCorner[0] = x;
            this->topLeftCorner[1] = y;
        }
        void Draw(const Camera& camera, double dT);

    private:
        size_t worldNumber;
        const GameWorld* gameWorld;
        Texture* image;
        TextLabel2DFixedWidth* label;
        TextLabel2DFixedWidth* selectedLabel;
        Point2D topLeftCorner;
        float size;

        DISALLOW_COPY_AND_ASSIGN(WorldSelectItem);  
    };

    std::vector<WorldSelectItem*> worldItems;

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