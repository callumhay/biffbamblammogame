/**
 * SummonPortalsEffectInfo.h
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

#ifndef __SUMMONPORTALSEFFECTINFO_H__
#define __SUMMONPORTALSEFFECTINFO_H__

#include "BossEffectEventInfo.h"

class SummonPortalsEffectInfo : public BossEffectEventInfo {
public:
    SummonPortalsEffectInfo(const BossBodyPart* part, float size, double timeInSecs,
        const Point2D& portal1Pos, const Point2D& portal2Pos, const Colour& portalColour,
        const Vector3D& offset = Vector3D(0,0,0)) : 
      BossEffectEventInfo(), bodyPart(part), size(size), timeInSecs(timeInSecs), 
      portal1Pos(portal1Pos), portal2Pos(portal2Pos), portalColour(portalColour), offset(offset) {}

    ~SummonPortalsEffectInfo() {}

    BossEffectEventInfo::Type GetType() const { return BossEffectEventInfo::SummonPortalsInfo; }

    const BossBodyPart* GetBodyPart() const { return this->bodyPart; }
    double GetTimeInSecs() const { return this->timeInSecs; }
    float GetSize() const { return this->size; }
    const Point2D& GetPortal1Pos() const { return this->portal1Pos; }
    const Point2D& GetPortal2Pos() const { return this->portal2Pos; }
    const Colour& GetPortalColour() const { return this->portalColour; }
    const Vector3D& GetOffset() const { return this->offset; }

private:
    const BossBodyPart* bodyPart;
    const float size;
    const double timeInSecs;
    const Point2D portal1Pos;
    const Point2D portal2Pos;
    const Colour portalColour;
    const Vector3D offset;

    DISALLOW_COPY_AND_ASSIGN(SummonPortalsEffectInfo);
};

#endif // __SUMMONPORTALSEFFECTINFO_H__