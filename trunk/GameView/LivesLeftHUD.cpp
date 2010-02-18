/**
 * LivesLeftHUD.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "LivesLeftHUD.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Texture.h"
#include "../BlammoEngine/Camera.h"

#include "../GameModel/GameModelConstants.h"

#include "../ResourceManager.h"

const Colour LivesLeftHUD::ELEMENT_BASE_COLOURS[] = { Colour(1.0f, 0.0f, 0.0f), Colour(1.0f, 0.5f, 0.0f),
																										  Colour(1.0f, 1.0f, 0.0f), Colour(0.6f, 1.0f, 0.0f),
																											Colour(0.0f, 1.0f, 0.0f), Colour(0.0f, 1.0f, 1.0f),
																											Colour(0.0f, 0.5f, 1.0f), Colour(0.0f, 0.0f, 1.0f),
																											Colour(0.3f, 0.0f, 1.0f), Colour(0.7f, 0.0f, 1.0f), 
																											Colour(1.0f, 0.0f, 1.0f)};

LivesLeftHUD::LivesLeftHUD() : currNumLivesLeft(0), ballLifeHUDTex(NULL) {
	// Load ball life HUD texture
	this->ballLifeHUDTex = ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BALL_LIFE_HUD, Texture::Trilinear);
	assert(this->ballLifeHUDTex != NULL);

	this->InitIdleColourInterpolations();
}

LivesLeftHUD::~LivesLeftHUD() {
	// Delete the HUD texture
	bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->ballLifeHUDTex);
	assert(success);
}

void LivesLeftHUD::InitIdleColourInterpolations() {
	int amountToReserve = GameModelConstants::GetInstance()->MAX_LIVES_LEFT;
	this->idleColourAnimations.reserve(amountToReserve);
	this->idleSizeAnimations.reserve(amountToReserve);
	this->elementCurrAnimationTypes.resize(amountToReserve, LivesLeftHUD::IdleAnimation);

	// Setup the colours for each ball life so that they change back and forth
	// between their neighbor's colours
	for (int i = 0; i < GameModelConstants::GetInstance()->MAX_LIVES_LEFT; i++) {
		// Idle Colour interpolation 
		Colour currColour(LivesLeftHUD::ELEMENT_BASE_COLOURS[i]);
		AnimationMultiLerp<Colour> currColourAnim = AnimationMultiLerp<Colour>(Colour());
		
		std::vector<double> timeValues;
		timeValues.reserve(3);
		timeValues.push_back(0.0);
		timeValues.push_back(1.0);
		timeValues.push_back(2.0);
		
		std::vector<Colour> colourValues;
		colourValues.reserve(3);
		colourValues.push_back(currColour);
		colourValues.push_back(LivesLeftHUD::ELEMENT_BASE_COLOURS[i+1]);
		colourValues.push_back(currColour);

		currColourAnim.SetRepeat(true);
		currColourAnim.SetLerp(timeValues, colourValues);
		
		this->idleColourAnimations.push_back(currColourAnim);

		// Idle size interpolation - these get setup every time a new ball life is added to the HUD
		AnimationMultiLerp<float> currSizeAnim(1.0f);

		timeValues[0] = 0.0;
		timeValues[1] = 1.0;
		timeValues[2] = 2.0;
		
		std::vector<float> sizeValues(3);
		sizeValues[0] = 1.0f;
		sizeValues[1] = 0.85f;
		sizeValues[2] = 1.0f;

		currSizeAnim.SetLerp(timeValues, sizeValues);
		currSizeAnim.SetRepeat(true);
		this->idleSizeAnimations.push_back(currSizeAnim);
	}
}

/**
 * Reinitialize the HUD to have no animations and zero lives.
 */
void LivesLeftHUD::Reinitialize() {
	this->creationAlphaAnimations.clear();
	this->creationShiftAnimations.clear();
	this->destructionColourAnimations.clear();
	this->destructionFallAnimations.clear();
	this->currNumLivesLeft = 0;
}

/**
 * Call this when a life or set of lives are lost in the game - this will
 * cause the HUD to update itself appropriately.
 */
void LivesLeftHUD::LivesLost(int numLives) {

	// Prompt animations and destruction of lives in the HUD
	int livesLeftAfter = this->currNumLivesLeft - numLives;
	int count = 0;
	for (int i = this->currNumLivesLeft-1; i >= livesLeftAfter; i--) {
		this->elementCurrAnimationTypes[i] = LivesLeftHUD::DestructionAnimation;

		// Destruction animations for balls
		ColourRGBA startColour = ColourRGBA(this->idleColourAnimations[i].GetInterpolantValue(), 1);
		ColourRGBA endColour   = ColourRGBA(0.0f, 0.0f, 0.0f, 0.0f);
		AnimationLerp<ColourRGBA> colourAnim(startColour);
		double startTime = static_cast<double>(count) / 2.0;
		colourAnim.SetLerp(startTime, startTime + 2.0, startColour, endColour);
		this->destructionColourAnimations[i] = colourAnim;

		AnimationLerp<float> fallAnim(0.0f);
		fallAnim.SetLerp(startTime, startTime + 2.0, 0.0f, 3*LivesLeftHUD::ELEMENT_SIZE);
		this->destructionFallAnimations[i] = fallAnim;

		count++;
	}

	// The current number of lives stored in this object will
	// be updated only AFTER the destruction animations have played
	//this->currNumLivesLeft = livesLeftAfter;
}

/**
 * Call this when a life or set of lives are gained in the game - this will
 * cause the HUD to update itself appropriately.
 */
void LivesLeftHUD::LivesGained(int numLives) {

	// Prompt animations and creation of lives in the HUD
	int livesLeftAfter = this->currNumLivesLeft + numLives;
	int count = 0;
	for (int i = this->currNumLivesLeft; i < livesLeftAfter; i++) {
		this->elementCurrAnimationTypes[i] = LivesLeftHUD::CreationAnimation;

		// Reset the idle animation for the gained life elements
		double midTimeValue =  1.25 + Randomizer::GetInstance()->RandomNumZeroToOne();
		std::vector<double> timeValues(3);
		timeValues[0] = 0.0;
		timeValues[1] = midTimeValue;
		timeValues[2] = 2*midTimeValue;
		
		std::vector<float> sizeValues(3);
		sizeValues[0] = 1.05f;
		sizeValues[1] = 0.82f;
		sizeValues[2] = 1.05f;

		this->idleSizeAnimations[i].SetLerp(timeValues, sizeValues);

		// Add creation animations
		AnimationLerp<float> alphaAnim = AnimationLerp<float>(0.0f);
		double startTime = static_cast<double>(count) / 2.0;
		alphaAnim.SetLerp(startTime, startTime + 1.0, 0.0f, 1.0f);
		this->creationAlphaAnimations[i] = alphaAnim;

		float startShiftLoc = 3*LivesLeftHUD::ELEMENT_SIZE;
		AnimationLerp<float> shiftAnim = AnimationLerp<float>(startShiftLoc);
		shiftAnim.SetLerp(startTime, startTime + 1.0, startShiftLoc, 0.0f);
		this->creationShiftAnimations[i] = shiftAnim;
		
		count++;
	}

	this->currNumLivesLeft = livesLeftAfter;	
	assert(this->currNumLivesLeft <= GameModelConstants::GetInstance()->MAX_LIVES_LEFT);
}

/**
 * Draw the HUD for the number of player lives left and tick any animations and/or effects
 * for the HUD elements.
 */
void LivesLeftHUD::Draw(double dT, int displayWidth, int displayHeight) {
	assert(this->currNumLivesLeft >= 0);

	// If there are no lives left then there is nothing to draw
	if (this->currNumLivesLeft == 0) {
		return;
	}
	
	int currYPos = displayHeight - LivesLeftHUD::BORDER_SPACING;
	int currXPos  = LivesLeftHUD::BORDER_SPACING;

	// Prepare OGL for drawing the timer
	glPushAttrib(GL_TEXTURE_BIT | GL_LIGHTING_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);			

	// Make world coordinates equal window coordinates
	Camera::PushWindowCoords();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// For each of the lives we draw a ball in the upper-left corner of the screen
	for (int i = 0; i < this->currNumLivesLeft; i++) {

		const BallElementAnimations currAnimationType = this->elementCurrAnimationTypes[i];

		Colour& currColour = this->idleColourAnimations[i].GetInterpolantValue();
		float currSize = this->idleSizeAnimations[i].GetInterpolantValue();
		const int outlineHalfSize = LivesLeftHUD::OUTLINE_SIZE + LivesLeftHUD::ELEMENT_HALF_SIZE;

		float currAlpha = 1.0f;
		float fallTranslation = 0.0f;
		float shiftTranslation = 0.0f;

		if (currAnimationType == LivesLeftHUD::CreationAnimation) {
			// If the current ball element is performing its creation animation then its alpha will be
			// affected - obtain its current alpha from the animation
			std::map<int, AnimationLerp<float>>::iterator currAlphaAnimIter = this->creationAlphaAnimations.find(i);
			std::map<int, AnimationLerp<float>>::iterator currShiftAnimIter = this->creationShiftAnimations.find(i);
			
			assert(currAlphaAnimIter != this->creationAlphaAnimations.end());
			assert(currShiftAnimIter != this->creationShiftAnimations.end());
			
			currAlpha = currAlphaAnimIter->second.GetInterpolantValue();
			shiftTranslation = currShiftAnimIter->second.GetInterpolantValue();

			bool isAlphaFinished = currAlphaAnimIter->second.Tick(dT);
			bool isShiftFinished = currShiftAnimIter->second.Tick(dT);

			if (isAlphaFinished && isShiftFinished) {
				this->creationAlphaAnimations.erase(currAlphaAnimIter);
				this->creationShiftAnimations.erase(currShiftAnimIter);

				this->elementCurrAnimationTypes[i] = LivesLeftHUD::IdleAnimation;
			}
		}
		else if (currAnimationType == LivesLeftHUD::DestructionAnimation) {
			// If the current ball is performing its destruction animation then its colour value
			// and translation will be affected
			std::map<int, AnimationLerp<float>>::iterator fallAnimIter				= this->destructionFallAnimations.find(i);
			std::map<int, AnimationLerp<ColourRGBA>>::iterator colourAnimIter = this->destructionColourAnimations.find(i);

			assert(fallAnimIter != this->destructionFallAnimations.end());
			assert(colourAnimIter != this->destructionColourAnimations.end());

			fallTranslation = fallAnimIter->second.GetInterpolantValue();
			ColourRGBA currRGBAColour = colourAnimIter->second.GetInterpolantValue();
			currColour = currRGBAColour.GetColour();
			currAlpha = currRGBAColour.A();

			bool isFallAnimFinished   = fallAnimIter->second.Tick(dT);
			bool isColourAnimFinished = colourAnimIter->second.Tick(dT);
			if (isFallAnimFinished && isColourAnimFinished) {
				this->destructionFallAnimations.erase(fallAnimIter);
				this->destructionColourAnimations.erase(colourAnimIter);
				this->elementCurrAnimationTypes[i] = LivesLeftHUD::IdleAnimation;
				this->currNumLivesLeft--;
			}
		}

		glPushMatrix();
		glTranslatef(currXPos + shiftTranslation, currYPos - fallTranslation, 0.0f);
		glScalef(currSize, currSize, currSize);

		// Draw a ball life graphic in the HUD
		this->ballLifeHUDTex->BindTexture();
		
		glBegin(GL_QUADS);
			// Black outline behind the coloured graphic of the ball life HUD element
			glColor4f(0.0f, 0.0f, 0.0f, currAlpha);
			glTexCoord2i(0, 0); glVertex2i(-outlineHalfSize, -outlineHalfSize);
			glTexCoord2i(1, 0); glVertex2i(outlineHalfSize, -outlineHalfSize);
			glTexCoord2i(1, 1); glVertex2i(outlineHalfSize, outlineHalfSize);
			glTexCoord2i(0, 1); glVertex2i(-outlineHalfSize, outlineHalfSize);

			// Coloured ball life HUD element
			glColor4f(currColour.R(), currColour.G(), currColour.B(), currAlpha);
			glTexCoord2i(0, 0); glVertex2i(-LivesLeftHUD::ELEMENT_HALF_SIZE, -LivesLeftHUD::ELEMENT_HALF_SIZE);
			glTexCoord2i(1, 0); glVertex2i(LivesLeftHUD::ELEMENT_HALF_SIZE, -LivesLeftHUD::ELEMENT_HALF_SIZE);
			glTexCoord2i(1, 1); glVertex2i(LivesLeftHUD::ELEMENT_HALF_SIZE, LivesLeftHUD::ELEMENT_HALF_SIZE);
			glTexCoord2i(0, 1); glVertex2i(-LivesLeftHUD::ELEMENT_HALF_SIZE, LivesLeftHUD::ELEMENT_HALF_SIZE);
		glEnd();
		
		this->ballLifeHUDTex->UnbindTexture();

		glPopMatrix();

		currXPos += LivesLeftHUD::ELEMENT_OVERLAP;

		this->idleSizeAnimations[i].Tick(dT);
	}

	// Pop modelview matrix
	glPopMatrix();
	// Pop the projection matrix
	Camera::PopWindowCoords();
	glPopAttrib();

	// Animate any animation values of the ball elements that require constant animation
	for (std::vector<AnimationMultiLerp<Colour>>::iterator iter = this->idleColourAnimations.begin();
		iter != this->idleColourAnimations.end(); ++iter) {
		iter->Tick(dT);
	}

}