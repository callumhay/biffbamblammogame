/**
 * SparkBurstEffectInfo.h
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

#ifndef __SPARKBURSTEFFECTINFO_H__
#define __SPARKBURSTEFFECTINFO_H__

#include "BossEffectEventInfo.h"

class SparkBurstEffectInfo : public BossEffectEventInfo {
public:
    SparkBurstEffectInfo(const BossBodyPart* part, double timeInSecs, const Colour& colour, const Vector3D& offset = Vector3D(0,0,0)) : 
      BossEffectEventInfo(), part(part), timeInSecs(timeInSecs), colour(colour), offset(offset) {}
    ~SparkBurstEffectInfo() {}

    BossEffectEventInfo::Type GetType() const { return BossEffectEventInfo::SparkBurstInfo; }

    const BossBodyPart* GetPart() const { return this->part; }
    double GetTimeInSecs() const { return this->timeInSecs; }
    const Colour& GetColour() const { return this->colour; }
    const Vector3D& GetOffset() const { return this->offset; }

private:
    const BossBodyPart* part;
    const double timeInSecs;
    const Colour colour;
    const Vector3D offset;

    DISALLOW_COPY_AND_ASSIGN(SparkBurstEffectInfo);
};

#endif // __SPARKBURSTEFFECTINFO_H__