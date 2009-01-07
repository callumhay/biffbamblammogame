#include "GameESPAssets.h"
#include "GameFontAssetsManager.h"
#include "GameViewConstants.h"

#include "../GameModel/GameBall.h"
#include "../GameModel/LevelPiece.h"

#include "../BlammoEngine/Texture2D.h"

#include "../ESPEngine/ESP.h"

GameESPAssets::GameESPAssets() : particleFader(1, 0), particleSmallGrowth(1.0f, 1.3f) {

	this->InitESPTextures();
}

GameESPAssets::~GameESPAssets() {
	// Delete any leftover emitters
	for (std::list<ESPEmitter*>::iterator iter = this->activeParticleEmitters.begin();
		iter != this->activeParticleEmitters.end(); iter++) {
			delete *iter;
	}
	this->activeParticleEmitters.clear();

	// Delete any effect textures
	for (std::vector<Texture2D*>::iterator iter = this->bangTextures.begin();
		iter != this->bangTextures.end(); iter++) {
		delete *iter;
	}
	this->bangTextures.clear();

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
}


/**
 * Adds a ball bouncing ESP - the effect that occurs when a ball typically
 * bounces off some object (e.g., block, paddle).
 */
void GameESPAssets::AddBallBounceESP(const GameBall& ball) {
	// No effects in the case of invisiball
	if ((ball.GetBallType() & GameBall::InvisiBall) == GameBall::InvisiBall) {
		return;
	}
	
	// In this case the effect is a basic onomatopeia word that occurs at the
	// position of the ball

	ESPPointEmitter* bounceEffect = new ESPPointEmitter();
	// Set up the emitter...
	bounceEffect->SetSpawnDelta(ESPInterval(-1, -1));
	bounceEffect->SetInitialSpd(ESPInterval(0.5f, 1.5f));
	bounceEffect->SetParticleLife(ESPInterval(1.2f, 1.5f));
	bounceEffect->SetParticleSize(ESPInterval(0.7f, 0.9f));
	bounceEffect->SetParticleRotation(ESPInterval(-15.0f, 15.0f));
	bounceEffect->SetEmitAngleInDegrees(10);
	bounceEffect->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	bounceEffect->SetParticleAlignment(ESP::ViewPointAligned);
	
	Vector2D ballVelocity = ball.GetVelocity();
	bounceEffect->SetEmitDirection(Vector3D(ballVelocity[0], ballVelocity[1], 0.0f));

	Point2D ballCenter = ball.GetBounds().Center();
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
	this->activeParticleEmitters.push_back(bounceEffect);
}

/**
 * Add the typical block break effect - visible when you destroy a typical block.
 */
void GameESPAssets::AddBasicBlockBreakEffect(const LevelPiece& block) {
	assert(this->bangTextures.size() != 0);

	// Choose a random bang texture
	unsigned int randomBangTexIndex = Randomizer::GetInstance()->RandomUnsignedInt() % this->bangTextures.size();
	Texture2D* randomBangTex = this->bangTextures[randomBangTexIndex];
	
	ESPInterval bangLifeInterval = ESPInterval(0.8f, 1.1f);
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
	//bangEffect->SetParticleRotation(ESPInterval(-15.0f, 15.0f));
	// TODO: reflect?
	bangEffect->SetParticleSize(ESPInterval(3.0f, 3.0f));	// TODO: option for size to have x, y, z specified...

	// Add effectors to the bang effect
	bangEffect->AddEffector(&this->particleFader);
	
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
	bangOnoEffect->SetParticleLife(bangLifeInterval);
	bangOnoEffect->SetParticleSize(ESPInterval(0.8f, 1.1f));
	bangOnoEffect->SetParticleRotation(ESPInterval(-25.0f, 25.0f));
	bangOnoEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.3f));
	bangOnoEffect->SetParticleAlignment(ESP::ViewPointAligned);
	//bangOnoEffect->SetEmitDirection(Vector3D(0.0f, 0.0f, 0.0f));
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
	bangOnoParticle->SetOnomatoplexSound(Onomatoplex::EXPLOSION, Onomatoplex::AWESOME);
	
	bangOnoEffect->AddParticle(bangOnoParticle);

	// Lastly, add the new emitters to the list of active emitters in order of back to front
	this->activeParticleEmitters.push_back(bangEffect);
	this->activeParticleEmitters.push_back(bangOnoEffect);
}

/**
 * Draw call that will draw all active effects and clear up all inactive effects
 * for the game.
 */
void GameESPAssets::DrawParticleEffects(double dT, const Camera& camera) {
	
	std::list<std::list<ESPEmitter*>::iterator> removeElements;

	// Go through all the particles and do book keeping and drawing
	for (std::list<ESPEmitter*>::iterator iter = this->activeParticleEmitters.begin();
		iter != this->activeParticleEmitters.end(); iter++) {
	
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
		
			this->activeParticleEmitters.erase(*iter);
	}

}
