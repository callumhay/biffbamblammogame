/**
 * SelectLevelMenuState.h
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

#ifndef __SELECTLEVELMENUSTATE_H__
#define __SELECTLEVELMENUSTATE_H__

#include "DisplayState.h"
#include "CgFxPostRefract.h"

#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Point.h"

#include "../ESPEngine/ESP.h"

#include "../GameSound/SoundCommon.h"

class FBObj;
class TextLabel2D;
class TextLabel2DFixedWidth;
class GameWorld;
class KeyboardHelperLabel;
class GameLevel;

class SelectLevelMenuState : public DisplayState {
public:
    SelectLevelMenuState(GameDisplay* display, const DisplayStateInfo& info, SoundID bgSoundLoopID = INVALID_SOUND_ID);
    ~SelectLevelMenuState();

    bool AllowsGameModelUpdates() const { return true; }

	void RenderFrame(double dT);

	void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
    void MousePressed(const GameControl::MouseButton& pressedButton) { UNUSED_PARAMETER(pressedButton); };
    void MouseReleased(const GameControl::MouseButton& releasedButton) { UNUSED_PARAMETER(releasedButton); };

	void DisplaySizeChanged(int width, int height);

    void LevelSelectionChanged();
    void PageSelectionChanged();

	DisplayState::DisplayStateType GetType() const;

private:
    static const int HORIZONTAL_TITLE_GAP = 60;
    static const int VERTICAL_TITLE_GAP   = 30;

    const GameWorld* world;
    TextLabel2D* worldLabel;
    TextLabel2D* totalNumWorldStarsLabel;
    TextLabel2D* totalNumGameStarsLabel;
    TextLabel2D* totalLabel;
    KeyboardHelperLabel* keyEscLabel;
    AnimationMultiLerp<float> pressEscAlphaAnim;
    AnimationMultiLerp<float> totalGameStarGlowPulseAnim;
    AnimationMultiLerp<float> fadeAnimation;
    AnimationMultiLerp<Colour> starGlowColourAnim;

    // Animation of the selection indicator
    AnimationMultiLerp<float> selectionAlphaOrangeAnim;   
    AnimationMultiLerp<float> selectionAlphaYellowAnim;   
    AnimationMultiLerp<float> selectionBorderAddAnim; 

    Texture* arrowTexture;
    Texture* starTexture;
    Texture* bossIconTexture;
    Texture* starGlowTexture;

    Texture2D* padlockTexture;
    Texture2D* padlockBrokenLeftTexture;
    Texture2D* padlockBrokenRightTexture;
    Texture2D* snapTexture;
	
    FBObj* menuFBO;
    FBObj* postMenuFBObj;

    SoundID bgSoundLoopID;

    ESPVolumeEmitter* nextPgArrowEmitter;
    ESPMultiColourEffector nextArrowFader;
    ESPVolumeEmitter* prevPgArrowEmitter;
    ESPMultiColourEffector prevArrowFader;

    ESPPointEmitter* fiveStarFGSparkleEmitter;
    ESPPointEmitter* fiveStarBGGlowEmitter;

    Texture2D* explosionTex;
    Texture2D* sphereNormalsTex;
    std::vector<Texture2D*> smokeTextures;
    CgFxPostRefract normalTexRefractEffect;
    ESPParticleColourEffector particleFader;
    ESPAnimatedAlphaEffector particleFlickerFader;
    ESPParticleAccelEffector lockBreakGravityEffector;

    ESPParticleColourEffector particleFireFastColourFader;
    ESPParticleScaleEffector particleSmallGrowth;
    ESPParticleScaleEffector particleMediumGrowth;
    ESPParticleScaleEffector particleSuperGrowth;
    ESPParticleRotateEffector smokeRotatorCW;
    ESPParticleRotateEffector smokeRotatorCCW;

    bool goBackToWorldSelectMenu;
    AnimationLerp<float> goBackMenuMoveAnim;
    AnimationLerp<float> goBackMenuAlphaAnim;

    bool goToStartLevel;    // true when the user has selected a level to play
    
    bool freezePlayerInput;
    bool levelWasUnlockedViaStarCost;
    bool playAutoStarUnlockAnim;
    bool playAutoBasicUnlockAnim;
    double autoUnlockAnimCountdown;

    void DrawStarTotalLabels(const Camera& camera, double dT);
    void DrawTitleStrip(const Camera& camera) const;
    void DrawPageSelection(const Camera& camera) const;
    void DrawLevelSelectMenu(const Camera& camera, double dT);
    void GoBackToWorldSelectMenu();
    void GoToStartLevel();
    void SetupLevelPages(const DisplayStateInfo& info);
    void ClearLevelPages();

    void MoveSelectionX(bool right);
    void MoveSelectionY(bool up);
    int GetNumItemsOnRow(int rowIdx);

    class AbstractLevelMenuItem {
    public:
        AbstractLevelMenuItem(SelectLevelMenuState* state, GameLevel* level,
            float width, const Point2D& topLeftCorner, bool isEnabled);
        virtual ~AbstractLevelMenuItem();

        virtual void RebuildItem(bool enabled, const Point2D& topLeftCorner);
        virtual float GetHeight() const = 0;

        virtual void Draw(const Camera& camera, double dT, bool isSelected) = 0;
        virtual void DrawAfter(const Camera& camera, double dT) = 0;

        const Point2D& GetTopLeftCorner() const { return this->topLeftCorner; }
        const float GetWidth() const { return this->width; }
        GameLevel* GetLevel() const { return this->level; }
        bool GetIsEnabled() const { return this->isEnabled; }
        void SetEnabled(bool enabled) {
            if (this->isEnabled != enabled) {
                this->RebuildItem(enabled, this->topLeftCorner);
            }
        }

        void ExecuteLockedAnimation();
        void ExecuteUnlockStarsPaidForAnimation();
        void ExecuteBasicUnlockAnimation();

    protected:
        static const float NUM_TO_NAME_GAP;
        static const float DISABLED_GREY_AMT;
        static const float PADLOCK_SCALE;
        static const float STAR_LOCKED_PADLOCK_SCALE;

        SelectLevelMenuState* state;

        Point2D topLeftCorner;
        float width;

        GameLevel* level;
        bool isEnabled;

        TextLabel2DFixedWidth* nameLabel;

        AnimationMultiLerp<float> lockedAnim;

        TextLabel2D* unlockNumStarsLabel;
        AnimationMultiLerp<Colour>* unlockStarColourAnim;
        AnimationMultiLerp<float>* unlockStarGlowPulseAnim;
        
        // Animations specific to unlocking the star cost
        static const double TOTAL_STAR_UNLOCK_TIME;
        bool isStarUnlockAnimPlaying;
        AnimationMultiLerp<float>* starShrinkAnim;
        AnimationLerp<float>* starAlphaAnim;
        AnimationMultiLerp<float>* lockShakeRotateAnim;
        AnimationMultiLerp<Vector2D>* lockShakeTranslateAnim;
        AnimationLerp<float>* lockShrinkAnim;
        AnimationLerp<float>* starLockFadeAnim;

        ESPPointEmitter* explosionEmitter;
        ESPPointEmitter* explosionOnoEmitter;
        ESPPointEmitter* shockwaveEffect;
        ESPPointEmitter* fireSmokeEmitter1;
        ESPPointEmitter* fireSmokeEmitter2;

        // Animations specific to basic unlocking
        bool isBasicUnlockAnimPlaying;
        ESPPointEmitter* lockSnapEmitter;
        ESPPointEmitter* lockSnapOnoEmitter;
        ESPPointEmitter* leftLockHalfEmitter;
        ESPPointEmitter* rightLockHalfEmitter;

        float GetUnlockStarSize() const;
        float GetUnlockStarCenterYOffset() const;

        void DrawBG(bool isSelected);
        void DrawPadlock(double dT, const Camera& camera);

        DISALLOW_COPY_AND_ASSIGN(AbstractLevelMenuItem);
    };

    class LevelMenuItem : public AbstractLevelMenuItem {
    public:
        LevelMenuItem(SelectLevelMenuState* state, int levelNum, GameLevel* level,
            float width, const Point2D& topLeftCorner, bool isEnabled);
        ~LevelMenuItem();
        
        void RebuildItem(bool enabled, const Point2D& topLeftCorner);

        void Draw(const Camera& camera, double dT, bool isSelected);
        void DrawAfter(const Camera& camera, double dT);
        float GetHeight() const;

    private:
        static const float NUM_TO_HIGH_SCORE_Y_GAP;
        static const float HIGH_SCORE_TO_STAR_Y_GAP;
        static const float STAR_GAP;

        GLuint activeStarDisplayList;
        GLuint inactiveStarDisplayList;
        float starSize;
        Point2D starDrawPos;
        int numStars;

        TextLabel2D* numLabel;
        TextLabel2D* highScoreLabel;

        void BuildStarDisplayList();

        DISALLOW_COPY_AND_ASSIGN(LevelMenuItem);
    };

    class BossLevelMenuItem : public AbstractLevelMenuItem {
    public:
        BossLevelMenuItem(SelectLevelMenuState* state, GameLevel* level,
            float width, float height, const Point2D& topLeftCorner, bool isEnabled, const Texture* bossTexture);
        ~BossLevelMenuItem();

        void RebuildItem(bool enabled, const Point2D& topLeftCorner);

        void Draw(const Camera& camera, double dT, bool isSelected);
        void DrawAfter(const Camera& camera, double dT);
        float GetHeight() const;

    private:
        static const float BOSS_ICON_GAP;
        static const float NUM_TO_BOSS_NAME_GAP;
        static const float BOSS_NAME_ICON_GAP;

        GLuint bossIconDisplayList;
        float bossIconSize;
        float height;

        TextLabel2D* bossLabel;
        const Texture* bossTexture;
        TextLabel2D* bossDeadLabel;

        void BuildBossIconDisplayList();

        DISALLOW_COPY_AND_ASSIGN(BossLevelMenuItem);
    };

    class LevelMenuPage {
    public:
        LevelMenuPage() : selectedItem(0), numRows(0) {}
        ~LevelMenuPage();
        
        size_t GetSelectedItemIndex() const { return this->selectedItem; }
        AbstractLevelMenuItem* GetSelectedItem() const { return this->levelItems[this->selectedItem]; }
        AbstractLevelMenuItem* GetItemAt(size_t idx) { return this->levelItems[idx]; }
        size_t GetNumLevelItems() const { return this->levelItems.size(); }
        void SetNumRows(int numRows) { this->numRows = numRows; }
        int GetNumRows() const { return this->numRows; }

        void SetSelectedItemIndex(size_t idx) { assert(idx < this->levelItems.size()); this->selectedItem = idx; }
        void AddLevelItem(AbstractLevelMenuItem* item) { 
            assert(item != NULL); 
            this->levelItems.push_back(item); 
        }
        void Draw(const Camera& camera, double dT);
        
        const AbstractLevelMenuItem* GetFirstItem() const { return this->levelItems.front(); }
        const AbstractLevelMenuItem* GetLastItem() const { return this->levelItems.back(); }

    private:
        int numRows;
        size_t selectedItem;
        std::vector<AbstractLevelMenuItem*> levelItems;
    };

    int numItemsPerRow;
    
    int selectedPage;
    std::vector<LevelMenuPage*> pages;

    DISALLOW_COPY_AND_ASSIGN(SelectLevelMenuState);
};

inline void SelectLevelMenuState::DisplaySizeChanged(int width, int height) {
    UNUSED_PARAMETER(width);
    UNUSED_PARAMETER(height);
}

inline DisplayState::DisplayStateType SelectLevelMenuState::GetType() const {
    return DisplayState::SelectLevelMenu;
}

inline void SelectLevelMenuState::LevelMenuPage::Draw(const Camera& camera, double dT) {
    for (size_t i = 0; i < this->levelItems.size(); i++) {
        this->levelItems[i]->Draw(camera, dT, this->selectedItem == i);
    }
    for (size_t i = 0; i < this->levelItems.size(); i++) {
        this->levelItems[i]->DrawAfter(camera, dT);
    }
}

#endif // __SELECTLEVELMENUSTATE_H__