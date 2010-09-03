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
};

#endif // __PADDLESHIELD_H__