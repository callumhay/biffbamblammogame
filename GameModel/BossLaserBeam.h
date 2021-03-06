/**
 * BossLaserBeam.h
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

#ifndef __BOSSLASERBEAM_H__
#define __BOSSLASERBEAM_H__

#include "Beam.h"
#include "GameModelConstants.h"
#include "GameEventManager.h"

class BossLaserBeam : public Beam {
public:
    static const double BEAM_EXPIRE_TIME_IN_SECONDS;

    BossLaserBeam(const GameModel* gameModel, const Collision::Ray2D& initialBeamRay, 
        float initialBeamRadius, double expireTimeInSecs);
    ~BossLaserBeam();

    Beam::BeamType GetType() const { return Beam::BossBeam; }
    bool CanDestroyLevelPieces() const { return false; }

    void UpdateCollisions(const GameModel* gameModel);
    int GetNumBaseBeamSegments() const;
    
    void SetBeamColour(const Colour& c);
    const Colour& GetBeamColour() const;

    void UpdateOriginBeamSegment(const GameModel* gameModel, const Collision::Ray2D& newOriginBeamRay);
    const Collision::Ray2D& GetInitialBeamRay() const;

private:
    static const int BASE_DAMAGE_PER_SECOND;

    Collision::Ray2D initialBeamRay;
    float initialBeamRadius;
    Colour colour;

    DISALLOW_COPY_AND_ASSIGN(BossLaserBeam);
};

inline int BossLaserBeam::GetNumBaseBeamSegments() const {
    return 1; // Only one base ray for any boss laser beam
}

inline void BossLaserBeam::SetBeamColour(const Colour& c) {
    this->colour = c;
}

inline const Colour& BossLaserBeam::GetBeamColour() const {
    return this->colour;
}

inline void BossLaserBeam::UpdateOriginBeamSegment(const GameModel* gameModel, 
                                                   const Collision::Ray2D& newOriginBeamRay) {
    this->initialBeamRay = newOriginBeamRay;
    this->UpdateCollisions(gameModel);
}

inline const Collision::Ray2D& BossLaserBeam::GetInitialBeamRay() const {
    return this->initialBeamRay;
}

#endif // __BOSSLASERBEAM_H__