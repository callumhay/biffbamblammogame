#ifndef __LIVESLEFTHUD_H__
#define __LIVESLEFTHUD_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Animation.h"

class Texture;

class LivesLeftHUD {
private:
	enum BallElementAnimations { CreationAnimation, IdleAnimation, DestructionAnimation };
	static const int OUTLINE_SIZE					= 1;
	static const int ELEMENT_SIZE					= 32;									// Size (width and height) of the elements in the HUD
	static const int ELEMENT_HALF_SIZE		= ELEMENT_SIZE/2;			
	static const int ELEMENT_OVERLAP			= ELEMENT_HALF_SIZE;	// Amount of overlap for the ball elements in the HUD

	static const int BORDER_SPACING				= 5 + ELEMENT_HALF_SIZE;	// Spacing between edge of the screen and the HUD
	static const int BETWEEN_SPACING			= 3 + ELEMENT_HALF_SIZE;	// Spacing between the elements in the HUD

	static const Colour ELEMENT_BASE_COLOURS[];

	// Ball Life Element animation values
	std::vector<BallElementAnimations> elementCurrAnimationTypes;
	
	// Idle animation values
	std::vector<AnimationMultiLerp<Colour>> idleColourAnimations;
	std::vector<AnimationMultiLerp<float>>  idleSizeAnimations;

	// Creation animation values
	std::map<int, AnimationLerp<float>> creationAlphaAnimations;
	std::map<int, AnimationLerp<float>> creationShiftAnimations;

	// Destruction animation values
	std::map<int, AnimationLerp<ColourRGBA>> destructionColourAnimations;
	std::map<int, AnimationLerp<float>> destructionFallAnimations; 

	int currNumLivesLeft;
	Texture *ballLifeHUDTex;

	void InitIdleColourInterpolations();

public:
	LivesLeftHUD();
	~LivesLeftHUD();

	void LivesLost(int numLives);
	void LivesGained(int numLives);
	void Draw(double dT, int displayWidth, int displayHeight);
};

#endif