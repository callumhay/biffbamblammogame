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

const float ClassicalBossAI::BOSS_HEIGHT = 13.0f;
const float ClassicalBossAI::BOSS_WIDTH = 25.0f;
const float ClassicalBossAI::HALF_BOSS_WIDTH = ClassicalBossAI::BOSS_WIDTH / 2.0f;

ClassicalBossAI::ClassicalBossAI(ClassicalBoss* boss) : BossAIState(), boss(boss),
currState(ClassicalBossAI::BasicMoveAndLaserSprayAIState), currVel(0.0f, 0.0f), desiredVel(0.0f, 0.0f) {
    assert(boss != NULL);
    
    // Setup the generic hurt colour animation
    {
        static const int NUM_FLASHES = 16;
        
        std::vector<double> timeValues;
        timeValues.reserve(2*NUM_FLASHES + 1);
        std::vector<ColourRGBA> colourValues;
        colourValues.reserve(timeValues.size());
        
        double timeInc = BossWeakpoint::INVULNERABLE_TIME_IN_SECS / static_cast<double>(2*NUM_FLASHES);
        double timeCount = 0.0;
        
        for (int i = 0; i <= 2*NUM_FLASHES; i++) {
            timeValues.push_back(timeCount);
            timeCount += timeInc;
        }
        for (int i = 0; i < NUM_FLASHES; i++) {
            colourValues.push_back(ColourRGBA(1.0f, 0.25f, 0.25f, 1.0f));
            colourValues.push_back(ColourRGBA(1.0f, 0.9f, 0.9f, 0.5f));
        }
        colourValues.push_back(ColourRGBA(1.0f, 1.0f, 1.0f, 1.0f));

        this->hurtColourAnim.SetLerp(timeValues, colourValues);
        this->hurtColourAnim.SetRepeat(false);
    }

    // Setup the arm fade-out animation (for when an arm falls off and dies)
    {
        static const double FIRST_FADE_OUT_TIME = 0.5;
        static const double NUM_FLASHES = 25;
        static const double TOTAL_ANIMATION_TIME = 4.0;
        static const double FLASH_TIME_INC = (TOTAL_ANIMATION_TIME - FIRST_FADE_OUT_TIME) / (2*NUM_FLASHES + 1);

        std::vector<double> timeValues;
        timeValues.reserve(2 + 2*NUM_FLASHES + 1);
        timeValues.push_back(0.0);
        timeValues.push_back(FIRST_FADE_OUT_TIME);
        for (int i = 0; i <= 2*NUM_FLASHES; i++) {
            timeValues.push_back(timeValues.back() + FLASH_TIME_INC);
        }

        static const float FIRST_ALPHA_FADE_OUT_VALUE  = 0.5f;
        static const float SECOND_ALPHA_FADE_OUT_VALUE = 0.25f;
        std::vector<ColourRGBA> alphaValues;
        alphaValues.reserve(timeValues.size());
        alphaValues.push_back(ColourRGBA(1,1,1,1));
        alphaValues.push_back(ColourRGBA(1,1,1,FIRST_ALPHA_FADE_OUT_VALUE));
        for (int i = 0; i < NUM_FLASHES; i++) {
            alphaValues.push_back(ColourRGBA(1.0f, 0.0f, 0.0f, SECOND_ALPHA_FADE_OUT_VALUE));
            alphaValues.push_back(ColourRGBA(1.0f, 0.8f, 0.8f, FIRST_ALPHA_FADE_OUT_VALUE));
        }
        alphaValues.push_back(ColourRGBA(0.0f, 0.0f, 0.0f, 0.0f));

        this->limbAlphaFadeoutAnim.SetLerp(timeValues, alphaValues);
        this->limbAlphaFadeoutAnim.SetRepeat(false);
    }

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

    if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
        currLaserDir.Rotate(ANGLE_BETWEEN_LASERS_IN_DEGS);
        gameModel->AddProjectile(new BossLaserProjectile(eyePos, currLaserDir));
    }

    // Left fan-out of lasers
    currLaserDir = initLaserDir;
    currLaserDir.Rotate(-ANGLE_BETWEEN_LASERS_IN_DEGS);
    gameModel->AddProjectile(new BossLaserProjectile(eyePos, currLaserDir));
    currLaserDir.Rotate(-ANGLE_BETWEEN_LASERS_IN_DEGS);
    gameModel->AddProjectile(new BossLaserProjectile(eyePos, currLaserDir));

    if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
        currLaserDir.Rotate(-ANGLE_BETWEEN_LASERS_IN_DEGS);
        gameModel->AddProjectile(new BossLaserProjectile(eyePos, currLaserDir));
    }
}

float ClassicalBossAI::GetBossMovementMinXBound(const GameLevel* level, float bossWidth) const {
    UNUSED_PARAMETER(level);
    return bossWidth / 1.4f + LevelPiece::PIECE_WIDTH;
}

float ClassicalBossAI::GetBossMovementMaxXBound(const GameLevel* level, float bossWidth) const {
    return level->GetLevelUnitWidth() - this->GetBossMovementMinXBound(level, bossWidth);
}

float ClassicalBossAI::GetBasicMovementHeight(const GameLevel* level) const {
    return level->GetLevelUnitHeight() - ClassicalBossAI::BOSS_HEIGHT;
}

float ClassicalBossAI::GetPrepLaserHeight(const GameLevel* level) const {
    return level->GetLevelUnitHeight() - ClassicalBossAI::BOSS_HEIGHT;
}

float ClassicalBossAI::GetMaxSpeed() const {
    return ClassicalBoss::ARMS_BODY_HEAD_MAX_SPEED;
}

Vector2D ClassicalBossAI::GetAcceleration() const {
    Vector2D accel = this->desiredVel - this->currVel;
    if (accel.IsZero()) {
        return Vector2D(0,0);
    }
    accel.Normalize();
    return ClassicalBoss::ARMS_BODY_HEAD_ACCELERATION * accel;
}

void ClassicalBossAI::UpdateMovement(double dT, GameModel* gameModel) {
    // Figure out how much to move and update the position of the boss
    if (!this->currVel.IsZero()) {
        
        Vector2D dMovement = dT * this->currVel;
        this->boss->alivePartsRoot->Translate(Vector3D(dMovement));

        // Update the position of the boss based on whether it is now colliding with the boundaries/walls of the level
        const GameLevel* level = gameModel->GetCurrentLevel();
        Vector2D correctionVec;
        if (level->CollideBossWithLevel(this->boss->alivePartsRoot->GenerateWorldAABB(), correctionVec)) {
            
            Vector3D correctionVec3D(correctionVec);
            this->boss->alivePartsRoot->Translate(correctionVec3D);
        }
    }

    // Update the speed based on the acceleration
    this->currVel = this->currVel + dT * this->GetAcceleration();
}

// END ClassicalBossAI *************************************************************

// BEGIN ArmsBodyHeadAI ************************************************************

const float ArmsBodyHeadAI::ARM_HEIGHT = 9.66f;
const float ArmsBodyHeadAI::ARM_WIDTH  = 3.097f;

const float ArmsBodyHeadAI::ARM_LIFE_POINTS = 200.0f;
const float ArmsBodyHeadAI::ARM_BALL_DAMAGE = 100.0f;

const double ArmsBodyHeadAI::LASER_SPRAY_RESET_TIME_IN_SECS = 1.5;

const double ArmsBodyHeadAI::ARM_ATTACK_DELTA_T = 0.4;

ArmsBodyHeadAI::ArmsBodyHeadAI(ClassicalBoss* boss) : ClassicalBossAI(boss),
leftArm(NULL), rightArm(NULL), leftArmSqrWeakpt(NULL), rightArmSqrWeakpt(NULL),
countdownToNextState(0.0), countdownToAttack(0.0), laserSprayCountdown(0.0), isAttackingWithArm(false),
nextAttackState(ClassicalBossAI::AttackBothArmsAIState), temptAttackCountdown(0.0), countdownToLaserBarrage(0.0), lastRecourceLaserCountdown(0.0) {
    
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

    // Setup laser barrage angles
    const float angleIncrement = 17;
    this->laserAngles.reserve(5);
    this->laserAngles.push_back(-2*angleIncrement);
    this->laserAngles.push_back(-angleIncrement);
    this->laserAngles.push_back(0);
    this->laserAngles.push_back(angleIncrement);
    this->laserAngles.push_back(2*angleIncrement);

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
        this->armShakeAnim.SetRepeat(true);
    }

    // Setup the arm attack animation
    {
        std::vector<double> timeValues;
        timeValues.reserve(5);
        timeValues.push_back(0.0);
        timeValues.push_back(ARM_ATTACK_DELTA_T);
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

    {
        static const double FINAL_JITTER_TIME = 0.3;
        static const double SHAKE_INC_TIME = 0.075;
        static const int NUM_SHAKES = (BossWeakpoint::INVULNERABLE_TIME_IN_SECS - FINAL_JITTER_TIME) / (2*SHAKE_INC_TIME + SHAKE_INC_TIME);

        std::vector<double> timeValues;
        timeValues.clear();
        timeValues.reserve(3 + NUM_SHAKES + 1);
        timeValues.push_back(0.0);
        timeValues.push_back(2.0 * FINAL_JITTER_TIME / 3.0);
        timeValues.push_back(FINAL_JITTER_TIME);
        for (int i = 0; i <= NUM_SHAKES*2; i++) {
            timeValues.push_back(timeValues.back() + SHAKE_INC_TIME);
        }
        assert(timeValues.back() <= BossWeakpoint::INVULNERABLE_TIME_IN_SECS);

        float hurtXPos = BOSS_WIDTH/5.0f;
        std::vector<Vector3D> moveValues;
        moveValues.reserve(timeValues.size());
        moveValues.push_back(Vector3D(0,0,0));
        moveValues.push_back(Vector3D(hurtXPos, LevelPiece::PIECE_HEIGHT, 0.0f));
        moveValues.push_back(Vector3D(0,0,0));
        for (int i = 0; i < NUM_SHAKES; i++) {
            moveValues.push_back(Vector3D(BOSS_WIDTH/30.0f, 0, 0));
            moveValues.push_back(Vector3D(-BOSS_WIDTH/30.0f, 0, 0));
        }
        moveValues.push_back(Vector3D(0.0f, 0.0f, 0.0f));
        
        this->leftArmHurtMoveAnim.SetLerp(timeValues, moveValues);
        this->leftArmHurtMoveAnim.SetRepeat(false);

        for (int i = 0; i < static_cast<int>(moveValues.size()); i++) {
            moveValues[i][0] *= -1;
        }
        this->rightArmHurtMoveAnim.SetLerp(timeValues, moveValues);
        this->rightArmHurtMoveAnim.SetRepeat(false);
    }

    {
        static const double COLOUR_FLASH_TIME = 0.20;
        
        std::vector<double> timeVals;
        timeVals.reserve(5);
        timeVals.push_back(0.0);
        timeVals.push_back(COLOUR_FLASH_TIME);
        timeVals.push_back(2*COLOUR_FLASH_TIME);
        timeVals.push_back(3*COLOUR_FLASH_TIME);
        timeVals.push_back(4*COLOUR_FLASH_TIME);

        std::vector<ColourRGBA> colourVals;
        colourVals.reserve(timeVals.size());
        colourVals.push_back(ColourRGBA(1,1,1,1));
        colourVals.push_back(ColourRGBA(1,0,0,1));
        colourVals.push_back(ColourRGBA(1,1,0,1));
        colourVals.push_back(ColourRGBA(1,0,0,1));
        colourVals.push_back(ColourRGBA(1,1,1,1));

        this->angryColourAnim.SetLerp(timeVals, colourVals);
        this->angryColourAnim.SetRepeat(true);
        
        static const int NUM_SHAKES = 15;
        static const double SHAKE_INC_TIME = 0.03;
        timeVals.clear();
        timeVals.reserve(1 + NUM_SHAKES + 1);
        timeVals.push_back(0.0);
        for (int i = 0; i <= NUM_SHAKES*2; i++) {
            timeVals.push_back(timeVals.back() + SHAKE_INC_TIME);
        }

        std::vector<Vector3D> moveVals;
        moveVals.reserve(timeVals.size());
        moveVals.push_back(Vector3D(0,0,0));
        for (int i = 0; i < NUM_SHAKES; i++) {
            float randomNum1 = Randomizer::GetInstance()->RandomNumNegOneToOne() * BOSS_WIDTH/20.0f;
            float randomNum2 = Randomizer::GetInstance()->RandomNumNegOneToOne() * BOSS_WIDTH/20.0f;
            moveVals.push_back(Vector3D(randomNum1, randomNum2, 0));
            moveVals.push_back(Vector3D(-randomNum1, -randomNum2, 0));
        }
        moveVals.push_back(Vector3D(0.0f, 0.0f, 0.0f));
        
        this->angryMoveAnim.SetLerp(timeVals, moveVals);
        this->angryMoveAnim.SetRepeat(false);
    }

    this->SetState(ClassicalBossAI::BasicMoveAndLaserSprayAIState);
}

ArmsBodyHeadAI::~ArmsBodyHeadAI() {
}

void ArmsBodyHeadAI::CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart) {
    UNUSED_PARAMETER(ball);
    UNUSED_PARAMETER(gameModel);

    assert(collisionPart != NULL);

    // Check to see if the ball hit one of the weakspots...
    if (collisionPart == this->rightArmSqrWeakpt) {
        this->leftArm->SetLocalTranslation(Vector3D(0,0,0));
        this->rightArm->SetLocalTranslation(Vector3D(0,0,0));

        // Check to see if the arm is dead, if it is then we change its state in the body of the boss
        if (this->rightArmSqrWeakpt->GetIsDestroyed()) {
            this->boss->ConvertAliveBodyPartToDeadBodyPart(boss->rightArmIdx);
            
            // Animate the right arm to fade out and "fall off" the body
            this->rightArm->AnimateColourRGBA(this->limbAlphaFadeoutAnim);

            AnimationMultiLerp<Vector3D> armDeathTransAnim = this->GenerateArmDeathTranslationAnimation(false);
            this->rightArm->AnimateLocalTranslation(armDeathTransAnim);

            AnimationMultiLerp<float> armDeathRotAnim = this->GenerateArmDeathRotationAnimation(false);
            this->rightArm->AnimateLocalZRotation(armDeathRotAnim);
        }
        if (this->currState != ClassicalBossAI::HurtRightArmAIState) {
            this->SetState(ClassicalBossAI::HurtRightArmAIState);
        }
    }
    else if (collisionPart == this->leftArmSqrWeakpt) {

        this->leftArm->SetLocalTranslation(Vector3D(0,0,0));
        this->rightArm->SetLocalTranslation(Vector3D(0,0,0));

        // Check to see if the arm is dead, if it is then we change its state in the body of the boss
        if (this->leftArmSqrWeakpt->GetIsDestroyed()) {
            this->boss->ConvertAliveBodyPartToDeadBodyPart(boss->leftArmIdx);
            
            // Animate the left arm to fade out and "fall off" the body
            this->leftArm->AnimateColourRGBA(this->limbAlphaFadeoutAnim);

            AnimationMultiLerp<Vector3D> armDeathTransAnim = this->GenerateArmDeathTranslationAnimation(true);
            this->leftArm->AnimateLocalTranslation(armDeathTransAnim);

            AnimationMultiLerp<float> armDeathRotAnim = this->GenerateArmDeathRotationAnimation(true);
            this->leftArm->AnimateLocalZRotation(armDeathRotAnim);
        }

        if (this->currState != ClassicalBossAI::HurtLeftArmAIState) {
            this->SetState(ClassicalBossAI::HurtLeftArmAIState);
        }
    }
}

void ArmsBodyHeadAI::CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart) {
    UNUSED_PARAMETER(gameModel);
    UNUSED_PARAMETER(projectile);
    UNUSED_PARAMETER(collisionPart);
}

void ArmsBodyHeadAI::CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle, BossBodyPart* collisionPart) {
    UNUSED_PARAMETER(gameModel);
    UNUSED_PARAMETER(paddle);
    UNUSED_PARAMETER(collisionPart);
}

void ArmsBodyHeadAI::SetState(ClassicalBossAI::AIState newState) {

    switch (newState) {

        case ClassicalBossAI::BasicMoveAndLaserSprayAIState:
            debug_output("Entering BasicMoveAndLaserSprayAIState");
            this->laserSprayCountdown  = ArmsBodyHeadAI::LASER_SPRAY_RESET_TIME_IN_SECS;
            this->countdownToNextState = this->GenerateBasicMoveTime();
            break;

        case ClassicalBossAI::ChasePaddleAIState:
            debug_output("Entering ChasePaddleAIState");
            this->armShakeAnim.ResetToStart();
            this->armShakeAnim.SetRepeat(true);
            this->temptAttackCountdown = this->GenerateTemptAttackTime();
            this->countdownToAttack    = this->GenerateChaseTime();
            this->lastRecourceLaserCountdown = this->GetTimeBetweenLaserBarrageShots();
            break;

        case ClassicalBossAI::AttackLeftArmAIState:
        case ClassicalBossAI::AttackRightArmAIState:
        case ClassicalBossAI::AttackBothArmsAIState:
            debug_output("Entering AttackxArmAIState");
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->armShakeAnim.ResetToStart();
            this->armShakeAnim.SetRepeat(false);
            this->armAttackYMovementAnim.ResetToStart();
            this->armAttackYMovementAnim.SetRepeat(false);
            break;

        case ClassicalBossAI::PrepLaserAIState:
            debug_output("Entering PrepLaserAIState");
            this->countdownToLaserBarrage = this->GetLaserChargeTime();
            this->movingDir = Randomizer::GetInstance()->RandomNegativeOrPositive();
            break;
        case ClassicalBossAI::MoveAndBarrageWithLaserAIState:
            debug_output("Entering MoveAndBarrageWithLaserAIState");
            this->laserShootTimer = 0.0;
            this->movingDir = -this->movingDir;
            break;

        case ClassicalBossAI::HurtLeftArmAIState:
            debug_output("Entering HurtLeftArmAIState");
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->hurtColourAnim.ResetToStart();
            this->boss->alivePartsRoot->AnimateColourRGBA(this->hurtColourAnim);
            this->leftArmHurtMoveAnim.ResetToStart();
            break;
        case ClassicalBossAI::HurtRightArmAIState:
            debug_output("Entering HurtRightArmAIState");
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->hurtColourAnim.ResetToStart();
            this->boss->alivePartsRoot->AnimateColourRGBA(this->hurtColourAnim);
            this->rightArmHurtMoveAnim.ResetToStart();
            break;

        case ClassicalBossAI::LostArmsAngryAIState:
            debug_output("Entering LostArmsAngryAIState");
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->angryColourAnim.ResetToStart();
            this->boss->alivePartsRoot->AnimateColourRGBA(this->angryColourAnim);
            this->angryMoveAnim.ResetToStart();

            // TODO: Signal an event for boss "GRRRRAAWWRRR"...
            
            break;

        default:
            assert(false);
            break;
    }

    this->currState = newState;
}

void ArmsBodyHeadAI::UpdateState(double dT, GameModel* gameModel) {

    switch (this->currState) {

        case ClassicalBossAI::BasicMoveAndLaserSprayAIState:
            this->ExecuteBasicMoveAndLaserSprayState(dT, gameModel);
            break;

        case ClassicalBossAI::ChasePaddleAIState:
            this->ExecuteChasePaddleState(dT, gameModel);
            break;

        case ClassicalBossAI::AttackLeftArmAIState:
            this->ExecuteArmAttackState(dT, true, false);
            break;
        case ClassicalBossAI::AttackRightArmAIState:
            this->ExecuteArmAttackState(dT, false, true);
            break;
        case ClassicalBossAI::AttackBothArmsAIState:
            this->ExecuteArmAttackState(dT, true, true);
            break;

        case ClassicalBossAI::PrepLaserAIState:
            this->ExecutePrepLaserState(dT, gameModel);
            break;
        case ClassicalBossAI::MoveAndBarrageWithLaserAIState:
            this->ExecuteMoveAndBarrageWithLaserState(dT, gameModel);
            break;

        case ClassicalBossAI::HurtLeftArmAIState:
            this->ExecuteHurtArmState(dT, true);
            break;
        case ClassicalBossAI::HurtRightArmAIState:
            this->ExecuteHurtArmState(dT, false);
            break;

        case ClassicalBossAI::LostArmsAngryAIState:
            this->ExecuteArmsLostAngryState(dT);
            break;

        default:
            assert(false);
            break;
    }
}


void ArmsBodyHeadAI::ExecuteBasicMoveAndLaserSprayState(double dT, GameModel* gameModel) {
    assert(gameModel != NULL);

    const GameLevel* level = gameModel->GetCurrentLevel();
    assert(level != NULL);

    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();

    // The basic movement state occurs further away from the paddle, closer to the top of the level.
    // Make sure we've moved up high enough...
    float avgBasicMoveHeight = this->GetBasicMovementHeight(level);
    float upDownDistance     = ClassicalBossAI::BOSS_HEIGHT / 6.0f;

    if (bossPos[1] < avgBasicMoveHeight - upDownDistance) {
        this->desiredVel[1] = this->GetMaxSpeed() / 3.0f;
    }
    else if (bossPos[1] > avgBasicMoveHeight + upDownDistance) {
        this->desiredVel[1] = -this->GetMaxSpeed() / 3.0f;
    }
    
    if (fabs(this->desiredVel[1]) < EPSILON) {
        this->desiredVel[1] = this->GetMaxSpeed() / 3.0f;
    }
        
    // Side-to-side movement is basic back and forth linear translation...
    if (bossPos[0] <= this->GetBossMovementMinXBound(level, BOSS_WIDTH)) {
        this->desiredVel[0] = this->GetMaxSpeed() / 2.0f;
    }
    else if (bossPos[0] >= this->GetBossMovementMaxXBound(level, BOSS_WIDTH)) {
        this->desiredVel[0] = -this->GetMaxSpeed() / 2.0f;
    }
    
    if (fabs(this->desiredVel[0]) < EPSILON) {
        this->desiredVel[0] = Randomizer::GetInstance()->RandomNegativeOrPositive() * this->GetMaxSpeed() / 2.0f;
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
            this->SetState(ClassicalBossAI::ChasePaddleAIState);
        }
        else {
            this->currVel    = Vector2D(0,0);
            this->desiredVel = Vector2D(0,0);
            this->SetState(ClassicalBossAI::PrepLaserAIState);
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

void ArmsBodyHeadAI::ExecuteChasePaddleState(double dT, GameModel* gameModel) {
    assert(gameModel != NULL);

    const PlayerPaddle* paddle = gameModel->GetPlayerPaddle();
    assert(paddle != NULL);
    const GameLevel* level = gameModel->GetCurrentLevel();
    assert(level != NULL);

    const Point2D& paddlePos = paddle->GetCenterPosition();
    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();

    bool allowedToAttack = false;
    bool leftArmIsDestroyed  = this->leftArmSqrWeakpt->GetIsDestroyed();
    bool rightArmIsDestroyed = this->rightArmSqrWeakpt->GetIsDestroyed();

    // Make sure the boss is at the right height from the paddle...
    if (bossPos[1] > this->GetFollowAndAttackHeight()) {
        this->desiredVel[1] = -this->GetMaxSpeed() / 2.0f;
    }
    else {
        this->currVel[1]    = 0.0f;
        this->desiredVel[1] = 0.0f;
        allowedToAttack = true;
    }
    
    // Find the vector from boss to paddle, and project onto the x-axis to figure out the movement direction
    Vector2D bossToPaddleVec = paddlePos - bossPos;
    
    // Check to see if we can attack yet (If we're anywhere near the paddle then perform the attack)
    
    Collision::AABB2D paddleAABB   = paddle->GetBounds().GenerateAABBFromLines();
    Collision::AABB2D leftArmAABB  = this->leftArmSqrWeakpt->GetWorldBounds().GenerateAABBFromLines();
    Collision::AABB2D rightArmAABB = this->rightArmSqrWeakpt->GetWorldBounds().GenerateAABBFromLines();

    bool paddleUnderLeftArm = !leftArmIsDestroyed &&
        ((paddleAABB.GetMin()[0] >= leftArmAABB.GetMin()[0] && paddleAABB.GetMin()[0] <= leftArmAABB.GetMax()[0]) || 
        (paddleAABB.GetMax()[0] <= leftArmAABB.GetMax()[0] && paddleAABB.GetMax()[0] >= leftArmAABB.GetMin()[0]));

    bool paddleUnderRightArm = !rightArmIsDestroyed &&
        ((paddleAABB.GetMin()[0] >= rightArmAABB.GetMin()[0] && paddleAABB.GetMin()[0] <= rightArmAABB.GetMax()[0]) || 
        (paddleAABB.GetMax()[0] <= rightArmAABB.GetMax()[0] && paddleAABB.GetMax()[0] >= rightArmAABB.GetMin()[0]));

    if (paddleUnderLeftArm || paddleUnderRightArm) {

        this->desiredVel[0] = 0.0f;
        this->currVel[0]    = 0.0f;

        if (allowedToAttack) {

            // Use a timer here to make the player 'tempt' the boss to attack with its arm(s)
            if (this->temptAttackCountdown <= 0.0) {
                
                if (!this->leftArmSqrWeakpt->GetIsDestroyed()) {
                    this->leftArm->SetLocalTranslation(Vector3D(0.0f, 0.0f, 0.0f));
                }

                if (!this->rightArmSqrWeakpt->GetIsDestroyed()) {
                    this->rightArm->SetLocalTranslation(Vector3D(0.0f, 0.0f, 0.0f));
                }

                this->SetState(this->nextAttackState);
                return;
            }
            else {
                // Shake the appropriate arm and set it up as the next attack state...
                this->nextAttackState = this->DetermineNextArmAttackState(bossToPaddleVec);

                float shakeAmt = this->armShakeAnim.GetInterpolantValue();
                this->armShakeAnim.Tick(dT);

                if (!this->leftArmSqrWeakpt->GetIsDestroyed() && this->nextAttackState == AttackLeftArmAIState ||
                    this->nextAttackState == AttackBothArmsAIState) {
                    this->leftArm->SetLocalTranslation(Vector3D(shakeAmt, 0.0f, 0.0f));
                }
                if (!this->rightArmSqrWeakpt->GetIsDestroyed() && this->nextAttackState == AttackRightArmAIState ||
                    this->nextAttackState == AttackBothArmsAIState) {
                    this->rightArm->SetLocalTranslation(Vector3D(shakeAmt, 0.0f, 0.0f));
                }
            }
            this->temptAttackCountdown -= dT;
        }
    }
    else {
        if (!leftArmIsDestroyed) {
            this->leftArm->SetLocalTranslation(Vector3D(0.0f, 0.0f, 0.0f));
        }
        if (!rightArmIsDestroyed) {
            this->rightArm->SetLocalTranslation(Vector3D(0.0f, 0.0f, 0.0f));
        }

        // If one of the bosses' arms is destroyed it should be trying to get its last surviving arm to
        // be above the paddle...
        if (leftArmIsDestroyed) {
            // Try to get the paddle under the right arm
            Vector2D rightArmToPaddle = paddlePos - rightArmAABB.GetCenter();
            this->desiredVel[0] = NumberFuncs::SignOf(rightArmToPaddle[0]) * this->GetMaxSpeed();
        }
        else if (rightArmIsDestroyed) {
            // Try to get the paddle under the left arm
            Vector2D leftArmToPaddle = paddlePos - leftArmAABB.GetCenter();
            this->desiredVel[0] = NumberFuncs::SignOf(leftArmToPaddle[0]) * this->GetMaxSpeed();
        }
        else {
            // ... otherwise we just chase the paddle around...

            float absBossToPaddleXDist = fabs(bossToPaddleVec[0]);
            if (absBossToPaddleXDist > ClassicalBoss::ARM_X_TRANSLATION_FROM_CENTER + ClassicalBoss::HALF_ARM_WIDTH) {
                this->desiredVel[0] = NumberFuncs::SignOf(bossToPaddleVec[0]) * this->GetMaxSpeed();
            }
            else {
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
        }
    }

    // Make sure the boss isn't too far to either side...
    bool bossStuckAtSide = false;
    if (bossPos[0] < this->GetBossMovementMinXBound(level, BOSS_WIDTH)) {
        this->desiredVel[0] = 0.0f;
        this->currVel[0] = 0.0f;
        this->boss->alivePartsRoot->Translate(Vector3D(this->GetBossMovementMinXBound(level, BOSS_WIDTH) - bossPos[0], 0.0f, 0.0f));
        bossStuckAtSide = true;

    }
    else if (bossPos[0] > this->GetBossMovementMaxXBound(level, BOSS_WIDTH)) {
        this->desiredVel[0] = 0.0f;
        this->currVel[0] = 0.0f;
        this->boss->alivePartsRoot->Translate(Vector3D(this->GetBossMovementMaxXBound(level, BOSS_WIDTH) - bossPos[0], 0.0f, 0.0f));
        bossStuckAtSide = true;
    }

    if (bossStuckAtSide) {
        if (!paddleUnderLeftArm && !paddleUnderRightArm) {
            // The player is playing hard to get, shoot at them with lasers
            if (this->lastRecourceLaserCountdown <= 0.0) {
                // Fire ze lazor!!
                Point2D eyePos = boss->GetEye()->GetTranslationPt2D();
                gameModel->AddProjectile(new BossLaserProjectile(eyePos, Vector2D::Normalize(paddlePos - eyePos)));
                this->lastRecourceLaserCountdown = this->GetTimeBetweenLaserBarrageShots();
            }
            else {
                this->lastRecourceLaserCountdown -= dT;
            }
        }
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
    if (this->temptAttackCountdown > 0 && !armIsDoneShaking) {
        float shakingMovement = this->armShakeAnim.GetInterpolantValue();

        // Animate the arm we are attacking with -- it's shaking to indicate the incoming attack
        if (!this->leftArmSqrWeakpt->GetIsDestroyed() && isLeftArmAttacking) {
            this->leftArm->SetLocalTranslation(Vector3D(shakingMovement, 0.0f, 0.0f));
        }
        if (!this->rightArmSqrWeakpt->GetIsDestroyed() && isRightArmAttacking) {
            this->rightArm->SetLocalTranslation(Vector3D(shakingMovement, 0.0f, 0.0f));
        }
    }
    else {
        bool attackIsDone = this->armAttackYMovementAnim.Tick(dT);

        // We are attacking with the arm!!!
        
        // Move the arm based on the attack animation
        float armYMovement = this->armAttackYMovementAnim.GetInterpolantValue();
        if (!this->leftArmSqrWeakpt->GetIsDestroyed() && isLeftArmAttacking) {
            this->leftArm->SetLocalTranslation(Vector3D(0.0f, armYMovement, 0.0f));
        }
        if (!this->rightArmSqrWeakpt->GetIsDestroyed() && isRightArmAttacking) {
            this->rightArm->SetLocalTranslation(Vector3D(0.0f, armYMovement, 0.0f));
        }

        // If the arm is moving downward then we are in the midst of attacking, set a flag to
        // indicate that the boss can hurt the paddle while it attacks
        if (this->armAttackYMovementAnim.GetCurrentTimeValue() <= ARM_ATTACK_DELTA_T) {
            this->isAttackingWithArm = true;
        }
        else {
            this->isAttackingWithArm = false;
        }
        
        if (attackIsDone) {
            this->isAttackingWithArm = false;

            if (!this->leftArmSqrWeakpt->GetIsDestroyed()) {
                this->leftArm->SetLocalTranslation(Vector3D(0.0f, 0.0f, 0.0f));
            }
            if (!this->rightArmSqrWeakpt->GetIsDestroyed()) {
                this->rightArm->SetLocalTranslation(Vector3D(0.0f, 0.0f, 0.0f));
            }

            this->SetState(ClassicalBossAI::BasicMoveAndLaserSprayAIState);
            return;
        }
    }
}

void ArmsBodyHeadAI::ExecutePrepLaserState(double dT, GameModel* gameModel) {
    assert(gameModel != NULL);

    const GameLevel* level = gameModel->GetCurrentLevel();
    assert(level != NULL);

    // Make sure the boss is at the proper height in the level for firing the laser
    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();

    bool allowedToAttack = true;

    // Make sure the boss is at the correct height...
    const float laserPrepHeight = this->GetPrepLaserHeight(level);
    if (bossPos[1] > laserPrepHeight + LevelPiece::PIECE_HEIGHT) {
        this->desiredVel[1] = -this->GetMaxSpeed() / 3.0f;
        allowedToAttack = false;
    }
    else if (bossPos[1] < laserPrepHeight - LevelPiece::PIECE_HEIGHT) {
        this->desiredVel[1] = this->GetMaxSpeed() / 3.0f;
        allowedToAttack = false;
    }

    // Make sure the boss is at the correct side of the level
    if (bossPos[0] > this->GetBossMovementMinXBound(level, BOSS_WIDTH) && 
        bossPos[0] < this->GetBossMovementMaxXBound(level, BOSS_WIDTH)) {

        this->desiredVel[0] = this->movingDir * this->GetMaxSpeed();
        allowedToAttack = false;
    }

    if (allowedToAttack) {
        this->currVel    = Vector2D(0.0f, 0.0f);
        this->desiredVel = Vector2D(0.0f, 0.0f);

        // Charge up the laser barrage...
        if (this->countdownToLaserBarrage <= 0.0) {
            this->SetState(ClassicalBossAI::MoveAndBarrageWithLaserAIState);
        }
        else {
            if (this->countdownToLaserBarrage == this->GetLaserChargeTime()) {
                // EVENT: Charging for laser barrage...
                // TODO
            }
            this->countdownToLaserBarrage -= dT;
        }
    }
}


void ArmsBodyHeadAI::ExecuteMoveAndBarrageWithLaserState(double dT, GameModel* gameModel) {
    assert(gameModel != NULL);

    const GameLevel* level = gameModel->GetCurrentLevel();
    assert(level != NULL);

    this->desiredVel[1] = 0.0f;
    this->currVel[1] = 0.0f;

    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();

    // Start moving across the entire screen firing lasers
    bool finishedMoving = movingDir < 0 && bossPos[0] <= this->GetBossMovementMinXBound(level, BOSS_WIDTH) ||
                          movingDir > 0 && bossPos[0] >= this->GetBossMovementMaxXBound(level, BOSS_WIDTH);

    if (finishedMoving) {
        this->desiredVel[0] = 0.0f;
        this->currVel[0] = 0.0f;

        // We're finished moving, leave this state
        this->SetState(ClassicalBossAI::ChasePaddleAIState);
    }
    else {
        this->desiredVel[0] = this->movingDir * this->GetMaxSpeed() / 5.5f;
    }

    // Determine whether we're shooting a laser
    if (this->laserShootTimer <= 0.0) {
        // Fire a laser downwards within a small cone
        Vector2D laserDir(0, -1);
        laserDir.Rotate(this->laserAngles[Randomizer::GetInstance()->RandomUnsignedInt() % this->laserAngles.size()]);

        Point2D eyePos = this->boss->GetEye()->GetTranslationPt2D();
        gameModel->AddProjectile(new BossLaserProjectile(eyePos, laserDir));

        this->laserShootTimer = this->GetTimeBetweenLaserBarrageShots();
    }
    else {
        this->laserShootTimer -= dT;
    }

}

void ArmsBodyHeadAI::ExecuteHurtArmState(double dT, bool isLeftArm) {
    this->currVel = Vector2D(0,0);
    this->desiredVel = Vector2D(0,0);

    bool isFinished = false;
    if (isLeftArm) {
        isFinished = this->leftArmHurtMoveAnim.Tick(dT);
        this->boss->alivePartsRoot->SetLocalTranslation(this->leftArmHurtMoveAnim.GetInterpolantValue());
    }
    else {
        isFinished = this->rightArmHurtMoveAnim.Tick(dT);
        this->boss->alivePartsRoot->SetLocalTranslation(this->rightArmHurtMoveAnim.GetInterpolantValue());
    }

    if (isFinished) {
        this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0,0,0));
        this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0,0,0));

        // Check to see if both arms are now gone, if they are then we move to a special state
        if (this->leftArm->GetIsDestroyed() && this->rightArm->GetIsDestroyed()) {
            this->SetState(ClassicalBossAI::LostArmsAngryAIState);
        }
        else {
            // If the boss is missing an arm then we go into basic move and laser spray state
            if (this->leftArm->GetIsDestroyed() || this->rightArm->GetIsDestroyed()) {
                this->SetState(ClassicalBossAI::BasicMoveAndLaserSprayAIState);
            }
            else {
                // ... otherwise we continue with the current AI routine by choosing a random state from it...
                if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
                    this->SetState(ClassicalBossAI::ChasePaddleAIState);
                }
                else {
                    this->SetState(ClassicalBossAI::BasicMoveAndLaserSprayAIState);
                }
            }
        }
    }
}

void ArmsBodyHeadAI::ExecuteArmsLostAngryState(double dT) {
    // No velocity
    this->currVel    = Vector2D(0,0);
    this->desiredVel = Vector2D(0,0);

    bool isFinished = this->angryMoveAnim.Tick(dT);
    this->boss->alivePartsRoot->SetLocalTranslation(this->angryMoveAnim.GetInterpolantValue());
    
    if (isFinished) {
        // Stop any colour animation that was set for this state
        this->boss->alivePartsRoot->ResetColourRGBAAnimation();
        // Clean up all the translations on the body of the boss for this state
        this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0,0,0));

        // Go to the next high-level AI state
        this->boss->SetNextAIState(new BodyHeadAI(this->boss));
    }
}


float ArmsBodyHeadAI::GetFollowAndAttackHeight() const {
    return 0.8f * this->GetMaxArmAttackYMovement() + ClassicalBossAI::BOSS_HEIGHT / 2.0f;
}

float ArmsBodyHeadAI::GetMaxArmAttackYMovement() const {
    return 0.7f * ClassicalBoss::ARM_HEIGHT;
}

ClassicalBossAI::AIState ArmsBodyHeadAI::DetermineNextArmAttackState(const Vector2D& bossToPaddleVec) const {

    // If one of the arms is destroyed then we have to return the other...
    if (this->leftArmSqrWeakpt->GetIsDestroyed()) {
        assert(!this->rightArmSqrWeakpt->GetIsDestroyed());
        return ClassicalBossAI::AttackRightArmAIState;
    }
    else if (this->rightArmSqrWeakpt->GetIsDestroyed()) {
        return ClassicalBossAI::AttackLeftArmAIState;
    }

    // How much damage has been done so far? These two percentages will maximally add to 2.0
    float percentLeftArm  = this->leftArmSqrWeakpt->GetCurrentLifePercentage();
    float percentRightArm = this->rightArmSqrWeakpt->GetCurrentLifePercentage();
    float totalLifePercent = (percentLeftArm + percentRightArm) / 2.0f;

    bool doBothArmAttack = false;
    if (totalLifePercent < 0.75) {
        doBothArmAttack = true;
    }

    if (doBothArmAttack) {
        return ClassicalBossAI::AttackBothArmsAIState;
    }
    else {
        if (bossToPaddleVec[0] < 0) {
            return ClassicalBossAI::AttackLeftArmAIState;
        }
        else {
            return ClassicalBossAI::AttackRightArmAIState;
        }
    }
}

AnimationMultiLerp<Vector3D> ArmsBodyHeadAI::GenerateArmDeathTranslationAnimation(bool isLeftArm) const {
    double lastTimeValue = this->limbAlphaFadeoutAnim.GetTimeValues().back();
    
    std::vector<double> timeValues;
    timeValues.reserve(2);
    timeValues.push_back(0.0);
    timeValues.push_back(lastTimeValue);

    Collision::AABB2D bossArmAABB = isLeftArm ? this->leftArm->GenerateWorldAABB() : this->rightArm->GenerateWorldAABB();

    std::vector<Vector3D> moveValues;
    moveValues.reserve(timeValues.size());
    moveValues.push_back(Vector3D(0,0,0));
    moveValues.push_back(Vector3D((isLeftArm ? -1 : 1) * 5 * ArmsBodyHeadAI::ARM_WIDTH, 
        -2*ArmsBodyHeadAI::ARM_HEIGHT - this->boss->alivePartsRoot->GetTranslationPt2D()[1], 0.0f));

    AnimationMultiLerp<Vector3D> armDeathTransAnim;
    armDeathTransAnim.SetLerp(timeValues, moveValues);
    armDeathTransAnim.SetRepeat(false);
    armDeathTransAnim.SetInterpolantValue(Vector3D(0,0,0));

    return armDeathTransAnim;
}

AnimationMultiLerp<float> ArmsBodyHeadAI::GenerateArmDeathRotationAnimation(bool isLeftArm) const {
    double lastTimeValue = this->limbAlphaFadeoutAnim.GetTimeValues().back();
    std::vector<double> timeValues;
    timeValues.reserve(3);
    timeValues.push_back(0.0);
    timeValues.push_back(lastTimeValue / 2.0);
    timeValues.push_back(lastTimeValue);

    std::vector<float> rotationValues;
    rotationValues.reserve(timeValues.size());
    rotationValues.push_back(0.0f);
    rotationValues.push_back(isLeftArm ? 90.0f : -90.0f);
    rotationValues.push_back(isLeftArm ? 100.0f : -100.0f);

    AnimationMultiLerp<float> armDeathRotAnim;
    armDeathRotAnim.SetLerp(timeValues, rotationValues);
    armDeathRotAnim.SetRepeat(false);
    armDeathRotAnim.SetInterpolantValue(0.0f);

    return armDeathRotAnim;
}

// END ArmsBodyHeadAI **************************************************************


// BEGIN BodyHeadAI ****************************************************************

const float BodyHeadAI::COLUMN_LIFE_POINTS = 100.0f;
const float BodyHeadAI::COLUMN_BALL_DAMAGE = 100.0f;

const double BodyHeadAI::LASER_SPRAY_RESET_TIME_IN_SECS = 1.5;

BodyHeadAI::BodyHeadAI(ClassicalBoss* boss) : 
ClassicalBossAI(boss), countdownToNextState(0.0), laserSprayCountdown(0.0), numConsecutiveTimesBasicMoveExecuted(0),
numConsecutiveBarrages(0), laserShootTimer(0) {

    // Make all of the body columns into weakpoints on the boss
    boss->ConvertAliveBodyPartToWeakpoint(boss->leftCol1Idx,  BodyHeadAI::COLUMN_LIFE_POINTS, BodyHeadAI::COLUMN_BALL_DAMAGE);
    boss->ConvertAliveBodyPartToWeakpoint(boss->leftCol2Idx,  BodyHeadAI::COLUMN_LIFE_POINTS, BodyHeadAI::COLUMN_BALL_DAMAGE);
    boss->ConvertAliveBodyPartToWeakpoint(boss->leftCol3Idx,  BodyHeadAI::COLUMN_LIFE_POINTS, BodyHeadAI::COLUMN_BALL_DAMAGE);
    boss->ConvertAliveBodyPartToWeakpoint(boss->rightCol1Idx, BodyHeadAI::COLUMN_LIFE_POINTS, BodyHeadAI::COLUMN_BALL_DAMAGE);
    boss->ConvertAliveBodyPartToWeakpoint(boss->rightCol2Idx, BodyHeadAI::COLUMN_LIFE_POINTS, BodyHeadAI::COLUMN_BALL_DAMAGE);
    boss->ConvertAliveBodyPartToWeakpoint(boss->rightCol3Idx, BodyHeadAI::COLUMN_LIFE_POINTS, BodyHeadAI::COLUMN_BALL_DAMAGE);

    columnWeakpts.reserve(6);
    columnWeakpts.push_back(static_cast<BossWeakpoint*>(boss->bodyParts[boss->leftCol1Idx]));
    columnWeakpts.push_back(static_cast<BossWeakpoint*>(boss->bodyParts[boss->leftCol2Idx]));
    columnWeakpts.push_back(static_cast<BossWeakpoint*>(boss->bodyParts[boss->leftCol3Idx]));
    columnWeakpts.push_back(static_cast<BossWeakpoint*>(boss->bodyParts[boss->rightCol1Idx]));
    columnWeakpts.push_back(static_cast<BossWeakpoint*>(boss->bodyParts[boss->rightCol2Idx]));
    columnWeakpts.push_back(static_cast<BossWeakpoint*>(boss->bodyParts[boss->rightCol3Idx]));

    // Setup the movement animation for when the boss loses a column
    {
        static const double SHAKE_INC_TIME = 0.075;
        static const int NUM_SHAKES = BossWeakpoint::INVULNERABLE_TIME_IN_SECS / (2*SHAKE_INC_TIME + SHAKE_INC_TIME);

        std::vector<double> timeValues;
        timeValues.clear();
        timeValues.reserve(1 + NUM_SHAKES + 1);
        timeValues.push_back(0.0);
        for (int i = 0; i <= NUM_SHAKES*2; i++) {
            timeValues.push_back(timeValues.back() + SHAKE_INC_TIME);
        }
        assert(timeValues.back() <= BossWeakpoint::INVULNERABLE_TIME_IN_SECS);

        std::vector<Vector3D> moveValues;
        moveValues.reserve(timeValues.size());
        moveValues.push_back(Vector3D(0.0f, 0.0f, 0.0f));
        for (int i = 0; i < NUM_SHAKES; i++) {
            float randomVal1 = Randomizer::GetInstance()->RandomNumNegOneToOne() * BOSS_WIDTH / 25.0f;
            float randomVal2 = Randomizer::GetInstance()->RandomNumNegOneToOne() * BOSS_WIDTH / 25.0f;
            moveValues.push_back(Vector3D(randomVal1, randomVal2, 0));
            moveValues.push_back(Vector3D(-randomVal1, -randomVal2, 0));
        }
        moveValues.push_back(Vector3D(0.0f, 0.0f, 0.0f));  

        this->columnHurtMoveAnim.SetLerp(timeValues, moveValues);
        this->columnHurtMoveAnim.SetRepeat(false);
    }

    this->SetState(ClassicalBossAI::BasicMoveAndLaserSprayAIState);
}

BodyHeadAI::~BodyHeadAI() {

}

void BodyHeadAI::CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart) {
    UNUSED_PARAMETER(gameModel);
    
    // Check to see if the collision was with any of the column weakpoints
    for (int i = 0; i < static_cast<int>(this->columnWeakpts.size()); i++) {
        BossWeakpoint* column = this->columnWeakpts[i];
        if (collisionPart == column) {
            
            // The column should now be destroyed...
            assert(column->GetIsDestroyed());
            
            this->boss->ConvertAliveBodyPartToDeadBodyPart(column);
            
            // Get the direction of the force that the ball exerted on the column on the x-axis...
            float ballXForceDir = (column->GetTranslationPt2D() - ball.GetCenterPosition2D())[0];

            // Animate the column to fade out and "fall off" the body
            column->AnimateColourRGBA(this->limbAlphaFadeoutAnim);
            AnimationMultiLerp<Vector3D> columnDeathTransAnim = this->GenerateColumnDeathTranslationAnimation(ballXForceDir);
            column->AnimateLocalTranslation(columnDeathTransAnim);
            AnimationMultiLerp<float> columnDeathRotAnim = this->GenerateColumnDeathRotationAnimation(ballXForceDir);
            column->AnimateLocalZRotation(columnDeathRotAnim);

            if (this->currState != ClassicalBossAI::LostColumnAIState) {
                this->SetState(ClassicalBossAI::LostColumnAIState);
            }

            break;
        }
    }
}

void BodyHeadAI::CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart) {
    UNUSED_PARAMETER(gameModel);
    UNUSED_PARAMETER(projectile);
    UNUSED_PARAMETER(collisionPart);
}

void BodyHeadAI::CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle, BossBodyPart* collisionPart) {
    UNUSED_PARAMETER(gameModel);
    UNUSED_PARAMETER(paddle);
    UNUSED_PARAMETER(collisionPart);
}

void BodyHeadAI::SetState(ClassicalBossAI::AIState newState) {
    switch (newState) {

        case ClassicalBossAI::BasicMoveAndLaserSprayAIState:
            debug_output("Entering BasicMoveAndLaserSprayAIState");
            this->laserSprayCountdown  = BodyHeadAI::LASER_SPRAY_RESET_TIME_IN_SECS;
            this->countdownToNextState = this->GenerateBasicMoveTime();
            break;

        case ClassicalBossAI::PrepLaserAIState:
            this->numConsecutiveTimesBasicMoveExecuted = 0;
            this->numConsecutiveBarrages = 0;
            this->countdownToLaserBarrage = this->GetLaserChargeTime();
            this->movingDir = Randomizer::GetInstance()->RandomNegativeOrPositive();
            this->laserShootTimer = this->GetTimeBetweenLaserBarrageShots();
            break;

        case ClassicalBossAI::MoveAndBarrageWithLaserAIState:
            this->numConsecutiveTimesBasicMoveExecuted = 0;
            this->movingDir = -this->movingDir;
            break;

        case ClassicalBossAI::LostColumnAIState:
            this->columnHurtMoveAnim.ResetToStart();
            break;

        case ClassicalBossAI::LostAllColumnsAIState:
            break;
        case ClassicalBossAI::LostBodyAngryAIState:
            break;
        
        default:
            assert(false);
            return;
    }

    this->currState = newState;
}

void BodyHeadAI::UpdateState(double dT, GameModel* gameModel) {
    switch (this->currState) {

        case ClassicalBossAI::BasicMoveAndLaserSprayAIState:
            this->ExecuteBasicMoveAndLaserSprayState(dT, gameModel);
            break;

        case ClassicalBossAI::PrepLaserAIState:
            this->ExecutePrepLaserState(dT, gameModel);
            break;

        case ClassicalBossAI::MoveAndBarrageWithLaserAIState:
            this->ExecuteMoveAndBarrageWithLaserState(dT, gameModel);
            break;

        case ClassicalBossAI::LostColumnAIState:
            this->ExecuteLostColumnState(dT);
            break;
        case ClassicalBossAI::LostAllColumnsAIState:
            this->ExecuteLostAllColumnsState(dT);
            break;
        case ClassicalBossAI::LostBodyAngryAIState:
            break;
        
        default:
            assert(false);
            return;
    }
}

void BodyHeadAI::ExecuteBasicMoveAndLaserSprayState(double dT, GameModel* gameModel) {
    assert(gameModel != NULL);

    const GameLevel* level = gameModel->GetCurrentLevel();
    assert(level != NULL);

    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();

    // The basic movement state occurs further away from the paddle, closer to the top of the level.
    // Make sure we've moved up high enough...
    float avgBasicMoveHeight = this->GetBasicMovementHeight(level);
    float upDownDistance     = ClassicalBossAI::BOSS_HEIGHT / 5.0f;

    if (bossPos[1] < avgBasicMoveHeight - upDownDistance) {
        this->desiredVel[1] = this->GetMaxSpeed() / 1.75f;
    }
    else if (bossPos[1] > avgBasicMoveHeight + upDownDistance) {
        this->desiredVel[1] = -this->GetMaxSpeed() / 1.75f;
    }
    
    if (fabs(this->desiredVel[1]) < EPSILON) {
        this->desiredVel[1] = this->GetMaxSpeed() / 1.75f;
    }
        
    // Side-to-side movement is basic back and forth linear translation...
    if (bossPos[0] <= this->GetBossMovementMinXBound(level, BOSS_WIDTH)) {
        this->desiredVel[0] = this->GetMaxSpeed() / 1.25f;
    }
    else if (bossPos[0] >= this->GetBossMovementMaxXBound(level, BOSS_WIDTH)) {
        this->desiredVel[0] = -this->GetMaxSpeed() / 1.25f;
    }
    
    if (fabs(this->desiredVel[0]) < EPSILON) {
        this->desiredVel[0] = Randomizer::GetInstance()->RandomNegativeOrPositive() * this->GetMaxSpeed();
    }

    if (this->countdownToNextState <= 0.0) {
        this->currVel    = Vector2D(0,0);
        this->desiredVel = Vector2D(0,0);
        
        if (this->numConsecutiveTimesBasicMoveExecuted > 3) {
            this->SetState(ClassicalBossAI::PrepLaserAIState);
            return;
        }
        else {
            if (Randomizer::GetInstance()->RandomUnsignedInt() % 3 == 0) {
                this->SetState(ClassicalBossAI::PrepLaserAIState);
            }
            else {
                this->SetState(ClassicalBossAI::BasicMoveAndLaserSprayAIState);
                this->numConsecutiveTimesBasicMoveExecuted++;
            }
        }
    }
    else {
        this->countdownToNextState -= dT;
    }

    // Fire ze lasers (if the countdown is done)
    if (this->laserSprayCountdown <= 0.0) {
        this->ExecuteLaserSpray(gameModel);
        this->laserSprayCountdown = BodyHeadAI::LASER_SPRAY_RESET_TIME_IN_SECS;
    }
    else {
        this->laserSprayCountdown -= dT;
    }
}

void BodyHeadAI::ExecutePrepLaserState(double dT, GameModel* gameModel) {
    assert(gameModel != NULL);

    const GameLevel* level = gameModel->GetCurrentLevel();
    assert(level != NULL);

    // Make sure the boss is at the proper height in the level for firing the laser
    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();

    bool allowedToAttack = true;

    // Make sure the boss is at the correct height...
    const float laserPrepHeight = this->GetPrepLaserHeight(level);
    if (bossPos[1] > laserPrepHeight + LevelPiece::PIECE_HEIGHT) {
        this->desiredVel[1] = -this->GetMaxSpeed() / 1.5f;
        allowedToAttack = false;
    }
    else if (bossPos[1] < laserPrepHeight - LevelPiece::PIECE_HEIGHT) {
        this->desiredVel[1] = this->GetMaxSpeed() / 1.5f;
        allowedToAttack = false;
    }

    // Make sure the boss is at the correct side of the level
    if (bossPos[0] > this->GetBossMovementMinXBound(level, BOSS_WIDTH) && 
        bossPos[0] < this->GetBossMovementMaxXBound(level, BOSS_WIDTH)) {

        this->desiredVel[0] = this->movingDir * this->GetMaxSpeed();
        allowedToAttack = false;
    }

    if (allowedToAttack) {
        this->currVel    = Vector2D(0.0f, 0.0f);
        this->desiredVel = Vector2D(0.0f, 0.0f);

        // Charge up the laser barrage...
        if (this->countdownToLaserBarrage <= 0.0) {
            this->SetState(ClassicalBossAI::MoveAndBarrageWithLaserAIState);
        }
        else {
            if (this->countdownToLaserBarrage == this->GetLaserChargeTime()) {
                // EVENT: Charging for laser barrage...
                // TODO
            }
            this->countdownToLaserBarrage -= dT;
        }
    }
}

void BodyHeadAI::ExecuteMoveAndBarrageWithLaserState(double dT, GameModel* gameModel) {
    assert(gameModel != NULL);

    const GameLevel* level = gameModel->GetCurrentLevel();
    assert(level != NULL);

    this->desiredVel[1] = 0.0f;
    this->currVel[1] = 0.0f;

    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();

    // Start moving across the entire screen firing lasers
    bool finishedMoving = movingDir < 0 && bossPos[0] <= this->GetBossMovementMinXBound(level, BOSS_WIDTH) ||
                          movingDir > 0 && bossPos[0] >= this->GetBossMovementMaxXBound(level, BOSS_WIDTH);

    if (finishedMoving) {
        this->desiredVel[0] = 0.0f;
        this->currVel[0] = 0.0f;

        // We're finished moving, we might repeat this state a few times... or just go back to the basic move state
        if (this->numConsecutiveBarrages > 3) {
            this->SetState(ClassicalBossAI::BasicMoveAndLaserSprayAIState);
        }
        else {
            if (Randomizer::GetInstance()->RandomUnsignedInt() % 3 == 0) {
                this->SetState(ClassicalBossAI::BasicMoveAndLaserSprayAIState);
            }
            else {
                this->SetState(ClassicalBossAI::MoveAndBarrageWithLaserAIState);
            }
        }
        
    }
    else {
        this->desiredVel[0] = this->movingDir * this->GetMaxSpeed() / 4.0f;
    }

    // Determine whether we're shooting a laser
    if (this->laserShootTimer <= 0.0) {
        static const float angleIncrement = 15;
        static float LASER_ANGLES[] = { -2*angleIncrement, -angleIncrement, 0, angleIncrement, 2*angleIncrement };
  
        // Fire a laser downwards within a small cone
        Vector2D laserDir(0, -1);
        laserDir.Rotate(LASER_ANGLES[Randomizer::GetInstance()->RandomUnsignedInt() % 5]);

        Point2D eyePos = this->boss->GetEye()->GetTranslationPt2D();
        gameModel->AddProjectile(new BossLaserProjectile(eyePos, laserDir));

        this->laserShootTimer = this->GetTimeBetweenLaserBarrageShots();
    }
    else {
        this->laserShootTimer -= dT;
    }
}

void BodyHeadAI::ExecuteLostColumnState(double dT) {
    // No velocity in this state
    this->currVel    = Vector2D(0,0);
    this->desiredVel = Vector2D(0,0);

    bool isFinished = this->columnHurtMoveAnim.Tick(dT);
    this->boss->alivePartsRoot->SetLocalTranslation(this->columnHurtMoveAnim.GetInterpolantValue());

    if (isFinished) {
        this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0,0,0));
        this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0,0,0));

        // Check to see if both arms are now gone, if they are then we move to a special state
        bool allColumnsDestroyed = true;
        for (int i = 0; i < static_cast<int>(this->columnWeakpts.size()); i++) {
            BossWeakpoint* column = this->columnWeakpts[i];
            if (!column->GetIsDestroyed()) {
                allColumnsDestroyed = false;
                break;
            }
        }

        if (allColumnsDestroyed) {
            this->SetState(ClassicalBossAI::LostAllColumnsAIState);
        }
        else {
            // Go into basic move and laser spray state
            this->SetState(ClassicalBossAI::BasicMoveAndLaserSprayAIState);
        }
    }

}

void BodyHeadAI::ExecuteLostAllColumnsState(double dT) {

}

AnimationMultiLerp<Vector3D> BodyHeadAI::GenerateColumnDeathTranslationAnimation(float xForceDir) const {

    double lastTimeValue = this->limbAlphaFadeoutAnim.GetTimeValues().back();
    
    std::vector<double> timeValues;
    timeValues.reserve(2);
    timeValues.push_back(0.0);
    timeValues.push_back(lastTimeValue);

    std::vector<Vector3D> moveValues;
    moveValues.reserve(timeValues.size());
    moveValues.push_back(Vector3D(0,0,0));
    moveValues.push_back(Vector3D(NumberFuncs::SignOf(xForceDir) * 8 * ClassicalBoss::COLUMN_WIDTH, 
        -4 * ClassicalBoss::COLUMN_HEIGHT - this->boss->alivePartsRoot->GetTranslationPt2D()[1], 0.0f));

    AnimationMultiLerp<Vector3D> columnDeathTransAnim;
    columnDeathTransAnim.SetLerp(timeValues, moveValues);
    columnDeathTransAnim.SetRepeat(false);
    columnDeathTransAnim.SetInterpolantValue(Vector3D(0,0,0));

    return columnDeathTransAnim;
}

AnimationMultiLerp<float> BodyHeadAI::GenerateColumnDeathRotationAnimation(float xForceDir) const {
    
    double lastTimeValue = this->limbAlphaFadeoutAnim.GetTimeValues().back();
    
    std::vector<double> timeValues;
    timeValues.reserve(2);
    timeValues.push_back(0.0);
    timeValues.push_back(lastTimeValue);

    std::vector<float> rotationValues;
    rotationValues.reserve(timeValues.size());
    rotationValues.push_back(0.0f);
    rotationValues.push_back(xForceDir < 0 ? 1800.0f : -1800.0f);

    AnimationMultiLerp<float> columnDeathRotAnim;
    columnDeathRotAnim.SetLerp(timeValues, rotationValues);
    columnDeathRotAnim.SetRepeat(false);
    columnDeathRotAnim.SetInterpolantValue(0.0f);

    return columnDeathRotAnim;
}

// END BodyHeadAI ******************************************************************