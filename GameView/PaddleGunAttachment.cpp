/**
 * PaddleGunAttachment.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2014
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "PaddleGunAttachment.h"
#include "GameViewConstants.h"

#include "../ResourceManager.h"

PaddleGunAttachment::PaddleGunAttachment() : gunMesh(NULL) {
	this->gunRecoilAnim.SetInterpolantValue(0.0f);
	this->gunMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->PADDLE_GUN_ATTACHMENT_MESH);
}

PaddleGunAttachment::~PaddleGunAttachment() {
    bool success = false;
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->gunMesh);
    assert(success);
    UNUSED_VARIABLE(success);
}

/**
 * Cause the laser paddle attachment to animate - like it's reacting to shooting
 * a bullet ... or a laser?!
 */
void PaddleGunAttachment::FirePaddleGun(const PlayerPaddle& paddle) {
    // Setup any animations for the laser attachment
    std::vector<float> translateVals;
    translateVals.reserve(3);
    translateVals.push_back(0);
    translateVals.push_back(paddle.GetHalfHeight());
    translateVals.push_back(0);
    std::vector<double> timeVals;
    timeVals.reserve(3);
    timeVals.push_back(0);
    timeVals.push_back(0.1f);
    timeVals.push_back(0.2f);
    this->gunRecoilAnim.SetLerp(timeVals, translateVals);
}