/**
 * NouveauBossMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "NouveauBossMesh.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Mesh.h"
#include "../GameModel/NouveauBoss.h"
#include "../ResourceManager.h"

const double NouveauBossMesh::INTRO_TIME_IN_SECS = 4.0;

NouveauBossMesh::NouveauBossMesh(NouveauBoss* boss) : BossMesh(), boss(boss),
bodyMesh(NULL), bottomCurlMesh(NULL), bottomHexSphereMesh(NULL),
sideCurlsMesh(NULL), sideSphereMesh(NULL), sideSphereFrillsMesh(NULL),
sideSphereHolderCurlMesh(NULL), topDomeMesh(NULL), topGazeboMesh(NULL),
topSphereMesh(NULL) {

    assert(boss != NULL);

    this->bodyMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_BOSS_BODY_MESH);
    assert(this->bodyMesh != NULL);
    this->bottomCurlMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_BOSS_BOTTOM_CURL_MESH);
    assert(this->bottomCurlMesh != NULL);
    this->bottomHexSphereMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_BOSS_BOTTOM_HEX_SPHERE_MESH);
    assert(this->bottomHexSphereMesh != NULL);
    this->sideCurlsMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_BOSS_SIDE_CURLS_MESH);
    assert(this->sideCurlsMesh != NULL);
    this->sideSphereMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_BOSS_SIDE_SPHERE_MESH);
    assert(this->sideSphereMesh != NULL);
    this->sideSphereFrillsMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_BOSS_SIDE_SPHERE_FRILLS_MESH);
    assert(this->sideSphereFrillsMesh != NULL);
    this->sideSphereHolderCurlMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_BOSS_SIDE_SPHERE_HOLDER_CURL_MESH);
    assert(this->sideSphereHolderCurlMesh != NULL);
    this->topDomeMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_BOSS_TOP_ENCLOSURE_DOME_MESH);
    assert(this->topDomeMesh != NULL);
    this->topGazeboMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_BOSS_TOP_ENCLOSURE_GAZEBO_MESH);
    assert(this->topGazeboMesh != NULL);
    this->topSphereMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_BOSS_TOP_SPHERE_MESH);
    assert(this->topSphereMesh != NULL);
}

NouveauBossMesh::~NouveauBossMesh() {
    this->boss = NULL;

    bool success = false;

    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->bodyMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->bottomCurlMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->bottomHexSphereMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->sideCurlsMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->sideSphereMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->sideSphereFrillsMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->sideSphereHolderCurlMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->topDomeMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->topGazeboMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->topSphereMesh);
    assert(success);

    UNUSED_VARIABLE(success);
}

double NouveauBossMesh::ActivateIntroAnimation() {
    
    // TODO


    return NouveauBossMesh::INTRO_TIME_IN_SECS;
}

void NouveauBossMesh::DrawBody(double dT, const Camera& camera, const BasicPointLight& keyLight,
                               const BasicPointLight& fillLight, const BasicPointLight& ballLight) {

    UNUSED_PARAMETER(dT);
    // Using data from the GameModel's boss object, we draw the various pieces of the boss in their correct
    // worldspace locations...

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

    // Core Body
    const BossBodyPart* body = this->boss->GetBody();  
    DRAW_BODY_PART(body, this->bodyMesh);

    // Side curls
    const BossBodyPart* leftSideCurl = this->boss->GetLeftSideCurls();
    DRAW_BODY_PART(leftSideCurl, this->sideCurlsMesh);
    const BossBodyPart* rightSideCurl = this->boss->GetRightSideCurls();
    DRAW_BODY_PART(rightSideCurl, this->sideCurlsMesh);

    // Side arms (curl holder, sphere, frills)
    const BossBodyPart* leftHolderCurl       = this->boss->GetLeftSideHolderCurl();
    const BossBodyPart* leftSideSphere       = this->boss->GetLeftSideSphere();
    const BossBodyPart* leftSideSphereFrills = this->boss->GetLeftSideSphereFrills();
    DRAW_BODY_PART(leftHolderCurl, this->sideSphereHolderCurlMesh);
    DRAW_BODY_PART(leftSideSphere, this->sideSphereMesh);
    DRAW_BODY_PART(leftSideSphereFrills, this->sideSphereFrillsMesh);

    const BossBodyPart* rightHolderCurl       = this->boss->GetRightSideHolderCurl();
    const BossBodyPart* rightSideSphere       = this->boss->GetRightSideSphere();
    const BossBodyPart* rightSideSphereFrills = this->boss->GetRightSideSphereFrills();
    DRAW_BODY_PART(rightHolderCurl, this->sideSphereHolderCurlMesh);
    DRAW_BODY_PART(rightSideSphere, this->sideSphereMesh);
    DRAW_BODY_PART(rightSideSphereFrills, this->sideSphereFrillsMesh);

    // Bottom hex-sphere
    const BossBodyPart* bottomHexSphere = this->boss->GetBottomHexSphere();
    DRAW_BODY_PART(bottomHexSphere, this->bottomHexSphereMesh);

    // Bottom curls
    for (int i = 0; i < NouveauBoss::NUM_CURLS; i++) {
        const BossBodyPart* bottomCurl = this->boss->GetBottomCurl(i);
        DRAW_BODY_PART(bottomCurl, this->bottomCurlMesh);
    }

    // Top sphere
    const BossBodyPart* topSphere = this->boss->GetTopSphere();
    DRAW_BODY_PART(topSphere, this->topSphereMesh);

    // Top Gazebo
    const BossBodyPart* topGazebo = this->boss->GetTopGazebo();
    DRAW_BODY_PART(topGazebo, this->topGazeboMesh);

    // Top Dome
    const BossBodyPart* topDome = this->boss->GetTopDome();
    DRAW_BODY_PART(topDome, this->topDomeMesh);

#undef BODY_PART_MESH
}

void NouveauBossMesh::DrawPostBodyEffects(double dT, const Camera& camera) {

}

Point3D NouveauBossMesh::GetBossFinalExplodingEpicenter() const {
    Point3D center(this->boss->GetBody()->GetTranslationPt2D(), NouveauBoss::BODY_CORE_MID_HALF_DEPTH);
    return center;
}