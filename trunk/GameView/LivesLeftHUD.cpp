/**
 * LivesLeftHUD.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
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

LivesLeftHUD::LivesLeftHUD() : currNumLivesLeft(0), heartTex(NULL), noHeartTex(NULL),
infinityTex(NULL), infiniteLivesOn(false) {

    this->heartTex = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_HEART, Texture::Trilinear);
	assert(this->heartTex != NULL);

    this->noHeartTex = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_NO_HEART, Texture::Trilinear);
	assert(this->noHeartTex != NULL);

    // Load infinity character texture
    this->infinityTex = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_INFINITY_CHAR, Texture::Trilinear);
    assert(this->infinityTex != NULL);

	this->InitIdleColourInterpolations();
}

LivesLeftHUD::~LivesLeftHUD() {
	// Delete the HUD texture
	bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->noHeartTex);
    UNUSED_VARIABLE(success);
	assert(success);
    
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->heartTex);
    assert(success);
    
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->infinityTex);
    assert(success);
}

void LivesLeftHUD::InitIdleColourInterpolations() {
	int amountToReserve = GameModelConstants::GetInstance()->MAXIMUM_POSSIBLE_LIVES;
	
    this->idleSizeAnimations.reserve(amountToReserve);
	this->elementCurrAnimationTypes.resize(amountToReserve, LivesLeftHUD::IdleAnimation);

    this->creationAlphaAnimations.resize(amountToReserve);
    this->creationSizeAnimations.resize(amountToReserve);
    this->destructionColourAnimations.resize(amountToReserve);
    this->destructionFallAnimations.resize(amountToReserve);
    this->destructionRotationAnimations.resize(amountToReserve);
 
	for (int i = 0; i < GameModelConstants::GetInstance()->MAXIMUM_POSSIBLE_LIVES; i++) {
		// Idle size interpolation - these get setup every time a new ball life is added to the HUD
		AnimationMultiLerp<float> currSizeAnim(1.0f);

        std::vector<double> timeValues(3);
		timeValues[0] = 0.0;
		timeValues[1] = 1.0;
		timeValues[2] = 2.0;
		
		std::vector<float> sizeValues(3);
		sizeValues[0] = 1.0f;
		sizeValues[1] = 1.2f;
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
	this->currNumLivesLeft = 0;
    this->infiniteLivesOn  = false;
}

/**
 * Call this when a life or set of lives are lost in the game - this will
 * cause the HUD to update itself appropriately.
 */
void LivesLeftHUD::LivesLost(int numLives) {
    if (infiniteLivesOn) { return; }

	// Prompt animations and destruction of lives in the HUD
	int livesLeftAfter = this->currNumLivesLeft - numLives;
	int count = 0;
	for (int i = this->currNumLivesLeft-1; i >= livesLeftAfter; i--) {
		this->elementCurrAnimationTypes[i] = LivesLeftHUD::DestructionAnimation;

		// Destruction animations for balls
        ColourRGBA startColour(1.0f, 1.0f, 1.0f, 1.0f);
		double startTime = static_cast<double>(count) / 2.0;
        double endTime   = startTime + 2.0;
		this->destructionColourAnimations[i].SetLerp(startTime, endTime, startColour, ColourRGBA(0.0f, 0.0f, 0.0f, 0.0f));
        this->destructionColourAnimations[i].SetInterpolantValue(startColour);
		this->destructionFallAnimations[i].SetLerp(startTime, endTime, 0.0f, 2.25f*LivesLeftHUD::ELEMENT_SIZE);
        this->destructionFallAnimations[i].SetInterpolantValue(0.0f);
        this->destructionRotationAnimations[i].SetLerp(startTime, endTime, 0.0f, 900.0f);
        this->destructionRotationAnimations[i].SetInterpolantValue(0.0f);
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
    if (infiniteLivesOn) { return; }

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
        sizeValues[0] = 0.0001f;
        sizeValues[1] = 1.5f;
        sizeValues[2] = 1.0f;

        std::vector<float> alphaValues(3);
        alphaValues[0] = 0.0f;
        alphaValues[1] = 1.0f;
        alphaValues[2] = 1.0f;

		double startTime = static_cast<double>(count) / 2.0;
        timeValues[0] = startTime;
        timeValues[1] = startTime + 0.25;
        timeValues[2] = startTime + 0.5;

        this->creationAlphaAnimations[i].SetLerp(timeValues, alphaValues);
        this->creationAlphaAnimations[i].SetInterpolantValue(alphaValues[0]);
        this->creationSizeAnimations[i].SetLerp(timeValues, sizeValues);
        this->creationSizeAnimations[i].SetInterpolantValue(sizeValues[0]);

		count++;
	}

	this->currNumLivesLeft = livesLeftAfter;	
    assert(this->currNumLivesLeft <= GameModelConstants::GetInstance()->MAXIMUM_POSSIBLE_LIVES);
}

/**
 * Draw the HUD for the number of player lives left and tick any animations and/or effects
 * for the HUD elements.
 */
void LivesLeftHUD::Draw(double dT, int displayWidth, int displayHeight) {
	UNUSED_PARAMETER(displayWidth);
	assert(this->currNumLivesLeft >= 0);

	// If there are no lives left then there is nothing to draw
	if (this->currNumLivesLeft == 0) {
		return;
	}
	
	const int currYPos = displayHeight - LivesLeftHUD::BORDER_SPACING;
	
	// Prepare OGL for drawing the timer
	glPushAttrib(GL_TEXTURE_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	
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

    int currXPos  = LivesLeftHUD::BORDER_SPACING;

    // If the infinite lives toggle is on then we draw a special HUD with the infinity character...
    if (this->infiniteLivesOn) {
        
        Colour currColour(1,1,1);
        float heartCurrSize = 1.0f;
        float currAlpha = 1.0f;

	    const BallElementAnimations& currAnimationType = this->elementCurrAnimationTypes[0];

        switch (currAnimationType) {
            case LivesLeftHUD::CreationAnimation: {
		        // If the current ball element is performing its creation animation then its alpha will be
		        // affected - obtain its current alpha from the animation
		        AnimationMultiLerp<float>& currAlphaAnim = this->creationAlphaAnimations[0];
		        AnimationMultiLerp<float>& currSizeAnim  = this->creationSizeAnimations[0];
    			
		        currAlpha = currAlphaAnim.GetInterpolantValue();
		        heartCurrSize = currSizeAnim.GetInterpolantValue();

		        bool isAlphaFinished = currAlphaAnim.Tick(dT);
		        bool isSizeFinished  = currSizeAnim.Tick(dT);

		        if (isAlphaFinished && isSizeFinished) {
			        this->elementCurrAnimationTypes[0] = LivesLeftHUD::IdleAnimation;
		        }
                break;
	        }
            case LivesLeftHUD::IdleAnimation:
                heartCurrSize = this->idleSizeAnimations[0].GetInterpolantValue();
                this->idleSizeAnimations[0].Tick(dT);
                break;

            default:
                break;
        }

	    glPushMatrix();
	    glTranslatef(currXPos, currYPos, 0.0f);
        glScalef(heartCurrSize, heartCurrSize, 1.0f);
        
        this->heartTex->BindTexture();
        glBegin(GL_QUADS);
        glColor4f(currColour.R(), currColour.G(), currColour.B(), 0.9f*currAlpha);
        glTexCoord2i(0, 0); glVertex2i(-LivesLeftHUD::ELEMENT_HALF_SIZE, -LivesLeftHUD::ELEMENT_HALF_SIZE);
        glTexCoord2i(1, 0); glVertex2i(LivesLeftHUD::ELEMENT_HALF_SIZE, -LivesLeftHUD::ELEMENT_HALF_SIZE);
        glTexCoord2i(1, 1); glVertex2i(LivesLeftHUD::ELEMENT_HALF_SIZE, LivesLeftHUD::ELEMENT_HALF_SIZE);
        glTexCoord2i(0, 1); glVertex2i(-LivesLeftHUD::ELEMENT_HALF_SIZE, LivesLeftHUD::ELEMENT_HALF_SIZE);
        glEnd();
        glPopMatrix();
        
        currXPos += LivesLeftHUD::ELEMENT_SIZE + LivesLeftHUD::BETWEEN_SPACING;
	    
        glPushMatrix();
	    glTranslatef(currXPos, currYPos, 0.0f);
        glScalef(0.8f, 0.8f, 1.0f);

        this->infinityTex->BindTexture();
        glBegin(GL_QUADS);
        glColor4f(1, 1, 1,  0.9f*currAlpha);
        glTexCoord2i(0, 0); glVertex2i(-LivesLeftHUD::ELEMENT_HALF_SIZE, -LivesLeftHUD::ELEMENT_HALF_SIZE);
        glTexCoord2i(1, 0); glVertex2i(LivesLeftHUD::ELEMENT_HALF_SIZE, -LivesLeftHUD::ELEMENT_HALF_SIZE);
        glTexCoord2i(1, 1); glVertex2i(LivesLeftHUD::ELEMENT_HALF_SIZE, LivesLeftHUD::ELEMENT_HALF_SIZE);
        glTexCoord2i(0, 1); glVertex2i(-LivesLeftHUD::ELEMENT_HALF_SIZE, LivesLeftHUD::ELEMENT_HALF_SIZE);
        glEnd();

        glPopMatrix();
    }
    else {
    
        // Draw the fixed number of life placeholders
        for (int i = 0; i < GameModelConstants::GetInstance()->MAXIMUM_POSSIBLE_LIVES; i++) {

            Colour currColour(1,1,1);
            float heartCurrSize = 1.0f;
            float currAlpha = 1.0f;
            float fallTranslation = 0.0f;
            float fallRotation = 0.0f;

	        const BallElementAnimations& currAnimationType = this->elementCurrAnimationTypes[i];

            switch (currAnimationType) {
                case LivesLeftHUD::CreationAnimation: {
		            // If the current ball element is performing its creation animation then its alpha will be
		            // affected - obtain its current alpha from the animation
		            AnimationMultiLerp<float>& currAlphaAnim = this->creationAlphaAnimations[i];
		            AnimationMultiLerp<float>& currSizeAnim  = this->creationSizeAnimations[i];
        			
		            currAlpha = currAlphaAnim.GetInterpolantValue();
		            heartCurrSize = currSizeAnim.GetInterpolantValue();

		            bool isAlphaFinished = currAlphaAnim.Tick(dT);
		            bool isSizeFinished  = currSizeAnim.Tick(dT);

		            if (isAlphaFinished && isSizeFinished) {
			            this->elementCurrAnimationTypes[i] = LivesLeftHUD::IdleAnimation;
		            }
                    break;
	            }
                case LivesLeftHUD::DestructionAnimation: {
		            // If the current ball is performing its destruction animation then its colour value
		            // and translation will be affected
		            AnimationLerp<float>& fallAnim = this->destructionFallAnimations[i];
		            AnimationLerp<ColourRGBA>& colourAnim = this->destructionColourAnimations[i];
                    AnimationLerp<float>& rotAnim = this->destructionRotationAnimations[i];

		            fallTranslation = fallAnim.GetInterpolantValue();
                    fallRotation = rotAnim.GetInterpolantValue();

		            const ColourRGBA& currRGBAColour = colourAnim.GetInterpolantValue();
		            currColour = currRGBAColour.GetColour();
		            currAlpha = currRGBAColour.A();
                    heartCurrSize = this->idleSizeAnimations[i].GetInterpolantValue();

		            bool isFallAnimFinished   = fallAnim.Tick(dT);
		            bool isColourAnimFinished = colourAnim.Tick(dT);
                    bool isFallRotFinished    = rotAnim.Tick(dT);

		            if (isFallAnimFinished && isColourAnimFinished && isFallRotFinished) {
			            this->elementCurrAnimationTypes[i] = LivesLeftHUD::IdleAnimation;
			            this->currNumLivesLeft--;
		            }
                    break;
	            }

                case LivesLeftHUD::IdleAnimation:
                    heartCurrSize = this->idleSizeAnimations[i].GetInterpolantValue();
                    this->idleSizeAnimations[i].Tick(dT);
                    break;

                default:
                    break;
            }

	        glPushMatrix();
	        glTranslatef(currXPos, currYPos, 0.0f);

            if (i < this->currNumLivesLeft) {

                glPushMatrix();
                glTranslatef(0, -fallTranslation, 0);
	            glRotatef(fallRotation, 0, 0, 1);
                glScalef(heartCurrSize, heartCurrSize, 1.0f);
                
                this->heartTex->BindTexture();
	            glBegin(GL_QUADS);
	            glColor4f(currColour.R(), currColour.G(), currColour.B(), 0.9f*currAlpha);
	            glTexCoord2i(0, 0); glVertex2i(-LivesLeftHUD::ELEMENT_HALF_SIZE, -LivesLeftHUD::ELEMENT_HALF_SIZE);
	            glTexCoord2i(1, 0); glVertex2i(LivesLeftHUD::ELEMENT_HALF_SIZE, -LivesLeftHUD::ELEMENT_HALF_SIZE);
	            glTexCoord2i(1, 1); glVertex2i(LivesLeftHUD::ELEMENT_HALF_SIZE, LivesLeftHUD::ELEMENT_HALF_SIZE);
	            glTexCoord2i(0, 1); glVertex2i(-LivesLeftHUD::ELEMENT_HALF_SIZE, LivesLeftHUD::ELEMENT_HALF_SIZE);
	            glEnd();
                glPopMatrix();
            }
            
            if (i >= this->currNumLivesLeft || currAnimationType == LivesLeftHUD::DestructionAnimation) {
                
                glScalef(0.8f, 0.8f, 1.0f);
    	        
                this->noHeartTex->BindTexture();
                glBegin(GL_QUADS);
	            glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
	            glTexCoord2i(0, 0); glVertex2i(-LivesLeftHUD::ELEMENT_HALF_SIZE, -LivesLeftHUD::ELEMENT_HALF_SIZE);
	            glTexCoord2i(1, 0); glVertex2i(LivesLeftHUD::ELEMENT_HALF_SIZE, -LivesLeftHUD::ELEMENT_HALF_SIZE);
	            glTexCoord2i(1, 1); glVertex2i(LivesLeftHUD::ELEMENT_HALF_SIZE, LivesLeftHUD::ELEMENT_HALF_SIZE);
	            glTexCoord2i(0, 1); glVertex2i(-LivesLeftHUD::ELEMENT_HALF_SIZE, LivesLeftHUD::ELEMENT_HALF_SIZE);
	            glEnd();
            }

            glPopMatrix();

            currXPos += LivesLeftHUD::ELEMENT_SIZE + LivesLeftHUD::BETWEEN_SPACING;
        }
    }
    this->heartTex->UnbindTexture();

	// Pop modelview matrix
	glPopMatrix();
	// Pop the projection matrix
	Camera::PopWindowCoords();
	glPopAttrib();

}