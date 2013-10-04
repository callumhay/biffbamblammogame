/**
 * SelectWorldMenuState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __SELECTWORLDMENUSTATE_H__
#define __SELECTWORLDMENUSTATE_H__

#include "DisplayState.h"
#include "CgFxGreyscale.h"
#include "CgFxPostRefract.h"

#include "../BlammoEngine/Animation.h"
#include "../ESPEngine/ESP.h"
#include "../GameSound/SoundCommon.h"

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
    SelectWorldMenuState(GameDisplay* display, const DisplayStateInfo& info);
    virtual ~SelectWorldMenuState();

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

    static const float SELECTED_TO_UNSELECTED_MENU_ITEM_HORIZ_GAP;
    static const float UNSELECTED_TO_UNSELECTED_MENU_ITEM_HORIZ_GAP;

    static const float SELECTED_MENU_ITEM_SIZE;
    static const float UNSELECTED_MENU_ITEM_SIZE;

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
    FBObj* postMenuFBObj;

    Texture2D* starryBG;
    Texture2D* padlockTex;

    SoundID bgSoundLoopID;

    // Inner class for representing a selectable world item in the world select menu
    class WorldSelectItem {
    public:
        static const float PADLOCK_SCALE;

        WorldSelectItem(SelectWorldMenuState* state, const GameWorld* world, bool isLocked);
        ~WorldSelectItem();

        void SetIsSelected(bool isSelected);
        void Draw(const Camera& camera, double dT, int selectedWorldNum);
        void DrawSelectionBorder(const Camera& camera, double dT, float orangeAlpha,
            float yellowAlpha, float sizeAmt);

        const GameWorld* GetWorld() const { return this->gameWorld; }

        bool GetIsLocked() const { return this->isLocked; }
        void ExecuteLockedAnimation();
        void ExecuteUnlockAnimation();

        void SetLockOffset(const Vector2D& offset);

    private:
        SelectWorldMenuState* state;
        
        CgFxGreyscale greyscaleEffect;
        
        bool isSelected;
        bool isLocked;
        const GameWorld* gameWorld;
        Texture2D* image;

        TextLabel2DFixedWidth* unselectedLabel;
        TextLabel2DFixedWidth* selectedLabel;
        TextLabel2D* selectedStarTotalLabel;
        TextLabel2D* unselectedStarTotalLabel;

        AnimationLerp<float> sizeAnim;
        AnimationMultiLerp<float> lockedAnim;

        bool unlockAnimActive;
        AnimationLerp<float> lockFadeAnim;
        AnimationLerp<float> greyscaleFactorAnim;
        
        Vector2D lockOffset;

        void Unlock();
        float GetXPosition(const Camera& camera, int selectedWorldNum) const;

        DISALLOW_COPY_AND_ASSIGN(WorldSelectItem);  
    };

    // World unlock animation variables
    class WorldUnlockAnimationTracker {
    public:
        WorldUnlockAnimationTracker(SelectWorldMenuState* state, WorldSelectItem* worldItem);
        ~WorldUnlockAnimationTracker();

        bool AreControlsLocked() const;
        void Draw(const Camera& camera, double dT, const Texture2D& bgTexture);

    private:
        SelectWorldMenuState* state;
        WorldSelectItem* worldItem;

        double countdownMoveToLockedWorld;
        double countdownWaitToShake;
        double countdownWaitToEnergySuck;
        bool unlockAnimExecuted;
        AnimationMultiLerp<Vector2D> lockShakeAnim;

        Texture2D* debrisTex;
        Texture2D* sparkleTex;
        Texture2D* lensFlareTex;
        Texture2D* hugeExplosionTex;
        Texture2D* sphereNormalsTex;
        std::vector<Texture2D*> smokeTextures;

        CgFxPostRefract normalTexRefractEffect;

        ESPParticleColourEffector particleFader;
        ESPParticleColourEffector particleHalfFader;
        ESPParticleColourEffector particleFireFastColourFader;
        ESPParticleScaleEffector particleMediumShrink;
        ESPParticleScaleEffector particleSmallGrowth;
        ESPParticleScaleEffector particleMediumGrowth;
        ESPParticleScaleEffector particleSuperGrowth;
        ESPParticleRotateEffector smokeRotatorCW;
        ESPParticleRotateEffector smokeRotatorCCW;

        ESPPointEmitter* energySuckEmitter;
        ESPPointEmitter* bigExplosionEmitter;
        ESPPointEmitter* bigExplosionOnoEmitter;
        ESPPointEmitter* shockwaveEffect;
        ESPPointEmitter* fireSmokeEmitter1;
        ESPPointEmitter* fireSmokeEmitter2;
        ESPPointEmitter* debrisEmitter;
        
        DISALLOW_COPY_AND_ASSIGN(WorldUnlockAnimationTracker);
    };
    WorldUnlockAnimationTracker* worldUnlockAnim;

    std::vector<WorldSelectItem*> worldItems;
    int selectedItemIdx;
    bool goBackToMainMenu;

    bool itemActivated;
    AnimationLerp<float> goToLevelSelectMoveAnim;
    AnimationLerp<float> goToLevelSelectAlphaAnim;

    Texture2D* starTexture;

    void GoBackToMainMenu();
    void MoveToNextWorld();
    void MoveToPrevWorld();
    void Init(const DisplayStateInfo& info);

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