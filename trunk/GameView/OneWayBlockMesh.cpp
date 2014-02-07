/**
 * OneWayBlockMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2014
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "OneWayBlockMesh.h"
#include "GameViewConstants.h"

OneWayBlockMesh::OneWayBlockMesh() : oneWayUpBlock(NULL), oneWayDownBlock(NULL), 
oneWayLeftBlock(NULL), oneWayRightBlock(NULL) {
    this->LoadMesh();
}

OneWayBlockMesh::~OneWayBlockMesh() {
    bool success = false;
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->oneWayUpBlock);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->oneWayDownBlock);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->oneWayLeftBlock);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->oneWayRightBlock);
    assert(success);
    UNUSED_VARIABLE(success);
}

void OneWayBlockMesh::DrawRegularPass(double dT, const Camera& camera, const BasicPointLight& keyLight,
                                      const BasicPointLight& fillLight, const BasicPointLight& ballLight) {
    UNUSED_PARAMETER(dT);

    for (std::map<const LevelPiece*, Mesh*>::const_iterator iter = this->frozenOneWayBlocks.begin();
        iter != this->frozenOneWayBlocks.end(); ++iter) {

            const LevelPiece* block = iter->first;
            const Point2D& blockCenter = block->GetCenter();
            Mesh* currMesh = iter->second;

            glPushMatrix();
            // Translate to the piece location in the game model...
            glTranslatef(blockCenter[0], blockCenter[1], 0.0f);
            currMesh->Draw(camera, keyLight, fillLight, ballLight);
            glPopMatrix();
    }
}

void OneWayBlockMesh::DrawTransparentNoBloomPass(double dT, const Camera& camera, const BasicPointLight& keyLight,
                                                 const BasicPointLight& fillLight, const BasicPointLight& ballLight) {
    UNUSED_PARAMETER(dT);

    glPushAttrib(GL_CURRENT_BIT);
    glColor4f(1, 1, 1, 0.75f); // One way blocks are partially transparent in this pass

    for (std::map<const LevelPiece*, Mesh*>::const_iterator iter = this->notFrozenOneWayBlocks.begin();
         iter != this->notFrozenOneWayBlocks.end(); ++iter) {

        const LevelPiece* block = iter->first;
        const Point2D& blockCenter = block->GetCenter();
        Mesh* currMesh = iter->second;

        glPushMatrix();
        // Translate to the piece location in the game model...
        glTranslatef(blockCenter[0], blockCenter[1], 0.0f);
        currMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    glPopAttrib();
}

void OneWayBlockMesh::SetAlphaMultiplier(float alpha) {

#define SET_ALPHA_ON_MESH(mesh) { \
    const std::map<std::string, MaterialGroup*>&  matGrps = mesh->GetMaterialGroups(); \
    for (std::map<std::string, MaterialGroup*>::const_iterator iter = matGrps.begin(); iter != matGrps.end(); ++iter) { \
        MaterialGroup* matGrp = iter->second; \
        matGrp->GetMaterial()->GetProperties()->alphaMultiplier = alpha; \
    } }
    
    SET_ALPHA_ON_MESH(oneWayUpBlock);
    SET_ALPHA_ON_MESH(oneWayDownBlock);
    SET_ALPHA_ON_MESH(oneWayLeftBlock);
    SET_ALPHA_ON_MESH(oneWayRightBlock);

#undef SET_ALPHA_ON_MESH
}

void OneWayBlockMesh::LoadMesh() {
    this->oneWayUpBlock    = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->ONE_WAY_BLOCK_UP_MESH);
    this->oneWayDownBlock  = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->ONE_WAY_BLOCK_DOWN_MESH);
    this->oneWayLeftBlock  = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->ONE_WAY_BLOCK_LEFT_MESH);
    this->oneWayRightBlock = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->ONE_WAY_BLOCK_RIGHT_MESH);

    assert(this->oneWayUpBlock    != NULL);
    assert(this->oneWayDownBlock  != NULL);
    assert(this->oneWayLeftBlock  != NULL);
    assert(this->oneWayRightBlock != NULL);
}

Mesh* OneWayBlockMesh::GetMeshForOneWayType(OneWayBlock::OneWayDir dir) const {
    switch (dir) {
        case OneWayBlock::OneWayUp:
            return this->oneWayUpBlock;
        case OneWayBlock::OneWayDown:
            return this->oneWayDownBlock;
        case OneWayBlock::OneWayLeft:
            return this->oneWayLeftBlock;
        case OneWayBlock::OneWayRight:
            return this->oneWayRightBlock;
        default:
            assert(false);
            break;
    }

    return this->oneWayUpBlock;
}