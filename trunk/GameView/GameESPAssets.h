/**
 * GameESPAssets.h
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

#ifndef __GAMESPASSETS_H__
#define __GAMESPASSETS_H__

#include "CgFXVolumetricEffect.h"
#include "CgFXPostRefract.h"
#include "CgFxFireBallEffect.h"

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
class BossLaserBeam;
class PaddleBlasterProjectile;
struct ESPInterval;
class GameSound;
class PortalProjectile;

class PuffOfSmokeEffectInfo;
class ShockwaveEffectInfo;
class ShortCircuitEffectInfo;
class DebrisEffectInfo;
class PortalSpawnEffectInfo;
class GenericEmitterEffectInfo;
class EnumGeneralEffectInfo;

/**
 * Stores, draws and changes emitter/sprite/particle assets for the game.
 */
class GameESPAssets {

private:
    typedef std::map<const GameBall*, std::list<ESPPointEmitter*> > BallEffectsMap;
    typedef BallEffectsMap::iterator BallEffectsMapIter;

    typedef std::list<ESPPointEmitter*> ProjectileEmitterCollection;
    typedef ProjectileEmitterCollection::iterator ProjectileEmitterCollectionIter;
    typedef std::map<const Projectile*, ProjectileEmitterCollection> ProjectileEmitterMap;
    typedef ProjectileEmitterMap::iterator ProjectileEmitterMapIter;

	// Currently active particle systems
	std::list<ESPAbstractEmitter*> activeGeneralEmitters;
	std::list<ESPEmitter*> activePaddleEmitters;

	std::map<const GameBall*, std::list<ESPEmitter*> > activeBallBGEmitters;
    BallEffectsMap boostBallEmitters;
	std::map<const GameItem*, std::list<ESPEmitter*> > activeItemDropEmitters;
	ProjectileEmitterMap activeProjectileEmitters;
    ProjectileEmitterMap activePostProjectileEmitters;
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
    ESPParticleColourEffector flameBlastSmokeColourFader;
    ESPParticleColourEffector particleFireFastColourFader;
    ESPParticleColourEffector particleSuperFireFastColourFader;
	ESPParticleColourEffector fireBallColourFader;
	ESPParticleColourEffector iceBallColourFader;
	ESPParticleColourEffector particleCloudColourFader;
	ESPParticleColourEffector particleFaderUberballTrail;
	ESPParticleColourEffector particleGravityArrowColour;
    ESPParticleColourEffector slowBallColourFader;
    ESPParticleColourEffector fastBallColourFader;
    ESPParticleColourEffector iceOriginColourEffector;
    ESPMultiColourEffector starColourFlasher;
    ESPMultiColourEffector fireOriginColourEffector;

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
    ESPParticleScaleEffector haloExpandPulse;

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
    ESPParticleRotateEffector moderateSpdLoopRotateEffectorCW;
    ESPParticleRotateEffector moderateSpdLoopRotateEffectorCCW;

	ESPParticleColourEffector beamBlastColourEffector;

	// Various textures used as particles for various effects
	std::vector<Texture2D*> bangTextures;
	std::vector<Texture2D*> splatTextures;
	std::vector<Texture2D*> smokeTextures;
	std::vector<Texture2D*> snowflakeTextures;
    std::vector<Texture2D*> boltTextures;
	std::vector<Texture2D*> rockTextures;
    std::vector<Texture2D*> cloudTextures;
    std::vector<Texture2D*> fireGlobTextures;
	std::vector<CgFxFireBallEffect*> moltenRockEffects;

    Mesh* fragileCannonBarrelMesh;
    Mesh* fragileCannonBasePostMesh;
    Mesh* fragileCannonBaseBarMesh;
    
    Mesh* portalMesh;

	// Ball and paddle related ESP effects
	std::map<const GameBall*, std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> > > ballEffects; // Stores each balls set of item-related (defined by unique ID) effects
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

	// Cached beam and laser paddle effects -----------------------------------
    ESPPointEmitter*  paddleLaserGlowAura;
	ESPPointEmitter*  paddleLaserGlowSparks;
	ESPVolumeEmitter* paddleBeamOriginUp;
	ESPPointEmitter*  paddleBeamBlastBits;
    ESPPointEmitter*  paddleCamBeamGlowInside;
    ESPPointEmitter*  paddleCamBeamGlowOutside;

    ESPVolumeEmitter* paddleBeamGlowSparks;
    ESPVolumeEmitter* stickyPaddleBeamGlowSparks0;
    ESPVolumeEmitter* stickyPaddleBeamGlowSparks1;
    ESPVolumeEmitter* stickyPaddleBeamGlowSparks2;

    typedef std::map<const Beam*, std::vector<ESPPointEmitter*> > BeamSegEmitterMap;
    typedef BeamSegEmitterMap::iterator BeamSegEmitterMapIter;
    BeamSegEmitterMap beamOriginEmitters;
    BeamSegEmitterMap beamEndEmitters;
	BeamSegEmitterMap beamBlockOnlyEndEmitters;
	BeamSegEmitterMap beamEndFallingBitsEmitters;
	BeamSegEmitterMap beamFlareEmitters;
    // -------------------------------------------------------------------------

    // Cached flame blaster and ice blaster effects ----------------------------
    ESPPointEmitter* paddleFlameBlasterOrigin;
    ESPPointEmitter* paddleIceBlasterOrigin;

    ProjectileEmitterMap  activeBlasterProjectileEffects;
    // -------------------------------------------------------------------------

	CgFxVolumetricEffect ghostBallSmoke;
	CgFxVolumetricEffect fireEffect;
	CgFxVolumetricEffect fireBallTrailEffect;
	CgFxVolumetricEffect iceBallTrailEffect;
    CgFxVolumetricEffect flameBlasterOriginFireEffect;
    CgFxVolumetricEffect flameBlasterFireEffect;
    CgFxFireBallEffect   flameBlasterParticleEffect;
    CgFxVolumetricEffect iceBlasterParticleEffect;
    CgFxVolumetricEffect iceBlasterCloudEffect;

    CgFxPostRefract refractFireEffect;
	CgFxPostRefract normalTexRefractEffect;
    CgFxPostRefract vapourTrailRefractEffect;

	// Initialization functions for effect stuffs
	void InitESPTextures();
	void InitStandaloneESPEffects();
    void InitMeshes();

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

	void AddPaddleGrowEffect(const PlayerPaddle* paddle);
	void AddPaddleShrinkEffect(const PlayerPaddle* paddle);
	void AddBallGrowEffect(const GameBall* ball);
	void AddBallShrinkEffect(const GameBall* ball);
	void AddLifeUpEffect(const PlayerPaddle* paddle);

	void InitLaserPaddleESPEffects();
    void InitPaddleBlasterESPEffects();

	ESPPointEmitter* CreateSpinningTargetESPEffect();
	
    void AddSingleEnergyProjectilePortalTeleportEffect(const Point2D& center, float sizeX, float sizeY, bool isSibling);
    void AddSingleObjectPortalTeleportEffect(const Point2D& center, float sizeX, float sizeY, const Colour& portalColour, bool isSibling);
	
	void AddCollateralProjectileEffects(const Projectile& projectile);
	void AddRocketProjectileEffects(const GameModel& gameModel, const RocketProjectile& projectile);
	void AddFireGlobProjectileEffects(const Projectile& projectile);
    void AddFlameBlastProjectileEffects(const GameModel& gameModel, const PaddleFlameBlasterProjectile& projectile);
    void AddIceBlastProjectileEffects(const GameModel& gameModel, const PaddleIceBlasterProjectile& projectile);
    void AddPaddleMineFiredEffects(const GameModel& gameModel, const PaddleMineProjectile& projectile);
    void AddPaddleMineAttachedEffects(const Projectile& projectile);
    void AddPortalProjectileEffects(const PortalProjectile& projectile);

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

    void AddHitWallEffect(const Projectile& projectile, const Point2D& hitPos, GameSound* sound);
	void AddLaserHitPrismBlockEffect(const Point2D& loc);
	void AddLaserHitWallEffect(const Point2D& loc);
    void AddFlameBlastHitWallEffect(float size, const Point2D& loc, GameSound* sound);
    void AddIceBlastHitWallEffect(float size, const Point2D& loc, GameSound* sound);
	void AddOrbHitWallEffect(const Projectile& projectile, const Point2D& loc, const Colour& baseColour, const Colour& brightColour);
    void AddLightningBoltHitWallEffect(float width, float height, const Point2D& loc);
    void AddEnergyShieldHitEffect(const Point2D& shieldCenter, const GameBall& ball);

    void AddBallHitTurretEffect(const GameBall& ball, const LevelPiece& block);

	void AddPaddleLaserBeamEffect(const Beam& beam);
    void AddBossLaserBeamEffect(const BossLaserBeam& beam);
    void AddTypicalBeamSegmentEffects(const Beam& beam, std::list<ESPEmitter*>& beamEmitters);
    std::list<ESPPointEmitter*> CreateBeamOriginEffects(const Beam& beam);
	ESPPointEmitter* CreateBeamEndEffect(const Beam& beam);
	ESPPointEmitter* CreateBeamEndBlockEffect(const Beam& beam);
	ESPPointEmitter* CreateBeamFallingBitEffect(const Beam& beam);
	ESPPointEmitter* CreateBeamFlareEffect(const Beam& beam);

    ESPPointEmitter* CreateMultiplierComboEffect(int multiplier, const Point2D& position);
    void CreateStarAcquiredEffect(const Point2D& position, std::list<ESPAbstractEmitter*>& emitterListToAddTo);

	void DrawProjectileEmitter(double dT, const Camera& camera, const Projectile& projectile, ESPPointEmitter* projectileEmitter);

	std::map<const GameBall*, std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> > >::iterator EnsureBallEffectsList(const GameBall& ball);

    void ChooseDispositionTextures(const GameItem::ItemDisposition& itemDisposition, Texture2D*& itemSpecificFillShapeTex,
        Texture2D*& itemSpecificOutlineShapeTex, Texture2D*& itemSepecificFaceTex) const;

    void AddItemDropFaceEmitters(const GameItem& item);
    void AddItemDropFaceEmitters(const GameItem& item, Texture2D* itemSpecificFillShapeTex,
        Texture2D* itemSpecificOutlineShapeTex, Texture2D* itemSepecificFaceTex,
        const ESPInterval& redRandomColour, const ESPInterval& greenRandomColour, const ESPInterval& blueRandomColour,
        const ESPInterval& alpha, int numParticlesPerEmitter);

    void RemoveAllProjectileEffectsFromMap(ProjectileEmitterMap& projectileMap);
    void RemoveProjectileEffectFromMap(const Projectile& projectile, ProjectileEmitterMap& projectileMap);

public:
	GameESPAssets();
	~GameESPAssets();

    void Update(const GameModel& gameModel);

    ESPPointEmitter* CreateShockwaveEffect(const Point3D& center, float startSize, 
        float lifeTime, bool reverse = false);

    // Effector Factory Methods -------------------------------------------------------------
    static void CreateFastFadeInFadeOutEffector(ESPMultiAlphaEffector& effector);

    // Effect Factory Methods ---------------------------------------------------------------
    static ESPPointEmitter* CreateBlockBreakSmashyBits(const Point3D& center, const ESPInterval& r, 
        const ESPInterval& g, const ESPInterval& b, bool gravity = true, size_t numParticles = 10);
    static ESPPointEmitter* CreateSingleEmphasisLineEffect(const Point3D& center, double totalEffectTime, 
        int numLines, float lengthMin, float lengthMax, bool reverse = true);
    static ESPPointEmitter* CreateContinuousEmphasisLineEffect(const Point3D& center, double totalEffectTime, 
        float lengthMin, float lengthMax, bool reverse = true);
    // --------------------------------------------------------------------------------------

    // Texture Collection Methods -----------------------------------------------------------
    static void GetRockTextures(std::vector<Texture2D*>& rockTextures);
    static void GetSnowflakeTextures(std::vector<Texture2D*>& snowflakeTextures);
    static void GetCloudTextures(std::vector<Texture2D*>& cloudTextures);
    // --------------------------------------------------------------------------------------

	void UpdateBGTexture(const Texture2D& bgTexture);

    void AddBossHurtEffect(const Point2D& pos, float width, float height);
    void AddBossAngryEffect(const Point2D& pos, float width, float height);

    void AddPuffOfSmokeEffect(const PuffOfSmokeEffectInfo& info);
    void AddShockwaveEffect(const ShockwaveEffectInfo& info);
    void AddDebrisEffect(const DebrisEffectInfo& info);
    void AddDebrisEffect(float minSize, float maxSize, float minLife, float maxLife, 
        const Point3D& pos, const Colour& colour, float force, const Vector3D& dir, int numParticles);

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
	void AddBouncePaddleEffect(const GameBall& ball, const PlayerPaddle& paddle, bool hitPaddleUnderside);
	void AddBounceBallBallEffect(const GameBall& ball1, const GameBall& ball2);
	void AddBlockHitByProjectileEffect(const Projectile& projectile, const LevelPiece& block, GameSound* sound);
    void AddSafetyNetHitByProjectileEffect(const Projectile& projectile, GameSound* sound);
    void AddBossHitByProjectileEffect(const Projectile& projectile, const BossBodyPart& collisionPart, GameSound* sound); 
	void AddBallHitLightningArcEffect(const GameBall& ball);

    void AddEnergyProjectilePortalTeleportEffect(const Point2D& enterPt, const Point2D& enterPortalCenter, 
        const Point2D& exitPortalCenter, float sizeX, float sizeY);
	void AddObjectPortalTeleportEffect(const Point2D& enterPt, float sizeX, float sizeY, 
        const Point2D& enterCenter, const Point2D& exitCenter, const Colour& portalColour);
    void ItemEnteredPortalBlockEffects(const GameItem& item, const PortalBlock& block);
	void AddCannonFireEffect(const Point3D& endOfCannonPt, const Vector2D& fireDir);
	void AddBasicBlockBreakEffect(const LevelPiece& block);
	void AddBombBlockBreakEffect(const LevelPiece& bomb);
	void AddInkBlockBreakEffect(const Camera& camera, const LevelPiece& inkBlock, const GameLevel& level, bool shootSpray);
	void AddRegenBlockSpecialBreakEffect(const GameModel& gameModel, const RegenBlock& regenBlock);
    void AddFragileCannonBreakEffect(const LevelPiece& block);

    void AddBlockDisintegrationEffect(const LevelPiece& block);

	void AddIceBitsBreakEffect(const Point2D& pos, float baseSize);
	void AddIceCubeBreakEffect(const Point2D& pos, float baseSize, const Colour& colour, float intensityMultiplier = 1.0f);
    void AddIceMeltedByFireEffect(const Point3D& pos, float width, float height);
    void AddFirePutOutByIceEffect(const Point3D& pos, float width, float height);

    void AddFireballCanceledEffect(const GameBall& ball);
    void AddIceballCanceledEffect(const GameBall& ball);
    void AddFireBlasterCanceledEffect(const PlayerPaddle& paddle);
    void AddIceBlasterCanceledEffect(const PlayerPaddle& paddle);

    void AddFireGlobDestroyedEffect(const Projectile& projectile);

	void AddBallSafetyNetDestroyedEffect(const Point2D& pos);
	void AddBallExplodedEffect(const GameBall* ball);
	void AddPaddleHitWallEffect(const PlayerPaddle& paddle, const Point2D& hitLoc);
	void AddPaddleHitByProjectileEffect(const PlayerPaddle& paddle, const Projectile& projectile, GameSound* sound);
    void AddPaddleHitByBeamEffect(const PlayerPaddle& paddle, const BeamSegment& beamSegment);
    void AddPaddleHitByBossPartEffect(const PlayerPaddle& paddle, const BossBodyPart& bossPart);
    void AddPaddleFrozeEffect(const PlayerPaddle& paddle);
    void AddPaddleUnfrozeEffect(const PlayerPaddle& paddle);
    //void AddPaddleOnFireEffect(const PlayerPaddle& paddle);

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
    void AddPortalSpawnEffect(const PortalSpawnEffectInfo& effectInfo);
    void AddGenericEmitterEffect(const GenericEmitterEffectInfo& effectInfo);
    void AddGeneralEnumEffect(const EnumGeneralEffectInfo& enumEffect);

	void TurnOffCurrentItemDropStars();

	void KillAllActiveEffects(bool killProjectiles);
	void KillAllActiveBallEffects(const GameBall& ball);
	void KillAllActiveTeslaLightningArcs();

    void PaddleFlipped();

	// Draw functions for various particle effects in the game
	void DrawParticleEffects(double dT, const Camera& camera);
	void DrawBeamEffects(double dT, const GameModel& gameModel, const Camera& camera);
	void DrawProjectileEffects(double dT, const Camera& camera);
    void DrawPostProjectileEffects(double dT, const Camera& camera);
    
    void DrawPaddleFlameBlasterEffects(double dT, const Camera& camera, const PlayerPaddle& paddle);
    void DrawPaddleIceBlasterEffects(double dT, const Camera& camera, const PlayerPaddle& paddle);
    void DrawPaddleBlasterProjectiles(double dT, const Camera& camera);

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
	
    float effectPos = (paddle.GetIsPaddleFlipped() ? -1.0f : 1.0f) * 
        (paddle.GetHalfHeight() + this->paddleLaserGlowAura->GetParticleSizeY().maxValue * 0.5f);
	
    this->paddleLaserGlowAura->SetAliveParticlePosition(0, effectPos, 0);
	this->paddleLaserGlowSparks->SetEmitPosition(Point3D(0, effectPos, 0));

    this->paddleLaserGlowAura->SetAliveParticleAlphaMax(paddle.GetAlpha());
    this->paddleLaserGlowSparks->SetAliveParticleAlphaMax(paddle.GetAlpha());

    this->paddleLaserGlowAura->Tick(dT);
	this->paddleLaserGlowAura->Draw(camera);
	this->paddleLaserGlowSparks->Tick(dT);
    this->paddleLaserGlowSparks->Draw(camera);
}

inline void GameESPAssets::DrawPaddleLaserBeamFiringEffects(double dT, const Camera& camera, const PlayerPaddle& paddle) {
	ESPInterval xSize(paddle.GetHalfFlatTopWidth() * 0.3f, paddle.GetHalfFlatTopWidth() * 0.6f);

    this->paddleBeamBlastBits->SetEmitDirection(paddle.GetUpVector());
	this->paddleBeamBlastBits->SetEmitPosition(Point3D(0, 0, -paddle.GetHalfHeight()));
	this->paddleBeamBlastBits->SetParticleSize(xSize);
    this->paddleBeamBlastBits->SetAliveParticleAlphaMax(paddle.GetAlpha());

    this->paddleBeamBlastBits->Tick(dT);
	this->paddleBeamBlastBits->Draw(camera);
}

#endif