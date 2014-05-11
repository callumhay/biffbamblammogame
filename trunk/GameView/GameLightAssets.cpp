/**
 * GameLightAssets.cpp
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

#include "GameLightAssets.h"
#include "GameViewConstants.h"
#include "GameDisplay.h"

const float GameLightAssets::DEFAULT_LIGHT_TOGGLE_TIME = 1.0f;

GameLightAssets::GameLightAssets() : 
ballLight(Point3D(0,0,0), GameViewConstants::GetInstance()->DEFAULT_BALL_LIGHT_COLOUR, GameViewConstants::GetInstance()->DEFAULT_BALL_LIGHT_ATTEN) {

    // Setup the Background lights with their default values
    this->SetBackgroundLightDefaults(
        BasicPointLight(GameViewConstants::GetInstance()->DEFAULT_BG_KEY_LIGHT_POSITION, GameViewConstants::GetInstance()->DEFAULT_BG_KEY_LIGHT_COLOUR, 
        GameViewConstants::GetInstance()->DEFAULT_BG_KEY_LIGHT_ATTEN),
        BasicPointLight(GameViewConstants::GetInstance()->DEFAULT_BG_FILL_LIGHT_POSITION, GameViewConstants::GetInstance()->DEFAULT_BG_FILL_LIGHT_COLOUR,  
        GameViewConstants::GetInstance()->DEFAULT_BG_FILL_LIGHT_ATTEN));

	// Setup the foreground lights (blocks, paddle, ball, bosses, etc.) with their default values
    this->SetForegroundLightDefaults(
        BasicPointLight(GameViewConstants::GetInstance()->DEFAULT_FG_KEY_LIGHT_POSITION, GameViewConstants::GetInstance()->DEFAULT_FG_KEY_LIGHT_COLOUR, 
        GameViewConstants::GetInstance()->DEFAULT_FG_KEY_LIGHT_ATTEN),
        BasicPointLight(GameViewConstants::GetInstance()->DEFAULT_FG_FILL_LIGHT_POSITION, GameViewConstants::GetInstance()->DEFAULT_FG_FILL_LIGHT_COLOUR, 
        GameViewConstants::GetInstance()->DEFAULT_FG_FILL_LIGHT_ATTEN));
}

GameLightAssets::~GameLightAssets() {
}

/**
 * Toggle the lights in the game either on or off (both foreground
 * and background lights are affected).
 * Returns: The time it takes to toggle the lights on/off.
 */
void GameLightAssets::ToggleLights(bool turnOn, double toggleTime) {
	
	// For the fill and key lights of the foreground and background we simply
	// turn them on or off...
	this->fgKeyLight.SetLightOn(turnOn, toggleTime);
	this->fgFillLight.SetLightOn(turnOn, toggleTime);
	this->bgKeyLight.SetLightOn(turnOn, toggleTime);
	this->bgFillLight.SetLightOn(turnOn, toggleTime);

	// The ball light needs to change how much/well it illuminates the scene as well...
	if (turnOn) {
		this->ballLight.SetLinearAttenuation(GameViewConstants::GetInstance()->DEFAULT_BALL_LIGHT_ATTEN);
		this->ballKeyLight.SetLightColourChange(GameViewConstants::GetInstance()->DEFAULT_BALL_KEY_LIGHT_COLOUR, toggleTime);
		this->paddleKeyLight.SetLightColourChange(GameViewConstants::GetInstance()->DEFAULT_PADDLE_KEY_LIGHT_COLOUR, toggleTime);
		this->paddleFillLight.SetLightColourChange(GameViewConstants::GetInstance()->DEFAULT_PADDLE_FILL_LIGHT_COLOUR, toggleTime);
        this->bossKeyLight.SetLightColourChange(GameViewConstants::GetInstance()->DEFAULT_BOSS_KEY_LIGHT_COLOUR, toggleTime);
        this->bossFillLight.SetLightColourChange(GameViewConstants::GetInstance()->DEFAULT_BOSS_FILL_LIGHT_COLOUR, toggleTime);
	}
	else {
		this->ballLight.SetLinearAttenuation(GameViewConstants::GetInstance()->BLACKOUT_BALL_LIGHT_ATTEN);
		this->ballKeyLight.SetLightColourChange(GameViewConstants::GetInstance()->BLACKOUT_BALL_KEY_LIGHT_COLOUR, toggleTime);
		this->paddleKeyLight.SetLightColourChange(GameViewConstants::GetInstance()->BLACKOUT_PADDLE_KEY_LIGHT_COLOUR, toggleTime);
		this->paddleFillLight.SetLightColourChange(GameViewConstants::GetInstance()->BLACKOUT_PADDLE_FILL_LIGHT_COLOUR, toggleTime);
        this->bossKeyLight.SetLightColourChange(GameViewConstants::GetInstance()->BLACKOUT_BOSS_KEY_LIGHT_COLOUR, toggleTime);
        this->bossFillLight.SetLightColourChange(GameViewConstants::GetInstance()->BLACKOUT_BOSS_FILL_LIGHT_COLOUR, toggleTime);
	}
}

void GameLightAssets::ToggleLightsForBossDeath(bool turnOn, double toggleTime) {
	// For the fill and key lights of the foreground and background we simply turn them on or off...
	this->fgKeyLight.SetLightOn(turnOn,  toggleTime);
	this->fgFillLight.SetLightOn(turnOn, toggleTime);
	this->bgKeyLight.SetLightOn(turnOn,  toggleTime);
	this->bgFillLight.SetLightOn(turnOn, toggleTime);
    this->ballLight.SetLightOn(turnOn,   toggleTime);

    // .. we don't touch the lights that affect the paddle, ball or the boss
}

/**
 * Changes the light colour in the level, while making sure it doesn't change the light colour
 * if the lights are blacked out (i.e., it doesn't turn lights back on, you need to toggle the lights back on using
 * the toggleLight function instead).
 */
/*
void GameLightAssets::ChangeLightColour(GameLightType lightType, const Colour& newLightColour, float animationTime) {
	assert(animationTime >= 0.0f);

	// Grab the object light associated with the given type
	PointLight* light = this->GetLightFromType(lightType);
	assert(light != NULL);

	if (animationTime == 0.0f) {
		// Just change the colour, now
		light->SetDiffuseColour(newLightColour);
	}
	else {
		// Change the light colour
		light->SetLightColourChange(newLightColour, animationTime);
	}
}
*/

/**
 * Sets the lights to strobe a particular colour at some strobe time interval.
 */
void GameLightAssets::StartStrobeLight(GameLightType lightType, const Colour& strobeColour, float strobeTime) {
	assert(strobeTime >= 0.0f);

	// Grab the object light associated with the given type
	PointLight* light = this->GetLightFromType(lightType);
	assert(light != NULL);

	// Set the light colour to strobe
	light->SetLightStrobeOn(strobeColour, strobeTime);
}

/**
 * Stops the given light type from strobing.
 */
void GameLightAssets::StopStrobeLight(GameLightType lightType) {
	// Grab the object light associated with the given type
	PointLight* light = this->GetLightFromType(lightType);
	assert(light != NULL);
	light->SetLightStrobeOff();
}

/**
 * Moves the given light from its current position to the new position provided
 * in the amount of time given.
 */
void GameLightAssets::ChangeLightPositionAndAttenuation(GameLightType lightType, const Point3D& newPosition, 
                                                        float newAtten, float changeTime) {
	assert(changeTime >= 0.0f);
	assert(lightType != GameLightAssets::FGBallLight);

	// Grab the object light associated with the given type
	PointLight* light = this->GetLightFromType(lightType);
	assert(light != NULL);

	if (changeTime == 0.0f) {
		// Just change the position, now
		light->SetPosition(newPosition);
        light->SetLinearAttenuation(newAtten);
	}
	else {
		// Change the light position and animate it
		light->SetLightPositionChange(newPosition, changeTime);
        light->SetLinearAttenuationChange(newAtten, changeTime);
	}	
}

/**
 * Called to restore the lights position to the default when it has been changed.
 */
void GameLightAssets::RestoreLightPositionAndAttenuation(GameLightType lightType, float restoreTime) {
	assert(restoreTime >= 0.0f);

	switch (lightType) {
		case FGKeyLight:
			this->fgKeyLight.SetLightPositionChange(this->defaultFGKeyLightProperties.GetPosition(), restoreTime);
            this->fgKeyLight.SetLinearAttenuationChange(this->defaultFGKeyLightProperties.GetLinearAttenuation(), restoreTime);
			break;
		case FGFillLight:
			this->fgFillLight.SetLightPositionChange(this->defaultFGFillLightProperties.GetPosition(), restoreTime);
            this->fgFillLight.SetLinearAttenuationChange(this->defaultFGFillLightProperties.GetLinearAttenuation(), restoreTime);
			break;
		case BallKeyLight:
			this->ballKeyLight.SetLightPositionChange(this->defaultFGKeyLightProperties.GetPosition(), restoreTime);
            this->ballKeyLight.SetLinearAttenuationChange(this->defaultFGKeyLightProperties.GetLinearAttenuation(), restoreTime);
			break;
		case BallFillLight:
			this->ballFillLight.SetLightPositionChange(this->defaultFGFillLightProperties.GetPosition(), restoreTime);
            this->ballFillLight.SetLinearAttenuationChange(this->defaultFGFillLightProperties.GetLinearAttenuation(), restoreTime);
			break;
		case PaddleKeyLight:
			this->paddleKeyLight.SetLightPositionChange(this->defaultFGKeyLightProperties.GetPosition(), restoreTime);
            this->paddleKeyLight.SetLinearAttenuationChange(this->defaultFGKeyLightProperties.GetLinearAttenuation(), restoreTime);
			break;
		case PaddleFillLight:
			this->paddleFillLight.SetLightPositionChange(this->defaultFGFillLightProperties.GetPosition(), restoreTime);
            this->paddleFillLight.SetLinearAttenuationChange(this->defaultFGFillLightProperties.GetLinearAttenuation(), restoreTime);
			break;
        case BossKeyLight:
            this->bossKeyLight.SetLightPositionChange(this->defaultFGKeyLightProperties.GetPosition(), restoreTime);
            this->bossKeyLight.SetLinearAttenuationChange(this->defaultFGKeyLightProperties.GetLinearAttenuation(), restoreTime);
            break;
        case BossFillLight:
            this->bossFillLight.SetLightPositionChange(this->defaultFGFillLightProperties.GetPosition(), restoreTime);
            this->bossFillLight.SetLinearAttenuationChange(this->defaultFGFillLightProperties.GetLinearAttenuation(), restoreTime);
            break;

        case BGKeyLight:
            this->bgKeyLight.SetLightPositionChange(this->defaultBGKeyLightProperties.GetPosition(), restoreTime);
            this->bgKeyLight.SetLinearAttenuationChange(this->defaultBGKeyLightProperties.GetLinearAttenuation(), restoreTime);
            break;
        case BGFillLight:
            this->bgFillLight.SetLightPositionChange(this->defaultBGFillLightProperties.GetPosition(), restoreTime);
            this->bgFillLight.SetLinearAttenuationChange(this->defaultBGFillLightProperties.GetLinearAttenuation(), restoreTime);
            break;

		default:
			assert(false);
			break;
	}
}

void GameLightAssets::RestoreLightColour(GameLightType lightType, float restoreTime) {
    assert(restoreTime >= 0.0f);

    switch (lightType) {
        case FGKeyLight:
            this->fgKeyLight.SetLightColourChange(this->defaultFGKeyLightProperties.GetDiffuseColour(), restoreTime);
            break;
        case FGFillLight:
            this->fgFillLight.SetLightColourChange(this->defaultFGFillLightProperties.GetDiffuseColour(), restoreTime);
            break;
        case BallKeyLight:
            this->ballKeyLight.SetLightColourChange(this->defaultFGKeyLightProperties.GetDiffuseColour(), restoreTime);
            break;
        case BallFillLight:
            this->ballFillLight.SetLightColourChange(this->defaultFGFillLightProperties.GetDiffuseColour(), restoreTime);
            break;
        case PaddleKeyLight:
            this->paddleKeyLight.SetLightColourChange(this->defaultFGKeyLightProperties.GetDiffuseColour(), restoreTime);
            break;
        case PaddleFillLight:
            this->paddleFillLight.SetLightColourChange(this->defaultFGFillLightProperties.GetDiffuseColour(), restoreTime);
            break;
        case BossKeyLight:
            this->bossKeyLight.SetLightColourChange(this->defaultFGKeyLightProperties.GetDiffuseColour(), restoreTime);
            break;
        case BossFillLight:
            this->bossFillLight.SetLightColourChange(this->defaultFGFillLightProperties.GetDiffuseColour(), restoreTime);
            break;

        case BGKeyLight:
            this->bgKeyLight.SetLightColourChange(this->defaultBGKeyLightProperties.GetDiffuseColour(), restoreTime);
            break;
        case BGFillLight:
            this->bgFillLight.SetLightColourChange(this->defaultBGFillLightProperties.GetDiffuseColour(), restoreTime);
            break;

        default:
            assert(false);
            break;
    }
}

/**
 * Called every frame with the delta time in seconds between frames - this will tick
 * all animations of the lights and anything else that needs to be informed of the
 * elapse of time.
 */
void GameLightAssets::Tick(double dT) {
	// Tick each light object...
	this->fgKeyLight.Tick(dT);
	this->fgFillLight.Tick(dT);
	this->ballLight.Tick(dT);

	this->bgFillLight.Tick(dT);
	this->bgKeyLight.Tick(dT);

	this->ballKeyLight.Tick(dT);
	this->ballFillLight.Tick(dT);
	this->paddleKeyLight.Tick(dT);
	this->paddleFillLight.Tick(dT);

    this->bossKeyLight.Tick(dT);
    this->bossFillLight.Tick(dT);
}

/**
 * Get the lights only affecting the game/level pieces.
 */
void GameLightAssets::GetPieceAffectingLights(BasicPointLight& fgKeyLight, BasicPointLight& fgFillLight, BasicPointLight& ballLight) const {
	this->fgKeyLight.ConvertToBasicPointLight(fgKeyLight);
	this->fgFillLight.ConvertToBasicPointLight(fgFillLight);
	this->ballLight.ConvertToBasicPointLight(ballLight);
}

/**
 * Get the lights only affecting the game ball(s).
 */
void GameLightAssets::GetBallAffectingLights(BasicPointLight& ballKeyLight, BasicPointLight& ballFillLight) const {
	this->ballKeyLight.ConvertToBasicPointLight(ballKeyLight);
	this->ballFillLight.ConvertToBasicPointLight(ballFillLight);
}

/**
 * Get the lights only affecting the player paddle.
 */
void GameLightAssets::GetPaddleAffectingLights(BasicPointLight& paddleKeyLight, BasicPointLight& paddleFillLight, BasicPointLight& ballLight) const {
	this->paddleKeyLight.ConvertToBasicPointLight(paddleKeyLight);
	this->paddleFillLight.ConvertToBasicPointLight(paddleFillLight);
	this->ballLight.ConvertToBasicPointLight(ballLight);
}

void GameLightAssets::GetBossAffectingLights(BasicPointLight& bossKeyLight, BasicPointLight& bossFillLight, BasicPointLight& ballLight) const {
    this->bossKeyLight.ConvertToBasicPointLight(bossKeyLight);
	this->bossFillLight.ConvertToBasicPointLight(bossFillLight);
	this->ballLight.ConvertToBasicPointLight(ballLight);
}

/**
 * Get the lights only affecting the background.
 */
void GameLightAssets::GetBackgroundAffectingLights(BasicPointLight& bgKeyLight, BasicPointLight& bgFillLight) const {
	this->bgKeyLight.ConvertToBasicPointLight(bgKeyLight);
	this->bgFillLight.ConvertToBasicPointLight(bgFillLight);
}

void GameLightAssets::SetBackgroundLightDefaults(const BasicPointLight& bgKeyAttributes,
                                                 const BasicPointLight& bgFillAttributes) {

    this->defaultBGKeyLightProperties  = bgKeyAttributes;
    this->defaultBGFillLightProperties = bgFillAttributes;
    this->bgKeyLight.SetFromBasicPointLight(bgKeyAttributes);
    this->bgFillLight.SetFromBasicPointLight(bgFillAttributes);
}

void GameLightAssets::SetForegroundLightDefaults(const BasicPointLight& fgKeyAttributes,
                                                 const BasicPointLight& fgFillAttributes) {

    this->defaultFGKeyLightProperties  = fgKeyAttributes;
    this->defaultFGFillLightProperties = fgFillAttributes;

    this->fgKeyLight.SetFromBasicPointLight(fgKeyAttributes);
    this->ballKeyLight.SetFromBasicPointLight(fgKeyAttributes);
    this->paddleKeyLight.SetFromBasicPointLight(fgKeyAttributes);
    this->bossKeyLight.SetFromBasicPointLight(fgKeyAttributes);

    this->fgFillLight.SetFromBasicPointLight(fgFillAttributes);
    this->ballFillLight.SetFromBasicPointLight(fgFillAttributes);
    this->paddleFillLight.SetFromBasicPointLight(fgFillAttributes);
    this->bossFillLight.SetFromBasicPointLight(fgFillAttributes);

    this->ballKeyLight.SetDiffuseColour(GameViewConstants::GetInstance()->DEFAULT_BALL_KEY_LIGHT_COLOUR);
    this->ballKeyLight.SetLinearAttenuation(GameViewConstants::GetInstance()->DEFAULT_BALL_KEY_LIGHT_ATTEN);
    this->ballFillLight.SetDiffuseColour(GameViewConstants::GetInstance()->DEFAULT_BALL_FILL_LIGHT_COLOUR);
    this->ballFillLight.SetLinearAttenuation(GameViewConstants::GetInstance()->DEFAULT_BALL_FILL_LIGHT_ATTEN);

    this->paddleKeyLight.SetDiffuseColour(GameViewConstants::GetInstance()->DEFAULT_PADDLE_KEY_LIGHT_COLOUR);
    this->paddleFillLight.SetDiffuseColour(GameViewConstants::GetInstance()->DEFAULT_PADDLE_FILL_LIGHT_COLOUR);

    this->bossKeyLight.SetDiffuseColour(GameViewConstants::GetInstance()->DEFAULT_BOSS_KEY_LIGHT_COLOUR);
    this->bossFillLight.SetDiffuseColour(GameViewConstants::GetInstance()->DEFAULT_BOSS_FILL_LIGHT_COLOUR);
}

/**
 * Private helper function to obtain the point light object based on the given light type.
 */
PointLight* GameLightAssets::GetLightFromType(GameLightType lightType) {
	// Get the light based on the type given
	switch (lightType) {
		case FGKeyLight:
			return &this->fgKeyLight;
		case FGFillLight:
			return &this->fgFillLight;
		case FGBallLight:
			return &this->ballLight;
		case BGKeyLight:
			return &this->bgKeyLight;
		case BGFillLight:
			return &this->bgFillLight;
		case BallKeyLight:
			return &this->ballKeyLight;
		case BallFillLight:
			return &this->ballFillLight;
		case PaddleKeyLight:
			return &this->paddleKeyLight;
		case PaddleFillLight:
			return &this->paddleFillLight;
        case BossKeyLight:
            return &this->bossKeyLight;
        case BossFillLight:
            return &this->bossFillLight;
		default:
			assert(false);
			return NULL;
	}
}

#ifdef _DEBUG
/**
 * Debug function for drawing the lights that affect the game - draws them as
 * coloured cubes.
 */
void GameLightAssets::DebugDrawLights() const {
	if (!GameDisplay::IsDrawDebugLightGeometryOn()) { return; }

	this->fgKeyLight.DebugDraw();
	this->fgFillLight.DebugDraw();
	this->ballLight.DebugDraw();
}
#endif