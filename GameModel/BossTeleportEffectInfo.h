/**
 * BossTeleportEffectInfo.h
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

#ifndef __BOSSTELEPORTEFFECTINFO_H__
#define __BOSSTELEPORTEFFECTINFO_H__

#include "BossEffectEventInfo.h"

class BossTeleportEffectInfo : public BossEffectEventInfo {
public:
    BossTeleportEffectInfo(const Point2D& originalPos, const Point2D& teleportPos) : BossEffectEventInfo(),
        originalPos(originalPos), teleportPos(teleportPos) {}

    ~BossTeleportEffectInfo() {}

    BossEffectEventInfo::Type GetType() const { return BossEffectEventInfo::TeleportInfo; }
    
    const Point2D& GetOriginalPosition() const { return this->originalPos; }
    const Point2D& GetTeleportPosition() const { return this->teleportPos; } 

private:
    const Point2D originalPos;
    const Point2D teleportPos;

    DISALLOW_COPY_AND_ASSIGN(BossTeleportEffectInfo);
};

#endif // __BOSSTELEPORTEFFECTINFO_H__