#include "GameESPAssets.h"
#include "GameFontAssetsManager.h"
#include "GameViewConstants.h"

#include "../GameModel/GameBall.h"
#include "../GameModel/LevelPiece.h"
#include "../GameModel/GameItem.h"
#include "../GameModel/UberBallItem.h"
#include "../GameModel/BallSpeedItem.h"
#include "../GameModel/InvisiBallItem.h"

#include "../BlammoEngine/Texture2D.h"

#include "../ESPEngine/ESP.h"

GameESPAssets::GameESPAssets() : 
particleFader(1, 0), particleFaderUberballTrail(Colour(1,0,0), 0.6f, 0), particlePulseUberballAura(0,0),
particleSmallGrowth(1.0f, 1.3f), particleMediumGrowth(1.0f, 1.6f),
uberBallEmitterAura(NULL), uberBallEmitterTrail(NULL), circleGradientTex(NULL), oldBallDir(0,0) {

	this->InitESPTextures();
	this->InitStandaloneESPEffects();
}

GameESPAssets::~GameESPAssets() {
	// Delete any leftover emitters
	for (std::list<ESPEmitter*>::iterator iter = this->activeGeneralEmitters.begin();
		iter != this->activeGeneralEmitters.end(); iter++) {
			delete *iter;
	}
	this->activeGeneralEmitters.clear();

	// Delete any effect textures
	for (std::vector<Texture2D*>::iterator iter = this->bangTextures.begin();
		iter != this->bangTextures.end(); iter++) {
		delete *iter;
	}
	this->bangTextures.clear();

	delete this->circleGradientTex;
	this->circleGradientTex = NULL;

	// Delete any standalone effects
	delete this->uberBallEmitterAura;
	this->uberBallEmitterAura = NULL;
	delete this->uberBallEmitterTrail;
	this->uberBallEmitterTrail = NULL;

}

/**
 * Private helper function for initializing textures used in the ESP effects.
 */
void GameESPAssets::InitESPTextures() {
	// Initialize bang textures (big boom thingys when there are explosions)
	if (this->bangTextures.size() == 0) {
		// TODO: fix texture filtering...
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
	
	if (this->circleGradientTex == NULL) {
		this->circleGradientTex = Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_CIRCLE_GRADIENT, Texture::Trilinear);
		assert(this->circleGradientTex != NULL);
	}
}

/**
 * Private helper function for initializing standalone ESP effects.
 */
void GameESPAssets::InitStandaloneESPEffects() {
	// Initialize effectors
	ScaleEffect uberBallPulseSettings;
	uberBallPulseSettings.pulseGrowthScale = 1.5f;
	uberBallPulseSettings.pulseRate = 1.5f;
	this->particlePulseUberballAura = ESPParticleScaleEffector(uberBallPulseSettings);
	
	// TODO: 
	// - Shader particle effects

	// Set up the uberball emitters...
	this->uberBallEmitterAura = new ESPPointEmitter();
	this->uberBallEmitterAura->SetSpawnDelta(ESPInterval(-1, -1));
	this->uberBallEmitterAura->SetInitialSpd(ESPInterval(0, 0));
	this->uberBallEmitterAura->SetParticleLife(ESPInterval(-1, -1));
	this->uberBallEmitterAura->SetParticleSize(ESPInterval(1.3f, 1.3f), ESPInterval(1.3f, 1.3f));
	this->uberBallEmitterAura->SetEmitAngleInDegrees(0);
	this->uberBallEmitterAura->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.0f));
	this->uberBallEmitterAura->SetParticleAlignment(ESP::ViewPointAligned);
	this->uberBallEmitterAura->SetEmitPosition(Point3D(0, 0, 0));
	this->uberBallEmitterAura->SetParticleColour(ESPInterval(1, 1), ESPInterval(0, 0), ESPInterval(0, 0), ESPInterval(0.75f, 0.75f));
	this->uberBallEmitterAura->AddEffector(&this->particlePulseUberballAura);
	this->uberBallEmitterAura->SetParticles(1, this->circleGradientTex);
	
	this->uberBallEmitterTrail = new ESPPointEmitter();
	this->uberBallEmitterTrail->SetSpawnDelta(ESPInterval(0.05f, 0.05f));
	this->uberBallEmitterTrail->SetInitialSpd(ESPInterval(4.0f, 4.0f));
	this->uberBallEmitterTrail->SetParticleLife(ESPInterval(1.0f, 1.0f));
	this->uberBallEmitterTrail->SetParticleSize(ESPInterval(1.3f, 1.3f), ESPInterval(1.3f, 1.3f));
	this->uberBallEmitterTrail->SetEmitAngleInDegrees(0);
	this->uberBallEmitterTrail->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.0f));
	this->uberBallEmitterTrail->SetParticleAlignment(ESP::ViewPointAligned);
	this->uberBallEmitterTrail->SetEmitPosition(Point3D(0, 0, 0));
	this->uberBallEmitterTrail->AddEffector(&this->particleFaderUberballTrail);
	this->uberBallEmitterTrail->SetParticles(20, this->circleGradientTex);	
	
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
	this->activeGeneralEmitters.push_back(bounceEffect);
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
	this->activeGeneralEmitters.push_back(bangEffect);
	this->activeGeneralEmitters.push_back(bangOnoEffect);
}

/**
 * Adds an effect for when a dropping item is acquired by the player paddle -
 * depending on the item type we create the effect.
 */
void GameESPAssets::AddItemAcquiredEffect(const Camera& camera, const GameItem& item) {
	// TODO.
}

/**
 * Adds an effect based on the given game item being activated or deactivated.
 */
void GameESPAssets::SetItemEffect(const GameItem& item, bool activate) {
	
	if (item.GetName() == UberBallItem::UBER_BALL_ITEM_NAME && activate) {
		this->uberBallEmitterAura->Reset();
	}
}

/**
 * Draw call that will draw all active effects and clear up all inactive effects
 * for the game.
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
}

/**
 * Draw the effects that occur as a item drops down towards the player paddle.
 */
void GameESPAssets::DrawItemDropEffects(double dT, const Camera& camera, const GameItem& item) {
	// TODO.
}

/**
 * Draw particle effects associated with the game ball.
 */
void GameESPAssets::DrawUberBallEffects(double dT, const Camera& camera, const GameBall& ball) {
	// Draw the basic aura around the ball
	this->uberBallEmitterAura->Draw(camera);
	this->uberBallEmitterAura->Tick(dT);

	// Create a kind of trail for the ball...
	Vector2D ballDir    = ball.GetDirection();
	if (this->oldBallDir != ballDir) {
		this->uberBallEmitterTrail->Reset();
	}
	this->oldBallDir = ballDir;
	Vector3D negBallDir = Vector3D(-ballDir[0], -ballDir[1], 0.0f);
	this->uberBallEmitterTrail->SetEmitDirection(negBallDir);
	this->uberBallEmitterTrail->Draw(camera);
	this->uberBallEmitterTrail->Tick(dT);
}