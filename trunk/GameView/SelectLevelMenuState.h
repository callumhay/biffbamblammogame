/**
 * SelectLevelMenuState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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

    // Animation of the selection indicator
    AnimationMultiLerp<float> selectionAlphaOrangeAnim;   
    AnimationMultiLerp<float> selectionAlphaYellowAnim;   
    AnimationMultiLerp<float> selectionBorderAddAnim; 

    Texture* arrowTexture;
    Texture* starTexture;
    Texture* bossIconTexture;
    Texture* starGlowTexture;

    Texture2D* padlockTexture;
	
    FBObj* menuFBO;
    FBObj* postMenuFBObj;

    SoundID bgSoundLoopID;

    ESPVolumeEmitter* nextPgArrowEmitter;
    ESPMultiColourEffector nextArrowFader;
    ESPVolumeEmitter* prevPgArrowEmitter;
    ESPMultiColourEffector prevArrowFader;

    Texture2D* explosionTex;
    Texture2D* sphereNormalsTex;
    std::vector<Texture2D*> smokeTextures;
    CgFxPostRefract normalTexRefractEffect;
    ESPParticleColourEffector particleFader;

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
    bool playAutoUnlockAnim;
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

        const Point2D& GetTopLeftCorner() const { return this->topLeftCorner; }
        const float GetWidth() const { return this->width; }
        GameLevel* GetLevel() const { return this->level; }
        bool GetIsEnabled() const { return this->isEnabled; }

        void ExecuteLockedAnimation();
        void ExecuteUnlockStarsPaidForAnimation();

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
        bool isUnlockAnimPlaying;
        AnimationMultiLerp<float>* starShrinkAnim;
        AnimationLerp<float>* starAlphaAnim;
        AnimationMultiLerp<float>* lockShakeRotateAnim;
        AnimationMultiLerp<Vector2D>* lockShakeTranslateAnim;
        AnimationLerp<float>* lockShrinkAnim;
        AnimationLerp<float>* lockFadeAnim;

        ESPPointEmitter* explosionEmitter;
        ESPPointEmitter* explosionOnoEmitter;
        ESPPointEmitter* shockwaveEffect;
        ESPPointEmitter* fireSmokeEmitter1;
        ESPPointEmitter* fireSmokeEmitter2;

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
        float GetHeight() const;

    private:
        static const float NUM_TO_HIGH_SCORE_Y_GAP;
        static const float HIGH_SCORE_TO_STAR_Y_GAP;

        GLuint starDisplayList;
        float starSize;

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
}

#endif // __SELECTLEVELMENUSTATE_H__