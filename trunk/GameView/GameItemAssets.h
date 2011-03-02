/**
 * GameItemAssets.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __GAMEITEMASSETS_H__
#define __GAMEITEMASSETS_H__

#include "../ResourceManager.h"
#include "../Blammopedia.h"

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Texture.h"

#include "../GameModel/GameItem.h"

class Mesh;
class GameESPAssets;
class Camera;
class GameItemTimer;
class BasicPointLight;

/**
 * Manages assets related to items - including dropping items and items that are already
 * activated in the game.
 */
class GameItemAssets {
public:
	GameItemAssets(GameESPAssets* espAssets);
	~GameItemAssets();
	
	bool LoadItemAssets();
	void DrawItem(double dT, const Camera& camera, const GameItem& gameItem, 
		const BasicPointLight& fgKeyLight, const BasicPointLight& fgFillLight, const BasicPointLight& ballLight) const;

	void DrawTimers(double dT, const Camera& camera);

	// On Event functions for signalling the start/stop of timers
	void TimerStarted(const GameItemTimer* timer);
	void TimerStopped(const GameItemTimer* timer);

	void ClearTimers();

	Texture2D* GetItemTexture(const GameItem::ItemType& itemType) const;

private:
	typedef std::map<const GameItemTimer*, AnimationMultiLerp<float> > TimerScaleAnimationMap;

	Mesh* item;	// Item, picked up by the player paddle
	GameESPAssets* espAssets;	// Effect assets

	// Private helper load and unload functions for item assets
	bool LoadItemMeshes();
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

	DISALLOW_COPY_AND_ASSIGN(GameItemAssets);
};

inline Texture2D* GameItemAssets::GetItemTexture(const GameItem::ItemType& itemType) const {
	Blammopedia* blammopedia = ResourceManager::GetInstance()->GetBlammopedia();
	assert(blammopedia != NULL);
	Blammopedia::ItemEntry* entry = blammopedia->GetItemEntry(itemType);
	return entry->GetItemTexture();
}

#endif