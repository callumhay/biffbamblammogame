/**
 * BossMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "BossMesh.h"
#include "ClassicalBossMesh.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Texture2D.h"

#include "../GameModel/ClassicalBoss.h"

#include "../ResourceManager.h"

BossMesh::BossMesh() : explosionAnimTex(NULL),
particleMediumGrowth(1.0f, 1.6f), particleLargeGrowth(1.0f, 2.2f),
//smokeColourFader(ColourRGBA(0.7f, 0.7f, 0.7f, 1.0f), ColourRGBA(0.1f, 0.1f, 0.1f, 0.1f)),
particleFireColourFader(ColourRGBA(1.0f, 1.0f, 0.1f, 1.0f), ColourRGBA(0.5f, 0.1f, 0.1f, 0.0f)),
//particleFader(1.0f, 0.0f),
rotateEffectorCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.25f, ESPParticleRotateEffector::CLOCKWISE)
//rotateEffectorCCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.25f, ESPParticleRotateEffector::COUNTER_CLOCKWISE)
{

    // Initialize the smoke textures...
    if (this->smokeTextures.empty()) {
		this->smokeTextures.reserve(6);
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

    this->explosionAnimTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_EXPLOSION_ANIMATION, Texture::Trilinear));
    assert(this->explosionAnimTex != NULL);

}

BossMesh::~BossMesh() {
    // Release the smoke textures
    bool success = false;

    for (std::vector<Texture2D*>::iterator iter = this->smokeTextures.begin();
		iter != this->smokeTextures.end(); ++iter) {
		
		success = ResourceManager::GetInstance()->ReleaseTextureResource(*iter);
		assert(success);	
	}
	this->smokeTextures.clear();

    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->explosionAnimTex);
    assert(success);

    UNUSED_VARIABLE(success);
}


BossMesh* BossMesh::Build(const GameWorld::WorldStyle& style, Boss* boss) {
    BossMesh* result = NULL;
    switch (style) {

        case GameWorld::Classical: {
            assert(dynamic_cast<ClassicalBoss*>(boss) != NULL);
            ClassicalBoss* classicalBoss = static_cast<ClassicalBoss*>(boss);
            result = new ClassicalBossMesh(classicalBoss);
            break;
        }

        case GameWorld::Deco:
            // TODO

        case GameWorld::Futurism:
            // TODO

        default:
            assert(false);
            break;

    }

    return result;
}

ESPPointEmitter* BossMesh::BuildFireSmokeEmitter(float width, float height) {
    float smallestDimension = std::min<float>(width, height);

    static const float MAX_SPAWN_DELTA = 0.16f;
    static const float MAX_LIFE = 1.75f;

	ESPPointEmitter* smokeEmitter = new ESPPointEmitter();
	smokeEmitter->SetSpawnDelta(ESPInterval(MAX_SPAWN_DELTA, MAX_SPAWN_DELTA));
	smokeEmitter->SetInitialSpd(ESPInterval(1.0f, 4.0f));
	smokeEmitter->SetParticleLife(ESPInterval(MAX_LIFE - 1.0f, MAX_LIFE));
    smokeEmitter->SetParticleSize(ESPInterval(0.45f * smallestDimension, 0.65f * smallestDimension));
	smokeEmitter->SetEmitAngleInDegrees(0);
	smokeEmitter->SetEmitDirection(Vector3D(0, 1, 0));
	smokeEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.25f * width), ESPInterval(0.0f, 0.33f * height), ESPInterval(0.0f));
	smokeEmitter->SetParticleAlignment(ESP::ScreenAligned);
	smokeEmitter->SetEmitPosition(Point3D(0.0f, 0.0f, 0.0f));
	smokeEmitter->AddEffector(&this->particleFireColourFader);
	smokeEmitter->AddEffector(&this->particleLargeGrowth);
	smokeEmitter->AddEffector(&this->rotateEffectorCW);
	
    int numParticles = static_cast<int>(MAX_LIFE / MAX_SPAWN_DELTA);
    assert(numParticles < 30);
    bool success = smokeEmitter->SetParticles(numParticles,
        this->smokeTextures[Randomizer::GetInstance()->RandomUnsignedInt() % this->smokeTextures.size()]);
	assert(success);
    UNUSED_VARIABLE(success);

    return smokeEmitter;
}

ESPPointEmitter* BossMesh::BuildExplodingEmitter(float width, float height) {
    float largestDimension = std::max<float>(width, height);

    static const float MAX_SPAWN_DELTA = 0.2f;
    
    static const double FPS = 60.0;
    static const float MAX_LIFE = 4*16.0 / FPS;

	ESPPointEmitter* explosionEmitter = new ESPPointEmitter();
	explosionEmitter->SetSpawnDelta(ESPInterval(0.2f, MAX_SPAWN_DELTA));
	explosionEmitter->SetInitialSpd(ESPInterval(1.0f));
	explosionEmitter->SetParticleLife(ESPInterval(MAX_LIFE));
    explosionEmitter->SetParticleSize(ESPInterval(0.25f * largestDimension, 0.5f * largestDimension));
	explosionEmitter->SetEmitAngleInDegrees(0);
	explosionEmitter->SetEmitDirection(Vector3D(0, 1, 0));
	explosionEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.25f * width), ESPInterval(0.0f, 0.5f * height), ESPInterval(0.0f));
	explosionEmitter->SetParticleAlignment(ESP::ScreenAligned);
	explosionEmitter->SetEmitPosition(Point3D(0.0f, 0.0f, 0.0f));
	explosionEmitter->AddEffector(&this->particleLargeGrowth);
	
    int numParticles = static_cast<int>(MAX_LIFE / MAX_SPAWN_DELTA);
    assert(numParticles < 30);
    bool success = explosionEmitter->SetAnimatedParticles(numParticles, this->explosionAnimTex, 64, 64, FPS);
	assert(success);
    UNUSED_VARIABLE(success);

    return explosionEmitter;
}