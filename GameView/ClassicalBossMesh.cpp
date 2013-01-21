/**
 * ClassicalBossMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "ClassicalBossMesh.h"
#include "GameViewConstants.h"

#include "../ResourceManager.h"

#include "../BlammoEngine/Mesh.h"

#include "../ESPEngine/ESP.h"

#include "../GameModel/ClassicalBoss.h"
#include "../GameModel/BossWeakpoint.h"

ClassicalBossMesh::ClassicalBossMesh(ClassicalBoss* boss) : BossMesh(), boss(boss),
eyeMesh(NULL), pedimentMesh(NULL), tablatureMesh(NULL), columnMesh(NULL),
baseMesh(NULL), armSquareMesh(NULL), restOfArmMesh(NULL), leftArmSmokeEmitter(NULL),
rightArmSmokeEmitter(NULL), leftArmExplodingEmitter(NULL), rightArmExplodingEmitter(NULL) {

    assert(boss != NULL);

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
    this->leftArmSmokeEmitter  = this->BuildFireSmokeEmitter(ClassicalBoss::ARM_WIDTH, ClassicalBoss::ARM_HEIGHT);
    this->rightArmSmokeEmitter = this->BuildFireSmokeEmitter(ClassicalBoss::ARM_WIDTH, ClassicalBoss::ARM_HEIGHT);

    this->leftArmExplodingEmitter  = this->BuildExplodingEmitter(ClassicalBoss::ARM_WIDTH, ClassicalBoss::ARM_HEIGHT);
    this->rightArmExplodingEmitter = this->BuildExplodingEmitter(ClassicalBoss::ARM_WIDTH, ClassicalBoss::ARM_HEIGHT);
}

ClassicalBossMesh::~ClassicalBossMesh() {

    // The boss pointer doesn't belong to this, just referenced
    this->boss = NULL;

    // Release the mesh assets...
    bool success = false;
    
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

    delete this->leftArmSmokeEmitter;
    this->leftArmSmokeEmitter = NULL;
    delete this->rightArmSmokeEmitter;
    this->rightArmSmokeEmitter = NULL;
    delete this->leftArmExplodingEmitter;
    this->leftArmExplodingEmitter = NULL;
    delete this->rightArmExplodingEmitter;
    this->rightArmExplodingEmitter = NULL;
}

void ClassicalBossMesh::Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
                             const BasicPointLight& fillLight, const BasicPointLight& ballLight) {
    
    // Using data from the GameModel's boss object, we draw the various pieces of the boss in their correct
    // worldspace locations...

    ColourRGBA currColour;

    // Eye...
    const BossBodyPart* eye = this->boss->GetEye();
    assert(eye != NULL);

    currColour = eye->GetColour();
    if (currColour.A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(eye->GetWorldTransform().begin());
        glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());
        this->eyeMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    // Pediment...
    const BossBodyPart* pediment = this->boss->GetPediment();
    assert(pediment != NULL);

    currColour = pediment->GetColour();
    if (currColour.A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(pediment->GetWorldTransform().begin());
        glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());
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
    currColour = topLeftTab->GetColour();
    if (currColour.A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(topLeftTab->GetWorldTransform().begin());
        glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());
        this->tablatureMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    currColour = topRightTab->GetColour();
    if (currColour.A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(topRightTab->GetWorldTransform().begin());
        glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());
        this->tablatureMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    currColour = bottomLeftTab->GetColour();
    if (currColour.A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(bottomLeftTab->GetWorldTransform().begin());
        glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());
        this->tablatureMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    currColour = bottomRightTab->GetColour();
    if (currColour.A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(bottomRightTab->GetWorldTransform().begin());
        glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());
        this->tablatureMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    // Columns
    std::vector<const BossBodyPart*> columns = this->boss->GetBodyColumns();   
    for (int i = 0; i < static_cast<int>(columns.size()); i++) {
        const BossBodyPart* column = columns[i];
        assert(column != NULL);

        currColour = column->GetColour();
        if (currColour.A() > 0.0f) {
            glPushMatrix();
            glMultMatrixf(column->GetWorldTransform().begin());
            glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());
            this->columnMesh->Draw(camera, keyLight, fillLight, ballLight);
            glPopMatrix();
        }
    }

    // Base
    const BossBodyPart* base = this->boss->GetBase();
    assert(base != NULL);

    currColour = base->GetColour();
    if (currColour.A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(base->GetWorldTransform().begin());
        glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());
        this->baseMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    // Arms
    const BossBodyPart* leftRestOfArm  = this->boss->GetLeftRestOfArm();
    const BossBodyPart* rightRestOfArm = this->boss->GetRightRestOfArm();
    const BossBodyPart* leftArmSquare  = this->boss->GetLeftArmSquare();
    const BossBodyPart* rightArmSquare = this->boss->GetRightArmSquare();
    assert(leftRestOfArm != NULL);
    assert(rightRestOfArm != NULL);
    assert(leftArmSquare != NULL);
    assert(rightArmSquare != NULL);

    currColour = leftRestOfArm->GetColour();
    if (currColour.A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(leftRestOfArm->GetWorldTransform().begin());
        glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());
        this->restOfArmMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    currColour = rightRestOfArm->GetColour();
    if (currColour.A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(rightRestOfArm->GetWorldTransform().begin());
        glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());
        this->restOfArmMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    currColour = leftArmSquare->GetColour();
    if (currColour.A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(leftArmSquare->GetWorldTransform().begin());
        glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());
        this->armSquareMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    currColour = rightArmSquare->GetColour();
    if (currColour.A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(rightArmSquare->GetWorldTransform().begin());
        glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());
        this->armSquareMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    this->DrawEffects(dT, camera);
}

void ClassicalBossMesh::DrawEffects(double dT, const Camera& camera) {

    // Draw the effects for the arms of the boss when it's hurt or destroyed...
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
                    this->leftArmSmokeEmitter->SetSpawnDelta(ESPInterval(lifePercentage*0.5f, lifePercentage));
                    this->leftArmSmokeEmitter->Tick(dT);

                    glPushMatrix();
                    glMultMatrixf(leftRestOfArm->GetWorldTransform().begin());
                    this->leftArmSmokeEmitter->Draw(camera);
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
                    this->rightArmSmokeEmitter->SetSpawnDelta(ESPInterval(lifePercentage*0.5f, lifePercentage));
                    this->rightArmSmokeEmitter->Tick(dT);

                    glPushMatrix();
                    glMultMatrixf(rightRestOfArm->GetWorldTransform().begin());
                    this->rightArmSmokeEmitter->Draw(camera);
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

}