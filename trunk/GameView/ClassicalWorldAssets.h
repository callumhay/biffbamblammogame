/**
 * ClassicalWorldAssets.h
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

	void Tick(double dT, const GameModel& model);

private:
    std::vector<Texture2D*> cloudTextures;

    CgFxVolumetricEffect fireEffect;
    ESPPointEmitter fireEmitter1, fireEmitter2;

    ESPParticleAccelEffector fireAccel1, fireAccel2;
    ESPParticleColourEffector fireColourFader;
    ESPParticleScaleEffector fireParticleScaler;

    void LoadBGLighting(GameAssets* assets) const;
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

inline void ClassicalWorldAssets::Tick(double dT, const GameModel& model) {
    GameWorldAssets::Tick(dT, model);

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