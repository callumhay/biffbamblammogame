/**
 * LaserTurretBlock.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __LASERTURRETBLOCK_H__
#define __LASERTURRETBLOCK_H__

#include "TurretBlock.h"

class LaserTurretBlock : public TurretBlock {
public:
    static const float BALL_DAMAGE_AMOUNT;
    static const float ONE_MORE_BALL_HIT_LIFE_PERCENT;
    static const float FIRE_RATE_IN_BULLETS_PER_SEC;

    static const float BARREL_OFFSET_ALONG_X;
    static const float BARREL_OFFSET_ALONG_Y;
    static const float BARREL_OFFSET_ALONG_Z;
    static const float BARREL_OFFSET_EXTENT_ALONG_Y;
    
    enum TurretAIState { IdleTurretState = 0, SeekingTurretState, TargetFoundTurretState, TargetLostTurretState };
    enum BarrelAnimationState { OneForwardTwoBack = 0, OneFiringTwoReloading, TwoForwardOneBack, TwoFiringOneReloading };

	LaserTurretBlock(unsigned int wLoc, unsigned int hLoc);
	~LaserTurretBlock();

	LevelPieceType GetType() const { return LevelPiece::LaserTurret; }

	bool ProjectilePassesThrough(const Projectile* projectile) const;
    int GetPointsOnChange(const LevelPiece& changeToPiece) const;

	// Collision related stuffs
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);

    void AITick(double dT, GameModel* gameModel);

    void GetBarrel1ExtentPosInLocalSpace(Point3D& pos) const;
    void GetBarrel2ExtentPosInLocalSpace(Point3D& pos) const;

    float GetRotationDegreesFromX() const;
    float GetBarrel1RecoilAmount() const;
    float GetBarrel2RecoilAmount() const;

    LaserTurretBlock::TurretAIState GetTurretAIState() const;
    LaserTurretBlock::BarrelAnimationState GetBarrelState() const;

private:
    static const int POINTS_ON_BLOCK_DESTROYED  = 800;
    static const int PIECE_STARTING_LIFE_POINTS = 600;
    
    static const float MAX_ROTATION_SPEED_IN_DEGS_PER_SEC;
    static const float ROTATION_ACCEL_IN_DEGS_PER_SEC_SQRD;
    static const float BARREL_RECOIL_TRANSLATION_AMT;

    
    static const float BARREL_RELOAD_TIME;
    static const float BARREL_RECOIL_TIME;

    static const double LOST_AND_FOUND_MIN_SEEK_TIME;
    static const double LOST_AND_FOUND_MAX_SEEK_TIME;

    static const int LOST_AND_FOUND_MIN_NUM_LOOK_TIMES;
    static const int LOST_AND_FOUND_MAX_NUM_LOOK_TIMES;

    float currLifePoints;


    TurretAIState currTurretState;
    BarrelAnimationState barrelAnimState;

    double lostAndFoundTimeCounter;
    double nextLostAndFoundSeekTime;
    int numSearchTimesCounter;
    int numTimesToSearch;
    
    // Turret-and-appearence-related variables
    float currRotationFromXInDegs;
    float currRotationSpd;
    float currRotationAccel;

    AnimationLerp<float> barrel1RecoilAnim;
    AnimationLerp<float> barrel2RecoilAnim;
    
    // AI update private members
    bool ExecutePaddleSeekingAI(double dT, const GameModel* model);
    bool ExecuteContinueTrackingAI(double dT, const GameModel* model);
    bool ContinueTrackingStateUpdateFromView(const GameModel* model);
    bool ExecuteLostAndFoundAI(double dT, const GameModel* model);

    // State update private members
    void SetTurretState(const TurretAIState& state);
    void SetBarrelState(const BarrelAnimationState& state, GameModel* model);
    void UpdateBarrelState(double dT, bool isAllowedToFire, GameModel* model);

    void GetFiringDirection(Vector2D& unitDir) const;
    void CanSeeAndFireAtPaddle(const GameModel* model, bool& canSeePaddle, bool& canFireAtPaddle) const;

    void UpdateSpeed();

    DISALLOW_COPY_AND_ASSIGN(LaserTurretBlock);
};


inline float LaserTurretBlock::GetRotationDegreesFromX() const {
    return this->currRotationFromXInDegs;
}
inline float LaserTurretBlock::GetBarrel1RecoilAmount() const {
    return this->barrel1RecoilAnim.GetInterpolantValue();
}
inline float LaserTurretBlock::GetBarrel2RecoilAmount() const {
    return this->barrel2RecoilAnim.GetInterpolantValue();
}

inline LaserTurretBlock::TurretAIState LaserTurretBlock::GetTurretAIState() const {
    return this->currTurretState;
}

inline LaserTurretBlock::BarrelAnimationState LaserTurretBlock::GetBarrelState() const {
    return this->barrelAnimState;
}

#endif // __LASERTURRETBLOCK_H__