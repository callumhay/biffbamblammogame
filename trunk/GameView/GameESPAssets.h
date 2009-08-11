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

#include "../GameModel/Onomatoplex.h"

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
class GameLevel;

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
	ESPParticleRotateEffector smokeRotatorCW;
	ESPParticleRotateEffector smokeRotatorCCW;
	ESPParticleRotateEffector loopRotateEffectorCW;

	// Various textures used as particles for various effects
	std::vector<Texture2D*> bangTextures;
	std::vector<Texture2D*> splatTextures;
	std::vector<Texture2D*> smokeTextures;
	
	Texture2D* circleGradientTex;
	Texture2D* starTex;
	Texture2D* starOutlineTex;
	Texture2D* evilStarTex;
	Texture2D* evilStarOutlineTex;
	Texture2D* explosionTex;
	Texture2D* explosionRayTex;
	Texture2D* laserBeamTex;
	Texture2D* upArrowTex;
	Texture2D* ballTex;
	Texture2D* ballTargetTex;

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
	static const int NUM_INK_CLOUD_PART_PARTICLES = 5;
	static const int NUM_INK_SPRAY_PARTICLES = 20;

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
	void AddPaddleCamBallESPEffects(std::vector<ESPPointEmitter*>& effectsList);

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
	//void AddBallBounceEffect(const Camera& camera, const GameBall& ball);	
	// TODO: 
	ESPPointEmitter* CreateBallBounceEffect(const GameBall& ball, Onomatoplex::SoundType soundType); 
	void AddBounceLevelPieceEffect(const Camera& camera, const GameBall& ball, const LevelPiece& block);
	void AddBouncePaddleEffect(const Camera& camera, const GameBall& ball, const PlayerPaddle& paddle);
	void AddBounceBallBallEffect(const Camera& camera, const GameBall& ball1, const GameBall& ball2);
	
	void AddBasicBlockBreakEffect(const Camera& camera, const LevelPiece& block);
	void AddBombBlockBreakEffect(const Camera& camera, const LevelPiece& bomb);
	void AddInkBlockBreakEffect(const Camera& camera, const LevelPiece& inkBlock, const GameLevel& level);
	void AddBallSafetyNetDestroyedEffect(const GameBall& ball);
	void AddPaddleHitWallEffect(const PlayerPaddle& paddle, const Point2D& hitLoc);
	void AddItemAcquiredEffect(const Camera& camera, const PlayerPaddle& paddle, const GameItem& item);
	void SetItemEffect(const GameItem& item, const GameModel& gameModel);
	void AddItemDropEffect(const Camera& camera, const GameItem& item, bool showStars);
	void RemoveItemDropEffect(const Camera& camera, const GameItem& item);
	void AddProjectileEffect(const Camera& camera, const Projectile& projectile);
	void RemoveProjectileEffect(const Camera& camera, const Projectile& projectile);

	void TurnOffCurrentItemDropStars(const Camera& camera);

	void KillAllActiveEffects();
	void KillAllActiveBallEffects(const GameBall& ball);

	// Draw functions for various particle effects in the game
	void DrawParticleEffects(double dT, const Camera& camera);

	void DrawItemDropEffects(double dT, const Camera& camera, const GameItem& item);

	void DrawUberBallEffects(double dT, const Camera& camera, const GameBall& ball);
	void DrawGhostBallEffects(double dT, const Camera& camera, const GameBall& ball);
	void DrawTargetBallEffects(double dT, const Camera& camera, const GameBall& ball, const PlayerPaddle& paddle);
	
	void DrawBackgroundBallEffects(double dT, const Camera& camera, const GameBall& ball);
	void DrawBackgroundPaddleEffects(double dT, const Camera& camera, const PlayerPaddle& paddle);
	void DrawPaddleLaserEffects(double dT, const Camera& camera, const PlayerPaddle& paddle);

};
#endif