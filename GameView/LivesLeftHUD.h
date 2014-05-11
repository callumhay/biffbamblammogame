/**
 * LivesLeftHUD.h
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

#ifndef __LIVESLEFTHUD_H__
#define __LIVESLEFTHUD_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Animation.h"

class Texture;

class LivesLeftHUD {
public:
	static const int OUTLINE_SIZE           = 1;
	static const int ELEMENT_SIZE           = 40;                   // Size (width and height) of the elements in the HUD
	static const int ELEMENT_HALF_SIZE      = ELEMENT_SIZE/2;

	static const int BORDER_SPACING     = 5 + ELEMENT_HALF_SIZE; // Spacing between edge of the screen and the HUD
	static const int BETWEEN_SPACING    = 5;                     // Spacing between the elements in the HUD

	LivesLeftHUD();
	~LivesLeftHUD();

    void ToggleInfiniteLivesDisplay(bool infiniteLivesOn);

	void Reinitialize();
	void LivesLost(int numLives);
	void LivesGained(int numLives);
	void Draw(double dT, int displayWidth, int displayHeight);

    void SetAlpha(float alpha);

private:
	enum BallElementAnimations { CreationAnimation, IdleAnimation, DestructionAnimation };

	// Ball Life Element animation values
	std::vector<BallElementAnimations> elementCurrAnimationTypes;
	
	// Idle animation values
	std::vector<AnimationMultiLerp<float> > idleSizeAnimations;

	// Creation animation values
    std::vector<AnimationMultiLerp<float> > creationSizeAnimations;
    std::vector<AnimationMultiLerp<float> > creationAlphaAnimations;

	// Destruction animation values
    std::vector<AnimationLerp<ColourRGBA> > destructionColourAnimations;
	std::vector<AnimationLerp<float> >      destructionFallAnimations;
    std::vector<AnimationLerp<float> >      destructionRotationAnimations;

	int currNumLivesLeft;
    Texture* heartTex;
    Texture* noHeartTex;
    Texture* infinityTex;
    bool infiniteLivesOn;

    float alpha;

	void InitIdleColourInterpolations();

    void TickState(double dT);

    DISALLOW_COPY_AND_ASSIGN(LivesLeftHUD);
};

inline void LivesLeftHUD::ToggleInfiniteLivesDisplay(bool infiniteLivesOn) {
    this->infiniteLivesOn = infiniteLivesOn;
}

inline void LivesLeftHUD::SetAlpha(float alpha) {
    this->alpha = alpha;
}

#endif