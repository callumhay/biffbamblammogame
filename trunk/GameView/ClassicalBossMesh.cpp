/**
 * ClassicalBossMesh.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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

#include "ClassicalBossMesh.h"
#include "GameViewConstants.h"
#include "GameAssets.h"
#include "GameLightAssets.h"
#include "CgFxBossWeakpoint.h"

#include "../ResourceManager.h"
#include "../BlammoEngine/Mesh.h"
#include "../ESPEngine/ESP.h"
#include "../GameSound/GameSound.h"
#include "../GameModel/ClassicalBoss.h"
#include "../GameModel/BossWeakpoint.h"

const double ClassicalBossMesh::INTRO_TIME_IN_SECS = 3.0;
const float ClassicalBossMesh::INTRO_SPARKLE_TIME_IN_SECS = 1.2;

ClassicalBossMesh::ClassicalBossMesh(ClassicalBoss* boss, GameSound* sound) : BossMesh(sound), boss(boss),
sparkleTex(NULL), glowTex(NULL), eyeMesh(NULL), pedimentMesh(NULL), tablatureMesh(NULL), columnMesh(NULL),
baseMesh(NULL), armSquareMesh(NULL), restOfArmMesh(NULL), leftArmFireEmitter(NULL),
rightArmFireEmitter(NULL), leftArmSmokeEmitter(NULL), rightArmSmokeEmitter(NULL), 
leftArmExplodingEmitter(NULL), rightArmExplodingEmitter(NULL), pedimentExplodingEmitter(NULL),
eyeExplodingEmitter(NULL), eyeSmokeEmitter(NULL), eyeFireEmitter(NULL),
particleGrowToSize(0.001f, 1.0f), particleFader(1.0f, 0.0f),
particleTwirl(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 1, ESPParticleRotateEffector::CLOCKWISE),
eyeGlowPulser(ScaleEffect(1.0f, 1.5f)), sparkleSoundID(INVALID_SOUND_ID+1), glowSoundID(INVALID_SOUND_ID+1) {

    assert(boss != NULL);

    // Load texture assets...
    this->sparkleTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_SPARKLE, Texture::Trilinear));
	assert(this->sparkleTex != NULL);
    this->glowTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_CLEAN_CIRCLE_GRADIENT, Texture::Trilinear));
	assert(this->glowTex != NULL);

    // Load the mesh assets...
    this->eyeMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->CLASSICAL_BOSS_EYE_MESH);
    assert(this->eyeMesh != NULL);
    this->pedimentMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->CLASSICAL_BOSS_PEDIMENT_MESH);
    assert(this->pedimentMesh != NULL);
    this->tablatureMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->CLASSICAL_BOSS_TABLATURE_MESH);
    assert(this->tablatureMesh != NULL);
    this->columnMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->CLASSICAL_BOSS_BODY_COLUMN_MESH);
    assert(this->columnMesh != NULL);
    this->baseMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->CLASSICAL_BOSS_BASE_MESH);
    assert(this->baseMesh != NULL);
    this->armSquareMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->CLASSICAL_BOSS_ARM_SQUARE_MESH);
    assert(this->armSquareMesh != NULL);
    this->restOfArmMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->CLASSICAL_BOSS_ARM_MESH);
    assert(this->restOfArmMesh != NULL);

    // Load effects assets...
    this->leftArmFireEmitter   = this->BuildFireEmitter(ClassicalBoss::ARM_WIDTH, ClassicalBoss::ARM_HEIGHT);
    this->leftArmSmokeEmitter  = this->BuildSmokeEmitter(ClassicalBoss::ARM_WIDTH, ClassicalBoss::ARM_HEIGHT);
    this->rightArmFireEmitter  = this->BuildFireEmitter(ClassicalBoss::ARM_WIDTH, ClassicalBoss::ARM_HEIGHT);
    this->rightArmSmokeEmitter = this->BuildSmokeEmitter(ClassicalBoss::ARM_WIDTH, ClassicalBoss::ARM_HEIGHT);

    this->eyeSmokeEmitter = this->BuildSmokeEmitter(ClassicalBoss::EYE_WIDTH, ClassicalBoss::EYE_HEIGHT);
    this->eyeFireEmitter  = this->BuildFireEmitter(ClassicalBoss::EYE_WIDTH, ClassicalBoss::EYE_HEIGHT);

    this->leftArmExplodingEmitter  = this->BuildExplodingEmitter(0.8f, this->boss->GetLeftRestOfArm(), ClassicalBoss::ARM_WIDTH, ClassicalBoss::ARM_HEIGHT);
    this->rightArmExplodingEmitter = this->BuildExplodingEmitter(0.8f, this->boss->GetRightRestOfArm(), ClassicalBoss::ARM_WIDTH, ClassicalBoss::ARM_HEIGHT);

    std::vector<const BossBodyPart*> columns = this->boss->GetBodyColumns();
    this->columnExplodingEmitters.reserve(6);
    for (int i = 0; i < 6; i++) {
        this->columnExplodingEmitters.push_back(this->BuildExplodingEmitter(0.5f, columns[i], ClassicalBoss::COLUMN_WIDTH, ClassicalBoss::COLUMN_HEIGHT));
    }

    static const float TABLATURE_SOUND_VOL_AMT = 0.75f;
    this->tablatureExplodingEmitters.reserve(4);
    // top-left
    this->tablatureExplodingEmitters.push_back(this->BuildExplodingEmitter(TABLATURE_SOUND_VOL_AMT, this->boss->GetTopLeftTablature(), 
        ClassicalBoss::TABLATURE_WIDTH, ClassicalBoss::TABLATURE_HEIGHT));
    // top-right
    this->tablatureExplodingEmitters.push_back(this->BuildExplodingEmitter(TABLATURE_SOUND_VOL_AMT, this->boss->GetTopRightTablature(),
        ClassicalBoss::TABLATURE_WIDTH, ClassicalBoss::TABLATURE_HEIGHT));
    // bottom-left
    this->tablatureExplodingEmitters.push_back(this->BuildExplodingEmitter(TABLATURE_SOUND_VOL_AMT, this->boss->GetBottomLeftTablature(),
        ClassicalBoss::TABLATURE_WIDTH, ClassicalBoss::TABLATURE_HEIGHT));
    // bottom-right
    this->tablatureExplodingEmitters.push_back(this->BuildExplodingEmitter(TABLATURE_SOUND_VOL_AMT, this->boss->GetBottomRightTablature(),
        ClassicalBoss::TABLATURE_WIDTH, ClassicalBoss::TABLATURE_HEIGHT));
    
    this->baseExplodingEmitter     = this->BuildExplodingEmitter(0.75f, this->boss->GetBase(), ClassicalBoss::BASE_WIDTH, ClassicalBoss::BASE_HEIGHT);
    this->pedimentExplodingEmitter = this->BuildExplodingEmitter(0.75f, this->boss->GetPediment(), ClassicalBoss::PEDIMENT_WIDTH, ClassicalBoss::PEDIMENT_HEIGHT);
    this->eyeExplodingEmitter      = this->BuildExplodingEmitter(1.0f,  this->boss->GetEye(), 1.5f * ClassicalBoss::EYE_WIDTH, 1.5f * ClassicalBoss::EYE_HEIGHT);

    const Texture2D* bossTexture = static_cast<const Texture2D*>(
        this->eyeMesh->GetMaterialGroups().begin()->second->GetMaterial()->GetProperties()->diffuseTexture);
    assert(bossTexture != NULL);
    this->weakpointMaterial->SetTexture(bossTexture);
}

ClassicalBossMesh::~ClassicalBossMesh() {

    // The boss pointer doesn't belong to this, just referenced
    this->boss = NULL;

    // No more texture for the weak point material
    this->weakpointMaterial->SetTexture(NULL);

    // Release the mesh assets...
    bool success = false;
    
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->sparkleTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->glowTex);
    assert(success);

    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->eyeMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->pedimentMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->tablatureMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->columnMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->baseMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->armSquareMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->restOfArmMesh);
    assert(success);

    UNUSED_VARIABLE(success);

    delete this->leftArmFireEmitter;
    this->leftArmFireEmitter = NULL;
    delete this->leftArmSmokeEmitter;
    this->leftArmSmokeEmitter = NULL;
    delete this->rightArmFireEmitter;
    this->rightArmFireEmitter = NULL;
    delete this->rightArmSmokeEmitter;
    this->rightArmSmokeEmitter = NULL;

    delete this->eyeFireEmitter;
    this->eyeFireEmitter = NULL;
    delete this->eyeSmokeEmitter;
    this->eyeSmokeEmitter = NULL;

    delete this->leftArmExplodingEmitter;
    this->leftArmExplodingEmitter = NULL;
    delete this->rightArmExplodingEmitter;
    this->rightArmExplodingEmitter = NULL;

    for (int i = 0; i < static_cast<int>(this->columnExplodingEmitters.size()); i++) {
        delete this->columnExplodingEmitters[i];
    }
    this->columnExplodingEmitters.clear();

    for (int i = 0; i < static_cast<int>(this->tablatureExplodingEmitters.size()); i++) {
        delete this->tablatureExplodingEmitters[i];
    }
    this->tablatureExplodingEmitters.clear();
    
    delete this->baseExplodingEmitter;
    this->baseExplodingEmitter = NULL;
    delete this->pedimentExplodingEmitter;
    this->pedimentExplodingEmitter = NULL;
    delete this->eyeExplodingEmitter;
    this->eyeExplodingEmitter = NULL;
}

double ClassicalBossMesh::ActivateIntroAnimation() {
    
    Point3D eyePos = this->boss->GetEye()->GetTranslationPt3D();

	this->introSparkle.SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	this->introSparkle.SetParticleLife(ESPInterval(INTRO_SPARKLE_TIME_IN_SECS));
    this->introSparkle.SetParticleSize(ESPInterval(4*ClassicalBoss::EYE_HEIGHT));
	this->introSparkle.SetEmitPosition(eyePos);
	this->introSparkle.SetParticleColour(ESPInterval(1), ESPInterval(1), ESPInterval(1), ESPInterval(1));
	this->introSparkle.AddEffector(&this->particleGrowToSize);
    this->introSparkle.AddEffector(&this->particleTwirl);
    this->introSparkle.AddEffector(&this->particleFader);
	this->introSparkle.SetParticles(1, this->sparkleTex);

    // Glowing in the eye
    this->eyePulseGlow.SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	this->eyePulseGlow.SetParticleLife(ESPInterval(ESPParticle::INFINITE_PARTICLE_LIFETIME));
	this->eyePulseGlow.SetParticleSize(ESPInterval(0.75f * ClassicalBoss::EYE_HEIGHT));
    this->eyePulseGlow.SetParticleAlignment(ESP::ScreenAligned);
    this->eyePulseGlow.SetEmitPosition(Point3D(0, 0, ClassicalBoss::EYE_DEPTH));
	this->eyePulseGlow.SetParticleColour(ESPInterval(1), ESPInterval(0), ESPInterval(0), ESPInterval(0.0f));
	this->eyePulseGlow.AddEffector(&this->eyeGlowPulser);
	this->eyePulseGlow.SetParticles(1, this->glowTex);

    //GameLightAssets::GetBlackoutToLightsOnPieceAffectingLights(this->eyeIntroKeyLight, this->eyeIntroFillLight,
    //    INTRO_TIME_IN_SECS - SPARKLE_TIME_IN_SECS);

    this->sparkleSoundID = INVALID_SOUND_ID;
    this->glowSoundID    = INVALID_SOUND_ID;

    this->introTimeCountdown = INTRO_TIME_IN_SECS;
    return this->introTimeCountdown;
}

void ClassicalBossMesh::DrawBody(double dT, const Camera& camera, const BasicPointLight& keyLight,
                                 const BasicPointLight& fillLight, const BasicPointLight& ballLight,
                                 const GameAssets* assets) {
    
    UNUSED_PARAMETER(dT);
    UNUSED_PARAMETER(assets);

    this->weakpointMaterial->SetLightAmt(keyLight, fillLight);

    // Using data from the GameModel's boss object, we draw the various pieces of the boss in their correct
    // world space locations...

    const ColourRGBA* currColour;

    // Arms
    const BossBodyPart* leftRestOfArm  = this->boss->GetLeftRestOfArm();
    const BossBodyPart* rightRestOfArm = this->boss->GetRightRestOfArm();
    const BossBodyPart* leftArmSquare  = this->boss->GetLeftArmSquare();
    const BossBodyPart* rightArmSquare = this->boss->GetRightArmSquare();
    assert(leftRestOfArm != NULL);
    assert(rightRestOfArm != NULL);
    assert(leftArmSquare != NULL);
    assert(rightArmSquare != NULL);

    currColour = &leftRestOfArm->GetColour();
    if (currColour->A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(leftRestOfArm->GetWorldTransform().begin());
        glColor4f(currColour->R(), currColour->G(), currColour->B(), currColour->A());
        this->restOfArmMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    currColour = &rightRestOfArm->GetColour();
    if (currColour->A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(rightRestOfArm->GetWorldTransform().begin());
        glColor4f(currColour->R(), currColour->G(), currColour->B(), currColour->A());
        this->restOfArmMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    currColour = &leftArmSquare->GetColour();
    if (currColour->A() > 0.0f) {

        glPushMatrix();
        glMultMatrixf(leftArmSquare->GetWorldTransform().begin());
        glColor4f(currColour->R(), currColour->G(), currColour->B(), currColour->A());
        if (leftArmSquare->GetType() == AbstractBossBodyPart::WeakpointBodyPart) {
            this->armSquareMesh->Draw(camera, this->weakpointMaterial);
        }
        else {
            this->armSquareMesh->Draw(camera, keyLight, fillLight, ballLight);
        }
        glPopMatrix();
    }

    currColour = &rightArmSquare->GetColour();
    if (currColour->A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(rightArmSquare->GetWorldTransform().begin());
        glColor4f(currColour->R(), currColour->G(), currColour->B(), currColour->A());
        if (rightArmSquare->GetType() == AbstractBossBodyPart::WeakpointBodyPart) {
            this->armSquareMesh->Draw(camera, this->weakpointMaterial);
        }
        else {
            this->armSquareMesh->Draw(camera, keyLight, fillLight, ballLight);
        }
        glPopMatrix();
    }

    // Eye...
    const BossBodyPart* eye = this->boss->GetEye();
    assert(eye != NULL);

    currColour = &eye->GetColour();
    if (currColour->A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(eye->GetWorldTransform().begin());
        glColor4f(currColour->R(), currColour->G(), currColour->B(), currColour->A());
        if (eye->GetType() == AbstractBossBodyPart::WeakpointBodyPart) {
            this->eyeMesh->Draw(camera, this->weakpointMaterial);
        }
        else {
            this->eyeMesh->Draw(camera, keyLight, fillLight, ballLight);
        }
        glPopMatrix();
    }

    // Pediment...
    const BossBodyPart* pediment = this->boss->GetPediment();
    assert(pediment != NULL);

    currColour = &pediment->GetColour();
    if (currColour->A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(pediment->GetWorldTransform().begin());
        glColor4f(currColour->R(), currColour->G(), currColour->B(), currColour->A());
        this->pedimentMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    // Tablatures
    const BossBodyPart* topLeftTab  = this->boss->GetTopLeftTablature();
    const BossBodyPart* topRightTab = this->boss->GetTopRightTablature();
    const BossBodyPart* bottomLeftTab  = this->boss->GetBottomLeftTablature();
    const BossBodyPart* bottomRightTab = this->boss->GetBottomRightTablature();
    assert(topLeftTab != NULL);
    assert(topRightTab != NULL);
    assert(bottomLeftTab != NULL);
    assert(bottomRightTab != NULL);

    // All tablatures have the same colour...
    currColour = &topLeftTab->GetColour();
    if (currColour->A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(topLeftTab->GetWorldTransform().begin());
        glColor4f(currColour->R(), currColour->G(), currColour->B(), currColour->A());
        this->tablatureMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    currColour = &topRightTab->GetColour();
    if (currColour->A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(topRightTab->GetWorldTransform().begin());
        glColor4f(currColour->R(), currColour->G(), currColour->B(), currColour->A());
        this->tablatureMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    currColour = &bottomLeftTab->GetColour();
    if (currColour->A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(bottomLeftTab->GetWorldTransform().begin());
        glColor4f(currColour->R(), currColour->G(), currColour->B(), currColour->A());
        this->tablatureMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    currColour = &bottomRightTab->GetColour();
    if (currColour->A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(bottomRightTab->GetWorldTransform().begin());
        glColor4f(currColour->R(), currColour->G(), currColour->B(), currColour->A());
        this->tablatureMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    // Columns
    std::vector<const BossBodyPart*> columns = this->boss->GetBodyColumns();   
    for (int i = 0; i < static_cast<int>(columns.size()); i++) {
        const BossBodyPart* column = columns[i];
        assert(column != NULL);

        currColour = &column->GetColour();
        if (currColour->A() > 0.0f) {
            glPushMatrix();
            glMultMatrixf(column->GetWorldTransform().begin());
            glColor4f(currColour->R(), currColour->G(), currColour->B(), currColour->A());
            if (column->GetType() == AbstractBossBodyPart::WeakpointBodyPart) {
                this->columnMesh->Draw(camera, this->weakpointMaterial);
            }
            else {
                this->columnMesh->Draw(camera, keyLight, fillLight, ballLight);
            }
            glPopMatrix();
        }
    }

    // Base
    const BossBodyPart* base = this->boss->GetBase();
    assert(base != NULL);

    currColour = &base->GetColour();
    if (currColour->A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(base->GetWorldTransform().begin());
        glColor4f(currColour->R(), currColour->G(), currColour->B(), currColour->A());
        this->baseMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }
}

void ClassicalBossMesh::DrawPostBodyEffects(double dT, const Camera& camera, const GameAssets* assets) {
    BossMesh::DrawPostBodyEffects(dT, camera, assets);

    const BossBodyPart* eye = this->boss->GetEye();

    // Check to see if we're drawing intro effects
    if (this->introTimeCountdown > 0.0) {
        static const double START_GLOW_TIME = INTRO_TIME_IN_SECS - INTRO_SPARKLE_TIME_IN_SECS/1.5f;
        
        if (this->sparkleSoundID == INVALID_SOUND_ID) {
            this->sparkleSoundID = assets->GetSound()->PlaySound(GameSound::ClassicalBossSparkleEvent, false, false);
        }

        this->introSparkle.Tick(dT);
        this->introSparkle.Draw(camera);

        if (this->introTimeCountdown < START_GLOW_TIME) {
            if (this->introTimeCountdown < INTRO_TIME_IN_SECS - INTRO_SPARKLE_TIME_IN_SECS &&
                this->glowSoundID == INVALID_SOUND_ID) {
                this->glowSoundID = assets->GetSound()->PlaySound(GameSound::BossGlowEvent, false, false);
            }
            glPushMatrix();
            glMultMatrixf(eye->GetWorldTransform().begin());
            this->eyePulseGlow.SetParticleAlpha(
                ESPInterval(0.7f * std::max<float>(0.0f, std::min<float>(1.0f, 
                1.0f - this->introTimeCountdown / START_GLOW_TIME))));

            this->eyePulseGlow.Tick(dT);
            this->eyePulseGlow.Draw(camera);

            glPopMatrix();
        }

        this->introTimeCountdown -= dT;
        return;
    }

    // Arm effects...
    const BossBodyPart* leftRestOfArm  = this->boss->GetLeftRestOfArm();
    const BossBodyPart* rightRestOfArm = this->boss->GetRightRestOfArm();
    const BossBodyPart* leftArmSquare  = this->boss->GetLeftArmSquare();
    const BossBodyPart* rightArmSquare = this->boss->GetRightArmSquare();

    if (leftArmSquare->GetAlpha() > 0.0f) {
        if (leftArmSquare->GetType() == AbstractBossBodyPart::WeakpointBodyPart) {
            const BossWeakpoint* leftArmSqr = static_cast<const BossWeakpoint*>(leftArmSquare);
            
            if (!leftArmSqr->GetIsDestroyed()) {
                if (leftArmSqr->GetCurrentLifePercentage() < 1.0) {
                    float lifePercentage = leftArmSqr->GetCurrentLifePercentage();
                    this->leftArmFireEmitter->SetSpawnDelta(ESPInterval(lifePercentage*0.4f, lifePercentage*0.9f));
                    this->leftArmFireEmitter->Tick(dT);
                    this->leftArmSmokeEmitter->SetSpawnDelta(ESPInterval(lifePercentage*0.4f, lifePercentage*0.9f));
                    this->leftArmSmokeEmitter->Tick(dT);

                    glPushMatrix();
                    glMultMatrixf(leftRestOfArm->GetWorldTransform().begin());
                    this->leftArmSmokeEmitter->Draw(camera);
                    this->leftArmFireEmitter->Draw(camera);
                    glPopMatrix();
                }
            }
            else {
                glPushMatrix();
                glMultMatrixf(leftRestOfArm->GetWorldTransform().begin());
                this->leftArmExplodingEmitter->Tick(dT);
                this->leftArmExplodingEmitter->Draw(camera);
                glPopMatrix();
            }
        }
    }

    if (rightArmSquare->GetAlpha() > 0.0f) {
        if (rightArmSquare->GetType() == AbstractBossBodyPart::WeakpointBodyPart) {
            const BossWeakpoint* rightArmSqr = static_cast<const BossWeakpoint*>(rightArmSquare);
            if (!rightArmSqr->GetIsDestroyed()) {
                if (rightArmSqr->GetCurrentLifePercentage() < 1.0) {
                    float lifePercentage = rightArmSqr->GetCurrentLifePercentage();
                    
                    this->rightArmFireEmitter->SetSpawnDelta(ESPInterval(lifePercentage*0.4f, lifePercentage*0.9f));
                    this->rightArmFireEmitter->Tick(dT);
                    this->rightArmSmokeEmitter->SetSpawnDelta(ESPInterval(lifePercentage*0.4f, lifePercentage*0.9f));
                    this->rightArmSmokeEmitter->Tick(dT);

                    glPushMatrix();
                    glMultMatrixf(rightRestOfArm->GetWorldTransform().begin());
                    this->rightArmSmokeEmitter->Draw(camera);
                    this->rightArmFireEmitter->Draw(camera);
                    glPopMatrix();
                }
            }
            else {
                glPushMatrix();
                glMultMatrixf(rightRestOfArm->GetWorldTransform().begin());
                this->rightArmExplodingEmitter->Tick(dT);
                this->rightArmExplodingEmitter->Draw(camera);
                glPopMatrix();
            }
        }
    }

    // Column effects
    std::vector<const BossBodyPart*> columns = this->boss->GetBodyColumns();   
    for (int i = 0; i < static_cast<int>(columns.size()); i++) {
        const BossBodyPart* column = columns[i];

        if (column->GetAlpha() > 0.0f && column->GetIsDestroyed()) {
            glPushMatrix();
            glMultMatrixf(column->GetWorldTransform().begin());
            this->columnExplodingEmitters[i]->Tick(dT);
            this->columnExplodingEmitters[i]->Draw(camera);
            glPopMatrix();
        }
    }

    const BossBodyPart* topLeftTab     = this->boss->GetTopLeftTablature();
    const BossBodyPart* topRightTab    = this->boss->GetTopRightTablature();
    const BossBodyPart* bottomLeftTab  = this->boss->GetBottomLeftTablature();
    const BossBodyPart* bottomRightTab = this->boss->GetBottomRightTablature();
    
    if (topLeftTab->GetAlpha() > 0.0f && topLeftTab->GetIsDestroyed()) {
        glPushMatrix();
        glMultMatrixf(topLeftTab->GetWorldTransform().begin());
        this->tablatureExplodingEmitters[0]->Tick(dT);
        this->tablatureExplodingEmitters[0]->Draw(camera);
        glPopMatrix();
    }
    if (topRightTab->GetAlpha() > 0.0f && topRightTab->GetIsDestroyed()) {
        glPushMatrix();
        glMultMatrixf(topRightTab->GetWorldTransform().begin());
        this->tablatureExplodingEmitters[1]->Tick(dT);
        this->tablatureExplodingEmitters[1]->Draw(camera);
        glPopMatrix();
    }
    if (bottomLeftTab->GetAlpha() > 0.0f && bottomLeftTab->GetIsDestroyed()) {
        glPushMatrix();
        glMultMatrixf(bottomLeftTab->GetWorldTransform().begin());
        this->tablatureExplodingEmitters[2]->Tick(dT);
        this->tablatureExplodingEmitters[2]->Draw(camera);
        glPopMatrix();
    }
    if (bottomRightTab->GetAlpha() > 0.0f && bottomRightTab->GetIsDestroyed()) {
        glPushMatrix();
        glMultMatrixf(bottomRightTab->GetWorldTransform().begin());
        this->tablatureExplodingEmitters[3]->Tick(dT);
        this->tablatureExplodingEmitters[3]->Draw(camera);
        glPopMatrix();
    }
    
    // Pediment effects
    const BossBodyPart* pediment = this->boss->GetPediment();
    if (pediment->GetAlpha() > 0.0f && pediment->GetIsDestroyed()) {
        glPushMatrix();
        glMultMatrixf(pediment->GetWorldTransform().begin());
        this->pedimentExplodingEmitter->Tick(dT);
        this->pedimentExplodingEmitter->Draw(camera);
        glPopMatrix();
    }

    // Eye effects
    if (eye->GetAlpha() > 0.0f) {
        
        // Draw the eye's glowing effect
        glPushMatrix();
        glMultMatrixf(eye->GetWorldTransform().begin());
        this->eyePulseGlow.SetParticleAlpha(0.7f*eye->GetAlpha());
        this->eyePulseGlow.Tick(dT);
        this->eyePulseGlow.Draw(camera);
        glPopMatrix();
        
        if (eye->GetType() == AbstractBossBodyPart::WeakpointBodyPart) {
            if (!eye->GetIsDestroyed()) {
                const BossWeakpoint* eyeWeakpt = static_cast<const BossWeakpoint*>(eye);
                float eyeLifePercentage = eyeWeakpt->GetCurrentLifePercentage();
                if (eyeLifePercentage < 1.0) {
                    
                    glPushMatrix();
                    glMultMatrixf(eye->GetWorldTransform().begin());

                    // Draw smoke
                    this->eyeSmokeEmitter->Tick(dT);
                    this->eyeSmokeEmitter->Draw(camera);

                    if (eyeLifePercentage <= 0.5) {
                        // Draw fire as well
                        this->eyeFireEmitter->Tick(dT);
                        this->eyeFireEmitter->Draw(camera);
                    }

                    glPopMatrix();
                }
            }
            else {
                glPushMatrix();
                glMultMatrixf(eye->GetWorldTransform().begin());
                this->eyeExplodingEmitter->SetParticleAlpha(eye->GetAlpha());
                this->eyeExplodingEmitter->Tick(dT);
                this->eyeExplodingEmitter->Draw(camera);
                glPopMatrix();
            }
        }
    }
}

Point3D ClassicalBossMesh::GetBossFinalExplodingEpicenter() const {
    // This bosses' final body part is its eye, so we return the very front-center of the eye
    Point3D eyePos(this->boss->GetEye()->GetTranslationPt2D(), ClassicalBoss::EYE_DEPTH);
    return eyePos;
}