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
    currLaserDir.Rotate(ANGLE_BETWEEN_LASERS_IN_DEGS);
    gameModel->AddProjectile(new BossLaserProjectile(eyePos, currLaserDir));

    // Left fan-out of lasers
    currLaserDir = initLaserDir;
    currLaserDir.Rotate(-ANGLE_BETWEEN_LASERS_IN_DEGS);
    gameModel->AddProjectile(new BossLaserProjectile(eyePos, currLaserDir));
    currLaserDir.Rotate(-ANGLE_BETWEEN_LASERS_IN_DEGS);
    gameModel->AddProjectile(new BossLaserProjectile(eyePos, currLaserDir));
    currLaserDir.Rotate(-ANGLE_BETWEEN_LASERS_IN_DEGS);
    gameModel->AddProjectile(new BossLaserProjectile(eyePos, currLaserDir));

}

float ClassicalBossAI::GetBossMovementMinXBound(const GameLevel* level, float bossWidth) const {
    UNUSED_PARAMETER(level);
    return bossWidth / 1.4f + LevelPiece::PIECE_WIDTH;
}

float ClassicalBossAI::GetBossMovementMaxXBound(const GameLevel* level, float bossWidth) const {
    return level->GetLevelUnitWidth() - this->GetBossMovementMinXBound(level, bossWidth);
}

// END ClassicalBossAI *************************************************************

// BEGIN ArmsBodyHeadAI ************************************************************

const float ArmsBodyHeadAI::BOSS_HEIGHT = 13.0f;
const float ArmsBodyHeadAI::BOSS_WIDTH = 25.0f;
const float ArmsBodyHeadAI::HALF_BOSS_WIDTH = ArmsBodyHeadAI::BOSS_WIDTH / 2.0f;

const float ArmsBodyHeadAI::ARM_HEIGHT = 9.66f;
const float ArmsBodyHeadAI::ARM_WIDTH  = 3.097f;

const float ArmsBodyHeadAI::ARM_LIFE_POINTS = 300.0f;
const float ArmsBodyHeadAI::ARM_BALL_DAMAGE = 100.0f;

const double ArmsBodyHeadAI::LASER_SPRAY_RESET_TIME_IN_SECS = 1.4;

const double ArmsBodyHeadAI::ARM_ATTACK_DELTA_T = 0.4;

ArmsBodyHeadAI::ArmsBodyHeadAI(ClassicalBoss* boss) : ClassicalBossAI(boss), currState(ArmsBodyHeadAI::BasicMoveAndLaserSprayAIState),
leftArm(NULL), rightArm(NULL), leftArmSqrWeakpt(NULL), rightArmSqrWeakpt(NULL), currVel(0.0f, 0.0f), desiredVel(0.0f, 0.0f),
countdownToNextState(0.0), countdownToAttack(0.0), laserSprayCountdown(0.0), isAttackingWithArm(false),
nextAttackState(ArmsBodyHeadAI::AttackBothArmsAIState), temptAttackCountdown(0.0), countdownToLaserBarrage(0.0) {
    
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

    // Setup the arm fade-out animation (for when an arm falls off and dies)
    {
        static const double FIRST_FADE_OUT_TIME = 5.0;
        std::vector<double> timeValues;
        timeValues.reserve(13);
        timeValues.push_back(0.0);
        timeValues.push_back(FIRST_FADE_OUT_TIME);
        timeValues.push_back(FIRST_FADE_OUT_TIME + 0.1);
        timeValues.push_back(FIRST_FADE_OUT_TIME + 0.2);
        timeValues.push_back(FIRST_FADE_OUT_TIME + 0.3);
        timeValues.push_back(FIRST_FADE_OUT_TIME + 0.4);
        timeValues.push_back(FIRST_FADE_OUT_TIME + 0.5);
        timeValues.push_back(FIRST_FADE_OUT_TIME + 0.6);
        timeValues.push_back(FIRST_FADE_OUT_TIME + 0.7);
        timeValues.push_back(FIRST_FADE_OUT_TIME + 0.8);
        timeValues.push_back(FIRST_FADE_OUT_TIME + 0.9);
        timeValues.push_back(FIRST_FADE_OUT_TIME + 1.0);
        timeValues.push_back(FIRST_FADE_OUT_TIME + 1.1);

        static const float FIRST_ALPHA_FADE_OUT_VALUE  = 0.15f;
        static const float SECOND_ALPHA_FADE_OUT_VALUE = FIRST_ALPHA_FADE_OUT_VALUE - 0.1f;
        std::vector<float> alphaValues;
        alphaValues.reserve(timeValues.size());
        alphaValues.push_back(1.0f);
        alphaValues.push_back(FIRST_ALPHA_FADE_OUT_VALUE);
        alphaValues.push_back(SECOND_ALPHA_FADE_OUT_VALUE);
        alphaValues.push_back(FIRST_ALPHA_FADE_OUT_VALUE);
        alphaValues.push_back(SECOND_ALPHA_FADE_OUT_VALUE);
        alphaValues.push_back(FIRST_ALPHA_FADE_OUT_VALUE);
        alphaValues.push_back(SECOND_ALPHA_FADE_OUT_VALUE);
        alphaValues.push_back(FIRST_ALPHA_FADE_OUT_VALUE);
        alphaValues.push_back(SECOND_ALPHA_FADE_OUT_VALUE);
        alphaValues.push_back(FIRST_ALPHA_FADE_OUT_VALUE);
        alphaValues.push_back(SECOND_ALPHA_FADE_OUT_VALUE);
        alphaValues.push_back(FIRST_ALPHA_FADE_OUT_VALUE);
        alphaValues.push_back(0.0f);

        this->armAlphaFadeoutAnim.SetLerp(timeValues, alphaValues);
        this->armAlphaFadeoutAnim.SetRepeat(false);
    }

    {
        static const int NUM_FLASHES = 10;
        
        std::vector<double> timeValues;
        timeValues.reserve(2*NUM_FLASHES + 1);
        std::vector<float> alphaValues;
        alphaValues.reserve(timeValues.size());
        
        double timeInc = BossWeakpoint::INVULNERABLE_TIME_IN_SECS / static_cast<double>(2*NUM_FLASHES);
        double timeCount = 0.0;
        
        for (int i = 0; i <= 2*NUM_FLASHES; i++) {
            timeValues.push_back(timeCount);
            timeCount += timeInc;
        }
        for (int i = 0; i < NUM_FLASHES; i++) {
            alphaValues.push_back(1.0f);
            alphaValues.push_back(0.25f);
        }
        alphaValues.push_back(1.0f);

        this->hurtAlphaAnim.SetLerp(timeValues, alphaValues);
        this->hurtAlphaAnim.SetRepeat(false);

        timeValues.clear();
        timeValues.reserve(4);
        timeValues.push_back(0.0);
        timeValues.push_back(0.15);
        timeValues.push_back(0.2);
        timeValues.push_back(0.22);
        
        std::vector<Vector3D> moveValues;
        moveValues.reserve(timeValues.size());
        moveValues.push_back(Vector3D(0,0,0));
        moveValues.push_back(Vector3D(BOSS_WIDTH/3.0f, LevelPiece::PIECE_HEIGHT, 0.0f));
        moveValues.push_back(Vector3D(BOSS_WIDTH/4.0f, 0.0f, 0.0f));
        moveValues.push_back(Vector3D(0.0f, 0.0f, 0.0f));

        this->leftArmHurtMoveAnim.SetLerp(timeValues, moveValues);
        this->leftArmHurtMoveAnim.SetRepeat(false);

        moveValues.clear();
        moveValues.reserve(timeValues.size());
        moveValues.push_back(Vector3D(0,0,0));
        moveValues.push_back(Vector3D(-BOSS_WIDTH/3.0f, LevelPiece::PIECE_HEIGHT, 0.0f));
        moveValues.push_back(Vector3D(-BOSS_WIDTH/4.0f, 0.0f, 0.0f));
        moveValues.push_back(Vector3D(0.0f, 0.0f, 0.0f));

        this->rightArmHurtMoveAnim.SetLerp(timeValues, moveValues);
        this->rightArmHurtMoveAnim.SetRepeat(false);
    }

    this->SetState(ArmsBodyHeadAI::PrepLaserAIState);//BasicMoveAndLaserSprayAIState);
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
    UNUSED_PARAMETER(ball);
    UNUSED_PARAMETER(gameModel);

    assert(collisionPart != NULL);

    // Check to see if the ball hit one of the weakspots...
    if (collisionPart == this->rightArmSqrWeakpt) {

        // Check to see if the arm is dead, if it is then we change its state in the body of the boss
        if (this->rightArmSqrWeakpt->GetIsDestroyed()) {
            this->boss->ConvertAliveBodyPartToDeadBodyPart(boss->rightArmIdx);
            
            // Animate the right arm to fade out and "fall off" the body
            this->rightArm->AnimateAlpha(this->armAlphaFadeoutAnim);

            AnimationMultiLerp<Vector3D> armDeathTransAnim = this->GenerateArmDeathTranslationAnimation(false);
            this->rightArm->AnimateLocalTranslation(armDeathTransAnim);

            AnimationMultiLerp<float> armDeathRotAnim = this->GenerateArmDeathRotationAnimation(false);
            this->rightArm->AnimateLocalZRotation(armDeathRotAnim);

            this->rightArmSqrWeakpt = NULL;
        }
        if (this->currState != ArmsBodyHeadAI::HurtRightArmAIState) {
            this->SetState(ArmsBodyHeadAI::HurtRightArmAIState);
        }
    }
    else if (collisionPart == this->leftArmSqrWeakpt) {

        // Check to see if the arm is dead, if it is then we change its state in the body of the boss
        if (this->leftArmSqrWeakpt->GetIsDestroyed()) {
            this->boss->ConvertAliveBodyPartToDeadBodyPart(boss->leftArmIdx);
            
            // Animate the left arm to fade out and "fall off" the body
            this->leftArm->AnimateAlpha(this->armAlphaFadeoutAnim);

            AnimationMultiLerp<Vector3D> armDeathTransAnim = this->GenerateArmDeathTranslationAnimation(true);
            this->leftArm->AnimateLocalTranslation(armDeathTransAnim);

            AnimationMultiLerp<float> armDeathRotAnim = this->GenerateArmDeathRotationAnimation(true);
            this->leftArm->AnimateLocalZRotation(armDeathRotAnim);

            this->leftArmSqrWeakpt = NULL;
        }

        if (this->currState != ArmsBodyHeadAI::HurtLeftArmAIState) {
            this->SetState(ArmsBodyHeadAI::HurtLeftArmAIState);
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

void ArmsBodyHeadAI::SetState(AIState newState) {

    switch (newState) {

        case ArmsBodyHeadAI::BasicMoveAndLaserSprayAIState:
            this->laserSprayCountdown  = ArmsBodyHeadAI::LASER_SPRAY_RESET_TIME_IN_SECS;
            this->countdownToNextState = this->GenerateBasicMoveTime();
            break;

        case ArmsBodyHeadAI::ChasePaddleAIState:
            this->armShakeAnim.ResetToStart();
            this->armShakeAnim.SetRepeat(true);
            this->temptAttackCountdown = this->GenerateTemptAttackTime();
            this->countdownToAttack    = this->GenerateChaseTime();
            break;

        case ArmsBodyHeadAI::AttackLeftArmAIState:
        case ArmsBodyHeadAI::AttackRightArmAIState:
        case ArmsBodyHeadAI::AttackBothArmsAIState:
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->armShakeAnim.ResetToStart();
            this->armShakeAnim.SetRepeat(false);
            this->armAttackYMovementAnim.ResetToStart();
            this->armAttackYMovementAnim.SetRepeat(false);
            break;

        case ArmsBodyHeadAI::PrepLaserAIState:
            this->countdownToLaserBarrage = this->GetLaserChargeTime();
            this->movingDir = Randomizer::GetInstance()->RandomNegativeOrPositive();
            break;
        case ArmsBodyHeadAI::MoveAndBarrageWithLaserAIState:
            this->laserShootTimer = 0.0;
            this->movingDir = -this->movingDir;
            break;

        case ArmsBodyHeadAI::HurtLeftArmAIState:
            this->hurtAlphaAnim.ResetToStart();
            this->boss->alivePartsRoot->AnimateAlpha(this->hurtAlphaAnim);
            this->leftArmHurtMoveAnim.ResetToStart();
            break;
        case ArmsBodyHeadAI::HurtRightArmAIState:
            this->hurtAlphaAnim.ResetToStart();
            this->boss->alivePartsRoot->AnimateAlpha(this->hurtAlphaAnim);
            this->rightArmHurtMoveAnim.ResetToStart();
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

        case ArmsBodyHeadAI::BasicMoveAndLaserSprayAIState:
            this->ExecuteBasicMoveAndLaserSprayState(dT, gameModel);
            break;

        case ArmsBodyHeadAI::ChasePaddleAIState:
            this->ExecuteChasePaddleState(dT, gameModel);
            break;

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
            this->ExecutePrepLaserState(dT, gameModel);
            break;
        case ArmsBodyHeadAI::MoveAndBarrageWithLaserAIState:
            this->ExecuteMoveAndBarrageWithLaserState(dT, gameModel);
            break;

        case ArmsBodyHeadAI::HurtLeftArmAIState:
            this->ExecuteHurtArmState(dT, true);
            break;
        case ArmsBodyHeadAI::HurtRightArmAIState:
            this->ExecuteHurtArmState(dT, false);
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
        Matrix4x4 movementMatrix = Matrix4x4::translationMatrix(Vector3D(dMovement));

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

void ArmsBodyHeadAI::ExecuteBasicMoveAndLaserSprayState(double dT, GameModel* gameModel) {
    assert(gameModel != NULL);

    const GameLevel* level = gameModel->GetCurrentLevel();
    assert(level != NULL);

    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();

    // The basic movement state occurs further away from the paddle, closer to the top of the level.
    // Make sure we've moved up high enough...
    float avgBasicMoveHeight = this->GetBasicMovementHeight(level);
    float upDownDistance     = ArmsBodyHeadAI::BOSS_HEIGHT / 6.0f;

    if (bossPos[1] < avgBasicMoveHeight - upDownDistance) {
        this->desiredVel[1] = this->GetMaxSpeed() / 2.0f;
    }
    else if (bossPos[1] > avgBasicMoveHeight + upDownDistance) {
        this->desiredVel[1] = -this->GetMaxSpeed() / 2.0f;
    }
    
    if (fabs(this->desiredVel[1]) < EPSILON) {
        this->desiredVel[1] = this->GetMaxSpeed() / 2.0f;
    }
        
    // Side-to-side movement is basic back and forth linear translation...
    if (bossPos[0] <= this->GetBossMovementMinXBound(level, BOSS_WIDTH)) {
        this->desiredVel[0] = this->GetMaxSpeed() / 1.33f;
    }
    else if (bossPos[0] >= this->GetBossMovementMaxXBound(level, BOSS_WIDTH)) {
        this->desiredVel[0] = -this->GetMaxSpeed() / 1.33f;
    }
    
    if (fabs(this->desiredVel[0]) < EPSILON) {
        this->desiredVel[0] = Randomizer::GetInstance()->RandomNegativeOrPositive() * this->GetMaxSpeed() / 1.33f;
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
            this->currVel    = Vector2D(0,0);
            this->desiredVel = Vector2D(0,0);
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

void ArmsBodyHeadAI::ExecuteChasePaddleState(double dT, GameModel* gameModel) {
    assert(gameModel != NULL);

    const PlayerPaddle* paddle = gameModel->GetPlayerPaddle();
    assert(paddle != NULL);
    const GameLevel* level = gameModel->GetCurrentLevel();
    assert(level != NULL);

    const Point2D& paddlePos = paddle->GetCenterPosition();
    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();

    bool allowedToAttack = false;

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
    Vector2D bossToPaddleVec   = paddlePos - bossPos;
    
    // Check to see if we can attack yet (If we're anywhere near the paddle then perform the attack)
    
    Collision::AABB2D paddleAABB   = paddle->GetBounds().GenerateAABBFromLines();
    Collision::AABB2D leftArmAABB  = this->leftArmSqrWeakpt->GetWorldBounds().GenerateAABBFromLines();
    Collision::AABB2D rightArmAABB = this->rightArmSqrWeakpt->GetWorldBounds().GenerateAABBFromLines();

    if ((paddleAABB.GetMin()[0] >= leftArmAABB.GetMin()[0] && paddleAABB.GetMin()[0] <= leftArmAABB.GetMax()[0]) || 
        (paddleAABB.GetMax()[0] <= leftArmAABB.GetMax()[0] && paddleAABB.GetMax()[0] >= leftArmAABB.GetMin()[0]) ||
        (paddleAABB.GetMin()[0] >= rightArmAABB.GetMin()[0] && paddleAABB.GetMin()[0] <= rightArmAABB.GetMax()[0]) || 
        (paddleAABB.GetMax()[0] <= rightArmAABB.GetMax()[0] && paddleAABB.GetMax()[0] >= rightArmAABB.GetMin()[0])) {

        this->desiredVel[0] = 0.0f;
        this->currVel[0]    = 0.0f;

        if (allowedToAttack) {

            // Use a timer here to make the player 'tempt' the boss to attack with its arm(s)
            if (this->temptAttackCountdown <= 0.0) {
                this->leftArm->SetLocalTranslation(Vector3D(0.0f, 0.0f, 0.0f));
                this->rightArm->SetLocalTranslation(Vector3D(0.0f, 0.0f, 0.0f));
                this->SetState(this->nextAttackState);
                return;
            }
            else {
                // Shake the appropriate arm and set it up as the next attack state...
                this->nextAttackState = this->DetermineNextArmAttackState(bossToPaddleVec);

                float shakeAmt = this->armShakeAnim.GetInterpolantValue();
                this->armShakeAnim.Tick(dT);

                if (this->nextAttackState == AttackLeftArmAIState || this->nextAttackState == AttackBothArmsAIState) {
                    this->leftArm->SetLocalTranslation(Vector3D(shakeAmt, 0.0f, 0.0f));
                }
                if (this->nextAttackState == AttackRightArmAIState || this->nextAttackState == AttackBothArmsAIState) {
                    this->rightArm->SetLocalTranslation(Vector3D(shakeAmt, 0.0f, 0.0f));
                }
            }
            this->temptAttackCountdown -= dT;
        }
    }
    else {
        this->leftArm->SetLocalTranslation(Vector3D(0.0f, 0.0f, 0.0f));
        this->rightArm->SetLocalTranslation(Vector3D(0.0f, 0.0f, 0.0f));

        float absBossToPaddleXDist = fabs(bossToPaddleVec[0]);
        if (absBossToPaddleXDist > ClassicalBoss::ARM_X_TRANSLATION_FROM_CENTER + ClassicalBoss::HALF_ARM_WIDTH) {
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
    }

    // Make sure the boss isn't to far to either side...
    if (bossPos[0] < this->GetBossMovementMinXBound(level, BOSS_WIDTH)) {
        this->desiredVel[0] = 0.0f;
        this->currVel[0] = 0.0f;
        this->boss->alivePartsRoot->Translate(Vector3D(this->GetBossMovementMinXBound(level, BOSS_WIDTH) - bossPos[0], 0.0f, 0.0f));
    }
    else if (bossPos[0] > this->GetBossMovementMaxXBound(level, BOSS_WIDTH)) {
        this->desiredVel[0] = 0.0f;
        this->currVel[0] = 0.0f;
        this->boss->alivePartsRoot->Translate(Vector3D(this->GetBossMovementMaxXBound(level, BOSS_WIDTH) - bossPos[0], 0.0f, 0.0f));
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
        if (isLeftArmAttacking) {
            this->leftArm->SetLocalTranslation(Vector3D(shakingMovement, 0.0f, 0.0f));
        }
        if (isRightArmAttacking) {
            this->rightArm->SetLocalTranslation(Vector3D(shakingMovement, 0.0f, 0.0f));
        }
    }
    else {
        bool attackIsDone = this->armAttackYMovementAnim.Tick(dT);

        // We are attacking with the arm!!!
        
        // Move the arm based on the attack animation
        float armYMovement = this->armAttackYMovementAnim.GetInterpolantValue();
        if (isLeftArmAttacking) {
            this->leftArm->SetLocalTranslation(Vector3D(0.0f, armYMovement, 0.0f));
        }
        if (isRightArmAttacking) {
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
            this->leftArm->SetLocalTranslation(Vector3D(0.0f, 0.0f, 0.0f));
            this->rightArm->SetLocalTranslation(Vector3D(0.0f, 0.0f, 0.0f));
            this->SetState(ArmsBodyHeadAI::BasicMoveAndLaserSprayAIState);
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
            this->SetState(ArmsBodyHeadAI::MoveAndBarrageWithLaserAIState);
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
        this->SetState(ArmsBodyHeadAI::ChasePaddleAIState);
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
    bool isFinished = false;
    if (isLeftArm) {
        isFinished = this->leftArmHurtMoveAnim.Tick(dT);
        this->leftArm->SetLocalTranslation(this->leftArmHurtMoveAnim.GetInterpolantValue());
    }
    else {
        isFinished = this->rightArmHurtMoveAnim.Tick(dT);
        this->rightArm->SetLocalTranslation(this->leftArmHurtMoveAnim.GetInterpolantValue());
    }

    if (isFinished) {
        this->leftArm->SetLocalTranslation(Vector3D(0,0,0));
        this->rightArm->SetLocalTranslation(Vector3D(0,0,0));

        // Check to see if both arms are now gone, if they are then we move to a special state
        if (this->leftArm->GetIsDestroyed() && this->rightArm->GetIsDestroyed()) {
            this->SetState(ArmsBodyHeadAI::LostArmsAngryAIState);
        }
        else {

            // ... otherwise we continue with the current AI routine by choosing a random state from it...
            if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
                this->SetState(ArmsBodyHeadAI::ChasePaddleAIState);
            }
            else {
                this->SetState(ArmsBodyHeadAI::BasicMoveAndLaserSprayAIState);
            }
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
    return level->GetLevelUnitHeight() - ArmsBodyHeadAI::BOSS_HEIGHT;
}

float ArmsBodyHeadAI::GetFollowAndAttackHeight() const {
    return 0.8f * this->GetMaxArmAttackYMovement() + ArmsBodyHeadAI::BOSS_HEIGHT / 2.0f;
}

float ArmsBodyHeadAI::GetMaxArmAttackYMovement() const {
    return 0.7f * ClassicalBoss::ARM_HEIGHT;
}

float ArmsBodyHeadAI::GetPrepLaserHeight(const GameLevel* level) const {
    return level->GetLevelUnitHeight() - ArmsBodyHeadAI::BOSS_HEIGHT;
}

ArmsBodyHeadAI::AIState ArmsBodyHeadAI::DetermineNextArmAttackState(const Vector2D& bossToPaddleVec) const {
    // How much damage has been done so far? These two percentages will maximally add to 2.0
    float percentLeftArm  = this->leftArmSqrWeakpt->GetCurrentLifePercentage();
    float percentRightArm = this->rightArmSqrWeakpt->GetCurrentLifePercentage();
    float totalLifePercent = (percentLeftArm + percentRightArm) / 2.0f;
    
    bool doBothArmAttack = false;
    if (totalLifePercent < 0.5) {
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

AnimationMultiLerp<Vector3D> ArmsBodyHeadAI::GenerateArmDeathTranslationAnimation(bool isLeftArm) const {
    double lastTimeValue = this->armAlphaFadeoutAnim.GetTimeValues().back();
    std::vector<double> timeValues;
    timeValues.reserve(2);
    timeValues.push_back(0.0);
    timeValues.push_back(lastTimeValue);

    std::vector<Vector3D> moveValues;
    moveValues.reserve(timeValues.size());
    moveValues.push_back(Vector3D(0,0,0));
    moveValues.push_back(Vector3D((isLeftArm ? -1 : 1) * 2 * ArmsBodyHeadAI::ARM_WIDTH, -ArmsBodyHeadAI::ARM_HEIGHT, 0.0f));

    AnimationMultiLerp<Vector3D> armDeathTransAnim;
    armDeathTransAnim.SetLerp(timeValues, moveValues);
    armDeathTransAnim.SetRepeat(false);
    armDeathTransAnim.SetInterpolantValue(Vector3D(0,0,0));

    return armDeathTransAnim;
}

AnimationMultiLerp<float> ArmsBodyHeadAI::GenerateArmDeathRotationAnimation(bool isLeftArm) const {
    double lastTimeValue = this->armAlphaFadeoutAnim.GetTimeValues().back();
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