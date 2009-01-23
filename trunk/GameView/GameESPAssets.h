#ifndef __GAMESPASSETS_H__
#define __GAMESPASSETS_H__

#include <list>

#include "CgFXVolumetricEffect.h"

#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Camera.h"

#include "../ESPEngine/ESPParticleColourEffector.h"
#include "../ESPEngine/ESPParticleScaleEffector.h"
#include "../ESPEngine/ESPParticleAccelEffector.h"

class LevelPiece;
class GameBall;
class GameItem;
class ESPEmitter;
class ESPPointEmitter;
class Texture2D;
class ESPShaderParticle;

/**
 * Stores, draws and changes emitter/sprite/particle assets for the game.
 */
class GameESPAssets {

private:
	// Currently active particle systems
	std::list<ESPEmitter*> activeGeneralEmitters;
	std::map<const GameItem*, std::list<ESPEmitter*>> activeItemDropEmitters; 

	// Standard effectors for the various ESP effects
	ESPParticleColourEffector particleFader;

	ESPParticleColourEffector particleFaderUberballTrail;
	ESPParticleScaleEffector particlePulseUberballAura;
	ESPParticleScaleEffector particlePulseItemDropAura;
	ESPParticleScaleEffector particleShrinkToNothing;

	ESPParticleAccelEffector ghostBallAccel1;

	ESPParticleScaleEffector particleSmallGrowth;
	ESPParticleScaleEffector particleMediumGrowth;


	// Various textures used as particles for various effects
	std::vector<Texture2D*> bangTextures;
	std::vector<Texture2D*> smokeTextures;
	Texture2D* circleGradientTex;
	Texture2D* starTex;
	Texture2D* starOutlineTex;
	//Texture2D* circleBullseyeTex;
	//Texture2D* circleOutlineTex;



	// Standalone ESP effects
	ESPPointEmitter* uberBallEmitterAura;
	ESPPointEmitter* uberBallEmitterTrail;
	
	//ESPPointEmitter* ghostBallEmitterAura;

	std::vector<ESPShaderParticle*> ghostSmokeParticles;
	CgFxVolumetricEffect ghostBallSmoke;
	ESPPointEmitter* ghostBallEmitterTrail;


	// Used for tweaking...
	Vector2D oldBallDir;
	void EffectsToResetOnBallVelChange();

	// Initialization functions for effect stuffs
	void InitESPTextures();
	void InitStandaloneESPEffects();

	void InitUberBallESPEffects();
	void InitGhostBallESPEffects();

public:
	GameESPAssets();
	~GameESPAssets();

	// Specific effects that can be made to occur in the game
	void AddBallBounceEffect(const Camera& camera, const GameBall& ball);
	void AddBasicBlockBreakEffect(const Camera& camera, const LevelPiece& block);
	void AddItemAcquiredEffect(const Camera& camera, const GameItem& item);
	void SetItemEffect(const GameItem& item, bool activate);

	void AddItemDropEffect(const Camera& camera, const GameItem& item);
	void RemoveItemDropEffect(const Camera& camera, const GameItem& item);

	void KillAllActiveEffects();

	// Draw functions for various particle effects in the game
	void DrawParticleEffects(double dT, const Camera& camera);
	void DrawItemDropEffects(double dT, const Camera& camera, const GameItem& item);
	void DrawUberBallEffects(double dT, const Camera& camera, const GameBall& ball);
	void DrawGhostBallEffects(double dT, const Camera& camera, const GameBall& ball);


};
#endif