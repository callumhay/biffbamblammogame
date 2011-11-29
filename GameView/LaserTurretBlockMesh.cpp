/**
 * LaserTurretBlockMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "LaserTurretBlockMesh.h"
#include "GameViewConstants.h"

#include "../GameModel/LaserTurretBlock.h"

#include "../ResourceManager.h"

LaserTurretBlockMesh::LaserTurretBlockMesh() : barrel1Mesh(NULL), barrel2Mesh(NULL),
headMesh(NULL), baseMesh(NULL) {
    this->LoadMesh();

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

}

LaserTurretBlockMesh::~LaserTurretBlockMesh() {
    this->Flush();

    bool success = false;
	success = ResourceManager::GetInstance()->ReleaseMeshResource(this->barrel1Mesh);
	assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->barrel2Mesh);
	assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->headMesh);
	assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->baseMesh);
    assert(success);
    UNUSED_VARIABLE(success);

	for (std::vector<Texture2D*>::iterator iter = this->smokeTextures.begin();
		iter != this->smokeTextures.end(); ++iter) {
		
		success = ResourceManager::GetInstance()->ReleaseTextureResource(*iter);
		assert(success);	
	}
	this->smokeTextures.clear();
}

void LaserTurretBlockMesh::Flush() {
    // Clean up the block data...
    for (BlockCollectionIter iter = this->blocks.begin(); iter != this->blocks.end(); ++iter) {
        BlockData* blockData = iter->second;
        delete blockData;
        blockData = NULL;
    }
    this->blocks.clear();
}

void LaserTurretBlockMesh::AddLaserTurretBlock(const LaserTurretBlock* laserTurretBlock) {
	assert(laserTurretBlock != NULL);
    std::pair<BlockCollectionIter, bool> insertResult = 
        this->blocks.insert(std::make_pair(laserTurretBlock, new BlockData(*laserTurretBlock, this->smokeTextures)));
	assert(insertResult.second);
}

void LaserTurretBlockMesh::RemoveLaserTurretBlock(const LaserTurretBlock* laserTurretBlock) {
    BlockCollectionConstIter findIter = this->blocks.find(laserTurretBlock);
    assert(findIter != this->blocks.end());

    BlockData* blockData = findIter->second;
    delete blockData;
    blockData = NULL;
    this->blocks.erase(findIter);
}

void LaserTurretBlockMesh::Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
                                const BasicPointLight& fillLight, const BasicPointLight& ballLight) {
    if (this->blocks.empty()) {
        return;
    }

	glPushAttrib(GL_CURRENT_BIT);

	// Go through each block and draw the barrels
    for (BlockCollectionConstIter iter = this->blocks.begin(); iter != this->blocks.end(); ++iter) {

		const LaserTurretBlock* currBlock = iter->first;
        BlockData* currBlockData = iter->second;

		const Point2D& blockCenter = currBlock->GetCenter();

		glPushMatrix();
		glTranslatef(blockCenter[0], blockCenter[1], 0.0f);

        // Draw any effects for the current block...
        currBlockData->DrawBlockEffects(dT, camera);
        float invFlashIntensity = 1.0f - currBlockData->GetFlashIntensity();
        glColor4f(1.0f, invFlashIntensity, invFlashIntensity, 1.0f);

        glRotatef(currBlock->GetRotationDegreesFromX(), 0, 0, 1);

        // Draw moving geometry for the current block...
        glPushMatrix();
        glTranslatef(currBlock->GetBarrel1RecoilAmount(), 0, 0);
        this->barrel1Mesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(currBlock->GetBarrel2RecoilAmount(), 0, 0);
        this->barrel2Mesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();

        this->headMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    glPopAttrib();
}

// Loads all the mesh assets for the laser turret block mesh
void LaserTurretBlockMesh::LoadMesh() {
    assert(this->barrel1Mesh == NULL);
	assert(this->barrel2Mesh == NULL);
    assert(this->headMesh == NULL);
	assert(this->baseMesh == NULL);
	assert(this->materialGroups.size() == 0);
	
    // Load the base mesh
    this->baseMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->LASER_TURRET_BASE_MESH);
    assert(this->baseMesh != NULL);
    const std::map<std::string, MaterialGroup*>& baseMatGrps = this->baseMesh->GetMaterialGroups();
    this->materialGroups.insert(baseMatGrps.begin(), baseMatGrps.end());

    assert(this->materialGroups.size() > 0);

    // Load the head mesh (holds the turret's barrels)
    this->headMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->LASER_TURRET_HEAD_MESH);
    assert(this->headMesh != NULL);

	// We don't add the turret's barrel material groups, we will draw those manually for each laser turret block
	// so that we can move them around (i.e., for recoil when the turret shoots lasers)
    this->barrel1Mesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->LASER_TURRET_BARREL_1);
    assert(this->barrel1Mesh != NULL);
    this->barrel2Mesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->LASER_TURRET_BARREL_2);
    assert(this->barrel2Mesh != NULL);
}

LaserTurretBlockMesh::BlockData::BlockData(const LaserTurretBlock& block,
                                           std::vector<Texture2D*>& smokeTextures) : 
block(block), fireySmokeEmitter(NULL), smokeySmokeEmitter(NULL),
particleMediumGrowth(1.0f, 1.6f), particleLargeGrowth(1.0f, 2.2f),
smokeColourFader(ColourRGBA(0.7f, 0.7f, 0.7f, 1.0f), ColourRGBA(0.1f, 0.1f, 0.1f, 0.1f)),
particleFireColourFader(ColourRGBA(1.0f, 1.0f, 0.1f, 1.0f), ColourRGBA(1.0f, 0.1f, 0.1f, 0.0f)),
rotateEffectorCW(2.0f, ESPParticleRotateEffector::CLOCKWISE),
rotateEffectorCCW(2.0f, ESPParticleRotateEffector::COUNTER_CLOCKWISE) {

	size_t randomTexIndex1 = Randomizer::GetInstance()->RandomUnsignedInt() % smokeTextures.size();
	size_t randomTexIndex2 = (randomTexIndex1 + 1) % smokeTextures.size();

    // Setup the emitters...
	this->fireySmokeEmitter = new ESPPointEmitter();
	this->fireySmokeEmitter->SetSpawnDelta(ESPInterval(0.2f, 0.35f));
	this->fireySmokeEmitter->SetInitialSpd(ESPInterval(0.8f, 1.5f));
	this->fireySmokeEmitter->SetParticleLife(ESPInterval(0.5f, 1.0f));
    this->fireySmokeEmitter->SetParticleSize(ESPInterval(0.1f * LevelPiece::PIECE_WIDTH, 0.45f * LevelPiece::PIECE_WIDTH));
	this->fireySmokeEmitter->SetEmitAngleInDegrees(30);
	this->fireySmokeEmitter->SetEmitDirection(Vector3D(0, 1, 0));
	this->fireySmokeEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f, LevelPiece::PIECE_HEIGHT/2.2f));
	this->fireySmokeEmitter->SetParticleAlignment(ESP::ScreenAligned);
	this->fireySmokeEmitter->SetEmitPosition(Point3D(0.0f, 0.0f, 0.0f));
	this->fireySmokeEmitter->AddEffector(&this->particleFireColourFader);
	this->fireySmokeEmitter->AddEffector(&this->particleMediumGrowth);
	this->fireySmokeEmitter->AddEffector(&this->rotateEffectorCW);
	bool result = this->fireySmokeEmitter->SetParticles(10, smokeTextures[randomTexIndex1]);
	assert(result);

	this->smokeySmokeEmitter = new ESPPointEmitter();
	this->smokeySmokeEmitter->SetSpawnDelta(ESPInterval(0.2f, 0.35f));
	this->smokeySmokeEmitter->SetInitialSpd(ESPInterval(0.8f, 1.5f));
	this->smokeySmokeEmitter->SetParticleLife(ESPInterval(0.5f, 1.0f));
    this->smokeySmokeEmitter->SetParticleSize(ESPInterval(0.1f * LevelPiece::PIECE_WIDTH, 0.45f * LevelPiece::PIECE_WIDTH));
	this->smokeySmokeEmitter->SetEmitAngleInDegrees(30);
	this->smokeySmokeEmitter->SetEmitDirection(Vector3D(0, 1, 0));
	this->smokeySmokeEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f, LevelPiece::PIECE_HEIGHT/2.2f));
	this->smokeySmokeEmitter->SetParticleAlignment(ESP::ScreenAligned);
	this->smokeySmokeEmitter->SetEmitPosition(Point3D(0.0f, 0.0f, 0.0f));
	this->smokeySmokeEmitter->AddEffector(&this->smokeColourFader);
	this->smokeySmokeEmitter->AddEffector(&this->particleLargeGrowth);
	this->smokeySmokeEmitter->AddEffector(&this->rotateEffectorCW);
	result = this->smokeySmokeEmitter->SetParticles(10, smokeTextures[randomTexIndex2]);
	assert(result);

    std::vector<float> values;
    values.reserve(3);
    values.push_back(0.0f);
    values.push_back(0.7f);
    values.push_back(0.0f);
    std::vector<double> times;
    times.reserve(3);
    times.push_back(0.0);
    times.push_back(0.5);
    times.push_back(1.0);
    this->redColourMultiplierAnim.SetLerp(times, values);
    this->redColourMultiplierAnim.SetRepeat(true);
    this->redColourMultiplierAnim.SetInterpolantValue(0.0f);
}

LaserTurretBlockMesh::BlockData::~BlockData() {
    delete this->smokeySmokeEmitter;
    this->smokeySmokeEmitter = NULL;
    delete this->fireySmokeEmitter;
    this->fireySmokeEmitter = NULL;
}

void LaserTurretBlockMesh::BlockData::DrawBlockEffects(double dT, const Camera& camera) {

    if (this->block.GetHealthPercent() <= 0.75f) {
        
        float percentToDeathFrom75 = (0.75f - this->block.GetHealthPercent()) / 0.75f;
        this->smokeySmokeEmitter->SetParticleLife(ESPInterval(0.5f + percentToDeathFrom75, 1.0f + percentToDeathFrom75));
        this->smokeySmokeEmitter->SetParticleSize(ESPInterval(0.25f*percentToDeathFrom75 + 0.1f * LevelPiece::PIECE_WIDTH,
            0.25f*percentToDeathFrom75 + 0.45f * LevelPiece::PIECE_WIDTH));
        
        this->smokeySmokeEmitter->Draw(camera);

        if (this->block.GetHealthPercent() <= 0.5f) {
            float percentToDeathFrom50 = (0.5f - this->block.GetHealthPercent()) / 0.5f;
            this->fireySmokeEmitter->SetInitialSpd(ESPInterval(0.3f*percentToDeathFrom50 + 0.8f, 0.3f*percentToDeathFrom50 + 1.5f));
            this->fireySmokeEmitter->SetParticleLife(ESPInterval(0.4f*percentToDeathFrom50 + 0.5f, 0.4f*percentToDeathFrom50 + 1.0f));
            this->fireySmokeEmitter->SetParticleSize(ESPInterval(0.2f*percentToDeathFrom50 + 0.1f * LevelPiece::PIECE_WIDTH,
                0.2f*percentToDeathFrom50 + 0.45f * LevelPiece::PIECE_WIDTH));

            this->fireySmokeEmitter->Draw(camera);
            this->fireySmokeEmitter->Tick(dT);

            if (this->block.GetHealthPercent() <= 0.25) {
                this->redColourMultiplierAnim.Tick(dT);
            }

        }

        this->smokeySmokeEmitter->Tick(dT);
    }
}

float LaserTurretBlockMesh::BlockData::GetFlashIntensity() const {
    return this->redColourMultiplierAnim.GetInterpolantValue();
}