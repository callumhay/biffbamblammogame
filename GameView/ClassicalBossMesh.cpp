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

#include "../GameModel/ClassicalBoss.h"

ClassicalBossMesh::ClassicalBossMesh(ClassicalBoss* boss) : BossMesh(), boss(boss),
eyeMesh(NULL), pedimentMesh(NULL), tablatureMesh(NULL), columnMesh(NULL),
baseMesh(NULL), armSquareMesh(NULL), restOfArmMesh(NULL) {

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
}

void ClassicalBossMesh::Tick(double dT) {
    // TODO?
}

void ClassicalBossMesh::Draw(const Camera& camera, const BasicPointLight& keyLight,
                             const BasicPointLight& fillLight, const BasicPointLight& ballLight) {
    
    // Using data from the GameModel's boss object, we draw the various pieces of the boss in their correct
    // worldspace locations...

    ColourRGBA currColour(1,1,1,1);

    // Eye...
    const BossBodyPart* eye = this->boss->GetEye();
    assert(eye != NULL);
    glPushMatrix();
    glMultMatrixf(eye->GetWorldTransform().begin());
    
    currColour = eye->GetColour();
    glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());
    this->eyeMesh->Draw(camera, keyLight, fillLight, ballLight);
    
    glPopMatrix();

    // Pediment...
    const BossBodyPart* pediment = this->boss->GetPediment();
    assert(pediment != NULL);
    glPushMatrix();
    glMultMatrixf(pediment->GetWorldTransform().begin());
    
    currColour = pediment->GetColour();
    glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());
    this->pedimentMesh->Draw(camera, keyLight, fillLight, ballLight);
    
    glPopMatrix();

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
    glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());

    glPushMatrix();
    glMultMatrixf(topLeftTab->GetWorldTransform().begin());
    
    this->tablatureMesh->Draw(camera, keyLight, fillLight, ballLight);

    glPopMatrix();

    glPushMatrix();
    glMultMatrixf(topRightTab->GetWorldTransform().begin());
    this->tablatureMesh->Draw(camera, keyLight, fillLight, ballLight);
    glPopMatrix();

    glPushMatrix();
    glMultMatrixf(bottomLeftTab->GetWorldTransform().begin());
    this->tablatureMesh->Draw(camera, keyLight, fillLight, ballLight);
    glPopMatrix();

    glPushMatrix();
    glMultMatrixf(bottomRightTab->GetWorldTransform().begin());
    this->tablatureMesh->Draw(camera, keyLight, fillLight, ballLight);
    glPopMatrix();

    // Columns

    // All columns have the same colour
    std::vector<const BossBodyPart*> columns = this->boss->GetBodyColumns();   

    currColour = columns.front()->GetColour();
    glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());

    for (int i = 0; i < static_cast<int>(columns.size()); i++) {
        const BossBodyPart* column = columns[i];
        assert(column != NULL);
        glPushMatrix();
        glMultMatrixf(column->GetWorldTransform().begin());
        this->columnMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    // Base
    const BossBodyPart* base = this->boss->GetBase();
    assert(base != NULL);
    glPushMatrix();
    glMultMatrixf(base->GetWorldTransform().begin());

    currColour = base->GetColour();
    glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());
    this->baseMesh->Draw(camera, keyLight, fillLight, ballLight);
    
    glPopMatrix();

    // Arms
    const BossBodyPart* leftRestOfArm  = this->boss->GetLeftRestOfArm();
    const BossBodyPart* rightRestOfArm = this->boss->GetRightRestOfArm();
    const BossBodyPart* leftArmSquare  = this->boss->GetLeftArmSquare();
    const BossBodyPart* rightArmSquare = this->boss->GetRightArmSquare();
    assert(leftRestOfArm != NULL);
    assert(rightRestOfArm != NULL);
    assert(leftArmSquare != NULL);
    assert(rightArmSquare != NULL);



    glPushMatrix();
    glMultMatrixf(leftRestOfArm->GetWorldTransform().begin());

    currColour = leftRestOfArm->GetColour();
    glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());
    this->restOfArmMesh->Draw(camera, keyLight, fillLight, ballLight);

    glPopMatrix();

    glPushMatrix();
    glMultMatrixf(rightRestOfArm->GetWorldTransform().begin());

    currColour = rightRestOfArm->GetColour();
    glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());
    this->restOfArmMesh->Draw(camera, keyLight, fillLight, ballLight);

    glPopMatrix();

    glPushMatrix();
    glMultMatrixf(leftArmSquare->GetWorldTransform().begin());

    currColour = leftArmSquare->GetColour();
    glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());
    this->armSquareMesh->Draw(camera, keyLight, fillLight, ballLight);

    glPopMatrix();

    glPushMatrix();
    glMultMatrixf(rightArmSquare->GetWorldTransform().begin());

    currColour = rightArmSquare->GetColour();
    glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());
    this->armSquareMesh->Draw(camera, keyLight, fillLight, ballLight);

    glPopMatrix();

}