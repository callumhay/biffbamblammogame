#include "GameESPAssets.h"
#include "GameFontAssetsManager.h"
#include "GameViewConstants.h"


#include "../GameModel/GameBall.h"
#include "../GameModel/LevelPiece.h"
#include "../GameModel/GameItem.h"
#include "../GameModel/UberBallItem.h"
#include "../GameModel/BallSpeedItem.h"
#include "../GameModel/InvisiBallItem.h"
#include "../GameModel/LaserPaddleItem.h"
#include "../GameModel/Projectile.h"
#include "../GameModel/PlayerPaddle.h"

#include "../BlammoEngine/Texture2D.h"


#include "../ESPEngine/ESP.h"

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

uberBallEmitterAura(NULL), 
uberBallEmitterTrail(NULL),
ghostBallEmitterTrail(NULL),
paddleLaserGlowAura(NULL),
paddleLaserGlowSparks(NULL),

explosionRayRotatorCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.5f, ESPParticleRotateEffector::CLOCKWISE),
explosionRayRotatorCCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.5f, ESPParticleRotateEffector::COUNTER_CLOCKWISE),

circleGradientTex(NULL), 
starTex(NULL), 
starOutlineTex(NULL),
explosionTex(NULL),
explosionRayTex(NULL),
laserBeamTex(NULL),

oldBallDir(0,0) {

	this->InitESPTextures();
	this->InitStandaloneESPEffects();
}

GameESPAssets::~GameESPAssets() {
	this->KillAllActiveEffects();

	// Delete any effect textures
	for (std::vector<Texture2D*>::iterator iter = this->bangTextures.begin();
		iter != this->bangTextures.end(); iter++) {
		delete *iter;
	}
	this->bangTextures.clear();

	for (std::vector<Texture2D*>::iterator iter = this->smokeTextures.begin();
		iter != this->smokeTextures.end(); iter++) {
		delete *iter;
	}
	this->smokeTextures.clear();

	delete this->circleGradientTex;
	this->circleGradientTex = NULL;
	delete this->starTex;
	this->starTex = NULL;
	delete this->starOutlineTex;
	this->starOutlineTex = NULL;
	delete this->explosionTex;
	this->explosionTex = NULL;
	delete this->explosionRayTex;
	this->explosionRayTex = NULL;
	delete this->laserBeamTex;
	this->laserBeamTex = NULL;

	// Delete any standalone effects
	delete this->uberBallEmitterAura;
	this->uberBallEmitterAura = NULL;
	delete this->uberBallEmitterTrail;
	this->uberBallEmitterTrail = NULL;
	delete this->ghostBallEmitterTrail;
	this->ghostBallEmitterTrail = NULL;
	delete this->paddleLaserGlowAura;
	this->paddleLaserGlowAura = NULL;
	delete this->paddleLaserGlowSparks;
	this->paddleLaserGlowSparks = NULL;

	for (std::vector<ESPShaderParticle*>::iterator iter = this->ghostSmokeParticles.begin(); iter != this->ghostSmokeParticles.end(); iter++) {
		delete *iter;
	}
	this->ghostSmokeParticles.clear();
}

/**
 * Kills all active effects currently being displayed in-game.
 */
void GameESPAssets::KillAllActiveEffects() {
	// Delete any leftover emitters
	for (std::list<ESPEmitter*>::iterator iter = this->activeGeneralEmitters.begin();
		iter != this->activeGeneralEmitters.end(); iter++) {
			delete *iter;
	}
	this->activeGeneralEmitters.clear();
	
	// Clear item drop emitters
	for (std::map<const GameItem*, std::list<ESPEmitter*>>::iterator iter = this->activeItemDropEmitters.begin();
		iter != this->activeItemDropEmitters.end(); iter++) {
	
			std::list<ESPEmitter*> currEmitterList = (*iter).second;
			for (std::list<ESPEmitter*>::iterator iter = currEmitterList.begin();
				iter != currEmitterList.end(); iter++) {
				delete *iter;
				*iter = NULL;
			}
			currEmitterList.clear();
	}
	this->activeItemDropEmitters.clear();

	// Clear projectile emitters
	for (std::map<const Projectile*, std::pair<std::list<ESPPointEmitter*>, std::list<ESPPointEmitter*>>>::iterator iter1 = this->activeProjectileEmitters.begin();
		iter1 != this->activeProjectileEmitters.end(); iter1++) {
	
			std::pair<std::list<ESPPointEmitter*>, std::list<ESPPointEmitter*>> fgbgPair = (*iter1).second;
			
			for (std::list<ESPPointEmitter*>::iterator iter2 = fgbgPair.first.begin();
				iter2 != fgbgPair.first.end(); iter2++) {
				delete *iter2;
				*iter2 = NULL;
			}
			fgbgPair.first.clear();
			
			for (std::list<ESPPointEmitter*>::iterator iter2 = fgbgPair.second.begin();
				iter2 != fgbgPair.second.end(); iter2++) {
				delete *iter2;
				*iter2 = NULL;
			}
			fgbgPair.second.clear();

	}
	this->activeProjectileEmitters.clear();
}

/**
 * Private helper function for initializing textures used in the ESP effects.
 */
void GameESPAssets::InitESPTextures() {
	// TODO: fix texture filtering...
	debug_output("Loading ESP Textures...");
	
	// Initialize bang textures (big boom thingys when there are explosions)
	if (this->bangTextures.size() == 0) {
		Texture2D* temp = Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_BANG1, Texture::Trilinear);
		assert(temp != NULL);
		this->bangTextures.push_back(temp);
		temp = Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_BANG2, Texture::Trilinear);
		assert(temp != NULL);
		this->bangTextures.push_back(temp);
		temp = Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_BANG3, Texture::Trilinear);
		assert(temp != NULL);
		this->bangTextures.push_back(temp);
	}
	
	// Initialize smoke textures (cartoony puffs of smoke)
	if (this->smokeTextures.size() == 0) {
		Texture2D* temp = Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_SMOKE1, Texture::Trilinear);
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_SMOKE2, Texture::Trilinear);
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_SMOKE3, Texture::Trilinear);
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_SMOKE4, Texture::Trilinear);
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_SMOKE5, Texture::Trilinear);
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_SMOKE6, Texture::Trilinear);
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);	
	}

	if (this->circleGradientTex == NULL) {
		this->circleGradientTex = Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_CIRCLE_GRADIENT, Texture::Trilinear);
		assert(this->circleGradientTex != NULL);
	}
	if (this->starTex == NULL) {
		this->starTex = Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_STAR, Texture::Trilinear);
		assert(this->starTex != NULL);	
	}
	if (this->starOutlineTex == NULL) {
		this->starOutlineTex = Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_STAR_OUTLINE, Texture::Trilinear);
		assert(this->starOutlineTex != NULL);
	}
	if (this->explosionTex == NULL) {
		this->explosionTex = Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_EXPLOSION_CLOUD, Texture::Trilinear);
		assert(this->explosionTex != NULL);
	}
	if (this->explosionRayTex == NULL) {
		this->explosionRayTex = Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_EXPLOSION_RAYS, Texture::Trilinear);
		assert(this->explosionRayTex != NULL);
	}
	if (this->laserBeamTex == NULL) {
		this->laserBeamTex = Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_LASER_BEAM, Texture::Trilinear);
		assert(this->laserBeamTex != NULL);
	}
}

// Private helper function for initializing the uber ball's effects
void GameESPAssets::InitUberBallESPEffects() {
	assert(this->uberBallEmitterAura == NULL);
	assert(this->uberBallEmitterTrail == NULL);


	// Initialize uberball effectors
	ScaleEffect uberBallPulseSettings;
	uberBallPulseSettings.pulseGrowthScale = 2.0f;
	uberBallPulseSettings.pulseRate = 1.5f;
	this->particlePulseUberballAura = ESPParticleScaleEffector(uberBallPulseSettings);

	// Set up the uberball emitters...
	this->uberBallEmitterAura = new ESPPointEmitter();
	this->uberBallEmitterAura->SetSpawnDelta(ESPInterval(-1));
	this->uberBallEmitterAura->SetInitialSpd(ESPInterval(0));
	this->uberBallEmitterAura->SetParticleLife(ESPInterval(-1));
	this->uberBallEmitterAura->SetParticleSize(ESPInterval(1.5f));
	this->uberBallEmitterAura->SetEmitAngleInDegrees(0);
	this->uberBallEmitterAura->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->uberBallEmitterAura->SetParticleAlignment(ESP::ScreenAligned);
	this->uberBallEmitterAura->SetEmitPosition(Point3D(0, 0, 0));
	this->uberBallEmitterAura->SetParticleColour(ESPInterval(1), ESPInterval(0), ESPInterval(0), ESPInterval(0.75f));
	this->uberBallEmitterAura->AddEffector(&this->particlePulseUberballAura);
	bool result = this->uberBallEmitterAura->SetParticles(1, this->circleGradientTex);
	assert(result);

	this->uberBallEmitterTrail = new ESPPointEmitter();
	this->uberBallEmitterTrail->SetSpawnDelta(ESPInterval(0.02f));
	this->uberBallEmitterTrail->SetInitialSpd(ESPInterval(5.0f));
	this->uberBallEmitterTrail->SetParticleLife(ESPInterval(1.3f));
	this->uberBallEmitterTrail->SetParticleSize(ESPInterval(1.3f), ESPInterval(1.3f));
	this->uberBallEmitterTrail->SetEmitAngleInDegrees(0);
	this->uberBallEmitterTrail->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->uberBallEmitterTrail->SetParticleAlignment(ESP::ViewPointAligned);
	this->uberBallEmitterTrail->SetEmitPosition(Point3D(0, 0, 0));
	this->uberBallEmitterTrail->AddEffector(&this->particleFaderUberballTrail);
	this->uberBallEmitterTrail->AddEffector(&this->particleShrinkToNothing);
	result = this->uberBallEmitterTrail->SetParticles(40, this->circleGradientTex);
	assert(result);
}

// Private helper function for initializing the ghost ball's effects
void GameESPAssets::InitGhostBallESPEffects() {
	assert(this->ghostBallEmitterTrail == NULL);

	this->ghostBallSmoke.SetTechnique(CgFxVolumetricEffect::SMOKESPRITE_TECHNIQUE_NAME);
	this->ghostBallSmoke.SetScale(0.5f);
	this->ghostBallSmoke.SetFrequency(0.25f);
	this->ghostBallSmoke.SetFlowDirection(Vector3D(0, 0, 1));
	this->ghostBallSmoke.SetMaskTexture(this->circleGradientTex);

	this->ghostBallEmitterTrail = new ESPPointEmitter();
	this->ghostBallEmitterTrail->SetSpawnDelta(ESPInterval(0.01f));
	this->ghostBallEmitterTrail->SetInitialSpd(ESPInterval(0.0f));
	this->ghostBallEmitterTrail->SetParticleLife(ESPInterval(0.5f));
	this->ghostBallEmitterTrail->SetParticleSize(ESPInterval(1.5f, 2.0f));
	this->ghostBallEmitterTrail->SetParticleColour(ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	this->ghostBallEmitterTrail->SetEmitAngleInDegrees(20);
	this->ghostBallEmitterTrail->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->ghostBallEmitterTrail->SetParticleAlignment(ESP::ViewPointAligned);
	this->ghostBallEmitterTrail->SetEmitPosition(Point3D(0, 0, 0));
	this->ghostBallEmitterTrail->AddEffector(&this->particleFader);
	this->ghostBallEmitterTrail->AddEffector(&this->ghostBallAccel1);

	for (int i = 0; i < GameESPAssets::NUM_GHOST_SMOKE_PARTICLES; i++) {
		ESPShaderParticle* temp = new ESPShaderParticle(&this->ghostBallSmoke);
		this->ghostSmokeParticles.push_back(temp);
		this->ghostBallEmitterTrail->AddParticle(temp);
	}
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
	this->paddleLaserGlowAura->SetParticleAlignment(ESP::ScreenAligned);
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
	this->paddleLaserGlowSparks->SetParticleAlignment(ESP::ScreenAligned);
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

	this->InitUberBallESPEffects();
	this->InitGhostBallESPEffects();
	
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
 * Adds a ball bouncing ESP - the effect that occurs when a ball typically
 * bounces off some object (e.g., block, paddle).
 */
void GameESPAssets::AddBallBounceEffect(const Camera& camera, const GameBall& ball) {
	// In this case the effect is a basic onomatopeia word that occurs at the
	// position of the ball

	ESPPointEmitter* bounceEffect = new ESPPointEmitter();
	// Set up the emitter...
	bounceEffect->SetSpawnDelta(ESPInterval(-1, -1));
	bounceEffect->SetInitialSpd(ESPInterval(0.5f, 1.5f));
	bounceEffect->SetParticleLife(ESPInterval(1.2f, 1.5f));
	bounceEffect->SetParticleSize(ESPInterval(0.7f, 0.9f), ESPInterval(1.0f, 1.0f));
	bounceEffect->SetParticleRotation(ESPInterval(-15.0f, 15.0f));
	bounceEffect->SetEmitAngleInDegrees(10);
	bounceEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.0f));
	bounceEffect->SetParticleAlignment(ESP::ViewPointAligned);
	
	Vector2D ballVelocity = ball.GetVelocity();
	bounceEffect->SetEmitDirection(Vector3D(ballVelocity[0], ballVelocity[1], 0.0f));

	Point2D ballCenter    = ball.GetBounds().Center();
	bounceEffect->SetEmitPosition(Point3D(ballCenter[0], ballCenter[1], 0.0f));
	
	// Add effectors...
	bounceEffect->AddEffector(&this->particleFader);
	bounceEffect->AddEffector(&this->particleSmallGrowth);

	// Add the single particle to the emitter...
	DropShadow dpTemp;
	dpTemp.amountPercentage = 0.10f;
	ESPOnomataParticle* bounceParticle = new ESPOnomataParticle(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Small));
	bounceParticle->SetDropShadow(dpTemp);

	// Set the severity of the effect...
	GameBall::BallSpeed ballSpd = ball.GetSpeed();
	switch (ballSpd) {
		case GameBall::SlowSpeed :
			bounceParticle->SetOnomatoplexSound(Onomatoplex::BOUNCE, Onomatoplex::NORMAL);
			break;
		case GameBall::NormalSpeed :
			bounceParticle->SetOnomatoplexSound(Onomatoplex::BOUNCE, Onomatoplex::AWESOME);
			break;
		case GameBall::FastSpeed :
			bounceParticle->SetOnomatoplexSound(Onomatoplex::BOUNCE, Onomatoplex::SUPER_AWESOME);
			break;
		default :
			assert(false);
			bounceParticle->SetOnomatoplexSound(Onomatoplex::BOUNCE, Onomatoplex::WEAK);
			break;
	}
	
	bounceEffect->AddParticle(bounceParticle);
	
	// Lastly, add the new emitter to the list of active emitters
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
	bangEffect->SetParticleAlignment(ESP::ViewPointAligned);
	bangEffect->SetEmitPosition(emitCenter);

	// Figure out some random proper orientation...
	// Two base rotations (for variety) : 180 or 0...
	float baseBangRotation = 0.0f;
	if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
		baseBangRotation = 180.0f;
	}
	bangEffect->SetParticleRotation(ESPInterval(baseBangRotation - 10.0f, baseBangRotation + 10.0f));

	// Four possible variations on reflection...
	ESPInterval sizeIntervalX(3.5f, 3.8f);
	ESPInterval sizeIntervalY(1.9f, 2.2f);
	if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
		sizeIntervalX = ESPInterval(-sizeIntervalX.maxValue, -sizeIntervalX.minValue);
	}
	if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
		sizeIntervalY = ESPInterval(-sizeIntervalY.maxValue, - sizeIntervalY.minValue);
	}
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
	bangOnoEffect->SetParticleAlignment(ESP::ViewPointAligned);
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
	// TODO
	bangOnoParticle->SetOnomatoplexSound(Onomatoplex::EXPLOSION, Onomatoplex::NORMAL);
	
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
	bombExplodeRayEffect->SetParticleAlignment(ESP::ViewPointAligned);
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
	bombExplodeFireEffect1->SetParticleAlignment(ESP::ViewPointAligned);
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
	bombExplodeFireEffect2->SetParticleAlignment(ESP::ViewPointAligned);
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
	bombOnoEffect->SetParticleAlignment(ESP::ViewPointAligned);
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
 * Adds an effect for when an item is dropping and not yet acquired by the player.
 */
void GameESPAssets::AddItemDropEffect(const Camera& camera, const GameItem& item) {

	std::list<ESPEmitter*> itemDropEmitters;

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

	// Aura around the ends of the droping item
	ESPPointEmitter* itemDropEmitterAura1 = new ESPPointEmitter();
	itemDropEmitterAura1->SetSpawnDelta(ESPInterval(-1));
	itemDropEmitterAura1->SetInitialSpd(ESPInterval(0));
	itemDropEmitterAura1->SetParticleLife(ESPInterval(-1));
	itemDropEmitterAura1->SetParticleSize(ESPInterval(GameItem::ITEM_HEIGHT + 0.6f), ESPInterval(GameItem::ITEM_HEIGHT + 0.6f));
	itemDropEmitterAura1->SetEmitAngleInDegrees(0);
	itemDropEmitterAura1->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	itemDropEmitterAura1->SetParticleAlignment(ESP::ViewPointAligned);
	itemDropEmitterAura1->SetEmitPosition(Point3D(GameItem::HALF_ITEM_WIDTH, 0, 0));
	itemDropEmitterAura1->SetParticleColour(redColour, greenColour, blueColour, ESPInterval(0.6f));
	itemDropEmitterAura1->AddEffector(&this->particlePulseItemDropAura);
	itemDropEmitterAura1->SetParticles(1, this->circleGradientTex);
	
	ESPPointEmitter* itemDropEmitterAura2 = new ESPPointEmitter();
	itemDropEmitterAura2->SetSpawnDelta(ESPInterval(-1));
	itemDropEmitterAura2->SetInitialSpd(ESPInterval(0));
	itemDropEmitterAura2->SetParticleLife(ESPInterval(-1));
	itemDropEmitterAura2->SetParticleSize(ESPInterval(GameItem::ITEM_HEIGHT + 0.6f), ESPInterval(GameItem::ITEM_HEIGHT + 0.6f));
	itemDropEmitterAura2->SetEmitAngleInDegrees(0);
	itemDropEmitterAura2->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	itemDropEmitterAura2->SetParticleAlignment(ESP::ViewPointAligned);
	itemDropEmitterAura2->SetEmitPosition(Point3D(-GameItem::HALF_ITEM_WIDTH, 0, 0));
	itemDropEmitterAura2->SetParticleColour(redColour, greenColour, blueColour, ESPInterval(0.6f));
	itemDropEmitterAura2->AddEffector(&this->particlePulseItemDropAura);
	itemDropEmitterAura2->SetParticles(1, this->circleGradientTex);

	// Middle emitter emits solid stars
	ESPPointEmitter* itemDropEmitterTrail1 = new ESPPointEmitter();
	itemDropEmitterTrail1 = new ESPPointEmitter();
	itemDropEmitterTrail1->SetSpawnDelta(ESPInterval(0.05f, 0.2f));
	itemDropEmitterTrail1->SetInitialSpd(ESPInterval(3.0f, 5.0f));
	itemDropEmitterTrail1->SetParticleLife(ESPInterval(1.2f, 2.0f));
	itemDropEmitterTrail1->SetParticleSize(ESPInterval(0.6f, 1.4f));
	itemDropEmitterTrail1->SetParticleColour(redRandomColour, greenRandomColour, blueRandomColour, alpha);
	itemDropEmitterTrail1->SetEmitAngleInDegrees(25);
	itemDropEmitterTrail1->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	itemDropEmitterTrail1->SetParticleAlignment(ESP::ViewPointAligned);
	itemDropEmitterTrail1->SetEmitDirection(Vector3D(0, 1, 0));
	itemDropEmitterTrail1->SetEmitPosition(Point3D(0, 0, 0));
	itemDropEmitterTrail1->AddEffector(&this->particleFader);
	itemDropEmitterTrail1->SetParticles(8, this->starTex);
	
	// Left emitter emits outlined stars
	ESPPointEmitter* itemDropEmitterTrail2 = new ESPPointEmitter();
	itemDropEmitterTrail2 = new ESPPointEmitter();
	itemDropEmitterTrail2->SetSpawnDelta(ESPInterval(0.05f, 0.2f));
	itemDropEmitterTrail2->SetInitialSpd(ESPInterval(2.5f, 4.0f));
	itemDropEmitterTrail2->SetParticleLife(ESPInterval(1.5f, 2.2f));
	itemDropEmitterTrail2->SetParticleSize(ESPInterval(0.4f, 1.1f));
	itemDropEmitterTrail2->SetParticleColour(redRandomColour, greenRandomColour, blueRandomColour, alpha);
	itemDropEmitterTrail2->SetEmitAngleInDegrees(10);
	itemDropEmitterTrail2->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	itemDropEmitterTrail2->SetParticleAlignment(ESP::ViewPointAligned);
	itemDropEmitterTrail2->SetEmitDirection(Vector3D(0, 1, 0));
	itemDropEmitterTrail2->SetEmitPosition(Point3D(-GameItem::ITEM_WIDTH/3, 0, 0));
	itemDropEmitterTrail2->AddEffector(&this->particleFader);
	itemDropEmitterTrail2->SetParticles(7, this->starOutlineTex);
	
	// Right emitter emits outlined stars
	ESPPointEmitter* itemDropEmitterTrail3 = new ESPPointEmitter();
	itemDropEmitterTrail3 = new ESPPointEmitter();
	itemDropEmitterTrail3->SetSpawnDelta(ESPInterval(0.05f, 0.2f));
	itemDropEmitterTrail3->SetInitialSpd(ESPInterval(2.5f, 4.0f));
	itemDropEmitterTrail3->SetParticleLife(ESPInterval(1.5f, 2.2f));
	itemDropEmitterTrail3->SetParticleSize(ESPInterval(0.4f, 1.1f));
	itemDropEmitterTrail3->SetParticleColour(redRandomColour, greenRandomColour, blueRandomColour, alpha);
	itemDropEmitterTrail3->SetEmitAngleInDegrees(10);
	itemDropEmitterTrail3->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	itemDropEmitterTrail3->SetParticleAlignment(ESP::ViewPointAligned);
	itemDropEmitterTrail3->SetEmitDirection(Vector3D(0, 1, 0));
	itemDropEmitterTrail3->SetEmitPosition(Point3D(GameItem::ITEM_WIDTH/3, 0, 0));
	itemDropEmitterTrail3->AddEffector(&this->particleFader);
	itemDropEmitterTrail3->SetParticles(7, this->starOutlineTex);

	// Add all the emitters for the item
	itemDropEmitters.push_back(itemDropEmitterAura1);
	itemDropEmitters.push_back(itemDropEmitterAura2);
	itemDropEmitters.push_back(itemDropEmitterTrail1);
	itemDropEmitters.push_back(itemDropEmitterTrail2);
	itemDropEmitters.push_back(itemDropEmitterTrail3);
	this->activeItemDropEmitters[&item] = itemDropEmitters;
}
/**
 * Removes an effect associated with a dropping item.
 */
void GameESPAssets::RemoveItemDropEffect(const Camera& camera, const GameItem& item) {
	if (this->activeItemDropEmitters.find(&item) != this->activeItemDropEmitters.end()) {
		this->activeItemDropEmitters.erase(&item);
	}
}

/**
 * Adds an effect for a active projectile in-game.
 */
void GameESPAssets::AddProjectileEffect(const Camera& camera, const Projectile& projectile) {
	switch (projectile.GetType()) {
		case Projectile::PaddleLaserProjectile:
			this->AddLaserPaddleESPEffects(projectile);
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
	if (this->activeProjectileEmitters.find(&projectile) != this->activeProjectileEmitters.end()) {
		this->activeProjectileEmitters.erase(&projectile);
	}
}

/**
 * Private helper function for making and adding a laser blast effect for the laser paddle item.
 */
void GameESPAssets::AddLaserPaddleESPEffects(const Projectile& projectile) {
	assert(projectile.GetType() == Projectile::PaddleLaserProjectile);
	
	Point2D projectilePos2D = projectile.GetPosition();
	Point3D projectilePos3D = Point3D(projectilePos2D[0], projectilePos2D[1], 0.0f);
	float projectileSpd     = projectile.GetVelocityMagnitude();
	Vector2D projectileDir  = projectile.GetVelocityDirection();

	// Create the basic laser beam (top-most effect)
	ESPPointEmitter* laserBeamEmitter = new ESPPointEmitter();
	laserBeamEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	laserBeamEmitter->SetInitialSpd(ESPInterval(0));
	laserBeamEmitter->SetParticleLife(ESPInterval(-1));
	laserBeamEmitter->SetParticleSize(ESPInterval(PaddleLaser::PADDLELASER_WIDTH), ESPInterval(PaddleLaser::PADDLELASER_HEIGHT));
	laserBeamEmitter->SetEmitAngleInDegrees(0);
	laserBeamEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	laserBeamEmitter->SetParticleAlignment(ESP::ViewPointAligned);
	laserBeamEmitter->SetEmitPosition(Point3D(0,0,0));
	laserBeamEmitter->SetParticleColour(ESPInterval(0.5f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	laserBeamEmitter->SetParticles(1, this->laserBeamTex);

	// Create the slightly-pulsing-glowing aura
	ESPPointEmitter* laserAuraEmitter = new ESPPointEmitter();
	laserAuraEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	laserAuraEmitter->SetInitialSpd(ESPInterval(0));
	laserAuraEmitter->SetParticleLife(ESPInterval(-1));
	laserAuraEmitter->SetParticleSize(ESPInterval(2*PaddleLaser::PADDLELASER_WIDTH), ESPInterval(1.8f*PaddleLaser::PADDLELASER_HEIGHT));
	laserAuraEmitter->SetEmitAngleInDegrees(0);
	laserAuraEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	laserAuraEmitter->SetParticleAlignment(ESP::ViewPointAligned);
	laserAuraEmitter->SetEmitPosition(Point3D(0,0,0));
	laserAuraEmitter->SetParticleColour(ESPInterval(0.65f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	laserAuraEmitter->AddEffector(&this->particlePulsePaddleLaser);
	laserAuraEmitter->SetParticles(1, this->circleGradientTex);

	// Create the trail effects
	ESPPointEmitter* laserTrailSparks = new ESPPointEmitter();
	laserTrailSparks = new ESPPointEmitter();
	laserTrailSparks->SetSpawnDelta(ESPInterval(0.01f, 0.033f));
	laserTrailSparks->SetInitialSpd(ESPInterval(projectileSpd));
	laserTrailSparks->SetParticleLife(ESPInterval(0.5f, 0.6f));
	laserTrailSparks->SetParticleSize(ESPInterval(0.4f, 0.85f));
	laserTrailSparks->SetParticleColour(ESPInterval(0.5f, 0.8f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	laserTrailSparks->SetEmitAngleInDegrees(15);
	laserTrailSparks->SetEmitDirection(Vector3D(-projectileDir[0], -projectileDir[1], 0.0f));
	laserTrailSparks->SetRadiusDeviationFromCenter(ESPInterval(0.5f*PaddleLaser::PADDLELASER_HALF_WIDTH));
	laserTrailSparks->SetParticleAlignment(ESP::ScreenAligned);
	laserTrailSparks->SetEmitPosition(projectilePos3D);
	laserTrailSparks->AddEffector(&this->particleFader);
	laserTrailSparks->AddEffector(&this->particleMediumShrink);
	laserTrailSparks->SetParticles(10, this->circleGradientTex);

	/*
	// TODO: OPTIMIZE!! me.
	ESPPointEmitter* laserVapourTrail = new ESPPointEmitter();
	laserVapourTrail->SetSpawnDelta(ESPInterval(0.01f));
	laserVapourTrail->SetInitialSpd(ESPInterval(projectileSpd));
	laserVapourTrail->SetParticleLife(ESPInterval(0.5f));
	laserVapourTrail->SetParticleSize(ESPInterval(1.5f*PaddleLaser::PADDLELASER_WIDTH));
	laserVapourTrail->SetRadiusDeviationFromCenter(ESPInterval(0));
	laserVapourTrail->SetParticleAlignment(ESP::ScreenAligned);
	laserVapourTrail->SetEmitPosition(projectilePos3D);
	laserVapourTrail->SetEmitDirection(Vector3D(-projectileDir[0], -projectileDir[1], 0.0f));
	laserVapourTrail->SetEmitAngleInDegrees(10);
	laserVapourTrail->SetParticles(GameESPAssets::NUM_LASER_VAPOUR_TRAIL_PARTICLES, &this->laserVapourTrailEffect);
	laserVapourTrail->AddEffector(&this->particleFader);
	*/
	std::list<ESPPointEmitter*> laserEmittersFG;
	std::list<ESPPointEmitter*> laserEmittersBG;
	//laserEmittersBG.push_front(laserVapourTrail);

	laserEmittersFG.push_back(laserTrailSparks);
	laserEmittersFG.push_back(laserAuraEmitter);
	laserEmittersFG.push_back(laserBeamEmitter);
	this->activeProjectileEmitters[&projectile] = std::make_pair<std::list<ESPPointEmitter*>, std::list<ESPPointEmitter*>>(laserEmittersFG, laserEmittersBG);
}

/**
 * Adds an effect for when a dropping item is acquired by the player paddle -
 * depending on the item type we create the effect.
 */
void GameESPAssets::AddItemAcquiredEffect(const Camera& camera, const GameItem& item) {
	// TODO.

		/*
	switch (item.GetItemType()) {
		case GameItem::Good:
			break;
		case GameItem::Bad:
			break;
		case GameItem::Neutral:
		default:
			break;
	}
	*/
}

/**
 * Adds an effect based on the given game item being activated or deactivated.
 */
void GameESPAssets::SetItemEffect(const GameItem& item, bool activate) {
	
	if (item.GetName() == UberBallItem::UBER_BALL_ITEM_NAME && activate) {
		this->uberBallEmitterAura->Reset();
	}
	else if (item.GetName() == LaserPaddleItem::LASER_PADDLE_ITEM_NAME && activate) {
		this->paddleLaserGlowAura->Reset();
		this->paddleLaserGlowSparks->Reset();
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
			this->activeGeneralEmitters.erase(*iter);
	}

	this->DrawProjectileEffects(dT, camera);
}

/**
 * Render the post processing emitter/sprite/particle effects - these require a
 * provided render to texture of the scene.
 */
void GameESPAssets::DrawPostProcessingESPEffects(double dT, const Camera& camera, Texture2D* sceneTex) {
	// TODO.
}

/**
 * Update and draw all projectile effects that are currently active.
 */
void GameESPAssets::DrawProjectileEffects(double dT, const Camera& camera) {
	for (std::map<const Projectile*, std::pair<std::list<ESPPointEmitter*>, std::list<ESPPointEmitter*>>>::iterator iter = this->activeProjectileEmitters.begin();
		iter != this->activeProjectileEmitters.end(); iter++) {
		
		const Projectile* currProjectile = iter->first;
		std::pair<std::list<ESPPointEmitter*>, std::list<ESPPointEmitter*>> fgbgPair = iter->second;

		assert(currProjectile != NULL);

		Point2D projectilePos2D		= currProjectile->GetPosition();

		// Update and draw the emitters, background then foreground...
		for (std::list<ESPPointEmitter*>::iterator emitIterBG = fgbgPair.second.begin(); emitIterBG != fgbgPair.second.end(); emitIterBG++) {
			this->DrawProjectileEmitter(dT, camera, projectilePos2D, *emitIterBG);
		}
		for (std::list<ESPPointEmitter*>::iterator emitIterFG = fgbgPair.first.begin(); emitIterFG != fgbgPair.first.end(); emitIterFG++) {
			this->DrawProjectileEmitter(dT, camera, projectilePos2D, *emitIterFG);
		}
	}
}

/**
 * Private helper function for drawing a single projectile at a given position.
 */
void GameESPAssets::DrawProjectileEmitter(double dT, const Camera& camera, const Point2D& projectilePos2D, ESPPointEmitter* projectileEmitter) {
	bool movesWithProjectile = projectileEmitter->OnlySpawnsOnce();
	if (movesWithProjectile) {
		glPushMatrix();
		glTranslatef(projectilePos2D[0], projectilePos2D[1], 0.0f);
	}
	else {
		projectileEmitter->SetEmitPosition(Point3D(projectilePos2D[0], projectilePos2D[1], 0.0f));
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
	std::list<ESPEmitter*> itemDropEmitters = itemDropEffectIter->second;
	for (std::list<ESPEmitter*>::iterator iter = itemDropEmitters.begin(); iter != itemDropEmitters.end(); iter++) {
		ESPEmitter* currEmitter = *iter;
		currEmitter->Draw(camera);
		currEmitter->Tick(dT);
	}
}

/**
 * Draw particle effects associated with the uberball.
 */
void GameESPAssets::DrawUberBallEffects(double dT, const Camera& camera, const GameBall& ball) {
	glPushMatrix();
	Point2D loc = ball.GetBounds().Center();
	glTranslatef(loc[0], loc[1], 0);

	// Draw the basic aura around the ball
	this->uberBallEmitterAura->Draw(camera);
	this->uberBallEmitterAura->Tick(dT);

	// Create a kind of trail for the ball...
	Vector2D ballDir    = ball.GetDirection();
	if (this->oldBallDir != ballDir) {
		this->EffectsToResetOnBallVelChange();
	}
	this->oldBallDir = ballDir;
	
	// Draw the trail of the ghost ball...
	Vector3D negBallDir = Vector3D(-ballDir[0], -ballDir[1], 0.0f);
	this->uberBallEmitterTrail->SetEmitDirection(negBallDir);
	this->uberBallEmitterTrail->Draw(camera);
	this->uberBallEmitterTrail->Tick(dT);

	glPopMatrix();
}

/**
 * Draw particle effects associated with the ghostball.
 */
void GameESPAssets::DrawGhostBallEffects(double dT, const Camera& camera, const GameBall& ball) {
	glPushMatrix();
	Point2D loc = ball.GetBounds().Center();
	glTranslatef(loc[0], loc[1], 0);

	Vector2D ballDir    = ball.GetDirection();
	Vector3D negBallDir = Vector3D(-ballDir[0], -ballDir[1], 0.0f);

	// Reset the trail effect when the ball suddenly changes velocity...
	if (this->oldBallDir != ballDir) {
		this->EffectsToResetOnBallVelChange();
	}
	this->oldBallDir = ballDir;

	// Rotate the negative ball -vel direction by some random amount and then affect the particle's velocities
	// by it, this gives the impression that the particles are waving around mysteriously (ghostlike... one might say)
	double randomDegrees = Randomizer::GetInstance()->RandomNumNegOneToOne() * 90;
	Vector3D accel = Matrix4x4::rotationMatrix('z', static_cast<float>(randomDegrees), true) * negBallDir;
	accel = ball.GetSpeed() * 4.0 * accel;
	this->ghostBallAccel1.SetAcceleration(accel);

	// Create a kind of trail for the ball...
	//this->ghostBallEmitterTrail->SetEmitPosition(Point3D(loc[0], loc[1], 0));
	this->ghostBallEmitterTrail->Draw(camera);
	this->ghostBallEmitterTrail->Tick(dT);
	
	glPopMatrix();
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

/**
 * Private helper function that resets certain effects when the ball changes
 * velocity - the reasoning behind this is that we want particles to stop moving
 * based on previous ball velocity / directions in the case where they are based off
 * such values.
 */
void GameESPAssets::EffectsToResetOnBallVelChange() {
	this->ghostBallEmitterTrail->Reset();
	this->uberBallEmitterTrail->Reset();
}