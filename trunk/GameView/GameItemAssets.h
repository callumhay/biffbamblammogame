#ifndef __GAMEITEMASSETS_H__
#define __GAMEITEMASSETS_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Texture.h"

#include "../GameModel/GameItem.h"

class Mesh;
class GameESPAssets;
class Camera;
class GameItemTimer;
class PointLight;



/**
 * Manages assets related to items - including dropping items and items that are already
 * activated in the game.
 */
class GameItemAssets {
private:
	typedef std::map<const GameItemTimer*, AnimationMultiLerp<float>> TimerScaleAnimationMap;

	Mesh* item;	// Item, picked up by the player paddle
	GameESPAssets* espAssets;	// Effect assets

	// Mapping of item names to their respective texture assets
	std::map<GameItem::ItemType, Texture*> itemTextures;
	std::map<GameItem::ItemType, Texture*> itemTimerTextures;
	std::map<GameItem::ItemType, Texture*> itemTimerFillerTextures;

	// Private helper load and unload functions for item assets
	bool LoadItemTextures();
	bool LoadItemMeshes();
	void UnloadItemTextures();
	void UnloadItemMeshes();

	/**
	 * Represents a single timer in the game HUD. This class is used privately
	 * to organize the HUD element's layout and draw it.
	 */
	class ItemTimerHUDElement {
		
	public:
		enum TimerState { TimerStarting, TimerRunning, TimerAlmostDone, TimerStopping, TimerDead };

		static const int TIMER_VERTICAL_SPACING = 5;

		ItemTimerHUDElement(GameItemAssets* itemAssets, const GameItemTimer* itemTimer);
		~ItemTimerHUDElement();

		void Draw(double dT, const Camera& camera, int x, int y, int width, int height);
	
		void StopTimer();

		int GetTextureWidth() const { return this->timerTexture->GetWidth(); }
		int GetTextureHeight() const { return this->timerTexture->GetHeight(); }

		bool IsTimerEnding() const { return (this->currState == TimerStopping) || this->IsDead(); }
		bool IsDead() const { return (this->currState == TimerDead); }

		const GameItem::ItemType GetItemType() const { return this->itemType; }

	private:
		static const double TIMER_ALMOST_DONE_PERCENTELAPSED;

		GameItemAssets* itemAssets;
		const GameItemTimer* itemTimer;	// Timer associated with this HUD timer element
		
		Colour timerColour;							// Cache of the timer colour to avoid looking it up each frame
		TimerState currState;						// Current state of the HUD/timer
		GameItem::ItemType itemType;		// Item type associated with this HUD timer element

		Texture* timerTexture;
		Texture* fillerTexture;

		AnimationMultiLerp<ColourRGBA> additiveColourAnimation;	// Any active additive colour animation for this
		AnimationMultiLerp<float> scaleAnimation;								// Any active scale animation for this

		void SetState(const TimerState& state);
		void Tick(double dT);

	};

	// Mapping of currently active timers to their respective HUD elements
	std::list<ItemTimerHUDElement*> activeItemTimers;

public:
	GameItemAssets(GameESPAssets* espAssets);
	~GameItemAssets();
	
	bool LoadItemAssets();
	void DrawItem(double dT, const Camera& camera, const GameItem& gameItem, 
		const PointLight& fgKeyLight, const PointLight& fgFillLight, const PointLight& ballLight) const;

	void DrawTimers(double dT, const Camera& camera);

	// On Event functions for signalling the start/stop of timers
	void TimerStarted(const GameItemTimer* timer);
	void TimerStopped(const GameItemTimer* timer);

	void ClearTimers();

};
#endif