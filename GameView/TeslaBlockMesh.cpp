/**
 * TeslaBlockMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
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

TeslaBlockMesh::TeslaBlockMesh() : teslaBaseMesh(NULL), teslaCoilMesh(NULL), flarePulse(0,0), /*sparkGravity(Vector3D(0, -9.8f, 0)), sparkTex(NULL), teslaSparks(NULL),*/
teslaCenterFlare(NULL), flareTex(NULL), shieldTex(NULL), shieldAlpha(1.0f) {
	this->LoadMesh();
	
	// Grab the flare texture
	this->flareTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BRIGHT_FLARE, 
																																																  Texture::Trilinear, GL_TEXTURE_2D));
	assert(this->flareTex != NULL);

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
	this->teslaCenterFlare->SetParticleColour(ESPInterval(0.9f), ESPInterval(0.79f), ESPInterval(1.0f), ESPInterval(1.0f));
	this->teslaCenterFlare->AddEffector(&this->flarePulse);
	
	bool result = this->teslaCenterFlare->SetParticles(1, this->flareTex);
    UNUSED_VARIABLE(result);
	assert(result);

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

	this->shieldTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_HALO, Texture::Trilinear));
	assert(this->shieldTex != NULL);
}

TeslaBlockMesh::~TeslaBlockMesh() {
    bool success = false;
	success = ResourceManager::GetInstance()->ReleaseMeshResource(this->teslaBaseMesh);
    assert(success);
	success = ResourceManager::GetInstance()->ReleaseMeshResource(this->teslaCoilMesh);
    assert(success);
	success = ResourceManager::GetInstance()->ReleaseTextureResource(this->flareTex);
    assert(success);
	success = ResourceManager::GetInstance()->ReleaseTextureResource(this->shieldTex);
    assert(success);

    UNUSED_VARIABLE(success);

	delete this->teslaCenterFlare;
	this->teslaCenterFlare = NULL;
}

void TeslaBlockMesh::Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, 
                          const BasicPointLight& fillLight, const BasicPointLight& ballLight) {

    UNUSED_PARAMETER(dT);
	UNUSED_PARAMETER(keyLight);
    UNUSED_PARAMETER(fillLight);
    UNUSED_PARAMETER(ballLight);
    
    if (this->teslaBlocks.empty()) {
        return;
    }

	glPushAttrib(GL_CURRENT_BIT);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	
	//Vector3D alignNormalVec = -camera.GetNormalizedViewVector();
	//Vector3D alignUpVec		  = camera.GetNormalizedUpVector();
	//Vector3D alignRightVec	= Vector3D::Normalize(Vector3D::cross(alignUpVec, alignNormalVec));
	//Matrix4x4 screenAlignMatrix(alignRightVec, alignUpVec, alignNormalVec);
	Matrix4x4 screenAlignMatrix = camera.GenerateScreenAlignMatrix();

	// Go through each tesla block and transform their center's appropriately
	for (TeslaBlockSetConstIter iter = this->teslaBlocks.begin(); iter != this->teslaBlocks.end(); ++iter) {
		const TeslaBlock* currTeslaBlock = *iter;
		assert(currTeslaBlock != NULL);
		
        if (!currTeslaBlock->GetIsChangable()) {
            const Point2D& blockCenter = currTeslaBlock->GetCenter();

		    glPushMatrix();
		    glTranslatef(blockCenter[0], blockCenter[1], 0.0f);

		    // Draw a shield around the Tesla block if it cannot be changed
		    if (!currTeslaBlock->GetIsChangable()) {
			    this->DrawTeslaShield(screenAlignMatrix);
            }

            glPopMatrix();
        }
	}
	glPopAttrib();
}

void TeslaBlockMesh::DrawPostEffects(double dT, const Camera& camera, const BasicPointLight& keyLight, 
                                     const BasicPointLight& fillLight, const BasicPointLight& ballLight) {

    float rotationAmt = dT * COIL_ROTATION_SPEED_DEGSPERSEC;

    glPushAttrib(GL_CURRENT_BIT);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

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
            this->teslaCenterFlare->Draw(camera, Vector3D(0,0,0), true);
            currRotationAmt += rotationAmt;
            currTeslaBlock->SetRotationAmount(currRotationAmt);
        }

        glPopMatrix();
    }

    glPopAttrib();

    this->teslaCenterFlare->Tick(dT);
}

void TeslaBlockMesh::SetAlphaMultiplier(float alpha) {
	this->teslaCoilMesh->SetAlpha(alpha);
	this->teslaCenterFlare->SetParticleAlpha(ESPInterval(alpha));
	this->shieldAlpha = std::min<float>(alpha, 0.75f);
}

void TeslaBlockMesh::DrawTeslaShield(const Matrix4x4& screenAlignMatrix) {
	glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	glMultMatrixf(screenAlignMatrix.begin());
	glColor4f(0.9f, 0.7f, 1.0f, this->shieldAlpha);
	glScalef(LevelPiece::PIECE_WIDTH * 1.2f, LevelPiece::PIECE_HEIGHT * 1.2f, 1.0f);
	
	this->shieldTex->BindTexture();
	GeometryMaker::GetInstance()->DrawQuad();

	glPopMatrix();
	glPopAttrib();
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