/**
 * BossBallBeamUpdateStrategy.h
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

#ifndef __BOSSBALLBEAMUPDATESTRATEGY_H__
#define __BOSSBALLBEAMUPDATESTRATEGY_H__

#include "EffectUpdateStrategy.h"
#include "../ESPEngine/ESPPointToPointBeam.h"
#include "../GameModel/GameModel.h"
#include "../GameModel/BossBodyPart.h"

class BossBallBeamUpdateStrategy : public EffectUpdateStrategy {
public:
    BossBallBeamUpdateStrategy(const BossBodyPart* bodyPart) : bodyPart(bodyPart), isDead(false) {
        assert(bodyPart != NULL);
        this->prevBossPos = bodyPart->GetTranslationPt2D();
    }
    BossBallBeamUpdateStrategy(const BossBodyPart* bodyPart, std::list<ESPPointToPointBeam*>& beams) : 
      bodyPart(bodyPart), beams(beams), isDead(false) {
        
        assert(bodyPart != NULL);
        this->prevBossPos = bodyPart->GetTranslationPt2D();
    }

    ~BossBallBeamUpdateStrategy() {
        for (std::list<ESPPointToPointBeam*>::iterator iter = this->beams.begin(); iter != this->beams.end(); ++iter) {
            delete (*iter);
        }
        this->beams.clear();
        for (std::list<ESPPointEmitter*>::iterator iter = this->ptEmitters.begin(); iter != this->ptEmitters.end(); ++iter) {
            delete (*iter);
        }
        this->ptEmitters.clear();
        for (std::list<ESPPointEmitter*>::iterator iter = this->ballEmitters.begin(); iter != this->ballEmitters.end(); ++iter) {
            delete (*iter);
        }
        this->ballEmitters.clear();
    }

    void AddBeamEmitter(ESPPointToPointBeam* beam) { 
        assert(beam != NULL);
        this->beams.push_back(beam);
    }
    void AddPointEmitter(ESPPointEmitter* ptEmitter) {
        assert(ptEmitter != NULL);
        this->ptEmitters.push_back(ptEmitter);
    }
    void AddBallEmitter(ESPPointEmitter* ptEmitter) {
        assert(ptEmitter != NULL);
        this->ballEmitters.push_back(ptEmitter);
    }

    void TickAndDraw(double dT, const Camera& camera, const GameModel& gameModel);
    bool IsDead() const { return (this->beams.empty() && this->ptEmitters.empty() && this->ballEmitters.empty()) || this->isDead; }

private:
    Point2D prevBossPos;
    bool isDead;
    const BossBodyPart* bodyPart;
    std::list<ESPPointToPointBeam*> beams;
    std::list<ESPPointEmitter*> ptEmitters;
    std::list<ESPPointEmitter*> ballEmitters;

};

inline void BossBallBeamUpdateStrategy::TickAndDraw(double dT, const Camera& camera,
                                                    const GameModel& gameModel) {
    
    
    // Check to see if the ball is no longer stopped, in which case this the effect is isDead...
    if (gameModel.GetGameBalls().empty() || !gameModel.GetGameBalls().front()->GetDirection().IsZero() ||
        gameModel.GetCurrentStateType() != GameState::BallInPlayStateType) {

        this->isDead = true;
        return;
    }

    Point3D bodyPartPos3D = this->bodyPart->GetTranslationPt3D();
    Point2D bodyPartPos2D = bodyPartPos3D.ToPoint2D();

    // If the body part has changed positions then the teleport has happened...
    if ((bodyPartPos2D - this->prevBossPos).SqrMagnitude() > LevelPiece::PIECE_WIDTH*LevelPiece::PIECE_WIDTH) {
        static const float LIFETIME_REMAINING = 1.3;
        for (std::list<ESPPointEmitter*>::iterator iter = this->ptEmitters.begin(); iter != this->ptEmitters.end(); ++iter) {
            (*iter)->SetParticleLife(ESPInterval(LIFETIME_REMAINING), true);
        }
        for (std::list<ESPPointEmitter*>::iterator iter = this->ballEmitters.begin(); iter != this->ballEmitters.end(); ++iter) {
            (*iter)->SetParticleLife(ESPInterval(LIFETIME_REMAINING), true);
        }
        // Fade the beam
        for (std::list<ESPPointToPointBeam*>::iterator iter = this->beams.begin(); iter != this->beams.end(); ++iter) {
            (*iter)->SetRemainingBeamLifetimeMax(LIFETIME_REMAINING);
        }
    }

    const GameBall& ball = *gameModel.GetGameBalls().front();

    glPushMatrix();
    glTranslatef(bodyPartPos3D[0], bodyPartPos3D[1], bodyPartPos3D[2]);
    for (std::list<ESPPointEmitter*>::iterator iter = this->ptEmitters.begin(); iter != this->ptEmitters.end();) {
        ESPPointEmitter* currEmitter = *iter;

        // Check to see if dead, if so erase it...
        if (currEmitter->IsDead()) {
            iter = this->ptEmitters.erase(iter);
            delete currEmitter;
            currEmitter = NULL;
        }
        else {
            // Not dead yet so we draw and tick - transform to the body part to draw
            currEmitter->Tick(dT);
            currEmitter->Draw(camera);
            ++iter;
        }
    }
    glPopMatrix();

    glPushMatrix();
    glTranslatef(ball.GetCenterPosition2D()[0], ball.GetCenterPosition2D()[1], 0);
    for (std::list<ESPPointEmitter*>::iterator iter = this->ballEmitters.begin(); iter != this->ballEmitters.end();) {
        ESPPointEmitter* currEmitter = *iter;

        // Check to see if dead, if so erase it...
        if (currEmitter->IsDead()) {
            iter = this->ballEmitters.erase(iter);
            delete currEmitter;
            currEmitter = NULL;
        }
        else {
            // Not dead yet so we draw and tick - transform to the body part to draw
            currEmitter->Tick(dT);
            currEmitter->Draw(camera);
            ++iter;
        }
    }
    glPopMatrix();

    for (std::list<ESPPointToPointBeam*>::iterator iter = this->beams.begin(); iter != this->beams.end();) {
        ESPPointToPointBeam* currBeam = *iter;

        // Check to see if isDead, if so erase it...
        if (currBeam->IsDead()) {
            iter = this->beams.erase(iter);
            delete currBeam;
            currBeam = NULL;
        }
        else {
            currBeam->SetStartAndEndPoints(bodyPartPos2D, ball.GetCenterPosition2D());
            currBeam->Tick(dT);
            currBeam->Draw(camera);
            ++iter;
        }
    }

    this->prevBossPos = bodyPartPos2D;
}

#endif // __BOSSBALLBEAMUPDATESTRATEGY_H__