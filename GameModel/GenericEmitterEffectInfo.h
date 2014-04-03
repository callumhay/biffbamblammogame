/**
 * GenericEmitterEffectInfo.h
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

#ifndef __GENERICEMITTEREFFECTTINFO_H__
#define __GENERICEMITTEREFFECTTINFO_H__

#include "GeneralEffectEventInfo.h"
#include "../ESPEngine/ESPAbstractEmitter.h"

class GenericEmitterEffectInfo : public GeneralEffectEventInfo {
public:
    GenericEmitterEffectInfo(const std::vector<ESPAbstractEmitter*>& emitters) : emitters(emitters) {
    }
    ~GenericEmitterEffectInfo() {
        for (std::vector<ESPAbstractEmitter*>::iterator iter = this->emitters.begin(); iter != this->emitters.end(); ++iter) {
            ESPAbstractEmitter* emitter = *iter;
            delete emitter;
            emitter = NULL;
        }
        this->emitters.clear();
    }

    GeneralEffectEventInfo::Type GetType() const { return GeneralEffectEventInfo::GenericEmitter; }

    // Caller takes ownership of all the memory!
    void TakeEmitters(std::vector<ESPAbstractEmitter*>& emitters) const { 
        emitters = this->emitters;
        this->emitters.clear();
    }

private:
    mutable std::vector<ESPAbstractEmitter*> emitters; // All of these are not owned by this, 
                                                       // they will be handed off to the GameView on arrival

    DISALLOW_COPY_AND_ASSIGN(GenericEmitterEffectInfo);
};

#endif // __GENERICEMITTEREFFECTTINFO_H__