/**
 * EnumBossEffectInfo.h
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

#ifndef __ENUMBOSSEFFECTINFO_H__
#define __ENUMBOSSEFFECTINFO_H__

#include "BossEffectEventInfo.h"

class EnumBossEffectInfo : public BossEffectEventInfo {
public:
    enum SpecificEffectType { 
        FrozenIceClouds,           // Icy clouds and vapour to indicate something was frozen
        IceBreak,                  // After being frozen, and a boss breaks free of it

        FuturismBossWarningFlare,  // A bright flash right before doing something mean to the player
        FuturismBossBeamEnergy,    // Energy streaks sucking into the boss as it fires a laser beam
        FuturismBossAttractorBeam, // Beam from the boss to the ball, that attracts the ball to the boss
        
    };

    EnumBossEffectInfo(SpecificEffectType specificType) : BossEffectEventInfo(),
        specificType(specificType), offset(0,0,0), size(1,1), bodyPart(NULL), timeInSecs(0.0) {}
    ~EnumBossEffectInfo() {}

    BossEffectEventInfo::Type GetType() const { return BossEffectEventInfo::EnumInfo; }
    SpecificEffectType GetSpecificType() const { return this->specificType; }

    void SetBodyPart(const BossBodyPart* bodyPart) { this->bodyPart = bodyPart; }
    const BossBodyPart* GetBodyPart() const { return this->bodyPart; }

    void SetOffset(const Vector3D& offset) { this->offset = offset; }
    const Vector3D& GetOffset() const { return this->offset; }

    void SetSize2D(const Vector2D& size) { this->size = size; }
    void SetSize1D(float size) { this->size[0] = this->size[1] = size; }
    const Vector2D& GetSize2D() const { return this->size; }
    float GetSize1D() const { return this->size[0]; }

    void SetDirection(const Vector2D& dir) { this->direction = Vector2D::Normalize(dir); }
    const Vector2D& GetDirection() const { return this->direction; }

    void SetTimeInSecs(double t) { this->timeInSecs = t; }
    double GetTimeInSecs() const { return this->timeInSecs; }

private:
    const SpecificEffectType specificType;

    const BossBodyPart* bodyPart;
    Vector3D offset;
    Vector2D size;
    Vector2D direction;
    double timeInSecs;

    DISALLOW_COPY_AND_ASSIGN(EnumBossEffectInfo);
};

#endif // __ENUMBOSSEFFECTINFO_H__