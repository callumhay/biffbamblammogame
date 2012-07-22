/**
 * CannonBlockMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "CannonBlockMesh.h"
#include "GameViewConstants.h"

#include "../ESPEngine/ESPPointEmitter.h"
#include "../GameModel/CannonBlock.h"
#include "../ResourceManager.h"

CannonBlockMesh::CannonBlockMesh() : cannonBlockBaseGeometry(NULL),
cannonBlockBarrelGeometry(NULL), haloTexture(NULL), haloExpandPulse(1.0f, 2.75f), haloFader(1.0f, 0.10f),
activeCannonEffectEmitter(NULL) {

	this->LoadMesh();

	assert(this->haloTexture == NULL);
	this->haloTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_HALO, Texture::Trilinear));
	assert(this->haloTexture != NULL);

	// Halo effect that pulses outwards
	this->activeCannonEffectEmitter = new ESPPointEmitter();
	this->activeCannonEffectEmitter->SetSpawnDelta(ESPInterval(0.5f));
	this->activeCannonEffectEmitter->SetInitialSpd(ESPInterval(0));
	this->activeCannonEffectEmitter->SetParticleLife(ESPInterval(0.5f));
	this->activeCannonEffectEmitter->SetParticleSize(ESPInterval(CannonBlock::CANNON_BARREL_LENGTH), ESPInterval(CannonBlock::CANNON_BARREL_LENGTH));
	this->activeCannonEffectEmitter->SetEmitAngleInDegrees(0);
	this->activeCannonEffectEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->activeCannonEffectEmitter->SetParticleAlignment(ESP::ScreenAligned);
	this->activeCannonEffectEmitter->SetEmitPosition(Point3D(0,0,0));
	this->activeCannonEffectEmitter->SetParticleColour(ESPInterval(0.75f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	this->activeCannonEffectEmitter->AddEffector(&this->haloExpandPulse);
	this->activeCannonEffectEmitter->AddEffector(&this->haloFader);
	bool result = this->activeCannonEffectEmitter->SetParticles(1, this->haloTexture);
    UNUSED_VARIABLE(result);
	assert(result);
}

CannonBlockMesh::~CannonBlockMesh() {
	bool success = ResourceManager::GetInstance()->ReleaseMeshResource(this->cannonBlockBarrelGeometry);
	assert(success);
	success = ResourceManager::GetInstance()->ReleaseMeshResource(this->cannonBlockBaseGeometry);
	assert(success);
	success = ResourceManager::GetInstance()->ReleaseTextureResource(this->haloTexture);
	assert(success);

	delete this->activeCannonEffectEmitter;
	this->activeCannonEffectEmitter = NULL;
}


void CannonBlockMesh::Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, 
                           const BasicPointLight& fillLight, const BasicPointLight& ballLight, 
                           bool lightsAreOff) const {

	bool doCannonActiveEffectTick = false;

	// Go through each of the cannon blocks and draw them, each with their proper,
	// respective barrel orientation
	float cannonRotationInDegs;
	for (std::set<const CannonBlock*>::const_iterator iter = this->cannonBlocks.begin(); iter != this->cannonBlocks.end(); ++iter) {
		const CannonBlock* currCannonBlock = *iter;
		const GameBall* loadedBall = currCannonBlock->GetLoadedBall();

		// If the current cannon block has a ball loaded in it with the ball camera 
		// on then don't draw the barrel
		if (loadedBall != NULL && loadedBall->GetIsBallCameraOn()) {
			continue;
		}

		cannonRotationInDegs = currCannonBlock->GetCurrentCannonAngleInDegs();
		const Point2D& blockCenter = currCannonBlock->GetCenter();
		const Colour& cannonColour = currCannonBlock->GetColour();

		// Make sure the proper colour is set for the barrel
		glColor4f(cannonColour.R(), cannonColour.G(), cannonColour.B(), 1.0f);

		glPushMatrix();
		// Translate to the piece location in the game model...
		glTranslatef(blockCenter[0], blockCenter[1], 0.0f);
		// Rotate the barrel to its current direction
		glRotatef(cannonRotationInDegs, 0, 0, 1);

		// If the cannon is loaded then we highlight it with an effect to draw attention to it
		if (currCannonBlock->GetIsLoaded()) {
			this->activeCannonEffectEmitter->Draw(camera);
			doCannonActiveEffectTick = true;
		}

		// If the lights are out and a ball is inside the current cannon block 
		// then we illuminate the cannon block
		if (lightsAreOff && currCannonBlock->GetLoadedBall() != NULL) {
			BasicPointLight newKeyLight(Point3D(0, 0, -10), Colour(1, 1, 1), 0.01f);
			this->cannonBlockBarrelGeometry->Draw(camera, newKeyLight, fillLight, ballLight);
		}
		else {
			this->cannonBlockBarrelGeometry->Draw(camera, keyLight, fillLight, ballLight);
		}
		glPopMatrix();
	}
	
	if (doCannonActiveEffectTick) {
		this->activeCannonEffectEmitter->Tick(dT);
	}
}

void CannonBlockMesh::SetAlphaMultiplier(float alpha) {
	const std::map<std::string, MaterialGroup*>&  barrelMatGrps = this->cannonBlockBarrelGeometry->GetMaterialGroups();
	for (std::map<std::string, MaterialGroup*>::const_iterator iter = barrelMatGrps.begin(); iter != barrelMatGrps.end(); ++iter) {
		MaterialGroup* matGrp = iter->second;
		matGrp->GetMaterial()->GetProperties()->alphaMultiplier = alpha;
	}
}

void CannonBlockMesh::LoadMesh() {
	assert(this->cannonBlockBaseGeometry == NULL);
	assert(this->cannonBlockBarrelGeometry == NULL);
	assert(this->materialGroups.size() == 0);
	
	this->cannonBlockBaseGeometry = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->CANNON_BLOCK_BASE_MESH);
	assert(this->cannonBlockBaseGeometry != NULL);
	const std::map<std::string, MaterialGroup*>&  baseMatGrps = this->cannonBlockBaseGeometry->GetMaterialGroups();

	// We don't add the barrel's material groups, we will draw those manually for each cannon block
	// so that we can orient the barrels according to each block (see CannonBlockMesh::Draw).
	this->cannonBlockBarrelGeometry = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->CANNON_BLOCK_BARREL_MESH);
	assert(this->cannonBlockBarrelGeometry != NULL);
	
	this->materialGroups.insert(baseMatGrps.begin(), baseMatGrps.end());
	
	assert(this->materialGroups.size() > 0);
}