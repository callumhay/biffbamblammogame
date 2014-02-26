/**
 * PaddleBeamAttachment.cpp
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

#include "PaddleBeamAttachment.h"
#include "GameViewConstants.h"

#include "../GameModel/GameModelConstants.h"
#include "../GameModel/PaddleLaserBeam.h"

#include "../ResourceManager.h"

const float PaddleBeamAttachment::BIG_TO_SMALL_RAY_RATIO = 0.5f;

PaddleBeamAttachment::PaddleBeamAttachment() : paddleBeamAttachmentMesh(NULL),
DARK_WASHED_OUT_BEAM_COLOUR(0.5f * 1.5f * GameModelConstants::GetInstance()->PADDLE_LASER_BEAM_COLOUR),
BRIGHT_WASHED_OUT_BEAM_COLOUR(1.5f * GameModelConstants::GetInstance()->PADDLE_LASER_BEAM_COLOUR){

    this->paddleBeamAttachmentMesh = ResourceManager::GetInstance()->GetObjMeshResource(
        GameViewConstants::GetInstance()->PADDLE_BEAM_ATTACHMENT_MESH);
    assert(this->paddleBeamAttachmentMesh != NULL);
}

PaddleBeamAttachment::~PaddleBeamAttachment() {
    bool success = false;
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->paddleBeamAttachmentMesh);
    assert(success);
    UNUSED_VARIABLE(success);
}

void PaddleBeamAttachment::DrawEffects(const PlayerPaddle& paddle) {
    if (paddle.GetIsLaserBeamFiring() ||
        paddle.HasPaddleType(PlayerPaddle::RocketPaddle | PlayerPaddle::RemoteControlRocketPaddle | PlayerPaddle::InvisiPaddle)) {
        return;
    }

    float alpha = paddle.GetAlpha();
    float rayLength = 5.0f * LevelPiece::PIECE_HEIGHT * paddle.GetPaddleScaleFactor();

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDepthMask(GL_FALSE); // We don't want outlines
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glTranslatef(0, 0, -paddle.GetHalfDepthTotal());
    glRotatef(0,0,1, paddle.GetZRotation());

    

    // Draw the highlight pre-beam(s) coming out of the paddle...

    // Deal with the special case where the paddle is sticky...
    if (paddle.HasPaddleType(PlayerPaddle::StickyPaddle)) {

        float beamSpacing = PaddleLaserBeam::GetStickyPaddleOriginBeamSpacing(paddle);

        Vector2D centerVec, leftVec, rightVec;
        float centerSize, leftSize, rightSize;
        StickyPaddleBeamDirGenerator::GetBeamValues(Vector2D(0,1), centerVec, leftVec, rightVec, centerSize, leftSize, rightSize);

        // There are 3 origin beams...

        // Center beam...
        this->DrawHighlightQuadVertices(alpha, centerVec, 0.5f*centerSize, rayLength);
       
        glPushMatrix();

         // Left beam...
        glTranslatef(-beamSpacing, 0, 0);
        this->DrawHighlightQuadVertices(alpha, leftVec, 0.5f*leftSize, rayLength);

        // Right beam...
        glTranslatef(2*beamSpacing, 0, 0);
        this->DrawHighlightQuadVertices(alpha, rightVec, 0.5f*rightSize, rayLength);

        glPopMatrix();
    }
    else {
        // There's just one origin beam...
        this->DrawHighlightQuadVertices(alpha, Vector2D(0,1), PaddleLaserBeam::GetInitialBeamRadius(paddle), rayLength);
    }
    glPopMatrix();
    glPopAttrib();
}

void PaddleBeamAttachment::DrawHighlightQuadVertices(float alpha, const Vector2D& rayUpDir, 
                                                     float halfOuterRayWidth, float rayLength) {

    const float halfInnerRayWidth = PaddleBeamAttachment::BIG_TO_SMALL_RAY_RATIO * halfOuterRayWidth;
    const Vector2D rayRightDir(rayUpDir[1], -rayUpDir[0]);

    const Vector2D rayUpVec = rayLength*rayUpDir;
    const Vector2D rayOuterRightVec = halfOuterRayWidth*rayRightDir;
    const Vector2D rayInnerRightVec = halfInnerRayWidth*rayRightDir;
    
    glBegin(GL_QUADS);

    glColor4f(DARK_WASHED_OUT_BEAM_COLOUR.R(), DARK_WASHED_OUT_BEAM_COLOUR.G(), DARK_WASHED_OUT_BEAM_COLOUR.B(), 0.6f*alpha);
    glVertex3f(-halfOuterRayWidth, 0, 0);
    glVertex3f(halfOuterRayWidth, 0, 0);
    
    Vector2D topLeft  = rayUpVec - rayOuterRightVec;
    Vector2D topRight = rayUpVec + rayOuterRightVec;
    glColor4f(DARK_WASHED_OUT_BEAM_COLOUR.R(), DARK_WASHED_OUT_BEAM_COLOUR.G(), DARK_WASHED_OUT_BEAM_COLOUR.B(), 0.0f);
    glVertex3f(topRight[0], topRight[1], 0);
    glVertex3f(topLeft[0],  topLeft[1], 0);

    glColor4f(BRIGHT_WASHED_OUT_BEAM_COLOUR.R(), BRIGHT_WASHED_OUT_BEAM_COLOUR.G(), BRIGHT_WASHED_OUT_BEAM_COLOUR.B(), 0.6f*alpha);
    glVertex3f(-halfInnerRayWidth, 0, 0);
    glVertex3f(halfInnerRayWidth, 0, 0);
    
    topLeft  = rayUpVec - rayInnerRightVec;
    topRight = rayUpVec + rayInnerRightVec;
    glColor4f(BRIGHT_WASHED_OUT_BEAM_COLOUR.R(), BRIGHT_WASHED_OUT_BEAM_COLOUR.G(), BRIGHT_WASHED_OUT_BEAM_COLOUR.B(), 0.0f);
    glVertex3f(topRight[0], topRight[1], 0);
    glVertex3f(topLeft[0], topLeft[1], 0);

    glEnd();
}