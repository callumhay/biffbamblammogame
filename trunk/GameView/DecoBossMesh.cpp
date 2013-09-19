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

#include "../BlammoEngine/Mesh.h"
#include "../GameModel/DecoBoss.h"
#include "../GameModel/DecoBossAIStates.h"
#include "../GameModel/BossWeakpoint.h"
#include "../ResourceManager.h"

const double DecoBossMesh::INTRO_TIME_IN_SECS = 4.0;

DecoBossMesh::DecoBossMesh(DecoBoss* boss) : BossMesh(), boss(boss),
coreMesh(NULL), lightningRelayMesh(NULL), gearMesh(NULL), scopingArm1Mesh(NULL),
scopingArm2Mesh(NULL), handMesh(NULL), leftBodyMesh(NULL), rightBodyMesh(NULL),
leftBodyExplodingEmitter(NULL), rightBodyExplodingEmitter(NULL), itemMesh(NULL) {

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
    this->handMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->DECO_BOSS_HAND_MESH);
    assert(this->handMesh != NULL);
    this->leftBodyMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->DECO_BOSS_LEFT_BODY_MESH);
    assert(this->leftBodyMesh != NULL);
    this->rightBodyMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->DECO_BOSS_RIGHT_BODY_MESH);
    assert(this->rightBodyMesh != NULL);

    this->itemMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->ITEM_MESH);
    assert(this->itemMesh != NULL);

    // The boss will only drop bad items...
    const Colour& itemEndColour = GameViewConstants::GetInstance()->GetItemColourFromDisposition(GameItem::Bad);
    this->itemMesh->SetColourForMaterial(GameViewConstants::GetInstance()->ITEM_END_MATGRP, itemEndColour);

    this->leftBodyExplodingEmitter  = this->BuildExplodingEmitter(DecoBoss::SIDE_BODY_PART_WIDTH, DecoBoss::SIDE_BODY_PART_HEIGHT);
    this->rightBodyExplodingEmitter = this->BuildExplodingEmitter(DecoBoss::SIDE_BODY_PART_WIDTH, DecoBoss::SIDE_BODY_PART_HEIGHT);
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
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->handMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->leftBodyMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->rightBodyMesh);
    assert(success);

    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->itemMesh);
    assert(success);

    UNUSED_VARIABLE(success);
    
    delete this->leftBodyExplodingEmitter;
    this->leftBodyExplodingEmitter = NULL;
    delete this->rightBodyExplodingEmitter;
    this->rightBodyExplodingEmitter = NULL;
}

double DecoBossMesh::ActivateIntroAnimation() {
    // TODO
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
                Texture2D* itemTexture = assets->GetItemAssets()->GetItemTexture(decoAIState->GetNextItemDropType());
                this->itemMesh->SetTextureForMaterial(GameViewConstants::GetInstance()->ITEM_LABEL_MATGRP, itemTexture);
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
    const BossBodyPart* leftArmHand = this->boss->GetLeftArmHand();
    DRAW_BODY_PART(leftArmHand, this->handMesh);

    // Right Arm
    const BossBodyPart* rightArmGear = this->boss->GetRightArmGear();
    DRAW_BODY_PART(rightArmGear, this->gearMesh);
    const BossBodyPart* rightArmScopingSeg1 = this->boss->GetRightArmScopingSeg1();
    DRAW_BODY_PART(rightArmScopingSeg1, this->scopingArm1Mesh);
    const BossBodyPart* rightArmScopingSeg2 = this->boss->GetRightArmScopingSeg2();
    DRAW_BODY_PART(rightArmScopingSeg2, this->scopingArm2Mesh);
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

void DecoBossMesh::DrawPostBodyEffects(double dT, const Camera& camera) {
    BossMesh::DrawPostBodyEffects(dT, camera);

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
}

Point3D DecoBossMesh::GetBossFinalExplodingEpicenter() const {
    Point3D center(this->boss->GetCore()->GetTranslationPt2D(), DecoBoss::CORE_HALF_DEPTH);
    return center;
}