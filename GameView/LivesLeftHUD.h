/**
 * LivesLeftHUD.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
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