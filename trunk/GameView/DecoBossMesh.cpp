/**
 * DecoBossMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "DecoBossMesh.h"
#include "GameViewConstants.h"
#include "GameAssets.h"
#include "InGameBossLevelDisplayState.h"

#include "../BlammoEngine/Mesh.h"
#include "../GameModel/DecoBoss.h"
#include "../GameModel/DecoBossAIStates.h"
#include "../GameModel/BossWeakpoint.h"
#include "../ResourceManager.h"

const double DecoBossMesh::INTRO_TIME_IN_SECS = 3.5;

DecoBossMesh::DecoBossMesh(DecoBoss* boss, GameSound* sound) : BossMesh(sound), boss(boss),
coreMesh(NULL), lightningRelayMesh(NULL), gearMesh(NULL), scopingArm1Mesh(NULL),
scopingArm2Mesh(NULL), scopingArm3Mesh(NULL), scopingArm4Mesh(NULL), handMesh(NULL), 
leftBodyMesh(NULL), rightBodyMesh(NULL), leftBodyExplodingEmitter(NULL), 
rightBodyExplodingEmitter(NULL), itemMesh(NULL), leftArmExplodingEmitter(NULL),
rightArmExplodingEmitter(NULL), bodyExplodingEmitter(NULL), glowTex(NULL), introTimeCountdown(0.0),
flareGlowTex(NULL), lensFlareTex(NULL), lightningRelayEmitter(NULL), flarePulse(0,0), 
eyeGlowSoundID(INVALID_SOUND_ID), lightningRelaySoundID(INVALID_SOUND_ID) {

    assert(boss != NULL);

    // Load all of the meshes for the boss
    this->coreMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->DECO_BOSS_CORE_MESH);
    assert(this->coreMesh != NULL);
    this->lightningRelayMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->DECO_BOSS_LIGHTNING_RELAY_MESH);
    assert(this->lightningRelayMesh != NULL);
    this->gearMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->DECO_BOSS_GEAR_MESH);
    assert(this->gearMesh != NULL);
    this->scopingArm1Mesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->DECO_BOSS_SCOPING_ARM1_MESH);
    assert(this->scopingArm1Mesh != NULL);
    this->scopingArm2Mesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->DECO_BOSS_SCOPING_ARM2_MESH);
    assert(this->scopingArm2Mesh != NULL);
    this->scopingArm3Mesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->DECO_BOSS_SCOPING_ARM3_MESH);
    assert(this->scopingArm3Mesh != NULL);
    this->scopingArm4Mesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->DECO_BOSS_SCOPING_ARM4_MESH);
    assert(this->scopingArm4Mesh != NULL);
    this->handMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->DECO_BOSS_HAND_MESH);
    assert(this->handMesh != NULL);
    this->leftBodyMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->DECO_BOSS_LEFT_BODY_MESH);
    assert(this->leftBodyMesh != NULL);
    this->rightBodyMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->DECO_BOSS_RIGHT_BODY_MESH);
    assert(this->rightBodyMesh != NULL);

    this->itemMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->ITEM_MESH);
    assert(this->itemMesh != NULL);

    this->glowTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_CLEAN_CIRCLE_GRADIENT, Texture::Trilinear));
    assert(this->glowTex != NULL);

    this->leftBodyExplodingEmitter  = this->BuildExplodingEmitter(0.75f, this->boss->GetLeftBody(), DecoBoss::SIDE_BODY_PART_WIDTH, DecoBoss::SIDE_BODY_PART_HEIGHT);
    this->rightBodyExplodingEmitter = this->BuildExplodingEmitter(0.75f, this->boss->GetRightBody(), DecoBoss::SIDE_BODY_PART_WIDTH, DecoBoss::SIDE_BODY_PART_HEIGHT);
    this->leftArmExplodingEmitter   = this->BuildExplodingEmitter(0.8f,  this->boss->GetLeftArm(), DecoBoss::ARM_WIDTH, DecoBoss::ARM_NOT_EXTENDED_HEIGHT);
    this->rightArmExplodingEmitter  = this->BuildExplodingEmitter(0.8f,  this->boss->GetRightArm(), DecoBoss::ARM_WIDTH, DecoBoss::ARM_NOT_EXTENDED_HEIGHT);
    this->bodyExplodingEmitter      = this->BuildExplodingEmitter(1.0f,  this->boss->GetCore(), DecoBoss::CORE_WIDTH, DecoBoss::CORE_HEIGHT);

    this->eyeGlowAlphaAnims.resize(DecoBoss::NUM_EYES);
    for (int i = 0; i < static_cast<int>(this->eyeGlowAlphaAnims.size()); i++) {
        this->eyeGlowAlphaAnims[i].ClearLerp();
        this->eyeGlowAlphaAnims[i].SetInterpolantValue(0.0f);
        this->eyeGlowAlphaAnims[i].SetRepeat(false);
    }
    this->lightningRelayAlphaAnim.ClearLerp();
    this->lightningRelayAlphaAnim.SetInterpolantValue(0.0f);
    this->lightningRelayAlphaAnim.SetRepeat(false);

    this->flareGlowTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_BRIGHT_FLARE, Texture::Trilinear, GL_TEXTURE_2D));
    assert(this->flareGlowTex != NULL);
    this->lensFlareTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_LENSFLARE, Texture::Trilinear, GL_TEXTURE_2D));
    assert(this->lensFlareTex != NULL);

    ScaleEffect flarePulseSettings;
    flarePulseSettings.pulseGrowthScale = 1.25f;
    flarePulseSettings.pulseRate        = 5.0f;
    this->flarePulse = ESPParticleScaleEffector(flarePulseSettings);

    this->lightningRelayEmitter = new ESPPointEmitter();
    this->lightningRelayEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
    this->lightningRelayEmitter->SetInitialSpd(ESPInterval(0));
    this->lightningRelayEmitter->SetParticleLife(ESPInterval(ESPParticle::INFINITE_PARTICLE_LIFETIME));
    this->lightningRelayEmitter->SetEmitAngleInDegrees(0);
    this->lightningRelayEmitter->SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
    this->lightningRelayEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    this->lightningRelayEmitter->SetEmitPosition(Point3D(0, 0, 0));
    this->lightningRelayEmitter->SetParticleSize(ESPInterval(DecoBoss::LIGHTNING_RELAY_GLOW_SIZE));
    this->lightningRelayEmitter->SetParticleColour(ESPInterval(0.9f), ESPInterval(0.78f), ESPInterval(1.0f), ESPInterval(1.0f));
    this->lightningRelayEmitter->AddEffector(&this->flarePulse);
    this->lightningRelayEmitter->SetParticles(1, this->flareGlowTex);

    {
        static const int NUM_CYCLES = 5;
        std::vector<double> timeVals;
        timeVals.reserve(1 + 2*NUM_CYCLES);
        timeVals.push_back(0.0);
        for (int i = 0; i < NUM_CYCLES; i++) {
            timeVals.push_back(timeVals.back() + 0.01 + 0.05*Randomizer::GetInstance()->RandomNumZeroToOne());
            timeVals.push_back(timeVals.back() + 0.01 + 0.05*Randomizer::GetInstance()->RandomNumZeroToOne());
        }

        std::vector<float> alphaVals;
        alphaVals.reserve(timeVals.size());
        alphaVals.push_back(1.0f);
        for (int i = 0; i < NUM_CYCLES; i++) {
            alphaVals.push_back(0.0f);
            alphaVals.push_back(1.0f);
        }

        this->eyeFlashingAlphaAnim.SetLerp(timeVals, alphaVals);
        this->eyeFlashingAlphaAnim.SetRepeat(true);
    }

}

DecoBossMesh::~DecoBossMesh() {
    this->boss = NULL;

    // Unload all the meshes for the boss
    bool success = false;
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->coreMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->lightningRelayMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->gearMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->scopingArm1Mesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->scopingArm2Mesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->scopingArm3Mesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->scopingArm4Mesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->handMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->leftBodyMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->rightBodyMesh);
    assert(success);

    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->itemMesh);
    assert(success);

    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->glowTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->flareGlowTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->lensFlareTex);
    assert(success);

    UNUSED_VARIABLE(success);
    
    delete this->leftBodyExplodingEmitter;
    this->leftBodyExplodingEmitter = NULL;
    delete this->rightBodyExplodingEmitter;
    this->rightBodyExplodingEmitter = NULL;
    delete this->leftArmExplodingEmitter;
    this->leftArmExplodingEmitter = NULL;
    delete this->rightArmExplodingEmitter;
    this->rightArmExplodingEmitter = NULL;
    delete this->bodyExplodingEmitter;
    this->bodyExplodingEmitter;

    delete this->lightningRelayEmitter;
    this->lightningRelayEmitter = NULL;
}

double DecoBossMesh::ActivateIntroAnimation() {
    static const float MAX_GLOW_ALPHA = 0.75f;

    double nextStartTime = INTRO_TIME_IN_SECS/3.0;
    const double LIGHTNING_RELAY_GLOW_TIME = 1.0;
    this->lightningRelayAlphaAnim.SetLerp(nextStartTime, nextStartTime + LIGHTNING_RELAY_GLOW_TIME, 0.0f, 1.0f);
    nextStartTime += LIGHTNING_RELAY_GLOW_TIME;

    const double TIME_PER_EYE_GLOW = 
        (INTRO_TIME_IN_SECS - nextStartTime - InGameBossLevelDisplayState::TIME_OF_UNPAUSE_BEFORE_INTRO_END) / static_cast<double>(DecoBoss::NUM_EYES-1);
    const double HALF_TIME_PER_EYE_GLOW = TIME_PER_EYE_GLOW / 2.0;

    this->eyeGlowAlphaAnims[0].SetLerp(nextStartTime, nextStartTime + TIME_PER_EYE_GLOW, 0.0f, MAX_GLOW_ALPHA);
    this->eyeGlowAlphaAnims[0].SetInterpolantValue(0.0f);
    this->eyeGlowAlphaAnims[0].SetRepeat(false);
    this->eyeGlowAlphaAnims[1].SetLerp(nextStartTime, nextStartTime + TIME_PER_EYE_GLOW, 0.0f, MAX_GLOW_ALPHA);
    this->eyeGlowAlphaAnims[1].SetInterpolantValue(0.0f);
    this->eyeGlowAlphaAnims[1].SetRepeat(false);
    nextStartTime += HALF_TIME_PER_EYE_GLOW;

    for (int i = 2; i < static_cast<int>(this->eyeGlowAlphaAnims.size()); i++) {
        this->eyeGlowAlphaAnims[i].SetLerp(nextStartTime, nextStartTime + TIME_PER_EYE_GLOW, 0.0f, MAX_GLOW_ALPHA);
        this->eyeGlowAlphaAnims[i].SetInterpolantValue(0.0f);
        this->eyeGlowAlphaAnims[i].SetRepeat(false);
        nextStartTime += HALF_TIME_PER_EYE_GLOW;
    }


    this->introTimeCountdown = INTRO_TIME_IN_SECS;
    return DecoBossMesh::INTRO_TIME_IN_SECS;
}

void DecoBossMesh::DrawBody(double dT, const Camera& camera, const BasicPointLight& keyLight,
                            const BasicPointLight& fillLight, const BasicPointLight& ballLight, 
                            const GameAssets* assets) {

    UNUSED_PARAMETER(dT);

    // Using data from the GameModel's boss object, we draw the various pieces of the boss in their correct
    // world space locations...

    ColourRGBA currColour;

#define DRAW_BODY_PART(bodyPart, bodyPartMesh) \
    assert(bodyPart != NULL); \
    currColour = bodyPart->GetColour(); \
    if (currColour.A() > 0.0f) { \
    glPushMatrix(); \
    glMultMatrixf(bodyPart->GetWorldTransform().begin()); \
    glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A()); \
    bodyPartMesh->Draw(camera, keyLight, fillLight, ballLight); \
    glPopMatrix(); \
    }
    
    // Middle body
    const BossBodyPart* core = this->boss->GetCore();

    currColour = core->GetColour();
    if (currColour.A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(core->GetWorldTransform().begin());
        glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());
        this->coreMesh->Draw(camera, keyLight, fillLight, ballLight);
        
        // Draw any loaded item that the boss is about to drop towards the paddle
        const BossAIState* currAIState = this->boss->GetCurrentAIState();
        if (currAIState != NULL) {
            using namespace decobossai;
            const DecoBossAIState* decoAIState = static_cast<const DecoBossAIState*>(currAIState);
            float itemLoadOffset = decoAIState->GetItemLoadOffset();
            if (itemLoadOffset > 0.0f) {
                // Draw the loaded item...
                glColor4f(1,1,1,1);
                glTranslatef(0.0f, DecoBoss::ITEM_LOAD_OFFSET_Y - itemLoadOffset, 0.0f);
                
                // Set the item mesh texture...
                GameItem::ItemType nextItemDropType = decoAIState->GetNextItemDropType();
                Texture2D* itemTexture = assets->GetItemAssets()->GetItemTexture(nextItemDropType);
                this->itemMesh->SetTextureForMaterial(GameViewConstants::GetInstance()->ITEM_LABEL_MATGRP, itemTexture);
               
                // The boss will only drop bad items...
                const Colour& itemEndColour = GameViewConstants::GetInstance()->GetItemColourFromDisposition(
                    GameItemFactory::GetInstance()->GetItemTypeDisposition(nextItemDropType));
                this->itemMesh->SetColourForMaterial(GameViewConstants::GetInstance()->ITEM_END_MATGRP, itemEndColour);

                this->itemMesh->Draw(camera, keyLight, fillLight, ballLight);
            }
        }

        glPopMatrix();
    }


    const BossBodyPart* lightningRelay = this->boss->GetLightningRelay();
    DRAW_BODY_PART(lightningRelay, this->lightningRelayMesh);

    // Left Arm
    const BossBodyPart* leftArmGear = this->boss->GetLeftArmGear();
    DRAW_BODY_PART(leftArmGear, this->gearMesh);
    const BossBodyPart* leftArmScopingSeg1 = this->boss->GetLeftArmScopingSeg1();
    DRAW_BODY_PART(leftArmScopingSeg1, this->scopingArm1Mesh);
    const BossBodyPart* leftArmScopingSeg2 = this->boss->GetLeftArmScopingSeg2();
    DRAW_BODY_PART(leftArmScopingSeg2, this->scopingArm2Mesh);
    const BossBodyPart* leftArmScopingSeg3 = this->boss->GetLeftArmScopingSeg3();
    DRAW_BODY_PART(leftArmScopingSeg3, this->scopingArm3Mesh);
    const BossBodyPart* leftArmScopingSeg4 = this->boss->GetLeftArmScopingSeg4();
    DRAW_BODY_PART(leftArmScopingSeg4, this->scopingArm4Mesh);
    const BossBodyPart* leftArmHand = this->boss->GetLeftArmHand();
    DRAW_BODY_PART(leftArmHand, this->handMesh);

    // Right Arm
    const BossBodyPart* rightArmGear = this->boss->GetRightArmGear();
    DRAW_BODY_PART(rightArmGear, this->gearMesh);
    const BossBodyPart* rightArmScopingSeg1 = this->boss->GetRightArmScopingSeg1();
    DRAW_BODY_PART(rightArmScopingSeg1, this->scopingArm1Mesh);
    const BossBodyPart* rightArmScopingSeg2 = this->boss->GetRightArmScopingSeg2();
    DRAW_BODY_PART(rightArmScopingSeg2, this->scopingArm2Mesh);
    const BossBodyPart* rightArmScopingSeg3 = this->boss->GetRightArmScopingSeg3();
    DRAW_BODY_PART(rightArmScopingSeg3, this->scopingArm3Mesh);
    const BossBodyPart* rightArmScopingSeg4 = this->boss->GetRightArmScopingSeg4();
    DRAW_BODY_PART(rightArmScopingSeg4, this->scopingArm4Mesh);
    const BossBodyPart* rightArmHand = this->boss->GetRightArmHand();
    DRAW_BODY_PART(rightArmHand, this->handMesh);

    // Left Body
    const BossBodyPart* leftBody = this->boss->GetLeftBody();
    DRAW_BODY_PART(leftBody, this->leftBodyMesh);

    // Right Body
    const BossBodyPart* rightBody = this->boss->GetRightBody();
    DRAW_BODY_PART(rightBody, this->rightBodyMesh);

#undef DRAW_BODY_PART
}

void DecoBossMesh::DrawPostBodyEffects(double dT, const Camera& camera, const GameAssets* assets) {
    BossMesh::DrawPostBodyEffects(dT, camera, assets);

    // Check to see if we're drawing intro effects
    if (this->introTimeCountdown > 0.0) {
        this->lightningRelayAlphaAnim.Tick(dT);

        for (int i = 0; i < static_cast<int>(this->eyeGlowAlphaAnims.size()); i++) {
            this->eyeGlowAlphaAnims[i].Tick(dT);
        }

        if (this->lightningRelaySoundID == INVALID_SOUND_ID && this->lightningRelayAlphaAnim.GetInterpolantValue() > 0.0f) {
            this->lightningRelaySoundID = assets->GetSound()->PlaySound(GameSound::DecoBossLightningRelayTurnOnEvent, false, false);
        }

        if (this->eyeGlowSoundID == INVALID_SOUND_ID && this->eyeGlowAlphaAnims[0].GetInterpolantValue() > 0.0f) {
            this->eyeGlowSoundID = assets->GetSound()->PlaySound(GameSound::BossGlowEvent, false, false);
        }

        this->introTimeCountdown -= dT;
    }

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    // Left side
    const BossBodyPart* leftBody = this->boss->GetLeftBody();
    if (leftBody->GetAlpha() > 0.0f) {
        if (leftBody->GetIsDestroyed()) {

            Point2D position = leftBody->GetTranslationPt2D();
            glPushMatrix();
            glTranslatef(position[0], position[1], 0);
            this->leftBodyExplodingEmitter->Tick(dT);
            this->leftBodyExplodingEmitter->Draw(camera);
            glPopMatrix();

        }
    }

    // Right side
    const BossBodyPart* rightBody = this->boss->GetRightBody();
    if (rightBody->GetAlpha() > 0.0f) {
        if (rightBody->GetIsDestroyed()) {

            Point2D position = rightBody->GetTranslationPt2D();
            glPushMatrix();
            glTranslatef(position[0], position[1], 0);
            this->leftBodyExplodingEmitter->Tick(dT);
            this->leftBodyExplodingEmitter->Draw(camera);
            glPopMatrix();

        }
    }

    // Left arm
    const BossBodyPart* leftArmGear = this->boss->GetLeftArmGear();
    if (leftArmGear->GetAlpha() > 0.0f) {
        const BossCompositeBodyPart* leftArm = this->boss->GetLeftArm();
        if (leftArm->GetIsDestroyed()) {
            Point2D position = leftArm->GetTranslationPt2D();
            glPushMatrix();
            glTranslatef(position[0], position[1], 0);
            this->leftArmExplodingEmitter->Tick(dT);
            this->leftArmExplodingEmitter->Draw(camera);
            glPopMatrix();
        }
    }

    // Right arm
    const BossBodyPart* rightArmGear = this->boss->GetRightArmGear();
    if (rightArmGear->GetAlpha() > 0.0f) {
        const BossCompositeBodyPart* rightArm = this->boss->GetRightArm();
        if (rightArm->GetIsDestroyed()) {
            Point2D position = rightArm->GetTranslationPt2D();
            glPushMatrix();
            glTranslatef(position[0], position[1], 0);
            this->rightArmExplodingEmitter->Tick(dT);
            this->rightArmExplodingEmitter->Draw(camera);
            glPopMatrix();
        }
    }

    // Core/Body
    const BossBodyPart* coreBody = this->boss->GetCore();
    if (coreBody->GetAlpha() > 0.0f) {
        Point3D coreBodyPos = coreBody->GetTranslationPt3D();

        // Draw the eyes...
        this->glowTex->BindTexture();
        glPushMatrix();
        Point3D eyeClusterCenterPos = this->boss->GetEyeClusterCenterPosition();
        glTranslatef(eyeClusterCenterPos[0], eyeClusterCenterPos[1], eyeClusterCenterPos[2]+0.1f);

        using namespace decobossai;

        float eyeBaseAlpha = coreBody->GetAlpha();
        DecoBossAIState::AIState currBossState = static_cast<const DecoBossAIState*>(this->boss->GetCurrentAIState())->GetCurrentAIState();
        if (currBossState == DecoBossAIState::RotatingLevelAIState) {
            this->eyeFlashingAlphaAnim.Tick(dT);
            eyeBaseAlpha *= this->eyeFlashingAlphaAnim.GetInterpolantValue();
        }

        for (int i = 0; i < static_cast<int>(this->eyeGlowAlphaAnims.size()); i++) {
            glColor4f(1.0f, 0.1f, 0.1f, eyeBaseAlpha * this->eyeGlowAlphaAnims[i].GetInterpolantValue());
            const Vector2D& offset = this->boss->GetEyeOffset(i);
            glTranslatef(offset[0], offset[1], 0.0f);
            GeometryMaker::GetInstance()->DrawQuad();
            glTranslatef(-offset[0], -offset[1], 0.0f);
        }
 
        glPopMatrix();
        this->glowTex->UnbindTexture();

        // Draw the lightning relay emitters
        glPushMatrix();
        
        glTranslatef(coreBodyPos[0], coreBodyPos[1], coreBodyPos[2]);
        glRotatef(this->boss->GetAlivePartsRoot()->GetLocalZRotation(), 0, 0, 1);

        this->lightningRelayEmitter->Tick(dT);
        this->lightningRelayEmitter->SetParticleAlpha(coreBody->GetAlpha() * this->lightningRelayAlphaAnim.GetInterpolantValue());
        this->lightningRelayEmitter->OverwriteEmittedPosition(this->boss->GetLeftLightningRelayOffset());
        this->lightningRelayEmitter->Draw(camera);
        this->lightningRelayEmitter->OverwriteEmittedPosition(this->boss->GetRightLightningRelayOffset());
        this->lightningRelayEmitter->Draw(camera);
        glPopMatrix();

        if (this->boss->GetIsStateMachineFinished()) {
            
            // Final body explosion emitter
            glPushMatrix();
            glTranslatef(coreBodyPos[0], coreBodyPos[1], coreBodyPos[2]);
            this->bodyExplodingEmitter->SetParticleAlpha(coreBody->GetAlpha());
            this->bodyExplodingEmitter->Tick(dT);
            this->bodyExplodingEmitter->Draw(camera);
            glPopMatrix();
        }
    }

    glPopAttrib();
}

Point3D DecoBossMesh::GetBossFinalExplodingEpicenter() const {
    Point3D center(this->boss->GetCore()->GetTranslationPt2D(), DecoBoss::CORE_HALF_DEPTH);
    return center;
}