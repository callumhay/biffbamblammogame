/**
 * GameLightAssets.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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
    static const float DEFAULT_LIGHT_TOGGLE_TIME;

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

	void ToggleLights(bool turnOn, double toggleTime);
    void ToggleLightsForBossDeath(bool turnOn, double toggleTime);

	//void ChangeLightColour(GameLightType lightType, const Colour& newLightColour, float animationTime);
	void StartStrobeLight(GameLightType lightType, const Colour& strobeColour, float strobeTime);
	void StopStrobeLight(GameLightType lightType);
	
	void ChangeLightPositionAndAttenuation(GameLightType lightType, const Point3D& newPosition, float newAtten, float changeTime);
	void RestoreLightPositionAndAttenuation(GameLightType lightType, float restoreTime);

	void Tick(double dT);

	void GetPieceAffectingLights(BasicPointLight& fgKeyLight, BasicPointLight& fgFillLight, BasicPointLight& ballLight) const;
	void GetBallAffectingLights(BasicPointLight& ballKeyLight, BasicPointLight& ballFillLight) const;
	void GetPaddleAffectingLights(BasicPointLight& paddleKeyLight, BasicPointLight& paddleFillLight, BasicPointLight& ballLight) const;
	void GetBossAffectingLights(BasicPointLight& bossKeyLight, BasicPointLight& bossFillLight, BasicPointLight& ballLight) const;
    void GetBackgroundAffectingLights(BasicPointLight& bgKeyLight, BasicPointLight& bgFillLight) const;
	
    PointLight& GetBallLight() { return this->ballLight; }
    PointLight& GetFGKeyLight() { return this->fgKeyLight; }

    void SetBackgroundLightDefaults(const BasicPointLight& bgKeyAttributes, const BasicPointLight& bgFillAttributes);
    void SetForegroundLightDefaults(const BasicPointLight& fgKeyAttributes, const BasicPointLight& fgFillAttributes);

#ifdef _DEBUG
	void DebugDrawLights() const;
#endif

private:
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
    // Lights just for the boss
    PointLight bossKeyLight, bossFillLight;

	PointLight* GetLightFromType(GameLightType lightType);

    BasicPointLight defaultBGKeyLightProperties;
    BasicPointLight defaultBGFillLightProperties;

    BasicPointLight defaultFGKeyLightProperties;
    BasicPointLight defaultFGFillLightProperties;

    DISALLOW_COPY_AND_ASSIGN(GameLightAssets);
};

#endif