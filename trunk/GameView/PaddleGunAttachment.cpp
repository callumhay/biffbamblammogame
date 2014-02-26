/**
 * PaddleGunAttachment.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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