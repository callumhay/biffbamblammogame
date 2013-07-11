/**
 * PaddleLaserBeam.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
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

private:
	static const int BASE_DAMAGE_PER_SECOND;

	bool reInitStickyPaddle;	// Indicates whether the beam should reinitialize its sticky paddle beams

	PlayerPaddle* paddle;

    DISALLOW_COPY_AND_ASSIGN(PaddleLaserBeam);
};

inline const Colour& PaddleLaserBeam::GetBeamColour() const {
    return GameModelConstants::GetInstance()->PADDLE_LASER_BEAM_COLOUR;
}

#endif // __PADDLELASERBEAM_H__