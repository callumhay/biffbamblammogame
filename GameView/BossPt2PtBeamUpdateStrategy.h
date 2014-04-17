/**
 * BossPt2PtBeamUpdateStrategy.h
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

#ifndef __BOSSPT2PTBEAMUPDATESTRATEGY_H__
#define __BOSSPT2PTBEAMUPDATESTRATEGY_H__

#include "EffectUpdateStrategy.h"
#include "../ESPEngine/ESPPointToPointBeam.h"
#include "../GameModel/BossBodyPart.h"

class BossPt2PtBeamUpdateStrategy : public EffectUpdateStrategy {
public:
    BossPt2PtBeamUpdateStrategy(const BossBodyPart* bodyPart) : bodyPart(bodyPart), offset(0,0,0) {
        assert(bodyPart != NULL);
    }
    BossPt2PtBeamUpdateStrategy(const BossBodyPart* bodyPart, std::list<ESPPointToPointBeam*>& beams) : 
      bodyPart(bodyPart), beams(beams), offset(0,0,0) {
        
        assert(bodyPart != NULL);
    }

    ~BossPt2PtBeamUpdateStrategy() {
        for (std::list<ESPPointToPointBeam*>::iterator iter = this->beams.begin(); iter != this->beams.end(); ++iter) {
            delete (*iter);
        }
        this->beams.clear();
    }

    void AddBeamEmitter(ESPPointToPointBeam* beam) { 
        assert(beam != NULL);
        this->beams.push_back(beam);
    }
    void SetOffset(const Vector3D& offset) {
        this->offset = offset;
    }

    void TickAndDraw(double dT, const Camera& camera, const GameModel& gameModel);
    bool IsDead() const { return this->beams.empty(); }

private:
    Vector3D offset;
    const BossBodyPart* bodyPart;
    std::list<ESPPointToPointBeam*> beams;
};

inline void BossPt2PtBeamUpdateStrategy::TickAndDraw(double dT, const Camera& camera,
                                                     const GameModel&) {
    
    Point3D bodyPartPos = this->bodyPart->GetTranslationPt3D() + this->offset;

    for (std::list<ESPPointToPointBeam*>::iterator iter = this->beams.begin(); iter != this->beams.end();) {
        ESPPointToPointBeam* currBeam = *iter;

        // Check to see if dead, if so erase it...
        if (currBeam->IsDead()) {
            iter = this->beams.erase(iter);
            delete currBeam;
            currBeam = NULL;
        }
        else {
            // Not dead yet so we draw and tick - transform to the body part to draw
            currBeam->SetStartPoint(bodyPartPos);
            currBeam->Tick(dT);
            currBeam->Draw(camera);
            ++iter;
        }
    }
}

#endif // __BOSSPT2PTBEAMUPDATESTRATEGY_H__