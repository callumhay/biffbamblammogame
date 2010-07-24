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
#include "../GameModel/GameItem.h"

class LevelPiece;
class PortalBlock;
class GameBall;
class GameItem;
class ESPEmitter;
class ESPPointEmitter;
class ESPVolumeEmitter;
class Texture;
class ESPShaderParticle;
class Projectile;
class PlayerPaddle;
class GameModel;
class GameLevel;
class Beam;
struct ESPInterval;

/**
 * Stores, draws and changes emitter/sprite/particle assets for the game.
 */
class GameESPAssets {

private:
	// Currently active particle systems
	std::list<ESPEmitter*> activeGeneralEmitters;
	std::list<ESPEmitter*> activePaddleEmitters;
	std::map<const GameBall*, std::list<ESPEmitter*> > activeBallBGEmitters;
	std::map<const GameItem*, std::list<ESPEmitter*> > activeItemDropEmitters;
	std::map<const Projectile*, std::list<ESPPointEmitter*> > activeProjectileEmitters;
	std::map<const Beam*, std::list<ESPEmitter*> > activeBeamEmitters;
	std::map<GameItem::ItemType, std::list<ESPEmitter*> > activeTimerHUDEmitters;
	//std::map<const LevelPiece*, std::list<ESPEmitter*> > activeLevelPieceEmitters;
	
	// Standard effectors for the various ESP effects
	ESPParticleColourEffector particleFader;
	ESPParticleColourEffector particleFireColourFader;
	ESPParticleColourEffector particleCloudColourFader;
	ESPParticleColourEffector particleFaderUberballTrail;
	ESPParticleColourEffector particleGravityArrowColour;

	ESPParticleScaleEffector  particlePulseUberballAura;
	ESPParticleScaleEffector  particlePulseItemDropAura;
	ESPParticleScaleEffector  particlePulsePaddleLaser;
	ESPParticleScaleEffector  beamEndPulse;
	ESPParticleScaleEffector  particleShrinkToNothing;

	ESPParticleAccelEffector ghostBallAccel1;
	ESPParticleAccelEffector gravity;

	ESPParticleScaleEffector particleSmallGrowth;
	ESPParticleScaleEffector particleMediumGrowth;
	ESPParticleScaleEffector particleLargeGrowth;
	ESPParticleScaleEffector particleMediumShrink;
	ESPParticleScaleEffector particleLargeVStretch;

	ESPParticleRotateEffector explosionRayRotatorCW;
	ESPParticleRotateEffector explosionRayRotatorCCW;
	ESPParticleRotateEffector smokeRotatorCW;
	ESPParticleRotateEffector smokeRotatorCCW;
	ESPParticleRotateEffector loopRotateEffectorCW;

	ESPParticleColourEffector beamBlastColourEffector;

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
	Texture2D* targetTex;
	Texture2D* haloTex;
	Texture2D* lensFlareTex;
	Texture2D* sparkleTex;
	Texture2D* spiralTex;
	Texture2D* sideBlastTex;
	Texture2D* hugeExplosionTex;

	// Ball and paddle related ESP effects
	std::map<const GameBall*, std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> > > ballEffects; // stores each balls set of item-related (defined by unique ID) effects
	std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> > paddleEffects;

	// Constants for the number of particles for particular effects
	static const int NUM_PADDLE_LASER_SPARKS = 15;
	static const int NUM_PADDLE_BEAM_ORIGIN_PARTICLES = 30;
	static const int NUM_GHOST_SMOKE_PARTICLES = 23;
	static const int NUM_UBER_BALL_TRAIL_PARTICLES = 35;
	static const int NUM_EXPLOSION_FIRE_CLOUD_PARTICLES  = 23;
	static const int NUM_LASER_VAPOUR_TRAIL_PARTICLES = 15;
	static const int NUM_PADDLE_SIZE_CHANGE_PARTICLES = 24;
	static const int NUM_BALL_SIZE_CHANGE_PARTICLES = 20;
	static const int NUM_ITEM_ACQUIRED_SPARKS = 20;
	static const int NUM_INK_CLOUD_PART_PARTICLES = 5;
	static const int NUM_INK_SPRAY_PARTICLES = 20;
	static const int NUM_EXPLOSION_SMOKE_PART_PARTICLES = 6;

	static const int NUM_SMOKE_TEXTURES = 6;

	// Cached Ball effects
	ESPPointEmitter* crazyBallAura;

	// Laser and beam effects
	ESPPointEmitter*  paddleLaserGlowAura;
	ESPPointEmitter*  paddleLaserGlowSparks;
	ESPVolumeEmitter* paddleBeamGlowSparks;
	ESPVolumeEmitter* paddleBeamOriginUp;
	ESPPointEmitter*  paddleBeamBlastBits;
	
	std::vector<ESPPointEmitter*> beamEndEmitters;
	std::vector<ESPPointEmitter*> beamBlockOnlyEndEmitters;
	std::vector<ESPPointEmitter*> beamEndFallingBitsEmitters;
	std::vector<ESPPointEmitter*> beamFlareEmitters;

	CgFxVolumetricEffect ghostBallSmoke;
	CgFxVolumetricEffect fireEffect;

	// Initialization functions for effect stuffs
	void InitESPTextures();
	void InitStandaloneESPEffects();

	void AddUberBallESPEffects(std::vector<ESPPointEmitter*>& effectsList);
	void AddGhostBallESPEffects(std::vector<ESPPointEmitter*>& effectsList);
	void AddGravityBallESPEffects(const GameBall* ball, std::vector<ESPPointEmitter*>& effectsList);
	void AddCrazyBallESPEffects(const GameBall* ball, std::vector<ESPPointEmitter*>& effectsList);
	void AddPaddleCamBallESPEffects(std::vector<ESPPointEmitter*>& effectsList);
	void AddBallCamPaddleESPEffects(std::vector<ESPPointEmitter*>& effectsList);

	void AddPaddleGrowEffect();
	void AddPaddleShrinkEffect();
	void AddBallGrowEffect(const GameBall* ball);
	void AddBallShrinkEffect(const GameBall* ball);
	void AddOneUpEffect(const PlayerPaddle* paddle);

	void InitLaserPaddleESPEffects();

	ESPPointEmitter* CreateSpinningTargetESPEffect();

	ESPPointEmitter* CreateTeleportEffect(const Point2D& center, const PortalBlock& block, bool isSibling);

	void AddCollateralProjectileEffects(const Projectile& projectile);
	void AddRocketProjectileEffects(const Projectile& projectile);

	void AddLaserPaddleESPEffects(const GameModel& gameModel, const Projectile& projectile);
	void AddLaserHitPrismBlockEffect(const Point2D& loc);
	void AddLaserHitWallEffect(const Point2D& loc);

	void AddRocketHitBlockEffect(float rocketSizeFactor, const Point2D& loc);

	void AddPaddleLaserBeamEffect(const Beam& beam);
	ESPPointEmitter* CreateBeamEndEffect();
	ESPPointEmitter* CreateBeamEndBlockEffect();
	ESPPointEmitter* CreateBeamFallingBitEffect();
	ESPPointEmitter* CreateBeamFlareEffect();

	ESPPointEmitter* CreateBlockBreakSmashyBits(const Point3D& center, const ESPInterval& r, const ESPInterval& g, const ESPInterval& b, 
		bool gravity = true, size_t numParticles = 10);

	void DrawProjectileEmitter(double dT, const Camera& camera, const Projectile& projectile, ESPPointEmitter* projectileEmitter);

public:
	GameESPAssets();
	~GameESPAssets();

	// Specific effects that can be made to occur in the game
	//void AddBallBounceEffect(const Camera& camera, const GameBall& ball);	

	ESPPointEmitter* CreateBallBounceEffect(const GameBall& ball, Onomatoplex::SoundType soundType); 
	void AddBounceLevelPieceEffect(const Camera& camera, const GameBall& ball, const LevelPiece& block);
	void AddBouncePaddleEffect(const Camera& camera, const GameBall& ball, const PlayerPaddle& paddle);
	void AddBounceBallBallEffect(const Camera& camera, const GameBall& ball1, const GameBall& ball2);
	void AddBlockHitByProjectileEffect(const Projectile& projectile, const LevelPiece& block);

	void AddPortalTeleportEffect(const Point2D& enterPt, const PortalBlock& block);
	void AddCannonFireEffect(const GameBall& ball, const CannonBlock& block);
	void AddBasicBlockBreakEffect(const Camera& camera, const LevelPiece& block);
	void AddBombBlockBreakEffect(const Camera& camera, const LevelPiece& bomb);
	void AddInkBlockBreakEffect(const Camera& camera, const LevelPiece& inkBlock, const GameLevel& level, bool shootSpray);
	void AddBallSafetyNetDestroyedEffect(const Point2D& pos);
	void AddBallExplodedEffect(const GameBall* ball);
	void AddPaddleHitWallEffect(const PlayerPaddle& paddle, const Point2D& hitLoc);
	void AddPaddleHitByProjectileEffect(const PlayerPaddle& paddle, const Projectile& projectile);
	void AddItemAcquiredEffect(const Camera& camera, const PlayerPaddle& paddle, const GameItem& item);
	void SetItemEffect(const GameItem& item, const GameModel& gameModel);
	void AddItemDropEffect(const Camera& camera, const GameItem& item, bool showStars);
	void RemoveItemDropEffect(const Camera& camera, const GameItem& item);

	void AddProjectileEffect(const GameModel& gameModel, const Projectile& projectile);
	void RemoveProjectileEffect(const Projectile& projectile);

	void AddBeamEffect(const Beam& beam);
	void UpdateBeamEffect(const Beam& beam);
	void RemoveBeamEffect(const Beam& beam);

	void AddTimerHUDEffect(GameItem::ItemType type, GameItem::ItemDisposition disposition);

	void TurnOffCurrentItemDropStars(const Camera& camera);

	void KillAllActiveEffects();
	void KillAllActiveBallEffects(const GameBall& ball);

	// Draw functions for various particle effects in the game
	void DrawParticleEffects(double dT, const Camera& camera, const Vector3D& worldTranslation);
	void DrawBeamEffects(double dT, const Camera& camera, const Vector3D& worldTranslation);
	void DrawProjectileEffects(double dT, const Camera& camera);


	void DrawItemDropEffects(double dT, const Camera& camera, const GameItem& item);

	void DrawUberBallEffects(double dT, const Camera& camera, const GameBall& ball);
	void DrawGhostBallEffects(double dT, const Camera& camera, const GameBall& ball);
	void DrawGravityBallEffects(double dT, const Camera& camera, const GameBall& ball, const Vector3D& gravityDir);
	void DrawCrazyBallEffects(double dT, const Camera& camera, const GameBall& ball);
	void DrawPaddleCamEffects(double dT, const Camera& camera, const GameBall& ball, const PlayerPaddle& paddle);
	void DrawBallCamEffects(double dT, const Camera& camera, const GameBall& ball, const PlayerPaddle& paddle);
	
	void DrawBackgroundBallEffects(double dT, const Camera& camera, const GameBall& ball);
	void DrawBackgroundPaddleEffects(double dT, const Camera& camera, const PlayerPaddle& paddle);

	void DrawPaddleLaserBulletEffects(double dT, const Camera& camera, const PlayerPaddle& paddle);
	void DrawPaddleLaserBeamBeforeFiringEffects(double dT, const Camera& camera, const PlayerPaddle& paddle);
	void DrawPaddleLaserBeamFiringEffects(double dT, const Camera& camera, const PlayerPaddle& paddle);

	void DrawTimerHUDEffect(double dT, const Camera& camera, GameItem::ItemType type);
};
#endif