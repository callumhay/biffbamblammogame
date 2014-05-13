/**
 * FuturismBossMesh.cpp
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

#include "FuturismBossMesh.h"
#include "GameViewConstants.h"
#include "GameAssets.h"
#include "GameESPAssets.h"
#include "GameFBOAssets.h"
#include "InGameBossLevelDisplayState.h"
#include "CgFxBossWeakpoint.h"
#include "CgFxPrism.h"
#include "PersistentTextureManager.h"

#include "../BlammoEngine/Mesh.h"
#include "../GameModel/FuturismBoss.h"

const double FuturismBossMesh::INTRO_TIME_IN_SECS = 4.0;

FuturismBossMesh::FuturismBossMesh(FuturismBoss* boss, GameSound* sound) : 
BossMesh(sound), boss(boss), coreCenterMesh(NULL), coreArmsMesh(NULL), 
coreBulbMesh(NULL), coreShieldMesh(NULL), 
leftRightShieldMesh(NULL), topBottomShieldMesh(NULL), 
damagedLeftRightShieldMesh(NULL), damagedTopBottomShieldMesh(NULL), 
damagedCoreShieldMesh(NULL), iceEncasingMesh(NULL),
topShieldExplodingEmitter(NULL), bottomShieldExplodingEmitter(NULL),
leftShieldExplodingEmitter(NULL), rightShieldExplodingEmitter(NULL),
coreExplodingEmitter(NULL), glowSoundID(INVALID_SOUND_ID)
{
    assert(boss != NULL);

    // Load the meshes...
    this->coreCenterMesh = ResourceManager::GetInstance()->GetObjMeshResource(
        GameViewConstants::GetInstance()->FUTURISM_BOSS_CORE_CENTER_MESH);
    assert(this->coreCenterMesh != NULL);
    this->coreArmsMesh = ResourceManager::GetInstance()->GetObjMeshResource(
        GameViewConstants::GetInstance()->FUTURISM_BOSS_CORE_ARMS_MESH);
    assert(this->coreArmsMesh != NULL);
    this->coreBulbMesh = ResourceManager::GetInstance()->GetObjMeshResource(
        GameViewConstants::GetInstance()->FUTURISM_BOSS_CORE_BULB_MESH);
    assert(this->coreBulbMesh != NULL);
    this->coreShieldMesh = ResourceManager::GetInstance()->GetObjMeshResource(
        GameViewConstants::GetInstance()->FUTURISM_BOSS_CORE_SHIELD_MESH);
    assert(this->coreShieldMesh != NULL);
    this->leftRightShieldMesh = ResourceManager::GetInstance()->GetObjMeshResource(
        GameViewConstants::GetInstance()->FUTURISM_BOSS_LEFT_AND_RIGHT_SHIELD_MESH);
    assert(this->leftRightShieldMesh != NULL);
    this->topBottomShieldMesh = ResourceManager::GetInstance()->GetObjMeshResource(
        GameViewConstants::GetInstance()->FUTURISM_BOSS_TOP_AND_BOTTOM_SHIELD_MESH);
    assert(this->topBottomShieldMesh != NULL);

    this->damagedLeftRightShieldMesh = ResourceManager::GetInstance()->GetObjMeshResource(
        GameViewConstants::GetInstance()->FUTURISM_BOSS_DAMAGED_LEFT_AND_RIGHT_SHIELD_MESH);
    assert(this->damagedLeftRightShieldMesh != NULL);
    this->damagedTopBottomShieldMesh = ResourceManager::GetInstance()->GetObjMeshResource(
        GameViewConstants::GetInstance()->FUTURISM_BOSS_DAMAGED_TOP_AND_BOTTOM_SHIELD_MESH);
    assert(this->damagedTopBottomShieldMesh != NULL);
    this->damagedCoreShieldMesh = ResourceManager::GetInstance()->GetObjMeshResource(
        GameViewConstants::GetInstance()->FUTURISM_BOSS_DAMAGED_CORE_SHIELD_MESH);
    assert(this->damagedCoreShieldMesh != NULL);

    this->iceEncasingMesh = ResourceManager::GetInstance()->GetObjMeshResource(
        GameViewConstants::GetInstance()->FUTURISM_BOSS_ICE_ENCASING);
    assert(this->iceEncasingMesh != NULL);

    PersistentTextureManager::GetInstance()->PreloadTexture2D(
        GameViewConstants::GetInstance()->TEXTURE_CLEAN_CIRCLE_GRADIENT);

    this->topShieldExplodingEmitter    = this->BuildExplodingEmitter(0.9f, this->boss->GetTopShield(), 
        FuturismBoss::TOP_BOTTOM_SHIELD_EXPLODE_WIDTH, FuturismBoss::TOP_BOTTOM_SHIELD_EXPLODE_HEIGHT, 2.0f);
    this->bottomShieldExplodingEmitter = this->BuildExplodingEmitter(0.9f, this->boss->GetBottomShield(),
        FuturismBoss::TOP_BOTTOM_SHIELD_EXPLODE_WIDTH, FuturismBoss::TOP_BOTTOM_SHIELD_EXPLODE_HEIGHT, 2.0f);
    this->leftShieldExplodingEmitter   = this->BuildExplodingEmitter(0.9f, this->boss->GetLeftShield(),
        FuturismBoss::LEFT_RIGHT_SHIELD_EXPLODE_WIDTH, FuturismBoss::LEFT_RIGHT_SHIELD_EXPLODE_HEIGHT, 2.0f);
    this->rightShieldExplodingEmitter  = this->BuildExplodingEmitter(0.9f, this->boss->GetRightShield(),
        FuturismBoss::LEFT_RIGHT_SHIELD_EXPLODE_WIDTH, FuturismBoss::LEFT_RIGHT_SHIELD_EXPLODE_HEIGHT, 2.0f);
    this->coreShieldExplodingEmitter   = this->BuildExplodingEmitter(0.9f, this->boss->GetCoreShield(),
        FuturismBoss::CORE_SHIELD_SIZE, FuturismBoss::CORE_SHIELD_SIZE, 2.0f);
    this->coreExplodingEmitter = this->BuildExplodingEmitter(0.9f, this->boss->GetCoreBody(),
        FuturismBoss::CORE_BOSS_SIZE, FuturismBoss::CORE_BOSS_SIZE, 1.45f);

    BossMesh::BuildShieldingColourAnimation(this->shieldColourAnim);
    {
        std::vector<double> timeValues;
        timeValues.reserve(4);
        timeValues.push_back(0.0);
        timeValues.push_back(0.65);
        timeValues.push_back(0.67);
        timeValues.push_back(1.32);

        std::vector<Colour> colourValues;
        colourValues.reserve(4);
        colourValues.push_back(Colour(1.0f, 0.0f, 0.0f));
        colourValues.push_back(Colour(1.0f, 1.0f, 1.0f));
        colourValues.push_back(Colour(1.0f, 1.0f, 1.0f));
        colourValues.push_back(Colour(1.0f, 0.0f, 0.0f));

        this->bulbFlashingAnim.SetLerp(timeValues, colourValues);
        this->bulbFlashingAnim.SetRepeat(true);
    }
    {
        std::vector<Texture2D*> cloudTextures;
        GameESPAssets::GetCloudTextures(cloudTextures);

        ESPMultiAlphaEffector mistAlpha;
        std::vector<std::pair<float, double> > alphaAndPercentages;
        alphaAndPercentages.reserve(4);
        alphaAndPercentages.push_back(std::make_pair(0.0f, 0.0));
        alphaAndPercentages.push_back(std::make_pair(0.25f, 0.25));
        alphaAndPercentages.push_back(std::make_pair(0.25f, 0.75));
        alphaAndPercentages.push_back(std::make_pair(0.0f, 1.0));
        mistAlpha.SetAlphasWithPercentage(alphaAndPercentages);

        this->frostMistShader.SetTechnique(CgFxVolumetricEffect::SMOKESPRITE_TECHNIQUE_NAME);
        this->frostMistShader.SetColour(Colour(1.0f, 1.0f, 1.0f));
        this->frostMistShader.SetScale(0.5f);
        this->frostMistShader.SetFrequency(0.8f);
        this->frostMistShader.SetFlowDirection(Vector3D(0, 0, 1));
        this->frostMistShader.SetTexture(cloudTextures[0]);

        frostMist.SetNumParticleLives(ESPParticle::INFINITE_PARTICLE_LIVES);
        frostMist.SetSpawnDelta(ESPInterval(0.15f, 0.25f));
        frostMist.SetInitialSpd(ESPInterval(0.25f, 1.0f));
        frostMist.SetParticleLife(ESPInterval(1.5f, 2.5f));
        frostMist.SetParticleSize(ESPInterval(FuturismBoss::CORE_BOSS_SIZE / 4.0f, FuturismBoss::CORE_BOSS_SIZE / 1.15f));
        frostMist.SetEmitAngleInDegrees(180);
        frostMist.SetRadiusDeviationFromCenter(ESPInterval(0, 0.6f*FuturismBoss::CORE_BOSS_HALF_SIZE), 
            ESPInterval(0, 0.6f*FuturismBoss::CORE_BOSS_HALF_SIZE), ESPInterval(0, FuturismBoss::CORE_Z_SHOOT_DIST_FROM_ORIGIN_WITH_SHIELD));
        frostMist.SetParticleRotation(ESPInterval(0, 359.99f));
        frostMist.SetParticleColour(ESPInterval(0.85f, 1.0f),ESPInterval(1.0f),ESPInterval(1.0f),ESPInterval(0.8f,1));
        frostMist.AddCopiedEffector(ESPParticleScaleEffector(1.0f, 1.5f));
        frostMist.AddCopiedEffector(mistAlpha);
        frostMist.AddCopiedEffector(ESPParticleRotateEffector(80.0f, ESPParticleRotateEffector::CLOCKWISE));
        frostMist.SetRandomTextureEffectParticles(20, &this->frostMistShader, cloudTextures);
    }

    // Set the weak-point material's texture
    const Texture2D* bossTexture = static_cast<const Texture2D*>(
        this->coreCenterMesh->GetMaterialGroups().begin()->second->GetMaterial()->GetProperties()->diffuseTexture);
    assert(bossTexture != NULL);
    this->weakpointMaterial->SetTexture(bossTexture);
}

FuturismBossMesh::~FuturismBossMesh() {
    // The boss pointer doesn't belong to this, just referenced
    this->boss = NULL;

    bool success = false;

    // Release the mesh assets...
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->coreCenterMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->coreArmsMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->coreBulbMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->coreShieldMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->leftRightShieldMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->topBottomShieldMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->damagedLeftRightShieldMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->damagedTopBottomShieldMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->damagedCoreShieldMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->iceEncasingMesh);
    assert(success);

    UNUSED_VARIABLE(success);

    delete this->topShieldExplodingEmitter;
    this->topShieldExplodingEmitter = NULL;
    delete this->bottomShieldExplodingEmitter;
    this->bottomShieldExplodingEmitter = NULL;
    delete this->leftShieldExplodingEmitter;
    this->leftShieldExplodingEmitter = NULL;
    delete this->rightShieldExplodingEmitter;
    this->rightShieldExplodingEmitter = NULL;
    delete this->coreShieldExplodingEmitter;
    this->coreShieldExplodingEmitter = NULL;
    delete this->coreExplodingEmitter;
    this->coreExplodingEmitter = NULL;
}

double FuturismBossMesh::ActivateIntroAnimation() {

    // Glowing in the eye
    this->eyePulseGlow.SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
    this->eyePulseGlow.SetParticleLife(ESPInterval(ESPParticle::INFINITE_PARTICLE_LIFETIME));
    this->eyePulseGlow.SetParticleSize(ESPInterval(0.75f*FuturismBoss::CORE_EYE_SIZE));
    this->eyePulseGlow.SetParticleAlignment(ESP::ScreenAligned);
    this->eyePulseGlow.SetEmitPosition(Point3D(0, 0, FuturismBoss::CORE_Z_SHOOT_DIST_FROM_ORIGIN_WITH_SHIELD));
    this->eyePulseGlow.SetParticleColour(ESPInterval(1), ESPInterval(0), ESPInterval(0), ESPInterval(0.0f));
    this->eyePulseGlow.AddCopiedEffector(ESPParticleScaleEffector(ScaleEffect(1.0f, 1.5f)));
    this->eyePulseGlow.SetParticles(1, PersistentTextureManager::GetInstance()->GetLoadedTexture(
        GameViewConstants::GetInstance()->TEXTURE_CLEAN_CIRCLE_GRADIENT));
    
    this->introTimeCountdown = FuturismBossMesh::INTRO_TIME_IN_SECS;
    return this->introTimeCountdown;
}


void FuturismBossMesh::DrawBody(double dT, const Camera& camera, const BasicPointLight& keyLight,
                                const BasicPointLight& fillLight, const BasicPointLight& ballLight,
                                const GameAssets*) {

    // Using data from the GameModel's boss object, we draw the various pieces of the boss in their correct
    // world space locations...
    const ColourRGBA* currColour;

    // Draw the core parts of the body
    const BossBodyPart* core = this->boss->GetCoreBody();
    currColour = &core->GetColour();
    if (currColour->A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(core->GetWorldTransform().begin());
        glColor4f(currColour->R(), currColour->G(), currColour->B(), currColour->A());

        this->coreCenterMesh->Draw(camera, keyLight, fillLight, ballLight);
        
        glTranslatef(0,0,this->boss->GetCurrArmMoveForwardOffset());
        this->coreArmsMesh->Draw(camera, keyLight, fillLight, ballLight);
        
        glPopMatrix();
    }

    // Draw the bulbs (weak points in the final stage of the boss)
    this->bulbFlashingAnim.Tick(dT);
    const BossBodyPart* coreTopBulb = this->boss->GetCoreTopBulb();
    const BossBodyPart* coreBottomBulb = this->boss->GetCoreBottomBulb();
    const BossBodyPart* coreLeftBulb = this->boss->GetCoreLeftBulb();
    const BossBodyPart* coreRightBulb = this->boss->GetCoreRightBulb();
    this->DrawBulbBodyPart(camera, keyLight, fillLight, ballLight, coreTopBulb, this->coreBulbMesh);
    this->DrawBulbBodyPart(camera, keyLight, fillLight, ballLight, coreBottomBulb, this->coreBulbMesh);
    this->DrawBulbBodyPart(camera, keyLight, fillLight, ballLight, coreLeftBulb, this->coreBulbMesh);
    this->DrawBulbBodyPart(camera, keyLight, fillLight, ballLight, coreRightBulb, this->coreBulbMesh);

    // Draw the shield parts of the body
    this->shieldColourAnim.Tick(dT);
    const Colour& shieldColour = this->shieldColourAnim.GetInterpolantValue();

    const BossBodyPart* coreShield = this->boss->GetCoreShield();
    currColour = &coreShield->GetColour();
    if (currColour->A() > 0.0f) {

        glPushMatrix();
        glMultMatrixf(coreShield->GetWorldTransform().begin());
        if (this->boss->IsCoreShieldVulnerable()) {
            glColor4f(shieldColour.R()*currColour->R(), shieldColour.G()*currColour->G(), 
                shieldColour.B()*currColour->B(), currColour->A());
        }
        else {
            glColor4f(currColour->R(), currColour->G(), currColour->B(), currColour->A());
        }
        
        if (coreShield->GetType() == AbstractBossBodyPart::WeakpointBodyPart) {
            this->damagedCoreShieldMesh->Draw(camera, keyLight, fillLight, ballLight);
        }
        else {
            this->coreShieldMesh->Draw(camera, keyLight, fillLight, ballLight);
        }
        glPopMatrix();
    }

    const BossBodyPart* topShield    = this->boss->GetTopShield();
    const BossBodyPart* bottomShield = this->boss->GetBottomShield();
    this->DrawDamagableBodyPart(camera, keyLight, fillLight, ballLight, topShield, 
        this->topBottomShieldMesh, this->damagedTopBottomShieldMesh);
    this->DrawDamagableBodyPart(camera, keyLight, fillLight, ballLight, bottomShield, 
        this->topBottomShieldMesh, this->damagedTopBottomShieldMesh);

    const BossBodyPart* leftShield  = this->boss->GetLeftShield();
    const BossBodyPart* rightShield = this->boss->GetRightShield();
    this->DrawDamagableBodyPart(camera, keyLight, fillLight, ballLight, leftShield, 
        this->leftRightShieldMesh, this->damagedLeftRightShieldMesh);
    this->DrawDamagableBodyPart(camera, keyLight, fillLight, ballLight, rightShield, 
        this->leftRightShieldMesh, this->damagedLeftRightShieldMesh);
}

void FuturismBossMesh::DrawPostBodyEffects(double dT, const Camera& camera, const GameModel& gameModel,
                                           const BasicPointLight& keyLight, const BasicPointLight& fillLight, 
                                           const BasicPointLight& ballLight, const GameAssets* assets) {

    BossMesh::DrawPostBodyEffects(dT, camera, gameModel, keyLight, fillLight, ballLight, assets);

    static const float MAX_EYE_GLOW_ALPHA = 0.7f;

    // Check to see if we're drawing intro effects
    if (this->introTimeCountdown > 0.0) {
        static const double START_GLOW_TIME = INTRO_TIME_IN_SECS / 1.5;
        if (this->introTimeCountdown <= START_GLOW_TIME) {
            if (this->glowSoundID == INVALID_SOUND_ID) {
                this->glowSoundID = assets->GetSound()->PlaySound(GameSound::BossGlowEvent, false, false);
            }

            glPushMatrix();
            glMultMatrixf(this->boss->GetCoreBody()->GetWorldTransform().begin());
            
            this->eyePulseGlow.SetParticleAlpha(ESPInterval(MAX_EYE_GLOW_ALPHA*NumberFuncs::LerpOverTime(
                INTRO_TIME_IN_SECS, START_GLOW_TIME, 0.0f, 1.0f, this->introTimeCountdown)));
            this->eyePulseGlow.Tick(dT);
            this->eyePulseGlow.Draw(camera);

            glPopMatrix();
        }
        
        this->introTimeCountdown -= dT;
        return;
    }

    // Draw the glowing eye...
    const BossBodyPart* core = this->boss->GetCoreBody();
    if (core->GetAlpha() > 0) {
        
        // Draw the core's eye glowing effect
        glPushMatrix();
        glMultMatrixf(core->GetWorldTransform().begin());
        this->eyePulseGlow.SetParticleAlpha(MAX_EYE_GLOW_ALPHA*core->GetAlpha());
        this->eyePulseGlow.Tick(dT);
        this->eyePulseGlow.Draw(camera);
        glPopMatrix();

        // Draw spark/smoke/etc. effects for any of the dead bulbs
        // TODO

        // If the core is destroyed, draw the exploding emitter...
        if (this->boss->AreAllBulbsDestroyed()) {
            Point2D corePos = core->GetTranslationPt2D();
            glPushMatrix();
            glTranslatef(corePos[0], corePos[1], 0.0f);
            //this->coreExplodingEmitter->SetParticleAlpha(core->GetAlpha());
            this->coreExplodingEmitter->Tick(dT);
            this->coreExplodingEmitter->Draw(camera);
            glPopMatrix();
        }
    }


    const BossBodyPart* topShield    = this->boss->GetTopShield();
    const BossBodyPart* bottomShield = this->boss->GetBottomShield();
    const BossBodyPart* leftShield   = this->boss->GetLeftShield();
    const BossBodyPart* rightShield  = this->boss->GetRightShield();
    const BossBodyPart* coreShield   = this->boss->GetCoreShield();
    this->DrawShieldPostEffects(dT, camera, FuturismBoss::TOP_BOTTOM_SHIELD_X_OFFSET, 
        FuturismBoss::TOP_BOTTOM_SHIELD_Y_OFFSET, topShield, this->topShieldExplodingEmitter);
    this->DrawShieldPostEffects(dT, camera, FuturismBoss::TOP_BOTTOM_SHIELD_X_OFFSET, 
        FuturismBoss::TOP_BOTTOM_SHIELD_Y_OFFSET, bottomShield, this->bottomShieldExplodingEmitter);
    this->DrawShieldPostEffects(dT, camera, FuturismBoss::LEFT_RIGHT_SHIELD_X_OFFSET, 
        FuturismBoss::LEFT_RIGHT_SHIELD_Y_OFFSET, leftShield, this->leftShieldExplodingEmitter);
    this->DrawShieldPostEffects(dT, camera, FuturismBoss::LEFT_RIGHT_SHIELD_X_OFFSET, 
        FuturismBoss::LEFT_RIGHT_SHIELD_Y_OFFSET, rightShield, this->rightShieldExplodingEmitter);
    this->DrawShieldPostEffects(dT, camera, 0.0f, 0.0f, coreShield, this->coreShieldExplodingEmitter);

    // Check to see if the boss is frozen, if so then draw the ice...
    // NOTE: This should be the very last thing we draw for the boss
    if (this->boss->IsFrozen()) {
        
        // Update the ice material...
        MaterialGroup* iceMatGrp = this->iceEncasingMesh->GetMaterialGroups().begin()->second;
        CgFxPrism* iceMaterial = static_cast<CgFxPrism*>(iceMatGrp->GetMaterial());
        iceMaterial->SetSceneTexture(assets->GetFBOAssets()->GetFullSceneFBO()->GetFBOTexture());

        // Draw the ice...
        glPushMatrix();
        glMultMatrixf(this->boss->GetAlivePartsRoot()->GetWorldTransform().begin());
        glRotatef(this->boss->GetIceRotationInDegs(), 0, 0, 1);
        glColor4f(1, 1, 1, 0.7f);
        
        this->iceEncasingMesh->Draw(camera, keyLight, fillLight, ballLight);

        this->frostMist.Tick(dT);
        this->frostMist.Draw(camera);

        glPopMatrix();
    }
}

Point3D FuturismBossMesh::GetBossFinalExplodingEpicenter() const {
    return Point3D(this->boss->GetCoreBody()->GetTranslationPt2D(), 
        FuturismBoss::CORE_Z_DIST_FROM_ORIGIN);
}

void FuturismBossMesh::DrawDamagableBodyPart(const Camera& camera, const BasicPointLight& keyLight,
                                             const BasicPointLight& fillLight, const BasicPointLight& ballLight,
                                             const BossBodyPart* bodyPart, Mesh* nonDmgMesh, Mesh * dmgMesh) const {
    assert(bodyPart != NULL);

    const ColourRGBA& currColour = bodyPart->GetColour();
    if (currColour.A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(bodyPart->GetWorldTransform().begin());
        
        const Colour& shieldColour = this->shieldColourAnim.GetInterpolantValue();
        glColor4f(currColour.R()*shieldColour.R(), currColour.G()*shieldColour.G(),
            currColour.B()*shieldColour.B(), currColour.A());

        if (bodyPart->GetType() == AbstractBossBodyPart::WeakpointBodyPart) {
            dmgMesh->Draw(camera, keyLight, fillLight, ballLight);
        } 
        else {
            nonDmgMesh->Draw(camera, keyLight, fillLight, ballLight);
        }

        glPopMatrix();
    }
}

void FuturismBossMesh::DrawBulbBodyPart(const Camera& camera, const BasicPointLight& keyLight,
                                        const BasicPointLight& fillLight, const BasicPointLight& ballLight,
                                        const BossBodyPart* bodyPart, Mesh* mesh) const {

    const ColourRGBA& currColour = bodyPart->GetColour();
    Colour finalColour = currColour.GetColour();
    if (currColour.A() > 0.0f) {
        if (this->boss->AreBulbsVulnerable()) {
            if (bodyPart->GetIsDestroyed()) {
                finalColour = 0.25f*currColour.GetColour();
            }
            else {
                finalColour = this->bulbFlashingAnim.GetInterpolantValue();
            }
        }

        glPushMatrix();
        glMultMatrixf(bodyPart->GetWorldTransform().begin());
        glTranslatef(0, 0, this->boss->GetCurrArmMoveForwardOffset());

        glColor4f(finalColour.R(), finalColour.G(), finalColour.B(), currColour.A());
        if (this->boss->AreBulbsVulnerable() && !bodyPart->GetIsDestroyed()) {
            mesh->Draw(camera, this->weakpointMaterial);
        }
        else {
            mesh->Draw(camera, keyLight, fillLight, ballLight);
        }

        glPopMatrix();
    }
}

void FuturismBossMesh::DrawShieldPostEffects(double dT, const Camera& camera, float xOffset, float yOffset,
                                             const BossBodyPart* shield, ESPPointEmitter* explodingEmitter) {

    if (shield->GetAlpha() > 0.0f && shield->GetIsDestroyed()) {
        glPushMatrix();
        glMultMatrixf(shield->GetWorldTransform().begin());
        glTranslatef(xOffset, yOffset, 0.0f);
        //explodingEmitter->SetParticleAlpha(shield->GetAlpha());
        explodingEmitter->Tick(dT);
        explodingEmitter->Draw(camera);
        glPopMatrix();
    }
}