/**
 * ClassicalBossAIStates.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#include "ClassicalBossAIStates.h"
#include "ClassicalBoss.h"
#include "BossWeakpoint.h"
#include "GameModel.h"
#include "PlayerPaddle.h"
#include "BossLaserProjectile.h"

using namespace classicalbossai;

// BEGIN ClassicalBossAI ***********************************************************

ClassicalBossAI::ClassicalBossAI(ClassicalBoss* boss) : BossAIState(), boss(boss) {
    assert(boss != NULL);
}

ClassicalBossAI::~ClassicalBossAI() {
    this->boss = NULL;
}

void ClassicalBossAI::ExecuteLaserSpray(GameModel* gameModel) {
    assert(gameModel != NULL);
    
    const PlayerPaddle* paddle = gameModel->GetPlayerPaddle();
    assert(paddle != NULL);
    
    static const float ANGLE_BETWEEN_LASERS_IN_DEGS = 15.0f;

    // The boss will fire a spray of laser bullets from its eye, in the general direction of the paddle...
    Point2D eyePos    = this->boss->GetEye()->GetTranslationPt2D();
    Point2D paddlePos = paddle->GetCenterPosition();

    Vector2D initLaserDir = Vector2D::Normalize(paddlePos - eyePos);
    Vector2D currLaserDir = initLaserDir;

    // Middle laser shot directly towards the paddle...
    gameModel->AddProjectile(new BossLaserProjectile(eyePos, currLaserDir));

    // Right fan-out of lasers
    currLaserDir.Rotate(ANGLE_BETWEEN_LASERS_IN_DEGS);
    gameModel->AddProjectile(new BossLaserProjectile(eyePos, currLaserDir));
    currLaserDir.Rotate(ANGLE_BETWEEN_LASERS_IN_DEGS);
    gameModel->AddProjectile(new BossLaserProjectile(eyePos, currLaserDir));

    // Left fan-out of lasers
    currLaserDir = initLaserDir;
    currLaserDir.Rotate(-ANGLE_BETWEEN_LASERS_IN_DEGS);
    gameModel->AddProjectile(new BossLaserProjectile(eyePos, currLaserDir));
    currLaserDir.Rotate(-ANGLE_BETWEEN_LASERS_IN_DEGS);
    gameModel->AddProjectile(new BossLaserProjectile(eyePos, currLaserDir));
}

// END ClassicalBossAI *************************************************************

// BEGIN ArmsBodyHeadAI ************************************************************

const float ArmsBodyHeadAI::ARM_LIFE_POINTS = 300.0f;
const float ArmsBodyHeadAI::ARM_BALL_DAMAGE = 100.0f;

const double ArmsBodyHeadAI::LASER_SPRAY_RESET_TIME_IN_SECS = 1.5;

ArmsBodyHeadAI::ArmsBodyHeadAI(ClassicalBoss* boss) : ClassicalBossAI(boss), currState(ArmsBodyHeadAI::BasicMoveAndLaserSprayAIState),
leftArm(NULL), rightArm(NULL), leftArmSqrWeakpt(NULL), rightArmSqrWeakpt(NULL), currVel(0.0f, 0.0f), desiredVel(0.0f, 0.0f),
countdownToNextState(0.0), countdownToAttack(0.0), laserSprayCountdown(0.0) {
    
    // Grab the parts of the boss that matter to this AI state...
    this->leftArm  = static_cast<BossCompositeBodyPart*>(boss->bodyParts[boss->leftArmIdx]);
    this->rightArm = static_cast<BossCompositeBodyPart*>(boss->bodyParts[boss->rightArmIdx]);
    
    // Turn the squares on both arms into weakpoints...
    boss->ConvertAliveBodyPartToWeakpoint(boss->leftArmSquareIdx, ArmsBodyHeadAI::ARM_LIFE_POINTS, ArmsBodyHeadAI::ARM_BALL_DAMAGE);
    boss->ConvertAliveBodyPartToWeakpoint(boss->rightArmSquareIdx, ArmsBodyHeadAI::ARM_LIFE_POINTS, ArmsBodyHeadAI::ARM_BALL_DAMAGE);
    
    assert(dynamic_cast<BossWeakpoint*>(boss->bodyParts[boss->leftArmSquareIdx])  != NULL);
    assert(dynamic_cast<BossWeakpoint*>(boss->bodyParts[boss->rightArmSquareIdx]) != NULL);

    this->leftArmSqrWeakpt  = static_cast<BossWeakpoint*>(boss->bodyParts[boss->leftArmSquareIdx]);
    this->rightArmSqrWeakpt = static_cast<BossWeakpoint*>(boss->bodyParts[boss->rightArmSquareIdx]);

    assert(this->leftArm != NULL && this->rightArm != NULL && this->leftArmSqrWeakpt != NULL &&
        this->rightArmSqrWeakpt != NULL);

    // Setup arm shaking animation
    {
        std::vector<double> timeValues;
        timeValues.reserve(32);
        timeValues.push_back(0.0);
        timeValues.push_back(0.01);
        timeValues.push_back(0.025);
        timeValues.push_back(0.07);
        timeValues.push_back(0.10);
        timeValues.push_back(0.13);
        timeValues.push_back(0.18);
        timeValues.push_back(0.2);
        timeValues.push_back(0.21);
        timeValues.push_back(0.225);
        timeValues.push_back(0.27);
        timeValues.push_back(0.315);
        timeValues.push_back(0.34);
        timeValues.push_back(0.377);
        timeValues.push_back(0.4);
        timeValues.push_back(0.412);
        timeValues.push_back(0.49);
        timeValues.push_back(0.55);
        timeValues.push_back(0.6);
        timeValues.push_back(0.61);
        timeValues.push_back(0.625);
        timeValues.push_back(0.67);
        timeValues.push_back(0.70);
        timeValues.push_back(0.73);
        timeValues.push_back(0.78);
        timeValues.push_back(0.8);
        timeValues.push_back(0.81);
        timeValues.push_back(0.825);
        timeValues.push_back(0.87);
        timeValues.push_back(0.915);
        timeValues.push_back(0.94);
        timeValues.push_back(0.977);
        timeValues.push_back(1.0);

        std::vector<float> movementValues;
        movementValues.reserve(32);
        float maxMove = 0.15f * ClassicalBoss::ARM_WIDTH;
        movementValues.push_back(0.0f);
        movementValues.push_back(maxMove * 0.6f);
        movementValues.push_back(-maxMove * 0.1f);
        movementValues.push_back(-maxMove);
        movementValues.push_back(-maxMove * 0.75f);
        movementValues.push_back(maxMove * 0.4f);
        movementValues.push_back(maxMove);
        movementValues.push_back(-0.2f * maxMove);
        movementValues.push_back(0.1f * maxMove);
        movementValues.push_back(-maxMove);
        movementValues.push_back(maxMove * 0.6f);
        movementValues.push_back(-maxMove * 0.1f);
        movementValues.push_back(-maxMove);
        movementValues.push_back(-maxMove * 0.75f);
        movementValues.push_back(maxMove * 0.4f);
        movementValues.push_back(maxMove);
        movementValues.push_back(-0.2f * maxMove);
        movementValues.push_back(0.6f * maxMove);
        movementValues.push_back(-maxMove * 0.75f);
        movementValues.push_back(maxMove * 0.4f);
        movementValues.push_back(maxMove);
        movementValues.push_back(-0.2f * maxMove);
        movementValues.push_back(0.1f * maxMove);
        movementValues.push_back(-maxMove);
        movementValues.push_back(maxMove * 0.6f);
        movementValues.push_back(-maxMove * 0.1f);
        movementValues.push_back(-maxMove);
        movementValues.push_back(-maxMove * 0.75f);
        movementValues.push_back(maxMove * 0.4f);
        movementValues.push_back(maxMove);
        movementValues.push_back(-0.2f * maxMove);
        movementValues.push_back(0.1f * maxMove);
        movementValues.push_back(0.0f);

        this->armShakeAnim.SetLerp(timeValues, movementValues);
        this->armShakeAnim.SetRepeat(false);
    }

    // Setup the arm attack animation
    {
        std::vector<double> timeValues;
        timeValues.reserve(5);
        timeValues.push_back(0.0);
        timeValues.push_back(0.4);
        timeValues.push_back(0.48);
        timeValues.push_back(1.0);
        timeValues.push_back(2.5);
        
        std::vector<float> movementValues;
        movementValues.reserve(5);
        movementValues.push_back(0.0f);
        movementValues.push_back(-1.15f * this->GetMaxArmAttackYMovement());
        movementValues.push_back(-this->GetMaxArmAttackYMovement());
        movementValues.push_back(-this->GetMaxArmAttackYMovement());
        movementValues.push_back(0.0f);

        this->armAttackYMovementAnim.SetLerp(timeValues, movementValues);
        this->armAttackYMovementAnim.SetRepeat(false);
    }

    this->SetState(ArmsBodyHeadAI::BasicMoveAndLaserSprayAIState);
}

ArmsBodyHeadAI::~ArmsBodyHeadAI() {
}

void ArmsBodyHeadAI::Tick(double dT, GameModel* gameModel) {
    // Update the state of the AI
    this->UpdateState(dT, gameModel);
    // Update the bosses' movement
    this->UpdateMovement(dT, gameModel);
}

void ArmsBodyHeadAI::CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart) {
}

void ArmsBodyHeadAI::CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart) {
}

void ArmsBodyHeadAI::CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle, BossBodyPart* collisionPart) {
}

void ArmsBodyHeadAI::SetState(AIState newState) {
    switch (newState) {

        case ArmsBodyHeadAI::ChanceAIState:
            break;

        case ArmsBodyHeadAI::BasicMoveAndLaserSprayAIState:
            this->laserSprayCountdown  = ArmsBodyHeadAI::LASER_SPRAY_RESET_TIME_IN_SECS;
            this->countdownToNextState = this->GenerateBasicMoveTime();
            break;

        case ArmsBodyHeadAI::ChasePaddleAIState:
            this->countdownToAttack = this->GenerateFollowTime();
            break;

        case ArmsBodyHeadAI::AttackLeftArmAIState:
        case ArmsBodyHeadAI::AttackRightArmAIState:
        case ArmsBodyHeadAI::AttackBothArmsAIState:
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->armShakeAnim.ResetToStart();
            this->armAttackYMovementAnim.ResetToStart();
            this->leftArmStartWorldT  = this->leftArm->GetWorldTransform();
            this->rightArmStartWorldT = this->rightArm->GetWorldTransform();
            break;

        case ArmsBodyHeadAI::PrepLaserAIState:
            break;
        case ArmsBodyHeadAI::MoveAndBarrageWithLaserAIState:
            break;

        case ArmsBodyHeadAI::HurtAIState:
            break;
        case ArmsBodyHeadAI::LostArmsAngryAIState:
            break;

        default:
            assert(false);
            break;
    }

    this->currState = newState;
}

void ArmsBodyHeadAI::UpdateState(double dT, GameModel* gameModel) {
    switch (this->currState) {

        case ArmsBodyHeadAI::ChanceAIState:
            break;

        case ArmsBodyHeadAI::BasicMoveAndLaserSprayAIState:
            this->ExecuteBasicMoveAndLaserSprayState(dT, gameModel);
            break;

        case ArmsBodyHeadAI::ChasePaddleAIState: {
            const PlayerPaddle* paddle = gameModel->GetPlayerPaddle();
            this->ExecuteFollowPaddleState(dT, paddle);
            break;
        }

        case ArmsBodyHeadAI::AttackLeftArmAIState:
            this->ExecuteArmAttackState(dT, true, false);
            break;
        case ArmsBodyHeadAI::AttackRightArmAIState:
            this->ExecuteArmAttackState(dT, false, true);
            break;
        case ArmsBodyHeadAI::AttackBothArmsAIState:
            this->ExecuteArmAttackState(dT, true, true);
            break;

        case ArmsBodyHeadAI::PrepLaserAIState:
            break;
        case ArmsBodyHeadAI::MoveAndBarrageWithLaserAIState:
            break;

        case ArmsBodyHeadAI::HurtAIState:
            break;
        case ArmsBodyHeadAI::LostArmsAngryAIState:
            break;

        default:
            assert(false);
            break;
    }
}

void ArmsBodyHeadAI::UpdateMovement(double dT, GameModel* gameModel) {
    // Figure out how much to move and update the position of the boss
    if (!this->currVel.IsZero()) {
        Vector2D dMovement = dT * this->currVel;
        this->boss->alivePartsRoot->Translate(Vector3D(dMovement));

        // Update the position of the boss based on whether it is now colliding with the boundaries/walls of the level
        const GameLevel* level = gameModel->GetCurrentLevel();
        Vector2D correctionVec;
        if (level->CollideBossWithLevel(this->boss->alivePartsRoot->GenerateWorldAABB(), correctionVec)) {
            this->boss->alivePartsRoot->Translate(Vector3D(correctionVec));
        }
    }

    // Update the speed based on the acceleration
    this->currVel = this->currVel + dT * this->GetAcceleration();
}

void ArmsBodyHeadAI::ExecuteBasicMoveAndLaserSprayState(double dT, GameModel* gameModel) {
    assert(gameModel != NULL);

    const GameLevel* level = gameModel->GetCurrentLevel();
    assert(level != NULL);

    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();

    // The basic movement state occurs further away from the paddle, closer to the top of the level.
    // Make sure we've moved up high enough...
    float avgBasicMoveHeight = this->GetBasicMovementHeight(level);
    float upDownDistance     = this->boss->GetAliveHeight() / 5.0f;
    float moveToTopDiff      = avgBasicMoveHeight - bossPos[1];
    
    if (bossPos[1] < avgBasicMoveHeight - upDownDistance) {
        this->desiredVel[1] = this->GetMaxSpeed() / 2.0f;
    }
    if (bossPos[1] > avgBasicMoveHeight + upDownDistance) {
        this->desiredVel[1] = -this->GetMaxSpeed() / 2.0f;
    }
        
    // Side-to-side movement is basic back and forth linear translation...
    float bossWidth = this->boss->GetAliveWidth();
    if (bossPos[0] <= bossWidth) {
        this->desiredVel[0] = this->GetMaxSpeed() / 1.25f;
    }
    else if (bossPos[0] >= level->GetLevelUnitWidth() - bossWidth) {
        this->desiredVel[0] = -this->GetMaxSpeed() / 1.25f;
    }
    
    if (fabs(this->desiredVel[0]) < EPSILON) {
        this->desiredVel[0] = Randomizer::GetInstance()->RandomNegativeOrPositive() * this->GetMaxSpeed() / 1.25f;
    }

    if (this->countdownToNextState <= 0.0) {
        // Determine the next state - we will either go back to chasing the paddle around and eventually attacking
        // with the arms, or we go into a laser charge and barrage state progression
        
        bool goToChaseState = true;
        
        // How much damage has been done so far? These two percentages will maximally add to 2.0
        float percentLeftArm  = this->leftArmSqrWeakpt->GetCurrentLifePercentage();
        float percentRightArm = this->rightArmSqrWeakpt->GetCurrentLifePercentage();
        
        // Laser barrages will happen more frequently when the boss has taken more damage...
        float totalLifePercent = (percentLeftArm + percentRightArm) / 2.0f;
        if (Randomizer::GetInstance()->RandomNumZeroToOne() >= (totalLifePercent - 0.1f)) {
            goToChaseState = false;
        }
        
        if (goToChaseState) {
            this->SetState(ArmsBodyHeadAI::ChasePaddleAIState);
        }
        else {
            this->SetState(ArmsBodyHeadAI::PrepLaserAIState);
        }

    }
    else {
        this->countdownToNextState -= dT;
    }

    // Fire ze lasers (if the countdown is done)
    if (this->laserSprayCountdown <= 0.0) {
        this->ExecuteLaserSpray(gameModel);
        this->laserSprayCountdown = ArmsBodyHeadAI::LASER_SPRAY_RESET_TIME_IN_SECS;
    }
    else {
        this->laserSprayCountdown -= dT;
    }

}

void ArmsBodyHeadAI::ExecuteFollowPaddleState(double dT, const PlayerPaddle* paddle) {
    assert(paddle != NULL);

    const Point2D& paddlePos = paddle->GetCenterPosition();
    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();

    bool allowedToAttack = false;

    // Make sure the boss is at the right height from the paddle...
    if (bossPos[1] > this->GetFollowAndAttackHeight()) {
        if (bossPos[1] > this->GetFollowAndAttackHeight()) {
            this->desiredVel[1] = -this->GetMaxSpeed() / 2.0f;
        }
    }
    else {
        this->currVel[1]    = 0.0f;
        this->desiredVel[1] = 0.0f;
        allowedToAttack = true;
    }
    
    // Find the vector from boss to paddle, and project onto the x-axis to figure out the movement direction
    Vector2D bossToPaddleVec   = paddlePos - bossPos;
    
    // Check to see if we can attack yet (If we're anywhere near the paddle then perform the attack)
    float absBossToPaddleXDist = fabs(bossToPaddleVec[0]);
    if (absBossToPaddleXDist <= (ClassicalBoss::ARM_X_TRANSLATION_FROM_CENTER + ClassicalBoss::HALF_ARM_WIDTH) &&
        absBossToPaddleXDist >= (ClassicalBoss::ARM_X_TRANSLATION_FROM_CENTER - ClassicalBoss::HALF_ARM_WIDTH)) {

        this->desiredVel[0] = 0.0f;

        // Attack immediately!
        if (allowedToAttack) {
            this->SetState(this->DetermineNextArmAttackState(bossToPaddleVec));
        }
        return;
    }
    else if (absBossToPaddleXDist > ClassicalBoss::ARM_X_TRANSLATION_FROM_CENTER + ClassicalBoss::HALF_ARM_WIDTH) {
        this->desiredVel[0] = NumberFuncs::SignOf(bossToPaddleVec[0]) * this->GetMaxSpeed();
    }
    else {
        // Determine the closest arm to use
        float xDirSignToPaddle = 0.0f;
        if (absBossToPaddleXDist < EPSILON) {
            xDirSignToPaddle = ((Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) ? 1 : -1);
        }
        else if (bossToPaddleVec[0] < 0) {
            // Move the boss away from the paddle
            xDirSignToPaddle = 1.0f;
        }
        else {
            xDirSignToPaddle = -1.0f;
        }

        // Set the direction of the boss so that it chases the paddle around a bit...
        this->desiredVel[0] = xDirSignToPaddle * this->GetMaxSpeed();
    }

    // Check to see if we should go into an attack state yet
    if (this->countdownToAttack <= 0.0) {
        if (allowedToAttack) {
            this->SetState(this->DetermineNextArmAttackState(bossToPaddleVec));
        }
    }
    else {
        this->countdownToAttack -= dT;
    }
}

void ArmsBodyHeadAI::ExecuteArmAttackState(double dT, bool isLeftArmAttacking, bool isRightArmAttacking) {
    
    bool armIsDoneShaking = this->armShakeAnim.Tick(dT);
    if (!armIsDoneShaking) {
        float shakingMovement = this->armShakeAnim.GetInterpolantValue();

        // Animate the arm we are attacking with -- it's shaking to indicate the incoming attack
        if (isLeftArmAttacking) {
            this->leftArm->SetWorldTransform(Matrix4x4::translationMatrix(Vector3D(shakingMovement, 0.0f, 0.0f)) * this->leftArmStartWorldT);
        }
        if (isRightArmAttacking) {
            this->rightArm->SetWorldTransform(Matrix4x4::translationMatrix(Vector3D(shakingMovement, 0.0f, 0.0f)) * this->rightArmStartWorldT);
        }
    }
    else {
        bool attackIsDone = this->armAttackYMovementAnim.Tick(dT);

        // We are attacking with the arm!!!
        
        // Move the arm based on the attack animation
        float armYMovement = this->armAttackYMovementAnim.GetInterpolantValue();
        if (isLeftArmAttacking) {
            this->leftArm->SetWorldTransform(Matrix4x4::translationMatrix(Vector3D(0.0f, armYMovement, 0.0f)) * this->leftArmStartWorldT);
        }
        if (isRightArmAttacking) {
            this->rightArm->SetWorldTransform(Matrix4x4::translationMatrix(Vector3D(0.0f, armYMovement, 0.0f)) * this->rightArmStartWorldT);
        }
        
        if (attackIsDone) {
            this->leftArm->SetWorldTransform(this->leftArmStartWorldT);
            this->rightArm->SetWorldTransform(this->rightArmStartWorldT);
            this->SetState(ArmsBodyHeadAI::BasicMoveAndLaserSprayAIState);
            return;
        }
    }
}

float ArmsBodyHeadAI::GetMaxSpeed() const {
    return ClassicalBoss::ARMS_BODY_HEAD_MAX_SPEED;
}

Vector2D ArmsBodyHeadAI::GetAcceleration() const {
    Vector2D accel = this->desiredVel - this->currVel;
    if (accel.IsZero()) {
        return Vector2D(0,0);
    }
    accel.Normalize();
    return ClassicalBoss::ARMS_BODY_HEAD_ACCELERATION * accel;
}

float ArmsBodyHeadAI::GetBasicMovementHeight(const GameLevel* level) const {
    return level->GetLevelUnitHeight() - 1.25f * this->boss->GetAliveHeight();
}

float ArmsBodyHeadAI::GetFollowAndAttackHeight() const {
    return 0.8f * this->GetMaxArmAttackYMovement() + this->boss->GetAliveHeight() / 2.0f;
}

float ArmsBodyHeadAI::GetMaxArmAttackYMovement() const {
    return 0.7f * ClassicalBoss::ARM_HEIGHT;
}

ArmsBodyHeadAI::AIState ArmsBodyHeadAI::DetermineNextArmAttackState(const Vector2D& bossToPaddleVec) const {
    // How much damage has been done so far? These two percentages will maximally add to 2.0
    float percentLeftArm  = this->leftArmSqrWeakpt->GetCurrentLifePercentage();
    float percentRightArm = this->rightArmSqrWeakpt->GetCurrentLifePercentage();
    float totalLifePercent = (percentLeftArm + percentRightArm) / 2.0f;
    
    bool doBothArmAttack = false;
    if (Randomizer::GetInstance()->RandomNumZeroToOne() >= (totalLifePercent - 0.05f)) {
        doBothArmAttack = true;
    }

    if (doBothArmAttack) {
        return ArmsBodyHeadAI::AttackBothArmsAIState;
    }
    else {
        if (bossToPaddleVec[0] < 0) {
            return ArmsBodyHeadAI::AttackLeftArmAIState;
        }
        else {
            return ArmsBodyHeadAI::AttackRightArmAIState;
        }
    }
}

void ExecuteLaserSpray(GameModel* gameModel);

// END ArmsBodyHeadAI **************************************************************