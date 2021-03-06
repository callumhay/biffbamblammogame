/**
 * GameItemAssets.cpp
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

#include "GameItemAssets.h"
#include "GameViewConstants.h"
#include "GameESPAssets.h"

// Blammo Engine Includes
#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Mesh.h"

// Game Model Includes
#include "../GameModel/GameItem.h"
#include "../GameModel/GameItemTimer.h"
#include "../GameModel/RandomItem.h"

// Game Sound Includes
#include "../GameSound/GameSound.h"

#include "../ResourceManager.h"

GameItemAssets::GameItemAssets(GameESPAssets* espAssets, GameSound* sound) : 
espAssets(espAssets), sound(sound), item(NULL), timerAlmostDoneSoundID(INVALID_SOUND_ID) {
}

GameItemAssets::~GameItemAssets() {
	// Unload all the item assets...
	this->UnloadItemMeshes();
	// Delete all active HUD timers...
	this->ClearTimers();

    bool success = false;
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->item);
    assert(success);
    UNUSED_VARIABLE(success);
}

/**
 * Private helper function for unloading item-related mesh assets.
 */
void GameItemAssets::UnloadItemMeshes() {
	if (this->item != NULL) {
		// Make sure there is not texture assoc with item or we will delete it twice!!
		this->item->SetTextureForMaterial(GameViewConstants::GetInstance()->ITEM_LABEL_MATGRP, NULL);
	}
}

/**
 * Private helper function for loading item-related mesh assets.
 * Returns: true on successful load, false otherwise.
 */
bool GameItemAssets::LoadItemMeshes() {
		if (this->item == NULL) {
			// Load the mesh for items
			this->item = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->ITEM_MESH);
		}

		if (this->item == NULL) {
			return false;
		}
		return true;
}

/**
 * Public function for loading all the item assets.
 * Returns: true on successful load, false otherwise.
 */
bool GameItemAssets::LoadItemAssets() {
	bool DidLoadMeshes	 = this->LoadItemMeshes();
	return DidLoadMeshes;
}

/**
 * Draw the given item as a item drop in-game.
 */
void GameItemAssets::DrawItem(double dT, const Camera& camera, const GameItem& gameItem,
                              const BasicPointLight& fgKeyLight, const BasicPointLight& fgFillLight, 
                              const BasicPointLight& ballLight) const  {

	// Set material for the image based on the item name/type
	GameItem::ItemType itemType	= gameItem.GetItemType();
    GameItem::ItemDisposition itemDisposition = gameItem.GetItemDisposition();
    if (itemType == GameItem::RandomItem) {
        assert(dynamic_cast<const RandomItem*>(&gameItem) != NULL);
        const RandomItem& randomItem = static_cast<const RandomItem&>(gameItem);
        
        itemType = randomItem.GetBlinkingRandomItemType();
        itemDisposition = GameItemFactory::GetInstance()->GetItemTypeDisposition(itemType);

    }

	Texture2D* itemTexture = this->GetItemTexture(itemType);
	this->item->SetTextureForMaterial(GameViewConstants::GetInstance()->ITEM_LABEL_MATGRP, itemTexture);
	
    const Colour& itemEndColour = GameViewConstants::GetInstance()->GetItemColourFromDisposition(itemDisposition);
	this->item->SetColourForMaterial(GameViewConstants::GetInstance()->ITEM_END_MATGRP, itemEndColour);
	
	glPushMatrix();
	const Point2D& center = gameItem.GetCenter();
	glTranslatef(center[0], center[1], 0.0f);
    glRotatef(gameItem.GetRotationInDegs(), 0, 0, 1);

	// Draw the item's effect and the item itself
	this->espAssets->DrawItemDropEffects(dT, camera, gameItem);
	
	// Set the colour multiply for the item - this allows the model to control the colour
	// and transparency of the model based on the state of the game
	const ColourRGBA& itemColourMultiply = gameItem.GetItemColour();
	glColor4f(itemColourMultiply.R(), itemColourMultiply.G(), itemColourMultiply.B(), itemColourMultiply.A());	
	this->item->Draw(camera, fgKeyLight, fgFillLight, ballLight);
	glPopMatrix();
}

/**
 * Draw the HUD timer for the given timer type.
 */
void GameItemAssets::DrawTimers(double dT, const Camera& camera, const GameModel& gameModel) {

	// If there are no timers to draw then we don't draw any
	if (this->activeItemTimers.size() == 0) {
		return;
	}

	const int displayWidth = camera.GetWindowWidth();
	//const int displayHeight = camera.GetWindowHeight();

	// Spacing along the vertical between timer graphics (in pixels)
	// Spacing along the horizontal of timer graphics distanced from the right edge of the game window (in pixels)
 	const int TIMER_BORDER_SPACING = 8;

	// There is only a max restriction on height, width follows based on the timer texture's width-to-height ratio
	const int MAX_TIMER_SIZE_SPACED = 80;
	// The timer size including spacing (includes spaces of BETWEEN_TIMER_SPACING at the bottom of timer)
	const int TIMER_SIZE_SPACED     = std::min<int>(MAX_TIMER_SIZE_SPACED, displayWidth / static_cast<int>(this->activeItemTimers.size()));
	const int TIMER_SIZE_NON_SPACED = TIMER_SIZE_SPACED - ItemTimerHUDElement::BETWEEN_TIMER_SPACING;

	// Tracked width at which to draw the next timer graphic
	int currXPos = displayWidth - ((displayWidth - (static_cast<int>(this->activeItemTimers.size()) * TIMER_SIZE_NON_SPACED) - 
        (static_cast<int>(this->activeItemTimers.size()-1) * ItemTimerHUDElement::BETWEEN_TIMER_SPACING)) / 2) - TIMER_SIZE_NON_SPACED/2;

	// Prepare OGL for drawing the timer
	glPushAttrib(GL_TEXTURE_BIT | GL_LIST_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	
	// Make world coordinates equal window coordinates
	Camera::PushWindowCoords();
	
	// Set the OGL state to properly draw the timers
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	for (std::list<ItemTimerHUDElement*>::iterator iter = this->activeItemTimers.begin(); iter != this->activeItemTimers.end(); ++iter) {
		ItemTimerHUDElement* currHUDElement = *iter;
		assert(currHUDElement != NULL);

		// Calculate the width and height as well as the x, y location of the HUD element for drawing
		int width  = TIMER_SIZE_NON_SPACED;
        int height = width * currHUDElement->GetTextureHeight() / currHUDElement->GetTextureWidth();
        int halfWidth = width / 2;

        currXPos -= halfWidth;
		currHUDElement->Draw(dT, camera, gameModel, currXPos, TIMER_BORDER_SPACING, width, height);
		currXPos -= halfWidth + ItemTimerHUDElement::BETWEEN_TIMER_SPACING;
	}

	glPopMatrix();

	// Pop the projection matrix
	Camera::PopWindowCoords();
	// Restore the OGL state
	glPopAttrib();
	
	// Remove any dead HUD elements...
	for (std::list<ItemTimerHUDElement*>::iterator iter = this->activeItemTimers.begin(); iter != this->activeItemTimers.end();) {
		ItemTimerHUDElement* timerHUDElement = *iter;
		if (timerHUDElement->IsDead()) {
			iter = this->activeItemTimers.erase(iter);
			delete timerHUDElement;
			timerHUDElement = NULL;
		}
		else {
			++iter;
		}
	}
}

/**
 * Signal to the item assets that a timer has started for the first time.
 */
void GameItemAssets::TimerStarted(const GameItemTimer* timer) {

	// We need to check to make sure that the timer being added doesn't already
	// exist (i.e., the same item isn't already in the HUD), if it is it must be replaced completely
	for (std::list<ItemTimerHUDElement*>::iterator iter = this->activeItemTimers.begin(); iter != this->activeItemTimers.end();) {
		ItemTimerHUDElement* timerHUDElement = *iter;
		GameItem::ItemType itemType = timerHUDElement->GetItemType();

		if (itemType == timer->GetTimerItemType()) {
			// If we're replacing an item then we should completely remove it and replace it
			iter = this->activeItemTimers.erase(iter);
			delete timerHUDElement;
			timerHUDElement = NULL;
		}
		else {
			++iter;
		}
	}

	// Add the new timer HUD element
	ItemTimerHUDElement* timerHUDElement = new ItemTimerHUDElement(this, timer);
	this->activeItemTimers.push_back(timerHUDElement);
}

/**
 * Signal to the item assets that a timer has stopped / is being destroyed.
 */
void GameItemAssets::TimerStopped(const GameItemTimer* timer, const GameModel& gameModel, bool didExpire) {
	// Find the timer HUD element based on the stopped timer,
	// stop the HUD element (this will trigger whatever stop animations, procedures are
	// required) and remove it from the mapping
	for (std::list<ItemTimerHUDElement*>::iterator iter = this->activeItemTimers.begin(); iter != this->activeItemTimers.end(); ++iter) {
		ItemTimerHUDElement* timerHUDElement = *iter;
		GameItem::ItemType itemType = timerHUDElement->GetItemType();

		if (itemType == timer->GetTimerItemType()) {
			timerHUDElement->StopTimer(gameModel, didExpire);
		}
	}
}

void GameItemAssets::ClearTimers() {
	// Delete all active HUD timers...
	for (std::list<ItemTimerHUDElement*>::iterator iter = this->activeItemTimers.begin(); iter != this->activeItemTimers.end(); ++iter) {
		ItemTimerHUDElement* hudElement = *iter;
		delete hudElement;
		hudElement = NULL;
	}
	this->activeItemTimers.clear();
}


// ItemTimerHUDElement Private Inner Class Functions *************************************************
// ***************************************************************************************************

// Amount of time in seconds that starts the 'almost done' state of the HUD
const double GameItemAssets::ItemTimerHUDElement::TIMER_ALMOST_DONE_TIME_LEFT = 3.0;	

GameItemAssets::ItemTimerHUDElement::ItemTimerHUDElement(GameItemAssets* itemAssets, 
                                                         const GameItemTimer* itemTimer) : 
itemTimer(itemTimer), timerTexture(NULL), fillerTexture(NULL), itemAssets(itemAssets) {
	
	assert(itemAssets != NULL);
	assert(itemTimer != NULL);

	this->itemType = itemTimer->GetTimerItemType();

	// Figure out what colour to make the fill based on how it
	// affects the player (red is bad, green is good, etc.)
    this->timerColour = GameViewConstants::GetInstance()->GetItemColourFromDisposition(itemTimer->GetTimerDisposition());

	// Check to see if a timer textures exist... if they don't we have a serious problem
	Blammopedia* blammopedia = ResourceManager::GetInstance()->GetBlammopedia();
	assert(blammopedia != NULL);

	// Find the item entry for the current item type...
	const Blammopedia::ItemEntry* itemEntry = blammopedia->GetItemEntry(itemTimer->GetTimerItemType());
	assert(itemEntry != NULL);

	this->timerTexture  = itemEntry->GetHUDOutlineTexture();
	this->fillerTexture = itemEntry->GetHUDFillTexture();
	assert(this->timerTexture != NULL);
	assert(this->fillerTexture != NULL);

	this->SetState(ItemTimerHUDElement::TimerStarting);
}

GameItemAssets::ItemTimerHUDElement::~ItemTimerHUDElement() {
    this->itemAssets->RemoveTimerAlmostDone(this);
}

void GameItemAssets::ItemTimerHUDElement::Tick(double dT, const GameModel& gameModel) {
	// Based on the current state of the HUD element we animate and activate various
	// functionality and visual cues...
	switch (this->currState) {
		
		case ItemTimerHUDElement::TimerStarting: {
				
				// Tick all the animations...
				bool colourAnimationDone = this->additiveColourAnimation.Tick(dT);
				bool scaleAnimationDone  = this->scaleAnimation.Tick(dT);

				// If the animations are done then we're done with this state too...
				if (colourAnimationDone && scaleAnimationDone) {
					this->SetState(ItemTimerHUDElement::TimerRunning);
				}
			}
			break;

		case ItemTimerHUDElement::TimerRunning: {
				// Tick all the animations, these should be in a loop while running...
				this->additiveColourAnimation.Tick(dT);
				this->scaleAnimation.Tick(dT);

				if (this->itemTimer->GetTimeLeft() <= ItemTimerHUDElement::TIMER_ALMOST_DONE_TIME_LEFT) {
					this->SetState(ItemTimerHUDElement::TimerAlmostDone);

                    if (GameState::IsGameInPlayState(gameModel)) {
                        // Start playing the loop for the timer almost being done...
                        this->itemAssets->AddTimerAlmostDone(this);
                    }
				}
			}
			break;

		case ItemTimerHUDElement::TimerAlmostDone:
			// Tick all the animations, these should be in a loop while running...
			this->additiveColourAnimation.Tick(dT);
			this->scaleAnimation.Tick(dT);
			break;

		case ItemTimerHUDElement::TimerStopping: {
				
				// Tick all the animations...
				bool colourAnimationDone = this->additiveColourAnimation.Tick(dT);
				bool scaleAnimationDone  = this->scaleAnimation.Tick(dT);

				// If the animations are done then we're done with this state too...
				if (colourAnimationDone && scaleAnimationDone) {
					this->SetState(ItemTimerHUDElement::TimerDead);
				}
			}
			break;

		case ItemTimerHUDElement::TimerDead:
			// Do nothing, the timer is le dead.
			break;

		default:
			assert(false);
			break;
	}
}

/**
 * Draw the Item HUD element in its current state.
 */
void GameItemAssets::ItemTimerHUDElement::Draw(double dT, const Camera& camera, const GameModel& gameModel, 
                                               int x, int y, int width, int height) {
	this->Tick(dT, gameModel);

	const float& ITEM_SCALE = this->scaleAnimation.GetInterpolantValue();
	const ColourRGBA& ITEM_ADDITIVE_COLOUR = this->additiveColourAnimation.GetInterpolantValue();
	const int HALF_WIDTH	 = width / 2;
	const int HALF_HEIGHT	 = height / 2;
	const int TRANSLATE_X	 = x + HALF_WIDTH;
	
	// Draw the filler for the timer (how much has elapsed)
	double percentElapsed = 1.0;
	if (this->itemTimer != NULL) {
		percentElapsed = this->itemTimer->GetPercentTimeElapsed();
	}

	double fillerHalfHeight = (HALF_HEIGHT - (HALF_HEIGHT * percentElapsed)) ;
	double fillerTexHeight  = (1.0 - percentElapsed);
	
	// Back fill - so that the filler that has expired so far stands out from the background
	glPushMatrix();
	glTranslatef(TRANSLATE_X, y + ITEM_SCALE*HALF_HEIGHT, 0.0f);
	
	// Draw any effects currently active for this HUD element
	this->itemAssets->espAssets->DrawTimerHUDEffect(dT, camera, this->itemType);

	glScalef(ITEM_SCALE, ITEM_SCALE, 0.0f);

	this->fillerTexture->BindTexture();
	glColor4f(ITEM_ADDITIVE_COLOUR.R(), ITEM_ADDITIVE_COLOUR.G(), ITEM_ADDITIVE_COLOUR.B(), 0.5f * ITEM_ADDITIVE_COLOUR.A());
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex2f(-HALF_WIDTH, -HALF_HEIGHT);
	glTexCoord2i(1, 0); glVertex2f(HALF_WIDTH, -HALF_HEIGHT);
	glTexCoord2i(1, 1); glVertex2f(HALF_WIDTH, HALF_HEIGHT);
	glTexCoord2i(0, 1); glVertex2f(-HALF_WIDTH, HALF_HEIGHT);
	glEnd();
	glPopMatrix();

	// The actual filler, which is constantly decreasing to zero
	glPushMatrix();
	glTranslatef(TRANSLATE_X, y + ITEM_SCALE*fillerHalfHeight, 0.0f);
	glScalef(ITEM_SCALE, ITEM_SCALE, 0.0f);

	glColor4f(std::min<float>(1.0f, this->timerColour.R() + ITEM_ADDITIVE_COLOUR.R()), 
						std::min<float>(1.0f, this->timerColour.G() + ITEM_ADDITIVE_COLOUR.G()), 
						std::min<float>(1.0f, this->timerColour.B() + ITEM_ADDITIVE_COLOUR.B()), ITEM_ADDITIVE_COLOUR.A());

	glBegin(GL_QUADS);
	glTexCoord2d(0, 0); glVertex2d(-HALF_WIDTH, -fillerHalfHeight);
	glTexCoord2d(1, 0); glVertex2d(HALF_WIDTH, -fillerHalfHeight);
	glTexCoord2d(1, fillerTexHeight); glVertex2d(HALF_WIDTH, fillerHalfHeight);
	glTexCoord2d(0, fillerTexHeight); glVertex2d(-HALF_WIDTH, fillerHalfHeight);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(TRANSLATE_X, y + ITEM_SCALE*HALF_HEIGHT, 0.0f);
	glScalef(ITEM_SCALE, ITEM_SCALE, 0.0f);

	this->timerTexture->BindTexture();
	glColor4f(1.0f, 1.0f, 1.0f, ITEM_ADDITIVE_COLOUR.A());
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex2f(-HALF_WIDTH, -HALF_HEIGHT);
	glTexCoord2i(1, 0); glVertex2f(HALF_WIDTH, -HALF_HEIGHT);
	glTexCoord2i(1, 1); glVertex2f(HALF_WIDTH, HALF_HEIGHT);
	glTexCoord2i(0, 1); glVertex2f(-HALF_WIDTH, HALF_HEIGHT);
	glEnd();
	this->timerTexture->UnbindTexture();
	glPopMatrix();

}

/**
 * Indicate to the timer that it has expired, this will start the process of the timer
 * disappearing from the HUD. NOTE: The HUD element will live longer than the timer object
 * this is important since the pointer will be discarded while it stops and becomes dead.
 */
void GameItemAssets::ItemTimerHUDElement::StopTimer(const GameModel& gameModel, bool didExpire) {
	// If the timer is already dead or stopping then we shouldn't tell it to be stopping again
	if (this->currState != ItemTimerHUDElement::TimerDead || this->currState != ItemTimerHUDElement::TimerStopping) {
		this->SetState(ItemTimerHUDElement::TimerStopping);

        // End the timer almost done sound loop
        this->itemAssets->RemoveTimerAlmostDone(this);
        
        if (GameState::IsGameInPlayState(gameModel) && didExpire) {
            // Play the timer end sound -- only play this if the game is in play still...
            this->itemAssets->sound->PlaySound(GameSound::ItemTimerEndedEvent, false, true);
        }
	}
}

/**
 * Set the current state of the item timer HUD. The states of the HUD are expected to unfold in
 * a very sequential particular order. For the typical, uninterrupted life of the HUD element
 * the timer goes through the following sequence of states:
 * TimerStarting -> TimerRunning -> TimerAlmostDone -> TimerStopping -> TimerDead
 *
 * However, the timer may be interrupted at anytime before the TimerStopping state is reached
 * by some outside call to the StopTimer() function which results in a jump to the TimerStopping
 * state.
 */
void GameItemAssets::ItemTimerHUDElement::SetState(const TimerState& state) {
		switch (state) {
		
		case ItemTimerHUDElement::TimerStarting: {
				this->itemAssets->espAssets->AddTimerHUDEffect(this->itemType, this->itemTimer->GetTimerDisposition());

				// Since the timer is starting up, we need to initialize animations for start of the timer
				std::vector<double> timeValues;
				timeValues.reserve(4);
				timeValues.push_back(0.0);
				timeValues.push_back(0.4);
				timeValues.push_back(0.6);
				timeValues.push_back(0.9);

				std::vector<ColourRGBA> colourValues;
				colourValues.reserve(4);
				colourValues.push_back(ColourRGBA(0.0f, 0.0f, 0.0f, 1.0f));
				colourValues.push_back(ColourRGBA(0.75f, 0.75f, 0.75f, 1.0f));
				colourValues.push_back(ColourRGBA(0.5f, 0.5f, 0.5f, 1.0f));
				colourValues.push_back(ColourRGBA(0.0f, 0.0f, 0.0f, 1.0f));

				this->additiveColourAnimation.SetLerp(timeValues, colourValues);
				this->additiveColourAnimation.SetRepeat(false);
				
				std::vector<float> scaleValues;
				scaleValues.reserve(4);
				scaleValues.push_back(0.0f);
				scaleValues.push_back(1.15f);
				scaleValues.push_back(0.9f);
				scaleValues.push_back(1.0f);

				this->scaleAnimation.SetLerp(timeValues, scaleValues);
				this->scaleAnimation.SetRepeat(false);
			}
			this->currState = ItemTimerHUDElement::TimerStarting;
			break;

		case ItemTimerHUDElement::TimerRunning: {
				assert(this->currState == ItemTimerHUDElement::TimerStarting);
			
				// Create the various animations for while the timer is running here
				std::vector<double> timeValues;
				timeValues.reserve(3);
				timeValues.push_back(0.0);
				timeValues.push_back(1.0);
				timeValues.push_back(2.0);

				std::vector<ColourRGBA> colourValues;
				colourValues.reserve(3);
				colourValues.push_back(ColourRGBA(0.0f, 0.0f, 0.0f, 1.0f));
				colourValues.push_back(ColourRGBA(0.33f, 0.33f, 0.33f, 1.0f));
				colourValues.push_back(ColourRGBA(0.0f, 0.0f, 0.0f, 1.0f));

				this->additiveColourAnimation.SetLerp(timeValues, colourValues);
				this->additiveColourAnimation.SetRepeat(true);
				
				std::vector<float> scaleValues;
				scaleValues.reserve(3);
				scaleValues.push_back(1.0f);
				scaleValues.push_back(1.03f);
				scaleValues.push_back(1.0f);

				this->scaleAnimation.SetLerp(timeValues, scaleValues);
				this->scaleAnimation.SetRepeat(true);
			}
			this->currState = ItemTimerHUDElement::TimerRunning;
			break;

		case ItemTimerHUDElement::TimerAlmostDone: {
				std::vector<double> timeValues;
				timeValues.reserve(3);
				timeValues.push_back(0.0);
				timeValues.push_back(0.2);
				timeValues.push_back(0.4);

				std::vector<ColourRGBA> colourValues;
				colourValues.reserve(3);
				colourValues.push_back(ColourRGBA(0.0f, 0.0f, 0.0f, 1.0f));
				colourValues.push_back(ColourRGBA(0.75f, 0.75f, 0.75f, 1.0f));
				colourValues.push_back(ColourRGBA(0.0f, 0.0f, 0.0f, 1.0f));

				std::vector<float> scaleValues;
				scaleValues.reserve(3);
				scaleValues.push_back(1.0);
				scaleValues.push_back(1.1f);
				scaleValues.push_back(1.0);

				this->additiveColourAnimation.SetLerp(timeValues, colourValues);
				this->additiveColourAnimation.SetRepeat(true);
				this->scaleAnimation.SetLerp(timeValues, scaleValues);
				this->scaleAnimation.SetRepeat(true);
			}
			this->currState = ItemTimerHUDElement::TimerAlmostDone;
			break;

		case ItemTimerHUDElement::TimerStopping: {
                static const double ANIM_TIME = 0.5;

				std::vector<double> timeValues;
				timeValues.reserve(2);
				timeValues.push_back(0.0);
				timeValues.push_back(ANIM_TIME);

				std::vector<ColourRGBA> colourValues;
				colourValues.reserve(2);
				colourValues.push_back(this->additiveColourAnimation.GetInterpolantValue());
				colourValues.push_back(ColourRGBA(0.0f, 0.0f, 0.0f, 0.0f));

				std::vector<float> scaleValues;
				scaleValues.reserve(2);
				scaleValues.push_back(this->scaleAnimation.GetInterpolantValue());
				scaleValues.push_back(1.5f);

				this->additiveColourAnimation.SetLerp(timeValues, colourValues);
				this->additiveColourAnimation.SetRepeat(false);
				this->scaleAnimation.SetLerp(timeValues, scaleValues);
				this->scaleAnimation.SetRepeat(false);

				// Since the timer is stopping it means that the item timer object may no longer be valid
				// (since after it stops it gets destroyed), so clean it up now to avoid hassle...
				assert(this->itemTimer != NULL);
				this->itemTimer = NULL;
			}
			this->currState = ItemTimerHUDElement::TimerStopping;
			break;

		case ItemTimerHUDElement::TimerDead:
			assert(this->currState == ItemTimerHUDElement::TimerStopping);
            this->itemAssets->RemoveTimerAlmostDone(this);
			this->currState = ItemTimerHUDElement::TimerDead;
			break;

		default:
			assert(false);
			break;
	}
}

void GameItemAssets::AddTimerAlmostDone(const ItemTimerHUDElement* timer) {
    if (this->timerAlmostDoneSoundID == INVALID_SOUND_ID) {
        assert(this->timersAlmostDone.empty());
        this->timerAlmostDoneSoundID = this->sound->PlaySound(GameSound::ItemTimerEndingLoop, true);
    }
    this->timersAlmostDone.insert(timer);
}

void GameItemAssets::RemoveTimerAlmostDone(const ItemTimerHUDElement* timer) {
    if (this->timersAlmostDone.erase(timer) == 1 && this->timersAlmostDone.empty()) {
        this->sound->StopSound(this->timerAlmostDoneSoundID);
        this->timerAlmostDoneSoundID = INVALID_SOUND_ID;
    }
}