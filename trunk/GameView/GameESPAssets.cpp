#include "GameESPAssets.h"
#include "GameFontAssetsManager.h"
#include "GameViewConstants.h"
#include "BallSafetyNetMesh.h"

#include "../GameModel/GameModel.h"
#include "../GameModel/GameLevel.h"
#include "../GameModel/GameBall.h"
#include "../GameModel/LevelPiece.h"
#include "../GameModel/Projectile.h"
#include "../GameModel/PlayerPaddle.h"

#include "../GameModel/GameItem.h"
#include "../GameModel/UberBallItem.h"
#include "../GameModel/GhostBallItem.h"
#include "../GameModel/BallSpeedItem.h"
#include "../GameModel/InvisiBallItem.h"
#include "../GameModel/LaserPaddleItem.h"
#include "../GameModel/PaddleSizeItem.h"
#include "../GameModel/BallSizeItem.h"
#include "../GameModel/OneUpItem.h"
#include "../GameModel/PaddleCamItem.h"

#include "../BlammoEngine/Texture.h"

#include "../ESPEngine/ESP.h"

#include "../ResourceManager.h"

GameESPAssets::GameESPAssets() : 
particleFader(1, 0), 
particleFireColourFader(ColourRGBA(1.0f, 1.0f, 0.1f, 1.0f), ColourRGBA(1.0f, 0.0f, 0.0f, 0.0f)),
particleCloudColourFader(ColourRGBA(1.0f, 1.0f, 1.0f, 1.0f), ColourRGBA(0.7f, 0.7f, 0.7f, 0.0f)),
particleFaderUberballTrail(Colour(1,0,0), 0.6f, 0), 

particleShrinkToNothing(1, 0),
particlePulseUberballAura(0, 0),
particlePulseItemDropAura(0, 0),
particlePulsePaddleLaser(0, 0),
particleSmallGrowth(1.0f, 1.3f), 
particleMediumGrowth(1.0f, 1.6f),
particleLargeGrowth(1.0f, 2.2f),
particleMediumShrink(1.0f, 0.25f),

ghostBallAccel1(Vector3D(1,1,1)),

paddleLaserGlowAura(NULL),
paddleLaserGlowSparks(NULL),

explosionRayRotatorCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.5f, ESPParticleRotateEffector::CLOCKWISE),
explosionRayRotatorCCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.5f, ESPParticleRotateEffector::COUNTER_CLOCKWISE),
smokeRotatorCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.25f, ESPParticleRotateEffector::CLOCKWISE),
smokeRotatorCCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.25f, ESPParticleRotateEffector::COUNTER_CLOCKWISE),
loopRotateEffectorCW(90.0f, ESPParticleRotateEffector::CLOCKWISE),

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
ballTargetTex(NULL) {

	this->InitESPTextures();
	this->InitStandaloneESPEffects();
}

GameESPAssets::~GameESPAssets() {
	this->KillAllActiveEffects();

	// Delete any effect textures
	for (std::vector<Texture2D*>::iterator iter = this->bangTextures.begin();
		iter != this->bangTextures.end(); iter++) {
		
		bool removed = ResourceManager::GetInstance()->ReleaseTextureResource(*iter);
		assert(removed);	
	}
	this->bangTextures.clear();

	for (std::vector<Texture2D*>::iterator iter = this->splatTextures.begin();
		iter != this->splatTextures.end(); iter++) {
		
		bool removed = ResourceManager::GetInstance()->ReleaseTextureResource(*iter);
		assert(removed);	
	}
	this->splatTextures.clear();

	for (std::vector<Texture2D*>::iterator iter = this->smokeTextures.begin();
		iter != this->smokeTextures.end(); iter++) {
		
		bool removed = ResourceManager::GetInstance()->ReleaseTextureResource(*iter);
		assert(removed);	
	}
	this->smokeTextures.clear();

	bool removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->circleGradientTex);
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
	removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->ballTargetTex);

	// Delete any standalone effects
	delete this->paddleLaserGlowAura;
	this->paddleLaserGlowAura = NULL;
	delete this->paddleLaserGlowSparks;
	this->paddleLaserGlowSparks = NULL;
}

/**
 * Kills all active effects currently being displayed in-game.
 */
void GameESPAssets::KillAllActiveEffects() {
	// Delete any leftover emitters
	for (std::list<ESPEmitter*>::iterator iter = this->activeGeneralEmitters.begin();
		iter != this->activeGeneralEmitters.end(); iter++) {
			delete *iter;
			*iter = NULL;
	}
	this->activeGeneralEmitters.clear();
	
	// Clear paddle BG emitters
	for (std::list<ESPEmitter*>::iterator iter = this->activePaddleEmitters.begin(); 
		iter != this->activePaddleEmitters.end(); iter++) {
		delete *iter;
		*iter = NULL;
	}
	this->activePaddleEmitters.clear();

	// Clear ball BG emiiters
	for (std::map<const GameBall*, std::list<ESPEmitter*>>::iterator iter = this->activeBallBGEmitters.begin(); 
		iter != this->activeBallBGEmitters.end(); iter++) {

		std::list<ESPEmitter*>& currEmitterList = iter->second;
		for (std::list<ESPEmitter*>::iterator iter2 = currEmitterList.begin(); iter2 != currEmitterList.end(); iter2++) {
				delete *iter2;
				*iter2 = NULL;
		}
		currEmitterList.clear();
	}
	this->activeBallBGEmitters.clear();

	// Clear item drop emitters
	for (std::map<const GameItem*, std::list<ESPEmitter*>>::iterator iter = this->activeItemDropEmitters.begin();	iter != this->activeItemDropEmitters.end(); iter++) {
			for (std::list<ESPEmitter*>::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++) {
				ESPEmitter* currEmitter = *iter2;
				delete currEmitter;
				currEmitter = NULL;
			}
			iter->second.clear();
	}
	this->activeItemDropEmitters.clear();

	// Clear projectile emitters
	for (std::map<const Projectile*, std::list<ESPEmitter*>>::iterator iter1 = this->activeProjectileEmitters.begin();
		iter1 != this->activeProjectileEmitters.end(); iter1++) {
	
			std::list<ESPEmitter*>& projEmitters = iter1->second;
			
			for (std::list<ESPEmitter*>::iterator iter2 = projEmitters.begin(); iter2 != projEmitters.end(); iter2++) {
				delete *iter2;
				*iter2 = NULL;
			}
			projEmitters.clear();
	}
	this->activeProjectileEmitters.clear();

	// Clear all ball emitters
	for (std::map<const GameBall*, std::map<std::string, std::vector<ESPPointEmitter*>>>::iterator iter1 = this->ballEffects.begin(); iter1 != this->ballEffects.end(); iter1++) {
		std::map<std::string, std::vector<ESPPointEmitter*>>& currBallEffects = iter1->second;
		
		for (std::map<std::string, std::vector<ESPPointEmitter*>>::iterator iter2 = currBallEffects.begin(); iter2 != currBallEffects.end(); iter2++) {
			std::vector<ESPPointEmitter*>& effectList = iter2->second;

			for (std::vector<ESPPointEmitter*>::iterator iter3 = effectList.begin(); iter3 != effectList.end(); iter3++) {
				ESPPointEmitter* currEmitter = *iter3;
				delete currEmitter;
				currEmitter = NULL;
			}
			effectList.clear();
		}
		currBallEffects.clear();
	}
	this->ballEffects.clear();

}

/**
 * Public function for destroying all effects associated with a given ball.
 * (for when the ball dies or something like that).
 */
void GameESPAssets::KillAllActiveBallEffects(const GameBall& ball) {
	// Check to see if there are any active effects for the ball, if not
	// then just exit this function
	std::map<const GameBall*, std::map<std::string, std::vector<ESPPointEmitter*>>>::iterator foundBallEffects = this->ballEffects.find(&ball);
	if (foundBallEffects == this->ballEffects.end()) {
		return;
	}

	// Iterate through all effects and delete them, then remove them from the list
	for (std::map<std::string, std::vector<ESPPointEmitter*>>::iterator iter = foundBallEffects->second.begin(); iter != foundBallEffects->second.end(); iter++) {
		std::vector<ESPPointEmitter*>& effectList = iter->second;
		for (std::vector<ESPPointEmitter*>::iterator iter2 = effectList.begin(); iter2 != effectList.end(); iter2++) {
			ESPPointEmitter* currEmitter = *iter2;
			delete currEmitter;
			currEmitter = NULL;
		}
		effectList.clear();
	}

	this->ballEffects.erase(foundBallEffects);
}

/**
 * Private helper function for initializing textures used in the ESP effects.
 */
void GameESPAssets::InitESPTextures() {
	// TODO: fix texture filtering...
	debug_output("Loading ESP Textures...");
	
	// Initialize bang textures (big boom thingys when there are explosions)
	if (this->bangTextures.size() == 0) {
		this->bangTextures.reserve(3);
		Texture2D* temp = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BANG1, Texture::Trilinear));
		assert(temp != NULL);
		this->bangTextures.push_back(temp);
		temp = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BANG2, Texture::Trilinear));
		assert(temp != NULL);
		this->bangTextures.push_back(temp);
		temp = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BANG3, Texture::Trilinear));
		assert(temp != NULL);
		this->bangTextures.push_back(temp);
	}
	
	// Initialize splat textures (splatty thingys when ink blocks and other messy, gooey things explode)
	if (this->splatTextures.size() == 0) {
		this->splatTextures.reserve(1);
		Texture2D* temp =  dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SPLAT1, Texture::Trilinear));
		assert(temp != NULL);
		this->splatTextures.push_back(temp);
	}

	// Initialize smoke textures (cartoony puffs of smoke)
	if (this->smokeTextures.size() == 0) {
		this->smokeTextures.reserve(6);
		Texture2D* temp = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE1, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE2, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE3, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE4, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE5, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE6, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);	
	}

	if (this->circleGradientTex == NULL) {
		this->circleGradientTex = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_CIRCLE_GRADIENT, Texture::Trilinear));
		assert(this->circleGradientTex != NULL);
	}
	if (this->starTex == NULL) {
		this->starTex = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_STAR, Texture::Trilinear));
		assert(this->starTex != NULL);	
	}
	if (this->starOutlineTex == NULL) {
		this->starOutlineTex = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_STAR_OUTLINE, Texture::Trilinear));
		assert(this->starOutlineTex != NULL);
	}
	if (this->evilStarTex == NULL) {
		this->evilStarTex = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_EVIL_STAR, Texture::Trilinear));
		assert(this->evilStarTex != NULL);	
	}
	if (this->evilStarOutlineTex == NULL) {
		this->evilStarOutlineTex = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_EVIL_STAR_OUTLINE, Texture::Trilinear));
		assert(this->evilStarOutlineTex != NULL);		
	}
	if (this->explosionTex == NULL) {
		this->explosionTex = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_EXPLOSION_CLOUD, Texture::Trilinear));
		assert(this->explosionTex != NULL);
	}
	if (this->explosionRayTex == NULL) {
		this->explosionRayTex = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_EXPLOSION_RAYS, Texture::Trilinear));
		assert(this->explosionRayTex != NULL);
	}
	if (this->laserBeamTex == NULL) {
		this->laserBeamTex = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_LASER_BEAM, Texture::Trilinear));
		assert(this->laserBeamTex != NULL);
	}
	if (this->upArrowTex == NULL) {
		this->upArrowTex = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_UP_ARROW, Texture::Trilinear));
		assert(this->upArrowTex != NULL);
	}
	if (this->ballTex == NULL) {
		this->ballTex = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BALL_LIFE_HUD, Texture::Trilinear));
		assert(this->ballTex != NULL);
	}
	if (this->ballTargetTex == NULL) {
		this->ballTargetTex = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BALLTARGET, Texture::Trilinear));
		assert(this->ballTargetTex != NULL);
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
	uberBallEmitterAura->SetParticleColour(ESPInterval(GameViewConstants::GetInstance()->UBER_BALL_COLOUR.R()), 
																				 ESPInterval(GameViewConstants::GetInstance()->UBER_BALL_COLOUR.G()), 
																				 ESPInterval(GameViewConstants::GetInstance()->UBER_BALL_COLOUR.B()), 
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
	
	effectsList.reserve(effectsList.size() + 2);
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
	ghostBallEmitterTrail->SetParticleColour(ESPInterval(GameViewConstants::GetInstance()->GHOST_BALL_COLOUR.R()), 
																					 ESPInterval(GameViewConstants::GetInstance()->GHOST_BALL_COLOUR.G()), 
																					 ESPInterval(GameViewConstants::GetInstance()->GHOST_BALL_COLOUR.B()), 
																					 ESPInterval(1.0f));

	ghostBallEmitterTrail->SetEmitAngleInDegrees(20);
	ghostBallEmitterTrail->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	ghostBallEmitterTrail->SetParticleAlignment(ESP::ScreenAligned);
	ghostBallEmitterTrail->SetEmitPosition(Point3D(0, 0, 0));
	ghostBallEmitterTrail->AddEffector(&this->particleFader);
	ghostBallEmitterTrail->AddEffector(&this->ghostBallAccel1);
	bool result = ghostBallEmitterTrail->SetParticles(GameESPAssets::NUM_GHOST_SMOKE_PARTICLES, &this->ghostBallSmoke);
	assert(result);

	effectsList.reserve(effectsList.size() + 1);
	effectsList.push_back(ghostBallEmitterTrail);
}

/**
 * Private helper function - sets up any ball effects that are associated with the paddle
 * camera effect.
 */
void GameESPAssets::AddPaddleCamBallESPEffects(std::vector<ESPPointEmitter*>& effectsList) {
	assert(effectsList.size() == 0);

	ESPPointEmitter* ballTargetEffect = new ESPPointEmitter();
	ballTargetEffect->SetSpawnDelta(ESPInterval(-1));
	ballTargetEffect->SetInitialSpd(ESPInterval(0));
	ballTargetEffect->SetParticleLife(ESPInterval(-1));
	ballTargetEffect->SetParticleSize(ESPInterval(GameBall::NormalSize));
	ballTargetEffect->SetEmitAngleInDegrees(0);
	ballTargetEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	ballTargetEffect->SetParticleAlignment(ESP::ScreenAligned);
	ballTargetEffect->SetEmitPosition(Point3D(0, 0, 0));
	ballTargetEffect->SetParticleColour(ESPInterval(1), ESPInterval(1), ESPInterval(1), ESPInterval(0.5f));

	ballTargetEffect->AddEffector(&this->loopRotateEffectorCW);

	bool result = ballTargetEffect->SetParticles(1, this->ballTargetTex);
	assert(result);

	effectsList.reserve(effectsList.size() + 1);
	effectsList.push_back(ballTargetEffect);
}

/**
 * Initialize the standalone effects for the paddle laser.
 */
void GameESPAssets::InitLaserPaddleESPEffects() {
	assert(this->paddleLaserGlowAura == NULL);
	assert(this->paddleLaserGlowSparks == NULL);

	this->paddleLaserGlowAura = new ESPPointEmitter();
	this->paddleLaserGlowAura->SetSpawnDelta(ESPInterval(-1));
	this->paddleLaserGlowAura->SetInitialSpd(ESPInterval(0));
	this->paddleLaserGlowAura->SetParticleLife(ESPInterval(-1));
	this->paddleLaserGlowAura->SetParticleSize(ESPInterval(1.5f));
	this->paddleLaserGlowAura->SetEmitAngleInDegrees(0);
	this->paddleLaserGlowAura->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->paddleLaserGlowAura->SetEmitPosition(Point3D(0, 0, 0));
	this->paddleLaserGlowAura->SetParticleColour(ESPInterval(0.5f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	this->paddleLaserGlowAura->AddEffector(&this->particlePulsePaddleLaser);
	bool result = this->paddleLaserGlowAura->SetParticles(1, this->circleGradientTex);
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
	
	// Initialize uberball effectors
	ScaleEffect uberBallPulseSettings;
	uberBallPulseSettings.pulseGrowthScale = 2.0f;
	uberBallPulseSettings.pulseRate = 1.5f;
	this->particlePulseUberballAura = ESPParticleScaleEffector(uberBallPulseSettings);

	// Ghost smoke effect used for ghostball
	this->ghostBallSmoke.SetTechnique(CgFxVolumetricEffect::SMOKESPRITE_TECHNIQUE_NAME);
	this->ghostBallSmoke.SetScale(0.5f);
	this->ghostBallSmoke.SetFrequency(0.25f);
	this->ghostBallSmoke.SetFlowDirection(Vector3D(0, 0, 1));
	this->ghostBallSmoke.SetMaskTexture(dynamic_cast<Texture2D*>(this->circleGradientTex));

	// Fire effect used in various things - like explosions and such.
	this->fireEffect.SetTechnique(CgFxVolumetricEffect::FIRESPRITE_TECHNIQUE_NAME);
	this->fireEffect.SetColour(Colour(1.00f, 1.00f, 1.00f));
	this->fireEffect.SetScale(0.5f);
	this->fireEffect.SetFrequency(1.0f);
	this->fireEffect.SetFlowDirection(Vector3D(0, 0, 1));
	this->fireEffect.SetMaskTexture(this->circleGradientTex);

	// Vapour trail - for cool noisy refraction
	this->vapourTrailEffect.SetTechnique(CgFxPostRefract::VAPOUR_TRAIL_TECHNIQUE_NAME);
	this->vapourTrailEffect.SetIndexOfRefraction(1.33f);
	this->vapourTrailEffect.SetWarpAmountParam(4.0f);
	this->vapourTrailEffect.SetScale(0.10f);
	this->vapourTrailEffect.SetFrequency(2.0f);
	this->vapourTrailEffect.SetMaskTexture(this->circleGradientTex);
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
void GameESPAssets::AddBounceLevelPieceEffect(const Camera& camera, const GameBall& ball, const LevelPiece& block) {
	// We don't do the effect for certain types of blocks...
	if (block.GetType() == LevelPiece::Bomb || block.GetType() == LevelPiece::Ink) {
		return;
	}

	// Add the new emitter to the list of active emitters
	this->activeGeneralEmitters.push_front(this->CreateBallBounceEffect(ball, Onomatoplex::BOUNCE));
}

/**
 * Adds ball bouncing effect when the ball bounces off the player paddle.
 */
void GameESPAssets::AddBouncePaddleEffect(const Camera& camera, const GameBall& ball, const PlayerPaddle& paddle) {
	if ((paddle.GetPaddleType() & PlayerPaddle::StickyPaddle) == PlayerPaddle::StickyPaddle) {
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
void GameESPAssets::AddBounceBallBallEffect(const Camera& camera, const GameBall& ball1, const GameBall& ball2) {
	// Obtain a reasonably centered position to show the effect
	const Point2D ball1Center = ball1.GetBounds().Center();
	const Point2D ball2Center = ball2.GetBounds().Center();
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
 * Add the typical block break effect - visible when you destroy a typical block.
 */
void GameESPAssets::AddBasicBlockBreakEffect(const Camera& camera, const LevelPiece& block) {
	assert(this->bangTextures.size() != 0);

	// Choose a random bang texture
	unsigned int randomBangTexIndex = Randomizer::GetInstance()->RandomUnsignedInt() % this->bangTextures.size();
	Texture2D* randomBangTex = this->bangTextures[randomBangTexIndex];
	
	ESPInterval bangLifeInterval		= ESPInterval(0.8f, 1.1f);
	ESPInterval bangOnoLifeInterval	= ESPInterval(bangLifeInterval.minValue + 0.3f, bangLifeInterval.maxValue + 0.3f);
	Point2D blockCenter = block.GetCenter();
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
	// TODO: make the severity more dynamic!!
	bangOnoEffect->SetParticles(1, bangTextLabel, Onomatoplex::EXPLOSION, Onomatoplex::NORMAL);

	// Lastly, add the new emitters to the list of active emitters in order of back to front
	this->activeGeneralEmitters.push_front(bangEffect);
	this->activeGeneralEmitters.push_back(bangOnoEffect);
}


void GameESPAssets::AddBallSafetyNetDestroyedEffect(const GameBall& ball) {
	assert(this->bangTextures.size() != 0);

	// Choose a random bang texture
	unsigned int randomBangTexIndex = Randomizer::GetInstance()->RandomUnsignedInt() % this->bangTextures.size();
	Texture2D* randomBangTex = this->bangTextures[randomBangTexIndex];
	
	ESPInterval bangLifeInterval		= ESPInterval(0.8f, 1.1f);
	ESPInterval bangOnoLifeInterval	= ESPInterval(bangLifeInterval.minValue + 0.3f, bangLifeInterval.maxValue + 0.3f);
	
	Point2D ballLoc = ball.GetBounds().Center();
	Point3D emitCenter  = Point3D(ballLoc[0], ballLoc[1] - ball.GetBounds().Radius() - BallSafetyNetMesh::SAFETY_NET_HEIGHT/2.0f, 0.0f);

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
 * Add the effect for when a bomb block is hit - explodey!
 */
void GameESPAssets::AddBombBlockBreakEffect(const Camera& camera, const LevelPiece& bomb) {
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

	// Lastly, add the new emitters to the list of active emitters
	this->activeGeneralEmitters.push_front(bombExplodeFireEffect2);
	this->activeGeneralEmitters.push_front(bombExplodeRayEffect);

	this->activeGeneralEmitters.push_back(bombOnoEffect);
}

/**
 * Add the effect for when an Ink block gets hit - splortch!
 */
void GameESPAssets::AddInkBlockBreakEffect(const Camera& camera, const LevelPiece& inkBlock, const GameLevel& level) {
	Point2D inkBlockCenter  = inkBlock.GetCenter();
	Point3D emitCenter  = Point3D(inkBlockCenter[0], inkBlockCenter[1], 0.0f);
	
	Colour inkBlockColour					= GameViewConstants::GetInstance()->INK_BLOCK_COLOUR;
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

	Point3D currCamPos = camera.GetCurrentCameraPosition();
	Vector3D negHalfLevelDim = -0.5 * Vector3D(level.GetLevelUnitWidth(), level.GetLevelUnitHeight(), 0.0f);
	Point3D emitCenterWorldCoords = emitCenter + negHalfLevelDim;
	Vector3D sprayVec = currCamPos - emitCenterWorldCoords;
	
	Vector3D inkySprayDir = Vector3D::Normalize(sprayVec);
	float distToCamera = sprayVec.length();

	// Inky spray at the camera
	ESPPointEmitter* inkySpray = new ESPPointEmitter();
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

	// Create an emitter for the bang texture
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
	assert(result);

	this->activeGeneralEmitters.push_back(inkyClouds1);
	this->activeGeneralEmitters.push_back(inkyClouds2);
	this->activeGeneralEmitters.push_back(inkyClouds3);
	this->activeGeneralEmitters.push_back(inkySpray);
	this->activeGeneralEmitters.push_back(splatEffect);
	this->activeGeneralEmitters.push_back(inkOnoEffect);
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

/**
 * Adds an effect for when an item is dropping and not yet acquired by the player.
 */
void GameESPAssets::AddItemDropEffect(const Camera& camera, const GameItem& item, bool showStars) {
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

	switch (item.GetItemType()) {
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
		default:
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
		itemDropEmitterTrail1->SetParticles(8, itemSpecificFillStarTex);
		
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
		itemDropEmitterTrail2->SetParticles(7, itemSpecificOutlineStarTex);
		
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
		itemDropEmitterTrail3->SetParticles(7, itemSpecificOutlineStarTex);

		// Add all the star emitters
		this->activeItemDropEmitters[&item].push_back(itemDropEmitterTrail1);
		this->activeItemDropEmitters[&item].push_back(itemDropEmitterTrail2);
		this->activeItemDropEmitters[&item].push_back(itemDropEmitterTrail3);
	}
	
}
/**
 * Removes an effect associated with a dropping item.
 */
void GameESPAssets::RemoveItemDropEffect(const Camera& camera, const GameItem& item) {

	// Try to find any effects associated with the given item
	std::map<const GameItem*, std::list<ESPEmitter*>>::iterator iter = this->activeItemDropEmitters.find(&item);
	if (iter != this->activeItemDropEmitters.end()) {
		
		// Delete all emitters associated with the item drop effect
		for (std::list<ESPEmitter*>::iterator effectIter = iter->second.begin(); effectIter != iter->second.end(); effectIter++) {
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
void GameESPAssets::TurnOffCurrentItemDropStars(const Camera& camera) {

	// Start by removing the effect for each item and then replace them with non-star effects
	for (std::map<const GameItem*, std::list<ESPEmitter*>>::iterator iter = this->activeItemDropEmitters.begin(); 
		iter != this->activeItemDropEmitters.end(); ++iter) {
		
		// Delete all emitters associated with the item drop effect
		for (std::list<ESPEmitter*>::iterator effectIter = iter->second.begin(); effectIter != iter->second.end(); effectIter++) {
			ESPEmitter* currEmitter = *effectIter;
			delete currEmitter;
			currEmitter = NULL;
		}
		iter->second.clear();
	
		this->AddItemDropEffect(camera, *iter->first, false);
	}
}

/**
 * Adds an effect for a active projectile in-game.
 */
void GameESPAssets::AddProjectileEffect(const GameModel& gameModel, const Projectile& projectile) {
	switch (projectile.GetType()) {
		case Projectile::PaddleLaserProjectile:
			this->AddLaserPaddleESPEffects(gameModel, projectile);
			break;
		default:
			assert(false);
			break;
	}
}

/**
 * Removes effects associated with the given projectile.
 */
void GameESPAssets::RemoveProjectileEffect(const Camera& camera, const Projectile& projectile) {
	
 	std::map<const Projectile*, std::list<ESPEmitter*>>::iterator projIter = this->activeProjectileEmitters.find(&projectile);
	if (projIter != this->activeProjectileEmitters.end()) {
			
		std::list<ESPEmitter*>& projEffects = projIter->second;
		for (std::list<ESPEmitter*>::iterator effectIter = projEffects.begin(); effectIter != projEffects.end(); effectIter++) {
			ESPEmitter* currEmitter = *effectIter;
			delete currEmitter;
			currEmitter = NULL;
		}
		projEffects.clear();

		this->activeProjectileEmitters.erase(&projectile);
	}
}

/**
 * Private helper function for making and adding a laser blast effect for the laser paddle item.
 */
void GameESPAssets::AddLaserPaddleESPEffects(const GameModel& gameModel, const Projectile& projectile) {
	assert(projectile.GetType() == Projectile::PaddleLaserProjectile);
	
	Point2D projectilePos2D = projectile.GetPosition();
	Point3D projectilePos3D = Point3D(projectilePos2D[0], projectilePos2D[1], 0.0f);
	float projectileSpd     = projectile.GetVelocityMagnitude();
	Vector2D projectileDir		= projectile.GetVelocityDirection();
	Vector3D projectileDir3D	= Vector3D(projectileDir[0], projectileDir[1], 0.0f);

	PlayerPaddle* paddle = gameModel.GetPlayerPaddle();

	// We only do the laser onomatopiea if we aren't in a special camera mode...
	if (!paddle->GetIsPaddleCameraOn()) {
		// Create laser onomatopiea
		ESPPointEmitter* laserOnoEffect = new ESPPointEmitter();
		// Set up the emitter...
		laserOnoEffect->SetSpawnDelta(ESPInterval(-1));
		laserOnoEffect->SetInitialSpd(ESPInterval(0.5f, 1.5f));
		laserOnoEffect->SetParticleLife(ESPInterval(0.75f, 1.25f));
		laserOnoEffect->SetParticleSize(ESPInterval(0.4f, 0.9f));
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
		ESPOnomataParticle* laserOnoParticle = new ESPOnomataParticle(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ElectricZap, GameFontAssetsManager::Small));
		laserOnoParticle->SetDropShadow(dpTemp);
		laserOnoParticle->SetOnomatoplexSound(Onomatoplex::SHOT, Onomatoplex::GOOD);
		laserOnoEffect->AddParticle(laserOnoParticle);
		
		this->activeGeneralEmitters.push_back(laserOnoEffect);
	}

	// Create the basic laser beam
	ESPPointEmitter* laserBeamEmitter = new ESPPointEmitter();
	laserBeamEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	laserBeamEmitter->SetInitialSpd(ESPInterval(0));
	laserBeamEmitter->SetParticleLife(ESPInterval(-1));
	if (paddle->GetIsPaddleCameraOn()) {
		laserBeamEmitter->SetParticleSize(ESPInterval(std::min<float>(PaddleLaser::PADDLELASER_WIDTH, PaddleLaser::PADDLELASER_HEIGHT)));
	}
	else {
		laserBeamEmitter->SetParticleSize(ESPInterval(PaddleLaser::PADDLELASER_WIDTH), ESPInterval(PaddleLaser::PADDLELASER_HEIGHT));
	}
	laserBeamEmitter->SetEmitAngleInDegrees(0);
	laserBeamEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	laserBeamEmitter->SetParticleAlignment(ESP::ScreenAligned);
	laserBeamEmitter->SetEmitPosition(Point3D(0,0,0));
	laserBeamEmitter->SetParticleColour(ESPInterval(0.5f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	laserBeamEmitter->SetParticles(1, this->laserBeamTex);

	// Create the slightly-pulsing-glowing aura
	ESPPointEmitter* laserAuraEmitter = new ESPPointEmitter();
	laserAuraEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	laserAuraEmitter->SetInitialSpd(ESPInterval(0));
	laserAuraEmitter->SetParticleLife(ESPInterval(-1));
	if (paddle->GetIsPaddleCameraOn()) {
		laserAuraEmitter->SetParticleSize(ESPInterval(std::min<float>(2*PaddleLaser::PADDLELASER_WIDTH, 1.8f*PaddleLaser::PADDLELASER_HEIGHT)));
	}
	else {
		laserAuraEmitter->SetParticleSize(ESPInterval(2*PaddleLaser::PADDLELASER_WIDTH), ESPInterval(1.8f*PaddleLaser::PADDLELASER_HEIGHT));
	}
	
	laserAuraEmitter->SetEmitAngleInDegrees(0);
	laserAuraEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	laserAuraEmitter->SetParticleAlignment(ESP::ScreenAligned);
	laserAuraEmitter->SetEmitPosition(Point3D(0,0,0));
	laserAuraEmitter->SetParticleColour(ESPInterval(0.65f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	laserAuraEmitter->AddEffector(&this->particlePulsePaddleLaser);
	laserAuraEmitter->SetParticles(1, this->circleGradientTex);

	// Create the trail effects
	ESPPointEmitter* laserTrailSparks = new ESPPointEmitter();
	laserTrailSparks->SetSpawnDelta(ESPInterval(0.01f, 0.033f));
	laserTrailSparks->SetInitialSpd(ESPInterval(projectileSpd));
	laserTrailSparks->SetParticleLife(ESPInterval(0.5f, 0.6f));
	laserTrailSparks->SetParticleSize(ESPInterval(0.4f, 0.85f));
	laserTrailSparks->SetParticleColour(ESPInterval(0.5f, 0.8f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	laserTrailSparks->SetEmitAngleInDegrees(15);
	laserTrailSparks->SetEmitDirection(Vector3D(-projectileDir[0], -projectileDir[1], 0.0f));
	laserTrailSparks->SetRadiusDeviationFromCenter(ESPInterval(0.5f*PaddleLaser::PADDLELASER_HALF_WIDTH));
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
 * Adds an effect for when a dropping item is acquired by the player paddle -
 * depending on the item type we create the effect.
 */
void GameESPAssets::AddItemAcquiredEffect(const Camera& camera, const PlayerPaddle& paddle, const GameItem& item) {
	ESPInterval redRandomColour(0.1f, 1.0f);
	ESPInterval greenRandomColour(0.1f, 1.0f);
	ESPInterval blueRandomColour(0.1f, 1.0f);
	ESPInterval redColour(0), greenColour(0), blueColour(0);
	ESPInterval alpha(1.0f);

	switch (item.GetItemType()) {
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

	// Pulsing aura
	ESPPointEmitter* paddlePulsingAura = new ESPPointEmitter();
	paddlePulsingAura->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	paddlePulsingAura->SetInitialSpd(ESPInterval(0));
	paddlePulsingAura->SetParticleLife(ESPInterval(1.0f));
	paddlePulsingAura->SetParticleSize(ESPInterval(2.0f * paddle.GetHalfWidthTotal()), ESPInterval(4.0f * paddle.GetHalfHeight()));
	paddlePulsingAura->SetEmitAngleInDegrees(0);
	paddlePulsingAura->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	paddlePulsingAura->SetParticleAlignment(ESP::ViewPointAligned);
	paddlePulsingAura->SetEmitPosition(Point3D(0, 0, 0));
	paddlePulsingAura->SetParticleColour(redColour, greenColour, blueColour, ESPInterval(0.6f));
	paddlePulsingAura->AddEffector(&this->particleLargeGrowth);
	paddlePulsingAura->AddEffector(&this->particleFader);
	bool result = paddlePulsingAura->SetParticles(1, this->circleGradientTex);
	assert(result);

	// Absorb glow sparks
	ESPPointEmitter* absorbGlowSparks = new ESPPointEmitter();
	absorbGlowSparks->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	absorbGlowSparks->SetInitialSpd(ESPInterval(3.0f, 4.5f));
	absorbGlowSparks->SetParticleLife(ESPInterval(0.75f, 1.0f));
	absorbGlowSparks->SetParticleSize(ESPInterval(0.1f, 0.75f));
	absorbGlowSparks->SetParticleColour(redColour, greenColour, blueColour, ESPInterval(0.6f));
	absorbGlowSparks->SetEmitAngleInDegrees(180);
	absorbGlowSparks->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	absorbGlowSparks->SetAsPointSpriteEmitter(true);
	absorbGlowSparks->SetIsReversed(true);
	absorbGlowSparks->SetEmitPosition(Point3D(0, 0, 0));
	absorbGlowSparks->AddEffector(&this->particleFader);
	absorbGlowSparks->AddEffector(&this->particleMediumShrink);
	result = absorbGlowSparks->SetParticles(NUM_ITEM_ACQUIRED_SPARKS, this->circleGradientTex);
	assert(result);
	
	this->activePaddleEmitters.push_back(paddlePulsingAura);
	this->activePaddleEmitters.push_back(absorbGlowSparks);
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



/**
 * Add the effect for when the player acquires a 1UP power-up.
 */
void GameESPAssets::AddOneUpEffect(const PlayerPaddle* paddle) {
	
	Point2D paddlePos2D = paddle->GetCenterPosition();
	Point3D paddlePos3D = Point3D(paddlePos2D[0], paddlePos2D[1], 0.0f);

	// Create an emitter for the 1UP text raising from the paddle
	ESPPointEmitter* oneUpTextEffect = new ESPPointEmitter();
	// Set up the emitter...
	oneUpTextEffect->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	oneUpTextEffect->SetInitialSpd(ESPInterval(2.0f));
	oneUpTextEffect->SetParticleLife(ESPInterval(2.0f));
	oneUpTextEffect->SetParticleSize(ESPInterval(1.0f, 1.0f), ESPInterval(1.0f, 1.0f));
	oneUpTextEffect->SetParticleRotation(ESPInterval(0.0f));
	oneUpTextEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	oneUpTextEffect->SetParticleAlignment(ESP::ScreenAligned);
	oneUpTextEffect->SetEmitPosition(paddlePos3D);
	oneUpTextEffect->SetEmitDirection(Vector3D(0, 1, 0));
	oneUpTextEffect->SetParticleColour(ESPInterval(GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR.R()), 
																		 ESPInterval(GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR.G()),
																		 ESPInterval(GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR.B()), ESPInterval(1.0f));
	// Add effectors...
	oneUpTextEffect->AddEffector(&this->particleFader);
	oneUpTextEffect->AddEffector(&this->particleSmallGrowth);

	// Add the single particle to the emitter...
	DropShadow dpTemp;
	dpTemp.amountPercentage = 0.15f;
	dpTemp.colour = Colour(1.0f, 1.0f, 1.0f);
	ESPOnomataParticle* oneUpTextParticle = new ESPOnomataParticle(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::HappyGood, GameFontAssetsManager::Medium), "1UP!");
	oneUpTextParticle->SetDropShadow(dpTemp);
	oneUpTextEffect->AddParticle(oneUpTextParticle);

	// Lastly, add the new emitters to the list of active emitters in order of back to front
	this->activeGeneralEmitters.push_back(oneUpTextEffect);
}

/**
 * Adds an effect based on the given game item being activated or deactivated.
 */
void GameESPAssets::SetItemEffect(const GameItem& item, const GameModel& gameModel) {
	if (item.GetName() == UberBallItem::UBER_BALL_ITEM_NAME) {
		const GameBall* ballAffected = item.GetBallAffected();
		assert(ballAffected != NULL);

		// If there are any effects assigned for the uber ball then we need to reset the trail
		std::map<const GameBall*, std::map<std::string, std::vector<ESPPointEmitter*>>>::iterator foundBallEffects = this->ballEffects.find(ballAffected);
		if (foundBallEffects != this->ballEffects.end()) {
			std::map<std::string, std::vector<ESPPointEmitter*>>::iterator foundUberBallFX = foundBallEffects->second.find(UberBallItem::UBER_BALL_ITEM_NAME);
			if (foundUberBallFX != foundBallEffects->second.end()) {
				std::vector<ESPPointEmitter*>& uberBallEffectsList = foundUberBallFX->second;
				assert(uberBallEffectsList.size() > 0);
				uberBallEffectsList[0]->Reset();
			}
		}

	}
	else if (item.GetName() == LaserPaddleItem::LASER_PADDLE_ITEM_NAME) {
		this->paddleLaserGlowAura->Reset();
		this->paddleLaserGlowSparks->Reset();
	}
	else if (item.GetName() == PaddleSizeItem::PADDLE_GROW_ITEM_NAME) {
		this->AddPaddleGrowEffect();
	}
	else if (item.GetName() == PaddleSizeItem::PADDLE_SHRINK_ITEM_NAME) {
		this->AddPaddleShrinkEffect();
	}
	else if (item.GetName() == BallSizeItem::BALL_GROW_ITEM_NAME) {
		this->AddBallGrowEffect(item.GetBallAffected());
	}
	else if (item.GetName() == BallSizeItem::BALL_SHRINK_ITEM_NAME) {
		this->AddBallShrinkEffect(item.GetBallAffected());
	}
	else if (item.GetName() == OneUpItem::ONE_UP_ITEM_NAME) {
		this->AddOneUpEffect(gameModel.GetPlayerPaddle());
	}
}

/**
 * Draw call that will draw all active effects and clear up all inactive effects
 * for the game. These are particle effects that require no render to texture or other fancy
 * shmancy type stuffs.
 */
void GameESPAssets::DrawParticleEffects(double dT, const Camera& camera) {
	
	std::list<std::list<ESPEmitter*>::iterator> removeElements;

	// Go through all the particles and do book keeping and drawing
	for (std::list<ESPEmitter*>::iterator iter = this->activeGeneralEmitters.begin();
		iter != this->activeGeneralEmitters.end(); iter++) {
	
		ESPEmitter* curr = *iter;

		// Check to see if dead, if so erase it...
		if (curr->IsDead()) {
			removeElements.push_back(iter);
		}
		else {
			// Not dead yet so we draw and tick
			curr->Draw(camera);
			curr->Tick(dT);
		}
	}

	for (std::list<std::list<ESPEmitter*>::iterator>::iterator iter = removeElements.begin();
		iter != removeElements.end(); iter++) {
			ESPEmitter* currEmitter = (**iter);
			this->activeGeneralEmitters.erase(*iter);
			delete currEmitter;
			currEmitter = NULL;
	}

	this->DrawProjectileEffects(dT, camera);
}

/**
 * Update and draw all projectile effects that are currently active.
 */
void GameESPAssets::DrawProjectileEffects(double dT, const Camera& camera) {
	for (std::map<const Projectile*, std::list<ESPEmitter*>>::iterator iter = this->activeProjectileEmitters.begin();
		iter != this->activeProjectileEmitters.end(); iter++) {
		
		const Projectile* currProjectile = iter->first;
		std::list<ESPEmitter*>& projEmitters = iter->second;

		assert(currProjectile != NULL);
		Point2D projectilePos2D		= currProjectile->GetPosition();

		// Update and draw the emitters, background then foreground...
		for (std::list<ESPEmitter*>::iterator emitIter = projEmitters.begin(); emitIter != projEmitters.end(); emitIter++) {
			this->DrawProjectileEmitter(dT, camera, projectilePos2D, *emitIter);
		}
	}
}

/**
 * Private helper function for drawing a single projectile at a given position.
 */
void GameESPAssets::DrawProjectileEmitter(double dT, const Camera& camera, const Point2D& projectilePos2D, ESPEmitter* projectileEmitter) {
	bool movesWithProjectile = projectileEmitter->OnlySpawnsOnce();
	if (movesWithProjectile) {
		glPushMatrix();
		glTranslatef(projectilePos2D[0], projectilePos2D[1], 0.0f);
	}
	else {
		ESPPointEmitter* temp = dynamic_cast<ESPPointEmitter*>(projectileEmitter);
		assert(temp != NULL);
		temp->SetEmitPosition(Point3D(projectilePos2D[0], projectilePos2D[1], 0.0f));
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
	std::map<const GameItem*, std::list<ESPEmitter*>>::iterator itemDropEffectIter = this->activeItemDropEmitters.find(&item);
	
	// If no effects were found then exit...
	if (itemDropEffectIter == this->activeItemDropEmitters.end()) {
		return;
	}
	
	// Draw the appropriate effects
	assert(itemDropEffectIter->second.size() > 0);
	for (std::list<ESPEmitter*>::iterator iter = itemDropEffectIter->second.begin(); iter != itemDropEffectIter->second.end(); iter++) {
		ESPEmitter* currEmitter = *iter;
		currEmitter->Draw(camera);
		currEmitter->Tick(dT);
	}
}

/**
 * Draw particle effects associated with the uberball.
 */
void GameESPAssets::DrawUberBallEffects(double dT, const Camera& camera, const GameBall& ball) {
	// Check to see if the ball has any associated uber ball effects, if not, then
	// create the effect and add it to the ball first
	std::map<const GameBall*, std::map<std::string, std::vector<ESPPointEmitter*>>>::iterator foundBallEffects = this->ballEffects.find(&ball);
	
	if (foundBallEffects == this->ballEffects.end()) {
		// Didn't even find a ball ... add one
		foundBallEffects = this->ballEffects.insert(std::make_pair(&ball, std::map<std::string, std::vector<ESPPointEmitter*>>())).first;
	}

	if (foundBallEffects->second.find(UberBallItem::UBER_BALL_ITEM_NAME) == foundBallEffects->second.end()) {
		// Didn't find an associated uber ball effect, so add one
		this->AddUberBallESPEffects(this->ballEffects[&ball][UberBallItem::UBER_BALL_ITEM_NAME]);
	}
	std::vector<ESPPointEmitter*>& uberBallEffectList = this->ballEffects[&ball][UberBallItem::UBER_BALL_ITEM_NAME];

	Vector2D ballDir = ball.GetDirection();
	Point2D ballPos  = ball.GetBounds().Center();

	glPushMatrix();
	Point2D loc = ball.GetBounds().Center();
	glTranslatef(loc[0], loc[1], 0);

	uberBallEffectList[1]->Draw(camera);
	uberBallEffectList[1]->Tick(dT);

	glPopMatrix();

	// Draw the trail...
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
	std::map<const GameBall*, std::map<std::string, std::vector<ESPPointEmitter*>>>::iterator foundBallEffects = this->ballEffects.find(&ball);
	
	if (foundBallEffects == this->ballEffects.end()) {
		// Didn't even find a ball ... add one
		foundBallEffects = this->ballEffects.insert(std::make_pair(&ball, std::map<std::string, std::vector<ESPPointEmitter*>>())).first;
	}

	if (foundBallEffects->second.find(GhostBallItem::GHOST_BALL_ITEM_NAME) == foundBallEffects->second.end()) {
		// Didn't find an associated uber ball effect, so add one
		this->AddGhostBallESPEffects(this->ballEffects[&ball][GhostBallItem::GHOST_BALL_ITEM_NAME]);
	}
	std::vector<ESPPointEmitter*>& ghostBallEffectList = this->ballEffects[&ball][GhostBallItem::GHOST_BALL_ITEM_NAME];

	glPushMatrix();
	Point2D loc = ball.GetBounds().Center();
	glTranslatef(loc[0], loc[1], 0);

	// Rotate the negative ball -vel direction by some random amount and then affect the particle's velocities
	// by it, this gives the impression that the particles are waving around mysteriously (ghostlike... one might say)
	double randomDegrees = Randomizer::GetInstance()->RandomNumNegOneToOne() * 90;
	Vector2D ballDir    = ball.GetDirection();
	Vector3D negBallDir = Vector3D(-ballDir[0], -ballDir[1], 0.0f);
	Vector3D accel = Matrix4x4::rotationMatrix('z', static_cast<float>(randomDegrees), true) * negBallDir;
	accel = ball.GetSpeed() * 4.0 * accel;
	this->ghostBallAccel1.SetAcceleration(accel);

	// Create a kind of trail for the ball...
	ghostBallEffectList[0]->Draw(camera);
	ghostBallEffectList[0]->Tick(dT);
	
	glPopMatrix();
}

/**
 * Draw effects associated with the ball when it has a target identifying itself
 */
void GameESPAssets::DrawTargetBallEffects(double dT, const Camera& camera, const GameBall& ball, const PlayerPaddle& paddle) {
	// Check to see if the ball has any associated camera paddle ball effects, if not, then
	// create the effect and add it to the ball first
	std::map<const GameBall*, std::map<std::string, std::vector<ESPPointEmitter*>>>::iterator foundBallEffects = this->ballEffects.find(&ball);
	
	if (foundBallEffects == this->ballEffects.end()) {
		// Didn't even find a ball ... add one
		foundBallEffects = this->ballEffects.insert(std::make_pair(&ball, std::map<std::string, std::vector<ESPPointEmitter*>>())).first;
	}

	if (foundBallEffects->second.find(PaddleCamItem::PADDLE_CAM_ITEM_NAME) == foundBallEffects->second.end()) {
		// Didn't find an associated paddle camera ball effect, so add one
		this->AddPaddleCamBallESPEffects(this->ballEffects[&ball][PaddleCamItem::PADDLE_CAM_ITEM_NAME]);
	}
	std::vector<ESPPointEmitter*>& paddleCamBallEffectList = this->ballEffects[&ball][PaddleCamItem::PADDLE_CAM_ITEM_NAME];

	glPushMatrix();
	Point2D ballLoc		= ball.GetBounds().Center();
	Point2D paddleLoc	= paddle.GetCenterPosition();
	
	glTranslatef(ballLoc[0], ballLoc[1], 0);

	// Change the colour and alpha of the target based on the proximity to the paddle
	float distanceToPaddle = Point2D::Distance(ballLoc, paddleLoc);
	const float MAX_DIST_AWAY = LevelPiece::PIECE_HEIGHT * 20.0f;
	distanceToPaddle = std::min<float>(MAX_DIST_AWAY, distanceToPaddle);
	
	// Lerp the colour and alpha between 0 and MAX_DIST_AWAY 
	Colour targetColour = GameViewConstants::GetInstance()->ITEM_BAD_COLOUR + distanceToPaddle * (GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR - GameViewConstants::GetInstance()->ITEM_BAD_COLOUR) / MAX_DIST_AWAY;
	float targetAlpha   = distanceToPaddle * 0.9f  / MAX_DIST_AWAY;
	
	paddleCamBallEffectList[0]->SetParticleColour(ESPInterval(targetColour.R()), ESPInterval(targetColour.G()), ESPInterval(targetColour.B()), ESPInterval(targetAlpha)), 
	paddleCamBallEffectList[0]->Draw(camera);
	paddleCamBallEffectList[0]->Tick(dT);

	glPopMatrix();
}

/**
 * Draw particle effects associated with the ball, which get drawn behind the ball.
 */
void GameESPAssets::DrawBackgroundBallEffects(double dT, const Camera& camera, const GameBall& ball) {
	std::list<std::list<ESPEmitter*>::iterator> removeElements;

	// Find the emitters corresponding to the given ball
	std::map<const GameBall*, std::list<ESPEmitter*>>::iterator tempIter = this->activeBallBGEmitters.find(&ball);
	if (tempIter == this->activeBallBGEmitters.end()) {
		return;
	}

	std::list<ESPEmitter*>& ballEmitters = tempIter->second;

	// Go through all the particles and do book keeping and drawing
	for (std::list<ESPEmitter*>::iterator iter = ballEmitters.begin(); iter != ballEmitters.end(); iter++) {
	
		ESPEmitter* curr = *iter;

		// Check to see if dead, if so erase it...
		if (curr->IsDead()) {
			removeElements.push_back(iter);
		}
		else {
			// Not dead yet so we draw and tick
			curr->Draw(camera);
			curr->Tick(dT);
		}
	}

	for (std::list<std::list<ESPEmitter*>::iterator>::iterator iter = removeElements.begin(); iter != removeElements.end(); iter++) {
			ESPEmitter* currEmitter = (**iter);
			tempIter->second.erase(*iter);
			delete currEmitter;
			currEmitter = NULL;
	}

	if (tempIter->second.size() == 0) {
		this->activeBallBGEmitters.erase(tempIter);
	}
}

/**
 * Draw particle effects associated with the paddle, which get drawn behind the paddle.
 */
void GameESPAssets::DrawBackgroundPaddleEffects(double dT, const Camera& camera, const PlayerPaddle& paddle) {
	std::list<std::list<ESPEmitter*>::iterator> removeElements;

	// Go through all the particles and do book keeping and drawing
	for (std::list<ESPEmitter*>::iterator iter = this->activePaddleEmitters.begin();
		iter != this->activePaddleEmitters.end(); iter++) {
	
		ESPEmitter* curr = *iter;

		// Check to see if dead, if so erase it...
		if (curr->IsDead()) {
			removeElements.push_back(iter);
		}
		else {
			// Not dead yet so we draw and tick
			curr->Draw(camera);
			curr->Tick(dT);
		}
	}

	for (std::list<std::list<ESPEmitter*>::iterator>::iterator iter = removeElements.begin(); iter != removeElements.end(); iter++) {
			ESPEmitter* currEmitter = (**iter);
			this->activePaddleEmitters.erase(*iter);
			delete currEmitter;
			currEmitter = NULL;
	}
}

/**
 * Draw particle effects associated with the laser paddle.
 * NOTE: You must transform these effects to be where the paddle is first!
 */
void GameESPAssets::DrawPaddleLaserEffects(double dT, const Camera& camera, const PlayerPaddle& paddle) {
	float effectPos = paddle.GetHalfHeight() + this->paddleLaserGlowAura->GetParticleSizeY().maxValue * 0.5f;
	this->paddleLaserGlowAura->SetEmitPosition(Point3D(0, effectPos, 0));
	this->paddleLaserGlowSparks->SetEmitPosition(Point3D(0, effectPos, 0));

	this->paddleLaserGlowAura->Draw(camera);
	this->paddleLaserGlowAura->Tick(dT);
	this->paddleLaserGlowSparks->Draw(camera);
	this->paddleLaserGlowSparks->Tick(dT);
}