/**
 * BossLaserBeam.cpp
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

#include "BossLaserBeam.h"
#include "GameEventManager.h"

const int BossLaserBeam::BASE_DAMAGE_PER_SECOND = 0;    // Damage per second that the beam does to blocks and stuff																															// NOTE: a typical block has about 100 life

BossLaserBeam::BossLaserBeam(const GameModel* gameModel, const Collision::Ray2D& initialBeamRay, 
                             float initialBeamRadius, double expireTimeInSecs) : 
Beam(BossLaserBeam::BASE_DAMAGE_PER_SECOND, expireTimeInSecs), 
colour(GameModelConstants::GetInstance()->BOSS_LASER_BEAM_COLOUR),
initialBeamRay(initialBeamRay), initialBeamRadius(initialBeamRadius) {
	this->UpdateCollisions(gameModel);
}

BossLaserBeam::~BossLaserBeam() {
}

void BossLaserBeam::UpdateCollisions(const GameModel* gameModel) {
    assert(gameModel != NULL);

    std::list<BeamSegment*> initialBeamSegs;
    initialBeamSegs.push_back(new BeamSegment(this->initialBeamRay, this->beamAlpha * this->initialBeamRadius, 
        this->beamAlpha * BASE_DAMAGE_PER_SECOND, NULL));

    this->BuildAndUpdateCollisionsForBeamParts(initialBeamSegs, gameModel);
}
