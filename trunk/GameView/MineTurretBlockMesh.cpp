/**
 * MineTurretBlockMesh.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "MineTurretBlockMesh.h"
#include "GameViewConstants.h"
#include "GameFontAssetsManager.h"

#include "../GameModel/MineTurretProjectile.h"

#include "../ResourceManager.h"

MineTurretBlockMesh::MineTurretBlockMesh() : TurretBlockMesh(), barrelMesh(NULL), headMesh(NULL), mineMesh(NULL) {
    this->LoadMesh();

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

MineTurretBlockMesh::~MineTurretBlockMesh() {
    this->Flush();

    bool success = false;
	success = ResourceManager::GetInstance()->ReleaseMeshResource(this->barrelMesh);
	assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->headMesh);
	assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->mineMesh);
    assert(success);

    UNUSED_VARIABLE(success);
}

void MineTurretBlockMesh::Flush() {
    // Clean up the block data...
    for (BlockCollectionIter iter = this->blocks.begin(); iter != this->blocks.end(); ++iter) {
        BlockData* blockData = iter->second;
        delete blockData;
        blockData = NULL;
    }
    this->blocks.clear();
}

void MineTurretBlockMesh::AddMineTurretBlock(const MineTurretBlock* mineTurretBlock) {
	assert(mineTurretBlock != NULL);
    std::pair<BlockCollectionIter, bool> insertResult = 
        this->blocks.insert(std::make_pair(mineTurretBlock,
        new BlockData(*mineTurretBlock, this->glowTexture, this->sparkleTexture, this->smokeTextures)));
	assert(insertResult.second);
}

void MineTurretBlockMesh::RemoveMineTurretBlock(const MineTurretBlock* mineTurretBlock) {
    BlockCollectionConstIter findIter = this->blocks.find(mineTurretBlock);
    assert(findIter != this->blocks.end());

    BlockData* blockData = findIter->second;
    delete blockData;
    blockData = NULL;
    this->blocks.erase(findIter);
}

void MineTurretBlockMesh::Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
                               const BasicPointLight& fillLight, const BasicPointLight& ballLight) {

    if (this->blocks.empty()) {
        return;
    }

    glPushAttrib(GL_CURRENT_BIT);

	// Go through each block and draw the barrels
    for (BlockCollectionConstIter iter = this->blocks.begin(); iter != this->blocks.end(); ++iter) {

		const MineTurretBlock* currBlock = iter->first;
        BlockData* currBlockData = iter->second;

		const Point2D& blockCenter = currBlock->GetCenter();

		glPushMatrix();
		glTranslatef(blockCenter[0], blockCenter[1], 0.0f);

        // Draw any effects for the current block...
        currBlockData->DrawBlockEffects(dT, camera);
        float invFlashIntensity = 1.0f - currBlockData->GetFlashIntensity();
        glColor4f(1.0f, invFlashIntensity, invFlashIntensity, 1.0f);

        glRotatef(currBlock->GetRotationDegreesFromX(), 0, 0, 1);

        // Draw the rocket geometry in the block...
        if (currBlock->HasMineLoaded()) {
            glPushMatrix();
            glTranslatef(currBlock->GetMineTranslationFromCenter(), 0, MineTurretBlock::BARREL_OFFSET_EXTENT_ALONG_Z);
            glRotatef(-90, 0, 0, 1);
            glScalef(0.5f, 0.5f, 0.5f);
            this->mineMesh->Draw(camera, keyLight, fillLight, ballLight);
            glPopMatrix();
        }

        // Draw moving geometry for the current block...
        glPushMatrix();
        glTranslatef(currBlock->GetBarrelRecoilAmount(), 0, 0);
        
        // Translate the barrel to the x-axis, rotate and then translate back
        glTranslatef(0, 
                     MineTurretBlock::BARREL_OFFSET_EXTENT_ALONG_Y,
                     MineTurretBlock::BARREL_OFFSET_EXTENT_ALONG_Z);
        glRotatef(currBlock->GetBarrelAxisRotationDegrees(), 1, 0, 0);
        glTranslatef(0, 
                     -MineTurretBlock::BARREL_OFFSET_EXTENT_ALONG_Y,
                     -MineTurretBlock::BARREL_OFFSET_EXTENT_ALONG_Z);

        this->barrelMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();

        this->headMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    glPopAttrib();
}

void MineTurretBlockMesh::DrawPostEffects(double dT, const Camera& camera) {
    if (this->blocks.empty()) {
        return;
    }

    for (BlockCollectionConstIter iter = this->blocks.begin(); iter != this->blocks.end(); ++iter) {

        const MineTurretBlock* currBlock = iter->first;
        BlockData* currBlockData = iter->second;

        const Point2D& blockCenter = currBlock->GetCenter();

        glPushMatrix();
        glTranslatef(blockCenter[0], blockCenter[1], 0.0f);
        currBlockData->DrawBlockPostEffects(dT, camera, this->lightPulseAnim.GetInterpolantValue());
        glPopMatrix();
    }

    this->lightPulseAnim.Tick(dT);
}

void MineTurretBlockMesh::SetAlphaMultiplier(float alpha) {
    this->barrelMesh->SetAlpha(alpha);
    this->headMesh->SetAlpha(alpha);
    this->mineMesh->SetAlpha(alpha);

    for (BlockCollectionConstIter iter = this->blocks.begin(); iter != this->blocks.end(); ++iter) {
        BlockData* currBlockData = iter->second;
        currBlockData->SetAlpha(alpha);
    }
}

// Informs the given block that it should emote that its state just changed
void MineTurretBlockMesh::AIStateChanged(const MineTurretBlock* block,
                                         const MineTurretBlock::TurretAIState& oldState,
                                         const MineTurretBlock::TurretAIState& newState) {
    assert(block != NULL);
    BlockCollectionConstIter findIter = this->blocks.find(block);
    assert(findIter != this->blocks.end());

    BlockData* currBlockData = findIter->second;
    currBlockData->BlockStateChanged(oldState, newState);
}

void MineTurretBlockMesh::MineShotByBlock(const MineTurretBlock* block) {
    assert(block != NULL);
    BlockCollectionConstIter findIter = this->blocks.find(block);
    assert(findIter != this->blocks.end());

    BlockData* currBlockData = findIter->second;
    currBlockData->MineShotByBlock();
}

// Loads all the mesh assets for the mine turret block mesh
void MineTurretBlockMesh::LoadMesh() {
	assert(this->barrelMesh == NULL);
    assert(this->headMesh == NULL);
    assert(this->mineMesh == NULL);

    // Load the rocket mesh
    this->mineMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->MINE_MESH);
    assert(this->mineMesh != NULL);

    // Load the head mesh (holds the turret's barrel)
    this->headMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->MINE_TURRET_HEAD_MESH);
    assert(this->headMesh != NULL);

	// We don't add the turret's barrel material group, we will draw those manually for each rocket turret block
	// so that we can move them around (i.e., for recoil when the turret shoots rockets)
    this->barrelMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->MINE_TURRET_BARREL_MESH);
    assert(this->barrelMesh != NULL);
}

MineTurretBlockMesh::BlockData::BlockData(const MineTurretBlock& block, Texture2D* glowTexture,
                                          Texture2D* sparkleTexture, std::vector<Texture2D*>& smokeTextures) : 
block(block), fireySmokeEmitter(NULL), smokeySmokeEmitter(NULL), 
glowTexture(glowTexture), sparkleTexture(sparkleTexture),
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
	bool result = this->fireySmokeEmitter->SetRandomTextureParticles(10, smokeTextures);
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
	result = this->smokeySmokeEmitter->SetRandomTextureParticles(10, smokeTextures);
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

MineTurretBlockMesh::BlockData::~BlockData() {
    delete this->smokeySmokeEmitter;
    this->smokeySmokeEmitter = NULL;
    delete this->fireySmokeEmitter;
    this->fireySmokeEmitter = NULL;
    delete this->emoteLabel;
    this->emoteLabel = NULL;
}

void MineTurretBlockMesh::BlockData::DrawBlockEffects(double dT, const Camera& camera) {

    // If the block is frozen then we don't draw the smoke/fire effects
    if (!this->block.HasStatus(LevelPiece::IceCubeStatus)) {

        if (this->block.GetHealthPercent() <= 0.75f) {
            
            float percentToDeathFrom75 = (0.75f - this->block.GetHealthPercent()) / 0.75f;
            this->smokeySmokeEmitter->SetParticleLife(ESPInterval(0.6f + percentToDeathFrom75, 1.2f + percentToDeathFrom75));
            this->smokeySmokeEmitter->SetParticleSize(ESPInterval(0.25f*percentToDeathFrom75 + 0.1f * LevelPiece::PIECE_WIDTH,
                0.25f*percentToDeathFrom75 + 0.45f * LevelPiece::PIECE_WIDTH));
            
            this->smokeySmokeEmitter->Tick(dT);
            this->smokeySmokeEmitter->Draw(camera);

            if (this->block.GetHealthPercent() <= 0.5f) {
                float percentToDeathFrom50 = (0.5f - this->block.GetHealthPercent()) / 0.5f;
                this->fireySmokeEmitter->SetInitialSpd(ESPInterval(0.4f*percentToDeathFrom50 + 0.8f, 0.4f*percentToDeathFrom50 + 1.5f));
                this->fireySmokeEmitter->SetParticleLife(ESPInterval(0.5f*percentToDeathFrom50 + 0.6f, 0.5f*percentToDeathFrom50 + 1.2f));
                this->fireySmokeEmitter->SetParticleSize(ESPInterval(0.2f*percentToDeathFrom50 + 0.1f * LevelPiece::PIECE_WIDTH,
                    0.2f*percentToDeathFrom50 + 0.45f * LevelPiece::PIECE_WIDTH));

                this->fireySmokeEmitter->Tick(dT);
                this->fireySmokeEmitter->Draw(camera);
                
                if (this->block.GetHealthPercent() <= this->block.GetLifePercentForOneMoreBallHit()) {
                    this->redColourMultiplierAnim.Tick(dT);
                }
            }
        }
    }
}

void MineTurretBlockMesh::BlockData::DrawBlockPostEffects(double dT, const Camera& camera, float lightPulseAmt) {
    if (this->emoteScaleAnim.GetInterpolantValue() != 0.0f) {
        this->emoteLabel->SetScale(this->emoteScaleAnim.GetInterpolantValue());
        this->emoteLabel->Draw3D(camera, 0.0f, 2*LevelPiece::PIECE_DEPTH);
    }
    this->emoteScaleAnim.Tick(dT);

    this->DrawLights(lightPulseAmt);
}

void MineTurretBlockMesh::BlockData::DrawLights(float pulse) {
    const float LIGHT_ALPHA = this->alpha * 0.8f;

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    if (this->block.GetTurretAIState() == MineTurretBlock::IdleTurretState) {
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
    if (this->block.GetTurretAIState() == MineTurretBlock::IdleTurretState) {
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

void MineTurretBlockMesh::BlockData::BlockStateChanged(const MineTurretBlock::TurretAIState& oldState,
                                                       const MineTurretBlock::TurretAIState& newState) {

    switch (newState) {

        case MineTurretBlock::IdleTurretState: {
            this->emoteLabel->SetText("");
            this->emoteScaleAnim.ClearLerp();
            this->emoteScaleAnim.SetInterpolantValue(0.0f);
            this->emoteScaleAnim.SetRepeat(false);
            break;
        }

        case MineTurretBlock::SeekingTurretState: {
            break;
        }

        case MineTurretBlock::TargetFoundTurretState: {
            if (oldState != MineTurretBlock::TargetFoundTurretState) {
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

        case MineTurretBlock::TargetLostTurretState: {
            if (oldState != MineTurretBlock::TargetLostTurretState) {
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

void MineTurretBlockMesh::BlockData::MineShotByBlock() {

}

void MineTurretBlockMesh::BlockData::SetAlpha(float alpha) {
    this->alpha = alpha;
}

void MineTurretBlockMesh::BlockData::InitTimeValueEmoteAnimVectors(std::vector<double>& times,
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