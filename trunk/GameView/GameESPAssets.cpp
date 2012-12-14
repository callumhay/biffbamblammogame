/**
 * GameESPAssets.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameESPAssets.h"
#include "GameFontAssetsManager.h"
#include "GameViewConstants.h"
#include "BallSafetyNetMesh.h"
#include "CgFXFireBallEffect.h"
#include "PointsHUD.h"

#include "../GameModel/GameModel.h"
#include "../GameModel/GameLevel.h"
#include "../GameModel/GameBall.h"
#include "../GameModel/LevelPiece.h"
#include "../GameModel/Projectile.h"
#include "../GameModel/PlayerPaddle.h"
#include "../GameModel/Beam.h"
#include "../GameModel/BallBoostModel.h"
#include "../GameModel/PortalBlock.h"
#include "../GameModel/CannonBlock.h"
#include "../GameModel/TeslaBlock.h"
#include "../GameModel/PaddleRocketProjectile.h"
#include "../GameModel/FireGlobProjectile.h"
#include "../GameModel/PaddleMineProjectile.h"

#include "../BlammoEngine/Texture.h"
#include "../BlammoEngine/Plane.h"

#include "../ResourceManager.h"

GameESPAssets::GameESPAssets() : 
particleFader(1, 0),
particleBoostFader(0.8f, 0.0f),
particleFireColourFader(ColourRGBA(1.0f, 1.0f, 0.1f, 1.0f), ColourRGBA(0.5f, 0.0f, 0.0f, 0.0f)),
fireBallColourFader(ColourRGBA(1.0f, 1.0f, 0.0f, 1.0f), ColourRGBA(0.4f, 0.15f, 0.0f, 0.2f)),
iceBallColourFader(ColourRGBA(GameModelConstants::GetInstance()->ICE_BALL_COLOUR, 1.0f), ColourRGBA(1.0f, 1.0f, 1.0f, 0.1f)),
particleCloudColourFader(ColourRGBA(1.0f, 1.0f, 1.0f, 1.0f), ColourRGBA(0.7f, 0.7f, 0.7f, 0.0f)),
particleFaderUberballTrail(Colour(1,0,0), 0.6f, 0),
particleGravityArrowColour(ColourRGBA(GameModelConstants::GetInstance()->GRAVITY_BALL_COLOUR, 1.0f), ColourRGBA(0.58, 0.0, 0.83, 0.1)),
flashColourFader(ColourRGBA(1,1,1,1), ColourRGBA(GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR, 0.0f)),

particleShrinkToNothing(1, 0),
particlePulseUberballAura(0, 0),
particlePulseItemDropAura(0, 0),
particlePulsePaddleLaser(0, 0),
particlePulseFireGlobAura(0, 0),
particlePulseIceBallAura(0, 0),
beamEndPulse(0, 0),
particleSmallGrowth(1.0f, 1.3f), 
particleMediumGrowth(1.0f, 1.6f),
particleLargeGrowth(1.0f, 2.2f),
particleSuperGrowth(1.0f, 5.0f),
particleMediumShrink(1.0f, 0.25f),
particleLargeVStretch(Vector2D(1.0f, 1.0f), Vector2D(1.0f, 4.0f)),
beamBlastColourEffector(ColourRGBA(0.75f, 1.0f, 1.0f, 1.0f), ColourRGBA(GameViewConstants::GetInstance()->LASER_BEAM_COLOUR, 0.8f)),

ghostBallAccel1(Vector3D(1,1,1)),
fireBallAccel1(Vector3D(1,1,1)),
iceBallAccel(Vector3D(1,1,1)),
gravity(Vector3D(0, -9.8, 0)),

crazyBallAura(NULL),
boostSparkleEmitterLight(NULL),
boostSparkleEmitterDark(NULL),

paddleLaserGlowAura(NULL),
paddleLaserGlowSparks(NULL),
paddleBeamGlowSparks(NULL),
paddleBeamOriginUp(NULL),
paddleBeamBlastBits(NULL),

explosionRayRotatorCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.5f, ESPParticleRotateEffector::CLOCKWISE),
explosionRayRotatorCCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.5f, ESPParticleRotateEffector::COUNTER_CLOCKWISE),
smokeRotatorCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.25f, ESPParticleRotateEffector::CLOCKWISE),
smokeRotatorCCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.25f, ESPParticleRotateEffector::COUNTER_CLOCKWISE),
loopRotateEffectorCW(90.0f, ESPParticleRotateEffector::CLOCKWISE),
loopRotateEffectorCCW(90.0f, ESPParticleRotateEffector::COUNTER_CLOCKWISE),

circleGradientTex(NULL), 
starTex(NULL), 
starOutlineTex(NULL),
evilStarTex(NULL),
evilStarOutlineTex(NULL),
explosionTex(NULL),
explosionRayTex(NULL),
laserBeamTex(NULL),
upArrowTex(NULL),
ballTex(NULL),
targetTex(NULL),
haloTex(NULL),
lensFlareTex(NULL),
sparkleTex(NULL),
spiralTex(NULL),
sideBlastTex(NULL),
hugeExplosionTex(NULL),
bubblyExplosionTex(NULL),
lightningBoltTex(NULL),
sphereNormalsTex(NULL),
//rectPrismTexture(NULL),
cloudTex(NULL),
vapourTrailTex(NULL),
heartTex(NULL) {

	this->InitESPTextures();
	this->InitStandaloneESPEffects();
}

GameESPAssets::~GameESPAssets() {
	this->KillAllActiveEffects(true);
	this->KillAllActiveTeslaLightningArcs();

    bool removed = false;
    UNUSED_VARIABLE(removed);

	// Delete any effect textures
	for (std::vector<Texture2D*>::iterator iter = this->bangTextures.begin();
		iter != this->bangTextures.end(); ++iter) {
		
		removed = ResourceManager::GetInstance()->ReleaseTextureResource(*iter);
		assert(removed);	
	}
	this->bangTextures.clear();

	for (std::vector<Texture2D*>::iterator iter = this->splatTextures.begin();
		iter != this->splatTextures.end(); ++iter) {
		
		removed = ResourceManager::GetInstance()->ReleaseTextureResource(*iter);
		assert(removed);	
	}
	this->splatTextures.clear();

	for (std::vector<Texture2D*>::iterator iter = this->smokeTextures.begin();
		iter != this->smokeTextures.end(); ++iter) {
		
		removed = ResourceManager::GetInstance()->ReleaseTextureResource(*iter);
		assert(removed);	
	}
	this->smokeTextures.clear();

	for (std::vector<Texture2D*>::iterator iter = this->snowflakeTextures.begin();
		iter != this->snowflakeTextures.end(); ++iter) {
		
		removed = ResourceManager::GetInstance()->ReleaseTextureResource(*iter);
		assert(removed);	
	}
	this->snowflakeTextures.clear();
	
	//for (std::vector<Texture2D*>::iterator iter = this->fireGlobTextures.begin();
	//	iter != this->fireGlobTextures.end(); ++iter) {
	//	bool removed = ResourceManager::GetInstance()->ReleaseTextureResource(*iter);
	//	assert(removed);	
	//}
	//this->fireGlobTextures.clear();

	for (std::vector<Texture2D*>::iterator iter = this->rockTextures.begin(); iter != this->rockTextures.end(); ++iter) {
		removed = ResourceManager::GetInstance()->ReleaseTextureResource(*iter);
		assert(removed);
	}
	this->rockTextures.clear();

	for (std::vector<CgFxFireBallEffect*>::iterator iter = this->moltenRockEffects.begin(); iter != this->moltenRockEffects.end(); ++iter) {
		CgFxFireBallEffect* effect = *iter;
		delete effect;
		effect = NULL;
	}
	this->moltenRockEffects.clear();

	removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->circleGradientTex);
	assert(removed);
	removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->starTex);
	assert(removed);
	removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->starOutlineTex);
	assert(removed);
	removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->evilStarTex);
	assert(removed);
	removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->evilStarOutlineTex);
	assert(removed);
	removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->explosionTex);
	assert(removed);
	removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->explosionRayTex);
	assert(removed);
	removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->laserBeamTex);
	assert(removed);
	removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->upArrowTex);
	assert(removed);
	removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->ballTex);
	assert(removed);
	removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->targetTex);
	assert(removed);
	removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->haloTex);
	assert(removed);
	removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->lensFlareTex);
	assert(removed);
	removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->sparkleTex);
	assert(removed);
	removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->spiralTex);
	assert(removed);
	removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->sideBlastTex);
	assert(removed);
	removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->hugeExplosionTex);
	assert(removed);
    removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->bubblyExplosionTex);
    assert(removed);
	removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->lightningBoltTex);
	assert(removed);
	removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->sphereNormalsTex);
	assert(removed);
	//removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->rectPrismTexture);
	//assert(removed);
	removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->cloudTex);
	assert(removed);
    removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->vapourTrailTex);
    assert(removed);
    removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->heartTex);
    assert(removed);

	// Delete any standalone effects
	delete this->crazyBallAura;
	this->crazyBallAura = NULL;
    delete this->boostSparkleEmitterLight;
    this->boostSparkleEmitterLight = NULL;
    delete this->boostSparkleEmitterDark;
    this->boostSparkleEmitterDark = NULL;

	delete this->paddleLaserGlowAura;
	this->paddleLaserGlowAura = NULL;
	delete this->paddleLaserGlowSparks;
	this->paddleLaserGlowSparks = NULL;
	delete this->paddleBeamGlowSparks;
	this->paddleBeamGlowSparks = NULL;
	delete this->paddleBeamOriginUp;
	this->paddleBeamOriginUp = NULL;
	delete this->paddleBeamBlastBits;
	this->paddleBeamBlastBits = NULL;
}

/**
 * Kills all active effects currently being displayed in-game.
 */
void GameESPAssets::KillAllActiveEffects(bool killProjectiles) {
	// Delete any leftover emitters
	for (std::list<ESPEmitter*>::iterator iter = this->activeGeneralEmitters.begin();
		iter != this->activeGeneralEmitters.end(); ++iter) {
			delete *iter;
			*iter = NULL;
	}
	this->activeGeneralEmitters.clear();
	
	// Clear paddle BG emitters
	for (std::list<ESPEmitter*>::iterator iter = this->activePaddleEmitters.begin(); 
		iter != this->activePaddleEmitters.end(); ++iter) {
		delete *iter;
		*iter = NULL;
	}
	this->activePaddleEmitters.clear();

	// Clear ball BG emiiters
	for (std::map<const GameBall*, std::list<ESPEmitter*> >::iterator iter = this->activeBallBGEmitters.begin();
		iter != this->activeBallBGEmitters.end(); ++iter) {

		std::list<ESPEmitter*>& currEmitterList = iter->second;
		for (std::list<ESPEmitter*>::iterator iter2 = currEmitterList.begin(); iter2 != currEmitterList.end(); ++iter2) {
		    delete *iter2;
		    *iter2 = NULL;
		}
		currEmitterList.clear();
	}
	this->activeBallBGEmitters.clear();

	// Clear item drop emitters
	for (std::map<const GameItem*, std::list<ESPEmitter*> >::iterator iter = this->activeItemDropEmitters.begin();	iter != this->activeItemDropEmitters.end(); ++iter) {
			for (std::list<ESPEmitter*>::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2) {
				ESPEmitter* currEmitter = *iter2;
				delete currEmitter;
				currEmitter = NULL;
			}
			iter->second.clear();
	}
	this->activeItemDropEmitters.clear();

    if (killProjectiles) {
	    // Clear projectile emitters
	    for (std::map<const Projectile*, std::list<ESPPointEmitter*> >::iterator iter1 = this->activeProjectileEmitters.begin();
		    iter1 != this->activeProjectileEmitters.end(); ++iter1) {
    	
			    std::list<ESPPointEmitter*>& projEmitters = iter1->second;
    			
			    for (std::list<ESPPointEmitter*>::iterator iter2 = projEmitters.begin(); iter2 != projEmitters.end(); ++iter2) {
				    delete *iter2;
				    *iter2 = NULL;
			    }
			    projEmitters.clear();
	    }
	    this->activeProjectileEmitters.clear();
    }

	// Clear beam emitters
	for (std::vector<ESPPointEmitter*>::iterator iter = this->beamEndEmitters.begin(); iter != this->beamEndEmitters.end(); ++iter) {
		ESPPointEmitter* currEmitter = *iter;
		delete currEmitter;
		currEmitter = NULL;
	}
	this->beamEndEmitters.clear();
	for (std::vector<ESPPointEmitter*>::iterator iter = this->beamBlockOnlyEndEmitters.begin(); iter != this->beamBlockOnlyEndEmitters.end(); ++iter) {
		ESPPointEmitter* currEmitter = *iter;
		delete currEmitter;
		currEmitter = NULL;
	}
	this->beamBlockOnlyEndEmitters.clear();
	for (std::vector<ESPPointEmitter*>::iterator iter = this->beamEndFallingBitsEmitters.begin(); iter != this->beamEndFallingBitsEmitters.end(); ++iter) {
		ESPPointEmitter* currEmitter = *iter;
		delete currEmitter;
		currEmitter = NULL;
	}	
	this->beamEndFallingBitsEmitters.clear();
	for (std::vector<ESPPointEmitter*>::iterator iter = this->beamFlareEmitters.begin(); iter != this->beamFlareEmitters.end(); ++iter) {
		ESPPointEmitter* currEmitter = *iter;
		delete currEmitter;
		currEmitter = NULL;
	}
	this->beamFlareEmitters.clear();

	// Clear all ball emitters
	for (std::map<const GameBall*, std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> > >::iterator iter1 = this->ballEffects.begin(); iter1 != this->ballEffects.end(); ++iter1) {
		std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> >& currBallEffects = iter1->second;
		
		for (std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> >::iterator iter2 = currBallEffects.begin(); iter2 != currBallEffects.end(); ++iter2) {
			std::vector<ESPPointEmitter*>& effectList = iter2->second;

			for (std::vector<ESPPointEmitter*>::iterator iter3 = effectList.begin(); iter3 != effectList.end(); ++iter3) {
				ESPPointEmitter* currEmitter = *iter3;
				delete currEmitter;
				currEmitter = NULL;
			}
			effectList.clear();
		}
		currBallEffects.clear();
	}
	this->ballEffects.clear();

	// Clear all the paddle emitters
	for (std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> >::iterator iter1 = this->paddleEffects.begin(); iter1 != this->paddleEffects.end(); ++iter1) {
		std::vector<ESPPointEmitter*>& currPaddleEffects = iter1->second;

		for (std::vector<ESPPointEmitter*>::iterator iter2 = currPaddleEffects.begin(); iter2 != currPaddleEffects.end(); ++iter2) {
			ESPPointEmitter* currEmitter = *iter2;
			delete currEmitter;
			currEmitter = NULL;
		}
		currPaddleEffects.clear();
	}
	this->paddleEffects.clear();

	// Clear all the emitters for HUD Timers
	for (std::map<GameItem::ItemType, std::list<ESPEmitter*> >::iterator iter1 = this->activeTimerHUDEmitters.begin(); iter1 != this->activeTimerHUDEmitters.end(); ++iter1) {
		std::list<ESPEmitter*>& emitterList = iter1->second;
		for (std::list<ESPEmitter*>::iterator iter2 = emitterList.begin(); iter2 != emitterList.end(); ++iter2) {
			ESPEmitter* currEmitter = *iter2;
			delete currEmitter;
			currEmitter = NULL;
		}
		emitterList.clear();
	}
	this->activeTimerHUDEmitters.clear();

    // Clear boost effects
    for (BallEffectsMapIter iter1 = this->boostBallEmitters.begin(); iter1 != this->boostBallEmitters.end(); ++iter1) {
        std::list<ESPPointEmitter*>& ballEmitters = iter1->second;

        for (std::list<ESPPointEmitter*>::iterator iter2 = ballEmitters.begin(); iter2 != ballEmitters.end(); ++iter2) {
            ESPPointEmitter* currEmitter = *iter2;
            delete currEmitter;
            currEmitter = NULL;
        }
    }
    this->boostBallEmitters.clear();
}

void GameESPAssets::KillAllActiveTeslaLightningArcs() {
	// Clear all of the tesla lightning arcs
	for (std::map<std::pair<const TeslaBlock*, const TeslaBlock*>, std::list<ESPPointToPointBeam*> >::iterator iter1 = this->teslaLightningArcs.begin();
		iter1 != this->teslaLightningArcs.end(); ++iter1) {
		
			std::list<ESPPointToPointBeam*>& lightningArcs = iter1->second;
			for (std::list<ESPPointToPointBeam*>::iterator iter2 = lightningArcs.begin(); iter2 != lightningArcs.end(); ++iter2) {
				ESPPointToPointBeam* currArc = *iter2;
				delete currArc;
				currArc = NULL;
			}
			lightningArcs.clear();
	}
	this->teslaLightningArcs.clear();
}

/**
 * Public function for destroying all effects associated with a given ball.
 * (for when the ball dies or something like that).
 */
void GameESPAssets::KillAllActiveBallEffects(const GameBall& ball) {

    // Get rid of all ball boosting effects for the ball as well
    for (BallEffectsMapIter iter1 = this->boostBallEmitters.begin(); iter1 != this->boostBallEmitters.end();) {
        const GameBall* currBall = iter1->first;
        if (currBall == &ball) {
            std::list<ESPPointEmitter*>& ballEmitters = iter1->second;
            // Kill the boost-related emitters for the ball
            for (std::list<ESPPointEmitter*>::iterator iter2 = ballEmitters.begin(); iter2 != ballEmitters.end(); ++iter2) {
                ESPPointEmitter* currEmitter = *iter2;
			    delete currEmitter;
			    currEmitter = NULL;
            }
            ballEmitters.clear();
            iter1 = this->boostBallEmitters.erase(iter1);
            break;
        }
        else {
            ++iter1;
        }
    }

	// Check to see if there are any active effects for the ball, if not
	// then just exit this function
	std::map<const GameBall*, std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> > >::iterator foundBallEffects = this->ballEffects.find(&ball);
	if (foundBallEffects != this->ballEffects.end()) {
	    // Iterate through all effects and delete them, then remove them from the list
	    for (std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> >::iterator iter = foundBallEffects->second.begin(); iter != foundBallEffects->second.end(); ++iter) {
		    std::vector<ESPPointEmitter*>& effectList = iter->second;
		    for (std::vector<ESPPointEmitter*>::iterator iter2 = effectList.begin(); iter2 != effectList.end(); ++iter2) {
			    ESPPointEmitter* currEmitter = *iter2;
			    delete currEmitter;
			    currEmitter = NULL;
		    }
		    effectList.clear();
	    }

	    this->ballEffects.erase(foundBallEffects);
    }
}

/**
 * Private helper function for initializing textures used in the ESP effects.
 */
void GameESPAssets::InitESPTextures() {
	// TODO: fix texture filtering...
	debug_output("Loading ESP Textures...");
	
	// Initialize bang textures (big boom thingys when there are explosions)
	if (this->bangTextures.empty()) {
		this->bangTextures.reserve(3);
		Texture2D* temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BANG1, Texture::Trilinear));
		assert(temp != NULL);
		this->bangTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BANG2, Texture::Trilinear));
		assert(temp != NULL);
		this->bangTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BANG3, Texture::Trilinear));
		assert(temp != NULL);
		this->bangTextures.push_back(temp);
	}
	
	// Initialize splat textures (splatty thingys when ink blocks and other messy, gooey things explode)
	if (this->splatTextures.empty()) {
		this->splatTextures.reserve(1);
		Texture2D* temp =  static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SPLAT1, Texture::Trilinear));
		assert(temp != NULL);
		this->splatTextures.push_back(temp);
	}

	// Initialize smoke textures (cartoony puffs of smoke)
	if (this->smokeTextures.empty()) {
		this->smokeTextures.reserve(NUM_SMOKE_TEXTURES);
		Texture2D* temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE1, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE2, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE3, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE4, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE5, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE6, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);	
	}

	// Initialize all the fire glob textures
	//if (this->fireGlobTextures.empty()) {
	//	this->fireGlobTextures.reserve(3);
	//	Texture2D* temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_FIRE_GLOB1, Texture::Trilinear));
	//	assert(temp != NULL);
	//	this->fireGlobTextures.push_back(temp);
	//	temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_FIRE_GLOB2, Texture::Trilinear));
	//	assert(temp != NULL);
	//	this->fireGlobTextures.push_back(temp);
	//	temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_FIRE_GLOB3, Texture::Trilinear));
	//	assert(temp != NULL);
	//	this->fireGlobTextures.push_back(temp);
	//}

	// Initialize all the snowflake textures
	if (this->snowflakeTextures.empty()) {
		this->snowflakeTextures.reserve(3);
		Texture2D* temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SNOWFLAKE1, Texture::Trilinear));
		assert(temp != NULL);
		this->snowflakeTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SNOWFLAKE2, Texture::Trilinear));
		assert(temp != NULL);
		this->snowflakeTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SNOWFLAKE3, Texture::Trilinear));
		assert(temp != NULL);
		this->snowflakeTextures.push_back(temp);
	}

	// Initialize all rock textures
	if (this->rockTextures.empty()) {
		this->rockTextures.reserve(5);
		Texture2D* temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ROCK1, Texture::Trilinear));
		assert(temp != NULL);
		this->rockTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ROCK2, Texture::Trilinear));
		assert(temp != NULL);
		this->rockTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ROCK3, Texture::Trilinear));
		assert(temp != NULL);
		this->rockTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ROCK4, Texture::Trilinear));
		assert(temp != NULL);
		this->rockTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ROCK5, Texture::Trilinear));
		assert(temp != NULL);
		this->rockTextures.push_back(temp);
	}
	// Initialize all of the molten rock effects with the rock textures as masks
	if (this->moltenRockEffects.empty()) {
		assert(!this->rockTextures.empty());
		this->moltenRockEffects.reserve(this->rockTextures.size());
		for (std::vector<Texture2D*>::iterator iter = this->rockTextures.begin(); iter != this->rockTextures.end(); ++iter) {
			CgFxFireBallEffect* newFireEffect = new CgFxFireBallEffect();
			newFireEffect->SetTechnique(CgFxFireBallEffect::NO_DEPTH_WITH_MASK_TECHNIQUE_NAME);
			newFireEffect->SetMaskTexture(*iter);
			newFireEffect->SetDarkFireColour(Colour(1.0f, 0.9f, 0.15f));
			newFireEffect->SetBrightFireColour(Colour(0.75f, 0.15f, 0.10f));
			newFireEffect->SetScale(0.6f);
			this->moltenRockEffects.push_back(newFireEffect);
		}
	}

	if (this->circleGradientTex == NULL) {
		this->circleGradientTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_CIRCLE_GRADIENT, Texture::Trilinear));
		assert(this->circleGradientTex != NULL);
	}
	if (this->starTex == NULL) {
		this->starTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_STAR, Texture::Trilinear));
		assert(this->starTex != NULL);	
	}
	if (this->starOutlineTex == NULL) {
		this->starOutlineTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_STAR_OUTLINE, Texture::Trilinear));
		assert(this->starOutlineTex != NULL);
	}
	if (this->evilStarTex == NULL) {
		this->evilStarTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_EVIL_STAR, Texture::Trilinear));
		assert(this->evilStarTex != NULL);	
	}
	if (this->evilStarOutlineTex == NULL) {
		this->evilStarOutlineTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_EVIL_STAR_OUTLINE, Texture::Trilinear));
		assert(this->evilStarOutlineTex != NULL);		
	}
	if (this->explosionTex == NULL) {
		this->explosionTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_EXPLOSION_CLOUD, Texture::Trilinear));
		assert(this->explosionTex != NULL);
	}
	if (this->explosionRayTex == NULL) {
		this->explosionRayTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_EXPLOSION_RAYS, Texture::Trilinear));
		assert(this->explosionRayTex != NULL);
	}
	if (this->laserBeamTex == NULL) {
		this->laserBeamTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_LASER_BEAM, Texture::Trilinear));
		assert(this->laserBeamTex != NULL);
	}
	if (this->upArrowTex == NULL) {
		this->upArrowTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_UP_ARROW, Texture::Trilinear));
		assert(this->upArrowTex != NULL);
	}
	if (this->ballTex == NULL) {
		this->ballTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SPHERE, Texture::Trilinear));
		assert(this->ballTex != NULL);
	}
	if (this->targetTex == NULL) {
		this->targetTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BALLTARGET, Texture::Trilinear));
		assert(this->targetTex != NULL);
	}
	if (this->haloTex == NULL) {
		this->haloTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_HALO, Texture::Trilinear));
		assert(this->haloTex != NULL);
	}
	if (this->lensFlareTex == NULL) {
		this->lensFlareTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_LENSFLARE, Texture::Trilinear));
		assert(this->lensFlareTex != NULL);
	}
	if (this->sparkleTex == NULL) {
		this->sparkleTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SPARKLE, Texture::Trilinear));
		assert(this->sparkleTex != NULL);
	}
	if (this->spiralTex == NULL) {
		this->spiralTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_TWISTED_SPIRAL, Texture::Trilinear));
		assert(this->spiralTex != NULL);
	}
	if (this->sideBlastTex == NULL) {
		this->sideBlastTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SIDEBLAST, Texture::Trilinear));
		assert(this->sideBlastTex != NULL);
	}
	if (this->hugeExplosionTex == NULL) {
		this->hugeExplosionTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_HUGE_EXPLOSION, Texture::Trilinear));
		assert(this->hugeExplosionTex != NULL);
	}
    if (this->bubblyExplosionTex == NULL) {
        this->bubblyExplosionTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BUBBLY_EXPLOSION, Texture::Trilinear));
		assert(this->bubblyExplosionTex != NULL);
    }
	if (this->lightningBoltTex == NULL) {
		this->lightningBoltTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_LIGHTNING_BOLT, Texture::Trilinear));
		assert(this->lightningBoltTex != NULL);
	}
	if (this->sphereNormalsTex == NULL) {
		this->sphereNormalsTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SPHERE_NORMALS, Texture::Trilinear));
		assert(this->sphereNormalsTex != NULL);
	}
	//if (this->rectPrismTexture != NULL) {
	//	this->rectPrismTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_RECT_PRISM_NORMALS, Texture::Trilinear));
	//	assert(this->rectPrismTexture != NULL);
	//}	
	if (this->cloudTex == NULL) {
		this->cloudTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_CLOUD, Texture::Trilinear));
		assert(this->cloudTex != NULL);
	}
    if (this->vapourTrailTex == NULL) {
        this->vapourTrailTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_VAPOUR_TRAIL, Texture::Trilinear));
        assert(this->vapourTrailTex != NULL);
    }
    if (this->heartTex == NULL) {
        this->heartTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_HEART, Texture::Trilinear));
        assert(this->heartTex != NULL);
    }

	debug_opengl_state();
}

// Private helper function for initializing the uber ball's effects
void GameESPAssets::AddUberBallESPEffects(std::vector<ESPPointEmitter*>& effectsList) {
	assert(effectsList.size() == 0);

	// Set up the uberball emitters...
	ESPPointEmitter* uberBallEmitterAura = new ESPPointEmitter();
	uberBallEmitterAura->SetSpawnDelta(ESPInterval(-1));
	uberBallEmitterAura->SetInitialSpd(ESPInterval(0));
	uberBallEmitterAura->SetParticleLife(ESPInterval(-1));
	uberBallEmitterAura->SetParticleSize(ESPInterval(1.5f));
	uberBallEmitterAura->SetEmitAngleInDegrees(0);
	uberBallEmitterAura->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	uberBallEmitterAura->SetParticleAlignment(ESP::ScreenAligned);
	uberBallEmitterAura->SetEmitPosition(Point3D(0, 0, 0));
	uberBallEmitterAura->SetParticleColour(ESPInterval(GameModelConstants::GetInstance()->UBER_BALL_COLOUR.R()), 
																				 ESPInterval(GameModelConstants::GetInstance()->UBER_BALL_COLOUR.G()), 
																				 ESPInterval(GameModelConstants::GetInstance()->UBER_BALL_COLOUR.B()), 
																			   ESPInterval(0.75f));
	uberBallEmitterAura->AddEffector(&this->particlePulseUberballAura);
	bool result = uberBallEmitterAura->SetParticles(1, this->circleGradientTex);
	assert(result);

	ESPPointEmitter* uberBallEmitterTrail = new ESPPointEmitter();
	uberBallEmitterTrail->SetSpawnDelta(ESPInterval(0.005f));
	uberBallEmitterTrail->SetInitialSpd(ESPInterval(0.0f));
	uberBallEmitterTrail->SetParticleLife(ESPInterval(0.5f));
	uberBallEmitterTrail->SetParticleSize(ESPInterval(1.3f), ESPInterval(1.3f));
	uberBallEmitterTrail->SetEmitAngleInDegrees(0);
	uberBallEmitterTrail->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	uberBallEmitterTrail->SetParticleAlignment(ESP::ScreenAligned);
	uberBallEmitterTrail->SetEmitPosition(Point3D(0, 0, 0));
	uberBallEmitterTrail->AddEffector(&this->particleFaderUberballTrail);
	uberBallEmitterTrail->AddEffector(&this->particleShrinkToNothing);
	result = uberBallEmitterTrail->SetParticles(GameESPAssets::NUM_UBER_BALL_TRAIL_PARTICLES, this->circleGradientTex);
	assert(result);
	
	effectsList.push_back(uberBallEmitterTrail);
	effectsList.push_back(uberBallEmitterAura);
}

// Private helper function for initializing the ghost ball's effects
void GameESPAssets::AddGhostBallESPEffects(std::vector<ESPPointEmitter*>& effectsList) {
	assert(effectsList.size() == 0);

	ESPPointEmitter* ghostBallEmitterTrail = new ESPPointEmitter();
	ghostBallEmitterTrail->SetSpawnDelta(ESPInterval(0.01f));
	ghostBallEmitterTrail->SetInitialSpd(ESPInterval(0.0f));
	ghostBallEmitterTrail->SetParticleLife(ESPInterval(0.5f));
	ghostBallEmitterTrail->SetParticleSize(ESPInterval(1.5f, 2.0f));
	ghostBallEmitterTrail->SetParticleColour(ESPInterval(GameModelConstants::GetInstance()->GHOST_BALL_COLOUR.R()), 
																					 ESPInterval(GameModelConstants::GetInstance()->GHOST_BALL_COLOUR.G()), 
																					 ESPInterval(GameModelConstants::GetInstance()->GHOST_BALL_COLOUR.B()), 
																					 ESPInterval(1.0f));

	ghostBallEmitterTrail->SetEmitAngleInDegrees(20);
	ghostBallEmitterTrail->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	ghostBallEmitterTrail->SetParticleAlignment(ESP::ScreenAligned);
	ghostBallEmitterTrail->SetEmitPosition(Point3D(0, 0, 0));
	ghostBallEmitterTrail->AddEffector(&this->particleFader);
	ghostBallEmitterTrail->AddEffector(&this->ghostBallAccel1);
	bool result = ghostBallEmitterTrail->SetParticles(GameESPAssets::NUM_GHOST_SMOKE_PARTICLES, &this->ghostBallSmoke);
    UNUSED_VARIABLE(result);
	assert(result);

	effectsList.push_back(ghostBallEmitterTrail);
}

void GameESPAssets::AddFireBallESPEffects(const GameBall* ball, std::vector<ESPPointEmitter*>& effectsList) {
	assert(ball != NULL);
	assert(effectsList.size() == 0);

	ESPPointEmitter* fireBallEmitterTrail = new ESPPointEmitter();
	fireBallEmitterTrail->SetSpawnDelta(ESPInterval(0.01f));
	fireBallEmitterTrail->SetInitialSpd(ESPInterval(0.0f));
	fireBallEmitterTrail->SetParticleLife(ESPInterval(0.3f, 0.45f));
	fireBallEmitterTrail->SetParticleSize(ESPInterval(2.25f*ball->GetBounds().Radius(), 3.25f*ball->GetBounds().Radius()));
	fireBallEmitterTrail->SetEmitAngleInDegrees(10);
	fireBallEmitterTrail->SetParticleRotation(ESPInterval(-180.0f, 180.0f));
	fireBallEmitterTrail->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	fireBallEmitterTrail->SetParticleAlignment(ESP::ScreenAligned);
	fireBallEmitterTrail->SetEmitPosition(Point3D(0, 0, 0));
	fireBallEmitterTrail->AddEffector(&this->fireBallColourFader);
	fireBallEmitterTrail->AddEffector(&this->particleMediumGrowth);
	fireBallEmitterTrail->AddEffector(&this->fireBallAccel1);
	bool result = fireBallEmitterTrail->SetParticles(GameESPAssets::NUM_FIRE_BALL_PARTICLES, &this->fireBallTrailEffect);
    UNUSED_VARIABLE(result);
	assert(result);

	effectsList.push_back(fireBallEmitterTrail);
}

void GameESPAssets::AddIceBallESPEffects(const GameBall* ball, std::vector<ESPPointEmitter*>& effectsList) {
	assert(ball != NULL);
	assert(effectsList.size() == 0);

	static const unsigned int NUM_SNOWFLAKES_PER_EMITTER = 5;
	bool result = false;
	
	ESPPointEmitter* iceBallEmitterAura = new ESPPointEmitter();
	iceBallEmitterAura->SetSpawnDelta(ESPInterval(-1));
	iceBallEmitterAura->SetInitialSpd(ESPInterval(0));
	iceBallEmitterAura->SetParticleLife(ESPInterval(-1));
	iceBallEmitterAura->SetParticleSize(ESPInterval(1.5f));
	iceBallEmitterAura->SetEmitAngleInDegrees(0);
	iceBallEmitterAura->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	iceBallEmitterAura->SetParticleAlignment(ESP::ScreenAligned);
	iceBallEmitterAura->SetEmitPosition(Point3D(0, 0, 0));
	iceBallEmitterAura->SetParticleColour(ESPInterval(GameModelConstants::GetInstance()->ICE_BALL_COLOUR.R()), 
																				ESPInterval(GameModelConstants::GetInstance()->ICE_BALL_COLOUR.G()), 
																				ESPInterval(GameModelConstants::GetInstance()->ICE_BALL_COLOUR.B()), 
																			  ESPInterval(1.0f));
	iceBallEmitterAura->AddEffector(&this->particlePulseIceBallAura);
	iceBallEmitterAura->AddEffector(&this->loopRotateEffectorCW);
	result = iceBallEmitterAura->SetParticles(1, this->circleGradientTex);
	assert(result);
	effectsList.push_back(iceBallEmitterAura);

	ESPPointEmitter* sparklesEmitterTrail = new ESPPointEmitter();
	sparklesEmitterTrail->SetSpawnDelta(ESPInterval(0.05f, 0.15f));
	sparklesEmitterTrail->SetInitialSpd(ESPInterval(2.0f, 6.0f));
	sparklesEmitterTrail->SetParticleLife(ESPInterval(0.4f, 0.8f));
	sparklesEmitterTrail->SetParticleSize(ESPInterval(0.33f * ball->GetBounds().Radius(), ball->GetBounds().Radius()));
	sparklesEmitterTrail->SetParticleColour(ESPInterval(0.9f, 1.0f), ESPInterval(0.95f, 1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	sparklesEmitterTrail->SetEmitAngleInDegrees(15);
	sparklesEmitterTrail->SetParticleRotation(ESPInterval(-180.0f, 180.0f));
	sparklesEmitterTrail->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.75f * ball->GetBounds().Radius()));
	sparklesEmitterTrail->SetAsPointSpriteEmitter(true);
	sparklesEmitterTrail->SetEmitPosition(Point3D(0, 0, 0));
	sparklesEmitterTrail->AddEffector(&this->gravity);
	result = sparklesEmitterTrail->SetParticles(11, this->sparkleTex);
	assert(result);
	effectsList.push_back(sparklesEmitterTrail);	
	
	float numSnowflakeFraction = 1.0f / static_cast<float>(NUM_SNOWFLAKES_PER_EMITTER);
	const Colour& iceColour = GameModelConstants::GetInstance()->ICE_BALL_COLOUR;
	for (size_t i = 0; i < this->snowflakeTextures.size(); i++) {
		ESPPointEmitter* snowFlakeEmitterTrail = new ESPPointEmitter();
		snowFlakeEmitterTrail->SetSpawnDelta(ESPInterval(0.5f * numSnowflakeFraction, numSnowflakeFraction));
		snowFlakeEmitterTrail->SetInitialSpd(ESPInterval(3.0f, 8.0f));
		snowFlakeEmitterTrail->SetParticleLife(ESPInterval(0.33f, 0.9f));
		snowFlakeEmitterTrail->SetParticleSize(ESPInterval(0.5f * ball->GetBounds().Radius(), 2.0f*ball->GetBounds().Radius()));
		snowFlakeEmitterTrail->SetParticleColour(ESPInterval(iceColour.R(), 1.0f), ESPInterval(iceColour.G(), 1.0f), ESPInterval(iceColour.B()), ESPInterval(1.0f));
		snowFlakeEmitterTrail->SetEmitAngleInDegrees(25);
		snowFlakeEmitterTrail->SetParticleRotation(ESPInterval(-180.0f, 180.0f));
		snowFlakeEmitterTrail->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.75f * ball->GetBounds().Radius()));
		snowFlakeEmitterTrail->SetParticleAlignment(ESP::ScreenAligned);
		snowFlakeEmitterTrail->SetEmitPosition(Point3D(0, 0, 0));
		snowFlakeEmitterTrail->AddEffector(&this->particleFader);
		snowFlakeEmitterTrail->AddEffector(&this->particleMediumGrowth);
		if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
			snowFlakeEmitterTrail->AddEffector(&this->smokeRotatorCW);
		}
		else {
			snowFlakeEmitterTrail->AddEffector(&this->smokeRotatorCCW);
		}
		result = snowFlakeEmitterTrail->SetParticles(NUM_SNOWFLAKES_PER_EMITTER, this->snowflakeTextures[i]);
		assert(result);

		effectsList.push_back(snowFlakeEmitterTrail);
	}

	ESPPointEmitter* iceBallCloudEmitterTrail = new ESPPointEmitter();
	iceBallCloudEmitterTrail->SetSpawnDelta(ESPInterval(0.01f, 0.05f));
	iceBallCloudEmitterTrail->SetInitialSpd(ESPInterval(0.0f));
	iceBallCloudEmitterTrail->SetParticleLife(ESPInterval(0.35f, 0.55f));
	iceBallCloudEmitterTrail->SetParticleSize(ESPInterval(2.25f*ball->GetBounds().Radius(), 3.25f*ball->GetBounds().Radius()));
	iceBallCloudEmitterTrail->SetEmitAngleInDegrees(10);
	iceBallCloudEmitterTrail->SetParticleRotation(ESPInterval(-180.0f, 180.0f));
	iceBallCloudEmitterTrail->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	iceBallCloudEmitterTrail->SetParticleAlignment(ESP::ScreenAligned);
	iceBallCloudEmitterTrail->SetEmitPosition(Point3D(0, 0, 0));
	iceBallCloudEmitterTrail->AddEffector(&this->iceBallColourFader);
	iceBallCloudEmitterTrail->AddEffector(&this->particleMediumGrowth);
	iceBallCloudEmitterTrail->AddEffector(&this->iceBallAccel);
	result = iceBallCloudEmitterTrail->SetParticles(15, &this->iceBallTrailEffect);
	assert(result);
	effectsList.push_back(iceBallCloudEmitterTrail);
}

/**
 * Private helper function - sets up any ball effects that are associated with the paddle
 * camera mode.
 */
void GameESPAssets::AddPaddleCamBallESPEffects(std::vector<ESPPointEmitter*>& effectsList) {
	assert(effectsList.size() == 0);
	effectsList.reserve(effectsList.size() + 1);
	effectsList.push_back(this->CreateSpinningTargetESPEffect());
}

/**
 * Private helper function - sets up any paddle effects that are associated with the ball
 * camera mode.
 */
void GameESPAssets::AddBallCamPaddleESPEffects(std::vector<ESPPointEmitter*>& effectsList) {
	assert(effectsList.size() == 0);
	effectsList.reserve(effectsList.size() + 1);
	effectsList.push_back(this->CreateSpinningTargetESPEffect());
}

/**
 * Initialize the standalone effects for the paddle laser.
 */
void GameESPAssets::InitLaserPaddleESPEffects() {
	assert(this->crazyBallAura == NULL);
    assert(this->boostSparkleEmitterLight == NULL);
    assert(this->boostSparkleEmitterDark == NULL);
	assert(this->paddleLaserGlowAura == NULL);
	assert(this->paddleLaserGlowSparks == NULL);
	assert(this->paddleBeamGlowSparks == NULL);
	assert(this->paddleBeamOriginUp == NULL);
	assert(this->paddleBeamBlastBits == NULL);

	bool result = false;

	this->crazyBallAura = new ESPPointEmitter();
	this->crazyBallAura->SetSpawnDelta(ESPInterval(-1));
	this->crazyBallAura->SetInitialSpd(ESPInterval(0));
	this->crazyBallAura->SetParticleLife(ESPInterval(-1));
	this->crazyBallAura->SetParticleSize(ESPInterval(1.5f));
	this->crazyBallAura->SetEmitAngleInDegrees(0);
	this->crazyBallAura->SetParticleAlignment(ESP::ScreenAligned);
	this->crazyBallAura->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->crazyBallAura->SetEmitPosition(Point3D(0, 0, 0));
	this->crazyBallAura->SetParticleColour(ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(0.0f), ESPInterval(1.0f));
	this->crazyBallAura->AddEffector(&this->particlePulsePaddleLaser);
	result = this->crazyBallAura->SetParticles(1, this->circleGradientTex);

    this->boostSparkleEmitterLight = new ESPPointEmitter();
    this->boostSparkleEmitterLight->SetSpawnDelta(ESPInterval(0.01f, 0.02f));
    this->boostSparkleEmitterLight->SetInitialSpd(ESPInterval(2*GameBall::DEFAULT_BALL_RADIUS, 4*GameBall::DEFAULT_BALL_RADIUS));
    this->boostSparkleEmitterLight->SetParticleLife(ESPInterval(0.75f, 1.5f));
    this->boostSparkleEmitterLight->SetParticleSize(ESPInterval(1.25f * GameBall::DEFAULT_BALL_RADIUS, 2.5f * GameBall::DEFAULT_BALL_RADIUS));
    this->boostSparkleEmitterLight->SetParticleColour(ESPInterval(1), ESPInterval(1), ESPInterval(1), ESPInterval(1));
    this->boostSparkleEmitterLight->SetEmitAngleInDegrees(0);
    this->boostSparkleEmitterLight->SetEmitDirection(Vector3D(1, 0, 0));
    this->boostSparkleEmitterLight->SetEmitPosition(Point3D(0,0,0));
    this->boostSparkleEmitterLight->SetParticleAlignment(ESP::ScreenAligned);
    this->boostSparkleEmitterLight->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    this->boostSparkleEmitterLight->AddEffector(&this->particleMediumShrink);
    this->boostSparkleEmitterLight->AddEffector(&this->particleFader);
    result = this->boostSparkleEmitterLight->SetParticles(26, this->sparkleTex);
    assert(result);

    this->boostSparkleEmitterDark = new ESPPointEmitter();
    this->boostSparkleEmitterDark->SetSpawnDelta(ESPInterval(0.02f, 0.04f));
    this->boostSparkleEmitterDark->SetInitialSpd(ESPInterval(2*GameBall::DEFAULT_BALL_RADIUS, 4*GameBall::DEFAULT_BALL_RADIUS));
    this->boostSparkleEmitterDark->SetParticleLife(ESPInterval(0.75f, 1.5f));
    this->boostSparkleEmitterDark->SetParticleSize(ESPInterval(1.45f * GameBall::DEFAULT_BALL_RADIUS, 2.66f * GameBall::DEFAULT_BALL_RADIUS));
    this->boostSparkleEmitterDark->SetParticleColour(ESPInterval(0), ESPInterval(0), ESPInterval(0), ESPInterval(1));
    this->boostSparkleEmitterDark->SetEmitAngleInDegrees(0);
    this->boostSparkleEmitterDark->SetEmitDirection(Vector3D(1, 0, 0));
    this->boostSparkleEmitterDark->SetEmitPosition(Point3D(0,0,0));
    this->boostSparkleEmitterDark->SetParticleAlignment(ESP::ScreenAligned);
    this->boostSparkleEmitterDark->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    this->boostSparkleEmitterDark->AddEffector(&this->particleMediumShrink);
    this->boostSparkleEmitterDark->AddEffector(&this->particleFader);
    result = this->boostSparkleEmitterDark->SetParticles(13, this->sparkleTex);
    assert(result);

	this->paddleLaserGlowAura = new ESPPointEmitter();
	this->paddleLaserGlowAura->SetSpawnDelta(ESPInterval(-1));
	this->paddleLaserGlowAura->SetInitialSpd(ESPInterval(0));
	this->paddleLaserGlowAura->SetParticleLife(ESPInterval(-1));
	this->paddleLaserGlowAura->SetParticleSize(ESPInterval(1.5f));
	this->paddleLaserGlowAura->SetEmitAngleInDegrees(0);
	this->paddleLaserGlowAura->SetParticleAlignment(ESP::ScreenAligned);
	this->paddleLaserGlowAura->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->paddleLaserGlowAura->SetEmitPosition(Point3D(0, 0, 0));
	this->paddleLaserGlowAura->SetParticleColour(ESPInterval(0.5f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	this->paddleLaserGlowAura->AddEffector(&this->particlePulsePaddleLaser);
	result = this->paddleLaserGlowAura->SetParticles(1, this->circleGradientTex);
	assert(result);

	this->paddleLaserGlowSparks = new ESPPointEmitter();
	this->paddleLaserGlowSparks->SetSpawnDelta(ESPInterval(0.033f));
	this->paddleLaserGlowSparks->SetInitialSpd(ESPInterval(1.5f, 3.5f));
	this->paddleLaserGlowSparks->SetParticleLife(ESPInterval(0.8f));
	this->paddleLaserGlowSparks->SetParticleSize(ESPInterval(0.1f, 0.5f));
	this->paddleLaserGlowSparks->SetParticleColour(ESPInterval(0.5f, 0.9f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	this->paddleLaserGlowSparks->SetEmitAngleInDegrees(90);
	this->paddleLaserGlowSparks->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->paddleLaserGlowSparks->SetAsPointSpriteEmitter(true);
	this->paddleLaserGlowSparks->SetEmitPosition(Point3D(0, 0, 0));
	this->paddleLaserGlowSparks->SetEmitDirection(Vector3D(0, 1, 0));
	this->paddleLaserGlowSparks->AddEffector(&this->particleFader);
	result = this->paddleLaserGlowSparks->SetParticles(NUM_PADDLE_LASER_SPARKS, this->circleGradientTex);
	assert(result);		
	
	this->paddleBeamGlowSparks = new ESPVolumeEmitter();
	this->paddleBeamGlowSparks->SetSpawnDelta(ESPInterval(0.005f, 0.01f));
	this->paddleBeamGlowSparks->SetInitialSpd(ESPInterval(2.0f, 5.0f));
	this->paddleBeamGlowSparks->SetParticleLife(ESPInterval(0.75f, 1.0f));
	this->paddleBeamGlowSparks->SetParticleColour(ESPInterval(0.6f, 1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	this->paddleBeamGlowSparks->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->paddleBeamGlowSparks->SetAsPointSpriteEmitter(true);
	this->paddleBeamGlowSparks->SetEmitDirection(Vector3D(0, 1, 0));
	this->paddleBeamGlowSparks->AddEffector(&this->particleFader);
	result = this->paddleBeamGlowSparks->SetParticles(50, this->circleGradientTex);
	assert(result);

	this->paddleBeamOriginUp = new ESPVolumeEmitter();
	this->paddleBeamOriginUp->SetSpawnDelta(ESPInterval(0.01f));
	this->paddleBeamOriginUp->SetInitialSpd(ESPInterval(3.0f, 8.0f));
	this->paddleBeamOriginUp->SetParticleLife(ESPInterval(1.0f));
	this->paddleBeamOriginUp->SetParticleColour(ESPInterval(0.9f, 1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	this->paddleBeamOriginUp->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->paddleBeamOriginUp->SetAsPointSpriteEmitter(true);
	this->paddleBeamOriginUp->SetEmitDirection(Vector3D(0, 1, 0));
	this->paddleBeamOriginUp->AddEffector(&this->particleFader);
	result = this->paddleBeamOriginUp->SetParticles(NUM_PADDLE_BEAM_ORIGIN_PARTICLES, this->sparkleTex);
	assert(result);

	this->paddleBeamBlastBits = new ESPPointEmitter();
	this->paddleBeamBlastBits->SetSpawnDelta(ESPInterval(0.001f, 0.005f));
	this->paddleBeamBlastBits->SetInitialSpd(ESPInterval(1.5f, 2.5f));
	this->paddleBeamBlastBits->SetParticleLife(ESPInterval(0.75f, 1.0f));
	this->paddleBeamBlastBits->SetEmitAngleInDegrees(75);
	this->paddleBeamBlastBits->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->paddleBeamBlastBits->SetAsPointSpriteEmitter(false);
	this->paddleBeamBlastBits->SetParticleAlignment(ESP::ScreenAlignedFollowVelocity);
	this->paddleBeamBlastBits->SetEmitPosition(Point3D(0, 0, 0));
	//this->paddleBeamBlastBits->SetParticleColour(ESPInterval(0.4f, 0.75f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	this->paddleBeamBlastBits->SetToggleEmitOnPlane(true, Vector3D(0, 0, 1));
	this->paddleBeamBlastBits->AddEffector(&this->particleLargeVStretch);
	this->paddleBeamBlastBits->AddEffector(&this->beamBlastColourEffector);
	result = this->paddleBeamBlastBits->SetParticles(35, this->circleGradientTex);
	assert(result);	
}

/**
 * Private helper function, called in order to create a spinning target effect that
 * can be attached to a game object to make it stand out to the player (e.g., in paddle/ball
 * cam mode attach it to the balls/paddle to make it more identifiable).
 */
ESPPointEmitter* GameESPAssets::CreateSpinningTargetESPEffect() {

	ESPPointEmitter* spinningTarget = new ESPPointEmitter();
	spinningTarget->SetSpawnDelta(ESPInterval(-1));
	spinningTarget->SetInitialSpd(ESPInterval(0));
	spinningTarget->SetParticleLife(ESPInterval(-1));
	spinningTarget->SetParticleSize(ESPInterval(2));
	spinningTarget->SetEmitAngleInDegrees(0);
	spinningTarget->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	spinningTarget->SetParticleAlignment(ESP::ScreenAligned);
	spinningTarget->SetEmitPosition(Point3D(0, 0, 0));
	spinningTarget->SetParticleColour(ESPInterval(1), ESPInterval(1), ESPInterval(1), ESPInterval(0.5f));
	spinningTarget->AddEffector(&this->loopRotateEffectorCW);

	bool result = spinningTarget->SetParticles(1, this->targetTex);
    UNUSED_VARIABLE(result);
	assert(result);

	return spinningTarget;
}

/**
 * Private helper function for initializing standalone ESP effects.
 */
void GameESPAssets::InitStandaloneESPEffects() {

	this->InitLaserPaddleESPEffects();

	// Pulsing effect for particles
	ScaleEffect itemDropPulseSettings;
	itemDropPulseSettings.pulseGrowthScale = 1.5f;
	itemDropPulseSettings.pulseRate = 1.0f;
	this->particlePulseItemDropAura = ESPParticleScaleEffector(itemDropPulseSettings);

	ScaleEffect paddleLaserPulseSettings;
	paddleLaserPulseSettings.pulseGrowthScale = 1.33f;
	paddleLaserPulseSettings.pulseRate = 1.0f;
	this->particlePulsePaddleLaser = ESPParticleScaleEffector(paddleLaserPulseSettings);
	
	ScaleEffect beamPulseSettings;
	beamPulseSettings.pulseGrowthScale = 1.25f;
	beamPulseSettings.pulseRate        = 0.5f;
	this->beamEndPulse = ESPParticleScaleEffector(beamPulseSettings);

	ScaleEffect fireGlobPulseSettings;
	fireGlobPulseSettings.pulseGrowthScale = 1.6f;
	fireGlobPulseSettings.pulseRate = 0.75f;
	this->particlePulseFireGlobAura = ESPParticleScaleEffector(fireGlobPulseSettings);

	// Initialize uberball effectors
	ScaleEffect uberBallPulseSettings;
	uberBallPulseSettings.pulseGrowthScale = 2.0f;
	uberBallPulseSettings.pulseRate = 1.5f;
	this->particlePulseUberballAura = ESPParticleScaleEffector(uberBallPulseSettings);

	ScaleEffect iceBallPulseSettings;
	iceBallPulseSettings.pulseGrowthScale = 1.65f;
	iceBallPulseSettings.pulseRate = 1.25f;
	this->particlePulseIceBallAura = ESPParticleScaleEffector(iceBallPulseSettings);

	// Ghost smoke effect used for ghostball
	this->ghostBallSmoke.SetTechnique(CgFxVolumetricEffect::SMOKESPRITE_TECHNIQUE_NAME);
	this->ghostBallSmoke.SetScale(0.5f);
	this->ghostBallSmoke.SetFrequency(0.25f);
	this->ghostBallSmoke.SetFlowDirection(Vector3D(0, 0, 1));
	this->ghostBallSmoke.SetMaskTexture(this->circleGradientTex);

	// Fire effect used in various things - like explosions and such.
	this->fireEffect.SetTechnique(CgFxVolumetricEffect::FIRESPRITE_TECHNIQUE_NAME);
	this->fireEffect.SetColour(Colour(1.00f, 1.00f, 1.00f));
	this->fireEffect.SetScale(0.25f);
	this->fireEffect.SetFrequency(1.0f);
	this->fireEffect.SetFlowDirection(Vector3D(0, 0, 1));
	this->fireEffect.SetMaskTexture(this->circleGradientTex);

	this->fireBallTrailEffect.SetTechnique(CgFxVolumetricEffect::FIRESPRITE_TECHNIQUE_NAME);
	this->fireBallTrailEffect.SetColour(Colour(1.0f, 1.0f, 1.0f));
	this->fireBallTrailEffect.SetScale(0.8f);
	this->fireBallTrailEffect.SetFrequency(1.2f);
	this->fireBallTrailEffect.SetFlowDirection(Vector3D(0, 0, 1));
	this->fireBallTrailEffect.SetMaskTexture(this->cloudTex);

	this->iceBallTrailEffect.SetTechnique(CgFxVolumetricEffect::SMOKESPRITE_TECHNIQUE_NAME);
	this->iceBallTrailEffect.SetColour(Colour(1.0f, 1.0f, 1.0f));
	this->iceBallTrailEffect.SetScale(0.5f);
	this->iceBallTrailEffect.SetFrequency(0.5f);
	this->iceBallTrailEffect.SetFlowDirection(Vector3D(0, 0, 1));
	this->iceBallTrailEffect.SetMaskTexture(this->cloudTex);

	// Setup the post refraction normal effect with the sphere normal - this is for forcefield
	// and shockwave effects
	this->normalTexRefractEffect.SetTechnique(CgFxPostRefract::NORMAL_TEXTURE_TECHNIQUE_NAME);
	this->normalTexRefractEffect.SetWarpAmountParam(27.0f);
	this->normalTexRefractEffect.SetIndexOfRefraction(1.2f);
	this->normalTexRefractEffect.SetNormalTexture(this->sphereNormalsTex);

    this->vapourTrailRefractEffect.SetTechnique(CgFxPostRefract::NORMAL_TEXTURE_TECHNIQUE_NAME);
	this->vapourTrailRefractEffect.SetWarpAmountParam(20.0f);
	this->vapourTrailRefractEffect.SetIndexOfRefraction(1.33f);
	this->vapourTrailRefractEffect.SetNormalTexture(this->vapourTrailTex);

	//this->iceRefractEffect.SetTechnique(CgFxPostRefract::NORMAL_TEXTURE_TECHNIQUE_NAME);
	//this->iceRefractEffect.SetWarpAmountParam(50.0f);
	//this->iceRefractEffect.SetIndexOfRefraction(1.33f);
	//this->iceRefractEffect.SetNormalTexture(this->rectPrismTexture);
}

/**
 * Creator method for making the onomata particle effect for when the ball hits various things.
 */
ESPPointEmitter* GameESPAssets::CreateBallBounceEffect(const GameBall& ball, Onomatoplex::SoundType soundType) {
	
	// The effect is a basic onomatopeia word that occurs at the position of the ball
	ESPPointEmitter* bounceEffect = new ESPPointEmitter();
	// Set up the emitter...
	bounceEffect->SetSpawnDelta(ESPInterval(-1, -1));
	bounceEffect->SetInitialSpd(ESPInterval(0.5f, 1.5f));
	bounceEffect->SetParticleLife(ESPInterval(1.2f, 1.5f));
	bounceEffect->SetParticleSize(ESPInterval(0.5f, 0.75f));
	bounceEffect->SetParticleRotation(ESPInterval(-15.0f, 15.0f));
	bounceEffect->SetEmitAngleInDegrees(10);
	bounceEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.0f));
	bounceEffect->SetParticleAlignment(ESP::ScreenAligned);
	bounceEffect->SetParticleColour(ESPInterval(0.6f, 1.0f), ESPInterval(0.6f, 1.0f), ESPInterval(0.6f, 1.0f), ESPInterval(1));
	
	Vector2D ballVelocity = ball.GetVelocity();
	bounceEffect->SetEmitDirection(Vector3D(ballVelocity[0], ballVelocity[1], 0.0f));

	Point2D ballCenter    = ball.GetBounds().Center();
	bounceEffect->SetEmitPosition(Point3D(ballCenter[0], ballCenter[1], 0.0f));
	
	// Add effectors...
	bounceEffect->AddEffector(&this->particleFader);
	bounceEffect->AddEffector(&this->particleSmallGrowth);

	// Add the single particle to the emitter
	DropShadow dpTemp;
	dpTemp.amountPercentage = 0.10f;

	// Font style is based off the type of sound
	GameFontAssetsManager::FontStyle fontStyle;
	switch (soundType) {
		case Onomatoplex::BOUNCE:
			fontStyle = GameFontAssetsManager::ExplosionBoom;
			break;
		case Onomatoplex::GOO:
			fontStyle = GameFontAssetsManager::SadBadGoo;
			break;
		default:
			fontStyle = GameFontAssetsManager::ExplosionBoom;
			break;
	}

	ESPOnomataParticle* bounceParticle = new ESPOnomataParticle(GameFontAssetsManager::GetInstance()->GetFont(fontStyle, GameFontAssetsManager::Small));
	bounceParticle->SetDropShadow(dpTemp);
	bounceParticle->SetOnomatoplexSound(soundType, ball.GetOnomatoplexExtremeness());
	bounceEffect->AddParticle(bounceParticle);

	return bounceEffect;
}

/**
 * Adds a ball bouncing ESP - the effect that occurs when a ball typically
 * bounces off a level piece (e.g., a block).
 */
void GameESPAssets::AddBounceLevelPieceEffect(const GameBall& ball, const LevelPiece& block) {
	// We don't do the effect for certain types of blocks...
	if (block.GetType() == LevelPiece::Bomb || block.GetType() == LevelPiece::Ink || block.GetType() == LevelPiece::Portal ||
		block.GetType() == LevelPiece::Cannon) {
		return;
	}

	// Add the new emitter to the list of active emitters
	this->activeGeneralEmitters.push_front(this->CreateBallBounceEffect(ball, Onomatoplex::BOUNCE));
}

/**
 * Adds ball bouncing effect when the ball bounces off the player paddle.
 */
void GameESPAssets::AddBouncePaddleEffect(const GameBall& ball, const PlayerPaddle& paddle) {
	// Shield takes priority over the sticky paddle
	if ((paddle.GetPaddleType() & PlayerPaddle::ShieldPaddle) == PlayerPaddle::ShieldPaddle) {
		// The ball hits a energy shield bbzzzap!
		this->AddEnergyShieldHitEffect(paddle.GetCenterPosition(), ball);
	}
	else if ((paddle.GetPaddleType() & PlayerPaddle::StickyPaddle) == PlayerPaddle::StickyPaddle && 
		       (paddle.GetPaddleType() & PlayerPaddle::ShieldPaddle) == NULL) {
		// The sticky paddle should make a spongy gooey sound when hit by the ball...
		this->activeGeneralEmitters.push_front(this->CreateBallBounceEffect(ball, Onomatoplex::GOO));
	}
	else {
		// Typical paddle bounce, similar to a regular block bounce effect
		this->activeGeneralEmitters.push_front(this->CreateBallBounceEffect(ball, Onomatoplex::BOUNCE));
	}
}

/**
 * Adds the effect that occurs when two balls bounce off of each other.
 */
void GameESPAssets::AddBounceBallBallEffect(const GameBall& ball1, const GameBall& ball2) {
	// Obtain a reasonably centered position to show the effect
	const Point2D& ball1Center = ball1.GetBounds().Center();
	const Point2D& ball2Center = ball2.GetBounds().Center();
	const Point2D emitPoint2D = Point2D::GetMidPoint(ball1Center, ball2Center);
	const Point3D emitPoint3D(emitPoint2D[0], emitPoint2D[1], 0.0f);
	
	// Small 'bang' star that appears at the position of the ball-ball collision
	// Create an emitter for the bang texture
	ESPPointEmitter* bangEffect = new ESPPointEmitter();
	bangEffect->SetSpawnDelta(ESPInterval(-1));
	bangEffect->SetInitialSpd(ESPInterval(0.0f));
	bangEffect->SetParticleLife(ESPInterval(0.8f, 1.1f));
	bangEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	bangEffect->SetParticleAlignment(ESP::ScreenAligned);
	bangEffect->SetEmitPosition(emitPoint3D);
	bangEffect->SetParticleRotation(ESPInterval(0.0f, 360.0f));
	bangEffect->SetParticleSize(ESPInterval(0.75f, 1.25f));
	bangEffect->SetParticleColour(ESPInterval(1.0f), ESPInterval(0.75f, 1.0f), ESPInterval(0.0f), ESPInterval(1.0f));
	bangEffect->AddEffector(&this->particleFader);
	bangEffect->AddEffector(&this->particleMediumGrowth);
	bangEffect->SetParticles(1, this->evilStarTex);

	// Basic onomatopeia word that occurs at the position of the ball-ball collision
	ESPPointEmitter* bounceEffect = new ESPPointEmitter();
	bounceEffect->SetSpawnDelta(ESPInterval(-1));
	bounceEffect->SetInitialSpd(ESPInterval(0.0f));
	bounceEffect->SetParticleLife(ESPInterval(1.2f, 1.5f));
	bounceEffect->SetParticleSize(ESPInterval(0.5f, 0.75f));
	bounceEffect->SetParticleRotation(ESPInterval(-15.0f, 15.0f));
	bounceEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.0f));
	bounceEffect->SetParticleAlignment(ESP::ScreenAligned);
	bounceEffect->SetParticleColour(ESPInterval(0.6f, 1.0f), ESPInterval(0.6f, 1.0f), ESPInterval(0.6f, 1.0f), ESPInterval(1));
	bounceEffect->SetEmitPosition(emitPoint3D);
	
	// Add effectors...
	bounceEffect->AddEffector(&this->particleFader);
	bounceEffect->AddEffector(&this->particleSmallGrowth);

	// Add the single particle to the emitter
	DropShadow dpTemp;
	dpTemp.amountPercentage = 0.10f;
	ESPOnomataParticle* bounceParticle = new ESPOnomataParticle(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Small));
	bounceParticle->SetDropShadow(dpTemp);
	bounceParticle->SetOnomatoplexSound(Onomatoplex::BOUNCE, std::max<Onomatoplex::Extremeness>(ball1.GetOnomatoplexExtremeness(), ball2.GetOnomatoplexExtremeness()));
	bounceEffect->AddParticle(bounceParticle);

	this->activeGeneralEmitters.push_front(bangEffect);
	this->activeGeneralEmitters.push_front(bounceEffect);
}

/**
 * Add effects based on what happens when a given projectile hits a given level piece.
 */
void GameESPAssets::AddBlockHitByProjectileEffect(const Projectile& projectile, const LevelPiece& block) {
	switch (projectile.GetType()) {
        case Projectile::BallLaserBulletProjectile:
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
			switch(block.GetType()) {

				case LevelPiece::PrismTriangle:
				case LevelPiece::Prism: 
					{
						// A laser bullet just hit a prism block... cause the proper effect
						Point2D midPoint = Point2D::GetMidPoint(projectile.GetPosition(), block.GetCenter()); 
						this->AddLaserHitPrismBlockEffect(midPoint);
					} 
					break;

				case LevelPiece::Solid:
                case LevelPiece::OneWay:
                case LevelPiece::NoEntry:
				case LevelPiece::SolidTriangle:
				case LevelPiece::Breakable:
				case LevelPiece::BreakableTriangle: 
				case LevelPiece::Tesla:
				case LevelPiece::Cannon:
				case LevelPiece::ItemDrop:
				case LevelPiece::Collateral:
                case LevelPiece::Switch:
                case LevelPiece::LaserTurret:
                case LevelPiece::RocketTurret:
                case LevelPiece::MineTurret:
					{
						bool blockIsFrozen = block.HasStatus(LevelPiece::IceCubeStatus);
						Point2D midPoint = Point2D::GetMidPoint(projectile.GetPosition(), block.GetCenter()); 
						if (blockIsFrozen) {
							// Frozen blocks reflect/refract laser beams...
							this->AddLaserHitPrismBlockEffect(midPoint);
						}
						else {
							// A laser just hit a block and was disapated by it... show the particle disintegrate
							this->AddLaserHitWallEffect(midPoint);
						}
					}
					break;

				case LevelPiece::Ink:
				case LevelPiece::Bomb:
					{
						bool blockIsFrozen = block.HasStatus(LevelPiece::IceCubeStatus);
						if (blockIsFrozen) {
							// Frozen blocks reflect/refract laser beams...
							Point2D midPoint = Point2D::GetMidPoint(projectile.GetPosition(), block.GetCenter()); 
							this->AddLaserHitPrismBlockEffect(midPoint);
						}
					}
					break;

				case LevelPiece::Empty:
				case LevelPiece::Portal:
					// Certain level pieces require no effects...
					break;

				default:
					// If you get an assert fail here then you'll need to add a particle
					// destruction effect for the new block type!
					debug_output("GameESPAssets: There's no implementation for the effects when a laser bullet projectile hits this block type!");
					assert(false);
					break;
			}
			break;
		
		case Projectile::PaddleRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
            // NOTE: THIS IS NOW TAKEN CARE OF BY THE RocketExploded event
			break;

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
            if (!block.ProjectileIsDestroyedOnCollision(&projectile)) {
                this->AddPaddleMineAttachedEffects(projectile);
            }
            break;

		case Projectile::CollateralBlockProjectile:
			break;

		case Projectile::FireGlobProjectile:
			break;

		default:
			debug_output("GameESPAssets: There's no implementation for the effects for this type of projectile when it hits any block!");
			assert(false);
			break;
	}
}

void GameESPAssets::AddSafetyNetHitByProjectileEffect(const Projectile& projectile) {
    switch (projectile.GetType()) {

        case Projectile::BallLaserBulletProjectile:
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
            this->AddLaserHitWallEffect(projectile.GetPosition());
            break;

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
            this->AddPaddleMineAttachedEffects(projectile);
            break;

        default:
            break;
    }
}

void GameESPAssets::AddBallHitLightningArcEffect(const GameBall& ball) {

	// Add the lightning bolt graphic and onomatopeia effect
	ESPInterval boltLifeInterval		= ESPInterval(0.8f, 1.1f);
	ESPInterval boltOnoLifeInterval	= ESPInterval(boltLifeInterval.minValue + 0.3f, boltLifeInterval.maxValue + 0.3f);
	ESPInterval sizeIntervalX(0.8f, 1.0f);
	ESPInterval sizeIntervalY(1.65f, 1.8f);

	Point3D emitPosition = ball.GetCenterPosition();

	// Create an emitter for the lightning bolt texture
	ESPPointEmitter* lightningBoltEffect = new ESPPointEmitter();
	
	// Set up the emitter...
	lightningBoltEffect->SetSpawnDelta(ESPInterval(-1, -1));
	lightningBoltEffect->SetInitialSpd(ESPInterval(0.0f, 0.0f));
	lightningBoltEffect->SetParticleLife(boltLifeInterval);
	lightningBoltEffect->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	lightningBoltEffect->SetParticleAlignment(ESP::ScreenAligned);
	lightningBoltEffect->SetEmitPosition(emitPosition);
	lightningBoltEffect->SetParticleRotation(ESPInterval(-15.0f, 15.0f));
	lightningBoltEffect->SetParticleSize(sizeIntervalX, sizeIntervalY);
	lightningBoltEffect->SetParticleColour(ESPInterval(1.0f), ESPInterval(0.9f, 1.0f), ESPInterval(0.0f), ESPInterval(1.0f));
	lightningBoltEffect->AddEffector(&this->particleFader);
	lightningBoltEffect->AddEffector(&this->particleMediumGrowth);
	bool result = lightningBoltEffect->SetParticles(1, this->lightningBoltTex);
    UNUSED_VARIABLE(result);
	assert(result);

	// Create an emitter for the sound of onomatopeia of the breaking block
	ESPPointEmitter* boltOnoEffect = new ESPPointEmitter();
	// Set up the emitter...
	boltOnoEffect->SetSpawnDelta(ESPInterval(-1, -1));
	boltOnoEffect->SetInitialSpd(ESPInterval(0.0f, 0.0f));
	boltOnoEffect->SetParticleLife(boltOnoLifeInterval);
	boltOnoEffect->SetParticleSize(ESPInterval(0.7f, 1.0f), ESPInterval(1.0f, 1.0f));
	boltOnoEffect->SetParticleRotation(ESPInterval(-20.0f, 20.0f));
	boltOnoEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.2f));
	boltOnoEffect->SetParticleAlignment(ESP::ScreenAligned);
	boltOnoEffect->SetEmitPosition(emitPosition);
	boltOnoEffect->AddEffector(&this->particleFader);
	boltOnoEffect->AddEffector(&this->particleSmallGrowth);

	// Add the single text particle to the emitter with the severity of the effect...
	TextLabel2D boltTextLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ElectricZap, GameFontAssetsManager::Small), "");
    boltTextLabel.SetScale(0.8f);
	boltTextLabel.SetColour(Colour(1, 1, 1));
	boltTextLabel.SetDropShadow(Colour(0, 0, 0), 0.1f);

	// TODO: Make this more dynamic...
	Onomatoplex::Extremeness severity = Onomatoplex::GOOD;
	Onomatoplex::SoundType type = Onomatoplex::ELECTRIC;
	boltOnoEffect->SetParticles(1, boltTextLabel, type, severity);	
	
	// Add the shockwave for the ball hitting the lightning...
	ESPPointEmitter* shockwaveEffect = this->CreateShockwaveEffect(emitPosition, 3.0f * GameBall::DEFAULT_BALL_RADIUS, 0.5f);
	assert(shockwaveEffect != NULL);

	// Add the bolt graphic, its sound graphic and the shockwave to the active general emitters
	this->activeGeneralEmitters.push_back(shockwaveEffect);
	this->activeGeneralEmitters.push_back(lightningBoltEffect);
	this->activeGeneralEmitters.push_back(boltOnoEffect);
}

ESPPointEmitter* GameESPAssets::CreateTeleportEffect(const Point2D& center, const PortalBlock& block, bool isSibling) {
	const Colour& portalColour = block.GetColour();
	// Create an emitter for the spirally teleportation texture
	ESPPointEmitter* spiralEffect = new ESPPointEmitter();
	spiralEffect->SetSpawnDelta(ESPInterval(-1, -1));
	spiralEffect->SetInitialSpd(ESPInterval(0.0f, 0.0f));
	spiralEffect->SetParticleLife(1.0f);
	spiralEffect->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	spiralEffect->SetParticleAlignment(ESP::ScreenAligned);
	spiralEffect->SetEmitPosition(Point3D(center));
	spiralEffect->SetParticleRotation(ESPInterval(0.0f, 359.999999f));
	spiralEffect->SetParticleSize(ESPInterval(LevelPiece::PIECE_HEIGHT));
	spiralEffect->SetParticleColour(ESPInterval(1.2f*portalColour.R()), ESPInterval(1.2f*portalColour.G()), ESPInterval(1.2f*portalColour.B()), ESPInterval(1));
	spiralEffect->AddEffector(&this->particleFader);
	spiralEffect->AddEffector(&this->particleLargeGrowth);

	if (isSibling) {
		spiralEffect->AddEffector(&this->smokeRotatorCCW);
	}
	else {
		spiralEffect->AddEffector(&this->smokeRotatorCW);
	}

	bool result = spiralEffect->SetParticles(1, spiralTex);
	assert(result);
	if (!result) {
		delete spiralEffect;
		spiralEffect = NULL;
	}
	return spiralEffect;
}

// Updates any required values that need to be refreshed every frame
void GameESPAssets::UpdateBGTexture(const Texture2D& bgTexture) {
	this->normalTexRefractEffect.SetFBOTexture(&bgTexture);
    this->vapourTrailRefractEffect.SetFBOTexture(&bgTexture);
}

/**
 * Builds a shockwave that distorts the background.
 *
 */
ESPPointEmitter* GameESPAssets::CreateShockwaveEffect(const Point3D& center, float startSize, float lifeTime) {
	// Add the shockwave for the ball hitting the lightning...
	assert(lifeTime > 0);

	ESPPointEmitter* shockwaveEffect = new ESPPointEmitter();
	shockwaveEffect->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	shockwaveEffect->SetInitialSpd(ESPInterval(0.0f, 0.0f));
	shockwaveEffect->SetParticleLife(ESPInterval(lifeTime));
	shockwaveEffect->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	shockwaveEffect->SetParticleAlignment(ESP::ScreenAligned);
	shockwaveEffect->SetEmitPosition(center);
	shockwaveEffect->SetParticleSize(ESPInterval(startSize));
	shockwaveEffect->SetParticleColour(ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	shockwaveEffect->AddEffector(&this->particleFader);
	shockwaveEffect->AddEffector(&this->particleSuperGrowth);
	bool result = shockwaveEffect->SetParticles(1, &this->normalTexRefractEffect);
    UNUSED_VARIABLE(result);
	assert(result);

	return shockwaveEffect;
}

/**
 * Add an effect to bring the user's attention to the fact that something just went though
 * a portal block and got teleported to its sibling.
 */
void GameESPAssets::AddPortalTeleportEffect(const Point2D& enterPt, const PortalBlock& block) {
	ESPPointEmitter* enterEffect = this->CreateTeleportEffect(enterPt, block, false);
	if (enterEffect == NULL) {
		return;
	}

	Vector2D toBallFromBlock = enterPt - block.GetCenter();
	ESPPointEmitter* exitEffect = this->CreateTeleportEffect(block.GetSiblingPortal()->GetCenter() + toBallFromBlock, *block.GetSiblingPortal(), true);
	if (exitEffect == NULL) {
		delete enterEffect;
		return;
	}

	this->activeGeneralEmitters.push_back(enterEffect);
	this->activeGeneralEmitters.push_back(exitEffect);
}

/**
 * Add an effect to the end of the cannon barrel for when it fires a ball out.
 */
void GameESPAssets::AddCannonFireEffect(const Point3D& endOfCannonPt, const Vector2D& fireDir) {
	// Don't bother if the ball camera is on...
	if (GameBall::GetIsBallCameraOn()) {
		return;
	}

	Vector3D emitDir(fireDir);
	bool result = true;

	// Basic bang
	ESPPointEmitter* cannonBlast = new ESPPointEmitter();
	cannonBlast->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	cannonBlast->SetInitialSpd(ESPInterval(0.001f));
	cannonBlast->SetParticleLife(ESPInterval(0.85f));
	cannonBlast->SetParticleSize(ESPInterval(1.8 * CannonBlock::CANNON_BARREL_HEIGHT), ESPInterval(2.7 * CannonBlock::CANNON_BARREL_HEIGHT));
	cannonBlast->SetEmitAngleInDegrees(0);
	cannonBlast->SetAsPointSpriteEmitter(false);
	cannonBlast->SetParticleAlignment(ESP::ScreenAlignedFollowVelocity);
	cannonBlast->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	cannonBlast->SetEmitPosition(endOfCannonPt + 1.5 * CannonBlock::CANNON_BARREL_HEIGHT * emitDir);
	cannonBlast->SetEmitDirection(emitDir);
	cannonBlast->SetToggleEmitOnPlane(true, Vector3D(0, 0, 1));
	cannonBlast->AddEffector(&this->particleFader);
	result = cannonBlast->SetParticles(1, this->sideBlastTex);
	assert(result);

	// Bits of stuff
	ESPPointEmitter* debrisBits = new ESPPointEmitter();
	debrisBits->SetNumParticleLives(1);
	debrisBits->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	debrisBits->SetInitialSpd(ESPInterval(3.5f, 7.0f));
	debrisBits->SetParticleLife(ESPInterval(1.0f, 1.75f));
	debrisBits->SetParticleSize(ESPInterval(0.4f * CannonBlock::CANNON_BARREL_HEIGHT, CannonBlock::CANNON_BARREL_HEIGHT));
	debrisBits->SetEmitAngleInDegrees(50);
	debrisBits->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	debrisBits->SetAsPointSpriteEmitter(true);
	debrisBits->SetEmitPosition(endOfCannonPt);
	debrisBits->SetEmitDirection(emitDir);
	debrisBits->AddEffector(&this->particleFireColourFader);
	result = debrisBits->SetParticles(15, this->starTex);
	assert(result);	
	
	// Create an emitter for the sound of onomatopeia of the cannon firing
	ESPPointEmitter* shotOnoEffect = new ESPPointEmitter();
	// Set up the emitter...
	shotOnoEffect->SetSpawnDelta(ESPInterval(-1, -1));
	shotOnoEffect->SetInitialSpd(ESPInterval(1.0f, 2.0f));
	shotOnoEffect->SetParticleLife(ESPInterval(0.5f, 0.8f));
	shotOnoEffect->SetParticleSize(ESPInterval(0.7f, 1.0f), ESPInterval(1.0f, 1.0f));
	shotOnoEffect->SetParticleRotation(ESPInterval(-20.0f, 20.0f));
	shotOnoEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.0f));
	shotOnoEffect->SetParticleAlignment(ESP::ScreenAligned);
	shotOnoEffect->SetEmitPosition(endOfCannonPt + Vector3D(0, -LevelPiece::PIECE_HEIGHT, 0));
	shotOnoEffect->SetEmitDirection(emitDir);
	
	// Add effectors...
	shotOnoEffect->AddEffector(&this->particleFader);
	shotOnoEffect->AddEffector(&this->particleSmallGrowth);

	// Add the single text particle to the emitter with the severity of the effect...
	TextLabel2D fireTextLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Small), "");
	fireTextLabel.SetColour(Colour(1, 1, 1));
	fireTextLabel.SetDropShadow(Colour(0, 0, 0), 0.1f);
	result = shotOnoEffect->SetParticles(1, fireTextLabel, Onomatoplex::SHOT, Onomatoplex::GOOD);
	assert(result);

	// Lastly, add the cannon emitters to the list of active emitters in order of back to front
	this->activeGeneralEmitters.push_back(debrisBits);
	this->activeGeneralEmitters.push_back(cannonBlast);
	this->activeGeneralEmitters.push_back(shotOnoEffect);
}

/**
 * Add the typical block break effect - visible when you destroy a typical block.
 */
void GameESPAssets::AddBasicBlockBreakEffect(const LevelPiece& block) {
	assert(this->bangTextures.size() != 0);

	// Choose a random bang texture
	unsigned int randomBangTexIndex = Randomizer::GetInstance()->RandomUnsignedInt() % this->bangTextures.size();
	Texture2D* randomBangTex = this->bangTextures[randomBangTexIndex];
	
	ESPInterval bangLifeInterval		= ESPInterval(0.8f, 1.1f);
	ESPInterval bangOnoLifeInterval	= ESPInterval(bangLifeInterval.minValue + 0.3f, bangLifeInterval.maxValue + 0.3f);
	const Point2D& blockCenter = block.GetCenter();
	Point3D emitCenter  = Point3D(blockCenter[0], blockCenter[1], 0.0f);

	// Create an emitter for the bang texture
	ESPPointEmitter* bangEffect = new ESPPointEmitter();
	
	// Set up the emitter...
	bangEffect->SetSpawnDelta(ESPInterval(-1, -1));
	bangEffect->SetInitialSpd(ESPInterval(0.0f, 0.0f));
	bangEffect->SetParticleLife(bangLifeInterval);
	bangEffect->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	bangEffect->SetParticleAlignment(ESP::ScreenAligned);
	bangEffect->SetEmitPosition(emitCenter);

	// Figure out some random proper orientation...
	// Two base rotations (for variety) : 180 or 0...
	float baseBangRotation = 0.0f;
	if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
		baseBangRotation = 180.0f;
	}
	bangEffect->SetParticleRotation(ESPInterval(baseBangRotation - 10.0f, baseBangRotation + 10.0f));

	ESPInterval sizeIntervalX(3.5f, 3.8f);
	ESPInterval sizeIntervalY(1.9f, 2.2f);
	bangEffect->SetParticleSize(sizeIntervalX, sizeIntervalY);

	// Add effectors to the bang effect
	bangEffect->AddEffector(&this->particleFader);
	bangEffect->AddEffector(&this->particleMediumGrowth);
	
	// Add the bang particle...
	bool result = bangEffect->SetParticles(1, randomBangTex);
	assert(result);
	if (!result) {
		delete bangEffect;
		return;
	}

	// Create an emitter for the sound of onomatopeia of the breaking block
	ESPPointEmitter* bangOnoEffect = new ESPPointEmitter();
	// Set up the emitter...
	bangOnoEffect->SetSpawnDelta(ESPInterval(-1, -1));
	bangOnoEffect->SetInitialSpd(ESPInterval(0.0f, 0.0f));
	bangOnoEffect->SetParticleLife(bangOnoLifeInterval);
	bangOnoEffect->SetParticleSize(ESPInterval(0.7f, 1.0f), ESPInterval(1.0f, 1.0f));
	bangOnoEffect->SetParticleRotation(ESPInterval(-20.0f, 20.0f));
	bangOnoEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.2f));
	bangOnoEffect->SetParticleAlignment(ESP::ScreenAligned);
	bangOnoEffect->SetEmitPosition(emitCenter);
	
	// Add effectors...
	bangOnoEffect->AddEffector(&this->particleFader);
	bangOnoEffect->AddEffector(&this->particleSmallGrowth);

	// Add the single text particle to the emitter with the severity of the effect...
	TextLabel2D bangTextLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium), "");
	bangTextLabel.SetColour(Colour(1, 1, 1));
	bangTextLabel.SetDropShadow(Colour(0, 0, 0), 0.1f);

	Onomatoplex::Extremeness severity = Onomatoplex::NORMAL;
	Onomatoplex::SoundType type = Onomatoplex::EXPLOSION;

	// TODO: make the severity more dynamic!!
	switch (block.GetType()) {

		case LevelPiece::Solid:
		case LevelPiece::SolidTriangle:
			severity = Onomatoplex::AWESOME;
			break;

		case LevelPiece::Prism:
		case LevelPiece::PrismTriangle:
			type = Onomatoplex::SHATTER;
			severity = Onomatoplex::AWESOME;
			this->activeGeneralEmitters.push_back(this->CreateBlockBreakSmashyBits(emitCenter, ESPInterval(0.0f), 
																						ESPInterval(0.1f, 1.0f), ESPInterval(0.5f, 1.0f)));
			break;

		case LevelPiece::ItemDrop:
		case LevelPiece::Collateral:
        case LevelPiece::OneWay:
        case LevelPiece::NoEntry:
        case LevelPiece::LaserTurret:
        case LevelPiece::RocketTurret:
        case LevelPiece::MineTurret:
			severity = Onomatoplex::SUPER_AWESOME;
			this->activeGeneralEmitters.push_back(this->CreateBlockBreakSmashyBits(emitCenter, ESPInterval(0.6f, 1.0f), 
																						ESPInterval(0.5f, 1.0f), ESPInterval(0.0f, 0.0f), false, 20));
			break;

		case LevelPiece::Cannon:
			severity = Onomatoplex::AWESOME;
			this->activeGeneralEmitters.push_back(this->CreateBlockBreakSmashyBits(emitCenter, ESPInterval(0.8f, 1.0f), 
																						ESPInterval(0.8f, 1.0f), ESPInterval(0.0f, 0.0f)));
			break;

		default:
			break;
	}
	bangOnoEffect->SetParticles(1, bangTextLabel, type, severity);

	// Lastly, add the new emitters to the list of active emitters in order of back to front
	this->activeGeneralEmitters.push_back(bangEffect);
	this->activeGeneralEmitters.push_back(bangOnoEffect);
}

ESPPointEmitter* GameESPAssets::CreateBlockBreakSmashyBits(const Point3D& center, const ESPInterval& r, 
																													 const ESPInterval& g, const ESPInterval& b, bool gravity, 
																													 size_t numParticles) {
		ESPPointEmitter* smashyBits = new ESPPointEmitter();
		smashyBits->SetNumParticleLives(1);
		smashyBits->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
		smashyBits->SetInitialSpd(ESPInterval(3.5f, 5.5f));
		smashyBits->SetParticleLife(ESPInterval(1.25f, 2.0f));
		smashyBits->SetParticleSize(ESPInterval(0.4f * LevelPiece::PIECE_HEIGHT, LevelPiece::PIECE_HEIGHT));
		smashyBits->SetEmitAngleInDegrees(180);
		smashyBits->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
		smashyBits->SetAsPointSpriteEmitter(true);
		smashyBits->SetEmitPosition(center);
		smashyBits->SetEmitDirection(Vector3D(0,1,0));
		smashyBits->SetToggleEmitOnPlane(true);
		smashyBits->SetParticleColour(r, g, b, ESPInterval(1.0f));
		smashyBits->AddEffector(&this->particleFader);
		if (gravity) {
			smashyBits->AddEffector(&this->gravity);
		}
		smashyBits->SetParticles(numParticles, this->starTex);

		return smashyBits;
}

/**
 * Add the effect for when something hits the active ball safety barrier/net and causes
 * it to be destroyed.
 */
void GameESPAssets::AddBallSafetyNetDestroyedEffect(const Point2D& pos) {
	assert(this->bangTextures.size() != 0);

	// Choose a random bang texture
	unsigned int randomBangTexIndex = Randomizer::GetInstance()->RandomUnsignedInt() % this->bangTextures.size();
	Texture2D* randomBangTex = this->bangTextures[randomBangTexIndex];
	
	ESPInterval bangLifeInterval		= ESPInterval(1.0f, 1.25f);
	ESPInterval bangOnoLifeInterval	= ESPInterval(bangLifeInterval.minValue + 0.3f, bangLifeInterval.maxValue + 0.3f);
	
	Point3D emitCenter(pos);

	// Create an emitter for the bang texture
	ESPPointEmitter* bangEffect = new ESPPointEmitter();
	
	// Set up the emitter...
	bangEffect->SetSpawnDelta(ESPInterval(-1, -1));
	bangEffect->SetInitialSpd(ESPInterval(0.0f, 0.0f));
	bangEffect->SetParticleLife(bangLifeInterval);
	bangEffect->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	bangEffect->SetParticleAlignment(ESP::ScreenAligned);
	bangEffect->SetEmitPosition(emitCenter);

	// Figure out some random proper orientation...
	// Two base rotations (for variety) : 180 or 0...
	float baseBangRotation = 0.0f;
	if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
		baseBangRotation = 180.0f;
	}
	bangEffect->SetParticleRotation(ESPInterval(baseBangRotation - 10.0f, baseBangRotation + 10.0f));

	ESPInterval sizeIntervalX(2.5f, 3.2f);
	ESPInterval sizeIntervalY(1.25f, 1.65f);
	bangEffect->SetParticleSize(sizeIntervalX, sizeIntervalY);

	// Add effectors to the bang effect
	bangEffect->AddEffector(&this->particleFader);
	bangEffect->AddEffector(&this->particleMediumGrowth);
	
	// Add the bang particle...
	bool result = bangEffect->SetParticles(1, randomBangTex);
	assert(result);
	if (!result) {
		delete bangEffect;
		return;
	}

	// Create an emitter for the sound of onomatopeia of the breaking block
	ESPPointEmitter* bangOnoEffect = new ESPPointEmitter();
	// Set up the emitter...
	bangOnoEffect->SetSpawnDelta(ESPInterval(-1, -1));
	bangOnoEffect->SetInitialSpd(ESPInterval(0.0f, 0.0f));
	bangOnoEffect->SetParticleLife(bangOnoLifeInterval);
	bangOnoEffect->SetParticleSize(ESPInterval(0.7f, 1.0f), ESPInterval(1.0f, 1.0f));
	bangOnoEffect->SetParticleRotation(ESPInterval(-20.0f, 20.0f));
	bangOnoEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.2f));
	bangOnoEffect->SetParticleAlignment(ESP::ScreenAligned);
	bangOnoEffect->SetEmitPosition(emitCenter);
	
	// Add effectors...
	bangOnoEffect->AddEffector(&this->particleFader);
	bangOnoEffect->AddEffector(&this->particleSmallGrowth);

	// Add the single particle to the emitter...
	DropShadow dpTemp;
	dpTemp.amountPercentage = 0.10f;
	ESPOnomataParticle* bangOnoParticle = new ESPOnomataParticle(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium));
	bangOnoParticle->SetDropShadow(dpTemp);

	// Set the severity of the effect...
	bangOnoParticle->SetOnomatoplexSound(Onomatoplex::EXPLOSION, Onomatoplex::GOOD);
	bangOnoEffect->AddParticle(bangOnoParticle);

	// Lastly, add the new emitters to the list of active emitters in order of back to front
	this->activeGeneralEmitters.push_front(bangEffect);
	this->activeGeneralEmitters.push_back(bangOnoEffect);
}

/**
 * Add the effect for when the ball explodes (usually due to its death by falling
 * off the level).
 */
void GameESPAssets::AddBallExplodedEffect(const GameBall* ball) {
	assert(ball != NULL);

	float ballRadius = ball->GetBounds().Radius();
	float smallSize  = 2 * ballRadius;
	float bigSize    = 2 * smallSize;
	Point2D ballCenter = ball->GetBounds().Center();
	Point3D emitCenter(ballCenter[0], ballCenter[1], 0.0f);

	// When a ball explodes there's a big bang star, with loud explosive onomatopiea
	// and lots of smoke and stars

	// Smokey firey clouds
	ESPInterval smokeColour(0.3f, 0.8f);
	for (int i = 0; i < 3; i++) {
		size_t randomTexIndex = Randomizer::GetInstance()->RandomUnsignedInt() % this->smokeTextures.size();

		ESPPointEmitter* smokeClouds = new ESPPointEmitter();
		smokeClouds->SetSpawnDelta(ESPInterval(0.0f));
		smokeClouds->SetNumParticleLives(1);
		smokeClouds->SetInitialSpd(ESPInterval(2.0f, 8.0f));
		smokeClouds->SetParticleLife(ESPInterval(0.5f, 2.0f));
		smokeClouds->SetParticleSize(ESPInterval(smallSize, bigSize));
		smokeClouds->SetRadiusDeviationFromCenter(ESPInterval(ballRadius));
		smokeClouds->SetParticleAlignment(ESP::ScreenAligned);
		smokeClouds->SetEmitPosition(emitCenter);
		smokeClouds->SetEmitAngleInDegrees(180);
		smokeClouds->SetParticles(GameESPAssets::NUM_EXPLOSION_SMOKE_PART_PARTICLES, this->smokeTextures[randomTexIndex]);
		smokeClouds->AddEffector(&this->particleLargeGrowth);
		smokeClouds->AddEffector(&this->particleFireColourFader);
		
		if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
			smokeClouds->AddEffector(&this->smokeRotatorCW);
		}
		else {
			smokeClouds->AddEffector(&this->smokeRotatorCCW);
		}
		this->activeGeneralEmitters.push_back(smokeClouds);
	}

	// Bang star
	unsigned int randomBangTexIndex = Randomizer::GetInstance()->RandomUnsignedInt() % this->bangTextures.size();
	Texture2D* randomBangTex = this->bangTextures[randomBangTexIndex];

	ESPPointEmitter* bangEffect = new ESPPointEmitter();
	bangEffect->SetSpawnDelta(ESPInterval(-1));
	bangEffect->SetInitialSpd(ESPInterval(0.0f, 0.0f));
	bangEffect->SetParticleLife(ESPInterval(1.5f));
	bangEffect->SetRadiusDeviationFromCenter(ESPInterval(0));
	bangEffect->SetParticleAlignment(ESP::ScreenAligned);
	bangEffect->SetEmitPosition(emitCenter);
	// Figure out some random proper orientation...
	// Two base rotations (for variety) : 180 or 0...
	float baseBangRotation = 0.0f;
	if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
		baseBangRotation = 180.0f;
	}
	bangEffect->SetParticleRotation(ESPInterval(baseBangRotation - 10.0f, baseBangRotation + 10.0f));
	bangEffect->SetParticleSize(ESPInterval(3 * bigSize), ESPInterval(2 * bigSize));
	bangEffect->AddEffector(&this->particleFader);
	bangEffect->AddEffector(&this->particleMediumGrowth);
	bangEffect->SetParticles(1, randomBangTex);
	
	this->activeGeneralEmitters.push_back(bangEffect);

	// Onomatopoeia for the explosion
	ESPPointEmitter* explodeOnoEffect = new ESPPointEmitter();
	// Set up the emitter...
	explodeOnoEffect->SetSpawnDelta(ESPInterval(-1));
	explodeOnoEffect->SetInitialSpd(ESPInterval(0.0f, 0.0f));
	explodeOnoEffect->SetParticleLife(ESPInterval(2.5f));
	explodeOnoEffect->SetParticleSize(ESPInterval(1.0f), ESPInterval(1.0f));
	explodeOnoEffect->SetParticleRotation(ESPInterval(-15.0f, 15.0f));
	explodeOnoEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	explodeOnoEffect->SetParticleAlignment(ESP::ScreenAligned);
	explodeOnoEffect->SetEmitPosition(emitCenter);
	explodeOnoEffect->SetParticleColour(ESPInterval(1), ESPInterval(1), ESPInterval(1), ESPInterval(1));
	
	// Add effectors...
	explodeOnoEffect->AddEffector(&this->particleFader);
	explodeOnoEffect->AddEffector(&this->particleMediumGrowth);

	// Add the single particle to the emitter...
	DropShadow dpTemp;
	dpTemp.colour = Colour(0,0,0);
	dpTemp.amountPercentage = 0.15f;
	ESPOnomataParticle* explodeOnoParticle = new ESPOnomataParticle(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Big));
	explodeOnoParticle->SetDropShadow(dpTemp);
	explodeOnoParticle->SetOnomatoplexSound(Onomatoplex::EXPLOSION, Onomatoplex::UBER);
	explodeOnoEffect->AddParticle(explodeOnoParticle);

	this->activeGeneralEmitters.push_back(explodeOnoEffect);
}

/**
 * Add the effect for when a bomb block is hit - explodey!
 */
void GameESPAssets::AddBombBlockBreakEffect(const LevelPiece& bomb) {
	Point2D bombCenter  = bomb.GetCenter();
	Point3D emitCenter  = Point3D(bombCenter[0], bombCenter[1], 0.0f);

	// Explosion rays
	ESPPointEmitter* bombExplodeRayEffect = new ESPPointEmitter();
	bombExplodeRayEffect->SetSpawnDelta(ESPInterval(-1.0f));
	bombExplodeRayEffect->SetInitialSpd(ESPInterval(0.0f));
	bombExplodeRayEffect->SetParticleLife(ESPInterval(2.0f));
	bombExplodeRayEffect->SetParticleSize(ESPInterval(5.0f));
	bombExplodeRayEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	bombExplodeRayEffect->SetParticleAlignment(ESP::ScreenAligned);
	bombExplodeRayEffect->SetEmitPosition(emitCenter);
	bombExplodeRayEffect->SetParticles(1, this->explosionRayTex);
	if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
		bombExplodeRayEffect->AddEffector(&this->explosionRayRotatorCW);
	}
	else {
		bombExplodeRayEffect->AddEffector(&this->explosionRayRotatorCCW);
	}
	bombExplodeRayEffect->AddEffector(&this->particleLargeGrowth);
	bombExplodeRayEffect->AddEffector(&this->particleFader);

	// Create the explosion cloud and fireyness for the bomb
	/*
	// TODO: Use this later for the fireball effect...
	ESPPointEmitter* bombExplodeFireEffect1 = new ESPPointEmitter();
	bombExplodeFireEffect1->SetSpawnDelta(ESPInterval(0.005f));
	bombExplodeFireEffect1->SetNumParticleLives(1);
	bombExplodeFireEffect1->SetInitialSpd(ESPInterval(3.0f, 3.5f));
	bombExplodeFireEffect1->SetParticleLife(ESPInterval(0.25f, 4.0f));
	bombExplodeFireEffect1->SetParticleSize(ESPInterval(1.0f, 3.0f));
	bombExplodeFireEffect1->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	bombExplodeFireEffect1->SetParticleAlignment(ESP::ScreenAligned);
	bombExplodeFireEffect1->SetEmitPosition(emitCenter);
	bombExplodeFireEffect1->SetEmitAngleInDegrees(180);
	bombExplodeFireEffect1->SetParticles(GameESPAssets::NUM_EXPLOSION_FIRE_SHADER_PARTICLES, &this->fireEffect);
	bombExplodeFireEffect1->AddEffector(&this->particleFireColourFader);
	bombExplodeFireEffect1->AddEffector(&this->particleLargeGrowth);
	*/

	ESPPointEmitter* bombExplodeFireEffect2 = new ESPPointEmitter();
	bombExplodeFireEffect2->SetSpawnDelta(ESPInterval(0.005f));
	bombExplodeFireEffect2->SetNumParticleLives(1);
	bombExplodeFireEffect2->SetInitialSpd(ESPInterval(3.0f, 3.3f));
	bombExplodeFireEffect2->SetParticleLife(ESPInterval(0.5f, 4.0f));
	bombExplodeFireEffect2->SetParticleSize(ESPInterval(1.0f, 3.0f));
	bombExplodeFireEffect2->SetRadiusDeviationFromCenter(ESPInterval(1.0f));
	bombExplodeFireEffect2->SetParticleAlignment(ESP::ScreenAligned);
	bombExplodeFireEffect2->SetEmitPosition(emitCenter);
	bombExplodeFireEffect2->SetEmitAngleInDegrees(180);
	bombExplodeFireEffect2->SetParticles(GameESPAssets::NUM_EXPLOSION_FIRE_CLOUD_PARTICLES, this->explosionTex);
	bombExplodeFireEffect2->AddEffector(&this->particleFireColourFader);
	bombExplodeFireEffect2->AddEffector(&this->particleLargeGrowth);

	// Create an emitter for the sound of onomatopeia of the exploding bomb
	ESPPointEmitter* bombOnoEffect = new ESPPointEmitter();
	// Set up the emitter...
	bombOnoEffect->SetSpawnDelta(ESPInterval(-1, -1));
	bombOnoEffect->SetInitialSpd(ESPInterval(0.0f, 0.0f));
	bombOnoEffect->SetParticleLife(ESPInterval(2.0f, 2.5f));
	bombOnoEffect->SetParticleSize(ESPInterval(1.0f, 1.0f), ESPInterval(1.0f, 1.0f));
	bombOnoEffect->SetParticleRotation(ESPInterval(-20.0f, 20.0f));
	bombOnoEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.2f));
	bombOnoEffect->SetParticleAlignment(ESP::ScreenAligned);
	bombOnoEffect->SetEmitPosition(emitCenter);
	bombOnoEffect->SetParticleColour(ESPInterval(0), ESPInterval(0), ESPInterval(0), ESPInterval(1));
	
	// Add effectors...
	bombOnoEffect->AddEffector(&this->particleFader);
	bombOnoEffect->AddEffector(&this->particleSmallGrowth);

	// Add the single particle to the emitter...
	DropShadow dpTemp;
	dpTemp.colour = Colour(1,1,1);
	dpTemp.amountPercentage = 0.10f;
	ESPOnomataParticle* bombOnoParticle = new ESPOnomataParticle(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Big));
	bombOnoParticle->SetDropShadow(dpTemp);

	// Set the severity of the effect...
	Onomatoplex::Extremeness severity = Onomatoplex::SUPER_AWESOME;
	if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
		severity = Onomatoplex::UBER;
	}
	bombOnoParticle->SetOnomatoplexSound(Onomatoplex::EXPLOSION, severity);
	bombOnoEffect->AddParticle(bombOnoParticle);

	// Shockwave...
	ESPPointEmitter* shockwave = this->CreateShockwaveEffect(emitCenter, LevelPiece::PIECE_WIDTH, 0.8f);
	assert(shockwave != NULL);

	// Lastly, add the new emitters to the list of active emitters
	this->activeGeneralEmitters.push_front(shockwave);
	this->activeGeneralEmitters.push_front(bombExplodeFireEffect2);
	this->activeGeneralEmitters.push_front(bombExplodeRayEffect);
	this->activeGeneralEmitters.push_back(bombOnoEffect);
}

/**
 * Add the effect for when an Ink block gets hit - splortch!
 */
void GameESPAssets::AddInkBlockBreakEffect(const Camera& camera, const LevelPiece& inkBlock, const GameLevel& level, bool shootSpray) {
	Point2D inkBlockCenter  = inkBlock.GetCenter();
	Point3D emitCenter  = Point3D(inkBlockCenter[0], inkBlockCenter[1], 0.0f);
	
	const Colour& inkBlockColour	= GameViewConstants::GetInstance()->INK_BLOCK_COLOUR;
	Colour lightInkBlockColour		= inkBlockColour + Colour(0.15f, 0.15f, 0.15f);
	Colour lighterInkBlockColour	= lightInkBlockColour + Colour(0.2f, 0.2f, 0.2f);

	unsigned int randomSmoke1 = Randomizer::GetInstance()->RandomUnsignedInt() % this->smokeTextures.size();
	unsigned int randomSmoke2 = Randomizer::GetInstance()->RandomUnsignedInt() % this->smokeTextures.size();
	unsigned int randomSmoke3 = Randomizer::GetInstance()->RandomUnsignedInt() % this->smokeTextures.size();

	// Inky clouds
	ESPPointEmitter* inkyClouds1 = new ESPPointEmitter();
	inkyClouds1->SetSpawnDelta(ESPInterval(0.0f));
	inkyClouds1->SetNumParticleLives(1);
	inkyClouds1->SetInitialSpd(ESPInterval(2.0f, 2.5f));
	inkyClouds1->SetParticleLife(ESPInterval(3.0f, 4.0f));
	inkyClouds1->SetParticleSize(ESPInterval(1.0f, 3.0f));
	inkyClouds1->SetRadiusDeviationFromCenter(ESPInterval(0.1f));
	inkyClouds1->SetParticleAlignment(ESP::ScreenAligned);
	inkyClouds1->SetEmitPosition(emitCenter);
	inkyClouds1->SetEmitAngleInDegrees(180);
	inkyClouds1->SetParticleColour(ESPInterval(inkBlockColour.R()), ESPInterval(inkBlockColour.G()), ESPInterval(inkBlockColour.B()), ESPInterval(1.0f));
	inkyClouds1->SetParticles(GameESPAssets::NUM_INK_CLOUD_PART_PARTICLES, this->smokeTextures[randomSmoke1]);
	inkyClouds1->AddEffector(&this->particleFader);
	inkyClouds1->AddEffector(&this->particleMediumGrowth);
	
	if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
		inkyClouds1->AddEffector(&this->smokeRotatorCW);
	}
	else {
		inkyClouds1->AddEffector(&this->smokeRotatorCCW);
	}

	ESPPointEmitter* inkyClouds2 = new ESPPointEmitter();
	inkyClouds2->SetSpawnDelta(ESPInterval(0.0f));
	inkyClouds2->SetNumParticleLives(1);
	inkyClouds2->SetInitialSpd(ESPInterval(2.0f, 2.5f));
	inkyClouds2->SetParticleLife(ESPInterval(3.0f, 4.0f));
	inkyClouds2->SetParticleSize(ESPInterval(1.0f, 3.0f));
	inkyClouds2->SetRadiusDeviationFromCenter(ESPInterval(0.1f));
	inkyClouds2->SetParticleAlignment(ESP::ScreenAligned);
	inkyClouds2->SetEmitPosition(emitCenter);
	inkyClouds2->SetEmitAngleInDegrees(180);
	inkyClouds2->SetParticleColour(ESPInterval(lightInkBlockColour.R()), ESPInterval(lightInkBlockColour.G()), ESPInterval(lightInkBlockColour.B()), ESPInterval(1.0f));
	inkyClouds2->SetParticles(GameESPAssets::NUM_INK_CLOUD_PART_PARTICLES, this->smokeTextures[randomSmoke2]);
	inkyClouds2->AddEffector(&this->particleFader);
	inkyClouds2->AddEffector(&this->particleMediumGrowth);
	
	if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
		inkyClouds2->AddEffector(&this->smokeRotatorCW);
	}
	else {
		inkyClouds2->AddEffector(&this->smokeRotatorCCW);
	}

	ESPPointEmitter* inkyClouds3 = new ESPPointEmitter();
	inkyClouds3->SetSpawnDelta(ESPInterval(0.0f));
	inkyClouds3->SetNumParticleLives(1);
	inkyClouds3->SetInitialSpd(ESPInterval(2.0f, 2.5f));
	inkyClouds3->SetParticleLife(ESPInterval(3.0f, 4.0f));
	inkyClouds3->SetParticleSize(ESPInterval(1.0f, 3.0f));
	inkyClouds3->SetRadiusDeviationFromCenter(ESPInterval(0.1f));
	inkyClouds3->SetParticleAlignment(ESP::ScreenAligned);
	inkyClouds3->SetEmitPosition(emitCenter);
	inkyClouds3->SetEmitAngleInDegrees(180);
	inkyClouds3->SetParticleColour(ESPInterval(lighterInkBlockColour.R()), ESPInterval(lighterInkBlockColour.G()), ESPInterval(lighterInkBlockColour.B()), ESPInterval(1.0f));
	inkyClouds3->SetParticles(GameESPAssets::NUM_INK_CLOUD_PART_PARTICLES, this->smokeTextures[randomSmoke3]);
	inkyClouds3->AddEffector(&this->particleFader);
	inkyClouds3->AddEffector(&this->particleMediumGrowth);
	
	if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
		inkyClouds3->AddEffector(&this->smokeRotatorCW);
	}
	else {
		inkyClouds3->AddEffector(&this->smokeRotatorCCW);
	}

	ESPPointEmitter* inkySpray = NULL;
	if (shootSpray) {
		Point3D currCamPos = camera.GetCurrentCameraPosition();
		Vector3D negHalfLevelDim = -0.5 * Vector3D(level.GetLevelUnitWidth(), level.GetLevelUnitHeight(), 0.0f);
		Point3D emitCenterWorldCoords = emitCenter + negHalfLevelDim;
		Vector3D sprayVec = currCamPos - emitCenterWorldCoords;
		
		
		Vector3D inkySprayDir = Vector3D::Normalize(sprayVec);
		float distToCamera = sprayVec.length();

		// Inky spray at the camera
		inkySpray = new ESPPointEmitter();
		inkySpray->SetSpawnDelta(ESPInterval(0.01f));
		inkySpray->SetNumParticleLives(1);
		inkySpray->SetInitialSpd(ESPInterval(distToCamera - 2.0f, distToCamera + 2.0f));
		inkySpray->SetParticleLife(ESPInterval(1.0f, 2.0f));
		inkySpray->SetParticleSize(ESPInterval(0.75f, 2.25f));
		inkySpray->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
		inkySpray->SetParticleAlignment(ESP::ScreenAligned);
		inkySpray->SetEmitPosition(emitCenter);
		inkySpray->SetEmitDirection(inkySprayDir);
		inkySpray->SetEmitAngleInDegrees(5);
		inkySpray->SetParticleColour(ESPInterval(inkBlockColour.R()), ESPInterval(inkBlockColour.G()), ESPInterval(inkBlockColour.B()), ESPInterval(1.0f));
		inkySpray->SetParticles(GameESPAssets::NUM_INK_SPRAY_PARTICLES, this->ballTex);
		inkySpray->AddEffector(&this->particleFader);
		inkySpray->AddEffector(&this->particleMediumGrowth);
	}

	// Create an emitter for the sound of onomatopeia of the bursting ink block
	ESPPointEmitter* inkOnoEffect = new ESPPointEmitter();
	// Set up the emitter...
	inkOnoEffect->SetSpawnDelta(ESPInterval(ESPPointEmitter::ONLY_SPAWN_ONCE));
	inkOnoEffect->SetInitialSpd(ESPInterval(0.0f, 0.0f));
	inkOnoEffect->SetParticleLife(ESPInterval(2.25f));
	inkOnoEffect->SetParticleSize(ESPInterval(1.0f, 1.0f), ESPInterval(1.0f, 1.0f));
	inkOnoEffect->SetParticleRotation(ESPInterval(-20.0f, 20.0f));
	inkOnoEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.2f));
	inkOnoEffect->SetParticleAlignment(ESP::ScreenAligned);
	inkOnoEffect->SetEmitPosition(emitCenter);
	inkOnoEffect->SetParticleColour(ESPInterval(1), ESPInterval(1), ESPInterval(1), ESPInterval(1));
	
	// Add effectors...
	inkOnoEffect->AddEffector(&this->particleFader);
	inkOnoEffect->AddEffector(&this->particleSmallGrowth);

	// Set the onomata particle
	TextLabel2D splatTextLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium), "");
	splatTextLabel.SetColour(Colour(1, 1, 1));
	splatTextLabel.SetDropShadow(Colour(0, 0, 0), 0.1f);
	Onomatoplex::Extremeness severity = Onomatoplex::Generator::GetInstance()->GetRandomExtremeness(Onomatoplex::GOOD, Onomatoplex::UBER);
	inkOnoEffect->SetParticles(1, splatTextLabel, Onomatoplex::GOO, severity);

	// Create the splat graphic for the ink block (analogous to the bang star for exploding blocks)
	assert(this->splatTextures.size() != 0);

	// Choose a random bang texture
	unsigned int randomSplatTexIndex = Randomizer::GetInstance()->RandomUnsignedInt() % this->splatTextures.size();
	Texture2D* randomSplatTex = this->splatTextures[randomSplatTexIndex];

	// Create an emitter for the splat
	ESPPointEmitter* splatEffect = new ESPPointEmitter();
	splatEffect->SetSpawnDelta(ESPInterval(ESPPointEmitter::ONLY_SPAWN_ONCE));
	splatEffect->SetInitialSpd(ESPInterval(0.0f));
	splatEffect->SetParticleLife(ESPInterval(1.75f));
	splatEffect->SetRadiusDeviationFromCenter(ESPInterval(0));
	splatEffect->SetParticleAlignment(ESP::ScreenAligned);
	splatEffect->SetEmitPosition(emitCenter);
	splatEffect->SetParticleColour(ESPInterval(inkBlockColour.R()), ESPInterval(inkBlockColour.G()), ESPInterval(inkBlockColour.B()), ESPInterval(1.0f));

	// Figure out some random proper orientation...
	// Two base rotations (for variety) : 180 or 0...
	float baseSplatRotation = 0.0f;
	if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
		baseSplatRotation = 180.0f;
	}
	splatEffect->SetParticleRotation(ESPInterval(baseSplatRotation - 10.0f, baseSplatRotation + 10.0f));
	splatEffect->SetParticleSize(ESPInterval(4.0f, 4.5f), ESPInterval(2.0f, 2.5f));

	// Add effectors to the bang effect
	splatEffect->AddEffector(&this->particleFader);
	splatEffect->AddEffector(&this->particleMediumGrowth);
	bool result = splatEffect->SetParticles(1, randomSplatTex);
	UNUSED_VARIABLE(result);
	assert(result);

	this->activeGeneralEmitters.push_back(inkyClouds1);
	this->activeGeneralEmitters.push_back(inkyClouds2);
	this->activeGeneralEmitters.push_back(inkyClouds3);
	if (shootSpray) {
		this->activeGeneralEmitters.push_back(inkySpray);
	}
	this->activeGeneralEmitters.push_back(splatEffect);
	this->activeGeneralEmitters.push_back(inkOnoEffect);
}

// Adds JUST the bits of snowflakey ice that come off the block when its frozen and gets hit
// by a ball...
void GameESPAssets::AddIceBitsBreakEffect(const LevelPiece& block) {
	const Colour& iceColour = GameModelConstants::GetInstance()->ICE_BALL_COLOUR;
	const Point2D& blockCenter = block.GetCenter();
	Point3D emitCenter(blockCenter, 0.0f);

	bool result = false;

	ESPPointEmitter* snowflakeBitsEffect = new ESPPointEmitter();
	snowflakeBitsEffect->SetSpawnDelta(ESPInterval(ESPPointEmitter::ONLY_SPAWN_ONCE));
	snowflakeBitsEffect->SetInitialSpd(ESPInterval(3.0f, 7.0f));
	snowflakeBitsEffect->SetParticleLife(ESPInterval(1.0f, 2.5f));
	snowflakeBitsEffect->SetEmitDirection(Vector3D(0, 1, 0));
	snowflakeBitsEffect->SetEmitAngleInDegrees(180);
	snowflakeBitsEffect->SetParticleSize(ESPInterval(LevelPiece::PIECE_WIDTH / 5.0f, LevelPiece::PIECE_WIDTH / 1.5f));
	snowflakeBitsEffect->SetParticleRotation(ESPInterval(-180.0f, 180.0f));
	snowflakeBitsEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f, LevelPiece::PIECE_WIDTH / 2.5f));
	snowflakeBitsEffect->SetEmitPosition(emitCenter);
	snowflakeBitsEffect->SetParticleAlignment(ESP::ScreenAligned);
	snowflakeBitsEffect->SetParticleColour(ESPInterval(iceColour.R(), 1.0f), ESPInterval(iceColour.G(), 1.0f),
																		     ESPInterval(iceColour.B(), 1.0f), ESPInterval(1.0f));
	snowflakeBitsEffect->AddEffector(&this->particleFader);
	if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
		snowflakeBitsEffect->AddEffector(&this->smokeRotatorCCW);
	}
	else {
		snowflakeBitsEffect->AddEffector(&this->smokeRotatorCW);
	}
	result = snowflakeBitsEffect->SetParticles(10, this->snowflakeTextures[Randomizer::GetInstance()->RandomUnsignedInt() % this->snowflakeTextures.size()]);
	assert(result);
	this->activeGeneralEmitters.push_back(snowflakeBitsEffect);
}

// When a block frozen in an ice cube gets smashed we use this effect instead of the typical block break effect
// for that particular block type
void GameESPAssets::AddIceCubeBlockBreakEffect(const LevelPiece& block, const Colour& colour) {
	const Colour& iceColour = GameModelConstants::GetInstance()->ICE_BALL_COLOUR;
	const Point2D& blockCenter = block.GetCenter();
	Point3D emitCenter(blockCenter, 0.0f);
	bool result = false;

	// Create the smashy block bits
	size_t randomIdx = Randomizer::GetInstance()->RandomUnsignedInt() % this->rockTextures.size();
	for (int i = 0; i < 3; i++) {
		ESPPointEmitter* smashBitsEffect = new ESPPointEmitter();
		smashBitsEffect->SetSpawnDelta(ESPInterval(ESPPointEmitter::ONLY_SPAWN_ONCE));
		smashBitsEffect->SetInitialSpd(ESPInterval(2.0f, 5.0f));
		smashBitsEffect->SetParticleLife(ESPInterval(1.0f, 2.5f));
		smashBitsEffect->SetEmitDirection(Vector3D(0, 1, 0));
		smashBitsEffect->SetEmitAngleInDegrees(180);
		smashBitsEffect->SetParticleSize(ESPInterval(LevelPiece::PIECE_WIDTH / 14.0f, LevelPiece::PIECE_WIDTH / 3.0f));
		smashBitsEffect->SetParticleRotation(ESPInterval(-180.0f, 180.0f));
		smashBitsEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f, LevelPiece::PIECE_WIDTH / 3.0f));
		smashBitsEffect->SetEmitPosition(emitCenter);
		smashBitsEffect->SetParticleAlignment(ESP::ScreenAligned);
		smashBitsEffect->SetParticleColour(ESPInterval(0.5f * colour.R(), 0.75f * colour.R()), ESPInterval(0.5f * colour.G(), 0.75f * colour.G()),
																			 ESPInterval(0.5f * colour.B(), 0.75f * colour.B()), ESPInterval(0.8f, 1.0f));
		smashBitsEffect->AddEffector(&this->gravity);
		smashBitsEffect->AddEffector(&this->particleFader);
		result = smashBitsEffect->SetParticles(2, this->rockTextures[(randomIdx + i) % this->rockTextures.size()]);
		assert(result);
		this->activeGeneralEmitters.push_back(smashBitsEffect);
	}

	// Create an emitter for a single large snowflake
	ESPPointEmitter* snowflakeBackingEffect = new ESPPointEmitter();
	snowflakeBackingEffect->SetSpawnDelta(ESPInterval(ESPPointEmitter::ONLY_SPAWN_ONCE));
	snowflakeBackingEffect->SetInitialSpd(ESPInterval(0.0f));
	snowflakeBackingEffect->SetParticleLife(ESPInterval(1.75f));
	snowflakeBackingEffect->SetRadiusDeviationFromCenter(ESPInterval(0));
	snowflakeBackingEffect->SetParticleAlignment(ESP::ScreenAligned);
	snowflakeBackingEffect->SetEmitPosition(emitCenter);
	snowflakeBackingEffect->SetParticleColour(ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	snowflakeBackingEffect->SetParticleRotation(ESPInterval(-180, 180));
	snowflakeBackingEffect->SetParticleSize(ESPInterval(1.5f * LevelPiece::PIECE_WIDTH));
	snowflakeBackingEffect->AddEffector(&this->particleFader);
	snowflakeBackingEffect->AddEffector(&this->particleMediumGrowth);
	result = snowflakeBackingEffect->SetParticles(1, this->snowflakeTextures[1]);
	assert(result);
	this->activeGeneralEmitters.push_back(snowflakeBackingEffect);

	// Create an emitter for the sound of onomatopeia of shattering block
	ESPPointEmitter* iceSmashOnoEffect = new ESPPointEmitter();
	// Set up the emitter...
	iceSmashOnoEffect->SetSpawnDelta(ESPInterval(ESPPointEmitter::ONLY_SPAWN_ONCE));
	iceSmashOnoEffect->SetInitialSpd(ESPInterval(0.0f, 0.0f));
	iceSmashOnoEffect->SetParticleLife(ESPInterval(2.25f));
	iceSmashOnoEffect->SetParticleSize(ESPInterval(1.0f, 1.0f), ESPInterval(1.0f, 1.0f));
	iceSmashOnoEffect->SetParticleRotation(ESPInterval(-20.0f, 20.0f));
	iceSmashOnoEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.2f));
	iceSmashOnoEffect->SetParticleAlignment(ESP::ScreenAligned);
	iceSmashOnoEffect->SetEmitPosition(emitCenter);
	iceSmashOnoEffect->SetParticleColour(ESPInterval(iceColour.R()), ESPInterval(iceColour.G()), 
																			 ESPInterval(iceColour.B()), ESPInterval(1));
	
	// Add effectors...
	iceSmashOnoEffect->AddEffector(&this->particleFader);
	iceSmashOnoEffect->AddEffector(&this->particleSmallGrowth);

	// Set the onomata particle
	TextLabel2D smashTextLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium), "");
	smashTextLabel.SetColour(iceColour);
	smashTextLabel.SetDropShadow(Colour(0, 0, 0), 0.15f);
	Onomatoplex::Extremeness severity = Onomatoplex::Generator::GetInstance()->GetRandomExtremeness(Onomatoplex::GOOD, Onomatoplex::UBER);
	result = iceSmashOnoEffect->SetParticles(1, smashTextLabel, Onomatoplex::SHATTER, severity);
	assert(result);

	this->activeGeneralEmitters.push_back(iceSmashOnoEffect);
}

// When a fire glob extinguishes this effect is invoked
void GameESPAssets::AddFireGlobDestroyedEffect(const Projectile& projectile) {

	ESPPointEmitter* fireDisperseEffect = new ESPPointEmitter();
    fireDisperseEffect->SetNumParticleLives(1);
	fireDisperseEffect->SetSpawnDelta(ESPInterval(0.01f, 0.015f));
	fireDisperseEffect->SetInitialSpd(ESPInterval(1.5f, 4.0f));
	fireDisperseEffect->SetParticleLife(ESPInterval(0.8f, 1.5f));
	fireDisperseEffect->SetParticleSize(ESPInterval(0.2f*projectile.GetWidth(), 0.5f*projectile.GetWidth()));
	fireDisperseEffect->SetEmitAngleInDegrees(180);
	fireDisperseEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    fireDisperseEffect->SetEmitPosition(Point3D(projectile.GetPosition(), 0));
    fireDisperseEffect->SetAsPointSpriteEmitter(true);
	fireDisperseEffect->AddEffector(&this->particleFireColourFader);
    fireDisperseEffect->AddEffector(&this->particleMediumGrowth);
    size_t randomTexIdx = Randomizer::GetInstance()->RandomUnsignedInt() % this->smokeTextures.size();
    bool result = fireDisperseEffect->SetParticles(8, this->smokeTextures[randomTexIdx]);
    UNUSED_VARIABLE(result);
	assert(result);

    this->activeGeneralEmitters.push_back(fireDisperseEffect);
}

/**
 * Adds the effect for a paddle hitting a wall.
 */
void GameESPAssets::AddPaddleHitWallEffect(const PlayerPaddle& paddle, const Point2D& hitLoc) {
	
	// Figure out which direction to shoot stuff...
	Vector2D dirOfWallHit2D = hitLoc - paddle.GetCenterPosition();
	Vector3D dirOfWallHit3D = Vector3D(dirOfWallHit2D[0], dirOfWallHit2D[1], 0.0f);
	Point3D hitPos3D = Point3D(hitLoc[0], hitLoc[1], 0.0f);

	// Onomatopeia for the hit...
	ESPPointEmitter* paddleWallOnoEffect = new ESPPointEmitter();
	paddleWallOnoEffect->SetSpawnDelta(ESPInterval(-1));
	paddleWallOnoEffect->SetInitialSpd(ESPInterval(1.0f, 1.75f));
	paddleWallOnoEffect->SetParticleLife(ESPInterval(1.5f, 2.0f));

	if (dirOfWallHit2D[0] < 0) {
		paddleWallOnoEffect->SetParticleRotation(ESPInterval(-45.0f, -15.0f));
	}
	else {
		paddleWallOnoEffect->SetParticleRotation(ESPInterval(15.0f, 45.0f));
	}

	paddleWallOnoEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	paddleWallOnoEffect->SetParticleAlignment(ESP::ScreenAligned);
	paddleWallOnoEffect->SetEmitPosition(hitPos3D);
	paddleWallOnoEffect->SetEmitDirection(dirOfWallHit3D);
	paddleWallOnoEffect->SetEmitAngleInDegrees(40);
	paddleWallOnoEffect->SetParticleColour(ESPInterval(0.5f, 1.0f), ESPInterval(0.5f, 1.0f), ESPInterval(0.5f, 1.0f), ESPInterval(1));

	paddleWallOnoEffect->AddEffector(&this->particleFader);
	paddleWallOnoEffect->AddEffector(&this->particleSmallGrowth);
	
	// Add the single particle to the emitter...
	DropShadow dpTemp;
	dpTemp.colour = Colour(0,0,0);
	dpTemp.amountPercentage = 0.10f;
	ESPOnomataParticle* paddleWallOnoParticle = new ESPOnomataParticle(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Small));
	paddleWallOnoParticle->SetDropShadow(dpTemp);

	// Set the severity of the effect...
	Onomatoplex::Extremeness severity;
	ESPInterval size;
	switch(paddle.GetPaddleSize()) {
		case PlayerPaddle::SmallestSize:
			severity = Onomatoplex::WEAK;
			size.maxValue = 0.75f;
			size.minValue = 0.6f;
			break;
		case PlayerPaddle::SmallerSize:
			severity = Onomatoplex::GOOD;
			size.maxValue = 1.0f;
			size.minValue = 0.75f;
			break;
		case PlayerPaddle::NormalSize:
			severity = Onomatoplex::AWESOME;
			size.maxValue = 1.2f;
			size.minValue = 0.75f;
			break;
		case PlayerPaddle::BiggerSize:
			severity = Onomatoplex::SUPER_AWESOME;
			size.maxValue = 1.5f;
			size.minValue = 1.0f;
			break;
		case PlayerPaddle::BiggestSize:
			severity = Onomatoplex::UBER;
			size.maxValue = 1.75f;
			size.minValue = 1.25f;
			break;
		default:
			assert(false);
			severity = Onomatoplex::GOOD;
			size.maxValue = 1.0f;
			size.minValue = 0.75f;
			break;
	}
	paddleWallOnoEffect->SetParticleSize(size);
	paddleWallOnoParticle->SetOnomatoplexSound(Onomatoplex::BOUNCE, severity);
	paddleWallOnoEffect->AddParticle(paddleWallOnoParticle);

	// Smashy star texture particle
	ESPPointEmitter* paddleWallStarEmitter = new ESPPointEmitter();
	paddleWallStarEmitter->SetSpawnDelta(ESPInterval(-1));
	paddleWallStarEmitter->SetInitialSpd(ESPInterval(1.5f, 3.0f));
	paddleWallStarEmitter->SetParticleLife(ESPInterval(1.2f, 2.0f));
	paddleWallStarEmitter->SetParticleSize(size);
	paddleWallStarEmitter->SetParticleColour(ESPInterval(1.0f), ESPInterval(0.5f, 1.0f), ESPInterval(0.0f, 0.0f), ESPInterval(1.0f));
	paddleWallStarEmitter->SetEmitAngleInDegrees(35);
	paddleWallStarEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.25f));
	paddleWallStarEmitter->SetParticleAlignment(ESP::ViewPlaneAligned);
	paddleWallStarEmitter->SetEmitDirection(dirOfWallHit3D);
	paddleWallStarEmitter->SetEmitPosition(hitPos3D);

	if (dirOfWallHit2D[0] < 0) {
		paddleWallStarEmitter->AddEffector(&this->explosionRayRotatorCCW);
	}
	else {
		paddleWallStarEmitter->AddEffector(&this->explosionRayRotatorCW);
	}

	paddleWallStarEmitter->AddEffector(&this->particleFader);
	paddleWallStarEmitter->SetParticles(3 + (Randomizer::GetInstance()->RandomUnsignedInt() % 3), this->starTex);
		
	this->activeGeneralEmitters.push_back(paddleWallStarEmitter);
	this->activeGeneralEmitters.push_back(paddleWallOnoEffect);
}

void GameESPAssets::AddBasicPaddleHitByProjectileEffect(const PlayerPaddle& paddle, const Projectile& projectile) {

	// Projectile type determines the severity and size
	ESPInterval size(1.0f);
	Onomatoplex::Extremeness severity = Onomatoplex::NORMAL;
	switch (projectile.GetType()) {
		case Projectile::CollateralBlockProjectile:
			severity = Onomatoplex::SUPER_AWESOME;
			size.minValue = 0.5f;
			size.maxValue = 0.9f;
			break;
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
			severity = Onomatoplex::GOOD;
			size.minValue = 0.35f;
			size.maxValue = 0.45f;
			break;
        case Projectile::BallLaserBulletProjectile:
            severity = Onomatoplex::NORMAL;
            size.minValue = 0.2f;
			size.maxValue = 0.3f;
            break;
		case Projectile::FireGlobProjectile: {
				const FireGlobProjectile* fireGlobProjectile = static_cast<const FireGlobProjectile*>(&projectile);
				switch (fireGlobProjectile->GetRelativeSize()) {
					case FireGlobProjectile::Small:
						severity = Onomatoplex::GOOD;
						size.minValue = 0.2f;
						size.maxValue = 0.4f;
						break;
					case FireGlobProjectile::Medium:
						severity = Onomatoplex::GOOD;
						size.minValue = 0.3f;
						size.maxValue = 0.5f;
						break;
					case FireGlobProjectile::Large:
						severity = Onomatoplex::AWESOME;
						size.minValue = 0.4f;
						size.maxValue = 0.75f;
						break;
					default:
						assert(false);
						break;
				}
			}
			break;
		default:
			assert(false);
			return;
	}

	Point3D hitPosition(paddle.GetCenterPosition());
	Vector3D emitDirection(paddle.GetUpVector());

	// Form the text for the sound particle...
	DropShadow dpTemp;
	dpTemp.colour = Colour(0,0,0);
	dpTemp.amountPercentage = 0.125f;
	ESPOnomataParticle* paddleOnoParticle = new ESPOnomataParticle(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::SadBadGoo, GameFontAssetsManager::Small));
	paddleOnoParticle->SetDropShadow(dpTemp);
	paddleOnoParticle->SetOnomatoplexSound(Onomatoplex::BADSAD, severity);

	// Onomatopeia for the hit...
	ESPPointEmitter* paddleOnoEffect = new ESPPointEmitter();
	paddleOnoEffect->SetSpawnDelta(ESPInterval(-1));
	paddleOnoEffect->SetInitialSpd(ESPInterval(1.0f, 1.75f));
	paddleOnoEffect->SetParticleLife(ESPInterval(2.5f));
	paddleOnoEffect->SetParticleRotation(ESPInterval(-45.0f, 45.0f));
	paddleOnoEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	paddleOnoEffect->SetParticleAlignment(ESP::ScreenAligned);
	paddleOnoEffect->SetEmitPosition(hitPosition);
	paddleOnoEffect->SetEmitDirection(emitDirection);
	paddleOnoEffect->SetEmitAngleInDegrees(10);
	paddleOnoEffect->SetParticleColour(ESPInterval(1.0f), ESPInterval(0.2f), ESPInterval(0.2f), ESPInterval(1));
	paddleOnoEffect->SetParticleSize(size);
	paddleOnoEffect->AddEffector(&this->particleFader);
	paddleOnoEffect->AddEffector(&this->particleSmallGrowth);
	paddleOnoEffect->AddParticle(paddleOnoParticle);

	this->activeGeneralEmitters.push_back(paddleOnoEffect);
	
	// Figure out the projectile's position projected onto the paddle's axis
	Vector2D vecToProjectile = projectile.GetPosition() - paddle.GetCenterPosition();
	Vector2D positivePaddleAxis = Vector2D::Rotate(paddle.GetZRotation(), PlayerPaddle::DEFAULT_PADDLE_RIGHT_VECTOR);
	float minHeightOfImpact = 2.0f * projectile.GetHeight();

	// Use a dot product to determine whether the projectile is on the positive side or not
	Point3D hitPositionRelativeToPaddleCenter = Point3D(0,0,0) + 
		Vector3D(Vector2D::Dot(vecToProjectile, positivePaddleAxis) * positivePaddleAxis + 
		minHeightOfImpact * paddle.GetHalfHeight() * paddle.GetUpVector());

	// Add firey impact at hit location...
	ESPPointEmitter* fireyImpactOnPaddle = new ESPPointEmitter();
	fireyImpactOnPaddle->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	fireyImpactOnPaddle->SetInitialSpd(ESPInterval(0.001f));
	fireyImpactOnPaddle->SetParticleLife(ESPInterval(0.85f));
	fireyImpactOnPaddle->SetParticleSize(ESPInterval(projectile.GetWidth(), 1.5f * projectile.GetWidth()), 
																			 ESPInterval(minHeightOfImpact, 3.0f * projectile.GetHeight()));
	fireyImpactOnPaddle->SetEmitAngleInDegrees(0);
	fireyImpactOnPaddle->SetAsPointSpriteEmitter(false);
	fireyImpactOnPaddle->SetParticleAlignment(ESP::ScreenAlignedFollowVelocity);
	fireyImpactOnPaddle->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	fireyImpactOnPaddle->SetEmitPosition(hitPositionRelativeToPaddleCenter);
	fireyImpactOnPaddle->SetEmitDirection(emitDirection);
	fireyImpactOnPaddle->SetToggleEmitOnPlane(true, Vector3D(0, 0, 1));
	fireyImpactOnPaddle->AddEffector(&this->particleFader);
	fireyImpactOnPaddle->SetParticles(1, this->sideBlastTex);

	this->activePaddleEmitters.push_back(fireyImpactOnPaddle);

	// Smashy star texture particle
	ESPPointEmitter* starEmitter = new ESPPointEmitter();
	starEmitter->SetSpawnDelta(ESPInterval(-1));
	starEmitter->SetInitialSpd(ESPInterval(3.0f, 5.5f));
	starEmitter->SetParticleLife(ESPInterval(1.2f, 2.0f));
	starEmitter->SetParticleSize(ESPInterval(paddle.GetHalfWidthTotal() * 0.2f, paddle.GetHalfWidthTotal() * 0.5f));
	starEmitter->SetParticleColour(ESPInterval(1.0f), ESPInterval(0.5f, 1.0f), ESPInterval(0.0f, 0.0f), ESPInterval(1.0f));
	starEmitter->SetEmitAngleInDegrees(35);
	starEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f, paddle.GetHalfWidthTotal() * 0.25f));
	starEmitter->SetParticleAlignment(ESP::ViewPlaneAligned);
	starEmitter->SetEmitDirection(Vector3D(-projectile.GetVelocityDirection()));
	starEmitter->SetEmitPosition(Point3D(paddle.GetCenterPosition()[0] + hitPositionRelativeToPaddleCenter[0],
																			 paddle.GetCenterPosition()[1] + hitPositionRelativeToPaddleCenter[1], 0.0f));

	if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
		starEmitter->AddEffector(&this->explosionRayRotatorCCW);
	}
	else {
		starEmitter->AddEffector(&this->explosionRayRotatorCW);
	}

	starEmitter->AddEffector(&this->particleFader);
	starEmitter->SetParticles(3 + (Randomizer::GetInstance()->RandomUnsignedInt() % 3), this->starTex);

	this->activeGeneralEmitters.push_back(starEmitter);
}

/**
 * Adds an effect for when the paddle is struck by a projectile.
 */
void GameESPAssets::AddPaddleHitByProjectileEffect(const PlayerPaddle& paddle, const Projectile& projectile) {
	// No effect if the paddle camera is on
	if (paddle.GetIsPaddleCameraOn()) {
		return;
	}

	switch (projectile.GetType()) {

		case Projectile::CollateralBlockProjectile:
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
		case Projectile::FireGlobProjectile:
			this->AddBasicPaddleHitByProjectileEffect(paddle, projectile);
			break;

		case Projectile::PaddleRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
            // NOTE: THIS IS TAKEN CARE OF BY THE RocketExplodedEvent
			break;

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
            // NOTE: THIS IS TAKEN CARE OF BY THE MineExplodedEvent
            break;

		default:
			assert(false);
			break;
	}
}

/**
 * Adds an effect for when an item is dropping and not yet acquired by the player.
 */
void GameESPAssets::AddItemDropEffect(const GameItem& item, bool showStars) {
	ESPInterval redRandomColour(0.1f, 1.0f);
	ESPInterval greenRandomColour(0.1f, 1.0f);
	ESPInterval blueRandomColour(0.1f, 1.0f);
	ESPInterval redColour(0), greenColour(0), blueColour(0);
	float itemAlpha = item.GetItemColour().A();
	ESPInterval alpha(itemAlpha*0.75f);

	// We choose a specific kind of sprite graphic based on whether we are dealing with a power-down or not
	// (evil stars for bad items!!)
	Texture2D* itemSpecificFillStarTex = this->starTex;
	Texture2D* itemSpecificOutlineStarTex = this->starOutlineTex;

	switch (item.GetItemDisposition()) {
		case GameItem::Good:
			greenRandomColour = ESPInterval(0.8f, 1.0f);
			redColour		= ESPInterval(GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR.R());
			greenColour = ESPInterval(GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR.G());
			blueColour	=	ESPInterval(GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR.B());
			break;

		case GameItem::Bad:
			redRandomColour		= ESPInterval(0.8f, 1.0f);
			greenRandomColour = ESPInterval(0.0f, 0.70f);
			blueRandomColour	= ESPInterval(0.0f, 0.70f);

			redColour		= ESPInterval(GameViewConstants::GetInstance()->ITEM_BAD_COLOUR.R());
			greenColour = ESPInterval(GameViewConstants::GetInstance()->ITEM_BAD_COLOUR.G());
			blueColour	=	ESPInterval(GameViewConstants::GetInstance()->ITEM_BAD_COLOUR.B());

			itemSpecificFillStarTex		 = this->evilStarTex;
			itemSpecificOutlineStarTex = this->evilStarOutlineTex;
			break;

		case GameItem::Neutral:
			blueRandomColour = ESPInterval(0.8f, 1.0f);
			redColour		= ESPInterval(GameViewConstants::GetInstance()->ITEM_NEUTRAL_COLOUR.R());
			greenColour = ESPInterval(GameViewConstants::GetInstance()->ITEM_NEUTRAL_COLOUR.G());
			blueColour	=	ESPInterval(GameViewConstants::GetInstance()->ITEM_NEUTRAL_COLOUR.B());
			break;

		default:
			assert(false);
			break;
	}

	// Aura around the ends of the droping item
	ESPPointEmitter* itemDropEmitterAura1 = new ESPPointEmitter();
	itemDropEmitterAura1->SetSpawnDelta(ESPInterval(-1));
	itemDropEmitterAura1->SetInitialSpd(ESPInterval(0));
	itemDropEmitterAura1->SetParticleLife(ESPInterval(-1));
	itemDropEmitterAura1->SetParticleSize(ESPInterval(GameItem::ITEM_HEIGHT + 0.6f), ESPInterval(GameItem::ITEM_HEIGHT + 0.6f));
	itemDropEmitterAura1->SetEmitAngleInDegrees(0);
	itemDropEmitterAura1->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	itemDropEmitterAura1->SetAsPointSpriteEmitter(true);
	itemDropEmitterAura1->SetEmitPosition(Point3D(GameItem::HALF_ITEM_WIDTH, 0, 0));
	itemDropEmitterAura1->SetParticleColour(redColour, greenColour, blueColour, ESPInterval(0.6f * itemAlpha));
	itemDropEmitterAura1->AddEffector(&this->particlePulseItemDropAura);
	itemDropEmitterAura1->SetParticles(1, this->circleGradientTex);
	
	ESPPointEmitter* itemDropEmitterAura2 = new ESPPointEmitter();
	itemDropEmitterAura2->SetSpawnDelta(ESPInterval(-1));
	itemDropEmitterAura2->SetInitialSpd(ESPInterval(0));
	itemDropEmitterAura2->SetParticleLife(ESPInterval(-1));
	itemDropEmitterAura2->SetParticleSize(ESPInterval(GameItem::ITEM_HEIGHT + 0.6f), ESPInterval(GameItem::ITEM_HEIGHT + 0.6f));
	itemDropEmitterAura2->SetEmitAngleInDegrees(0);
	itemDropEmitterAura2->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	itemDropEmitterAura2->SetAsPointSpriteEmitter(true);
	itemDropEmitterAura2->SetEmitPosition(Point3D(-GameItem::HALF_ITEM_WIDTH, 0, 0));
	itemDropEmitterAura2->SetParticleColour(redColour, greenColour, blueColour, ESPInterval(0.6f * itemAlpha));
	itemDropEmitterAura2->AddEffector(&this->particlePulseItemDropAura);
	itemDropEmitterAura2->SetParticles(1, this->circleGradientTex);
	
	// Add the aura emitters
	this->activeItemDropEmitters[&item].push_back(itemDropEmitterAura1);
	this->activeItemDropEmitters[&item].push_back(itemDropEmitterAura2);

	if (showStars) {
		// Middle emitter emits solid stars
		ESPPointEmitter* itemDropEmitterTrail1 = new ESPPointEmitter();
		itemDropEmitterTrail1->SetSpawnDelta(ESPInterval(0.08f, 0.2f));
		itemDropEmitterTrail1->SetInitialSpd(ESPInterval(3.0f, 5.0f));
		itemDropEmitterTrail1->SetParticleLife(ESPInterval(1.2f, 2.0f));
		itemDropEmitterTrail1->SetParticleSize(ESPInterval(0.6f, 1.4f));
		itemDropEmitterTrail1->SetParticleColour(redRandomColour, greenRandomColour, blueRandomColour, alpha);
		itemDropEmitterTrail1->SetEmitAngleInDegrees(25);
		itemDropEmitterTrail1->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
		itemDropEmitterTrail1->SetAsPointSpriteEmitter(true);
		itemDropEmitterTrail1->SetEmitDirection(Vector3D(0, 1, 0));
		itemDropEmitterTrail1->SetEmitPosition(Point3D(0, 0, 0));
		itemDropEmitterTrail1->AddEffector(&this->particleFader);
		//itemDropEmitterTrail1->AddEffector(&this->smokeRotatorCW);
		itemDropEmitterTrail1->SetParticles(12, itemSpecificFillStarTex);
		
		// Left emitter emits outlined stars
		ESPPointEmitter* itemDropEmitterTrail2 = new ESPPointEmitter();
		itemDropEmitterTrail2->SetSpawnDelta(ESPInterval(0.08f, 0.2f));
		itemDropEmitterTrail2->SetInitialSpd(ESPInterval(2.5f, 4.0f));
		itemDropEmitterTrail2->SetParticleLife(ESPInterval(1.5f, 2.2f));
		itemDropEmitterTrail2->SetParticleSize(ESPInterval(0.4f, 1.1f));
		itemDropEmitterTrail2->SetParticleColour(redRandomColour, greenRandomColour, blueRandomColour, alpha);
		itemDropEmitterTrail2->SetEmitAngleInDegrees(10);
		itemDropEmitterTrail2->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
		itemDropEmitterTrail2->SetAsPointSpriteEmitter(true);
		itemDropEmitterTrail2->SetEmitDirection(Vector3D(0, 1, 0));
		itemDropEmitterTrail2->SetEmitPosition(Point3D(-GameItem::ITEM_WIDTH/3, 0, 0));
		itemDropEmitterTrail2->AddEffector(&this->particleFader);
		itemDropEmitterTrail2->SetParticles(10, itemSpecificOutlineStarTex);
		
		// Right emitter emits outlined stars
		ESPPointEmitter* itemDropEmitterTrail3 = new ESPPointEmitter();
		itemDropEmitterTrail3->SetSpawnDelta(ESPInterval(0.08f, 0.2f));
		itemDropEmitterTrail3->SetInitialSpd(ESPInterval(2.5f, 4.0f));
		itemDropEmitterTrail3->SetParticleLife(ESPInterval(1.5f, 2.2f));
		itemDropEmitterTrail3->SetParticleSize(ESPInterval(0.4f, 1.1f));
		itemDropEmitterTrail3->SetParticleColour(redRandomColour, greenRandomColour, blueRandomColour, alpha);
		itemDropEmitterTrail3->SetEmitAngleInDegrees(10);
		itemDropEmitterTrail3->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
		itemDropEmitterTrail3->SetAsPointSpriteEmitter(true);
		itemDropEmitterTrail3->SetEmitDirection(Vector3D(0, 1, 0));
		itemDropEmitterTrail3->SetEmitPosition(Point3D(GameItem::ITEM_WIDTH/3, 0, 0));
		itemDropEmitterTrail3->AddEffector(&this->particleFader);
		itemDropEmitterTrail3->SetParticles(10, itemSpecificOutlineStarTex);

		// Add all the star emitters
		this->activeItemDropEmitters[&item].push_back(itemDropEmitterTrail1);
		this->activeItemDropEmitters[&item].push_back(itemDropEmitterTrail2);
		this->activeItemDropEmitters[&item].push_back(itemDropEmitterTrail3);
	}
	
}
/**
 * Removes an effect associated with a dropping item.
 */
void GameESPAssets::RemoveItemDropEffect(const GameItem& item) {

	// Try to find any effects associated with the given item
	std::map<const GameItem*, std::list<ESPEmitter*> >::iterator iter = this->activeItemDropEmitters.find(&item);
	if (iter != this->activeItemDropEmitters.end()) {
		
		// Delete all emitters associated with the item drop effect
		for (std::list<ESPEmitter*>::iterator effectIter = iter->second.begin(); effectIter != iter->second.end(); ++effectIter) {
			ESPEmitter* currEmitter = *effectIter;
			delete currEmitter;
			currEmitter = NULL;
		}
		iter->second.clear();

		this->activeItemDropEmitters.erase(&item);
	}
}

/**
 * Completely turn off the stars on the item drop emitters.
 */
void GameESPAssets::TurnOffCurrentItemDropStars() {

	// Start by removing the effect for each item and then replace them with non-star effects
	for (std::map<const GameItem*, std::list<ESPEmitter*> >::iterator iter = this->activeItemDropEmitters.begin();
		iter != this->activeItemDropEmitters.end(); ++iter) {
		
		// Delete all emitters associated with the item drop effect
		for (std::list<ESPEmitter*>::iterator effectIter = iter->second.begin(); effectIter != iter->second.end(); ++effectIter) {
			ESPEmitter* currEmitter = *effectIter;
			delete currEmitter;
			currEmitter = NULL;
		}
		iter->second.clear();
	
		this->AddItemDropEffect(*iter->first, false);
	}
}

/**
 * Adds an effect for a active projectile in-game. This function is called when the projectile is
 * first created (i.e., shot into existance).
 */
void GameESPAssets::AddProjectileEffect(const GameModel& gameModel, const Projectile& projectile) {
	switch (projectile.GetType()) {
        case Projectile::BallLaserBulletProjectile:
            this->AddLaserBallESPEffects(gameModel, projectile);
            break;

		case Projectile::PaddleLaserBulletProjectile:
			this->AddLaserPaddleESPEffects(gameModel, projectile);
			break;

        case Projectile::LaserTurretBulletProjectile:
            this->AddLaserTurretESPEffects(gameModel, projectile);
            break;

		case Projectile::PaddleRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
            assert(dynamic_cast<const RocketProjectile*>(&projectile) != NULL);
			this->AddRocketProjectileEffects(*static_cast<const RocketProjectile*>(&projectile));
			break;

        case Projectile::PaddleMineBulletProjectile:
            assert(dynamic_cast<const PaddleMineProjectile*>(&projectile) != NULL);
            this->AddPaddleMineFiredEffects(gameModel, *static_cast<const PaddleMineProjectile*>(&projectile));
            break;

        case Projectile::MineTurretBulletProjectile:
            // TODO... ?
            break;

		case Projectile::CollateralBlockProjectile:
			this->AddCollateralProjectileEffects(projectile);
			break;

		case Projectile::FireGlobProjectile:
			this->AddFireGlobProjectileEffects(projectile);
			break;

		default:
			assert(false);
			break;
	}
}

/**
 * Removes effects associated with the given projectile.
 */
void GameESPAssets::RemoveProjectileEffect(const Projectile& projectile) {
	
 	std::map<const Projectile*, std::list<ESPPointEmitter*> >::iterator projIter = this->activeProjectileEmitters.find(&projectile);
	if (projIter != this->activeProjectileEmitters.end()) {
			
		std::list<ESPPointEmitter*>& projEffects = projIter->second;
		for (std::list<ESPPointEmitter*>::iterator effectIter = projEffects.begin(); effectIter != projEffects.end(); ++effectIter) {
			ESPPointEmitter* currEmitter = *effectIter;
			delete currEmitter;
			currEmitter = NULL;
		}
		projEffects.clear();

		this->activeProjectileEmitters.erase(projIter);
	}
}

void GameESPAssets::RemoveAllProjectileEffects() {
   	std::map<const Projectile*, std::list<ESPPointEmitter*> >::iterator projIter = this->activeProjectileEmitters.begin();
	for (; projIter != this->activeProjectileEmitters.end(); ++projIter) {
			
		std::list<ESPPointEmitter*>& projEffects = projIter->second;
		for (std::list<ESPPointEmitter*>::iterator effectIter = projEffects.begin(); effectIter != projEffects.end(); ++effectIter) {
			ESPPointEmitter* currEmitter = *effectIter;
			delete currEmitter;
			currEmitter = NULL;
		}
		projEffects.clear();
	}
    this->activeProjectileEmitters.clear();
}

/**
 * Set the beam emitters for the paddle laser beam effect.
 */
void GameESPAssets::AddPaddleLaserBeamEffect(const Beam& beam) {
	assert(this->activeBeamEmitters.find(&beam) == this->activeBeamEmitters.end());

	const std::list<BeamSegment*>& beamSegments = beam.GetBeamParts();
	assert(beamSegments.size() > 0);

	std::list<ESPEmitter*> beamEmitters;

	// The first laser has a cool blast for where it comes out of the paddle...
	BeamSegment* startSegment = *beam.GetBeamParts().begin();
	Point3D beamSegOrigin3D(startSegment->GetStartPoint());

	Vector3D beamRightVec(startSegment->GetBeamSegmentRay().GetUnitDirection());
	Vector3D beamDiagonalVec = startSegment->GetRadius() * Vector3D(beamRightVec[1], -beamRightVec[0], PlayerPaddle::PADDLE_HALF_DEPTH);
	beamDiagonalVec.Abs();

	this->paddleBeamOriginUp->SetEmitDirection(Vector3D(startSegment->GetBeamSegmentRay().GetUnitDirection()));
	ESPInterval xSize(0.5f * startSegment->GetRadius(), 1.5f * startSegment->GetRadius());
	this->paddleBeamOriginUp->SetParticleSize(xSize);
	this->paddleBeamOriginUp->SetEmitVolume(beamSegOrigin3D - beamDiagonalVec, beamSegOrigin3D + beamDiagonalVec);
	beamEmitters.push_back(this->paddleBeamOriginUp);

	size_t beamEndCounter      = 0;
	size_t beamFlareCounter    = 0;
	size_t beamBlockEndCounter = 0;
	for (std::list<BeamSegment*>::const_iterator iter = beamSegments.begin(); iter != beamSegments.end(); ++iter) {
		const BeamSegment* currentBeamSeg = *iter;

		// Each beam segment has a blasty thingying at the end of it, if it ends at a 
		// prism block then it also has a lens flare...
		
		// Make sure there are enough buffered end effects for use...
		while (this->beamEndEmitters.size() <= beamEndCounter) {
			this->beamEndEmitters.push_back(this->CreateBeamEndEffect());
		}

		// Blasty thingy
		ESPPointEmitter* beamEndEmitter = this->beamEndEmitters[beamEndCounter];
		assert(beamEndEmitter != NULL);
		beamEndEmitter->SetParticleSize(2.5 * currentBeamSeg->GetRadius());
		beamEndEmitter->SetEmitPosition(Point3D(currentBeamSeg->GetEndPoint()));
		beamEndEmitter->Reset();
		beamEndEmitter->Tick(1.0f);
		beamEmitters.push_back(beamEndEmitter);
		beamEndCounter++;

		// Lens flare
		if (currentBeamSeg->GetCollidingPiece() != NULL) {
			if (currentBeamSeg->GetCollidingPiece()->IsLightReflectorRefractor()) {
				while (this->beamFlareEmitters.size() <= beamFlareCounter) {
					this->beamFlareEmitters.push_back(this->CreateBeamFlareEffect());
				}
				ESPPointEmitter* beamStartFlare = this->beamFlareEmitters[beamFlareCounter];
				assert(beamStartFlare != NULL);
				beamStartFlare->SetParticleSize(5 * currentBeamSeg->GetRadius());
				beamStartFlare->SetEmitPosition(Point3D(currentBeamSeg->GetEndPoint()));
				beamStartFlare->Reset();
				beamStartFlare->Tick(1.0f);
				beamEmitters.push_back(beamStartFlare);
				beamFlareCounter++;
			}
			else {
				// Add beam end block emitter
				while (this->beamBlockOnlyEndEmitters.size() <= beamBlockEndCounter) {
					this->beamBlockOnlyEndEmitters.push_back(this->CreateBeamEndBlockEffect());	
				}
				ESPPointEmitter* beamBlockEndEffect = this->beamBlockOnlyEndEmitters[beamBlockEndCounter];
				assert(beamBlockEndEffect != NULL);
				ESPInterval xSize(0.5f * currentBeamSeg->GetRadius(), 1.0f * currentBeamSeg->GetRadius());
				ESPInterval ySize(xSize.maxValue);
				beamBlockEndEffect->SetParticleSize(xSize, ySize);
				beamBlockEndEffect->SetEmitPosition(Point3D(currentBeamSeg->GetEndPoint()));
				beamBlockEndEffect->SetEmitDirection(Vector3D(-currentBeamSeg->GetBeamSegmentRay().GetUnitDirection()));
				beamBlockEndEffect->Reset();
				beamBlockEndEffect->Tick(1.0f);
				beamEmitters.push_back(beamBlockEndEffect);

				while (this->beamEndFallingBitsEmitters.size() <= beamBlockEndCounter) {
					this->beamEndFallingBitsEmitters.push_back(this->CreateBeamFallingBitEffect());
				}
				ESPPointEmitter* beamFallingBitEffect = this->beamEndFallingBitsEmitters[beamBlockEndCounter];
				assert(beamFallingBitEffect != NULL);
				beamFallingBitEffect->SetParticleSize(xSize);
				beamFallingBitEffect->SetEmitPosition(Point3D(currentBeamSeg->GetEndPoint()));
				beamFallingBitEffect->SetEmitDirection(Vector3D(-currentBeamSeg->GetBeamSegmentRay().GetUnitDirection()));
				beamFallingBitEffect->Reset();
				beamFallingBitEffect->Tick(1.0f);
				beamEmitters.push_back(beamFallingBitEffect);

				beamBlockEndCounter++;
			}
		}
	}

	// Add all the beams to the active beams, associated with the given beam
	this->activeBeamEmitters.erase(&beam);
	this->activeBeamEmitters.insert(std::make_pair(&beam, beamEmitters));
}

/**
 * Spawns a new beam end emitter effect.
 */
ESPPointEmitter* GameESPAssets::CreateBeamEndEffect() {
	ESPPointEmitter* beamEndEffect = new ESPPointEmitter();
	beamEndEffect->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	beamEndEffect->SetInitialSpd(ESPInterval(0));
	beamEndEffect->SetParticleLife(ESPInterval(ESPParticle::INFINITE_PARTICLE_LIFETIME));
	beamEndEffect->SetEmitAngleInDegrees(0);
	beamEndEffect->SetParticleAlignment(ESP::ScreenAligned);
	beamEndEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	beamEndEffect->SetEmitPosition(Point3D(0, 0, 0));
	beamEndEffect->SetParticleColour(ESPInterval(0.75f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	beamEndEffect->AddEffector(&this->beamEndPulse);
	bool result = beamEndEffect->SetParticles(1, this->circleGradientTex);
    UNUSED_VARIABLE(result);
	assert(result);

	return beamEndEffect;
}

/**
 * Spawns a new beam end emitter effect for blocks that are not reflecting/refracting the beam -
 * this effect looks like sparky stretched flashing rebounding off the block.
 */
ESPPointEmitter* GameESPAssets::CreateBeamEndBlockEffect() {
	ESPPointEmitter* beamBlockEndEffect = new ESPPointEmitter();
	beamBlockEndEffect->SetSpawnDelta(ESPInterval(0.005f, 0.015f));
	beamBlockEndEffect->SetInitialSpd(ESPInterval(2.0f, 3.5f));
	beamBlockEndEffect->SetParticleLife(ESPInterval(0.4f, 0.65f));
	beamBlockEndEffect->SetEmitAngleInDegrees(87);
	beamBlockEndEffect->SetAsPointSpriteEmitter(false);
	beamBlockEndEffect->SetParticleAlignment(ESP::ScreenAlignedFollowVelocity);
	beamBlockEndEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	beamBlockEndEffect->SetEmitPosition(Point3D(0, 0, 0));
	beamBlockEndEffect->SetParticleColour(ESPInterval(0.8f, 1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	beamBlockEndEffect->SetToggleEmitOnPlane(true, Vector3D(0, 0, 1));
	beamBlockEndEffect->AddEffector(&this->particleLargeVStretch);
	//beamBlockEndEffect->AddEffector(&this->particleFader);
	//beamBlockEndEffect->AddEffector(&this->gravity);
	bool result = beamBlockEndEffect->SetParticles(35, this->circleGradientTex);
    UNUSED_VARIABLE(result);
	assert(result);

	return beamBlockEndEffect;	

}

/**
 * Spawns a new beam end emitter effect for blocks that are not reflecting/refracting the beam -
 * this effect looks like little bits/sparks falling off the block.
 */
ESPPointEmitter* GameESPAssets::CreateBeamFallingBitEffect() {
	ESPPointEmitter* beamFallingBitEffect = new ESPPointEmitter();
	beamFallingBitEffect->SetSpawnDelta(ESPInterval(0.01f, 0.02f));
	beamFallingBitEffect->SetInitialSpd(ESPInterval(2.0f, 3.5f));
	beamFallingBitEffect->SetParticleLife(ESPInterval(0.8f, 1.0f));
	beamFallingBitEffect->SetEmitAngleInDegrees(80);
	beamFallingBitEffect->SetAsPointSpriteEmitter(true);
	beamFallingBitEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	beamFallingBitEffect->SetEmitPosition(Point3D(0, 0, 0));
	beamFallingBitEffect->SetParticleColour(ESPInterval(0.9f, 1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	//beamFallingBitEffect->SetToggleEmitOnPlane(true, Vector3D(0, 0, 1));
	beamFallingBitEffect->AddEffector(&this->particleMediumShrink);
	beamFallingBitEffect->AddEffector(&this->gravity);
	bool result = beamFallingBitEffect->SetParticles(25, this->circleGradientTex);
    UNUSED_VARIABLE(result);
	assert(result);

	return beamFallingBitEffect;	
}

/**
 * Spawns a new beam flare emitter for special places on the beam.
 */
ESPPointEmitter* GameESPAssets::CreateBeamFlareEffect() {
	ESPPointEmitter* beamFlareEffect = new ESPPointEmitter();
	beamFlareEffect->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	beamFlareEffect->SetInitialSpd(ESPInterval(0));
	beamFlareEffect->SetParticleLife(ESPInterval(ESPParticle::INFINITE_PARTICLE_LIFETIME));
	beamFlareEffect->SetEmitAngleInDegrees(0);
	beamFlareEffect->SetParticleAlignment(ESP::ScreenAligned);
	beamFlareEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	beamFlareEffect->SetEmitPosition(Point3D(0, 0, 0));
	beamFlareEffect->SetParticleColour(ESPInterval(0.75f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(0.8f));
	beamFlareEffect->AddEffector(&this->beamEndPulse);
	bool result = beamFlareEffect->SetParticles(1, this->lensFlareTex);
    UNUSED_VARIABLE(result);
	assert(result);

	return beamFlareEffect;
}

/**
 * Add a brand new set of effects for the given beam so that they are active and drawn
 * in the game.
 */
void GameESPAssets::AddBeamEffect(const Beam& beam) {
	std::list<ESPEmitter*> beamEmitters;
	switch (beam.GetBeamType()) {
		case Beam::PaddleLaserBeam:
			this->AddPaddleLaserBeamEffect(beam);
			break;
		default:
			assert(false);
			break;
	}
}

/**
 * Update the effects for the given beam - the beam must already have been added via
 * the AddBeamEffect function. The beam's effects will not be recreated, but instead they
 * will simply have their values changed (saves time - instead of deleting and creating heap stuffs).
 */
void GameESPAssets::UpdateBeamEffect(const Beam& beam) {
	// Clean up the effects and add a new one with the new updated parameters...
	this->RemoveBeamEffect(beam);
	this->AddBeamEffect(beam);
}

/**
 * Remove all effects associated with the given beam if any exist.
 */
void GameESPAssets::RemoveBeamEffect(const Beam& beam) {
	std::map<const Beam*, std::list<ESPEmitter*> >::iterator foundBeamIter = this->activeBeamEmitters.find(&beam);
	if (foundBeamIter != this->activeBeamEmitters.end()) {
		this->activeBeamEmitters.erase(foundBeamIter);	
	}
	else {
		assert(false);
	}
}

// Adds a tesla lightning arc effect
void GameESPAssets::AddTeslaLightningBarrierEffect(const TeslaBlock& block1, const TeslaBlock& block2, const Vector3D& levelTranslation) {

	Point3D startPt = Point3D(block1.GetCenter()) + levelTranslation;
	Point3D endPt   = Point3D(block2.GetCenter()) + levelTranslation;

	// Base the number of segments on the distance between the tesla blocks!!
	float distance = Point2D::Distance(block1.GetCenter(), block2.GetCenter());
	static const float NUM_SEGMENTS_PER_UNIT_DIST = 1.5f;
	int totalSegments = NUM_SEGMENTS_PER_UNIT_DIST * distance;

	// Create the large central lightining arc
	ESPPointToPointBeam* bigLightningArc = new ESPPointToPointBeam();
	bigLightningArc->SetStartAndEndPoints(startPt, endPt);
	bigLightningArc->SetColour(ColourRGBA(1,1,1,1));
	bigLightningArc->SetBeamLifetime(ESPInterval(ESPBeam::INFINITE_BEAM_LIFETIME));
	bigLightningArc->SetNumBeamShots(1);
	bigLightningArc->SetMainBeamAmplitude(ESPInterval(0.0f, 0.7f * LevelPiece::PIECE_HEIGHT));
	bigLightningArc->SetMainBeamThickness(ESPInterval(LevelPiece::PIECE_WIDTH / 9.0f, LevelPiece::PIECE_WIDTH / 8.0f));
	bigLightningArc->SetNumMainESPBeamSegments(totalSegments);
	
	// Create some purpleish smaller arcs
	ESPPointToPointBeam* smallerLightningArcs = new ESPPointToPointBeam();
	smallerLightningArcs->SetStartAndEndPoints(startPt, endPt);
	smallerLightningArcs->SetColour(ColourRGBA(0.9f, 0.75f, 1.0f, 1.0f));
	smallerLightningArcs->SetBeamLifetime(ESPInterval(ESPBeam::INFINITE_BEAM_LIFETIME));
	smallerLightningArcs->SetNumBeamShots(2);
	smallerLightningArcs->SetMainBeamAmplitude(ESPInterval(0.1f * LevelPiece::PIECE_HEIGHT, 0.9f * LevelPiece::PIECE_HEIGHT));
	smallerLightningArcs->SetMainBeamThickness(ESPInterval(LevelPiece::PIECE_WIDTH / 20.0f, LevelPiece::PIECE_WIDTH / 18.0f));
	smallerLightningArcs->SetNumMainESPBeamSegments(totalSegments);

	std::list<ESPPointToPointBeam*> lightningArcs;
	lightningArcs.push_back(bigLightningArc);
	lightningArcs.push_back(smallerLightningArcs);

	std::pair<std::map<std::pair<const TeslaBlock*, const TeslaBlock*>, std::list<ESPPointToPointBeam*> >::const_iterator, bool> insertResult =
		this->teslaLightningArcs.insert(std::make_pair(std::make_pair(&block1, &block2), lightningArcs));
	assert(insertResult.second);
}

// Removes a tesla lightning arc barrier from the effects list
void GameESPAssets::RemoveTeslaLightningBarrierEffect(const TeslaBlock& block1, const TeslaBlock& block2) {
	// Find the effect for the lightning arc among the existing effects
	std::map<std::pair<const TeslaBlock*, const TeslaBlock*>, std::list<ESPPointToPointBeam*> >::iterator findIter = this->teslaLightningArcs.find(std::make_pair(&block1, &block2));
	if (findIter == this->teslaLightningArcs.end()) {
		findIter = this->teslaLightningArcs.find(std::make_pair(&block2, &block1));
		assert(findIter != this->teslaLightningArcs.end());
	}

	// Clean up all of the effects
	std::list<ESPPointToPointBeam*>& lightningArcs = findIter->second;
	for (std::list<ESPPointToPointBeam*>::iterator iter = lightningArcs.begin(); iter != lightningArcs.end(); ++iter) {
		ESPPointToPointBeam* currArc = *iter;
		delete currArc;
		currArc = NULL;
	}
	lightningArcs.clear();
	this->teslaLightningArcs.erase(findIter);
}

void GameESPAssets::SetTeslaLightiningAlpha(float alpha) {
	for (std::map<std::pair<const TeslaBlock*, const TeslaBlock*>, std::list<ESPPointToPointBeam*> >::iterator iter1 = this->teslaLightningArcs.begin();
		iter1 != this->teslaLightningArcs.end(); ++iter1) {
		
		std::list<ESPPointToPointBeam*>& lightningArcs = iter1->second;
		for (std::list<ESPPointToPointBeam*>::iterator iter2 = lightningArcs.begin(); iter2 != lightningArcs.end(); ++iter2) {
			ESPPointToPointBeam* arc = *iter2;
			arc->SetAlpha(alpha);
		}
	}
}

/**
 * Adds a Timer HUD effect for the given item type.
 */
void GameESPAssets::AddTimerHUDEffect(GameItem::ItemType type, GameItem::ItemDisposition disposition) {
	// Remove any previously active effects for the given item type
	std::map<GameItem::ItemType, std::list<ESPEmitter*> >::iterator foundEffects = this->activeTimerHUDEmitters.find(type);
	if (foundEffects != this->activeTimerHUDEmitters.end()) {
		std::list<ESPEmitter*>& foundEmitters = foundEffects->second;
		for (std::list<ESPEmitter*>::iterator iter = foundEmitters.begin(); iter != foundEmitters.end(); ++iter) {
			ESPEmitter* currEmitter = *iter;
			delete currEmitter;
			currEmitter = NULL;
		}
		this->activeTimerHUDEmitters.erase(foundEffects);
	}

	ESPInterval redRandomColour(0.1f, 0.8f);
	ESPInterval greenRandomColour(0.1f, 0.8f);
	ESPInterval blueRandomColour(0.1f, 0.8f);
	ESPInterval redColour(0), greenColour(0), blueColour(0);

	// We choose a specific kind of sprite graphic based on whether we are dealing with a power-down or not
	// (evil stars for bad items!!)
	Texture2D* itemSpecificFillStarTex = this->starTex;
	Texture2D* itemSpecificOutlineStarTex = this->starOutlineTex;

	switch (disposition) {
		case GameItem::Good:
			greenRandomColour = ESPInterval(0.8f, 1.0f);
			redColour		= ESPInterval(GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR.R());
			greenColour = ESPInterval(GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR.G());
			blueColour	=	ESPInterval(GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR.B());
			break;

		case GameItem::Bad:
			redRandomColour		= ESPInterval(0.8f, 1.0f);
			redColour		= ESPInterval(GameViewConstants::GetInstance()->ITEM_BAD_COLOUR.R());
			greenColour = ESPInterval(GameViewConstants::GetInstance()->ITEM_BAD_COLOUR.G());
			blueColour	=	ESPInterval(GameViewConstants::GetInstance()->ITEM_BAD_COLOUR.B());

			itemSpecificFillStarTex		 = this->evilStarTex;
			itemSpecificOutlineStarTex = this->evilStarOutlineTex;
			break;

		case GameItem::Neutral:
			blueRandomColour = ESPInterval(0.8f, 1.0f);
			redColour		= ESPInterval(GameViewConstants::GetInstance()->ITEM_NEUTRAL_COLOUR.R());
			greenColour = ESPInterval(GameViewConstants::GetInstance()->ITEM_NEUTRAL_COLOUR.G());
			blueColour	=	ESPInterval(GameViewConstants::GetInstance()->ITEM_NEUTRAL_COLOUR.B());
			break;

		default:
			assert(false);
			break;
	}

	const ESPInterval starSpawnDelta(0.02f, 0.06f);
	const ESPInterval starInitialSpd(40.0f, 60.0f);
	const ESPInterval starLife(2.0f, 2.5f);
	const ESPInterval starSize(5.0f, 15.0f); // In pixels on the screen...

	ESPPointEmitter* solidStarEmitter = new ESPPointEmitter();
	solidStarEmitter->SetSpawnDelta(starSpawnDelta);
	solidStarEmitter->SetInitialSpd(starInitialSpd);
	solidStarEmitter->SetParticleLife(starLife);
	solidStarEmitter->SetNumParticleLives(1);
	solidStarEmitter->SetParticleSize(starSize);
	solidStarEmitter->SetParticleColour(redRandomColour, greenRandomColour, blueRandomColour, ESPInterval(1.0f));
	solidStarEmitter->SetEmitAngleInDegrees(180);
	solidStarEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	//solidStarEmitter->SetAsPointSpriteEmitter(true);
	solidStarEmitter->SetParticleAlignment(ESP::NoAlignment);
	solidStarEmitter->SetEmitDirection(Vector3D(0, 1, 0));
	solidStarEmitter->SetToggleEmitOnPlane(true, Vector3D(0, 0, 1));
	solidStarEmitter->SetEmitPosition(Point3D(0, 0, 0));
	solidStarEmitter->AddEffector(&this->particleFader);
	bool result = solidStarEmitter->SetParticles(7, itemSpecificFillStarTex);
	assert(result);

	ESPPointEmitter* outlineStarEmitter = new ESPPointEmitter();
	outlineStarEmitter->SetSpawnDelta(starSpawnDelta);
	outlineStarEmitter->SetInitialSpd(starInitialSpd);
	outlineStarEmitter->SetParticleLife(starLife);
	outlineStarEmitter->SetNumParticleLives(1);
	outlineStarEmitter->SetParticleSize(starSize);
	outlineStarEmitter->SetParticleColour(redRandomColour, greenRandomColour, blueRandomColour, ESPInterval(1.0f));
	outlineStarEmitter->SetEmitAngleInDegrees(180);
	outlineStarEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	//outlineStarEmitter->SetAsPointSpriteEmitter(true);
	outlineStarEmitter->SetParticleAlignment(ESP::NoAlignment);
	outlineStarEmitter->SetEmitDirection(Vector3D(0, 1, 0));
	outlineStarEmitter->SetToggleEmitOnPlane(true, Vector3D(0, 0, 1));
	outlineStarEmitter->SetEmitPosition(Point3D(0, 0, 0));
	outlineStarEmitter->AddEffector(&this->particleFader);
	result = outlineStarEmitter->SetParticles(7, itemSpecificOutlineStarTex);
	assert(result);

	ESPPointEmitter* haloExpandingAura = new ESPPointEmitter();
	haloExpandingAura->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	haloExpandingAura->SetInitialSpd(ESPInterval(0));
	haloExpandingAura->SetParticleLife(ESPInterval(2.5f));
	haloExpandingAura->SetParticleSize(ESPInterval(100), ESPInterval(75)); // In pixels on the screen...
	haloExpandingAura->SetEmitAngleInDegrees(0);
	haloExpandingAura->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	haloExpandingAura->SetParticleAlignment(ESP::NoAlignment);
	haloExpandingAura->SetEmitPosition(Point3D(0, 0, 0));
	haloExpandingAura->SetParticleColour(redColour, greenColour, blueColour, ESPInterval(0.8f));
	haloExpandingAura->AddEffector(&this->particleLargeGrowth);
	haloExpandingAura->AddEffector(&this->particleFader);
	result = haloExpandingAura->SetParticles(1, this->haloTex);
	assert(result);

	std::list<ESPEmitter*> timerHUDEmitters;
	timerHUDEmitters.push_back(solidStarEmitter);
	timerHUDEmitters.push_back(outlineStarEmitter);
	timerHUDEmitters.push_back(haloExpandingAura);

	this->activeTimerHUDEmitters.insert(std::make_pair(type, timerHUDEmitters));
}

/**
 * Add the projectile effects for the given collateral block projectile.
 */
void GameESPAssets::AddCollateralProjectileEffects(const Projectile& projectile) {
	assert(projectile.GetType() == Projectile::CollateralBlockProjectile);

	const Point2D& projectilePos2D = projectile.GetPosition();
	Point3D projectilePos3D = Point3D(projectilePos2D[0], projectilePos2D[1], 0.0f);
	const Vector2D& projectileDir = projectile.GetVelocityDirection();
	Vector3D projectileDir3D = Vector3D(projectileDir[0], projectileDir[1], 0.0f);
	float projectileSpd = projectile.GetVelocityMagnitude();

	// Create the trail of smoke and fire effects...
	ESPPointEmitter* fireCloudTrail = new ESPPointEmitter();
	fireCloudTrail->SetSpawnDelta(ESPInterval(0.008f, 0.015f));
	fireCloudTrail->SetInitialSpd(ESPInterval(projectileSpd));
	fireCloudTrail->SetParticleLife(ESPInterval(0.7f, 0.9f));
	fireCloudTrail->SetParticleSize(ESPInterval(projectile.GetHalfWidth(), projectile.GetWidth()));
	fireCloudTrail->SetEmitAngleInDegrees(5);
	fireCloudTrail->SetEmitDirection(-projectileDir3D);
	fireCloudTrail->SetRadiusDeviationFromCenter(ESPInterval(0.5f * projectile.GetHalfHeight()));
	fireCloudTrail->SetAsPointSpriteEmitter(true);
	fireCloudTrail->SetEmitPosition(projectilePos3D - 1.5f * projectile.GetHeight() * projectileDir3D);
	fireCloudTrail->AddEffector(&this->particleLargeGrowth);
	fireCloudTrail->AddEffector(&this->particleFireColourFader);
	fireCloudTrail->SetParticles(10, this->explosionTex);

	this->activeProjectileEmitters[&projectile].push_back(fireCloudTrail);
}

// Add effects for the rocket projectile (e.g., fire trail and smoke)
void GameESPAssets::AddRocketProjectileEffects(const RocketProjectile& projectile) {

	ESPPointEmitter* fireyTrailEmitter = new ESPPointEmitter();
	fireyTrailEmitter->SetSpawnDelta(ESPInterval(0.01f));
	fireyTrailEmitter->SetInitialSpd(ESPInterval(0.0f));
	fireyTrailEmitter->SetParticleLife(ESPInterval(0.5f));
	fireyTrailEmitter->SetParticleSize(ESPInterval(0.9f*projectile.GetWidth(), projectile.GetWidth()));
	fireyTrailEmitter->SetEmitAngleInDegrees(15);
	fireyTrailEmitter->SetEmitDirection(Vector3D(0, -1, 0));
	fireyTrailEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	fireyTrailEmitter->SetParticleAlignment(ESP::ScreenAligned);
    fireyTrailEmitter->SetEmitPosition(Point3D(0, 0, projectile.GetZOffset()));
	fireyTrailEmitter->AddEffector(&this->particleFireColourFader);
	fireyTrailEmitter->AddEffector(&this->particleLargeGrowth);
	bool result = fireyTrailEmitter->SetParticles(25, &this->fireEffect);
	assert(result);

	size_t randomTexIndex1 = Randomizer::GetInstance()->RandomUnsignedInt() % this->smokeTextures.size();
	size_t randomTexIndex2 = (randomTexIndex1 + 1) % this->smokeTextures.size();

	ESPPointEmitter* smokeyTrailEmitter1 = new ESPPointEmitter();
	smokeyTrailEmitter1->SetSpawnDelta(ESPInterval(0.1f));
	smokeyTrailEmitter1->SetInitialSpd(ESPInterval(1.0f));
	smokeyTrailEmitter1->SetParticleLife(ESPInterval(0.5f, 0.75f));
	smokeyTrailEmitter1->SetParticleSize(ESPInterval(0.75f*projectile.GetWidth(), 0.85f*projectile.GetWidth()));
	smokeyTrailEmitter1->SetEmitAngleInDegrees(45);
	smokeyTrailEmitter1->SetEmitDirection(Vector3D(0, -1, 0));
	smokeyTrailEmitter1->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	smokeyTrailEmitter1->SetParticleAlignment(ESP::ScreenAligned);
	smokeyTrailEmitter1->SetEmitPosition(Point3D(0, 0, projectile.GetZOffset()));
	smokeyTrailEmitter1->AddEffector(&this->particleFireColourFader);
	smokeyTrailEmitter1->AddEffector(&this->particleLargeGrowth);
	smokeyTrailEmitter1->AddEffector(&this->explosionRayRotatorCW);
	result = smokeyTrailEmitter1->SetParticles(5, this->smokeTextures[randomTexIndex1]);
	assert(result);

	ESPPointEmitter* smokeyTrailEmitter2 = new ESPPointEmitter();
	smokeyTrailEmitter2->SetSpawnDelta(ESPInterval(0.1f));
	smokeyTrailEmitter2->SetInitialSpd(ESPInterval(1.0f));
	smokeyTrailEmitter2->SetParticleLife(ESPInterval(0.5f, 0.75f));
	smokeyTrailEmitter2->SetParticleSize(ESPInterval(0.75f*projectile.GetWidth(), 0.85f*projectile.GetWidth()));
	smokeyTrailEmitter2->SetEmitAngleInDegrees(45);
	smokeyTrailEmitter2->SetEmitDirection(Vector3D(0, -1, 0));
	smokeyTrailEmitter2->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	smokeyTrailEmitter2->SetParticleAlignment(ESP::ScreenAligned);
	smokeyTrailEmitter2->SetEmitPosition(Point3D(0, 0, projectile.GetZOffset()));
	smokeyTrailEmitter2->AddEffector(&this->particleFireColourFader);
	smokeyTrailEmitter2->AddEffector(&this->particleLargeGrowth);
	smokeyTrailEmitter2->AddEffector(&this->explosionRayRotatorCCW);
	result = smokeyTrailEmitter2->SetParticles(5, this->smokeTextures[randomTexIndex2]);
	assert(result);

	std::list<ESPPointEmitter*>& projectileEmitters = this->activeProjectileEmitters[&projectile];
	projectileEmitters.push_back(smokeyTrailEmitter1);
	projectileEmitters.push_back(smokeyTrailEmitter2);
	projectileEmitters.push_back(fireyTrailEmitter);
}

// Add effects for a fire glob projectile (from blocks on fire)
void GameESPAssets::AddFireGlobProjectileEffects(const Projectile& projectile) {

	static const ESPInterval SMOKE_LIFE = ESPInterval(0.5f, 1.0f);
	static const ESPInterval FIRE_TRAIL_LIFE = ESPInterval(SMOKE_LIFE.minValue - 0.05f, SMOKE_LIFE.maxValue - 0.1f);
	static const ESPInterval SMOKE_SPAWN_DELTA = ESPInterval(0.2f, 0.35f);
	static const ESPInterval SMOKE_SPD = ESPInterval(0.8f, 1.5f);
	static const ESPInterval FIRE_TRAIL_SPD = ESPInterval(SMOKE_SPD.minValue - 0.1f, SMOKE_SPD.maxValue - 0.25f);
	static const int NUM_SMOKE_PARTICLES_PER_EMITTER = 4;

	size_t randomTexIndex1 = Randomizer::GetInstance()->RandomUnsignedInt() % this->smokeTextures.size();
	size_t randomTexIndex2 = (randomTexIndex1 + 1) % this->smokeTextures.size();

	ESPPointEmitter* smokeyTrailEmitter1 = new ESPPointEmitter();
	smokeyTrailEmitter1->SetSpawnDelta(SMOKE_SPAWN_DELTA);
	smokeyTrailEmitter1->SetInitialSpd(SMOKE_SPD);
	smokeyTrailEmitter1->SetParticleLife(SMOKE_LIFE);
	smokeyTrailEmitter1->SetParticleSize(ESPInterval(0.5f*projectile.GetWidth(), 1.0f*projectile.GetWidth()));
	smokeyTrailEmitter1->SetEmitAngleInDegrees(30);
	smokeyTrailEmitter1->SetEmitDirection(Vector3D(0, 1, 0));
	smokeyTrailEmitter1->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	smokeyTrailEmitter1->SetParticleAlignment(ESP::ScreenAligned);
	smokeyTrailEmitter1->SetEmitPosition(Point3D(0, 0, 0));
	smokeyTrailEmitter1->AddEffector(&this->particleFireColourFader);
	smokeyTrailEmitter1->AddEffector(&this->particleLargeGrowth);
	smokeyTrailEmitter1->AddEffector(&this->explosionRayRotatorCW);
	bool result = smokeyTrailEmitter1->SetParticles(NUM_SMOKE_PARTICLES_PER_EMITTER, this->smokeTextures[randomTexIndex1]);
	assert(result);

	ESPPointEmitter* smokeyTrailEmitter2 = new ESPPointEmitter();
	smokeyTrailEmitter2->SetSpawnDelta(SMOKE_SPAWN_DELTA);
	smokeyTrailEmitter2->SetInitialSpd(SMOKE_SPD);
	smokeyTrailEmitter2->SetParticleLife(SMOKE_LIFE);
	smokeyTrailEmitter2->SetParticleSize(ESPInterval(0.75f*projectile.GetWidth(), 0.85f*projectile.GetWidth()));
	smokeyTrailEmitter2->SetEmitAngleInDegrees(30);
	smokeyTrailEmitter2->SetEmitDirection(Vector3D(0, 1, 0));
	smokeyTrailEmitter2->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	smokeyTrailEmitter2->SetParticleAlignment(ESP::ScreenAligned);
	smokeyTrailEmitter2->SetEmitPosition(Point3D(0, 0, 0));
	smokeyTrailEmitter2->AddEffector(&this->particleFireColourFader);
	smokeyTrailEmitter2->AddEffector(&this->particleLargeGrowth);
	smokeyTrailEmitter2->AddEffector(&this->explosionRayRotatorCCW);
	result = smokeyTrailEmitter2->SetParticles(NUM_SMOKE_PARTICLES_PER_EMITTER, this->smokeTextures[randomTexIndex2]);
	smokeyTrailEmitter2->SimulateTicking(0.5f);
	assert(result);

	size_t randomRockIdx = Randomizer::GetInstance()->RandomUnsignedInt() % this->moltenRockEffects.size();
	ESPPointEmitter* fireRock = new ESPPointEmitter();
	fireRock->SetSpawnDelta(ESPInterval(-1));
	fireRock->SetInitialSpd(ESPInterval(0));
	fireRock->SetParticleLife(ESPInterval(-1));
	fireRock->SetParticleSize(ESPInterval(projectile.GetWidth()), ESPInterval(projectile.GetHeight()));
	fireRock->SetEmitAngleInDegrees(0);
	fireRock->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	fireRock->SetParticleAlignment(ESP::ScreenAligned);
	fireRock->SetParticleRotation(ESPInterval(0.0f, 359.9999f));
	fireRock->SetEmitPosition(Point3D(0, 0, 0));
	result = fireRock->SetParticles(1, this->moltenRockEffects[randomRockIdx]);
	assert(result);

	if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
		fireRock->AddEffector(&this->loopRotateEffectorCW);
	}
	else {
		fireRock->AddEffector(&loopRotateEffectorCCW);
	}

	// Add a fire aura around the rock...
	ESPPointEmitter* fireRockAura = new ESPPointEmitter();
	fireRockAura->SetSpawnDelta(ESPInterval(-1));
	fireRockAura->SetInitialSpd(ESPInterval(0));
	fireRockAura->SetParticleLife(ESPInterval(-1));
	fireRockAura->SetParticleSize(ESPInterval(1.3f * projectile.GetWidth()), ESPInterval(1.3f * projectile.GetHeight()));
	fireRockAura->SetEmitAngleInDegrees(0);
	fireRockAura->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	fireRockAura->SetParticleAlignment(ESP::ScreenAligned);
	fireRockAura->SetEmitPosition(Point3D(0, 0, 0));
	fireRockAura->SetParticleColour(ESPInterval(0.9f), ESPInterval(0.8f), ESPInterval(0.0f), ESPInterval(1.0f));
	fireRockAura->AddEffector(&this->particlePulseFireGlobAura);
	result = fireRockAura->SetParticles(1, this->circleGradientTex);
	assert(result);

	ESPPointEmitter* fireBallEmitterTrail = new ESPPointEmitter();
	fireBallEmitterTrail->SetSpawnDelta(ESPInterval(0.1f, 0.15f));
	fireBallEmitterTrail->SetInitialSpd(FIRE_TRAIL_SPD);
	fireBallEmitterTrail->SetParticleLife(FIRE_TRAIL_LIFE);
	fireBallEmitterTrail->SetParticleSize(ESPInterval(1.15f * projectile.GetWidth(), 1.3f * projectile.GetWidth()));
	fireBallEmitterTrail->SetEmitAngleInDegrees(15);
	fireBallEmitterTrail->SetParticleRotation(ESPInterval(-180.0f, 180.0f));
	fireBallEmitterTrail->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	fireBallEmitterTrail->SetParticleAlignment(ESP::ScreenAligned);
	fireBallEmitterTrail->SetEmitPosition(Point3D(0, 0, 0));
	fireBallEmitterTrail->SetEmitDirection(Vector3D(0, 1, 0));
	fireBallEmitterTrail->AddEffector(&this->fireBallColourFader);
	fireBallEmitterTrail->AddEffector(&this->particleMediumGrowth);
	result = fireBallEmitterTrail->SetParticles(9, &this->fireBallTrailEffect);
	assert(result);

	std::list<ESPPointEmitter*>& projectileEmitters = this->activeProjectileEmitters[&projectile];
	projectileEmitters.push_back(smokeyTrailEmitter1);
	projectileEmitters.push_back(smokeyTrailEmitter2);
	projectileEmitters.push_back(fireBallEmitterTrail);
	projectileEmitters.push_back(fireRockAura);
	projectileEmitters.push_back(fireRock);
}

// Effects for when a mine is fired from the player paddle.
void GameESPAssets::AddPaddleMineFiredEffects(const GameModel& gameModel, 
                                              const PaddleMineProjectile& projectile) {

    const Point2D& projectilePos2D  = projectile.GetPosition();
    const Vector2D& projectileDir2D = projectile.GetVelocityDirection();
    PlayerPaddle* paddle = gameModel.GetPlayerPaddle();

	// We only do the mine onomatopiea if we aren't in a special camera mode...
	if (!paddle->GetIsPaddleCameraOn() && !GameBall::GetIsBallCameraOn()) {
		
        // Create mine launch onomatopeia
		ESPPointEmitter* launchOnoEffect = new ESPPointEmitter();
		// Set up the emitter...
		launchOnoEffect->SetSpawnDelta(ESPInterval(-1));
		launchOnoEffect->SetInitialSpd(ESPInterval(0.5f, 1.5f));
		launchOnoEffect->SetParticleLife(ESPInterval(0.75f, 1.25f));
		launchOnoEffect->SetParticleSize(ESPInterval(0.4f, 0.7f));
		launchOnoEffect->SetParticleRotation(ESPInterval(-20.0f, 20.0f));
		launchOnoEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
		launchOnoEffect->SetParticleAlignment(ESP::ScreenAligned);
		launchOnoEffect->SetEmitPosition(Point3D(projectilePos2D));
		launchOnoEffect->SetEmitDirection(Vector3D(projectileDir2D));
		launchOnoEffect->SetEmitAngleInDegrees(45);
		launchOnoEffect->SetParticleColour(ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
		
		// Add effectors...
		launchOnoEffect->AddEffector(&this->particleFader);
		launchOnoEffect->AddEffector(&this->particleSmallGrowth);

		// Add the single particle to the emitter...
		DropShadow dpTemp;
		dpTemp.colour = Colour(0,0,0);
		dpTemp.amountPercentage = 0.10f;
		ESPOnomataParticle* launchOnoParticle = 
            new ESPOnomataParticle(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, 
                                                                                 GameFontAssetsManager::Small));
		launchOnoParticle->SetDropShadow(dpTemp);
		launchOnoParticle->SetOnomatoplexSound(Onomatoplex::LAUNCH, Onomatoplex::GOOD);
		launchOnoEffect->AddParticle(launchOnoParticle);
		
		this->activeGeneralEmitters.push_back(launchOnoEffect);
    }

	// Create a dispertion of sparks coming out of the paddle
	ESPPointEmitter* particleSparks = new ESPPointEmitter();
	particleSparks->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	particleSparks->SetInitialSpd(ESPInterval(3.0f, 5.25f));
	particleSparks->SetParticleLife(ESPInterval(0.25f, 0.55f));
    particleSparks->SetParticleSize(ESPInterval(MineProjectile::WIDTH_DEFAULT / 4.0f, MineProjectile::WIDTH_DEFAULT / 3.0f));
	particleSparks->SetParticleColour(ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	particleSparks->SetEmitAngleInDegrees(25);
    particleSparks->SetParticleAlignment(ESP::ScreenAligned);
    particleSparks->SetEmitPosition(Point3D(0, 0, 0));
    particleSparks->SetEmitDirection(Vector3D(paddle->GetUpVector()));
	particleSparks->AddEffector(&this->particleFader);
	particleSparks->SetParticles(15, this->circleGradientTex);    
    
    this->activePaddleEmitters.push_back(particleSparks);
}

void GameESPAssets::AddPaddleMineAttachedEffects(const Projectile& projectile) {

    const Point2D& projectilePos2D  = projectile.GetPosition();

    // Create mine launch onomatopeia
	ESPPointEmitter* attachOnoEffect = new ESPPointEmitter();
	// Set up the emitter...
	attachOnoEffect->SetSpawnDelta(ESPInterval(-1));
	attachOnoEffect->SetInitialSpd(ESPInterval(0.0f));
	attachOnoEffect->SetParticleLife(ESPInterval(0.75f, 1.1f));
	attachOnoEffect->SetParticleSize(ESPInterval(0.4f, 0.7f));
	attachOnoEffect->SetParticleRotation(ESPInterval(-20.0f, 20.0f));
	attachOnoEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	attachOnoEffect->SetParticleAlignment(ESP::ScreenAligned);
	attachOnoEffect->SetEmitPosition(Point3D(projectilePos2D));
	attachOnoEffect->SetEmitAngleInDegrees(45);
	attachOnoEffect->SetParticleColour(ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	
	// Add effectors...
	attachOnoEffect->AddEffector(&this->particleFader);
	attachOnoEffect->AddEffector(&this->particleSmallGrowth);

	// Add the single particle to the emitter...
	DropShadow dpTemp;
	dpTemp.colour = Colour(0,0,0);
	dpTemp.amountPercentage = 0.10f;
	ESPOnomataParticle* attachOnoParticle = 
        new ESPOnomataParticle(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, 
                                                                             GameFontAssetsManager::Small));
	attachOnoParticle->SetDropShadow(dpTemp);
	attachOnoParticle->SetOnomatoplexSound(Onomatoplex::ATTACH, Onomatoplex::GOOD);
	attachOnoEffect->AddParticle(attachOnoParticle);
	
	this->activeGeneralEmitters.push_back(attachOnoEffect);
}

/**
 * Private helper function for making and adding a laser blast effect for the laser paddle item.
 */
void GameESPAssets::AddLaserPaddleESPEffects(const GameModel& gameModel, const Projectile& projectile) {
	assert(projectile.GetType() == Projectile::PaddleLaserBulletProjectile);
	
	const Point2D& projectilePos2D  = projectile.GetPosition();
	Point3D projectilePos3D         = Point3D(projectilePos2D[0], projectilePos2D[1], 0.0f);
	const Vector2D& projectileDir   = projectile.GetVelocityDirection();
	Vector3D projectileDir3D        = Vector3D(projectileDir[0], projectileDir[1], 0.0f);
	PlayerPaddle* paddle = gameModel.GetPlayerPaddle();

	// We only do the laser onomatopiea if we aren't in a special camera mode...
	if (!paddle->GetIsPaddleCameraOn() && !GameBall::GetIsBallCameraOn()) {
		// Create laser onomatopeia
		ESPPointEmitter* laserOnoEffect = new ESPPointEmitter();
		// Set up the emitter...
		laserOnoEffect->SetSpawnDelta(ESPInterval(-1));
		laserOnoEffect->SetInitialSpd(ESPInterval(0.5f, 1.5f));
		laserOnoEffect->SetParticleLife(ESPInterval(0.75f, 1.25f));
		laserOnoEffect->SetParticleSize(ESPInterval(0.4f, 0.7f));
		laserOnoEffect->SetParticleRotation(ESPInterval(-20.0f, 20.0f));
		laserOnoEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
		laserOnoEffect->SetParticleAlignment(ESP::ScreenAligned);
		laserOnoEffect->SetEmitPosition(projectilePos3D);
		laserOnoEffect->SetEmitDirection(projectileDir3D);
		laserOnoEffect->SetEmitAngleInDegrees(45);
		laserOnoEffect->SetParticleColour(ESPInterval(0.25f, 1.0f), ESPInterval(0.6f, 1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
		
		// Add effectors...
		laserOnoEffect->AddEffector(&this->particleFader);
		laserOnoEffect->AddEffector(&this->particleSmallGrowth);

		// Add the single particle to the emitter...
		DropShadow dpTemp;
		dpTemp.colour = Colour(0,0,0);
		dpTemp.amountPercentage = 0.10f;
		ESPOnomataParticle* laserOnoParticle = 
            new ESPOnomataParticle(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ElectricZap, 
                                                                                 GameFontAssetsManager::Small));
		laserOnoParticle->SetDropShadow(dpTemp);
		laserOnoParticle->SetOnomatoplexSound(Onomatoplex::SHOT, Onomatoplex::GOOD);
		laserOnoEffect->AddParticle(laserOnoParticle);
		
		this->activeGeneralEmitters.push_back(laserOnoEffect);
	}

    this->AddLaserESPEffects(gameModel, projectile, Colour(0.5f, 1.0f, 1.0f), Colour(0.8f, 1.0f, 1.0f));
}

void GameESPAssets::AddLaserBallESPEffects(const GameModel& gameModel, const Projectile& projectile) {
    this->AddLaserESPEffects(gameModel, projectile, Colour(0.5f, 1.0f, 1.0f), Colour(0.8f, 1.0f, 1.0f));
}

void GameESPAssets::AddLaserTurretESPEffects(const GameModel& gameModel, const Projectile& projectile) {
    // TODO: Add onomatopeia?
    this->AddLaserESPEffects(gameModel, projectile, Colour(0.25f, 1.0f, 0.75f), Colour(0.3f, 1.0f, 1.0f));
}

void GameESPAssets::AddLaserESPEffects(const GameModel& gameModel, const Projectile& projectile,
                                       const Colour& baseColour, const Colour& brightColour) {

	const Point2D& projectilePos2D  = projectile.GetPosition();
	Point3D projectilePos3D         = Point3D(projectilePos2D[0], projectilePos2D[1], 0.0f);
	float projectileSpd             = projectile.GetVelocityMagnitude();
	const Vector2D& projectileDir   = projectile.GetVelocityDirection();
	Vector3D projectileDir3D        = Vector3D(projectileDir[0], projectileDir[1], 0.0f);

	PlayerPaddle* paddle = gameModel.GetPlayerPaddle();

	// Create the basic laser beam
	ESPPointEmitter* laserBeamEmitter = new ESPPointEmitter();
	laserBeamEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	laserBeamEmitter->SetInitialSpd(ESPInterval(0));
	laserBeamEmitter->SetParticleLife(ESPInterval(-1));
	if (paddle->GetIsPaddleCameraOn() || GameBall::GetIsBallCameraOn()) {
		laserBeamEmitter->SetParticleSize(ESPInterval(std::min<float>(projectile.GetWidth(), projectile.GetHeight())));
	}
	else {
		laserBeamEmitter->SetParticleSize(ESPInterval(projectile.GetWidth()), ESPInterval(projectile.GetHeight()));
	}
	laserBeamEmitter->SetEmitAngleInDegrees(0);
	laserBeamEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	laserBeamEmitter->SetParticleAlignment(ESP::ScreenAligned);
	laserBeamEmitter->SetEmitPosition(Point3D(0,0,0));
    laserBeamEmitter->SetParticleColour(ESPInterval(baseColour.R()),
        ESPInterval(baseColour.G()), ESPInterval(baseColour.B()), ESPInterval(1.0f));
	laserBeamEmitter->SetParticles(1, this->laserBeamTex);

	// Create the slightly-pulsing-glowing aura
	ESPPointEmitter* laserAuraEmitter = new ESPPointEmitter();
	laserAuraEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	laserAuraEmitter->SetInitialSpd(ESPInterval(0));
	laserAuraEmitter->SetParticleLife(ESPInterval(-1));
	if (paddle->GetIsPaddleCameraOn() || GameBall::GetIsBallCameraOn()) {
		laserAuraEmitter->SetParticleSize(ESPInterval(std::min<float>(2*projectile.GetWidth(), 1.8f*projectile.GetHeight())));
	}
	else {
		laserAuraEmitter->SetParticleSize(ESPInterval(2*projectile.GetWidth()), ESPInterval(1.8f*projectile.GetHeight()));
	}
	
	laserAuraEmitter->SetEmitAngleInDegrees(0);
	laserAuraEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	laserAuraEmitter->SetParticleAlignment(ESP::ScreenAligned);
	laserAuraEmitter->SetEmitPosition(Point3D(0,0,0));
	laserAuraEmitter->SetParticleColour(ESPInterval(1.3f * baseColour.R()), ESPInterval(1.3f * baseColour.G()),
        ESPInterval(1.3f * baseColour.B()), ESPInterval(1.0f));
	laserAuraEmitter->AddEffector(&this->particlePulsePaddleLaser);
	laserAuraEmitter->SetParticles(1, this->circleGradientTex);

	// Create the trail effects
	ESPPointEmitter* laserTrailSparks = new ESPPointEmitter();
	laserTrailSparks->SetSpawnDelta(ESPInterval(0.01f, 0.033f));
	laserTrailSparks->SetInitialSpd(ESPInterval(projectileSpd));
	laserTrailSparks->SetParticleLife(ESPInterval(0.5f, 0.6f));
	laserTrailSparks->SetParticleSize(ESPInterval(0.8f * projectile.GetHalfWidth(), 0.8f * projectile.GetWidth()));
    laserTrailSparks->SetParticleColour(ESPInterval(baseColour.R(), brightColour.R()), ESPInterval(baseColour.G(), brightColour.G()),
        ESPInterval(baseColour.B(), brightColour.B()), ESPInterval(1.0f));
	laserTrailSparks->SetEmitAngleInDegrees(15);
	laserTrailSparks->SetEmitDirection(Vector3D(-projectileDir[0], -projectileDir[1], 0.0f));
	laserTrailSparks->SetRadiusDeviationFromCenter(ESPInterval(0.5f * projectile.GetHalfWidth()));
	laserTrailSparks->SetAsPointSpriteEmitter(true);
	laserTrailSparks->SetEmitPosition(projectilePos3D);
	laserTrailSparks->AddEffector(&this->particleFader);
	laserTrailSparks->AddEffector(&this->particleMediumShrink);
	laserTrailSparks->SetParticles(10, this->circleGradientTex);

	this->activeProjectileEmitters[&projectile].push_back(laserAuraEmitter);
	this->activeProjectileEmitters[&projectile].push_back(laserBeamEmitter);
	this->activeProjectileEmitters[&projectile].push_back(laserTrailSparks);
}

/**
 * Add the proper particle effects for when a laser hits a prism block
 * (big shiny reflection effect with partial lens flare).
 */
void GameESPAssets::AddLaserHitPrismBlockEffect(const Point2D& loc) {

	// Create a short-lived shinny glow effect
	ESPPointEmitter* shinyGlowEmitter = new ESPPointEmitter();
	shinyGlowEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	shinyGlowEmitter->SetInitialSpd(ESPInterval(0));
	shinyGlowEmitter->SetParticleLife(ESPInterval(0.5f));
	shinyGlowEmitter->SetParticleSize(ESPInterval(LevelPiece::PIECE_WIDTH));
	shinyGlowEmitter->SetEmitAngleInDegrees(0);
	shinyGlowEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	shinyGlowEmitter->SetParticleAlignment(ESP::ScreenAligned);
	shinyGlowEmitter->SetEmitPosition(Point3D(loc[0], loc[1], 0.0f));
	shinyGlowEmitter->SetParticleColour(ESPInterval(0.75f, 0.9f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	shinyGlowEmitter->AddEffector(&this->particleMediumShrink);
	shinyGlowEmitter->AddEffector(Randomizer::GetInstance()->RandomNegativeOrPositive() < 0 ? &this->smokeRotatorCW : &this->smokeRotatorCCW);
	shinyGlowEmitter->SetParticles(1, this->circleGradientTex);

	// Create a brief lens-flare effect
	ESPPointEmitter* lensFlareEmitter = new ESPPointEmitter();
	lensFlareEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	lensFlareEmitter->SetInitialSpd(ESPInterval(0));
	lensFlareEmitter->SetParticleLife(ESPInterval(1.0f));
	lensFlareEmitter->SetParticleSize(ESPInterval(2*LevelPiece::PIECE_WIDTH));
	lensFlareEmitter->SetEmitAngleInDegrees(0);
	lensFlareEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	lensFlareEmitter->SetParticleAlignment(ESP::ScreenAligned);
	lensFlareEmitter->SetEmitPosition(Point3D(loc[0], loc[1], 0.0f));
	lensFlareEmitter->SetParticleColour(ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	lensFlareEmitter->AddEffector(&this->particleFader);
	lensFlareEmitter->AddEffector(&this->particleLargeGrowth);
	lensFlareEmitter->AddEffector(Randomizer::GetInstance()->RandomNegativeOrPositive() < 0 ? &this->smokeRotatorCW : &this->smokeRotatorCCW);
	lensFlareEmitter->SetParticles(1, this->lensFlareTex);

	this->activeGeneralEmitters.push_back(lensFlareEmitter);
	this->activeGeneralEmitters.push_back(shinyGlowEmitter);
}

/**
 * Adds an effect for when a laser bullet hits a wall and disintegrates.
 */
void GameESPAssets::AddLaserHitWallEffect(const Point2D& loc) {
	const Point3D EMITTER_LOCATION = Point3D(loc[0], loc[1], 0.0f);

	// Create a VERY brief lens-flare effect
	ESPPointEmitter* lensFlareEmitter = new ESPPointEmitter();
	lensFlareEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	lensFlareEmitter->SetInitialSpd(ESPInterval(0));
	lensFlareEmitter->SetParticleLife(ESPInterval(0.9f));
	lensFlareEmitter->SetParticleSize(ESPInterval(LevelPiece::PIECE_WIDTH));
	lensFlareEmitter->SetEmitAngleInDegrees(0);
	lensFlareEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	lensFlareEmitter->SetParticleAlignment(ESP::ScreenAligned);
	lensFlareEmitter->SetEmitPosition(EMITTER_LOCATION);
	lensFlareEmitter->SetParticleColour(ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	lensFlareEmitter->AddEffector(&this->particleFader);
	lensFlareEmitter->AddEffector(&this->particleMediumGrowth);
	lensFlareEmitter->AddEffector(Randomizer::GetInstance()->RandomNegativeOrPositive() < 0 ? &this->smokeRotatorCW : &this->smokeRotatorCCW);
	lensFlareEmitter->SetParticles(1, this->lensFlareTex);

	// Create a dispertion of particle bits
	ESPPointEmitter* particleSparks = new ESPPointEmitter();
	particleSparks->SetSpawnDelta(ESPInterval(0.01f, 0.02f));
	particleSparks->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	particleSparks->SetInitialSpd(ESPInterval(1.5f, 4.0f));
	particleSparks->SetParticleLife(ESPInterval(0.5f, 0.75f));
	particleSparks->SetParticleSize(ESPInterval(LevelPiece::PIECE_WIDTH / 10.0f, LevelPiece::PIECE_WIDTH / 8.0f));
	particleSparks->SetParticleColour(ESPInterval(0.8f, 1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	particleSparks->SetEmitAngleInDegrees(180);
	particleSparks->SetAsPointSpriteEmitter(true);
	particleSparks->SetEmitPosition(EMITTER_LOCATION);
	particleSparks->AddEffector(&this->particleFader);
	particleSparks->AddEffector(&this->particleMediumGrowth);
	particleSparks->SetParticles(15, this->circleGradientTex);

	this->activeGeneralEmitters.push_back(lensFlareEmitter);
	this->activeGeneralEmitters.push_back(particleSparks);
}

void GameESPAssets::AddMultiplierComboEffect(int multiplier, const Point2D& position) {
    
	// The effect is a basic onomatopeia multiplier word e.g., "Combo (2x)!"
    std::stringstream comboStrStream;
    comboStrStream << "Combo (" << multiplier << "x)!";

	ESPPointEmitter* comboEffect = new ESPPointEmitter();
	comboEffect->SetSpawnDelta(ESPInterval(-1, -1));
	comboEffect->SetParticleLife(ESPInterval(2.25f));
	comboEffect->SetParticleSize(ESPInterval(0.5f, 0.75f));
	comboEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.0f));
	comboEffect->SetParticleAlignment(ESP::ScreenAligned);
	comboEffect->SetEmitPosition(Point3D(position));

    comboEffect->AddEffector(&this->particleLargeGrowth);
    this->flashColourFader.SetEndColour(GameViewConstants::GetInstance()->GetMultiplierColour(multiplier));
    comboEffect->AddEffector(&this->flashColourFader);

	// Add the single particle to the emitter
	DropShadow dpTemp;
	dpTemp.amountPercentage = 0.10f;
    dpTemp.colour = Colour(0, 0, 0);

	ESPOnomataParticle* textParticle = new ESPOnomataParticle(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small),
        comboStrStream.str());
	textParticle->SetDropShadow(dpTemp);
    comboEffect->AddParticle(textParticle);

    this->activeGeneralEmitters.push_back(comboEffect);
}

// This is currently not in use...
//void GameESPAssets::AddPointAwardEffect(const PointAward& pointAward, const PlayerPaddle& paddle) {
//    std::string awardName(PointsHUD::GetPointNotificationName(pointAward));
//
//	// The effect is a basic onomatopeia point score with optional name (e.g., "Paddle Daredevil +1000pts!")
//    std::stringstream ptStrStream;
//    if (!awardName.empty()) {
//        ptStrStream << awardName << " ";
//    }
//    if (pointAward.GetTotalPointAmount() > 0) {
//        ptStrStream << "+";
//    }
//    ptStrStream << pointAward.GetTotalPointAmount() << "pts!";
//
//	DropShadow dpTemp;
//	dpTemp.amountPercentage = 0.05f;
//    dpTemp.colour = Colour(0, 0, 0);
//
//	ESPOnomataParticle* textParticle = new ESPOnomataParticle(
//        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small),
//        ptStrStream.str());
//	textParticle->SetDropShadow(dpTemp);
//
//	ESPPointEmitter* ptTextEffect = new ESPPointEmitter();
//	ptTextEffect->SetSpawnDelta(ESPInterval(-1, -1));
//	ptTextEffect->SetParticleLife(ESPInterval(1.5f));
//	ptTextEffect->SetParticleSize(ESPInterval(0.75f));
//	ptTextEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.0f));
//	ptTextEffect->SetParticleAlignment(ESP::ScreenAligned);
//    ptTextEffect->SetEmitPosition(Point3D(paddle.GetCenterPosition() - Vector2D(0, 2.0f * paddle.GetHalfHeight())));
//
//    this->flashColourFader.SetEndColour(GameViewConstants::GetInstance()->GetMultiplierColour(pointAward.GetMultiplierAmount()));
//    ptTextEffect->AddEffector(&this->flashColourFader);
//    ptTextEffect->AddEffector(&this->particleLargeGrowth);
//    ptTextEffect->AddParticle(textParticle);
//
//    this->activeGeneralEmitters.push_back(ptTextEffect);
//}

void GameESPAssets::AddBallBoostEffect(const BallBoostModel& boostModel) {
    bool result = false;
    const Vector2D& boostDir = boostModel.GetBallBoostDirection();

    // Add a boost effect for every ball that may have been boosted by the boost model...
    const std::list<GameBall*>& boostedBalls =  boostModel.GetBalls();
    for (std::list<GameBall*>::const_iterator iter = boostedBalls.begin(); iter != boostedBalls.end(); ++iter) {
        const GameBall* currGameBall = *iter;
        float ballRadius     = currGameBall->GetBounds().Radius();
        Point3D ballPosition = currGameBall->GetCenterPosition();
        Vector3D emitDir(-boostDir);
        //Point2D boostPos = currGameBall->GetCenterPosition2D() - ballRadius * boostDir;

	    ESPPointEmitter* boostSparkles = new ESPPointEmitter();
	    boostSparkles->SetNumParticleLives(1);
	    boostSparkles->SetSpawnDelta(ESPInterval(0.005f, 0.008f));
	    boostSparkles->SetInitialSpd(ESPInterval(2.0f, 3.0f));
	    boostSparkles->SetParticleLife(ESPInterval(0.9f, 2.0f));
        boostSparkles->SetParticleSize(ESPInterval(ballRadius * 2.0f, ballRadius * 3.0f));
	    boostSparkles->SetParticleColour(ESPInterval(0.75f, 1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	    boostSparkles->SetEmitAngleInDegrees(17);
	    boostSparkles->SetEmitDirection(emitDir);
	    boostSparkles->SetAsPointSpriteEmitter(true);
        boostSparkles->SetEmitPosition(ballPosition);
	    boostSparkles->AddEffector(&this->particleFader);
        boostSparkles->AddEffector(&this->particleMediumShrink);
	    result = boostSparkles->SetParticles(20, this->sparkleTex);
	    assert(result);

	    ESPPointEmitter* glowEmitterTrail = new ESPPointEmitter();
        glowEmitterTrail->SetNumParticleLives(1);
	    glowEmitterTrail->SetSpawnDelta(ESPInterval(0.00575f));
	    glowEmitterTrail->SetInitialSpd(ESPInterval(0.0f));
	    glowEmitterTrail->SetParticleLife(ESPInterval(ballRadius*2.5f));
	    glowEmitterTrail->SetParticleSize(ESPInterval(1.3f), ESPInterval(1.3f));
	    glowEmitterTrail->SetEmitAngleInDegrees(0);
	    glowEmitterTrail->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	    glowEmitterTrail->SetParticleAlignment(ESP::ScreenAligned);
	    glowEmitterTrail->SetEmitPosition(Point3D(0, 0, 0));
        glowEmitterTrail->SetParticleColour(ESPInterval(0.70f, 1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
        glowEmitterTrail->AddEffector(&this->particleBoostFader);
	    glowEmitterTrail->AddEffector(&this->particleShrinkToNothing);
	    result = glowEmitterTrail->SetParticles(30, this->circleGradientTex);
	    assert(result);

        ESPPointEmitter* vapourTrailEffect = new ESPPointEmitter();
        vapourTrailEffect->SetNumParticleLives(1);
	    vapourTrailEffect->SetSpawnDelta(ESPInterval(0.0235f));
	    vapourTrailEffect->SetInitialSpd(ESPInterval(3.0f));
	    vapourTrailEffect->SetParticleLife(ESPInterval(1.0f));
	    vapourTrailEffect->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	    vapourTrailEffect->SetEmitPosition(ballPosition);
        vapourTrailEffect->SetEmitDirection(emitDir);
	    vapourTrailEffect->SetParticleSize(ESPInterval(ballRadius*2.5f));
	    vapourTrailEffect->SetParticleColour(ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
        vapourTrailEffect->SetParticleAlignment(ESP::ScreenAlignedFollowVelocity);
        vapourTrailEffect->SetToggleEmitOnPlane(true);
	    vapourTrailEffect->AddEffector(&this->particleFader);
        vapourTrailEffect->AddEffector(&this->particleLargeGrowth);
        result = vapourTrailEffect->SetParticles(6, &this->vapourTrailRefractEffect);
	    assert(result);

        this->boostBallEmitters[currGameBall].push_back(vapourTrailEffect);
        this->boostBallEmitters[currGameBall].push_back(glowEmitterTrail);
        this->boostBallEmitters[currGameBall].push_back(boostSparkles);
    }
}

void GameESPAssets::AddBallAcquiredBoostEffect(const GameBall& ball, const Colour& colour) {
    // Create the emitters/effects for when boosts are gained
    static const float TOTAL_HALO_LIFE  = 0.8f;
    static const int NUM_HALO_PARTICLES = 3;

    ESPPointEmitter* boostGainedHaloEmitter = new ESPPointEmitter();
    boostGainedHaloEmitter->SetSpawnDelta(ESPInterval(TOTAL_HALO_LIFE / static_cast<float>(NUM_HALO_PARTICLES)));
	boostGainedHaloEmitter->SetInitialSpd(ESPInterval(0));
	boostGainedHaloEmitter->SetParticleLife(ESPInterval(TOTAL_HALO_LIFE));
    boostGainedHaloEmitter->SetNumParticleLives(1);
    boostGainedHaloEmitter->SetParticleSize(ESPInterval(2.5f * ball.GetBounds().Radius()));
	boostGainedHaloEmitter->SetEmitAngleInDegrees(0);
	boostGainedHaloEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    boostGainedHaloEmitter->SetParticleAlignment(ESP::ScreenAligned);
	boostGainedHaloEmitter->SetEmitPosition(Point3D(0,0,0));
    boostGainedHaloEmitter->SetParticleColour(ESPInterval(colour.R()), ESPInterval(colour.G()), ESPInterval(colour.B()), ESPInterval(1.0f));
    boostGainedHaloEmitter->AddEffector(&this->particleSuperGrowth);
	boostGainedHaloEmitter->AddEffector(&this->particleFader);
    bool result = boostGainedHaloEmitter->SetParticles(NUM_HALO_PARTICLES, this->haloTex);
    UNUSED_VARIABLE(result);
	assert(result);

	ESPPointEmitter* sinkParticles = new ESPPointEmitter();
	sinkParticles->SetSpawnDelta(ESPInterval(0.025f, 0.05f));
	sinkParticles->SetNumParticleLives(1);
	sinkParticles->SetInitialSpd(ESPInterval(2.6f, 4.5f));
	sinkParticles->SetParticleLife(ESPInterval(1.0f, 1.25f));
	sinkParticles->SetParticleSize(ESPInterval(ESPInterval(2.25f * ball.GetBounds().Radius(), 3.5f * ball.GetBounds().Radius())));
	sinkParticles->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.75f * ball.GetBounds().Radius()));
    sinkParticles->SetParticleAlignment(ESP::ScreenAligned);
	sinkParticles->SetEmitPosition(Point3D(0,0,0));
	sinkParticles->SetEmitDirection(Vector3D(0, 1, 0));
	sinkParticles->SetEmitAngleInDegrees(180);
	sinkParticles->SetIsReversed(true);
    sinkParticles->SetParticleColour(ESPInterval(colour.R(), 1.0f), ESPInterval(colour.G(), 1.0f) , 
                                     ESPInterval(colour.B()), ESPInterval(1.0f));
	sinkParticles->AddEffector(&this->particleFader);
	sinkParticles->AddEffector(&this->particleMediumShrink);    
    sinkParticles->SetParticles(15, this->sparkleTex);

    this->activeBallBGEmitters[&ball].push_back(boostGainedHaloEmitter);
    this->activeBallBGEmitters[&ball].push_back(sinkParticles);
}

// Add the effect for when the rocket goes off after it hits a block
void GameESPAssets::AddRocketBlastEffect(float rocketSizeFactor, const Point2D& loc) {
	ESPInterval bangLifeInterval	= ESPInterval(1.2f);
	ESPInterval bangOnoLifeInterval	= ESPInterval(bangLifeInterval.maxValue + 0.4f);
	Point3D emitCenter(loc);

	// Create an emitter for the bang texture
	ESPPointEmitter* explosionEffect = new ESPPointEmitter();
	
	// Set up the emitter...
	explosionEffect->SetSpawnDelta(ESPInterval(-1));
	explosionEffect->SetInitialSpd(ESPInterval(0.0f, 0.0f));
	explosionEffect->SetParticleLife(bangLifeInterval);
	explosionEffect->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	explosionEffect->SetParticleAlignment(ESP::ScreenAligned);
	explosionEffect->SetEmitPosition(emitCenter);

	// Figure out some random proper orientation...
	explosionEffect->SetParticleRotation(ESPInterval(-25.0f, 25.0f));

	ESPInterval sizeIntervalX(rocketSizeFactor * 4 * LevelPiece::PIECE_WIDTH);
	ESPInterval sizeIntervalY(rocketSizeFactor * 5.5 * LevelPiece::PIECE_HEIGHT);
	explosionEffect->SetParticleSize(sizeIntervalX, sizeIntervalY);

	// Add effectors to the bang effect
	explosionEffect->AddEffector(&this->particleFader);
	explosionEffect->AddEffector(&this->particleMediumGrowth);
	
	// Add the explosion particle...
	bool result = explosionEffect->SetParticles(1, this->hugeExplosionTex);
    UNUSED_VARIABLE(result);
	assert(result);

	// Explosion rays...
	ESPPointEmitter* explodeRayEffect = new ESPPointEmitter();
	explodeRayEffect->SetSpawnDelta(ESPInterval(-1.0f));
	explodeRayEffect->SetInitialSpd(ESPInterval(0.0f));
	explodeRayEffect->SetParticleLife(ESPInterval(1.0f));
	explodeRayEffect->SetParticleSize(sizeIntervalX);
	explodeRayEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	explodeRayEffect->SetParticleAlignment(ESP::ScreenAligned);
	explodeRayEffect->SetEmitPosition(emitCenter);
	explodeRayEffect->SetParticles(1, this->explosionRayTex);
	if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
		explodeRayEffect->AddEffector(&this->explosionRayRotatorCW);
	}
	else {
		explodeRayEffect->AddEffector(&this->explosionRayRotatorCCW);
	}
	explodeRayEffect->AddEffector(&this->particleLargeGrowth);
	explodeRayEffect->AddEffector(&this->particleFader);

	// Add debris bits
	ESPPointEmitter* debrisBits = new ESPPointEmitter();
	debrisBits->SetSpawnDelta(ESPInterval(-1));
	debrisBits->SetInitialSpd(ESPInterval(4.5f, 6.0f));
	debrisBits->SetParticleLife(ESPInterval(2.5f, 3.5f));
	debrisBits->SetParticleSize(ESPInterval(rocketSizeFactor * LevelPiece::PIECE_WIDTH / 6.0f, rocketSizeFactor * LevelPiece::PIECE_WIDTH / 5.0f));
	debrisBits->SetParticleColour(ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(0.2f), ESPInterval(1.0f));
	debrisBits->SetEmitAngleInDegrees(180);
	debrisBits->SetAsPointSpriteEmitter(true);
	debrisBits->SetEmitPosition(emitCenter);
	debrisBits->AddEffector(&this->particleFader);
	debrisBits->AddEffector(&this->particleMediumGrowth);
	debrisBits->AddEffector(&this->gravity);
	debrisBits->SetParticles(15, this->circleGradientTex);

	// Create an emitter for the sound of onomatopeia of the breaking block
	ESPPointEmitter* bangOnoEffect = new ESPPointEmitter();
	// Set up the emitter...
	bangOnoEffect->SetSpawnDelta(ESPInterval(-1));
	bangOnoEffect->SetInitialSpd(ESPInterval(0.0f));
	bangOnoEffect->SetParticleLife(bangOnoLifeInterval);
	bangOnoEffect->SetParticleSize(ESPInterval(rocketSizeFactor * 1.0f));
	bangOnoEffect->SetParticleRotation(ESPInterval(-20.0f, 20.0f));
	bangOnoEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.2f));
	bangOnoEffect->SetParticleAlignment(ESP::ScreenAligned);
	bangOnoEffect->SetEmitPosition(emitCenter);
	
	// Add effectors...
	bangOnoEffect->AddEffector(&this->particleFader);
	bangOnoEffect->AddEffector(&this->particleSmallGrowth);

	// Add the single text particle to the emitter with the severity of the effect...
	TextLabel2D bangTextLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Big), "");
	bangTextLabel.SetColour(Colour(1, 1, 1));
	bangTextLabel.SetDropShadow(Colour(0, 0, 0), 0.1f);

    Onomatoplex::Extremeness severity;
    if (rocketSizeFactor >= 1) {
	    severity = Onomatoplex::UBER;
    }
    else if (rocketSizeFactor > 0.6f) {
        severity = Onomatoplex::AWESOME;
    }
    else {
        severity = Onomatoplex::GOOD;
    }

	Onomatoplex::SoundType type = Onomatoplex::EXPLOSION;

	bangOnoEffect->SetParticles(1, bangTextLabel, type, severity);

	// Add shockwave
	ESPPointEmitter* shockwave = this->CreateShockwaveEffect(emitCenter, sizeIntervalX.maxValue, 1.0f);
	assert(shockwave != NULL);

	// Lastly, add the new emitters to the list of active emitters in order of back to front
	this->activeGeneralEmitters.push_back(shockwave);
	this->activeGeneralEmitters.push_back(debrisBits);
	this->activeGeneralEmitters.push_back(this->CreateBlockBreakSmashyBits(emitCenter, ESPInterval(0.6f, 1.0f), 
																				ESPInterval(0.5f, 1.0f), ESPInterval(0.0f, 0.0f), false, 20));
	this->activeGeneralEmitters.push_back(explosionEffect);
	this->activeGeneralEmitters.push_back(explodeRayEffect);
	this->activeGeneralEmitters.push_back(bangOnoEffect);
}

void GameESPAssets::AddMineBlastEffect(const MineProjectile& mine, const Point2D& loc) {
	ESPInterval bangLifeInterval	= ESPInterval(1.0f);
	ESPInterval bangOnoLifeInterval	= ESPInterval(bangLifeInterval.maxValue + 0.4f);
	Point3D emitCenter(loc);

	// Create an emitter for the bang texture
	ESPPointEmitter* explosionEffect = new ESPPointEmitter();
	
	// Set up the emitter...
	explosionEffect->SetSpawnDelta(ESPInterval(-1));
	explosionEffect->SetInitialSpd(ESPInterval(0.0f, 0.0f));
	explosionEffect->SetParticleLife(bangLifeInterval);
	explosionEffect->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	explosionEffect->SetParticleAlignment(ESP::ScreenAligned);
	explosionEffect->SetEmitPosition(emitCenter);

	// Figure out some random proper orientation...
	explosionEffect->SetParticleRotation(ESPInterval(-180.0f, 179.9999999f));

    float mineSizeFactor = mine.GetVisualScaleFactor();
    float size = 3*mine.GetExplosionRadius(); // We make the size a bit bigger than it should be to make it more noticable
	explosionEffect->SetParticleSize(ESPInterval(size));

	// Add effectors to the bang effect
	explosionEffect->AddEffector(&this->particleFader);
	explosionEffect->AddEffector(&this->particleMediumGrowth);
	
	// Add the explosion particle...
	bool result = explosionEffect->SetParticles(1, this->bubblyExplosionTex);
    UNUSED_VARIABLE(result);
	assert(result);

	// Create an emitter for the sound of onomatopeia of the breaking block
	ESPPointEmitter* bangOnoEffect = new ESPPointEmitter();
	// Set up the emitter...
	bangOnoEffect->SetSpawnDelta(ESPInterval(-1));
	bangOnoEffect->SetInitialSpd(ESPInterval(0.0f));
	bangOnoEffect->SetParticleLife(bangOnoLifeInterval);
	bangOnoEffect->SetParticleSize(ESPInterval(mineSizeFactor));
	bangOnoEffect->SetParticleRotation(ESPInterval(-20.0f, 20.0f));
	bangOnoEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.2f));
	bangOnoEffect->SetParticleAlignment(ESP::ScreenAligned);
	bangOnoEffect->SetEmitPosition(emitCenter);
	
	// Add effectors...
	bangOnoEffect->AddEffector(&this->particleFader);
	bangOnoEffect->AddEffector(&this->particleSmallGrowth);

	// Add the single text particle to the emitter with the severity of the effect...
    TextLabel2D bangTextLabel(GameFontAssetsManager::GetInstance()->GetFont(
        GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium), "");
	bangTextLabel.SetColour(Colour(1, 1, 1));
	bangTextLabel.SetDropShadow(Colour(0, 0, 0), 0.075f);

    Onomatoplex::Extremeness severity;
    if (mineSizeFactor >= 1) {
        severity = Onomatoplex::GOOD;
    }
    else if (mineSizeFactor > 0.6f) {
        severity = Onomatoplex::PRETTY_GOOD;
    }
    else {
        severity = Onomatoplex::NORMAL;
    }

	Onomatoplex::SoundType type = Onomatoplex::EXPLOSION;

	bangOnoEffect->SetParticles(1, bangTextLabel, type, severity);

	// Add shockwave
	ESPPointEmitter* shockwave = this->CreateShockwaveEffect(emitCenter, size, 0.75f);
	assert(shockwave != NULL);

	// Lastly, add the new emitters to the list of active emitters in order of back to front
	this->activeGeneralEmitters.push_back(shockwave);
	this->activeGeneralEmitters.push_back(explosionEffect);
	this->activeGeneralEmitters.push_back(bangOnoEffect);
}

void GameESPAssets::AddEnergyShieldHitEffect(const Point2D& shieldCenter, const GameBall& ball) {
	Vector2D emitDirection = ball.GetCenterPosition2D() - shieldCenter;
	Point3D emitPosition3D(ball.GetCenterPosition());

	// Add energy bits
	ESPPointEmitter* sparkleEnergyBits = new ESPPointEmitter();
	sparkleEnergyBits->SetNumParticleLives(1);
	sparkleEnergyBits->SetSpawnDelta(ESPInterval(0.005f, 0.02f));
	sparkleEnergyBits->SetInitialSpd(ESPInterval(2.0f, 4.0f));
	sparkleEnergyBits->SetParticleLife(ESPInterval(1.0f, 2.0f));
	sparkleEnergyBits->SetParticleSize(ESPInterval(2.0f, 3.0f));
	sparkleEnergyBits->SetParticleColour(ESPInterval(0.75f, 1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	sparkleEnergyBits->SetEmitAngleInDegrees(20);
	sparkleEnergyBits->SetEmitDirection(Vector3D(emitDirection));
	sparkleEnergyBits->SetAsPointSpriteEmitter(true);
	sparkleEnergyBits->SetEmitPosition(emitPosition3D);
	sparkleEnergyBits->AddEffector(&this->particleFader);
	sparkleEnergyBits->AddEffector(&this->particleMediumShrink);
	bool result = sparkleEnergyBits->SetParticles(7, this->sparkleTex);
	assert(result);
	
	ESPPointEmitter* solidEnergyBits = new ESPPointEmitter();
	solidEnergyBits->SetNumParticleLives(1);
	solidEnergyBits->SetSpawnDelta(ESPInterval(0.005f, 0.02f));
	solidEnergyBits->SetInitialSpd(ESPInterval(2.5f, 4.0f));
	solidEnergyBits->SetParticleLife(ESPInterval(0.5f, 1.0f));
	solidEnergyBits->SetParticleSize(ESPInterval(0.25f, 0.75f));
	solidEnergyBits->SetParticleColour(ESPInterval(0.5f, 0.7f), ESPInterval(0.7f, 1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	solidEnergyBits->SetEmitAngleInDegrees(25);
	solidEnergyBits->SetEmitDirection(Vector3D(emitDirection));
	solidEnergyBits->SetAsPointSpriteEmitter(true);
	solidEnergyBits->SetEmitPosition(emitPosition3D);
	solidEnergyBits->AddEffector(&this->particleFader);
	solidEnergyBits->AddEffector(&this->particleMediumShrink);
	result = solidEnergyBits->SetParticles(10, this->circleGradientTex);
	assert(result);

	// Add a small shockwave
	ESPPointEmitter* shockwave = this->CreateShockwaveEffect(emitPosition3D, 2.0f * GameBall::DEFAULT_BALL_RADIUS, 0.75f);
	assert(shockwave != NULL);

	// Add sound for the ball hitting the shield
	ESPPointEmitter* onomataEffect = this->CreateBallBounceEffect(ball, Onomatoplex::ELECTRIC);
		
	this->activeGeneralEmitters.push_back(shockwave);
	this->activeGeneralEmitters.push_back(solidEnergyBits);
	this->activeGeneralEmitters.push_back(sparkleEnergyBits);
	this->activeGeneralEmitters.push_back(onomataEffect);
}

/**
 * Adds an effect for when a dropping item is acquired by the player paddle -
 * depending on the item type we create the effect.
 */
void GameESPAssets::AddItemAcquiredEffect(const Camera& camera, const PlayerPaddle& paddle, const GameItem& item) {
	ESPInterval redRandomColour(0.1f, 0.8f);
	ESPInterval greenRandomColour(0.1f, 0.8f);
	ESPInterval blueRandomColour(0.1f, 0.8f);
	ESPInterval redColour(0), greenColour(0), blueColour(0);

	switch (item.GetItemDisposition()) {
		case GameItem::Good:
			greenRandomColour = ESPInterval(0.8f, 1.0f);
			redColour		= ESPInterval(GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR.R());
			greenColour = ESPInterval(GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR.G());
			blueColour	=	ESPInterval(GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR.B());
			break;
		case GameItem::Bad:
			redRandomColour = ESPInterval(0.8f, 1.0f);
			redColour		= ESPInterval(GameViewConstants::GetInstance()->ITEM_BAD_COLOUR.R());
			greenColour = ESPInterval(GameViewConstants::GetInstance()->ITEM_BAD_COLOUR.G());
			blueColour	=	ESPInterval(GameViewConstants::GetInstance()->ITEM_BAD_COLOUR.B());
			break;
		case GameItem::Neutral:
			blueRandomColour = ESPInterval(0.8f, 1.0f);
			redColour		= ESPInterval(GameViewConstants::GetInstance()->ITEM_NEUTRAL_COLOUR.R());
			greenColour = ESPInterval(GameViewConstants::GetInstance()->ITEM_NEUTRAL_COLOUR.G());
			blueColour	=	ESPInterval(GameViewConstants::GetInstance()->ITEM_NEUTRAL_COLOUR.B());
		default:
			break;
	}

	if (paddle.GetIsPaddleCameraOn()) {
		const float HEIGHT_TO_WIDTH_RATIO = static_cast<float>(camera.GetWindowHeight()) / static_cast<float>(camera.GetWindowWidth());
		const float HALO_WIDTH						= 2.5f * paddle.GetHalfWidthTotal();
		const float HALO_HEIGHT						= HEIGHT_TO_WIDTH_RATIO * HALO_WIDTH;

		// When the paddle camera is on we just display a halo on screen...
		ESPPointEmitter* haloExpandingAura = new ESPPointEmitter();
		haloExpandingAura->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
		haloExpandingAura->SetInitialSpd(ESPInterval(0));
		haloExpandingAura->SetParticleLife(ESPInterval(3.0f));
		haloExpandingAura->SetParticleSize(ESPInterval(HALO_WIDTH), ESPInterval(HALO_HEIGHT));
		haloExpandingAura->SetEmitAngleInDegrees(0);
		haloExpandingAura->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
		haloExpandingAura->SetParticleAlignment(ESP::ScreenAligned);
		haloExpandingAura->SetEmitPosition(Point3D(0, 0, 0));
		haloExpandingAura->SetParticleColour(redColour, greenColour, blueColour, ESPInterval(0.8f));
		haloExpandingAura->AddEffector(&this->particleLargeGrowth);
		haloExpandingAura->AddEffector(&this->particleFader);

		bool result = haloExpandingAura->SetParticles(1, this->haloTex);
        UNUSED_VARIABLE(result);
		assert(result);

		this->activePaddleEmitters.push_back(haloExpandingAura);
	}
	else {
		// We're not in paddle camera mode, so show the item acquired effect normally...

		// Pulsing aura
		ESPPointEmitter* paddlePulsingAura = new ESPPointEmitter();
		paddlePulsingAura->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
		paddlePulsingAura->SetInitialSpd(ESPInterval(0));
		paddlePulsingAura->SetParticleLife(ESPInterval(1.0f));
		paddlePulsingAura->SetParticleSize(ESPInterval(2.0f * paddle.GetHalfWidthTotal()), ESPInterval(4.0f * paddle.GetHalfHeight()));
		paddlePulsingAura->SetEmitAngleInDegrees(0);
		paddlePulsingAura->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
		paddlePulsingAura->SetParticleAlignment(ESP::ScreenAligned);
		paddlePulsingAura->SetEmitPosition(Point3D(0, 0, 0));
		paddlePulsingAura->SetParticleColour(redColour, greenColour, blueColour, ESPInterval(0.7f));
		paddlePulsingAura->AddEffector(&this->particleLargeGrowth);
		paddlePulsingAura->AddEffector(&this->particleFader);
		bool result = paddlePulsingAura->SetParticles(1, this->circleGradientTex);
		assert(result);

		// Halo expanding aura
		ESPPointEmitter* haloExpandingAura = new ESPPointEmitter();
		haloExpandingAura->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
		haloExpandingAura->SetInitialSpd(ESPInterval(0));
		haloExpandingAura->SetParticleLife(ESPInterval(1.0f));
		haloExpandingAura->SetParticleSize(ESPInterval(3.0f * paddle.GetHalfWidthTotal()), ESPInterval(5.0f * paddle.GetHalfHeight()));
		haloExpandingAura->SetEmitAngleInDegrees(0);
		haloExpandingAura->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
		haloExpandingAura->SetParticleAlignment(ESP::ScreenAligned);
		haloExpandingAura->SetEmitPosition(Point3D(0, 0, 0));
		haloExpandingAura->SetParticleColour(redColour, greenColour, blueColour, ESPInterval(0.8f));
		haloExpandingAura->AddEffector(&this->particleLargeGrowth);
		haloExpandingAura->AddEffector(&this->particleFader);
		result = haloExpandingAura->SetParticles(1, this->haloTex);
		assert(result);	

		// Absorb glow sparks
		ESPPointEmitter* absorbGlowSparks = new ESPPointEmitter();
		absorbGlowSparks->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
		absorbGlowSparks->SetInitialSpd(ESPInterval(2.5f, 4.0f));
		absorbGlowSparks->SetParticleLife(ESPInterval(1.0f, 1.5f));
		absorbGlowSparks->SetParticleSize(ESPInterval(0.3f, 0.8f));
		absorbGlowSparks->SetParticleColour(redRandomColour, greenRandomColour, blueRandomColour, ESPInterval(0.8f));
		absorbGlowSparks->SetEmitAngleInDegrees(180);
		absorbGlowSparks->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
		absorbGlowSparks->SetAsPointSpriteEmitter(true);
		absorbGlowSparks->SetParticleAlignment(ESP::ScreenAligned);
		absorbGlowSparks->SetIsReversed(true);
		absorbGlowSparks->SetEmitPosition(Point3D(0, 0, 0));
		absorbGlowSparks->AddEffector(&this->particleFader);
		absorbGlowSparks->AddEffector(&this->particleMediumShrink);
		result = absorbGlowSparks->SetParticles(NUM_ITEM_ACQUIRED_SPARKS, this->circleGradientTex);
		assert(result);
		
		this->activePaddleEmitters.push_back(haloExpandingAura);
		this->activePaddleEmitters.push_back(paddlePulsingAura);
		this->activePaddleEmitters.push_back(absorbGlowSparks);
	}
}

/**
 * Add the effect for when the paddle grows.
 */
void GameESPAssets::AddPaddleGrowEffect() {
	std::vector<Vector3D> directions;
	directions.push_back(Vector3D(0,1,0));
	directions.push_back(Vector3D(0,-1,0));
	directions.push_back(Vector3D(1,0,0));
	directions.push_back(Vector3D(-1,0,0));

	for (unsigned int i = 0; i < 4; i++) {
		ESPPointEmitter* paddleGrowEffect = new ESPPointEmitter();
		paddleGrowEffect->SetSpawnDelta(ESPInterval(0.005));
		paddleGrowEffect->SetNumParticleLives(1);
		paddleGrowEffect->SetInitialSpd(ESPInterval(2.0f, 3.5f));
		paddleGrowEffect->SetParticleLife(ESPInterval(0.8f, 2.00f));
		paddleGrowEffect->SetParticleSize(ESPInterval(0.25f, 1.0f));
		paddleGrowEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
		paddleGrowEffect->SetParticleAlignment(ESP::AxisAligned);
		paddleGrowEffect->SetEmitPosition(Point3D(0,0,0));
		paddleGrowEffect->SetEmitDirection(directions[i]);
		paddleGrowEffect->SetEmitAngleInDegrees(23);
		paddleGrowEffect->SetParticleColour(ESPInterval(0.0f, 0.5f), ESPInterval(0.8f, 1.0f), ESPInterval(0.0f, 0.5f), ESPInterval(1.0f));
		paddleGrowEffect->SetParticles(GameESPAssets::NUM_PADDLE_SIZE_CHANGE_PARTICLES/4, this->upArrowTex);
		paddleGrowEffect->AddEffector(&this->particleFader);
		paddleGrowEffect->AddEffector(&this->particleMediumGrowth);

		this->activePaddleEmitters.push_back(paddleGrowEffect);
	}
}

/**
 * Add the effect for when the paddle shrinks.
 */
void GameESPAssets::AddPaddleShrinkEffect() {
	std::vector<Vector3D> directions;
	directions.push_back(Vector3D(0,1,0));
	directions.push_back(Vector3D(0,-1,0));
	directions.push_back(Vector3D(1,0,0));
	directions.push_back(Vector3D(-1,0,0));

	for (unsigned int i = 0; i < 4; i++) {
		ESPPointEmitter* paddleShrinkEffect = new ESPPointEmitter();
		paddleShrinkEffect->SetSpawnDelta(ESPInterval(0.005));
		paddleShrinkEffect->SetNumParticleLives(1);
		paddleShrinkEffect->SetInitialSpd(ESPInterval(2.0f, 3.0f));
		paddleShrinkEffect->SetParticleLife(ESPInterval(0.8f, 1.25f));
		paddleShrinkEffect->SetParticleSize(ESPInterval(0.75f, 1.5f));
		paddleShrinkEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
		paddleShrinkEffect->SetParticleAlignment(ESP::AxisAligned);
		paddleShrinkEffect->SetEmitPosition(Point3D(0,0,0));
		paddleShrinkEffect->SetEmitDirection(directions[i]);
		paddleShrinkEffect->SetEmitAngleInDegrees(23);
		paddleShrinkEffect->SetIsReversed(true);
		paddleShrinkEffect->SetParticleColour(ESPInterval(0.8f, 1.0f), ESPInterval(0.0f, 0.5f) , ESPInterval(0.0f, 0.5f), ESPInterval(1.0f));
		paddleShrinkEffect->SetParticles(GameESPAssets::NUM_PADDLE_SIZE_CHANGE_PARTICLES/4, this->upArrowTex);
		paddleShrinkEffect->AddEffector(&this->particleFader);
		paddleShrinkEffect->AddEffector(&this->particleMediumShrink);

		this->activePaddleEmitters.push_back(paddleShrinkEffect);
	}
}

/**
 * Add the effect for when the ball grows - bunch of arrows that point and move outward around the ball.
 */
void GameESPAssets::AddBallGrowEffect(const GameBall* ball) {
	assert(ball != NULL);

	std::vector<Vector3D> directions;
	directions.push_back(Vector3D(0,1,0));
	directions.push_back(Vector3D(0,-1,0));
	directions.push_back(Vector3D(1,0,0));
	directions.push_back(Vector3D(-1,0,0));

	for (unsigned int i = 0; i < 4; i++) {
		ESPPointEmitter* ballGrowEffect = new ESPPointEmitter();
		ballGrowEffect->SetSpawnDelta(ESPInterval(0.005));
		ballGrowEffect->SetNumParticleLives(1);
		ballGrowEffect->SetInitialSpd(ESPInterval(1.5f, 2.5f));
		ballGrowEffect->SetParticleLife(ESPInterval(0.8f, 2.00f));
		ballGrowEffect->SetParticleSize(ESPInterval(0.2f, 0.8f));
		ballGrowEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
		ballGrowEffect->SetParticleAlignment(ESP::AxisAligned);
		ballGrowEffect->SetEmitPosition(Point3D(0,0,0));
		ballGrowEffect->SetEmitDirection(directions[i]);
		ballGrowEffect->SetEmitAngleInDegrees(23);
		ballGrowEffect->SetParticleColour(ESPInterval(0.0f, 0.5f), ESPInterval(0.8f, 1.0f), ESPInterval(0.0f, 0.5f), ESPInterval(1.0f));
		ballGrowEffect->SetParticles(GameESPAssets::NUM_PADDLE_SIZE_CHANGE_PARTICLES/4, this->upArrowTex);
		ballGrowEffect->AddEffector(&this->particleFader);
		ballGrowEffect->AddEffector(&this->particleMediumGrowth);

		this->activeBallBGEmitters[ball].push_back(ballGrowEffect);
	}
}

/**
 * Add the effect for when the ball shrinks - bunch of arrows that point and move inward around the ball.
 */
void GameESPAssets::AddBallShrinkEffect(const GameBall* ball) {
	assert(ball != NULL);

	std::vector<Vector3D> directions;
	directions.push_back(Vector3D(0,1,0));
	directions.push_back(Vector3D(0,-1,0));
	directions.push_back(Vector3D(1,0,0));
	directions.push_back(Vector3D(-1,0,0));

	for (unsigned int i = 0; i < 4; i++) {
		ESPPointEmitter* ballShrinkEffect = new ESPPointEmitter();
		ballShrinkEffect->SetSpawnDelta(ESPInterval(0.005));
		ballShrinkEffect->SetNumParticleLives(1);
		ballShrinkEffect->SetInitialSpd(ESPInterval(1.0f, 2.0f));
		ballShrinkEffect->SetParticleLife(ESPInterval(0.8f, 1.25f));
		ballShrinkEffect->SetParticleSize(ESPInterval(0.5f, 1.25f));
		ballShrinkEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
		ballShrinkEffect->SetParticleAlignment(ESP::AxisAligned);
		ballShrinkEffect->SetEmitPosition(Point3D(0,0,0));
		ballShrinkEffect->SetEmitDirection(directions[i]);
		ballShrinkEffect->SetEmitAngleInDegrees(23);
		ballShrinkEffect->SetIsReversed(true);
		ballShrinkEffect->SetParticleColour(ESPInterval(0.8f, 1.0f), ESPInterval(0.0f, 0.5f) , ESPInterval(0.0f, 0.5f), ESPInterval(1.0f));
		ballShrinkEffect->SetParticles(GameESPAssets::NUM_BALL_SIZE_CHANGE_PARTICLES/4, this->upArrowTex);
		ballShrinkEffect->AddEffector(&this->particleFader);
		ballShrinkEffect->AddEffector(&this->particleMediumShrink);

		this->activeBallBGEmitters[ball].push_back(ballShrinkEffect);
	}
}

void GameESPAssets::AddGravityBallESPEffects(const GameBall* ball, std::vector<ESPPointEmitter*>& effectsList) {
	assert(ball != NULL);
	const float BALL_RADIUS = ball->GetBounds().Radius();

	ESPPointEmitter* ballGravityEffect = new ESPPointEmitter();
	ballGravityEffect->SetSpawnDelta(ESPInterval(0.1f, 0.15f));
	ballGravityEffect->SetNumParticleLives(ESPParticle::INFINITE_PARTICLE_LIVES);
	ballGravityEffect->SetInitialSpd(ESPInterval(1.0f, 2.0f));
	ballGravityEffect->SetParticleLife(ESPInterval(1.8f, 2.2f));
	ballGravityEffect->SetParticleSize(ESPInterval(1.5f*BALL_RADIUS, 2.0f*BALL_RADIUS));
	ballGravityEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.8f*BALL_RADIUS), ESPInterval(0), ESPInterval(0.0f, 0.8f*BALL_RADIUS));
	ballGravityEffect->SetParticleAlignment(ESP::ScreenAlignedFollowVelocity);
	ballGravityEffect->SetEmitPosition(Point3D(0,0,0));
	ballGravityEffect->SetEmitDirection(Vector3D(0, -1, 0));	// Downwards gravity vector is always in -y direction
	ballGravityEffect->SetEmitAngleInDegrees(0.0f);
	ballGravityEffect->SetToggleEmitOnPlane(true, Vector3D(0, 0, 1));

	ballGravityEffect->SetParticleColour(ESPInterval(0.8f, 1.0f), ESPInterval(0.0f, 0.5f) , ESPInterval(0.0f, 0.5f), ESPInterval(1.0f));
	ballGravityEffect->SetParticles(10, this->upArrowTex);
	ballGravityEffect->AddEffector(&this->particleGravityArrowColour);
	ballGravityEffect->AddEffector(&this->particleSmallGrowth);

	effectsList.push_back(ballGravityEffect);
}

void GameESPAssets::AddCrazyBallESPEffects(const GameBall* ball, std::vector<ESPPointEmitter*>& effectsList) {
	// Create an emitter for insanity text...
	ESPPointEmitter* crazyTextEffect = new ESPPointEmitter();
	// Set up the emitter...
	crazyTextEffect->SetSpawnDelta(ESPInterval(0.10f, 0.20f));
	crazyTextEffect->SetInitialSpd(ESPInterval(1.0f, 3.0f));
	crazyTextEffect->SetParticleLife(ESPInterval(0.8f, 1.7f));
	crazyTextEffect->SetParticleSize(ESPInterval(0.7f, 1.0f));
	crazyTextEffect->SetParticleRotation(ESPInterval(-45.0f, 45.0f));
	crazyTextEffect->SetRadiusDeviationFromCenter(ESPInterval(0.8f * ball->GetBounds().Radius(), ball->GetBounds().Radius()));
	crazyTextEffect->SetParticleAlignment(ESP::ScreenAligned);
	crazyTextEffect->SetEmitPosition(Point3D(0,0,0));
	crazyTextEffect->SetEmitAngleInDegrees(180.0f);
	crazyTextEffect->SetParticleColour(ESPInterval(0.15f, 1.0f), ESPInterval(0.15f, 1.0f), ESPInterval(0.15f, 1.0f), ESPInterval(1.0f));
	crazyTextEffect->AddEffector(&this->particleFader);
	crazyTextEffect->AddEffector(&this->particleMediumGrowth);

	// Add the single text particle to the emitter...
	TextLabel2D crazyTextLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Small), "");
	crazyTextLabel.SetColour(Colour(1, 1, 1));
	crazyTextLabel.SetDropShadow(Colour(0, 0, 0), 0.1f);
	crazyTextEffect->SetParticles(12, crazyTextLabel, Onomatoplex::CRAZY, Onomatoplex::GOOD);

	// Create an emitter for green particles emitting from the ball...
	ESPPointEmitter* crazySparks = new ESPPointEmitter();
	crazySparks->SetSpawnDelta(ESPInterval(0.01f, 0.03f));
	crazySparks->SetInitialSpd(ESPInterval(0.0f, 2.0f));
	crazySparks->SetParticleLife(ESPInterval(0.6f, 0.8f));
	crazySparks->SetParticleSize(ESPInterval(0.33f * ball->GetBounds().Radius(), ball->GetBounds().Radius()));
	crazySparks->SetParticleColour(ESPInterval(0.48f, 0.58f), ESPInterval(0.88f, 1.0f), ESPInterval(0.0f), ESPInterval(1.0f));
	crazySparks->SetEmitAngleInDegrees(20);
	crazySparks->SetRadiusDeviationFromCenter(0.25f * ball->GetBounds().Radius());
	crazySparks->SetAsPointSpriteEmitter(true);
	crazySparks->SetEmitPosition(Point3D(0,0,0));
	crazySparks->AddEffector(&this->particleFader);
	crazySparks->SetParticles(15, this->circleGradientTex);

	effectsList.push_back(crazyTextEffect);
	effectsList.push_back(crazySparks);

}

/**
 * Add the effect for when the player acquires a 1UP power-up.
 */
void GameESPAssets::AddLifeUpEffect(const PlayerPaddle* paddle) {
	
	Point2D paddlePos2D = paddle->GetCenterPosition();
	Point3D paddlePos3D = Point3D(paddlePos2D[0], paddlePos2D[1], 0.0f);

	// Create an emitter for the Life-UP! text raising from the paddle
	ESPPointEmitter* textEffect = new ESPPointEmitter();
	// Set up the emitter...
	textEffect->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	textEffect->SetInitialSpd(ESPInterval(2.0f));
	textEffect->SetParticleLife(ESPInterval(2.0f));
	textEffect->SetParticleSize(ESPInterval(1.0f, 1.0f), ESPInterval(1.0f, 1.0f));
	textEffect->SetParticleRotation(ESPInterval(0.0f));
	textEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	textEffect->SetParticleAlignment(ESP::ScreenAligned);
	textEffect->SetEmitPosition(paddlePos3D + Vector3D(0, 0.5f*PlayerPaddle::PADDLE_WIDTH_TOTAL, 0));
	textEffect->SetEmitDirection(Vector3D(0, 1, 0));
	textEffect->SetParticleColour(ESPInterval(1), ESPInterval(1), ESPInterval(1), ESPInterval(1.0f));
	// Add effectors...
	textEffect->AddEffector(&this->particleFader);
	textEffect->AddEffector(&this->particleSmallGrowth);

	// Add the single particle to the emitter...
	DropShadow dpTemp;
	dpTemp.amountPercentage = 0.15f;
	dpTemp.colour = Colour(0.0f, 0.0f, 0.0f);
    ESPOnomataParticle* oneUpTextParticle = new ESPOnomataParticle(GameFontAssetsManager::GetInstance()->GetFont(
        GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), "Life-UP!");
	oneUpTextParticle->SetDropShadow(dpTemp);
	textEffect->AddParticle(oneUpTextParticle);
    
    // Create an emitter for the heart graphic
    ESPPointEmitter* heartEffect = new ESPPointEmitter();
	heartEffect->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	heartEffect->SetInitialSpd(ESPInterval(2.0f));
	heartEffect->SetParticleLife(ESPInterval(2.0f));
    heartEffect->SetParticleSize(ESPInterval(0.5f*PlayerPaddle::PADDLE_WIDTH_TOTAL), ESPInterval(0.5f*PlayerPaddle::PADDLE_WIDTH_TOTAL));
	heartEffect->SetParticleRotation(ESPInterval(0.0f));
	heartEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	heartEffect->SetParticleAlignment(ESP::ScreenAligned);
	heartEffect->SetEmitPosition(paddlePos3D);
	heartEffect->SetEmitDirection(Vector3D(0, 1, 0));
	heartEffect->SetParticleColour(ESPInterval(1), ESPInterval(1), ESPInterval(1), ESPInterval(1.0f));
	// Add effectors...
	heartEffect->AddEffector(&this->particleFader);
	heartEffect->AddEffector(&this->particleSmallGrowth);
    heartEffect->SetParticles(1, this->heartTex);

	// Lastly, add the new emitters to the list of active emitters in order of back to front
	this->activeGeneralEmitters.push_back(heartEffect);
    this->activeGeneralEmitters.push_back(textEffect);
}

/**
 * Adds an effect based on the given game item being activated or deactivated.
 */
void GameESPAssets::SetItemEffect(const GameItem& item, const GameModel& gameModel) {
	switch(item.GetItemType()) {

		case GameItem::UberBallItem: {

				const GameBall* ballAffected = item.GetBallAffected();
				assert(ballAffected != NULL);

				// If there are any effects assigned for the uber ball then we need to reset the trail
				std::map<const GameBall*, std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> > >::iterator foundBallEffects = this->ballEffects.find(ballAffected);
				if (foundBallEffects != this->ballEffects.end()) {
					std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> >::iterator foundUberBallFX = foundBallEffects->second.find(GameItem::UberBallItem);
					if (foundUberBallFX != foundBallEffects->second.end()) {
						std::vector<ESPPointEmitter*>& uberBallEffectsList = foundUberBallFX->second;
						assert(uberBallEffectsList.size() > 0);
						uberBallEffectsList[0]->Reset();
					}
				}
			}
			break;

		case GameItem::IceBallItem:
			{
				// If there are any effects assigned for the ice ball then we need to reset the trail
				const GameBall* ballAffected = item.GetBallAffected();
				assert(ballAffected != NULL);
				std::map<const GameBall*, std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> > >::iterator foundBallEffects = this->ballEffects.find(ballAffected);
				if (foundBallEffects != this->ballEffects.end()) {
					std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> >::iterator foundIceBallFX = foundBallEffects->second.find(GameItem::IceBallItem);
					if (foundIceBallFX != foundBallEffects->second.end()) {
						std::vector<ESPPointEmitter*>& emitters = foundIceBallFX->second;
						for (std::vector<ESPPointEmitter*>::iterator iter = emitters.begin(); iter != emitters.end(); ++iter) {
							ESPPointEmitter* currEmitter = *iter;
							currEmitter->Reset();
						}	
					}
				}
			}
			break;


		case GameItem::LaserBulletPaddleItem: {
				this->paddleLaserGlowAura->Reset();
				this->paddleLaserGlowSparks->Reset();
			}
			break;

		case GameItem::LaserBeamPaddleItem: {
				this->paddleBeamOriginUp->Reset();
				this->paddleBeamBlastBits->Reset();
			}
			break;

		case GameItem::CrazyBallItem:
			this->crazyBallAura->Reset();
			break;

		case GameItem::PaddleGrowItem: {
				this->AddPaddleGrowEffect();
			}
			break;

		case GameItem::PaddleShrinkItem: {
				this->AddPaddleShrinkEffect();
			}
			break;

		case GameItem::BallGrowItem: {
				this->AddBallGrowEffect(item.GetBallAffected());
			}
			break;

		case GameItem::BallShrinkItem: {
				this->AddBallShrinkEffect(item.GetBallAffected());
			}
			break;

		case GameItem::LifeUpItem: {
                this->AddLifeUpEffect(gameModel.GetPlayerPaddle());
			}
			break;

		default:
			break;
	}
}

/**
 * Draw call that will draw all active effects and clear up all inactive effects
 * for the game. These are particle effects that require no render to texture or other fancy
 * shmancy type stuffs.
 */
void GameESPAssets::DrawParticleEffects(double dT, const Camera& camera) {
	// Go through all the other particles and do book keeping and drawing
	for (std::list<ESPEmitter*>::iterator iter = this->activeGeneralEmitters.begin(); iter != this->activeGeneralEmitters.end();) {
		ESPEmitter* curr = *iter;
		assert(curr != NULL);

		// Check to see if dead, if so erase it...
		if (curr->IsDead()) {
			iter = this->activeGeneralEmitters.erase(iter);
			delete curr;
			curr = NULL;
		}
		else {
			// Not dead yet so we draw and tick
			curr->Draw(camera);
			curr->Tick(dT);
			++iter;
		}
	}
}

/**
 * Update and draw all projectile effects that are currently active.
 */
void GameESPAssets::DrawProjectileEffects(double dT, const Camera& camera) {
	for (std::map<const Projectile*, std::list<ESPPointEmitter*> >::iterator iter = this->activeProjectileEmitters.begin();
		iter != this->activeProjectileEmitters.end(); ++iter) {
		
		const Projectile* currProjectile = iter->first;
		if (!currProjectile->GetIsActive()) {
			continue;
		}

		std::list<ESPPointEmitter*>& projEmitters = iter->second;

		assert(currProjectile != NULL);

		// Update and draw the emitters, background then foreground...
		for (std::list<ESPPointEmitter*>::iterator emitIter = projEmitters.begin(); emitIter != projEmitters.end(); ++emitIter) {
			this->DrawProjectileEmitter(dT, camera, *currProjectile, *emitIter);
		}
	}
}

/**
 * Private helper function for drawing a single projectile at a given position.
 */
void GameESPAssets::DrawProjectileEmitter(double dT, const Camera& camera, const Projectile& projectile, ESPPointEmitter* projectileEmitter) {

	// In the case where the particle only spawns once, we have a stationary particle that needs to move
	// to its current position and have an orientation to its current direction
	bool movesWithProjectile = projectileEmitter->OnlySpawnsOnce();
	if (movesWithProjectile) {
		glPushMatrix();
		glTranslatef(projectile.GetPosition()[0], projectile.GetPosition()[1], 0.0f);

		// If the projectile is not symetrical then we rotate it so that it doesn't look strange in paddle camera mode
		if (projectileEmitter->GetParticleSizeX() != projectileEmitter->GetParticleSizeY()) {
			// Calculate the angle to rotate it about the z-axis
			float angleToRotate = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(projectile.GetVelocityDirection(), Vector2D(0, 1))))));
			if (projectile.GetVelocityDirection()[0] > 0) {
				angleToRotate *= -1.0;
			}
			glRotatef(angleToRotate, 0.0f, 0.0f, 1.0f);
		}
	}
	else {
		// We want all the emitting, moving particles attached to the projectile to move with the projectile and
		// fire opposite its trajectory
        Point3D emitPos = Point3D(projectile.GetPosition() - projectile.GetHalfHeight() * projectile.GetVelocityDirection(), projectile.GetZOffset());
		projectileEmitter->SetEmitPosition(emitPos);
		projectileEmitter->SetEmitDirection(Vector3D(-projectile.GetVelocityDirection()[0], -projectile.GetVelocityDirection()[1], 0.0f));
	}
	
	projectileEmitter->Draw(camera);
	projectileEmitter->Tick(dT);

	if (movesWithProjectile) {
		glPopMatrix();
	}
}

/**
 * Draw the effects that occur as a item drops down towards the player paddle.
 * NOTE: You must transform these effects to be where the item is first!
 */
void GameESPAssets::DrawItemDropEffects(double dT, const Camera& camera, const GameItem& item) {
	// Find the effects for the item we want to draw
	std::map<const GameItem*, std::list<ESPEmitter*> >::iterator itemDropEffectIter = this->activeItemDropEmitters.find(&item);
	
	// If no effects were found then exit...
	if (itemDropEffectIter == this->activeItemDropEmitters.end()) {
		return;
	}
	
	// Draw the appropriate effects
	assert(itemDropEffectIter->second.size() > 0);
	for (std::list<ESPEmitter*>::iterator iter = itemDropEffectIter->second.begin(); iter != itemDropEffectIter->second.end(); ++iter) {
		ESPEmitter* currEmitter = *iter;
		currEmitter->Draw(camera);
		currEmitter->Tick(dT);
	}
}

// Private helper function to ensure a ball has an associated effects list
std::map<const GameBall*, std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> > >::iterator
GameESPAssets::EnsureBallEffectsList(const GameBall& ball) {
	// Check to see if the ball has any associated effects, if not, then we add one for the ball
	std::map<const GameBall*, std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> > >::iterator foundBallEffects = this->ballEffects.find(&ball);
	
	if (foundBallEffects == this->ballEffects.end()) {
		// Didn't even find a ball ... add one
		foundBallEffects = this->ballEffects.insert(std::make_pair(&ball, std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> >())).first;
	}

	return foundBallEffects;
}

/**
 * Draw particle effects associated with the uberball.
 */
void GameESPAssets::DrawUberBallEffects(double dT, const Camera& camera, const GameBall& ball) {
	// Check to see if the ball has any associated uber ball effects, if not, then
	// create the effect and add it to the ball first
	std::map<const GameBall*, std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> > >::iterator foundBallEffects = 
		this->EnsureBallEffectsList(ball);

	if (foundBallEffects->second.find(GameItem::UberBallItem) == foundBallEffects->second.end()) {
		// Didn't find an associated uber ball effect, so add one
		this->AddUberBallESPEffects(this->ballEffects[&ball][GameItem::UberBallItem]);
	}
	std::vector<ESPPointEmitter*>& uberBallEffectList = this->ballEffects[&ball][GameItem::UberBallItem];

	Point2D ballPos  = ball.GetBounds().Center();

	glPushMatrix();
	const Point2D& loc = ball.GetBounds().Center();
	glTranslatef(loc[0], loc[1], 0);

	uberBallEffectList[1]->SetParticleSize(ESPInterval(3.0f*ball.GetBounds().Radius()));
	uberBallEffectList[1]->Draw(camera);
	uberBallEffectList[1]->Tick(dT);

	glPopMatrix();

	// Draw the trail...
	uberBallEffectList[0]->SetParticleSize(ESPInterval(2.5f*ball.GetBounds().Radius(), 3.5f*ball.GetBounds().Radius()));
	uberBallEffectList[0]->SetEmitPosition(Point3D(ballPos[0], ballPos[1], 0.0f));
	uberBallEffectList[0]->Draw(camera);
	uberBallEffectList[0]->Tick(dT);
}

/**
 * Draw particle effects associated with the ghostball.
 */
void GameESPAssets::DrawGhostBallEffects(double dT, const Camera& camera, const GameBall& ball) {
	// Check to see if the ball has any associated ghost ball effects, if not, then
	// create the effect and add it to the ball first
	std::map<const GameBall*, std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> > >::iterator foundBallEffects = 
		this->EnsureBallEffectsList(ball);

	if (foundBallEffects->second.find(GameItem::GhostBallItem) == foundBallEffects->second.end()) {
		// Didn't find an associated ghost ball effect, so add one
		this->AddGhostBallESPEffects(this->ballEffects[&ball][GameItem::GhostBallItem]);
	}
	std::vector<ESPPointEmitter*>& ghostBallEffectList = this->ballEffects[&ball][GameItem::GhostBallItem];

	glPushMatrix();
	const Point2D& loc = ball.GetBounds().Center();
	glTranslatef(loc[0], loc[1], 0);

	// Rotate the negative ball velocity direction by some random amount and then affect the particle's velocities
	// by it, this gives the impression that the particles are waving around mysteriously (ghostlike... one might say)
	double randomDegrees = Randomizer::GetInstance()->RandomNumNegOneToOne() * 90;
	const Vector2D& ballDir = ball.GetDirection();

	Vector2D accelVec = Vector2D::Rotate(static_cast<float>(randomDegrees), -ballDir);
	accelVec = ball.GetSpeed() * 4.0f * accelVec;
	this->ghostBallAccel1.SetAcceleration(Vector3D(accelVec, 0.0f));

	// Draw the ghostly trail for the ball...
	ghostBallEffectList[0]->SetParticleSize(ESPInterval(2.5f*ball.GetBounds().Radius(), 3.5f*ball.GetBounds().Radius()));
	ghostBallEffectList[0]->Draw(camera);
	ghostBallEffectList[0]->Tick(dT);
	
	glPopMatrix();
}

void GameESPAssets::DrawFireBallEffects(double dT, const Camera& camera, const GameBall& ball) {
	// Check to see if the ball has any associated fire ball effects, if not, then
	// create the effect and add it to the ball first
	std::map<const GameBall*, std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> > >::iterator foundBallEffects = 
		this->EnsureBallEffectsList(ball);

	if (foundBallEffects->second.find(GameItem::FireBallItem) == foundBallEffects->second.end()) {
		// Didn't find an associated fire ball effect, so add one
		this->AddFireBallESPEffects(&ball, this->ballEffects[&ball][GameItem::FireBallItem]);
	}

	std::vector<ESPPointEmitter*>& fireBallEffectList = this->ballEffects[&ball][GameItem::FireBallItem];

	glPushMatrix();
	const Point2D& loc = ball.GetBounds().Center();
	glTranslatef(loc[0], loc[1], 0);

	// Rotate the negative ball velocity direction by some random amount and then affect the particle's velocities
	// by it, this gives the impression that the particles are flickering like fire
	double randomDegrees = Randomizer::GetInstance()->RandomNumNegOneToOne() * 30;
	const Vector2D& ballDir = ball.GetDirection();

	Vector2D accelVec = Vector2D::Rotate(static_cast<float>(randomDegrees), -ballDir);
	accelVec = ball.GetSpeed() * 3.25f * accelVec;
	this->fireBallAccel1.SetAcceleration(Vector3D(accelVec, 0.0f));

	for (std::vector<ESPPointEmitter*>::iterator iter = fireBallEffectList.begin(); iter != fireBallEffectList.end(); ++iter) {
		ESPPointEmitter* emitter = *iter;

		emitter->SetParticleSize(ESPInterval(2.33f*ball.GetBounds().Radius(), 3.33f*ball.GetBounds().Radius()));
		emitter->Draw(camera);
		emitter->Tick(dT);
	}

	glPopMatrix();
}

void GameESPAssets::DrawIceBallEffects(double dT, const Camera& camera, const GameBall& ball) {
	// Check to see if the ball has any associated ice ball effects, if not, then
	// create the effect and add it to the ball first
	std::map<const GameBall*, std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> > >::iterator foundBallEffects = 
		this->EnsureBallEffectsList(ball);

	if (foundBallEffects->second.find(GameItem::IceBallItem) == foundBallEffects->second.end()) {
		// Didn't find an associated ice ball effect, so add one
		this->AddIceBallESPEffects(&ball, this->ballEffects[&ball][GameItem::IceBallItem]);
	}

	std::vector<ESPPointEmitter*>& iceBallEffectList = this->ballEffects[&ball][GameItem::IceBallItem];
	assert(iceBallEffectList.size() >= (3 + this->snowflakeTextures.size()));
	size_t lastIdx = iceBallEffectList.size()-1;
	const Point2D& loc  = ball.GetBounds().Center();
	const Vector2D& dir = ball.GetDirection();

	iceBallEffectList[0]->SetParticleSize(ESPInterval(3.0f * ball.GetBounds().Radius()));
	iceBallEffectList[1]->SetParticleSize(ESPInterval(1.25f * ball.GetBounds().Radius(), 2.0f * ball.GetBounds().Radius()));
	for (size_t i = 0; i < this->snowflakeTextures.size(); i++) {
		iceBallEffectList[i+2]->SetParticleSize(ESPInterval(ball.GetBounds().Radius(), 2.5f * ball.GetBounds().Radius()));
	}
	iceBallEffectList[lastIdx]->SetParticleSize(ESPInterval(2.25f*ball.GetBounds().Radius(), 3.25f*ball.GetBounds().Radius()));

	// Rotate the negative ball velocity direction by some random amount and then affect the particle's velocities
	// by it, this gives the impression that the particles are flickering like fire
	double randomDegrees = Randomizer::GetInstance()->RandomNumNegOneToOne() * 20;
	Vector2D accelVec = Vector2D::Rotate(static_cast<float>(randomDegrees), -dir);
	accelVec = ball.GetSpeed() * 2.75f * accelVec;
	this->iceBallAccel.SetAcceleration(Vector3D(accelVec, 0.0f));

	// The aura always needs to be drawn centered on the ball
	glPushMatrix();
	glTranslatef(loc[0], loc[1], 0);
	iceBallEffectList[0]->Draw(camera);
	iceBallEffectList[0]->Tick(dT);
	iceBallEffectList[lastIdx]->Draw(camera);
	iceBallEffectList[lastIdx]->Tick(dT);
	glPopMatrix();

	for (size_t i = 1; i < lastIdx; i++) {
		ESPPointEmitter* emitter = iceBallEffectList[i];

		emitter->SetEmitDirection(-Vector3D(dir));
		emitter->SetEmitPosition(Point3D(loc));
		emitter->Draw(camera);
		emitter->Tick(dT);
	}
}

void GameESPAssets::DrawGravityBallEffects(double dT, const Camera& camera, const GameBall& ball, const Vector3D& gravityDir) {
	// Check to see if the ball has any associated gravity ball effects, if not, then
	// create the effect and add it to the ball first
	std::map<const GameBall*, std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> > >::iterator foundBallEffects = 
		this->EnsureBallEffectsList(ball);

	if (foundBallEffects->second.find(GameItem::GravityBallItem) == foundBallEffects->second.end()) {
		// Didn't find an associated gravity ball effect, so add one
		this->AddGravityBallESPEffects(&ball, this->ballEffects[&ball][GameItem::GravityBallItem]);
	}

	std::vector<ESPPointEmitter*>& gravityBallEffectList = this->ballEffects[&ball][GameItem::GravityBallItem];

	glPushMatrix();
	const Point2D& loc = ball.GetBounds().Center();
	glTranslatef(loc[0], loc[1], 0);

	for (std::vector<ESPPointEmitter*>::iterator iter = gravityBallEffectList.begin(); iter != gravityBallEffectList.end(); ++iter) {
		ESPPointEmitter* emitter = *iter;

		// If the gravity direction is changing then reset it - we reset the emitter so the
		// user doesn't see random particles flying around inbetween the transitions
		if (emitter->GetEmitDirection() != gravityDir) {
			emitter->SetEmitDirection(gravityDir);
			emitter->Reset();
		}
		emitter->Tick(dT);
		emitter->Draw(camera);
	}

	glPopMatrix();
}

void GameESPAssets::DrawCrazyBallEffects(double dT, const Camera& camera, const GameBall& ball) {
	// Check to see if the ball has any associated crazy ball effects, if not, then
	// create the effect and add it to the ball first
	std::map<const GameBall*, std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> > >::iterator foundBallEffects = 
		this->EnsureBallEffectsList(ball);

	if (foundBallEffects->second.find(GameItem::CrazyBallItem) == foundBallEffects->second.end()) {
		// Didn't find an associated crazy ball effect, so add one
		this->AddCrazyBallESPEffects(&ball, this->ballEffects[&ball][GameItem::CrazyBallItem]);
	}

	std::vector<ESPPointEmitter*>& crazyBallEffectList = this->ballEffects[&ball][GameItem::CrazyBallItem];

	// Reset all of the effects for the ball if it's inside a cannon block
	if (ball.IsLoadedInCannonBlock()) {
		for (std::vector<ESPPointEmitter*>::iterator iter = crazyBallEffectList.begin(); iter != crazyBallEffectList.end(); ++iter) {
			ESPPointEmitter* emitter = *iter;
			emitter->Reset();
		}
		return;
	}

	glPushMatrix();
	const Point2D& loc = ball.GetBounds().Center();
	glTranslatef(loc[0], loc[1], 0);

	this->crazyBallAura->SetParticleSize(ESPInterval(2.75f*ball.GetBounds().Radius()));
	this->crazyBallAura->Tick(dT);
	this->crazyBallAura->Draw(camera);
	glPopMatrix();

	for (std::vector<ESPPointEmitter*>::iterator iter = crazyBallEffectList.begin(); iter != crazyBallEffectList.end(); ++iter) {
		ESPPointEmitter* emitter = *iter;
		emitter->SetEmitPosition(ball.GetCenterPosition());
		emitter->SetEmitDirection(-Vector3D(ball.GetDirection()));
		emitter->Tick(dT);
		emitter->Draw(camera);
	}
}

/**
 * Draw effects associated with the ball when it has a target identifying itself - this happens in paddle camera mode
 * to make it easier for the player to see the ball.
 */
void GameESPAssets::DrawPaddleCamEffects(double dT, const Camera& camera, const GameBall& ball, const PlayerPaddle& paddle) {
	// Check to see if the ball has any associated camera paddle ball effects, if not, then
	// create the effect and add it to the ball first
	std::map<const GameBall*, std::map<GameItem::ItemType, std::vector<ESPPointEmitter*> > >::iterator foundBallEffects = 
		this->EnsureBallEffectsList(ball);

	if (foundBallEffects->second.find(GameItem::PaddleCamItem) == foundBallEffects->second.end()) {
		// Didn't find an associated paddle camera ball effect, so add one
		this->AddPaddleCamBallESPEffects(this->ballEffects[&ball][GameItem::PaddleCamItem]);
	}
	std::vector<ESPPointEmitter*>& paddleCamBallEffectList = this->ballEffects[&ball][GameItem::PaddleCamItem];

	glPushMatrix();
	const Point2D& ballLoc		= ball.GetBounds().Center();
	const Point2D& paddleLoc	= paddle.GetCenterPosition();
	
	// The only effect so far is a spinning target on the ball...
	glTranslatef(ballLoc[0], ballLoc[1], 0);

	// Change the colour and alpha of the target based on the proximity to the paddle
	float distanceToPaddle = Point2D::Distance(ballLoc, paddleLoc);
	const float MAX_DIST_AWAY = LevelPiece::PIECE_HEIGHT * 20.0f;
	distanceToPaddle = std::min<float>(MAX_DIST_AWAY, distanceToPaddle);
	
	// Lerp the colour and alpha between 0 and MAX_DIST_AWAY 
	Colour targetColour = GameViewConstants::GetInstance()->ITEM_BAD_COLOUR + distanceToPaddle * (GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR - GameViewConstants::GetInstance()->ITEM_BAD_COLOUR) / MAX_DIST_AWAY;
	float targetAlpha   = distanceToPaddle * 0.9f  / MAX_DIST_AWAY;
	
	paddleCamBallEffectList[0]->SetParticleColour(ESPInterval(targetColour.R()), ESPInterval(targetColour.G()), ESPInterval(targetColour.B()), ESPInterval(targetAlpha)), 
	paddleCamBallEffectList[0]->SetParticleSize(ESPInterval(ball.GetBallSize()));
	paddleCamBallEffectList[0]->Draw(camera);
	paddleCamBallEffectList[0]->Tick(dT);

	glPopMatrix();
}

/**
 * Draw effects associated with the ball camera mode when it has a target identifying itself - this happens in ball camera mode
 * to make it easier for the player to see the paddle.
 */
void GameESPAssets::DrawBallCamEffects(double dT, const Camera& camera, const GameBall& ball, const PlayerPaddle& paddle) {
	// Check to see if the paddle has any associated ball cam effects, if not, then create the effect and add it to the paddle first
	if (this->paddleEffects.find(GameItem::BallCamItem) == this->paddleEffects.end()) {
		// Didn't find an associated ball camera effect, so add one
		this->AddBallCamPaddleESPEffects(this->paddleEffects[GameItem::BallCamItem]);
	}
	std::vector<ESPPointEmitter*>& ballCamPaddleEffectList = this->paddleEffects[GameItem::BallCamItem];

	glPushMatrix();
	const Point2D& ballLoc		= ball.GetBounds().Center();
	const Point2D& paddleLoc	= paddle.GetCenterPosition();
	
	// The only effect so far is a spinning target on the paddle...
	glTranslatef(paddleLoc[0], paddleLoc[1], 0);

	// Change the colour and alpha of the target based on the proximity to the paddle
	float distanceToPaddle = Point2D::Distance(ballLoc, paddleLoc);
	const float MAX_DIST_AWAY = LevelPiece::PIECE_HEIGHT * 20.0f;
	distanceToPaddle = std::min<float>(MAX_DIST_AWAY, distanceToPaddle);
	
	// Lerp the colour and alpha between 0 and MAX_DIST_AWAY 
	Colour targetColour = GameViewConstants::GetInstance()->ITEM_BAD_COLOUR + distanceToPaddle * (GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR - GameViewConstants::GetInstance()->ITEM_BAD_COLOUR) / MAX_DIST_AWAY;
	float targetAlpha   = distanceToPaddle * 0.9f  / MAX_DIST_AWAY;
	
	ballCamPaddleEffectList[0]->SetParticleColour(ESPInterval(targetColour.R()), ESPInterval(targetColour.G()), ESPInterval(targetColour.B()), ESPInterval(targetAlpha)), 
	ballCamPaddleEffectList[0]->SetParticleSize(ESPInterval(paddle.GetHalfWidthTotal()*2));
	ballCamPaddleEffectList[0]->Draw(camera);
	ballCamPaddleEffectList[0]->Tick(dT);

	glPopMatrix();
}

void GameESPAssets::DrawBulletTimeBallsBoostEffects(double dT, const Camera& camera, const GameModel& gameModel) {
    const BallBoostModel* boostModel = gameModel.GetBallBoostModel();
    assert(boostModel != NULL);
    const Vector2D& boostDir  = boostModel->GetBallBoostDirection();

    float rotationAngleInDegs = Trig::radiansToDegrees(atan2(-boostDir[1], -boostDir[0]));

    // Tick the effect...
    float actualTickAmt = dT * boostModel->GetInverseTimeDialation();
    this->boostSparkleEmitterLight->Tick(actualTickAmt);
    this->boostSparkleEmitterDark->Tick(actualTickAmt);

    Point2D emitterPos;
    const std::list<GameBall*>& balls = gameModel.GetGameBalls();
    
    for (std::list<GameBall*>::const_iterator ballIter = balls.begin(); ballIter != balls.end(); ++ballIter) {
        GameBall* currBall = *ballIter;
        // Don't draw the effect for balls that cannot currently boost...
        if (!currBall->IsBallAllowedToBoost()) {
            continue;
        }
        emitterPos = currBall->GetCenterPosition2D() - (currBall->GetBounds().Radius() * boostDir);

        glPushMatrix();
        glTranslatef(emitterPos[0], emitterPos[1], 0);

        // The emitter, by default, is emitting on the x-axis direction, we need to rotate it to suit
        // the direction of the boost...
        glRotatef(rotationAngleInDegs, 0, 0, 1);

        this->boostSparkleEmitterDark->Draw(camera);
        this->boostSparkleEmitterLight->Draw(camera);
        
        glPopMatrix();
    }
}

void GameESPAssets::DrawBallBoostingEffects(double dT, const Camera& camera) {
    for (BallEffectsMapIter iter1 = this->boostBallEmitters.begin(); iter1 != this->boostBallEmitters.end();) {
        const GameBall* currBall = iter1->first;

        std::list<ESPPointEmitter*>& ballEmitters = iter1->second;

        if (!currBall->IsBallAllowedToBoostIgnoreAlreadyBoosting()) {
            // Kill the emitters for the ball since it can't boost...
            for (std::list<ESPPointEmitter*>::iterator iter2 = ballEmitters.begin(); iter2 != ballEmitters.end(); ++iter2) {
                ESPPointEmitter* currEmitter = *iter2;
			    delete currEmitter;
			    currEmitter = NULL;
            }
            ballEmitters.clear();
        }

        for (std::list<ESPPointEmitter*>::iterator iter2 = ballEmitters.begin(); iter2 != ballEmitters.end();) {
            ESPPointEmitter* currEmitter = *iter2;
		    // Check to see if dead, if so erase it...
		    if (currEmitter->IsDead()) {
			    delete currEmitter;
			    currEmitter = NULL;
			    iter2 = ballEmitters.erase(iter2);
		    }
		    else {
			    // Not dead yet, update its position based on the current ball's positon
                // and draw/tick it
                currEmitter->SetEmitPosition(currBall->GetCenterPosition());
                if (!currBall->GetDirection().IsZero()) {
                    currEmitter->SetEmitDirection(Vector3D(-currBall->GetDirection()));
                }
			    currEmitter->Draw(camera);
			    currEmitter->Tick(dT);
			    ++iter2;
		    }
        }
        
        // Remove the ball from the map of boost effect entries if it's done
        if (ballEmitters.empty()) {
            iter1 = this->boostBallEmitters.erase(iter1);
        }
        else {
            ++iter1;
        }
    }
}

/**
 * Draw particle effects associated with the ball, which get drawn behind the ball.
 */
void GameESPAssets::DrawBackgroundBallEffects(double dT, const Camera& camera, const GameBall& ball) {
	// Find the emitters corresponding to the given ball
	std::map<const GameBall*, std::list<ESPEmitter*> >::iterator tempIter = this->activeBallBGEmitters.find(&ball);
	if (tempIter == this->activeBallBGEmitters.end()) {
		return;
	}

	std::list<ESPEmitter*>& ballEmitters = tempIter->second;

	// Go through all the particles and do book keeping and drawing
	for (std::list<ESPEmitter*>::iterator iter = ballEmitters.begin(); iter != ballEmitters.end(); ) {
	
		ESPEmitter* curr = *iter;

		// Check to see if dead, if so erase it...
		if (curr->IsDead()) {
			delete curr;
			curr = NULL;
			iter = ballEmitters.erase(iter);
		}
		else {
			// Not dead yet so we draw and tick
			curr->Draw(camera);
			curr->Tick(dT);
			++iter;
		}
	}

	// If there are no background emitters left for the ball then remove that ball from
	// the ball background emitter mapping
	if (tempIter->second.size() == 0) {
		this->activeBallBGEmitters.erase(tempIter);
	}
}

/**
 * Draw particle effects associated with the paddle, which get drawn behind the paddle.
 */
void GameESPAssets::DrawBackgroundPaddleEffects(double dT, const Camera& camera) {
	// Go through all the particles and do book keeping and drawing
	for (std::list<ESPEmitter*>::iterator iter = this->activePaddleEmitters.begin();
		iter != this->activePaddleEmitters.end();) {
	
		ESPEmitter* curr = *iter;

		// Check to see if dead, if so erase it...
		if (curr->IsDead()) {
            iter = this->activePaddleEmitters.erase(iter);
            delete curr;
            curr = NULL;
		}
		else {
			// Not dead yet so we draw and tick
			curr->Draw(camera);
			curr->Tick(dT);
            ++iter;
		}
	}
}

void GameESPAssets::TickButDontDrawBackgroundPaddleEffects(double dT) {
    // Just tick, don't draw
	for (std::list<ESPEmitter*>::iterator iter = this->activePaddleEmitters.begin();
		iter != this->activePaddleEmitters.end();) {
	
		ESPEmitter* curr = *iter;

		// Check to see if dead, if so erase it...
		if (curr->IsDead()) {
            iter = this->activePaddleEmitters.erase(iter);
            delete curr;
            curr = NULL;
		}
		else {
			// Not dead yet so we tick
			curr->Tick(dT);
            ++iter;
		}
	}
}

void GameESPAssets::DrawTeslaLightningArcs(double dT, const Camera& camera) {
	for (std::map<std::pair<const TeslaBlock*, const TeslaBlock*>, std::list<ESPPointToPointBeam*> >::iterator iter = this->teslaLightningArcs.begin();
		iter != this->teslaLightningArcs.end(); ++iter) {

		std::list<ESPPointToPointBeam*>& arcs = iter->second;
		for (std::list<ESPPointToPointBeam*>::iterator arcIter = arcs.begin(); arcIter != arcs.end(); ++arcIter) {
			ESPPointToPointBeam* currArc = *arcIter;
			currArc->Tick(dT);
			currArc->Draw(camera);
		}
	}
}

/**
 * Draw all the beams that are currently active in the game.
 */
void GameESPAssets::DrawBeamEffects(double dT, const Camera& camera, const Vector3D& worldTranslation) {
	for (std::map<const Beam*, std::list<ESPEmitter*> >::iterator iter = this->activeBeamEmitters.begin();
		iter != this->activeBeamEmitters.end(); ++iter) {

		const Beam* beam = iter->first;
        UNUSED_VARIABLE(beam);
		std::list<ESPEmitter*>& beamEmitters = iter->second;
		assert(beam != NULL);

		// Update and draw the emitters...
		for (std::list<ESPEmitter*>::iterator emitIter = beamEmitters.begin(); emitIter != beamEmitters.end(); ++emitIter) {
			ESPEmitter* currentEmitter = *emitIter;
			assert(currentEmitter != NULL);
			currentEmitter->Draw(camera, worldTranslation, false);
			currentEmitter->Tick(dT);
		}
	}
}

void GameESPAssets::DrawTimerHUDEffect(double dT, const Camera& camera, GameItem::ItemType type) {

	// Try to find any emitters associated with the given item type
	std::map<GameItem::ItemType, std::list<ESPEmitter*> >::iterator foundEmitters = this->activeTimerHUDEmitters.find(type);
	if (foundEmitters == this->activeTimerHUDEmitters.end()) {
		return;
	}

	// Go through all the particles and do book keeping and drawing
	std::list<ESPEmitter*>& timerHUDEmitterList = foundEmitters->second;
	for (std::list<ESPEmitter*>::iterator iter = timerHUDEmitterList.begin();	iter != timerHUDEmitterList.end();) {
		ESPEmitter* currEmitter = *iter;

		// Check to see if dead, if so erase it...
		if (currEmitter->IsDead()) {
			iter = timerHUDEmitterList.erase(iter);
			delete currEmitter;
			currEmitter = NULL;
		}
		else {
			// Not dead yet so we draw and tick
			currEmitter->Draw(camera);
			currEmitter->Tick(dT);
			++iter;
		}
	}

	if (timerHUDEmitterList.size() == 0) {
		this->activeTimerHUDEmitters.erase(foundEmitters);
	}

}