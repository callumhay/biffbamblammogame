#ifndef __GAMESPASSETS_H__
#define __GAMESPASSETS_H__

#include <list>

#include "CgFXVolumetricEffect.h"
#include "CgFXPostRefract.h"

#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Camera.h"

#include "../ESPEngine/ESPParticleColourEffector.h"
#include "../ESPEngine/ESPParticleScaleEffector.h"
#include "../ESPEngine/ESPParticleAccelEffector.h"
#include "../ESPEngine/ESPParticleRotateEffector.h"

class LevelPiece;
class GameBall;
class GameItem;
class ESPEmitter;
class ESPPointEmitter;
class Texture;
class ESPShaderParticle;
class Projectile;
class PlayerPaddle;
class GameModel;

/**
 * Stores, draws and changes emitter/sprite/particle assets for the game.
 */
class GameESPAssets {

private:
	// Currently active particle systems
	std::list<ESPEmitter*> activeGeneralEmitters;
	std::list<ESPEmitter*> activePaddleEmitters;
	std::map<const GameBall*, std::list<ESPEmitter*>>		activeBallBGEmitters;
	std::map<const GameItem*, std::list<ESPEmitter*>>		activeItemDropEmitters; 
	std::map<const Projectile*, std::list<ESPEmitter*>> activeProjectileEmitters;
	
	// Standard effectors for the various ESP effects
	ESPParticleColourEffector particleFader;
	ESPParticleColourEffector particleFireColourFader;
	ESPParticleColourEffector particleCloudColourFader;

	ESPParticleColourEffector particleFaderUberballTrail;
	ESPParticleScaleEffector  particlePulseUberballAura;
	ESPParticleScaleEffector  particlePulseItemDropAura;
	ESPParticleScaleEffector  particlePulsePaddleLaser;
	ESPParticleScaleEffector  particleShrinkToNothing;

	ESPParticleAccelEffector ghostBallAccel1;

	ESPParticleScaleEffector particleSmallGrowth;
	ESPParticleScaleEffector particleMediumGrowth;
	ESPParticleScaleEffector particleLargeGrowth;
	ESPParticleScaleEffector particleMediumShrink;
	//ESPParticleScaleEffector particleLargeStretch;

	ESPParticleRotateEffector explosionRayRotatorCW;
	ESPParticleRotateEffector explosionRayRotatorCCW;

	// Various textures used as particles for various effects
	std::vector<Texture2D*> bangTextures;
	std::vector<Texture2D*> smokeTextures;
	Texture2D* circleGradientTex;
	Texture2D* starTex;
	Texture2D* starOutlineTex;
	Texture2D* explosionTex;
	Texture2D* explosionRayTex;
	Texture2D* laserBeamTex;
	Texture2D* upArrowTex;

	// Ball related ESP effects - stores each balls set of item-related (defined by unique string ID) effects
	std::map<const GameBall*, std::map<std::string, std::vector<ESPPointEmitter*>>> ballEffects;

	// Constants for the number of particles for particular effects
	static const int NUM_PADDLE_LASER_SPARKS = 15;
	static const int NUM_GHOST_SMOKE_PARTICLES = 23;
	static const int NUM_UBER_BALL_TRAIL_PARTICLES = 35;
	static const int NUM_EXPLOSION_FIRE_CLOUD_PARTICLES  = 25;
	static const int NUM_LASER_VAPOUR_TRAIL_PARTICLES = 15;
	static const int NUM_PADDLE_SIZE_CHANGE_PARTICLES = 24;
	static const int NUM_BALL_SIZE_CHANGE_PARTICLES = 20;
	static const int NUM_ITEM_ACQUIRED_SPARKS = 20;

	ESPPointEmitter* paddleLaserGlowAura;
	ESPPointEmitter* paddleLaserGlowSparks;

	CgFxVolumetricEffect ghostBallSmoke;
	CgFxVolumetricEffect fireEffect;
	CgFxPostRefract vapourTrailEffect;

	// Initialization functions for effect stuffs
	void InitESPTextures();
	void InitStandaloneESPEffects();

	void AddUberBallESPEffects(std::vector<ESPPointEmitter*>& effectsList);
	void AddGhostBallESPEffects(std::vector<ESPPointEmitter*>& effectsList);

	void AddPaddleGrowEffect();
	void AddPaddleShrinkEffect();
	void AddBallGrowEffect(const GameBall* ball);
	void AddBallShrinkEffect(const GameBall* ball);
	void AddOneUpEffect(const PlayerPaddle* paddle);

	void InitLaserPaddleESPEffects();

	void AddLaserPaddleESPEffects(const Projectile& projectile);
	void DrawProjectileEffects(double dT, const Camera& camera);
	void DrawProjectileEmitter(double dT, const Camera& camera, const Point2D& projectilePos2D, ESPEmitter* projectileEmitter);

public:
	GameESPAssets();
	~GameESPAssets();

	// Specific effects that can be made to occur in the game
	void AddBallBounceEffect(const Camera& camera, const GameBall& ball);
	void AddBasicBlockBreakEffect(const Camera& camera, const LevelPiece& block);
	void AddBombBlockBreakEffect(const Camera& camera, const LevelPiece& bomb);
	void AddBallSafetyNetDestroyedEffect(const GameBall& ball);
	void AddPaddleHitWallEffect(const PlayerPaddle& paddle, const Point2D& hitLoc);
	void AddItemAcquiredEffect(const Camera& camera, const PlayerPaddle& paddle, const GameItem& item);
	void SetItemEffect(const GameItem& item, const GameModel& gameModel);
	void AddItemDropEffect(const Camera& camera, const GameItem& item);
	void RemoveItemDropEffect(const Camera& camera, const GameItem& item);
	void AddProjectileEffect(const Camera& camera, const Projectile& projectile);
	void RemoveProjectileEffect(const Camera& camera, const Projectile& projectile);

	void KillAllActiveEffects();
	void KillAllActiveBallEffects(const GameBall& ball);

	// Draw functions for various particle effects in the game
	void DrawParticleEffects(double dT, const Camera& camera);
	void DrawPostProcessingESPEffects(double dT, const Camera& camera, const Texture2D* sceneTex);

	void DrawItemDropEffects(double dT, const Camera& camera, const GameItem& item);

	void DrawUberBallEffects(double dT, const Camera& camera, const GameBall& ball);
	void DrawGhostBallEffects(double dT, const Camera& camera, const GameBall& ball);
	
	void DrawBackgroundBallEffects(double dT, const Camera& camera, const GameBall& ball);
	void DrawBackgroundPaddleEffects(double dT, const Camera& camera, const PlayerPaddle& paddle);
	void DrawPaddleLaserEffects(double dT, const Camera& camera, const PlayerPaddle& paddle);

};
#endif