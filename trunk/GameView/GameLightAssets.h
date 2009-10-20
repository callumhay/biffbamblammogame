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
	enum GameLightType { FGKeyLight,				// Foreground key light  (key light affecting all foreground objects except the paddle and ball)
											 FGFillLight,				// Foreground fill light (fill light affecting all foreground objects except the paddle and ball)
											 FGBallLight,				// Foreground ball light (light coming from the ball that affects all foreground objects except the ball)
											 BGKeyLight,				// Background key light  (key light affecting all background objects)
											 BGFillLight,				// Background fill light (fill light affecting all background objects)
											 BallKeyLight,			// Key light affecting only the ball
											 BallFillLight,			// Fill light affecting only the ball
											 PaddleKeyLight,		// Key light affecting only the paddle
											 PaddleFillLight		// Fill light affecting only the paddle
											};

	GameLightAssets();
	~GameLightAssets();

	void ToggleLights(bool turnOn);
	void ChangeLightColour(GameLightType lightType, const Colour& newLightColour, float animationTime);
	void StartStrobeLight(GameLightType lightType, const Colour& strobeColour, float strobeTime);
	void StopStrobeLight(GameLightType lightType);
	void MoveLight(GameLightType lightType, const Point3D& newPosition, float moveTime);

	void Tick(double dT);

	void GetPieceAffectingLights(PointLight& fgKeyLight, PointLight& fgFillLight, PointLight& ballLight) const;
	void GetBallAffectingLights(PointLight& ballKeyLight, PointLight& ballFillLight) const;
	void GetPaddleAffectingLights(PointLight& paddleKeyLight, PointLight& paddleFillLight, PointLight& ballLight) const;
	void GetBackgroundAffectingLights(PointLight& bgKeyLight, PointLight& bgFillLight) const;
	PointLight& GetBallLight() { return this->ballLight; };

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