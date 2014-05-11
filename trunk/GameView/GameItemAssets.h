/**
 * GameItemAssets.h
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

#ifndef __GAMEITEMASSETS_H__
#define __GAMEITEMASSETS_H__

#include "../ResourceManager.h"
#include "../Blammopedia.h"

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Texture.h"

#include "../GameModel/GameItem.h"

#include "../GameSound/SoundCommon.h"

class GameSound;
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
	GameItemAssets(GameESPAssets* espAssets, GameSound* sound);
	~GameItemAssets();
	
	bool LoadItemAssets();
	void DrawItem(double dT, const Camera& camera, const GameItem& gameItem,
		const BasicPointLight& fgKeyLight, const BasicPointLight& fgFillLight, const BasicPointLight& ballLight) const;

	void DrawTimers(double dT, const Camera& camera, const GameModel& gameModel);

	// On Event functions for signaling the start/stop of timers
	void TimerStarted(const GameItemTimer* timer);
	void TimerStopped(const GameItemTimer* timer, const GameModel& gameModel, bool didExpire);

	void ClearTimers();

	Texture2D* GetItemTexture(const GameItem::ItemType& itemType) const;

private:
	typedef std::map<const GameItemTimer*, AnimationMultiLerp<float> > TimerScaleAnimationMap;

	Mesh* item;	                // Item, picked up by the player paddle
	GameESPAssets* espAssets;	// Effect assets
    GameSound* sound;           // Sound module

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

		static const int BETWEEN_TIMER_SPACING = 5;

		ItemTimerHUDElement(GameItemAssets* itemAssets, const GameItemTimer* itemTimer);
		~ItemTimerHUDElement();

		void Draw(double dT, const Camera& camera, const GameModel& gameModel, 
            int x, int y, int width, int height);
	
		void StopTimer(const GameModel& gameModel, bool didExpire);

		int GetTextureWidth() const { return this->timerTexture->GetWidth(); }
		int GetTextureHeight() const { return this->timerTexture->GetHeight(); }

		bool IsTimerEnding() const { return (this->currState == TimerStopping) || this->IsDead(); }
		bool IsDead() const { return (this->currState == TimerDead); }

		const GameItem::ItemType GetItemType() const { return this->itemType; }

	private:
		static const double TIMER_ALMOST_DONE_TIME_LEFT;

		GameItemAssets* itemAssets;
		const GameItemTimer* itemTimer;	// Timer associated with this HUD timer element
		
		Colour timerColour;             // Cache of the timer colour to avoid looking it up each frame
		TimerState currState;           // Current state of the HUD/timer
		GameItem::ItemType itemType;    // Item type associated with this HUD timer element

		Texture* timerTexture;
		Texture* fillerTexture;

		AnimationMultiLerp<ColourRGBA> additiveColourAnimation;  // Any active additive colour animation for this
		AnimationMultiLerp<float> scaleAnimation;                // Any active scale animation for this

		void SetState(const TimerState& state);
		void Tick(double dT, const GameModel& gameModel);
	};

	// Mapping of currently active timers to their respective HUD elements
	std::list<ItemTimerHUDElement*> activeItemTimers;
    
    // Used to limit the number of "timer almost done" sound loops playing to one
    SoundID timerAlmostDoneSoundID;
    std::set<const ItemTimerHUDElement*> timersAlmostDone;
    void AddTimerAlmostDone(const ItemTimerHUDElement* timer);
    void RemoveTimerAlmostDone(const ItemTimerHUDElement* timer);

	DISALLOW_COPY_AND_ASSIGN(GameItemAssets);
};

inline Texture2D* GameItemAssets::GetItemTexture(const GameItem::ItemType& itemType) const {
	Blammopedia* blammopedia = ResourceManager::GetInstance()->GetBlammopedia();
	assert(blammopedia != NULL);
	Blammopedia::ItemEntry* entry = blammopedia->GetItemEntry(itemType);
	return entry->GetItemTexture();
}

#endif