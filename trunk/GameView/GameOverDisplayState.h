/**
 * GameOverDisplayState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __GAMEOVERDISPLAYSTATE_H__
#define __GAMEOVERDISPLAYSTATE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/TextLabel.h"
#include "../BlammoEngine/Animation.h"

// GameSound Includes
#include "../GameSound/GameSound.h"

#include "DisplayState.h"
#include "InGameRenderPipeline.h"
#include "CgFxPostDeath.h"
#include "GameMenu.h"

class GameDisplay;

class GameOverDisplayState : public DisplayState {

public:
	GameOverDisplayState(GameDisplay* display);
	~GameOverDisplayState();

    bool AllowsGameModelUpdates() const { return true; }

	void RenderFrame(double dT);

	void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
    void MousePressed(const GameControl::MouseButton& pressedButton) { UNUSED_PARAMETER(pressedButton); };
    void MouseReleased(const GameControl::MouseButton& releasedButton) { UNUSED_PARAMETER(releasedButton); };

	void DisplaySizeChanged(int width, int height);
	DisplayState::DisplayStateType GetType() const;

private:
	static const char* GAME_OVER_TEXT;
    static const int GAME_OVER_LABEL_X_BORDER;
    static const int GAME_OVER_LABEL_Y_BORDER;

	static const Colour MENU_ITEM_IDLE_COLOUR;
	static const Colour MENU_ITEM_SEL_COLOUR;
	static const Colour MENU_ITEM_ACTIVE_COLOUR;
	static const Colour MENU_ITEM_GREYED_COLOUR;

    SoundID gameOverSoundID;

	TextLabel2D gameOverLabel;
    InGameRenderPipeline renderPipeline;
    CgFxPostDeath deathFSEffect;

    AnimationLerp<float> fastFadeAnim;
    AnimationLerp<float> fadeAnim;
    AnimationMultiLerp<float> moveMenuAnim;
    AnimationMultiLerp<float> moveLabelAnim;

    Texture2D* starryBG;

    float maxMenuItemWidth;
    float menuHeight;

	GameMenu* gameOverMenu;
    int retryMenuItem;
    int backToMainMenuItem;
    int exitGameItem;
	
    enum AnimationState { FadeToDeathState, ShowingTextState, IdleState, FadeOutState };
    AnimationState currState;
    int selectedAndActivatedItem;

    void UpdateAndDrawState(double dT);
    void SetState(const AnimationState& state);

    void SetupMenu();

	class TopMenuEventHandler : public GameMenuEventHandler {
	public:
		TopMenuEventHandler(GameOverDisplayState* state) : state(state) { assert(state != NULL); }
		~TopMenuEventHandler() {}
		void GameMenuItemHighlightedEvent(int itemIndex);
		void GameMenuItemActivatedEvent(int itemIndex);
		void GameMenuItemChangedEvent(int itemIndex);
        void EscMenu();
	private:
		GameOverDisplayState* state;
	};

    class QuitVerifyMenuEventHandler : public VerifyMenuEventHandlerWithSound {
    public:
        QuitVerifyMenuEventHandler(GameOverDisplayState* state);
        ~QuitVerifyMenuEventHandler() {}

        void MenuItemConfirmed();

    private:
        GameOverDisplayState* state;
    };

	TopMenuEventHandler* topMenuEventHandler;
    QuitVerifyMenuEventHandler* quitSubMenuEventHandler;

    DISALLOW_COPY_AND_ASSIGN(GameOverDisplayState);
};

inline void GameOverDisplayState::ButtonReleased(const GameControl::ActionButton& releasedButton) {
	UNUSED_PARAMETER(releasedButton);
}

inline void GameOverDisplayState::DisplaySizeChanged(int width, int height) {
	UNUSED_PARAMETER(width);
	UNUSED_PARAMETER(height);
}

inline DisplayState::DisplayStateType GameOverDisplayState::GetType() const {
	return DisplayState::GameOver;
}

#endif
