/**
 * ClassicalWorldAssets.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __CLASSICALWORLDASSETS_H__
#define __CLASSICALWORLDASSETS_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Animation.h"

#include "GameWorldAssets.h"
#include "CgFxVolumetricEffect.h"

#include "../ESPEngine/ESP.h"

class ClassicalWorldAssets : public GameWorldAssets {
public:
	ClassicalWorldAssets(GameAssets* assets);
	~ClassicalWorldAssets();

	GameWorld::WorldStyle GetStyle() const;
	void DrawBackgroundModel(const Camera& camera, const BasicPointLight& bgKeyLight,
        const BasicPointLight& bgFillLight);
	void DrawBackgroundEffects(const Camera& camera);
	void FadeBackground(bool fadeout, float fadeTime);
	void ResetToInitialState();

	void Tick(double dT);

private:
    Texture2D* cloudTex;

    CgFxVolumetricEffect fireEffect;
    ESPPointEmitter fireEmitter1, fireEmitter2;

    ESPParticleAccelEffector fireAccel1, fireAccel2;
    ESPParticleColourEffector fireColourFader;
    ESPParticleScaleEffector fireParticleScaler;

    void InitializeEmitters();

    DISALLOW_COPY_AND_ASSIGN(ClassicalWorldAssets);
};

inline GameWorld::WorldStyle ClassicalWorldAssets::GetStyle() const {
	return GameWorld::Classical;
}

inline void ClassicalWorldAssets::FadeBackground(bool fadeout, float fadeTime) {
    GameWorldAssets::FadeBackground(fadeout, fadeTime);
}

inline void ClassicalWorldAssets::ResetToInitialState() {
    GameWorldAssets::ResetToInitialState();
}

inline void ClassicalWorldAssets::DrawBackgroundEffects(const Camera& camera) {
    this->fireEmitter1.Draw(camera);
    this->fireEmitter2.Draw(camera);
}

inline void ClassicalWorldAssets::Tick(double dT) {
    GameWorldAssets::Tick(dT);

    static const float CONE_SIZE_IN_DEGREES = 10.0f;
    static const float ACCEL_MAG = 12.0f;
	
    // Rotate the up direction by some random amount and then affect the fire particle's velocities
	// by it, this gives the impression that the particles are flickering like fire
	double randomDegrees = Randomizer::GetInstance()->RandomNumNegOneToOne() * CONE_SIZE_IN_DEGREES;
	Vector2D accelVec = ACCEL_MAG * Vector2D::Rotate(static_cast<float>(randomDegrees), Vector2D(0, 1));
	this->fireAccel1.SetAcceleration(Vector3D(accelVec, 0.0f));

	randomDegrees = Randomizer::GetInstance()->RandomNumNegOneToOne() * CONE_SIZE_IN_DEGREES;
	accelVec = ACCEL_MAG * Vector2D::Rotate(static_cast<float>(randomDegrees), Vector2D(0, 1));
	this->fireAccel2.SetAcceleration(Vector3D(accelVec, 0.0f));

    // Tick the emitters
    float alpha = this->bgFadeAnim.GetInterpolantValue();
    this->fireEffect.SetAlphaMultiplier(alpha);
    this->fireEmitter1.SetParticleAlpha(ESPInterval(alpha));
    this->fireEmitter1.Tick(dT);
    this->fireEmitter2.SetParticleAlpha(ESPInterval(alpha));
    this->fireEmitter2.Tick(dT);   
}

#endif // __CLASSICALWORLDASSETS_H__