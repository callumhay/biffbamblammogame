/**
 * TeslaBlockMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "TeslaBlockMesh.h"
#include "GameViewConstants.h"
#include "CgFxPostRefract.h"

#include "../BlammoEngine/Texture2D.h"
#include "../ESPEngine/ESPPointEmitter.h"
#include "../GameModel/TeslaBlock.h"

const float TeslaBlockMesh::COIL_ROTATION_SPEED_DEGSPERSEC = 270;

TeslaBlockMesh::TeslaBlockMesh() : teslaBaseMesh(NULL), teslaCoilMesh(NULL), flarePulse(0,0), 
/*sparkGravity(Vector3D(0, -9.8f, 0)), sparkTex(NULL), teslaSparks(NULL),*/
teslaCenterFlare(NULL), flareTex(NULL), haloTexture(NULL), haloExpandPulse(1.0f, 3.0f), haloFader(1.0f, 0.15f) {
	this->LoadMesh();
	
	// Grab the flare texture
	this->flareTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_BRIGHT_FLARE, Texture::Trilinear, GL_TEXTURE_2D));
	assert(this->flareTex != NULL);

    this->haloTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_HALO, Texture::Trilinear));
    assert(this->haloTexture != NULL);

    // Halo effect that pulses outwards when a switch block is turned on
    const float HALO_LIFETIME = 1.5f;
    const int NUM_HALOS = 3;
    this->shieldEmitter = new ESPPointEmitter();
    this->shieldEmitter->SetSpawnDelta(ESPInterval(HALO_LIFETIME / static_cast<float>(NUM_HALOS)));
    this->shieldEmitter->SetInitialSpd(ESPInterval(0));
    this->shieldEmitter->SetParticleLife(ESPInterval(HALO_LIFETIME));
    this->shieldEmitter->SetParticleSize(ESPInterval(1.5 * LevelPiece::HALF_PIECE_WIDTH), ESPInterval(1.5 * LevelPiece::HALF_PIECE_HEIGHT));
    this->shieldEmitter->SetEmitAngleInDegrees(0);
    this->shieldEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    this->shieldEmitter->SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
    this->shieldEmitter->SetEmitPosition(Point3D(0,0,0));
    this->shieldEmitter->SetParticleColour(ESPInterval(0.9f), ESPInterval(0.7f), ESPInterval(1.0f), ESPInterval(0.85f));
    this->shieldEmitter->AddEffector(&this->haloExpandPulse);
    this->shieldEmitter->AddEffector(&this->haloFader);
    this->shieldEmitter->SetParticles(NUM_HALOS, this->haloTexture);

	// Set up the pulse effector for the flare emitter
	ScaleEffect flarePulseSettings;
	flarePulseSettings.pulseGrowthScale = 1.25f;
	flarePulseSettings.pulseRate        = 5.0f;
	this->flarePulse = ESPParticleScaleEffector(flarePulseSettings);

	// Setup the flare emitter
	this->teslaCenterFlare = new ESPPointEmitter();
	this->teslaCenterFlare->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	this->teslaCenterFlare->SetInitialSpd(ESPInterval(0));
	this->teslaCenterFlare->SetParticleLife(ESPInterval(ESPParticle::INFINITE_PARTICLE_LIFETIME));
	this->teslaCenterFlare->SetEmitAngleInDegrees(0);
	this->teslaCenterFlare->SetParticleAlignment(ESP::ScreenAligned);
	this->teslaCenterFlare->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->teslaCenterFlare->SetEmitPosition(Point3D(0, 0, 0));
	this->teslaCenterFlare->SetParticleSize(ESPInterval(LevelPiece::PIECE_WIDTH));
	this->teslaCenterFlare->SetParticleColour(ESPInterval(0.9f), ESPInterval(0.8f), ESPInterval(1.0f), ESPInterval(1.0f));
	this->teslaCenterFlare->AddEffector(&this->flarePulse);
	this->teslaCenterFlare->SetParticles(1, this->flareTex);

	/*
	// DECIDED NOT TO DO SPARKS SINCE EACH TESLA BLOCK WOULD NEED ITS OWN AND THEY WOULD NEED TO BE RESET
	// EVERY TIME A TESLA BLOCK TURNS OFF.. NOT WORTH IT
	// Grab the spark texture
	this->sparkTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SPARKLE, 
		Texture::Trilinear, GL_TEXTURE_2D));
	assert(this->sparkTex != NULL);
	
	this->teslaSparks = new ESPPointEmitter();
	// ...
	*/
}

TeslaBlockMesh::~TeslaBlockMesh() {
    bool success = false;
	success = ResourceManager::GetInstance()->ReleaseMeshResource(this->teslaBaseMesh);
    assert(success);
	success = ResourceManager::GetInstance()->ReleaseMeshResource(this->teslaCoilMesh);
    assert(success);
	success = ResourceManager::GetInstance()->ReleaseTextureResource(this->flareTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->haloTexture);
    assert(success);
    UNUSED_VARIABLE(success);

	delete this->teslaCenterFlare;
	this->teslaCenterFlare = NULL;
    delete this->shieldEmitter;
    this->shieldEmitter = NULL;
}

void TeslaBlockMesh::DrawPostEffects(double dT, const Camera& camera, const BasicPointLight& keyLight, 
                                     const BasicPointLight& fillLight, const BasicPointLight& ballLight) {

    if (this->teslaBlocks.empty()) {
        return;
    }

    this->teslaCenterFlare->Tick(dT);
    this->shieldEmitter->Tick(dT);

    float rotationAmt = dT * COIL_ROTATION_SPEED_DEGSPERSEC;

    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);
    
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    Matrix4x4 screenAlignMatrix = camera.GenerateScreenAlignMatrix();

    for (TeslaBlockSetIter iter = this->teslaBlocks.begin(); iter != this->teslaBlocks.end(); ++iter) {
        TeslaBlock* currTeslaBlock = *iter;
        assert(currTeslaBlock != NULL);

        const Vector3D& currRotationAxis = currTeslaBlock->GetRotationAxis();
        float currRotationAmt = currTeslaBlock->GetRotationAmount();
        const Point2D& blockCenter = currTeslaBlock->GetCenter();

        glPushMatrix();
        glTranslatef(blockCenter[0], blockCenter[1], 0.0f);

        glPushMatrix();
        glRotatef(currRotationAmt, currRotationAxis[0], currRotationAxis[1], currRotationAxis[2]);
        glScalef(1.2f, 1.2f, 1.2f);
        this->teslaCoilMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();

        // Only draw the flare in the middle and only rotate the center if the Tesla block is active
        if (currTeslaBlock->GetIsElectricityActive()) {
            this->teslaCenterFlare->DrawWithDepth(camera);
            currRotationAmt += rotationAmt;
            currTeslaBlock->SetRotationAmount(currRotationAmt);
        }

        // Draw a shield around the Tesla block if it cannot be changed
        if (!currTeslaBlock->GetIsChangable()) {
            this->shieldEmitter->Draw(camera);
        }

        glPopMatrix();
    }

    glPopAttrib();

}

void TeslaBlockMesh::SetAlphaMultiplier(float alpha) {
	this->teslaCoilMesh->SetAlpha(alpha);
	this->teslaCenterFlare->SetParticleAlpha(ESPInterval(alpha));
}

void TeslaBlockMesh::LoadMesh() {
	assert(this->teslaBaseMesh == NULL);
	assert(this->teslaCoilMesh == NULL);
	assert(this->materialGroups.size() == 0);
	
	this->teslaBaseMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->TESLA_BLOCK_BASE_MESH);
	assert(this->teslaBaseMesh != NULL);
	const std::map<std::string, MaterialGroup*>&  baseMatGrps = this->teslaBaseMesh->GetMaterialGroups();

	// We don't add the coil's material groups, we will draw those manually for each tesla block
	// so that we can move them around
	this->teslaCoilMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->TESLA_BLOCK_COIL_MESH);
	assert(this->teslaCoilMesh != NULL);
	
	this->materialGroups.insert(baseMatGrps.begin(), baseMatGrps.end());
	
	assert(this->materialGroups.size() > 0);
}