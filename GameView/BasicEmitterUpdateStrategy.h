/**
 * BasicEmitterUpdateStrategy.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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

#ifndef __BASICEMITTERUPDATESTRATEGY_H__
#define __BASICEMITTERUPDATESTRATEGY_H__

#include "EffectUpdateStrategy.h"
#include "../ESPEngine/ESPAbstractEmitter.h"

class BasicEmitterUpdateStrategy : public EffectUpdateStrategy {
public:

    BasicEmitterUpdateStrategy() {}
    explicit BasicEmitterUpdateStrategy(ESPAbstractEmitter* emitter) {
        assert(emitter != NULL);
        this->AddEmitter(emitter);
    }
    ~BasicEmitterUpdateStrategy() {
        for (std::list<ESPAbstractEmitter*>::iterator iter = this->emitters.begin(); iter != this->emitters.end(); ++iter) {
            delete (*iter);
        }
        this->emitters.clear();
    }

    void AddEmitter(ESPAbstractEmitter* emitter) { 
        assert(emitter != NULL);
        this->emitters.push_back(emitter);
    }

    void TickAndDraw(double dT, const Camera& camera, const GameModel& gameModel);
    bool IsDead() const { return this->emitters.empty(); }

private:
    std::list<ESPAbstractEmitter*> emitters;
};

inline void BasicEmitterUpdateStrategy::TickAndDraw(double dT, const Camera& camera,
                                                    const GameModel&) {

    for (std::list<ESPAbstractEmitter*>::iterator iter = this->emitters.begin(); 
         iter != this->emitters.end();) {

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
}

#endif // __BASICEMITTERUPDATESTRATEGY_H__