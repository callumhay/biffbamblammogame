#include "GameLightAssets.h"
#include "GameViewConstants.h"
#include "GameDisplay.h"

GameLightAssets::GameLightAssets() {

	// Initialize default light values
	// Foreground lights:
	this->fgKeyLight  = PointLight(GameViewConstants::GetInstance()->DEFAULT_FG_KEY_LIGHT_POSITION, 
		GameViewConstants::GetInstance()->DEFAULT_FG_KEY_LIGHT_COLOUR, 0.0f);
	this->fgFillLight = PointLight(GameViewConstants::GetInstance()->DEFAULT_FG_FILL_LIGHT_POSITION, 
		GameViewConstants::GetInstance()->DEFAULT_FG_FILL_LIGHT_COLOUR,  0.037f);

	this->ballLight		= PointLight(Point3D(0,0,0), 
		GameViewConstants::GetInstance()->DEFAULT_BALL_LIGHT_COLOUR, 
		GameViewConstants::GetInstance()->DEFAULT_BALL_LIGHT_ATTEN);
	
	// Background lights:
	this->bgKeyLight	= PointLight(GameViewConstants::GetInstance()->DEFAULT_BG_KEY_LIGHT_POSITION, 
		GameViewConstants::GetInstance()->DEFAULT_BG_KEY_LIGHT_COLOUR, 0.0f);

	this->bgFillLight = PointLight(GameViewConstants::GetInstance()->DEFAULT_BG_FILL_LIGHT_POSITION, 
		GameViewConstants::GetInstance()->DEFAULT_BG_FILL_LIGHT_COLOUR,  0.025f);

	// Ball and paddle specific foreground lights
	this->ballKeyLight	= this->fgKeyLight;
	this->ballKeyLight.SetDiffuseColour(GameViewConstants::GetInstance()->DEFAULT_BALL_KEY_LIGHT_COLOUR);
	this->ballFillLight	= this->fgFillLight;
	this->ballFillLight.SetDiffuseColour(Colour(0,0,0));
	this->paddleKeyLight = this->fgKeyLight;
	this->paddleFillLight = this->fgFillLight;
}

GameLightAssets::~GameLightAssets() {
}

/**
 * Toggle the lights in the game either on or off (both foreground
 * and background lights are affected).
 */
void GameLightAssets::ToggleLights(bool turnOn) {
	// For the fill and key lights of the foreground and background we simply
	// turn them on or off...
	this->fgKeyLight.SetLightOn(turnOn, 1.0f);
	this->fgFillLight.SetLightOn(turnOn, 1.0f);
	this->bgKeyLight.SetLightOn(turnOn, 1.0f);
	this->bgFillLight.SetLightOn(turnOn, 1.0f);

	// The ball light needs to change how much/well it illuminates the scene as well...
	if (turnOn) {
		this->ballLight.SetLinearAttenuation(GameViewConstants::GetInstance()->DEFAULT_BALL_LIGHT_ATTEN);
	}
	else {
		this->ballLight.SetLinearAttenuation(GameViewConstants::GetInstance()->BLACKOUT_BALL_LIGHT_ATTEN);
	}
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
void GameLightAssets::ChangeLightPosition(GameLightType lightType, const Point3D& newPosition, float changeTime) {
	assert(changeTime >= 0.0f);
	assert(lightType != GameLightAssets::FGBallLight);

	// Grab the object light associated with the given type
	PointLight* light = this->GetLightFromType(lightType);
	assert(light != NULL);

	if (changeTime == 0.0f) {
		// Just change the position, now
		light->SetPosition(newPosition);
	}
	else {
		// Change the light position and animate it
		light->SetLightPositionChange(newPosition, changeTime);
	}	
}

/**
 * Called to restore the lights position to the default when it has been changed.
 */
void GameLightAssets::RestoreLightPosition(GameLightType lightType, float restoreTime) {
	assert(restoreTime >= 0.0f);

	switch (lightType) {
		case FGKeyLight:
			this->fgKeyLight.SetLightPositionChange(GameViewConstants::GetInstance()->DEFAULT_FG_KEY_LIGHT_POSITION, restoreTime);
			break;
		case FGFillLight:
			this->fgFillLight.SetLightPositionChange(GameViewConstants::GetInstance()->DEFAULT_FG_FILL_LIGHT_POSITION, restoreTime);
			break;
		case BGKeyLight:
			this->bgKeyLight.SetLightPositionChange(GameViewConstants::GetInstance()->DEFAULT_BG_KEY_LIGHT_POSITION, restoreTime);
			break;
		case BGFillLight:
			this->bgFillLight.SetLightPositionChange(GameViewConstants::GetInstance()->DEFAULT_BG_FILL_LIGHT_POSITION, restoreTime);
			break;
		case BallKeyLight:
			this->ballKeyLight.SetLightPositionChange(GameViewConstants::GetInstance()->DEFAULT_BALL_KEY_LIGHT_POSITION, restoreTime);
			break;
		case BallFillLight:
			this->ballFillLight.SetLightPositionChange(GameViewConstants::GetInstance()->DEFAULT_BALL_FILL_LIGHT_POSITION, restoreTime);
			break;
		case PaddleKeyLight:
			this->paddleKeyLight.SetLightPositionChange(GameViewConstants::GetInstance()->DEFAULT_PADDLE_KEY_LIGHT_POSITION, restoreTime);
			break;
		case PaddleFillLight:
			this->paddleFillLight.SetLightPositionChange(GameViewConstants::GetInstance()->DEFAULT_PADDLE_FILL_LIGHT_POSITION, restoreTime);
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
}

/**
 * Get the lights only affecting the game/level pieces.
 */
void GameLightAssets::GetPieceAffectingLights(PointLight& fgKeyLight, PointLight& fgFillLight, PointLight& ballLight) const {
	fgKeyLight = this->fgKeyLight;
	fgFillLight = this->fgFillLight;
	ballLight = this->ballLight;
}

/**
 * Get the lights only affecting the game ball(s).
 */
void GameLightAssets::GetBallAffectingLights(PointLight& ballKeyLight, PointLight& ballFillLight) const {
	ballKeyLight = this->ballKeyLight;
	ballFillLight = this->ballFillLight;
}

/**
 * Get the lights only affecting the player paddle.
 */
void GameLightAssets::GetPaddleAffectingLights(PointLight& paddleKeyLight, PointLight& paddleFillLight, PointLight& ballLight) const {
	paddleKeyLight = this->paddleKeyLight;
	paddleFillLight = this->paddleFillLight;
	ballLight = this->ballLight;
}

/**
 * Get the lights only affecting the background.
 */
void GameLightAssets::GetBackgroundAffectingLights(PointLight& bgKeyLight, PointLight& bgFillLight) const {
	bgKeyLight = this->bgKeyLight;
	bgFillLight = this->bgFillLight;
}

/**
 * Private helper function to obtain the point light object based on the given light type.
 */
PointLight* GameLightAssets::GetLightFromType(GameLightType lightType) {
	// Get the light based on the type given
	switch (lightType) {
		case FGKeyLight:
			return &this->fgKeyLight;
			break;
		case FGFillLight:
			return &this->fgFillLight;
			break;
		case FGBallLight:
			return &this->ballLight;
			break;
		case BGKeyLight:
			return &this->bgKeyLight;
			break;
		case BGFillLight:
			return &this->bgFillLight;
			break;
		case BallKeyLight:
			return &this->ballKeyLight;
			break;
		case BallFillLight:
			return &this->ballFillLight;
			break;
		case PaddleKeyLight:
			return &this->paddleKeyLight;
			break;
		case PaddleFillLight:
			return &this->paddleFillLight;
			break;
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