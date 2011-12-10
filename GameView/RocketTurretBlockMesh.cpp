/**
 * RocketTurretBlockMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "RocketTurretBlockMesh.h"
#include "GameViewConstants.h"
#include "GameFontAssetsManager.h"

#include "../ResourceManager.h"

RocketTurretBlockMesh::RocketTurretBlockMesh() : barrelMesh(NULL),
headMesh(NULL), baseMesh(NULL), glowTexture(NULL), sparkleTexture(NULL) {
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

    this->glowTexture = static_cast<Texture2D*>(
        ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_CIRCLE_GRADIENT, Texture::Trilinear));
    assert(this->glowTexture != NULL);
    this->sparkleTexture = static_cast<Texture2D*>(
        ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_SPARKLE, Texture::Trilinear));
    assert(this->sparkleTexture != NULL);

    std::vector<float> values;
    values.reserve(3);
    values.push_back(0.66f);
    values.push_back(1.0f);
    values.push_back(0.66f);
    std::vector<double> times;
    times.reserve(3);
    times.push_back(0.0);
    times.push_back(1.0);
    times.push_back(2.0);

    this->lightPulseAnim.SetLerp(times, values);
    this->lightPulseAnim.SetRepeat(true);
}

RocketTurretBlockMesh::~RocketTurretBlockMesh() {
    this->Flush();

    bool success = false;
	success = ResourceManager::GetInstance()->ReleaseMeshResource(this->barrelMesh);
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

    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->glowTexture);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->sparkleTexture);
    assert(success);
}

void RocketTurretBlockMesh::Flush() {
    // Clean up the block data...
    for (BlockCollectionIter iter = this->blocks.begin(); iter != this->blocks.end(); ++iter) {
        BlockData* blockData = iter->second;
        delete blockData;
        blockData = NULL;
    }
    this->blocks.clear();
}

void RocketTurretBlockMesh::AddRocketTurretBlock(const RocketTurretBlock* rocketTurretBlock) {
	assert(rocketTurretBlock != NULL);
    std::pair<BlockCollectionIter, bool> insertResult = 
        this->blocks.insert(std::make_pair(rocketTurretBlock,
        new BlockData(*rocketTurretBlock, this->glowTexture, this->sparkleTexture, this->smokeTextures)));
	assert(insertResult.second);
}

void RocketTurretBlockMesh::RemoveRocketTurretBlock(const RocketTurretBlock* rocketTurretBlock) {
    BlockCollectionConstIter findIter = this->blocks.find(rocketTurretBlock);
    assert(findIter != this->blocks.end());

    BlockData* blockData = findIter->second;
    delete blockData;
    blockData = NULL;
    this->blocks.erase(findIter);
}

void RocketTurretBlockMesh::Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
                                 const BasicPointLight& fillLight, const BasicPointLight& ballLight) {
    if (this->blocks.empty()) {
        return;
    }

    glPushAttrib(GL_CURRENT_BIT);

	// Go through each block and draw the barrels
    for (BlockCollectionConstIter iter = this->blocks.begin(); iter != this->blocks.end(); ++iter) {

		const RocketTurretBlock* currBlock = iter->first;
        BlockData* currBlockData = iter->second;

		const Point2D& blockCenter = currBlock->GetCenter();

		glPushMatrix();
		glTranslatef(blockCenter[0], blockCenter[1], 0.0f);

        // Draw any effects for the current block...
        currBlockData->DrawBlockEffects(dT, camera, this->lightPulseAnim.GetInterpolantValue());
        float invFlashIntensity = 1.0f - currBlockData->GetFlashIntensity();
        glColor4f(1.0f, invFlashIntensity, invFlashIntensity, 1.0f);

        glRotatef(currBlock->GetRotationDegreesFromX(), 0, 0, 1);

        // Draw moving geometry for the current block...
        glPushMatrix();
        glTranslatef(currBlock->GetBarrelRecoilAmount(), 0, 0);
        this->barrelMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();

        this->headMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    glPopAttrib();

    this->lightPulseAnim.Tick(dT);
}

void RocketTurretBlockMesh::SetAlphaMultiplier(float alpha) {
    this->barrelMesh->SetAlpha(alpha);
    this->headMesh->SetAlpha(alpha);

    for (BlockCollectionConstIter iter = this->blocks.begin(); iter != this->blocks.end(); ++iter) {
        BlockData* currBlockData = iter->second;
        currBlockData->SetAlpha(alpha);
    }
}

// Informs the given block that it should emote that its state just changed
void RocketTurretBlockMesh::AIStateChanged(const RocketTurretBlock* block,
                                           const RocketTurretBlock::TurretAIState& oldState,
                                           const RocketTurretBlock::TurretAIState& newState) {
    assert(block != NULL);
    BlockCollectionConstIter findIter = this->blocks.find(block);
    assert(findIter != this->blocks.end());

    BlockData* currBlockData = findIter->second;
    currBlockData->BlockStateChanged(oldState, newState);
}

// Loads all the mesh assets for the laser turret block mesh
void RocketTurretBlockMesh::LoadMesh() {
	assert(this->barrelMesh == NULL);
    assert(this->headMesh == NULL);
	assert(this->baseMesh == NULL);
	assert(this->materialGroups.size() == 0);
	
    // Load the base mesh
    this->baseMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->TURRET_BASE_MESH);
    assert(this->baseMesh != NULL);
    const std::map<std::string, MaterialGroup*>& baseMatGrps = this->baseMesh->GetMaterialGroups();
    this->materialGroups.insert(baseMatGrps.begin(), baseMatGrps.end());

    assert(this->materialGroups.size() > 0);

    // Load the head mesh (holds the turret's barrel)
    this->headMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->ROCKET_TURRET_HEAD_MESH);
    assert(this->headMesh != NULL);

	// We don't add the turret's barrel material group, we will draw those manually for each rocket turret block
	// so that we can move them around (i.e., for recoil when the turret shoots rockets)
    this->barrelMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->ROCKET_TURRET_BARREL_MESH);
    assert(this->barrelMesh != NULL);
}

RocketTurretBlockMesh::BlockData::BlockData(const RocketTurretBlock& block, Texture2D* glowTexture,
                                           Texture2D* sparkleTexture, std::vector<Texture2D*>& smokeTextures) : 
block(block), fireySmokeEmitter(NULL), smokeySmokeEmitter(NULL), glowTexture(glowTexture), sparkleTexture(sparkleTexture),
emoteLabel(new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Small), "")),
particleMediumGrowth(1.0f, 1.6f), particleLargeGrowth(1.0f, 2.2f),
smokeColourFader(ColourRGBA(0.7f, 0.7f, 0.7f, 1.0f), ColourRGBA(0.1f, 0.1f, 0.1f, 0.1f)),
particleFireColourFader(ColourRGBA(1.0f, 1.0f, 0.1f, 1.0f), ColourRGBA(1.0f, 0.1f, 0.1f, 0.0f)),
rotateEffectorCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.25f, ESPParticleRotateEffector::CLOCKWISE),
rotateEffectorCCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.25f, ESPParticleRotateEffector::COUNTER_CLOCKWISE),
alpha(1.0f) {
    
    assert(glowTexture != NULL);
    assert(sparkleTexture != NULL);

    this->emoteLabel->SetTopLeftCorner(LevelPiece::PIECE_WIDTH / 2.0f, LevelPiece::PIECE_HEIGHT / 2.1f);
    this->emoteLabel->SetDropShadow(Colour(0,0,0), 0.2f);

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

    this->emoteScaleAnim.ClearLerp();
    this->emoteScaleAnim.SetInterpolantValue(0.0f);
    this->emoteScaleAnim.SetRepeat(false);
}

RocketTurretBlockMesh::BlockData::~BlockData() {
    delete this->smokeySmokeEmitter;
    this->smokeySmokeEmitter = NULL;
    delete this->fireySmokeEmitter;
    this->fireySmokeEmitter = NULL;
    delete this->emoteLabel;
    this->emoteLabel = NULL;
}

void RocketTurretBlockMesh::BlockData::DrawBlockEffects(double dT, const Camera& camera, float lightPulseAmt) {

    if (this->block.GetHealthPercent() <= 0.75f) {
        
        float percentToDeathFrom75 = (0.75f - this->block.GetHealthPercent()) / 0.75f;
        this->smokeySmokeEmitter->SetParticleLife(ESPInterval(0.6f + percentToDeathFrom75, 1.2f + percentToDeathFrom75));
        this->smokeySmokeEmitter->SetParticleSize(ESPInterval(0.25f*percentToDeathFrom75 + 0.1f * LevelPiece::PIECE_WIDTH,
            0.25f*percentToDeathFrom75 + 0.45f * LevelPiece::PIECE_WIDTH));
        
        this->smokeySmokeEmitter->Draw(camera);

        if (this->block.GetHealthPercent() <= 0.5f) {
            float percentToDeathFrom50 = (0.5f - this->block.GetHealthPercent()) / 0.5f;
            this->fireySmokeEmitter->SetInitialSpd(ESPInterval(0.4f*percentToDeathFrom50 + 0.8f, 0.4f*percentToDeathFrom50 + 1.5f));
            this->fireySmokeEmitter->SetParticleLife(ESPInterval(0.5f*percentToDeathFrom50 + 0.6f, 0.5f*percentToDeathFrom50 + 1.2f));
            this->fireySmokeEmitter->SetParticleSize(ESPInterval(0.2f*percentToDeathFrom50 + 0.1f * LevelPiece::PIECE_WIDTH,
                0.2f*percentToDeathFrom50 + 0.45f * LevelPiece::PIECE_WIDTH));

            this->fireySmokeEmitter->Draw(camera);
            this->fireySmokeEmitter->Tick(dT);

            if (this->block.GetHealthPercent() <= RocketTurretBlock::ONE_MORE_BALL_HIT_LIFE_PERCENT) {
                this->redColourMultiplierAnim.Tick(dT);
            }
        }

        this->smokeySmokeEmitter->Tick(dT);
    }

    if (this->emoteScaleAnim.GetInterpolantValue() != 0.0f) {
        this->emoteLabel->SetScale(this->emoteScaleAnim.GetInterpolantValue());
        this->emoteLabel->Draw3D(camera, 0.0f, 2*LevelPiece::PIECE_DEPTH);
    }
    this->emoteScaleAnim.Tick(dT);

    this->DrawLights(lightPulseAmt);
}

void RocketTurretBlockMesh::BlockData::DrawLights(float pulse) {
    const float LIGHT_ALPHA = this->alpha * 0.8f;

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
	
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    if (this->block.GetTurretAIState() == RocketTurretBlock::IdleTurretState) {
        glColor4f(1,0,0,LIGHT_ALPHA);    
    }
    else {
        glColor4f(0,1,0,LIGHT_ALPHA);
    }

    float glowScale = pulse * 0.33f;

    this->glowTexture->BindTexture();
    glPushMatrix();
    glTranslatef(LevelPiece::PIECE_WIDTH / 2.0f, 0.0f, 0.0f);
    glScalef(glowScale, glowScale, 1.0f);
    GeometryMaker::GetInstance()->DrawQuad();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-LevelPiece::PIECE_WIDTH / 2.0f, 0.0f, 0.0f);
    glScalef(glowScale, glowScale, 1.0f);
    GeometryMaker::GetInstance()->DrawQuad();
    glPopMatrix();

    float doubleGlowScale = 2.5f * glowScale;
    if (this->block.GetTurretAIState() == RocketTurretBlock::IdleTurretState) {
        glColor4f(1.0f, 0.9f, 0.9f, LIGHT_ALPHA);    
    }
    else {
        glColor4f(0.9f, 1.0f, 0.9f, LIGHT_ALPHA);
    }
    this->sparkleTexture->BindTexture();
    glPushMatrix();
    glTranslatef(LevelPiece::PIECE_WIDTH / 2.0f, 0.0f, 0.0f);
    glScalef(doubleGlowScale, doubleGlowScale, 1.0f);
    GeometryMaker::GetInstance()->DrawQuad();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-LevelPiece::PIECE_WIDTH / 2.0f, 0.0f, 0.0f);
    glScalef(doubleGlowScale, doubleGlowScale, 1.0f);
    GeometryMaker::GetInstance()->DrawQuad();
    glPopMatrix();
    this->sparkleTexture->UnbindTexture();

    glPopAttrib();
}

void RocketTurretBlockMesh::BlockData::BlockStateChanged(const RocketTurretBlock::TurretAIState& oldState,
                                                         const RocketTurretBlock::TurretAIState& newState) {

    switch (newState) {

        case RocketTurretBlock::IdleTurretState: {
            this->emoteLabel->SetText("");
            this->emoteScaleAnim.ClearLerp();
            this->emoteScaleAnim.SetInterpolantValue(0.0f);
            this->emoteScaleAnim.SetRepeat(false);
            break;
        }

        case RocketTurretBlock::SeekingTurretState: {
            /*
            if (oldState != RocketTurretBlock::IdleTurretState) {
                this->emoteLabel->SetText("...");
                this->emoteLabel->SetColour(Colour(1,1,1));
                
                std::vector<float> values;
                std::vector<double> times;
                BlockData::InitTimeValueEmoteAnimVectors(times, values, 3.0f);

                this->emoteScaleAnim.SetLerp(times, values);
                this->emoteScaleAnim.SetRepeat(false);
            }
            */
            break;
        }

        case RocketTurretBlock::TargetFoundTurretState: {
            if (oldState != RocketTurretBlock::TargetFoundTurretState) {
                this->emoteLabel->SetText("!");
                this->emoteLabel->SetColour(Colour(1,0,0));

                std::vector<float> values;
                std::vector<double> times;
                BlockData::InitTimeValueEmoteAnimVectors(times, values, 2.5f);

                this->emoteScaleAnim.SetLerp(times, values);
                this->emoteScaleAnim.SetRepeat(false);
            }
            break;
        }

        case RocketTurretBlock::TargetLostTurretState: {
            if (oldState != RocketTurretBlock::TargetLostTurretState) {
                this->emoteLabel->SetText("?");
                this->emoteLabel->SetColour(Colour(1, 0.7f, 0));

                std::vector<float> values;
                std::vector<double> times;
                BlockData::InitTimeValueEmoteAnimVectors(times, values, 1.5f);

                this->emoteScaleAnim.SetLerp(times, values);
                this->emoteScaleAnim.SetRepeat(false);
            }
            break;
        }

        default:
            assert(false);
            break;

    }
}

void RocketTurretBlockMesh::BlockData::SetAlpha(float alpha) {
    this->alpha = alpha;
}

void RocketTurretBlockMesh::BlockData::InitTimeValueEmoteAnimVectors(std::vector<double>& times,
                                                                    std::vector<float>& values,
                                                                    float lifeTimeInSecs) {
    values.reserve(4);
    values.push_back(0.0f);
    values.push_back(1.0f);
    values.push_back(1.0f);
    values.push_back(0.0f);

    times.reserve(4);
    times.push_back(0.0);
    times.push_back(0.25);
    times.push_back(0.25 + lifeTimeInSecs);
    times.push_back(0.25 + lifeTimeInSecs + 0.25);
}