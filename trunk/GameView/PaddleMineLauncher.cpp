/**
 * PaddleMineLauncher.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "PaddleMineLauncher.h"
#include "GameViewConstants.h"

#include "../GameModel/PaddleMineProjectile.h"

PaddleMineLauncher::PaddleMineLauncher() : paddleMineAttachmentMesh(NULL), mineMesh(NULL) {
    this->loadingMineAnim.SetInterpolantValue(0.0f);
    this->loadingMineAnim.SetLerp(PlayerPaddle::PADDLE_MINE_LAUNCH_DELAY*0.9, MineProjectile::HEIGHT_DEFAULT);

    this->paddleMineAttachmentMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->PADDLE_MINE_ATTACHMENT_MESH);
    assert(this->paddleMineAttachmentMesh != NULL);
    this->mineMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->MINE_MESH);
    assert(this->mineMesh != NULL);
}

PaddleMineLauncher::~PaddleMineLauncher() {
    bool success = false;
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->paddleMineAttachmentMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->mineMesh);
    assert(success);
    UNUSED_VARIABLE(success);
}

void PaddleMineLauncher::FireMine() {
    this->loadingMineAnim.ResetToStart();
}