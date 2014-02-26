/**
 * PaddleMineLauncher.cpp
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

#include "PaddleMineLauncher.h"
#include "GameViewConstants.h"

#include "../GameModel/PlayerPaddle.h"

PaddleMineLauncher::PaddleMineLauncher() : paddleMineAttachment(NULL), isActive(false) {
    this->paddleMineAttachment = ResourceManager::GetInstance()->GetObjMeshResource(
        GameViewConstants::GetInstance()->PADDLE_MINE_ATTACHMENT_MESH);
    assert(this->paddleMineAttachment != NULL);
    
    this->pulseAlphaAnim.SetLerp(0.0, 1.0, 2.0, 1.0f, 0.75f, 1.0f);
    this->pulseAlphaAnim.SetRepeat(true);

    this->Deactivate();
}

PaddleMineLauncher::~PaddleMineLauncher() {
    bool success = false;
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->paddleMineAttachment);
    assert(success);
    UNUSED_VARIABLE(success);
}

void PaddleMineLauncher::Activate() {
    if (this->isActive) { return; }

    this->flickerAnim.ClearLerp();
    this->flickerAnim.SetInterpolantValue(1.0f);
    this->pulseAlphaAnim.ResetToStart();
    this->isActive = true;
}

void PaddleMineLauncher::Deactivate() {
    if (!this->isActive) { return; }
    this->isActive = false;
}

void PaddleMineLauncher::FireMine() {

    std::vector<double> timeVals;
    timeVals.reserve(8);
    timeVals.push_back(0.0);
    timeVals.push_back(0.08 * PlayerPaddle::PADDLE_MINE_LAUNCH_DELAY);
    timeVals.push_back(0.13 * PlayerPaddle::PADDLE_MINE_LAUNCH_DELAY);
    timeVals.push_back(0.17 * PlayerPaddle::PADDLE_MINE_LAUNCH_DELAY);
    timeVals.push_back(0.28 * PlayerPaddle::PADDLE_MINE_LAUNCH_DELAY);
    timeVals.push_back(0.37 * PlayerPaddle::PADDLE_MINE_LAUNCH_DELAY);
    timeVals.push_back(0.4 * PlayerPaddle::PADDLE_MINE_LAUNCH_DELAY);
    timeVals.push_back(PlayerPaddle::PADDLE_MINE_LAUNCH_DELAY);

    std::vector<float> alphaVals;
    alphaVals.reserve(timeVals.size());
    alphaVals.push_back(0.0f);
    alphaVals.push_back(0.5f);
    alphaVals.push_back(0.25f);
    alphaVals.push_back(0.75f);
    alphaVals.push_back(0.15f);
    alphaVals.push_back(0.85f);
    alphaVals.push_back(0.05f);
    alphaVals.push_back(1.0f);

    this->flickerAnim.SetLerp(timeVals, alphaVals);
    this->flickerAnim.SetRepeat(false);
}

void PaddleMineLauncher::DrawEffects(double dT, const PlayerPaddle& paddle) {

    this->flickerAnim.Tick(dT);
    this->pulseAlphaAnim.Tick(dT);

    if (paddle.HasPaddleType(PlayerPaddle::RocketPaddle | PlayerPaddle::RemoteControlRocketPaddle | PlayerPaddle::InvisiPaddle)) {
        return;
    }

    float alpha = this->flickerAnim.GetInterpolantValue() * this->pulseAlphaAnim.GetInterpolantValue() * paddle.GetAlpha();

    float negHalfPaddleDepth      = -paddle.GetHalfDepthTotal();
    float halfBigHighlightWidth   = 0.3f * paddle.GetHalfFlatTopWidth();
    float halfSmallHighlightWidth = 0.5f * halfBigHighlightWidth;
    float highlightHeight         = 3.5f * LevelPiece::PIECE_HEIGHT * paddle.GetPaddleScaleFactor();

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDepthMask(GL_FALSE); // We don't want outlines
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glTranslatef(0, 0, negHalfPaddleDepth);
    glRotatef(0,0,1, paddle.GetZRotation());

    glBegin(GL_QUADS);

    // Big highlight
    static const float BIG_HIGHLIGHT_VAL = 0.75f;
    glColor4f(BIG_HIGHLIGHT_VAL, BIG_HIGHLIGHT_VAL, BIG_HIGHLIGHT_VAL, 0.4f*alpha);
    glVertex3f(-halfBigHighlightWidth, 0, 0);
    glVertex3f(halfBigHighlightWidth, 0, 0);
    glColor4f(1, 1, 1, 0.0f);
    glVertex3f(halfBigHighlightWidth,  highlightHeight, 0);
    glVertex3f(-halfBigHighlightWidth, highlightHeight, 0);

    // Highlight behind the lasers
    static const float SMALL_HIGHLIGHT_VAL = 1.0f;
    glColor4f(SMALL_HIGHLIGHT_VAL, SMALL_HIGHLIGHT_VAL, SMALL_HIGHLIGHT_VAL, 0.4f*alpha);
    glVertex3f(-halfSmallHighlightWidth, 0, 0);
    glVertex3f(halfSmallHighlightWidth, 0, 0);
    glColor4f(1, 1, 1, 0.0f);
    glVertex3f(halfSmallHighlightWidth,  highlightHeight, 0);
    glVertex3f(-halfSmallHighlightWidth, highlightHeight, 0);

    glEnd();

    glPopMatrix();
    glPopAttrib();
}