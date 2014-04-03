/**
 * BossBodyPartEmitterUpdateStrategy.h
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

#ifndef __BOSSBODYPARTEMITTERUPDATESTRATEGY_H__
#define __BOSSBODYPARTEMITTERUPDATESTRATEGY_H__

#include "EffectUpdateStrategy.h"
#include "../ESPEngine/ESPAbstractEmitter.h"
#include "../GameModel/BossBodyPart.h"

class BossBodyPartEmitterUpdateStrategy : public EffectUpdateStrategy {
public:
    BossBodyPartEmitterUpdateStrategy(const BossBodyPart* bodyPart) : bodyPart(bodyPart), offset(0,0,0) {
        assert(bodyPart != NULL);
    }
    BossBodyPartEmitterUpdateStrategy(const BossBodyPart* bodyPart, std::list<ESPAbstractEmitter*>& emitters) : 
      bodyPart(bodyPart), emitters(emitters), offset(0,0,0) {
        
        assert(bodyPart != NULL);
    }

    ~BossBodyPartEmitterUpdateStrategy() {
        for (std::list<ESPAbstractEmitter*>::iterator iter = this->emitters.begin(); iter != this->emitters.end(); ++iter) {
            delete (*iter);
        }
        this->emitters.clear();
    }

    void AddEmitter(ESPAbstractEmitter* emitter) { 
        assert(emitter != NULL);
        this->emitters.push_back(emitter);
    }

    void SetOffset(const Vector3D& offset) {
        this->offset = offset;
    }

    void TickAndDraw(double dT, const Camera& camera);
    bool IsDead() const { return this->emitters.empty(); }

private:
    const BossBodyPart* bodyPart;
    Vector3D offset;
    std::list<ESPAbstractEmitter*> emitters;
};

inline void BossBodyPartEmitterUpdateStrategy::TickAndDraw(double dT, const Camera& camera) {
    
    Point3D bodyPartPos = this->bodyPart->GetTranslationPt3D() + this->offset;

    glPushMatrix();
    glTranslatef(bodyPartPos[0], bodyPartPos[1], bodyPartPos[2]);

    for (std::list<ESPAbstractEmitter*>::iterator iter = this->emitters.begin(); iter != this->emitters.end();) {
        ESPAbstractEmitter* currEmitter = *iter;

        // Check to see if dead, if so erase it...
        if (currEmitter->IsDead()) {
            iter = this->emitters.erase(iter);
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
}

#endif // __BOSSBODYPARTEMITTERUPDATESTRATEGY_H__