/**
 * GameLightAssets.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __GAMELIGHTASSETS_H__
#define __GAMELIGHTASSETS_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Light.h"
#include "../BlammoEngine/Animation.h"

/**
 * Helper class that contains light assets for the game. This is responsible for
 * controlling all animations, attributes and events associated with lights as the game plays.
 */
class GameLightAssets {
	
public:
	enum GameLightType { 
        FGKeyLight,         // Foreground key light  (key light affecting all foreground objects except the paddle and ball)
        FGFillLight,        // Foreground fill light (fill light affecting all foreground objects except the paddle and ball)
        FGBallLight,        // Foreground ball light (light coming from the ball that affects all foreground objects except the ball)
        BGKeyLight,         // Background key light  (key light affecting all background objects)
        BGFillLight,        // Background fill light (fill light affecting all background objects)
        BallKeyLight,       // Key light affecting only the ball
        BallFillLight,      // Fill light affecting only the ball
        PaddleKeyLight,     // Key light affecting only the paddle
        PaddleFillLight     // Fill light affecting only the paddle
    };

	GameLightAssets();
	~GameLightAssets();

	void ToggleLights(bool turnOn);
	//void ChangeLightColour(GameLightType lightType, const Colour& newLightColour, float animationTime);
	void StartStrobeLight(GameLightType lightType, const Colour& strobeColour, float strobeTime);
	void StopStrobeLight(GameLightType lightType);
	
	void ChangeLightPosition(GameLightType lightType, const Point3D& newPosition, float changeTime);
	void RestoreLightPosition(GameLightType lightType, float restoreTime);

	void Tick(double dT);

	void GetPieceAffectingLights(BasicPointLight& fgKeyLight, BasicPointLight& fgFillLight, BasicPointLight& ballLight) const;
	void GetBallAffectingLights(BasicPointLight& ballKeyLight, BasicPointLight& ballFillLight) const;
	void GetPaddleAffectingLights(BasicPointLight& paddleKeyLight, BasicPointLight& paddleFillLight, BasicPointLight& ballLight) const;
	void GetBackgroundAffectingLights(BasicPointLight& bgKeyLight, BasicPointLight& bgFillLight) const;
	PointLight& GetBallLight() { return this->ballLight; };

	bool GetIsBlackOutActive() const {
		return this->isBlackOutActive;
	}

#ifdef _DEBUG
	void DebugDrawLights() const;
#endif

private:
	bool isBlackOutActive;	// Whether or not all lights are in black-out mode

	// Lights for typical foreground geometry: key, fill and ball
	PointLight fgKeyLight;		// The key light for all general foreground geometry (not including ball(s) and paddle)
	PointLight fgFillLight;		// The fill light for all general foregorund geometry (not including ball(s) and paddle)
	PointLight ballLight;			// The light emitted from the center of the ball or avg. center of all balls

	// Lights for the background geometry
	PointLight bgFillLight, bgKeyLight;
	// Lights just for the ball
	PointLight ballKeyLight, ballFillLight;
	// Lights just for the paddle
	PointLight paddleKeyLight, paddleFillLight;

	PointLight* GetLightFromType(GameLightType lightType);

};
#endif