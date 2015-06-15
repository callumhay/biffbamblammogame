/**
 * SelectWorldMenuState.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
    SelectWorldMenuState(GameDisplay* display, const DisplayStateInfo& info, SoundID bgSoundLoopID = INVALID_SOUND_ID);
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
    static const int HORIZONTAL_TITLE_GAP = 60;
    static const int VERTICAL_TITLE_GAP   = 30;

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

    TextLabel2D* totalNumGameStarsLabel;
    TextLabel2D* totalLabel;
    AnimationMultiLerp<float> totalGameStarGlowPulseAnim;
    Texture2D* starTexture;
    Texture2D* starGlowTexture;

	FBObj* menuFBO;
    FBObj* postMenuFBObj;

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

        SoundID worldUnlockSoundID;

        ESPParticleColourEffector particleFader;
        ESPParticleColourEffector particleHalfFader;
        ESPParticleColourEffector particleFireFastColourFader;
        ESPParticleScaleEffector particleMediumShrink;
        ESPParticleScaleEffector particleSmallGrowth;
        ESPParticleScaleEffector particleMediumGrowth;
        ESPParticleScaleEffector particleSuperGrowth;
        ESPParticleRotateEffector smokeRotatorCW;
        ESPParticleRotateEffector smokeRotatorCCW;

        ESPPointEmitter* timedEnergySuckEmitter;
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
    AnimationMultiLerp<float> arcadeFlashAnim;


    void DrawStarTotal(double dT);

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