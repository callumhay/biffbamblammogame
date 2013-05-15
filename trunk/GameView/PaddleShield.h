/**
 * PaddleShield.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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
	ESPPointEmitter* auraEnergyOut;						// Aura energy the emits out from the shield
	ESPParticleColourEffector particleFader;
	ESPParticleScaleEffector  particleShrink;
	ESPParticleScaleEffector particleGrowth;
	
	AnimationLerp<float> shieldSizeAnimation;

	enum ShieldState { Deactivated = 0, Activating, Sustained, Deactivating };
	ShieldState state;

	void DrawRefractionWithAura(const PlayerPaddle& paddle, const Camera& camera, double dT);

	// Disallow copy and assign
	PaddleShield& operator=(const PaddleShield& p);
	PaddleShield(const PaddleShield& p);
};

#endif // __PADDLESHIELD_H__