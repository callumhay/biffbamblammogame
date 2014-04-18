/**
 * DebrisEffectInfo.h
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

#ifndef __DEBRISEFFECTINFO_H__
#define __DEBRISEFFECTINFO_H__

#include "BossEffectEventInfo.h"

class DebrisEffectInfo : public BossEffectEventInfo {
public:
    DebrisEffectInfo(const BossBodyPart* part, const Point2D& explosionCenter, const Colour& colour, 
        double minLifeOfDebrisInSecs, double maxLifeOfDebrisInSecs, int numDebrisBits, float sizeMultiplier = 1.0f,
        float forceMultiplier = 1.0f) :  BossEffectEventInfo(), part(part), explosionCenter(explosionCenter), colour(colour), 
          minLifeOfDebrisInSecs(minLifeOfDebrisInSecs), maxLifeOfDebrisInSecs(maxLifeOfDebrisInSecs), 
          numDebrisBits(numDebrisBits), sizeMultiplier(sizeMultiplier), forceMultiplier(forceMultiplier),
            overrideDirection(false) {

        assert(part != NULL);
        assert(minLifeOfDebrisInSecs > 0.0 && maxLifeOfDebrisInSecs >= minLifeOfDebrisInSecs);
        assert(numDebrisBits > 0);
    }

    ~DebrisEffectInfo() {}

    BossEffectEventInfo::Type GetType() const { return BossEffectEventInfo::DebrisInfo; }

    const BossBodyPart* GetPart() const { return this->part; }
    const Point2D& GetExplosionCenter() const { return this->explosionCenter; }
    const Colour& GetColour() const { return this->colour; }
    double GetMinLifeOfDebrisInSecs() const { return this->minLifeOfDebrisInSecs; }
    double GetMaxLifeOfDebrisInSecs() const { return this->maxLifeOfDebrisInSecs; }
    int GetNumDebrisBits() const { return this->numDebrisBits; }
    float GetSizeMultiplier() const { return this->sizeMultiplier; }
    float GetForceMultiplier() const { return this->forceMultiplier; }
    
    void OverrideDirection(const Vector3D& dir) {
        this->overrideDirection = true;
        this->overridenDir = dir;
        this->overridenDir.Normalize();
    }
    void OverrideDirection(const Vector2D& dir) {
        this->OverrideDirection(Vector3D(dir, 0));
    }
    bool IsDirectionOverriden() const { return this->overrideDirection; }
    const Vector3D& GetOverridenDirection() const { return this->overridenDir; }

private:
    const BossBodyPart* part;
    const Point2D explosionCenter;
    const Colour& colour;
    double minLifeOfDebrisInSecs;
    double maxLifeOfDebrisInSecs;
    const int numDebrisBits;
    float sizeMultiplier;
    float forceMultiplier;

    bool overrideDirection;
    Vector3D overridenDir;

    DISALLOW_COPY_AND_ASSIGN(DebrisEffectInfo);
};

#endif // __DEBRISEFFECTINFO_H__