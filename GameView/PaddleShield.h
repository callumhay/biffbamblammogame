/**
 * PaddleShield.h
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

#ifndef __PADDLESHIELD_H__
#define __PADDLESHIELD_H__

#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Animation.h"
#include "../ESPEngine/ESPParticleColourEffector.h"
#include "../ESPEngine/ESPParticleScaleEffector.h"

class Texture2D;
class CgFxPostRefract;
class ESPPointEmitter;
class PlayerPaddle;
class Camera;

class PaddleShield {
public:
	PaddleShield();
	~PaddleShield();

    void SetAlpha(float alpha);

	void ActivateShield(const PlayerPaddle& paddle, const Texture2D& bgTexture);
	void DeactivateShield();

	void DrawAndTick(const PlayerPaddle& paddle, const Camera& camera, double dT);

private:
	const Colour SHIELD_COLOUR;

	static const double ACTIVATING_TIME;
	static const double DEACTIVATING_TIME;

	// Textures and effects used to display the shield
	Texture2D* sphereNormalsTex;
	CgFxPostRefract* shieldRefractEffect;
	
	Texture2D* sparkleTex;
	Texture2D* haloTex;
	//ESPPointEmitter* paddleShieldAura;

	ESPPointEmitter* particleEnergyInAndOut;	// Particles that get sucked toward the paddle when the shield forms
	ESPParticleColourEffector particleFader;
	ESPParticleScaleEffector  particleShrink;
	ESPParticleScaleEffector particleGrowth;
	
	AnimationLerp<float> shieldSizeAnimation;
    AnimationMultiLerp<float> shieldGlowAnimation;

	enum ShieldState { Deactivated = 0, Activating, Sustained, Deactivating };
	ShieldState state;

    float alpha;

	void DrawRefractionWithAura(const PlayerPaddle& paddle, const Camera& camera, double dT);

	// Disallow copy and assign
    DISALLOW_COPY_AND_ASSIGN(PaddleShield);
};

inline void PaddleShield::SetAlpha(float alpha) {
    assert(alpha >= 0.0f && alpha <= 1.0f);
    this->alpha = alpha;
}

#endif // __PADDLESHIELD_H__