/**
 * PaddleLaserBeam.h
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

#ifndef __PADDLELASERBEAM_H__
#define __PADDLELASERBEAM_H__

#include "Beam.h"
#include "GameModelConstants.h"

class PaddleLaserBeam : public Beam {
public:
	static const double BEAM_EXPIRE_TIME_IN_SECONDS;

	PaddleLaserBeam(PlayerPaddle* paddle, const GameModel* gameModel);
	~PaddleLaserBeam();
	
    bool Tick(double dT, const GameModel* gameModel);
    void TickAlpha();

    Beam::BeamType GetType() const { return Beam::PaddleBeam; }
    bool CanDestroyLevelPieces() const { return true; }
    
	void UpdateCollisions(const GameModel* gameModel);
	int GetNumBaseBeamSegments() const;
    const Colour& GetBeamColour() const;

    static float GetStickyPaddleOriginBeamSpacing(const PlayerPaddle& paddle);
    static float GetInitialBeamRadius(const PlayerPaddle& paddle);
    
private:
	static const int BASE_DAMAGE_PER_SECOND;

	PlayerPaddle* paddle;

    DISALLOW_COPY_AND_ASSIGN(PaddleLaserBeam);
};

inline const Colour& PaddleLaserBeam::GetBeamColour() const {
    return GameModelConstants::GetInstance()->PADDLE_LASER_BEAM_COLOUR;
}

class StickyPaddleBeamDirGenerator {
public:
    static void ReinitializeBeams();
    static void GetBeamValues(const Vector2D& upVec, Vector2D& centerVec, Vector2D& leftVec, Vector2D& rightVec,
        float& centerSize, float& leftSize, float& rightSize);

private:
    static bool isInit;
    static int rotateCenterBeamAmt;
    static int rotateLeftBeamAmt;
    static int rotateRightBeamAmt;
    static float stickyPaddleOriginBeamSizeCenter;
    static float stickyPaddleOriginBeamSizeLeft;
    static float stickyPaddleOriginBeamSizeRight;

    StickyPaddleBeamDirGenerator() {}
    ~StickyPaddleBeamDirGenerator() {}
    DISALLOW_COPY_AND_ASSIGN(StickyPaddleBeamDirGenerator);
};



#endif // __PADDLELASERBEAM_H__