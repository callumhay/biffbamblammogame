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
    
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    // Using data from the GameModel's boss object, we draw the various pieces of the boss in their correct
    // worldspace locations...
    const BossBodyPart* eye = this->boss->GetEye();
    assert(eye != NULL);
    
    glPushMatrix();
    glMultMatrixf(eye->GetWorldTransform().begin());
    this->eyeMesh->Draw(camera, keyLight, fillLight, ballLight);
    glPopMatrix();

    const BossBodyPart* pediment = this->boss->GetPediment();
    assert(pediment != NULL);
    glPushMatrix();
    glMultMatrixf(pediment->GetWorldTransform().begin());
    this->pedimentMesh->Draw(camera, keyLight, fillLight, ballLight);
    glPopMatrix();

}