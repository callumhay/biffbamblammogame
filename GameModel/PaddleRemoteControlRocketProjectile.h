/**
 * PaddleRemoteControlRocketProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __PADDLEREMOTECONTROLROCKETPROJECTILE_H__
#define __PADDLEREMOTECONTROLROCKETPROJECTILE_H__

#include "RocketProjectile.h"

/**
 * Projectile for the rocket shot from the player paddle when the player acquires a rocket paddle item.
 */
class PaddleRemoteControlRocketProjectile : public RocketProjectile {
public:
	static const float PADDLE_REMOTE_CONTROL_ROCKET_HEIGHT_DEFAULT;
	static const float PADDLE_REMOTE_CONTROL_ROCKET_WIDTH_DEFAULT;

    static const double TIME_BEFORE_FUEL_RUNS_OUT_IN_SECS;

    static const float MAX_VELOCITY_BEFORE_THRUST;
    static const float MAX_VELOCITY_WITH_THRUST;

	PaddleRemoteControlRocketProjectile(const Point2D& spawnLoc, const Vector2D& rocketVelDir, 
        float width, float height);
    PaddleRemoteControlRocketProjectile(const PaddleRemoteControlRocketProjectile& copy);
	~PaddleRemoteControlRocketProjectile();
    
    void Setup(GameModel& gameModel);
    void Teardown(GameModel& gameModel);

    void Tick(double seconds, const GameModel& model);
    bool ModifyLevelUpdate(double dT, GameModel& model);

    ProjectileType GetType() const {
        return Projectile::PaddleRemoteCtrlRocketBulletProjectile;
    }

    float GetDamage() const {
        // Obtain the size relative to the the normal size of the rocket (1.0) this rocket
        // is currently - used to determine how much it will destroy.
        return (this->GetWidth() / PADDLE_REMOTE_CONTROL_ROCKET_WIDTH_DEFAULT) * 300.0f;
    }

    float GetVisualScaleFactor() const { return this->GetWidth() / this->GetDefaultWidth(); }
    
    float GetAccelerationMagnitude() const { return 5.0f; }
    float GetRotationAccelerationMagnitude() const { return 60.0f; }

    float GetMaxVelocityMagnitude() const;
    float GetMaxRotationVelocityMagnitude() const { return 180.0f; }

    float GetDefaultHeight() const { return PADDLE_REMOTE_CONTROL_ROCKET_HEIGHT_DEFAULT; }
    float GetDefaultWidth() const  { return PADDLE_REMOTE_CONTROL_ROCKET_WIDTH_DEFAULT;  }

    enum RocketSteering { LeftRocketSteering = -1, NoRocketSteering = 0, RightRocketSteering = 1};
    void ControlRocketSteering(const RocketSteering& steering, float magnitudePercent);
    void ControlRocketThrust(float magnitudePercent);

    double GetTimeUntilFuelRunsOut() const { return NumberFuncs::Lerp<double>(0, STARTING_FUEL_AMOUNT, 0, TIME_BEFORE_FUEL_RUNS_OUT_IN_SECS, this->currFuelAmt); }
    float GetCurrentFuelAmount() const { return this->currFuelAmt; }
    float GetCurrentFlashingAmount() const { return this->currFlashColourAmt; }
    float GetCurrentAppliedThrustAmount() const { return this->currAppliedThrust; }

private:
    static const float MAX_APPLIED_ACCELERATION;
    static const float DECELLERATION_OF_APPLIED_ACCEL;
    
    static const float MAX_APPLIED_THRUST;
    static const float THRUST_DECREASE_RATE;
    static const double MIN_TIME_BETWEEN_THRUSTS;
    
    static const float STARTING_FUEL_AMOUNT;
    static const double RATE_OF_FUEL_CONSUMPTION;
    static const float FUEL_AMOUNT_TO_START_FLASHING;

    double fuelTimeCountdown; // Time until fuel runs out
    float currFuelAmt;        // [0, 100] level of the fuel left in this rocket, when fuel runs out it explodes
    float currFlashColourAmt; // [0, 1] Amount of colour to apply to flashing
    float currFlashFreq;      // The frequency in Hz of the flashing
    double flashTimeCounter;  // Internal counter for determining where in a flash cycle we are

    Vector2D currAppliedAccelDir; // The current applied acceleration direction vector
    float currAppliedAccelMag;    // The current applied acceleration magnitude scalar

    float currAppliedThrust;           // The current applied thrust magnitude
    double timeUntilThrustIsAvailable; // Countdown until when thrust can be used again

    Vector2D GetAppliedAcceleration() const { return this->currAppliedAccelMag * this->currAppliedAccelDir; }
    void SetAppliedAcceleration(const Vector2D& accel);

    bool IsRocketStillTakingOff() const { return this->GetVelocityMagnitude() < 0.75f * MAX_VELOCITY_BEFORE_THRUST; }
};

inline float PaddleRemoteControlRocketProjectile::GetMaxVelocityMagnitude() const {
    if (this->currAppliedThrust > 0.0f) {
        return MAX_VELOCITY_WITH_THRUST;
    }
    else {
        return MAX_VELOCITY_BEFORE_THRUST;
    }
}

inline void PaddleRemoteControlRocketProjectile::ControlRocketSteering(const RocketSteering& steering,
                                                                       float magnitudePercent) {
    // Can't control a rocket if it's inside a cannon block
    if (this->IsLoadedInCannonBlock()) { 
        return;
    }
    this->SetAppliedAcceleration(static_cast<int>(steering) * magnitudePercent * MAX_APPLIED_ACCELERATION * this->GetRightVectorDirection());
}

inline void PaddleRemoteControlRocketProjectile::SetAppliedAcceleration(const Vector2D& accel) {
    this->currAppliedAccelMag = accel.Magnitude();
    if (this->currAppliedAccelMag > EPSILON) {
        this->currAppliedAccelDir = accel / this->currAppliedAccelMag;
    }
    else {
        this->currAppliedAccelDir = Vector2D(0,0);
    }
}

#endif // __PADDLEREMOTECONTROLROCKETPROJECTILE_H__