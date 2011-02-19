/**
 * SelectLevelMenuState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __SELECTLEVELMENUSTATE_H__
#define __SELECTLEVELMENUSTATE_H__

#include "DisplayState.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Point.h"

class FBObj;
class CgFxBloom;
class TextLabel2D;
class TextLabel2DFixedWidth;
class GameWorld;
class KeyboardHelperLabel;
class GameLevel;

class SelectLevelMenuState : public DisplayState {
public:
    SelectLevelMenuState(GameDisplay* display, const GameWorld* world);
    ~SelectLevelMenuState();

	void RenderFrame(double dT);
	void ButtonPressed(const GameControl::ActionButton& pressedButton);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
	void DisplaySizeChanged(int width, int height);
	DisplayState::DisplayStateType GetType() const;

private:
    static const int HORIZONTAL_TITLE_GAP = 20;
    static const int VERTICAL_TITLE_GAP   = 30;

    const GameWorld* world;
    TextLabel2D* worldLabel;
    TextLabel2D* totalNumStarsLabel;
    KeyboardHelperLabel* keyEscLabel;
    AnimationMultiLerp<float> pressEscAlphaAnim;

    AnimationLerp<float> fadeAnimation;

    // Animation of the selection indicator
    AnimationMultiLerp<float> selectionAlphaOrangeAnim;   
    AnimationMultiLerp<float> selectionAlphaYellowAnim;   
    AnimationMultiLerp<float> selectionBorderAddAnim; 

    Texture* starTexture;
	CgFxBloom* bloomEffect;
	FBObj* menuFBO;

    bool goBackToWorldSelectMenu;
    AnimationLerp<float> goBackMenuMoveAnim;
    AnimationLerp<float> goBackMenuAlphaAnim;

    bool goToStartLevel;    // true when the user has selected a level to play
    
    void DrawStarTotalLabel(const Camera& camera);
    void DrawTitleStrip(const Camera& camera) const;
    void DrawLevelSelectMenu(const Camera& camera, double dT);
    void GoBackToWorldSelectMenu();
    void GoToStartLevel();
    void SetupLevelItems();

    class LevelMenuItem {
    public:
        LevelMenuItem(int levelNum, const GameLevel* level, float width, const Point2D& topLeftCorner, const Texture* starTexture);
        ~LevelMenuItem();
        
        const Point2D& GetTopLeftCorner() const { return this->topLeftCorner; }
        const float GetWidth() const { return this->width; }
        float GetHeight() const;
        const GameLevel* GetLevel() const { return this->level; }
        bool GetIsEnabled() const { return this->isEnabled; }

        void Draw(const Camera& camera, double dT);

    private:
        static const float NUM_TO_NAME_GAP;
        static const float NUM_TO_HIGH_SCORE_Y_GAP;
        static const float HIGH_SCORE_TO_STAR_Y_GAP;
        Point2D topLeftCorner;
        float width;

        const GameLevel* level;
        bool isEnabled;
        bool isSelected;

        GLuint starDisplayList;
        float starSize;

        TextLabel2D* numLabel;
        TextLabel2DFixedWidth* nameLabel;
        TextLabel2D* highScoreLabel; 

        const Texture* starTexture;
        
        DISALLOW_COPY_AND_ASSIGN(LevelMenuItem);
    };

    int selectedItem;
    std::vector<LevelMenuItem*> levelItems;

    DISALLOW_COPY_AND_ASSIGN(SelectLevelMenuState);
};

inline void SelectLevelMenuState::DisplaySizeChanged(int width, int height) {
    UNUSED_PARAMETER(width);
    UNUSED_PARAMETER(height);
}

inline DisplayState::DisplayStateType SelectLevelMenuState::GetType() const {
    return DisplayState::SelectLevelMenu;
}

#endif // __SELECTLEVELMENUSTATE_H__