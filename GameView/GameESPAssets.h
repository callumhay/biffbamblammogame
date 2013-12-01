/**
 * GameESPAssets.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __GAMESPASSETS_H__
#define __GAMESPASSETS_H__

#include <list>

#include "CgFXVolumetricEffect.h"
#include "CgFXPostRefract.h"

#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Camera.h"

#include "../ESPEngine/ESP.h"

#include "../GameModel/Onomatoplex.h"
#include "../GameModel/GameItem.h"

class LevelPiece;
class PortalBlock;
class RegenBlock;
class GameBall;
class GameItem;
class Texture;
class Projectile;
class PlayerPaddle;
class GameModel;
class GameLevel;
class Beam;
class CgFxFireBallEffect;
struct ESPInterval;

class PuffOfSmokeEffectInfo;
class ShockwaveEffectInfo;
class ShortCircuitEffectInfo;
class DebrisEffectInfo;

/**
 * Stores, draws and changes emitter/sprite/particle assets for the game.
 */
class GameESPAssets {

private:
    typedef std::map<const GameBall*, std::list<ESPPointEmitter*> > BallEffectsMap;
    typedef BallEffectsMap::iterator BallEffectsMapIter;

	// Currently active particle systems
	std::list<ESPEmitter*> activeGeneralEmitters;
	std::list<ESPEmitter*> activePaddleEmitters;

	std::map<const GameBall*, std::list<ESPEmitter*> > activeBallBGEmitters;
    BallEffectsMap boostBallEmitters;
	std::map<const GameItem*, std::list<ESPEmitter*> > activeItemDropEmitters;
	std::map<const Projectile*, std::list<ESPPointEmitter*> > activeProjectileEmitters;
	std::map<const Beam*, std::list<ESPEmitter*> > activeBeamEmitters;
	std::map<GameItem::ItemType, std::list<ESPEmitter*> > activeTimerHUDEmitters;
	//std::map<const LevelPiece*, std::list<ESPEmitter*> > activeLevelPieceEmitters;
	std::map<std::pair<const TeslaBlock*, const TeslaBlock*>, std::list<ESPPointToPointBeam*> > teslaLightningArcs;
	
	// Standard effectors for the various ESP effects
	ESPParticleColourEffector particleFader;
    ESPParticleColourEffector particleBoostFader;
	ESPParticleColourEffector particleFireColourFader;
    ESPParticleColourEffector particleWaterVapourColourFader;
    ESPParticleColourEffector particleSmokeColourFader;
    ESPParticleColourEffector particleFireFastColourFader;
    ESPParticleColourEffector particleSuperFireFastColourFader;
	ESPParticleColourEffector fireBallColourFader;
	ESPParticleColourEffector iceBallColourFader;
	ESPParticleColourEffector particleCloudColourFader;
	ESPParticleColourEffector particleFaderUberballTrail;
	ESPParticleColourEffector particleGravityArrowColour;
    ESPParticleColourEffector flashColourFader;
    ESPParticleColourEffector slowBallColourFader;
    ESPParticleColourEffector fastBallColourFader;
    ESPMultiColourEffector starColourFlasher;

	ESPParticleScaleEffector particlePulseUberballAura;
	ESPParticleScaleEffector particlePulseItemDropAura;
	ESPParticleScaleEffector particlePulsePaddleLaser;
	ESPParticleScaleEffector particlePulseFireGlobAura;
	ESPParticleScaleEffector particlePulseIceBallAura;
    ESPParticleScaleEffector particlePulseOrb;
	ESPParticleScaleEffector beamEndPulse;
	ESPParticleScaleEffector particleShrinkToNothing;
    ESPParticleScaleEffector particleLargeGrowthSuperFastPulser;
    ESPParticleScaleEffector flarePulse;

	ESPParticleAccelEffector ghostBallAccel1;
	ESPParticleAccelEffector gravity;
	ESPParticleAccelEffector fireBallAccel1;
	ESPParticleAccelEffector iceBallAccel;

	ESPParticleScaleEffector particleSmallGrowth;
	ESPParticleScaleEffector particleMediumGrowth;
	ESPParticleScaleEffector particleLargeGrowth;
	ESPParticleScaleEffector particleSuperGrowth;
	ESPParticleScaleEffector particleMediumShrink;
	ESPParticleScaleEffector particleLargeVStretch;
    ESPParticleScaleEffector particleMedVStretch;

	ESPParticleRotateEffector explosionRayRotatorCW;
	ESPParticleRotateEffector explosionRayRotatorCCW;
	ESPParticleRotateEffector smokeRotatorCW;
	ESPParticleRotateEffector smokeRotatorCCW;
    ESPParticleRotateEffector fastRotatorCW;
    ESPParticleRotateEffector fastRotatorCCW;
	ESPParticleRotateEffector loopRotateEffectorCW;
	ESPParticleRotateEffector loopRotateEffectorCCW;

	ESPParticleColourEffector beamBlastColourEffector;

	// Various textures used as particles for various effects
	std::vector<Texture2D*> bangTextures;
	std::vector<Texture2D*> splatTextures;
	std::vector<Texture2D*> smokeTextures;
	std::vector<Texture2D*> snowflakeTextures;
    std::vector<Texture2D*> boltTextures;
	std::vector<Texture2D*> rockTextures;
    std::vector<Texture2D*> cloudTextures;
	std::vector<CgFxFireBallEffect*> moltenRockEffects;
	
    Texture2D* cleanCircleGradientTex;
	Texture2D* circleGradientTex;
	Texture2D* starTex;
	Texture2D* starOutlineTex;
	Texture2D* evilStarTex;
	Texture2D* explosionTex;
	Texture2D* explosionRayTex;
	Texture2D* laserBeamTex;
	Texture2D* upArrowTex;
	Texture2D* ballTex;
	Texture2D* circleTargetTex;
	Texture2D* haloTex;
	Texture2D* lensFlareTex;
	Texture2D* sparkleTex;
	Texture2D* spiralTex;
	Texture2D* sideBlastTex;
	Texture2D* hugeExplosionTex;
    Texture2D* bubblyExplosionTex;
	Texture2D* lightningBoltTex;
	Texture2D* sphereNormalsTex;
    Texture2D* cloudNormalTex;
    Texture2D* vapourTrailTex;
    Texture2D* heartTex;
    Texture2D* chevronTex;
    Texture2D* infinityTex;
    Texture2D* dropletTex;
    Texture2D* flareTex;

    Texture2D* happyFaceTex;
    Texture2D* neutralFaceTex;
    Texture2D* sadFaceTex;

    Texture2D* plusTex;
    Texture2D* plusOutlineTex;
    Texture2D* circleTex;
    Texture2D* outlinedHoopTex;
    Texture2D* xTex;
    Texture2D* xOutlineTex;

    Texture2D* lightningAnimTex;

	// Ball and paddle related ESP effects
	std::map<const GameBall*, std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> > > ballEffects; // stores each balls set of item-related (defined by unique ID) effects
	std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> > paddleEffects;

	// Constants for the number of particles for particular effects
	static const int NUM_PADDLE_LASER_SPARKS = 15;
	static const int NUM_PADDLE_BEAM_ORIGIN_PARTICLES = 30;
	static const int NUM_GHOST_SMOKE_PARTICLES = 23;
	static const int NUM_FIRE_BALL_PARTICLES = 25;
	static const int NUM_UBER_BALL_TRAIL_PARTICLES = 35;
	static const int NUM_EXPLOSION_FIRE_CLOUD_PARTICLES  = 23;
	static const int NUM_LASER_VAPOUR_TRAIL_PARTICLES = 15;
	static const int NUM_PADDLE_SIZE_CHANGE_PARTICLES = 24;
	static const int NUM_BALL_SIZE_CHANGE_PARTICLES = 20;
	static const int NUM_ITEM_ACQUIRED_SPARKS = 20;
	static const int NUM_INK_CLOUD_PART_PARTICLES = 6;
	static const int NUM_INK_SPRAY_PARTICLES = 20;
	static const int NUM_EXPLOSION_SMOKE_PART_PARTICLES = 6;

	static const int NUM_SMOKE_TEXTURES = 6;

	// Cached Ball effects
	ESPPointEmitter* crazyBallAura;
    ESPPointEmitter* boostSparkleEmitterLight;
    ESPPointEmitter* boostSparkleEmitterDark;

	// Cached paddle effects
    ESPPointEmitter*  paddleLaserGlowAura;
	ESPPointEmitter*  paddleLaserGlowSparks;
	ESPVolumeEmitter* paddleBeamGlowSparks;
	ESPVolumeEmitter* paddleBeamOriginUp;
	ESPPointEmitter*  paddleBeamBlastBits;
	
    typedef std::map<const Beam*, std::vector<ESPPointEmitter*> > BeamSegEmitterMap;
    typedef BeamSegEmitterMap::iterator BeamSegEmitterMapIter;
    BeamSegEmitterMap beamOriginEmitters;
    BeamSegEmitterMap beamEndEmitters;
	BeamSegEmitterMap beamBlockOnlyEndEmitters;
	BeamSegEmitterMap beamEndFallingBitsEmitters;
	BeamSegEmitterMap beamFlareEmitters;

	CgFxVolumetricEffect ghostBallSmoke;
	CgFxVolumetricEffect fireEffect;
	CgFxVolumetricEffect fireBallTrailEffect;
	CgFxVolumetricEffect iceBallTrailEffect;

    CgFxPostRefract refractFireEffect;
	CgFxPostRefract normalTexRefractEffect;
    CgFxPostRefract vapourTrailRefractEffect;

	// Initialization functions for effect stuffs
	void InitESPTextures();
	void InitStandaloneESPEffects();

	void AddUberBallESPEffects(std::vector<ESPPointEmitter*>& effectsList);
	void AddGhostBallESPEffects(std::vector<ESPPointEmitter*>& effectsList);
	void AddFireBallESPEffects(const GameBall* ball, std::vector<ESPPointEmitter*>& effectsList);
	void AddIceBallESPEffects(const GameBall* ball, std::vector<ESPPointEmitter*>& effectsList);
	void AddGravityBallESPEffects(const GameBall* ball, std::vector<ESPPointEmitter*>& effectsList);
	void AddCrazyBallESPEffects(const GameBall* ball, std::vector<ESPPointEmitter*>& effectsList);
    void AddSlowBallESPEffects(const GameBall* ball, std::vector<ESPPointEmitter*>& effectsList);
    void AddFastBallESPEffects(const GameBall* ball, std::vector<ESPPointEmitter*>& effectsList);
	void AddPaddleCamBallESPEffects(std::vector<ESPPointEmitter*>& effectsList);
	void AddBallCamPaddleESPEffects(std::vector<ESPPointEmitter*>& effectsList);

	void AddPaddleGrowEffect();
	void AddPaddleShrinkEffect();
	void AddBallGrowEffect(const GameBall* ball);
	void AddBallShrinkEffect(const GameBall* ball);
	void AddLifeUpEffect(const PlayerPaddle* paddle);

	void InitLaserPaddleESPEffects();

	ESPPointEmitter* CreateSpinningTargetESPEffect();
	ESPPointEmitter* CreateTeleportEffect(const Point2D& center, const PortalBlock& block, bool isSibling);
	
	void AddCollateralProjectileEffects(const Projectile& projectile);
	void AddRocketProjectileEffects(const RocketProjectile& projectile);
	void AddFireGlobProjectileEffects(const Projectile& projectile);
    
    void AddPaddleMineFiredEffects(const GameModel& gameModel, const PaddleMineProjectile& projectile);
    void AddPaddleMineAttachedEffects(const Projectile& projectile);

	void AddBasicPaddleHitByProjectileEffect(const PlayerPaddle& paddle, const Projectile& projectile);

	void AddLaserPaddleESPEffects(const GameModel& gameModel, const Projectile& projectile);
    void AddLaserBallESPEffects(const GameModel& gameModel, const Projectile& projectile);
    void AddLaserTurretESPEffects(const GameModel& gameModel, const Projectile& projectile);
    void AddLaserBossESPEffects(const GameModel& gameModel, const Projectile& projectile);
    void AddLaserESPEffects(const GameModel& gameModel, const Projectile& projectile, const Colour& baseColour,
        const Colour& brightColour, bool hasTrail = true);

    void AddOrbESPEffects(const Projectile& projectile, const Colour& baseColour,
        const Colour& brightColour);
    
    void AddLightningBoltESPEffects(const GameModel& gameModel, const Projectile& projectile);

    void AddHitWallEffect(const Projectile& projectile, const Point2D& hitPos);
	void AddLaserHitPrismBlockEffect(const Point2D& loc);
	void AddLaserHitWallEffect(const Point2D& loc);
	void AddOrbHitWallEffect(const Projectile& projectile, const Point2D& loc, const Colour& baseColour, const Colour& brightColour);
    void AddLightningBoltHitWallEffect(float width, float height, const Point2D& loc);
    void AddEnergyShieldHitEffect(const Point2D& shieldCenter, const GameBall& ball);

    void AddBallHitTurretEffect(const GameBall& ball, const LevelPiece& block);

	void AddPaddleLaserBeamEffect(const Beam& beam);
    void AddBossLaserBeamEffect(const Beam& beam);
    void AddTypicalBeamSegmentEffects(const Beam& beam, std::list<ESPEmitter*>& beamEmitters);
    std::list<ESPPointEmitter*> CreateBeamOriginEffects(const Beam& beam);
	ESPPointEmitter* CreateBeamEndEffect(const Beam& beam);
	ESPPointEmitter* CreateBeamEndBlockEffect(const Beam& beam);
	ESPPointEmitter* CreateBeamFallingBitEffect(const Beam& beam);
	ESPPointEmitter* CreateBeamFlareEffect(const Beam& beam);

    ESPPointEmitter* CreateMultiplierComboEffect(int multiplier, const Point2D& position);
    void CreateStarAcquiredEffect(const Point2D& position, std::list<ESPEmitter*>& emitterListToAddTo);

	void DrawProjectileEmitter(double dT, const Camera& camera, const Projectile& projectile, ESPPointEmitter* projectileEmitter);

	std::map<const GameBall*, std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> > >::iterator EnsureBallEffectsList(const GameBall& ball);

    void ChooseDispositionTextures(const GameItem::ItemDisposition& itemDisposition, Texture2D*& itemSpecificFillShapeTex,
        Texture2D*& itemSpecificOutlineShapeTex, Texture2D*& itemSepecificFaceTex) const;

    void AddItemDropEmitters(const GameItem& item, Texture2D* itemSpecificFillShapeTex,
        Texture2D* itemSpecificOutlineShapeTex, Texture2D* itemSepecificFaceTex,
        const ESPInterval& redRandomColour, const ESPInterval& greenRandomColour, const ESPInterval& blueRandomColour,
        const ESPInterval& alpha, int numParticlesPerEmitter);

public:
	GameESPAssets();
	~GameESPAssets();

	void UpdateBGTexture(const Texture2D& bgTexture);

	ESPPointEmitter* CreateShockwaveEffect(const Point3D& center, float startSize, float lifeTime);
	ESPPointEmitter* CreateBlockBreakSmashyBits(const Point3D& center, const ESPInterval& r, const ESPInterval& g, const ESPInterval& b, 
		bool gravity = true, size_t numParticles = 10);

    void AddBossHurtEffect(const Point2D& pos, float width, float height);
    void AddBossAngryEffect(const Point2D& pos, float width, float height);

    void AddPuffOfSmokeEffect(const PuffOfSmokeEffectInfo& info);
    void AddShockwaveEffect(const ShockwaveEffectInfo& info);
    void AddDebrisEffect(const DebrisEffectInfo& info);

    void AddMultiplierComboEffect(int multiplier, const Point2D& position, const PlayerPaddle& paddle);
    //void AddPointAwardEffect(const PointAward& pointAward, const PlayerPaddle& paddle);
    void AddStarAcquiredEffect(const Point2D& pointAwardPos);

	// Specific effects that can be made to occur in the game
	//void AddBallBounceEffect(const Camera& camera, const GameBall& ball);	

    void AddBallBoostEffect(const BallBoostModel& boostModel);
    void AddBallAcquiredBoostEffect(const GameBall& ball, const Colour& colour);

    void AddRocketBlastEffect(float rocketSizeFactor, const Point2D& loc);
    void AddMineBlastEffect(const MineProjectile& mine, const Point2D& loc);

	ESPPointEmitter* CreateBallBounceEffect(const GameBall& ball, Onomatoplex::SoundType soundType); 
	void AddBounceLevelPieceEffect(const GameBall& ball, const LevelPiece& block);
    void AddBounceBossEffect(const GameBall& ball);
    void AddMiscBallPieceCollisionEffect(const GameBall& ball, const LevelPiece& block);
	void AddBouncePaddleEffect(const GameBall& ball, const PlayerPaddle& paddle);
	void AddBounceBallBallEffect(const GameBall& ball1, const GameBall& ball2);
	void AddBlockHitByProjectileEffect(const Projectile& projectile, const LevelPiece& block);
    void AddSafetyNetHitByProjectileEffect(const Projectile& projectile);
    void AddBossHitByProjectileEffect(const Projectile& projectile, const BossBodyPart& collisionPart); 
	void AddBallHitLightningArcEffect(const GameBall& ball);

	void AddPortalTeleportEffect(const Point2D& enterPt, const PortalBlock& block);
	void AddCannonFireEffect(const Point3D& endOfCannonPt, const Vector2D& fireDir);
	void AddBasicBlockBreakEffect(const LevelPiece& block);
	void AddBombBlockBreakEffect(const LevelPiece& bomb);
	void AddInkBlockBreakEffect(const Camera& camera, const LevelPiece& inkBlock, const GameLevel& level, bool shootSpray);
	void AddRegenBlockSpecialBreakEffect(const RegenBlock& regenBlock);

    void AddBlockDisintegrationEffect(const LevelPiece& block);

	void AddIceBitsBreakEffect(const LevelPiece& block);
	void AddIceCubeBlockBreakEffect(const LevelPiece& block, const Colour& colour);
    void AddIceMeltedByFireEffect(const LevelPiece& block);
    void AddFirePutOutByIceEffect(const LevelPiece& block);

    void AddFireballCancelledEffect(const GameBall* ball);
    void AddIceballCancelledEffect(const GameBall* ball);

    void AddFireGlobDestroyedEffect(const Projectile& projectile);

	void AddBallSafetyNetDestroyedEffect(const Point2D& pos);
	void AddBallExplodedEffect(const GameBall* ball);
	void AddPaddleHitWallEffect(const PlayerPaddle& paddle, const Point2D& hitLoc);
	void AddPaddleHitByProjectileEffect(const PlayerPaddle& paddle, const Projectile& projectile);
    void AddPaddleHitByBeamEffect(const PlayerPaddle& paddle, const BeamSegment& beamSegment);
    void AddPaddleHitByBossPartEffect(const PlayerPaddle& paddle, const BossBodyPart& bossPart);

	ESPPointEmitter* CreateItemNameEffect(const PlayerPaddle& paddle, const GameItem& item);
    void AddItemAcquiredEffect(const Camera& camera, const PlayerPaddle& paddle, const GameItem& item);
	void SetItemEffect(const GameItem& item, const GameModel& gameModel);
	void AddItemDropEffect(const GameItem& item, bool showParticles);
	void RemoveItemDropEffect(const GameItem& item);

	void AddProjectileEffect(const GameModel& gameModel, const Projectile& projectile);
	void RemoveProjectileEffect(const Projectile& projectile);
    void RemoveAllProjectileEffects();

	void AddBeamEffect(const Beam& beam);
	void UpdateBeamEffect(const Beam& beam);
	void RemoveBeamEffect(const Beam& beam, bool removeCachedEffects);

	void AddTeslaLightningBarrierEffect(const TeslaBlock& block1, const TeslaBlock& block2, const Vector3D& levelTranslation);
	void RemoveTeslaLightningBarrierEffect(const TeslaBlock& block1, const TeslaBlock& block2);
	void SetTeslaLightiningAlpha(float alpha);

	void AddTimerHUDEffect(GameItem::ItemType type, GameItem::ItemDisposition disposition);

    void AddShortCircuitEffect(const ShortCircuitEffectInfo& effectInfo);
    void AddStarSmashEffect(const Point3D& pos, const Vector3D& dir, const ESPInterval& size, 
        const ESPInterval& lifeInSecs, Onomatoplex::Extremeness extremeness);

	void TurnOffCurrentItemDropStars();

	void KillAllActiveEffects(bool killProjectiles);
	void KillAllActiveBallEffects(const GameBall& ball);
	void KillAllActiveTeslaLightningArcs();

	// Draw functions for various particle effects in the game
	void DrawParticleEffects(double dT, const Camera& camera);
	void DrawBeamEffects(double dT, const Camera& camera, const Vector3D& worldTranslation);
	void DrawProjectileEffects(double dT, const Camera& camera);

	void DrawItemDropEffects(double dT, const Camera& camera, const GameItem& item);

	void DrawUberBallEffects(double dT, const Camera& camera, const GameBall& ball);
	void DrawGhostBallEffects(double dT, const Camera& camera, const GameBall& ball);
	void DrawFireBallEffects(double dT, const Camera& camera, const GameBall& ball);
	void DrawIceBallEffects(double dT, const Camera& camera, const GameBall& ball);
	void DrawGravityBallEffects(double dT, const Camera& camera, const GameBall& ball, const Vector3D& gravityDir);
	void DrawCrazyBallEffects(double dT, const Camera& camera, const GameBall& ball);
    void DrawSlowBallEffects(double dT, const Camera& camera, const GameBall& ball);
    void DrawFastBallEffects(double dT, const Camera& camera, const GameBall& ball);
	void DrawPaddleCamEffects(double dT, const Camera& camera, const GameBall& ball, const PlayerPaddle& paddle);
	void DrawBallCamEffects(double dT, const Camera& camera, const GameBall& ball, const PlayerPaddle& paddle);
	
    void ResetBulletTimeBallBoostEffects();
    void ResetProjectileEffects(const Projectile& projectile);
    void DrawBulletTimeBallsBoostEffects(double dT, const Camera& camera, const GameModel& gameModel);
    void DrawBallBoostingEffects(double dT, const Camera& camera);

	void DrawBackgroundBallEffects(double dT, const Camera& camera, const GameBall& ball);
	void DrawBackgroundPaddleEffects(double dT, const Camera& camera, const PlayerPaddle& paddle);
    void TickButDontDrawBackgroundPaddleEffects(double dT);

	void DrawPaddleLaserBulletEffects(double dT, const Camera& camera, const PlayerPaddle& paddle);
	void DrawPaddleLaserBeamBeforeFiringEffects(double dT, const Camera& camera, const PlayerPaddle& paddle);
	void DrawPaddleLaserBeamFiringEffects(double dT, const Camera& camera, const PlayerPaddle& paddle);

	void DrawTeslaLightningArcs(double dT, const Camera& camera);
	void DrawTimerHUDEffect(double dT, const Camera& camera, GameItem::ItemType type);
};

inline void GameESPAssets::ResetBulletTimeBallBoostEffects() {
    this->boostSparkleEmitterLight->Reset();
    this->boostSparkleEmitterDark->Reset();
}

/**
 * Draw particle effects associated with the laser bullet paddle.
 * NOTE: You must transform these effects to be where the paddle is first!
 */
inline void GameESPAssets::DrawPaddleLaserBulletEffects(double dT, const Camera& camera, const PlayerPaddle& paddle) {
	float effectPos = paddle.GetHalfHeight() + this->paddleLaserGlowAura->GetParticleSizeY().maxValue * 0.5f;
	
    this->paddleLaserGlowAura->SetEmitPosition(Point3D(0, effectPos, 0));
	this->paddleLaserGlowSparks->SetEmitPosition(Point3D(0, effectPos, 0));

    this->paddleLaserGlowAura->SetAliveParticleAlphaMax(paddle.GetAlpha());
    this->paddleLaserGlowSparks->SetAliveParticleAlphaMax(paddle.GetAlpha());

	this->paddleLaserGlowAura->Draw(camera);
	this->paddleLaserGlowAura->Tick(dT);
	this->paddleLaserGlowSparks->Draw(camera);
	this->paddleLaserGlowSparks->Tick(dT);
}

/**
 * Draw particle effects associated with the laser beam paddle.
 * NOTE: You must transform these effects to be where the paddle is first!
 */
inline void GameESPAssets::DrawPaddleLaserBeamBeforeFiringEffects(double dT, const Camera& camera, const PlayerPaddle& paddle) {
	float tempXBound = 0.7f * paddle.GetHalfFlatTopWidth();
    float tempYBound = paddle.GetHalfHeight();
	float tempZBound = 0.9f * paddle.GetHalfDepthTotal();
	assert(tempXBound > 0);
	assert(tempZBound > 0);

	this->paddleBeamGlowSparks->SetEmitVolume(Point3D(-tempXBound, tempYBound, -tempZBound), Point3D(tempXBound, tempYBound, tempZBound));
	this->paddleBeamGlowSparks->SetParticleSize(ESPInterval(0.1f * paddle.GetHalfFlatTopWidth(), 0.2f * paddle.GetHalfFlatTopWidth()));
    this->paddleBeamGlowSparks->SetAliveParticleAlphaMax(paddle.GetAlpha());

	this->paddleBeamGlowSparks->Draw(camera);
	this->paddleBeamGlowSparks->Tick(dT);
}

inline void GameESPAssets::DrawPaddleLaserBeamFiringEffects(double dT, const Camera& camera, const PlayerPaddle& paddle) {
	ESPInterval xSize(paddle.GetHalfFlatTopWidth() * 0.3f, paddle.GetHalfFlatTopWidth() * 0.6f);

	this->paddleBeamBlastBits->SetEmitPosition(Point3D(0, 0, -paddle.GetHalfHeight()));
	this->paddleBeamBlastBits->SetParticleSize(xSize);
    this->paddleBeamBlastBits->SetAliveParticleAlphaMax(paddle.GetAlpha());

	this->paddleBeamBlastBits->Draw(camera);
	this->paddleBeamBlastBits->Tick(dT);
}

#endif