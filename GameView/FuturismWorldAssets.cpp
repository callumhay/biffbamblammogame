/**
 * FuturismWorldAssets.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "FuturismWorldAssets.h"
#include "GameViewConstants.h"

// TODO: Get rid of this
#include "DecoSkybox.h"

FuturismWorldAssets::FuturismWorldAssets() :
GameWorldAssets(new DecoSkybox(),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->FUTURISM_BACKGROUND_MESH),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->FUTURISM_PADDLE_MESH),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->FUTURISM_BLOCK_MESH)),

rotateEffectorCW(0, 5, ESPParticleRotateEffector::CLOCKWISE),
rotateEffectorCCW(0, 5, ESPParticleRotateEffector::COUNTER_CLOCKWISE)
{
    this->InitializeTextures();
    this->InitializeEmitters();
}

FuturismWorldAssets::~FuturismWorldAssets() {
    bool success = false;
    for (std::vector<Texture2D*>::iterator iter = this->shardTextures.begin();
         iter != this->shardTextures.end(); ++iter) {
        success = ResourceManager::GetInstance()->ReleaseTextureResource(*iter);
        assert(success);
    }
    this->shardTextures.clear();

    for (std::vector<ESPVolumeEmitter*>::iterator iter = this->shardEmitters.begin();
         iter != this->shardEmitters.end(); ++iter) {
        ESPVolumeEmitter* currEmitter = *iter;
        delete currEmitter;
        currEmitter = NULL;
    }
    this->shardEmitters.clear();

    UNUSED_VARIABLE(success);
}

void FuturismWorldAssets::Tick(double dT) {
    GameWorldAssets::Tick(dT);

    for (std::vector<ESPVolumeEmitter*>::iterator iter = this->shardEmitters.begin();
         iter != this->shardEmitters.end(); ++iter) {
        (*iter)->Tick(dT);
    }
}

void FuturismWorldAssets::DrawBackgroundEffects(const Camera& camera) {

}

void FuturismWorldAssets::DrawBackgroundModel(const Camera& camera, const BasicPointLight& bgKeyLight,
                                              const BasicPointLight& bgFillLight) {

    const Colour& currBGModelColour = this->currBGMeshColourAnim.GetInterpolantValue();

    // Draw background sprites first
    for (std::vector<ESPVolumeEmitter*>::iterator iter = this->shardEmitters.begin();
         iter != this->shardEmitters.end(); ++iter) {
        (*iter)->Draw(camera);
    }

    // Draw the model with the current background colour
	glPushAttrib(GL_CURRENT_BIT);
	glColor4f(currBGModelColour.R(), currBGModelColour.G(), currBGModelColour.B(), this->bgFadeAnim.GetInterpolantValue());
	this->background->Draw(camera, bgKeyLight, bgFillLight);
	glPopAttrib();
}

void FuturismWorldAssets::FadeBackground(bool fadeout, float fadeTime) {
    GameWorldAssets::FadeBackground(fadeout, fadeTime);
}

void FuturismWorldAssets::ResetToInitialState() {
    GameWorldAssets::ResetToInitialState();

}

void FuturismWorldAssets::InitializeTextures() {
    assert(this->shardTextures.empty());
    
    this->shardTextures.reserve(4);

    Texture2D* shardTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_FUTURISM_SHARD_1, Texture::Trilinear));
    assert(shardTex != NULL);
    this->shardTextures.push_back(shardTex);

    shardTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_FUTURISM_SHARD_2, Texture::Trilinear));
    assert(shardTex != NULL);
    this->shardTextures.push_back(shardTex);

    shardTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_FUTURISM_SHARD_3, Texture::Trilinear));
    assert(shardTex != NULL);
    this->shardTextures.push_back(shardTex);

    shardTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_FUTURISM_SHARD_4, Texture::Trilinear));
    assert(shardTex != NULL);
    this->shardTextures.push_back(shardTex);
}

void FuturismWorldAssets::InitializeEmitters() {
    assert(this->shardEmitters.empty());
    
    this->shardEmitters.reserve(this->shardTextures.size());

	// Setup the spiral emitters that come out at the very back, behind the buildings
	ESPInterval emitterSpawn(0.6f, 1.2f);
	ESPInterval emitterLife(20.0f, 25.0f);
	ESPInterval emitterSpd(5.0f, 7.0f);

    ESPInterval xSize(6.0f, 12.0f);
    ESPInterval ySize(3.0f, 6.0f);

	const Point3D minPt(-60.0f, -30.0f, -80.0f);
	const Point3D maxPt(60.0f, -25.0f, -67.0f);

    Colour shardColour(1,1,1);

    for (int i = 0; i < 3; i++) {
        Texture2D* shardTexture = this->shardTextures[i];
        assert(shardTexture != NULL);

        ESPVolumeEmitter* shardEmitter = new ESPVolumeEmitter();
	    shardEmitter->SetSpawnDelta(emitterSpawn);
	    shardEmitter->SetInitialSpd(emitterSpd);
	    shardEmitter->SetParticleLife(emitterLife);
	    shardEmitter->SetParticleSize(xSize, ySize);
	    shardEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	    shardEmitter->SetEmitVolume(minPt, maxPt);
	    shardEmitter->SetEmitDirection(Vector3D(0, 1, 0));
	    shardEmitter->SetParticleAlignment(ESP::ScreenAligned);
	    shardEmitter->SetParticleColour(ESPInterval(shardColour.R()), ESPInterval(shardColour.G()), ESPInterval(shardColour.B()), ESPInterval(0.5f, 1.0f));
	    if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
		    shardEmitter->AddEffector(&this->rotateEffectorCW);
	    }
	    else {
		    shardEmitter->AddEffector(&this->rotateEffectorCCW);
	    }
        shardEmitter->SetParticles(20, shardTexture);
        this->shardEmitters.push_back(shardEmitter);
    }

    ESPVolumeEmitter* shardEmitter = new ESPVolumeEmitter();
	shardEmitter->SetSpawnDelta(emitterSpawn);
	shardEmitter->SetInitialSpd(emitterSpd);
	shardEmitter->SetParticleLife(emitterLife);
	shardEmitter->SetParticleSize(ESPInterval(3.0f, 8.0f));
	shardEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	shardEmitter->SetEmitVolume(minPt, maxPt);
	shardEmitter->SetEmitDirection(Vector3D(0, 1, 0));
	shardEmitter->SetParticleAlignment(ESP::ScreenAligned);
	shardEmitter->SetParticleColour(ESPInterval(shardColour.R()), ESPInterval(shardColour.G()), ESPInterval(shardColour.B()), ESPInterval(0.5f, 1.0f));
	if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
		shardEmitter->AddEffector(&this->rotateEffectorCW);
	}
	else {
		shardEmitter->AddEffector(&this->rotateEffectorCCW);
	}
    shardEmitter->SetParticles(20, this->shardTextures[3]);

    this->shardEmitters.push_back(shardEmitter);

	// Tick all the emitters for a bit to get them to look like they've been spawning for awhile
	for (unsigned int i = 0; i < 60; i++) {
        for (std::vector<ESPVolumeEmitter*>::iterator iter = this->shardEmitters.begin();
             iter != this->shardEmitters.end(); ++iter) {
            ESPVolumeEmitter* currEmitter = *iter;
            currEmitter->Tick(0.5);
        }
	}

}