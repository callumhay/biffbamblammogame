/**
 * FuturismBossMesh.cpp
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

#include "FuturismBossMesh.h"
#include "GameViewConstants.h"
#include "GameAssets.h"
#include "InGameBossLevelDisplayState.h"
#include "CgFxBossWeakpoint.h"

#include "../BlammoEngine/Mesh.h"
#include "../GameModel/FuturismBoss.h"

const double FuturismBossMesh::INTRO_TIME_IN_SECS = 2.0;

FuturismBossMesh::FuturismBossMesh(FuturismBoss* boss, GameSound* sound) : BossMesh(sound), boss(boss),
coreCenterMesh(NULL), coreArmsMesh(NULL), coreBulbMesh(NULL), coreShieldMesh(NULL), leftRightShieldMesh(NULL),
topBottomShieldMesh(NULL), circleGlowTex(NULL) {
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

    this->circleGlowTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_CLEAN_CIRCLE_GRADIENT, Texture::Trilinear));
    assert(this->circleGlowTex != NULL);

    BossMesh::BuildShieldingColourAnimation(this->shieldColourAnim);

    {
        std::vector<double> timeVals;
        timeVals.reserve(3);
        timeVals.push_back(0.0);
        timeVals.push_back(1.0);
        timeVals.push_back(2.0);

        std::vector<float> pulseVals;
        pulseVals.reserve(timeVals.size());
        pulseVals.push_back(1.0f);
        pulseVals.push_back(1.33f);
        pulseVals.push_back(1.0f);

        this->glowCirclePulseAnim.SetLerp(timeVals, pulseVals);
        this->glowCirclePulseAnim.SetRepeat(true);
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

    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->circleGlowTex);
    assert(success);

    UNUSED_VARIABLE(success);
}

double FuturismBossMesh::ActivateIntroAnimation() {
    // TODO

    return FuturismBossMesh::INTRO_TIME_IN_SECS;
}


void FuturismBossMesh::DrawBody(double dT, const Camera& camera, const BasicPointLight& keyLight,
                                const BasicPointLight& fillLight, const BasicPointLight& ballLight,
                                const GameAssets* assets) {
    UNUSED_PARAMETER(assets);

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
        this->coreArmsMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

#define DRAW_POSSIBLE_WEAKPOINT_BODY_PART(bodyPart, bodyPartMesh) \
    assert(bodyPart != NULL); \
    currColour = &bodyPart->GetColour(); \
    if (currColour->A() > 0.0f) { \
        glPushMatrix(); \
        glMultMatrixf(bodyPart->GetWorldTransform().begin()); \
        glColor4f(currColour->R(), currColour->G(), currColour->B(), currColour->A()); \
        if (bodyPart->GetType() == AbstractBossBodyPart::WeakpointBodyPart) { \
            bodyPartMesh->Draw(camera, this->weakpointMaterial); \
        } \
        else { \
            bodyPartMesh->Draw(camera, keyLight, fillLight, ballLight); \
        } \
        glPopMatrix(); \
    }

    const BossBodyPart* coreTopBulb = this->boss->GetCoreTopBulb();
    const BossBodyPart* coreBottomBulb = this->boss->GetCoreBottomBulb();
    const BossBodyPart* coreLeftBulb = this->boss->GetCoreLeftBulb();
    const BossBodyPart* coreRightBulb = this->boss->GetCoreRightBulb();
    DRAW_POSSIBLE_WEAKPOINT_BODY_PART(coreTopBulb, this->coreBulbMesh);
    DRAW_POSSIBLE_WEAKPOINT_BODY_PART(coreBottomBulb, this->coreBulbMesh);
    DRAW_POSSIBLE_WEAKPOINT_BODY_PART(coreLeftBulb, this->coreBulbMesh);
    DRAW_POSSIBLE_WEAKPOINT_BODY_PART(coreRightBulb, this->coreBulbMesh);

#undef DRAW_POSSIBLE_WEAKPOINT_BODY_PART

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
        this->coreShieldMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

#define DRAW_BODY_PART(bodyPart, bodyPartMesh) \
    assert(bodyPart != NULL); \
    currColour = &bodyPart->GetColour(); \
    if (currColour->A() > 0.0f) { \
        glPushMatrix(); \
        glMultMatrixf(bodyPart->GetWorldTransform().begin()); \
        glColor4f(currColour->R()*shieldColour.R(), currColour->G()*shieldColour.G(), \
            currColour->B()*shieldColour.B(), currColour->A()); \
        bodyPartMesh->Draw(camera, keyLight, fillLight, ballLight); \
        glPopMatrix(); \
    }

    const BossBodyPart* topShield    = this->boss->GetTopShield();
    const BossBodyPart* bottomShield = this->boss->GetBottomShield();
    DRAW_BODY_PART(topShield, this->topBottomShieldMesh);
    DRAW_BODY_PART(bottomShield, this->topBottomShieldMesh);

    const BossBodyPart* leftShield  = this->boss->GetLeftShield();
    const BossBodyPart* rightShield = this->boss->GetRightShield();
    DRAW_BODY_PART(leftShield, this->leftRightShieldMesh);
    DRAW_BODY_PART(rightShield, this->leftRightShieldMesh);

#undef DRAW_BODY_PART
}

void FuturismBossMesh::DrawPostBodyEffects(double dT, const Camera& camera,
                                           const GameAssets* assets) {

    BossMesh::DrawPostBodyEffects(dT, camera, assets);
    // TODO
}

Point3D FuturismBossMesh::GetBossFinalExplodingEpicenter() const {
    return Point3D(this->boss->GetCoreBody()->GetTranslationPt2D(), 
        FuturismBoss::CORE_Z_DIST_FROM_ORIGIN);
}