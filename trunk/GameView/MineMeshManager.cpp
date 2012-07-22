/**
 * MineMeshManager.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "MineMeshManager.h"
#include "GameViewConstants.h"

#include "../GameModel/PaddleMineProjectile.h"

MineMeshManager::MineMeshManager() : mineMesh(NULL) {
    this->mineMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->MINE_MESH);
    assert(this->mineMesh != NULL);
}

MineMeshManager::~MineMeshManager() {
    bool success = false;
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->mineMesh);
    assert(success);
    UNUSED_VARIABLE(success);
}


void MineMeshManager::Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
                           const BasicPointLight& fillLight, const BasicPointLight& ballLight) {

    glPushAttrib(GL_COLOR_BUFFER_BIT);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    for (MineInstanceMapConstIter iter = this->mineInstanceMap.begin(); iter != this->mineInstanceMap.end(); ++iter) {
        MineInstance* mineInstance = iter->second;
        mineInstance->Draw(dT, camera, keyLight, fillLight, ballLight, this->mineMesh);
    }
    glPopAttrib();
}

void MineMeshManager::MineInstance::Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
                                         const BasicPointLight& fillLight, const BasicPointLight& ballLight, Mesh* mineMesh) {
    glPushMatrix();

    const Point2D& position = this->mine->GetPosition();
    glTranslatef(position[0], position[1], this->mine->GetHalfWidth());

    float currRotation = this->mine->GetCurrentRotation();
    glRotatef(currRotation, 1, 0, 0);
    glRotatef(0.75f * currRotation, 0, 1, 0);
    
    float visualScale = this->mine->GetVisualScaleFactor();
    glScalef(visualScale, visualScale, visualScale);
    
    mineMesh->Draw(camera, keyLight, fillLight, ballLight);

    glPopMatrix();
}
