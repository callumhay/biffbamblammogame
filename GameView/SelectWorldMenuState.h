/**
 * SelectWorldMenuState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __SELECTWORLDMENUSTATE_H__
#define __SELECTWORLDMENUSTATE_H__

#include "DisplayState.h"
#include "CgFxGreyscale.h"
#include "../BlammoEngine/Animation.h"

class TextLabel2D;
class FBObj;
class CgFxBloom;
class KeyboardHelperLabel;
class GameWorld;
class Texture;
class TextLabel2DFixedWidth;

/** 
 * Menu state for displaying all levels that have been unlocked by the user playing the game.
 * This menu gives access to the world's levels in the SelectLevelMenuState.
 */
class SelectWorldMenuState : public DisplayState {
public:
    SelectWorldMenuState(GameDisplay* display);
    SelectWorldMenuState(GameDisplay* display, const GameWorld* selectedWorld);
    ~SelectWorldMenuState();

    bool AllowsGameModelUpdates() const { return true; }

	void RenderFrame(double dT);

	void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
    void MousePressed(const GameControl::MouseButton& pressedButton) { UNUSED_PARAMETER(pressedButton); };
    void MouseReleased(const GameControl::MouseButton& releasedButton) { UNUSED_PARAMETER(releasedButton); };

	void DisplaySizeChanged(int width, int height);
	DisplayState::DisplayStateType GetType() const;

private:
    static const char* WORLD_SELECT_TITLE;
    static const float MENU_ITEM_HORIZ_GAP;

    TextLabel2D* worldSelectTitleLbl;
    KeyboardHelperLabel* keyEscLabel;
    AnimationLerp<float> fadeAnimation;
    AnimationMultiLerp<float> pressEscAlphaAnim;

    // Animation of the alphas for the selection indicator
    AnimationMultiLerp<float> selectionAlphaOrangeAnim;   
    AnimationMultiLerp<float> selectionAlphaYellowAnim;   
    // Animation of the scales for the selection indicator
    AnimationMultiLerp<float> selectionBorderAddYellowAnim; 

	CgFxBloom* bloomEffect;
	FBObj* menuFBO;

    Texture2D* starryBG;
    Texture2D* padlockTex;

    // Inner class for representing a selectable world item in the world select menu
    class WorldSelectItem {
    public:
        WorldSelectItem(SelectWorldMenuState* state, const GameWorld* world, size_t worldNumber,
            float size, bool isLocked);
        ~WorldSelectItem();

        void SetIsSelected(bool isSelected);

        void SetTopLeftCorner(float x, float y) {
            this->topLeftCorner[0] = x;
            this->topLeftCorner[1] = y;
        }
        void Draw(const Camera& camera, double dT);
        void DrawSelectionBorder(const Camera& camera, double dT, float orangeAlpha, float yellowAlpha, float sizeAmt);

        const GameWorld* GetWorld() const { return this->gameWorld; }

        bool GetIsLocked() const { return this->isLocked; }
        void ExecuteLockedAnimation();

    private:
        static const float PADLOCK_SCALE;

        SelectWorldMenuState* state;
        bool isSelected;
        bool isLocked;
        size_t worldNumber;
        const GameWorld* gameWorld;
        Texture2D* image;
        TextLabel2DFixedWidth* label;
        TextLabel2DFixedWidth* selectedLabel;
        Point2D topLeftCorner;
        float baseSize;

        TextLabel2D* starTotalLabel;

        AnimationLerp<float> sizeAnim;
        AnimationMultiLerp<float> lockedAnim;

        DISALLOW_COPY_AND_ASSIGN(WorldSelectItem);  
    };

    std::vector<WorldSelectItem*> worldItems;
    int selectedItemIdx;
    bool goBackToMainMenu;

    bool itemActivated;
    AnimationLerp<float> goToLevelSelectMoveAnim;
    AnimationLerp<float> goToLevelSelectAlphaAnim;

    Texture2D* starTexture;
    
    CgFxGreyscale greyscaleEffect;

    void GoBackToMainMenu();
    void Init(int selectedIdx);

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