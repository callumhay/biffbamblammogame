/**
 * StarSmashEffectInfo.h
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

#ifndef __STARSMASHEFFECTINFO_H__
#define __STARSMASHEFFECTINFO_H__

#include "GeneralEffectEventInfo.h"
#include "Onomatoplex.h"

class StarSmashEffectInfo : public GeneralEffectEventInfo {
public:
    StarSmashEffectInfo(const Point2D& position, const Vector2D& dir, double size, double timeInSecs,
        Onomatoplex::Extremeness extremeness): position(position), direction(dir), size(size),
        timeInSecs(timeInSecs), extremeness(extremeness) {};

    ~StarSmashEffectInfo() {};

    const Point2D& GetPosition() const { return this->position; }
    const Vector2D& GetDirection() const { return this->direction; }
    double GetSize() const { return this->size; }
    double GetTimeInSecs() const { return this->timeInSecs; }
    Onomatoplex::Extremeness GetExtremeness() const { return this->extremeness; }

    GeneralEffectEventInfo::Type GetType() const { return GeneralEffectEventInfo::StarSmash; }

private:
    const Point2D position;     // Position in level space where the effect should occur
    const Vector2D direction;
    const double size;
    const double timeInSecs;
    const Onomatoplex::Extremeness extremeness;

    DISALLOW_COPY_AND_ASSIGN(StarSmashEffectInfo);
};

#endif // __STARSMASHEFFECTINFO_H__