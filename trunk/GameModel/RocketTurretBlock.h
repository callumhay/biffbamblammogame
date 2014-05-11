/**
 * RocketTurretBlock.h
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

#ifndef __ROCKETTURRETBLOCK_H__
#define __ROCKETTURRETBLOCK_H__

#include "TurretBlock.h"

class RocketTurretBlock : public TurretBlock {
public:
    static const float ROCKET_HOLE_RADIUS;

    static const float BARREL_OFFSET_EXTENT_ALONG_X;
    static const float BARREL_OFFSET_EXTENT_ALONG_Y;
    static const float BARREL_OFFSET_EXTENT_ALONG_Z;

    enum TurretAIState { IdleTurretState = 0, SeekingTurretState, TargetFoundTurretState, TargetLostTurretState };

	RocketTurretBlock(unsigned int wLoc, unsigned int hLoc);
	~RocketTurretBlock();

	LevelPieceType GetType() const { return LevelPiece::RocketTurret; }

	bool ProjectilePassesThrough(const Projectile* projectile) const;
    int GetPointsOnChange(const LevelPiece& changeToPiece) const;

	// Collision related stuffs
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);

    void AITick(double dT, GameModel* gameModel);

    void GetBarrelInfo(Point3D& endOfBarrelPt, Vector2D& barrelDir) const;
    void GetFiringDirection(Vector2D& unitDir) const;
    float GetRotationDegreesFromX() const;
    float GetBarrelRecoilAmount() const;

    bool HasRocketLoaded() const {
        return (this->barrelState == BarrelForwardRocketLoading || this->barrelState == BarrelForwardRocketLoaded);
    }
    float GetRocketTranslationFromCenter() const {
        return this->rocketMovementAnim.GetInterpolantValue();
    }

    RocketTurretBlock::TurretAIState GetTurretAIState() const;

private:
    static const int POINTS_ON_BLOCK_DESTROYED  = 800;

    static const float MAX_ROTATION_SPEED_IN_DEGS_PER_SEC;
    static const float ROTATION_ACCEL_IN_DEGS_PER_SEC_SQRD;
    static const float BARREL_RECOIL_TRANSLATION_AMT;
    static const float ROCKET_DISPLACEMENT_PRE_LOAD;
    static const float ROCKET_DISPLACEMENT_ON_LOAD;

    static const float FIRE_RATE_IN_ROCKETS_PER_SEC;
    static const float BARREL_RELOAD_TIME;
    static const float BARREL_RECOIL_TIME;
    static const float ROCKET_LOAD_TIME;

    static const double LOST_AND_FOUND_MIN_SEEK_TIME;
    static const double LOST_AND_FOUND_MAX_SEEK_TIME;

    static const int LOST_AND_FOUND_MIN_NUM_LOOK_TIMES;
    static const int LOST_AND_FOUND_MAX_NUM_LOOK_TIMES;

    enum BarrelAnimationState { BarrelForwardRocketLoading = 0, BarrelForwardRocketLoaded,
                                BarrelRecoiling, BarrelBack, BarrelReloading };

    float currLifePoints;
    TurretAIState currTurretState;
    BarrelAnimationState barrelState;

    // Turret-and-appearence-related variables
    float currRotationFromXInDegs;
    float currRotationSpd;
    float currRotationAccel;

    double lostAndFoundTimeCounter;
    double nextLostAndFoundSeekTime;
    int numSearchTimesCounter;
    int numTimesToSearch;

    AnimationLerp<float> barrelMovementAnim;
    AnimationLerp<float> rocketMovementAnim;

    // AI update private members
    bool ExecutePaddleSeekingAI(double dT, const GameModel* model);
    bool ExecuteContinueTrackingAI(double dT, const GameModel* model);
    bool ContinueTrackingStateUpdateFromView(const GameModel* model);
    bool ExecuteLostAndFoundAI(double dT, const GameModel* model);

    void SetTurretState(const TurretAIState& state);
    void SetBarrelState(const BarrelAnimationState& state, GameModel* model);
    void UpdateBarrelState(double dT, bool isAllowedToFire, GameModel* model);

    void CanSeeAndFireAtPaddle(const GameModel* model, bool& canSeePaddle, bool& canFireAtPaddle) const;
    void UpdateSpeed();

    DISALLOW_COPY_AND_ASSIGN(RocketTurretBlock);
};

inline float RocketTurretBlock::GetRotationDegreesFromX() const {
    return this->currRotationFromXInDegs;
}

inline float RocketTurretBlock::GetBarrelRecoilAmount() const {
    return this->barrelMovementAnim.GetInterpolantValue();
}

inline RocketTurretBlock::TurretAIState RocketTurretBlock::GetTurretAIState() const {
    return this->currTurretState;
}

#endif // __ROCKETTURRETBLOCK_H__