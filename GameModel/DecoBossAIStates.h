/**
 * DecoBossAIStates.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#include "../BlammoEngine/Algebra.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Colour.h"

#include "BossAIState.h"

class DecoBoss;
class BossBodyPart;
class BossWeakpoint;
class BossCompositeBodyPart;

namespace decobossai {

/**
 * Abstract superclass for all of the DecoBoss AI state machine classes.
 */
class DecoBossAIState : public BossAIState {
public:
    DecoBossAIState(DecoBoss* boss);
    virtual ~DecoBossAIState();

    // Inherited functions
    Boss* GetBoss() const;
    virtual bool CanHurtPaddleWithBody() const { return false; }
    virtual bool IsStateMachineFinished() const { return false; }
    Collision::AABB2D GenerateDyingAABB() const;

protected:
    DecoBoss* boss;

    enum AIState {
        FiringLeftArmVertAIState, FiringRightArmVertAIState, 
        FiringLeftArmHorizAIState, FiringRightArmHorizAIState, RotatingLevelAIState,
        ElectrifiedAIState, AngryAIState, FinalDeathThroesAIState 
    };


    virtual void SetState(DecoBossAIState::AIState newState) = 0;
 
    
private:
    DISALLOW_COPY_AND_ASSIGN(DecoBossAIState);
};

class DecoBossStage1 : public DecoBossAIState {
public:
    DecoBossStage1(DecoBoss* boss);
    ~DecoBossStage1();

private:
    static const float DEFAULT_ACCELERATION;

    // Inherited Functions
    void SetState(DecoBossAIState::AIState newState);
    float GetAccelerationMagnitude() const { return DEFAULT_ACCELERATION; }
};



}; // namespace decobossai