/**
 * BossLaserBeam.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __BOSSLASERBEAM_H__
#define __BOSSLASERBEAM_H__

#include "Beam.h"
#include "GameModelConstants.h"

class BossLaserBeam : public Beam {
public:
    static const double BEAM_EXPIRE_TIME_IN_SECONDS;

    BossLaserBeam(const Collision::Ray2D& initialBeamRay, float initialBeamRadius, const GameModel* gameModel);
    ~BossLaserBeam();

    Beam::BeamType GetType() const { return Beam::BossBeam; }
    bool CanDestroyLevelPieces() const { return false; }

    void UpdateCollisions(const GameModel* gameModel);
    int GetNumBaseBeamSegments() const;
    const Colour& GetBeamColour() const;

private:
    static const int BASE_DAMAGE_PER_SECOND;

    Collision::Ray2D initialBeamRay;
    float initialBeamRadius;

    DISALLOW_COPY_AND_ASSIGN(BossLaserBeam);
};

inline int BossLaserBeam::GetNumBaseBeamSegments() const {
    return 1; // Only one base ray for any boss laser beam
}

inline const Colour& BossLaserBeam::GetBeamColour() const {
    return GameModelConstants::GetInstance()->BOSS_LASER_BEAM_COLOUR;
}

#endif // __BOSSLASERBEAM_H__