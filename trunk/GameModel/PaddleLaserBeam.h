/**
 * PaddleLaserBeam.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __PADDLELASERBEAM_H__
#define __PADDLELASERBEAM_H__

#include "Beam.h"

class PaddleLaserBeam : public Beam {
public:
	static const double BEAM_EXPIRE_TIME_IN_SECONDS;

	PaddleLaserBeam(PlayerPaddle* paddle, const GameLevel* level);
	~PaddleLaserBeam();
	
	void UpdateCollisions(const GameLevel* level);
	int GetNumBaseBeamSegments() const;

private:
	static const int BASE_DAMAGE_PER_SECOND;

	bool reInitStickyPaddle;	// Indicates whether the beam should reinitialize its sticky paddle beams

	PlayerPaddle* paddle;
};

#endif // __PADDLELASERBEAM_H__