/**
 * SelectLevelMenuState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
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

#include "../ESPEngine/ESPMultiColourEffector.h"
#include "../ESPEngine/ESPPointEmitter.h"
#include "../ESPEngine/ESPVolumeEmitter.h"

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
    TextLabel2D* totalNumStarsLabel;
    KeyboardHelperLabel* keyEscLabel;
    AnimationMultiLerp<float> pressEscAlphaAnim;

    AnimationLerp<float> fadeAnimation;

    // Animation of the selection indicator
    AnimationMultiLerp<float> selectionAlphaOrangeAnim;   
    AnimationMultiLerp<float> selectionAlphaYellowAnim;   
    AnimationMultiLerp<float> selectionBorderAddAnim; 

    Texture* arrowTexture;
    Texture* starTexture;
    Texture* bossIconTexture;
    Texture2D* starryBG;
    Texture2D* padlockTexture;
	CgFxBloom* bloomEffect;
	FBObj* menuFBO;

    ESPVolumeEmitter* nextPgArrowEmitter;
    ESPMultiColourEffector nextArrowFader;
    ESPVolumeEmitter* prevPgArrowEmitter;
    ESPMultiColourEffector prevArrowFader;

    bool goBackToWorldSelectMenu;
    AnimationLerp<float> goBackMenuMoveAnim;
    AnimationLerp<float> goBackMenuAlphaAnim;

    bool goToStartLevel;    // true when the user has selected a level to play
    
    void DrawStarTotalLabel(const Camera& camera);
    void DrawTitleStrip(const Camera& camera) const;
    void DrawPageSelection(const Camera& camera) const;
    void DrawLevelSelectMenu(const Camera& camera, double dT);
    void GoBackToWorldSelectMenu();
    void GoToStartLevel();
    void SetupLevelPages();
    void ClearLevelPages();

    void MoveSelectionX(bool right);
    void MoveSelectionY(bool up);
    int GetNumItemsOnRow(int rowIdx);

    class AbstractLevelMenuItem {
    public:
        AbstractLevelMenuItem(SelectLevelMenuState* state, int levelNum, const GameLevel* level,
            float width, const Point2D& topLeftCorner, bool isEnabled);
        virtual ~AbstractLevelMenuItem();

        const Point2D& GetTopLeftCorner() const { return this->topLeftCorner; }
        const float GetWidth() const { return this->width; }
        const GameLevel* GetLevel() const { return this->level; }
        bool GetIsEnabled() const { return this->isEnabled; }
        virtual float GetHeight() const = 0;

        virtual void Draw(const Camera& camera, double dT, bool isSelected) = 0;

        void ExecuteLockedAnimation();

    protected:
        static const float NUM_TO_NAME_GAP;
        static const float DISABLED_GREY_AMT;
        static const float PADLOCK_SCALE;

        SelectLevelMenuState* state;

        Point2D topLeftCorner;
        float width;

        const GameLevel* level;
        bool isEnabled;

        TextLabel2D* numLabel;
        TextLabel2DFixedWidth* nameLabel;

        AnimationMultiLerp<float> lockedAnim;

        void DrawBG(bool isSelected);
        void DrawPadlock(double dT);

        DISALLOW_COPY_AND_ASSIGN(AbstractLevelMenuItem);
    };

    class LevelMenuItem : public AbstractLevelMenuItem {
    public:
        LevelMenuItem(SelectLevelMenuState* state, int levelNum, const GameLevel* level,
            float width, const Point2D& topLeftCorner, bool isEnabled, const Texture* starTexture);
        ~LevelMenuItem();
        
        void Draw(const Camera& camera, double dT, bool isSelected);
        float GetHeight() const;

    private:
        static const float NUM_TO_HIGH_SCORE_Y_GAP;
        static const float HIGH_SCORE_TO_STAR_Y_GAP;

        GLuint starDisplayList;
        float starSize;

        TextLabel2D* highScoreLabel; 

        const Texture* starTexture;
        
        DISALLOW_COPY_AND_ASSIGN(LevelMenuItem);
    };

    class BossLevelMenuItem : public AbstractLevelMenuItem {
    public:
        BossLevelMenuItem(SelectLevelMenuState* state, int levelNum, const GameLevel* level,
            float width, float height, const Point2D& topLeftCorner, bool isEnabled, const Texture* bossTexture);
        ~BossLevelMenuItem();

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

        DISALLOW_COPY_AND_ASSIGN(BossLevelMenuItem);
    };

    class LevelMenuPage {
    public:
        LevelMenuPage() : selectedItem(0) {}
        ~LevelMenuPage();
        
        size_t GetSelectedItemIndex() const { return this->selectedItem; }
        AbstractLevelMenuItem* GetSelectedItem() const { return this->levelItems[this->selectedItem]; }
        size_t GetNumLevelItems() const { return this->levelItems.size(); }

        void SetSelectedItemIndex(size_t idx) { assert(idx < this->levelItems.size()); this->selectedItem = idx; }
        void AddLevelItem(AbstractLevelMenuItem* item) { assert(item != NULL); this->levelItems.push_back(item); }
        void Draw(const Camera& camera, double dT);
        
        const AbstractLevelMenuItem* GetFirstItem() const { return this->levelItems.front(); }
        const AbstractLevelMenuItem* GetLastItem() const { return this->levelItems.back(); }

    private:
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