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
#include "GameModel.h"
#include "PlayerPaddle.h"

using namespace classicalbossai;

// BEGIN ArmsBodyHeadAI ************************************************************

ArmsBodyHeadAI::ArmsBodyHeadAI(ClassicalBoss* boss) : ClassicalBossAI(boss), currState(ArmsBodyHeadAI::FollowPaddleAIState),
leftArm(NULL), rightArm(NULL), leftArmSqrWeakpt(NULL), rightArmSqrWeakpt(NULL), currVel(0.0f, 0.0f), desiredVel(0.0f, 0.0f),
countdownToAttack(0.0) {
    
    // Grab the parts of the boss that matter to this AI state...
    this->leftArm  = static_cast<BossCompositeBodyPart*>(boss->bodyParts[boss->leftArmIdx]);
    this->rightArm = static_cast<BossCompositeBodyPart*>(boss->bodyParts[boss->rightArmIdx]);
    this->leftArmSqrWeakpt  = static_cast<BossBodyPart*>(boss->bodyParts[boss->leftArmSquareIdx]);
    this->rightArmSqrWeakpt = static_cast<BossBodyPart*>(boss->bodyParts[boss->rightArmSquareIdx]);

    assert(this->leftArm != NULL && this->rightArm != NULL && this->leftArmSqrWeakpt != NULL &&
        this->rightArmSqrWeakpt != NULL);

    // Setup arm shaking animation
    {
        std::vector<double> timeValues;
        timeValues.reserve(9);
        timeValues.push_back(0.0);
        timeValues.push_back(0.01);
        timeValues.push_back(0.025);
        timeValues.push_back(0.07);
        timeValues.push_back(0.0833);
        timeValues.push_back(0.10);
        timeValues.push_back(0.13);
        timeValues.push_back(0.18);
        timeValues.push_back(0.2);

        std::vector<float> movementValues;
        movementValues.reserve(9);
        float maxMove = 0.1f * ClassicalBoss::ARM_WIDTH;
        movementValues.push_back(0.0f);
        movementValues.push_back(maxMove * 0.6f);
        movementValues.push_back(-maxMove * 0.1f);
        movementValues.push_back(-maxMove);
        movementValues.push_back(-maxMove * 0.75f);
        movementValues.push_back(maxMove * 0.4f);
        movementValues.push_back(maxMove);
        movementValues.push_back(-0.2f * maxMove);
        movementValues.push_back(0.0f);

        this->armShakeAnim.SetLerp(timeValues, movementValues);
        this->armShakeAnim.SetRepeat(false);
    }

    // Setup the arm attack animation
    {
        std::vector<double> timeValues;
        timeValues.reserve(4);
        timeValues.push_back(0.0);
        timeValues.push_back(0.2);
        timeValues.push_back(1.0);
        timeValues.push_back(1.5);
        
        std::vector<float> movementValues;
        movementValues.reserve(4);
        movementValues.push_back(0.0f);
        movementValues.push_back(this->GetMaxArmAttackYMovement());
        movementValues.push_back(this->GetMaxArmAttackYMovement());
        movementValues.push_back(0.0f);

        this->armAttackYMovementAnim.SetLerp(timeValues, movementValues);
        this->armAttackYMovementAnim.SetRepeat(false);
    }

    this->SetState(ArmsBodyHeadAI::FollowPaddleAIState);
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

        case ArmsBodyHeadAI::BasicMovementAIState:
            break;

        case ArmsBodyHeadAI::FollowPaddleAIState:
            this->countdownToAttack = this->GenerateFollowTime();
            break;

        case ArmsBodyHeadAI::AttackLeftArmAIState:
        case ArmsBodyHeadAI::AttackRightArmAIState:
        case ArmsBodyHeadAI::AttackBothArmsAIState:
            this->desiredVel = Vector2D(0,0);
            this->armShakeAnim.ResetToStart();
            this->armAttackYMovementAnim.ResetToStart();
            this->leftArmStartWorldPos  = this->leftArm->GetTranslationPt3D();
            this->rightArmStartWorldPos = this->rightArm->GetTranslationPt3D();
            break;

        case ArmsBodyHeadAI::FarLeftPrepLaserAIState:
            break;
        case ArmsBodyHeadAI::FarRightPrepLaserAIState:
            break;
        case ArmsBodyHeadAI::MoveLeftLaserAIState:
            break;
        case ArmsBodyHeadAI::MoveRightLaserAIState:
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

        case ArmsBodyHeadAI::BasicMovementAIState: {
            const GameLevel* level = gameModel->GetCurrentLevel();
            this->ExecuteBasicMovementState(dT, level);
            break;
        }

        case ArmsBodyHeadAI::FollowPaddleAIState: {
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

        case ArmsBodyHeadAI::FarLeftPrepLaserAIState:
            break;
        case ArmsBodyHeadAI::FarRightPrepLaserAIState:
            break;
        case ArmsBodyHeadAI::MoveLeftLaserAIState:
            break;
        case ArmsBodyHeadAI::MoveRightLaserAIState:
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
    Vector2D dMovement = dT * this->currVel;
    this->boss->alivePartsRoot->Translate(Vector3D(dMovement));

    // Update the position of the boss based on whether it is now colliding with the boundaries/walls of the level
    const GameLevel* level = gameModel->GetCurrentLevel();
    Vector2D correctionVec;
    if (level->CollideBossWithLevel(this->boss->alivePartsRoot->GenerateWorldAABB(), correctionVec)) {
        this->boss->alivePartsRoot->Translate(Vector3D(correctionVec));
    }

    // Update the speed based on the acceleration
    this->currVel = this->currVel + dT * this->GetAcceleration();
}

void ArmsBodyHeadAI::ExecuteBasicMovementState(double dT, const GameLevel* level) {
    assert(level != NULL);

    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();

    // The basic movement state occurs further away from the paddle, closer to the top of the level.
    // Make sure we've moved up high enough...
    float moveToTopDiff = this->GetBasicMovementHeight(level) - bossPos[1];
    if (moveToTopDiff != 0.0f) {
        this->desiredVel[1] = NumberFuncs::SignOf(moveToTopDiff) * this->GetMaxSpeed();
    }
    else {
        // Use a sin wave to control the up and down movement...


        // Side-to-side movement is basic back and forth linear translation...


        // If we're ready to go back to a specific attack state then we need to start following the paddle -
        // get into a position closer to the paddle first though...
        // TODO

    }
}

void ArmsBodyHeadAI::ExecuteFollowPaddleState(double dT, const PlayerPaddle* paddle) {
    assert(paddle != NULL);

    const Point2D& paddlePos = paddle->GetCenterPosition();
    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();

    // Find the vector from boss to paddle, and project onto the x-axis to figure out the movement direction
    Vector2D bossToPaddleVec   = paddlePos - bossPos;
    
    // Check to see if we can attack yet (If we're anywhere near the paddle then perform the attack)
    float absBossToPaddleXDist = fabs(bossToPaddleVec[0]);
    if (absBossToPaddleXDist <= (ClassicalBoss::ARM_X_TRANSLATION_FROM_CENTER + ClassicalBoss::HALF_ARM_WIDTH) &&
        absBossToPaddleXDist >= (ClassicalBoss::ARM_X_TRANSLATION_FROM_CENTER - ClassicalBoss::HALF_ARM_WIDTH)) {

        this->desiredVel[0] = 0.0f;
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

        if (bossToPaddleVec[0] < 0) {
            this->SetState(ArmsBodyHeadAI::AttackLeftArmAIState);
        }
        else {
            this->SetState(ArmsBodyHeadAI::AttackRightArmAIState);
        }
    }
    else {
        this->countdownToAttack -= dT;
    }
}

void ArmsBodyHeadAI::ExecuteArmAttackState(double dT, bool isLeftArmAttacking, bool isRightArmAttacking) {
    
    float shakingMovement = this->armShakeAnim.GetInterpolantValue();
    if (shakingMovement != 0.0f) {
        // Animate the arm we are attacking with -- it's shaking to indicate the incoming attack

        if (isLeftArmAttacking) {
            this->leftArm->SetWorldTranslation(this->leftArmStartWorldPos + Vector3D(shakingMovement, 0.0f, 0.0f));
        }
        if (isRightArmAttacking) {
            this->rightArm->SetWorldTranslation(this->rightArmStartWorldPos + Vector3D(shakingMovement, 0.0f, 0.0f));
        }
    }

    bool armIsDoneShaking = this->armShakeAnim.Tick(dT);
    if (armIsDoneShaking) {
        // We are attacking with the arm!!!
        
        // Move the arm based on the attack animation
        float armYMovement = this->armAttackYMovementAnim.GetInterpolantValue();
        if (isLeftArmAttacking) {
            this->leftArm->SetWorldTranslation(this->leftArmStartWorldPos + Vector3D(0.0f, armYMovement, 0.0f));
        }
        if (isRightArmAttacking) {
            this->rightArm->SetWorldTranslation(this->rightArmStartWorldPos + Vector3D(0.0f, armYMovement, 0.0f));
        }
        
        bool attackIsDone = this->armAttackYMovementAnim.Tick(dT);
        if (attackIsDone) {

            this->SetState(ArmsBodyHeadAI::BasicMovementAIState);
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
    return level->GetLevelUnitHeight() - this->boss->GetAliveHeight();
}

float ArmsBodyHeadAI::GetFollowAndAttackHeight() const {
    return 0.8f * this->GetMaxArmAttackYMovement() + this->boss->GetAliveHeight() / 2.0f;
}

float ArmsBodyHeadAI::GetMaxArmAttackYMovement() const {
    return 0.7f * ClassicalBoss::ARM_HEIGHT;
}

// END ArmsBodyHeadAI **************************************************************