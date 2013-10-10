/**
 * DecoBossAIStates.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#include "DecoBossAIStates.h"
#include "DecoBoss.h"
#include "GameModel.h"
#include "PlayerPaddle.h"
#include "BossLightningBoltProjectile.h"
#include "ElectrifiedEffectInfo.h"
#include "ShockwaveEffectInfo.h"
#include "SparkBurstEffectInfo.h"
#include "ShortCircuitEffectInfo.h"
#include "LevelShakeEffectInfo.h"
#include "ExpandingHaloEffectInfo.h"
#include "StarSmashEffectInfo.h"

#include "../GameSound/GameSound.h"

using namespace decobossai;

const float DecoBossAIState::DEFAULT_ACCELERATION = 0.225f * PlayerPaddle::DEFAULT_ACCELERATION;
const float DecoBossAIState::DEFAULT_MAX_X_SPEED = 7.0f;
const float DecoBossAIState::DEFAULT_MAX_Y_SPEED = 13.0f;

const float DecoBossAIState::DEFAULT_ARM_ROTATION_SPEED_DEGS_PER_SEC = 90.0f;

const float DecoBossAIState::SHOOT_AT_PADDLE_RANDOM_ROT_ANGLE_IN_DEGS = 13.25f;

const float DecoBossAIState::DEFAULT_LEVEL_ROTATION_SPEED_DEGS_PER_SEC = 13.0f;
const float DecoBossAIState::DEFAULT_LEVEL_ROTATION_AMT_IN_DEGS        = 360.0f;

const double DecoBossAIState::TOTAL_ELECTRIFIED_TIME_IN_SECS = 3.1;
const double DecoBossAIState::TOTAL_RETALIATION_TIME_IN_SECS = 2.0;
const double DecoBossAIState::TOTAL_ELECTRIFIED_AND_RETALIATION_TIME_IN_SECS = 
    TOTAL_ELECTRIFIED_TIME_IN_SECS + TOTAL_RETALIATION_TIME_IN_SECS;

DecoBossAIState::DecoBossAIState(DecoBoss* boss) : BossAIState(), boss(boss), shootCountdown(0.0),
numShotsUntilNextState(0), dropItemCountdown(0.0), nextDropItemType(GameItem::PaddleShrinkItem),
sideToSideNumDropCountdown(0), currLeftArmRotInDegs(0.0f), currRightArmRotInDegs(0.0f),
currLevelRotationAmtInDegs(0.0f), rotateShakeCountdown(0.0), rotationDir(1), rotateLevelSoundID(INVALID_SOUND_ID) {

    assert(boss != NULL);
    
    // Setup animations...

    // For when the boss is angry
    this->angryMoveAnim = Boss::BuildBossAngryShakeAnim(1.0f);
    
    // For when items are loading out of the drop hole of the boss
    this->itemLoadingAnim.SetLerp(0.0, 0.8, 0.0, DecoBoss::GetItemDropAnimDisplacement());
    this->itemLoadingAnim.SetRepeat(false);
    this->itemLoadingAnim.SetInterpolantValue(0.0);

    // TODO: Make these animations more visceral!
    // For when the boss extends its arms
    {
        std::vector<float> armTranslationValues;
        armTranslationValues.reserve(2);
        armTranslationValues.push_back(0.0);
        armTranslationValues.push_back(-DecoBoss::ARM_SCOPING_SEG1_Y_RETRACT_TRANSLATION);
        
        static const double TIME_PER_SEGMENT = 0.2;
        std::vector<double> timeValues;
        timeValues.reserve(armTranslationValues.size());
        timeValues.push_back(0.0);
        timeValues.push_back(TIME_PER_SEGMENT);

        this->armSeg1RotateExtendAnim.SetLerp(timeValues, armTranslationValues);
        this->armSeg1RotateExtendAnim.SetRepeat(false);

        timeValues[0] = timeValues.back();
        timeValues[1] = timeValues[0] + TIME_PER_SEGMENT;
        armTranslationValues[0] = 0.0;
        armTranslationValues[1] = -DecoBoss::ARM_SCOPING_SEG2_Y_RETRACT_TRANSLATION;
        
        this->armSeg2RotateExtendAnim.SetLerp(timeValues, armTranslationValues);
        this->armSeg2RotateExtendAnim.SetRepeat(false);

        timeValues[0] = timeValues.back();
        timeValues[1] = timeValues[0] + TIME_PER_SEGMENT;
        armTranslationValues[0] = 0.0;
        armTranslationValues[1] = -DecoBoss::ARM_SCOPING_SEG3_Y_RETRACT_TRANSLATION;

        this->armSeg3RotateExtendAnim.SetLerp(timeValues, armTranslationValues);
        this->armSeg3RotateExtendAnim.SetRepeat(false);

        timeValues[0] = timeValues.back();
        timeValues[1] = timeValues[0] + TIME_PER_SEGMENT;
        armTranslationValues[0] = 0.0;
        armTranslationValues[1] = -DecoBoss::ARM_SCOPING_SEG4_Y_RETRACT_TRANSLATION;

        this->armSeg4RotateExtendAnim.SetLerp(timeValues, armTranslationValues);
        this->armSeg4RotateExtendAnim.SetRepeat(false);
    }
    {
        std::vector<float> armTranslationValues;
        armTranslationValues.reserve(2);
        armTranslationValues.push_back(-DecoBoss::ARM_SCOPING_SEG4_Y_RETRACT_TRANSLATION);
        armTranslationValues.push_back(0.0);

        static const double TIME_PER_SEGMENT = 0.3;
        std::vector<double> timeValues;
        timeValues.reserve(armTranslationValues.size());
        timeValues.push_back(0.0);
        timeValues.push_back(TIME_PER_SEGMENT);

        this->armSeg4RetractAnim.SetLerp(timeValues, armTranslationValues);
        this->armSeg4RetractAnim.SetRepeat(false);

        timeValues[0] = timeValues.back();
        timeValues[1] = timeValues[0] + TIME_PER_SEGMENT;
        armTranslationValues[0] = -DecoBoss::ARM_SCOPING_SEG3_Y_RETRACT_TRANSLATION;
        armTranslationValues[1] = 0.0;

        this->armSeg3RetractAnim.SetLerp(timeValues, armTranslationValues);
        this->armSeg3RetractAnim.SetRepeat(false);

        timeValues[0] = timeValues.back();
        timeValues[1] = timeValues[0] + TIME_PER_SEGMENT;
        armTranslationValues[0] = -DecoBoss::ARM_SCOPING_SEG2_Y_RETRACT_TRANSLATION;
        armTranslationValues[1] = 0.0;

        this->armSeg2RetractAnim.SetLerp(timeValues, armTranslationValues);
        this->armSeg2RetractAnim.SetRepeat(false);

        timeValues[0] = timeValues.back();
        timeValues[1] = timeValues[0] + TIME_PER_SEGMENT;
        armTranslationValues[0] = -DecoBoss::ARM_SCOPING_SEG1_Y_RETRACT_TRANSLATION;
        armTranslationValues[1] = 0.0;

        this->armSeg1RetractAnim.SetLerp(timeValues, armTranslationValues);
        this->armSeg1RetractAnim.SetRepeat(false);
    }
    {
        std::vector<float> armTranslationValues;
        armTranslationValues.reserve(2);
        armTranslationValues.push_back(0.0);
        armTranslationValues.push_back(-DecoBoss::ARM_SCOPING_SEG1_Y_RETRACT_TRANSLATION);

        static const double TIME_PER_SEGMENT = 0.1;
        std::vector<double> timeValues;
        timeValues.reserve(armTranslationValues.size());
        timeValues.push_back(0.0);
        timeValues.push_back(TIME_PER_SEGMENT);

        this->armSeg1AttackExtendAnim.SetLerp(timeValues, armTranslationValues);
        this->armSeg1AttackExtendAnim.SetRepeat(false);

        timeValues[0] = timeValues.back();
        timeValues[1] = timeValues[0] + TIME_PER_SEGMENT;
        armTranslationValues[0] = 0.0;
        armTranslationValues[1] = -DecoBoss::ARM_SCOPING_SEG2_Y_RETRACT_TRANSLATION;

        this->armSeg2AttackExtendAnim.SetLerp(timeValues, armTranslationValues);
        this->armSeg2AttackExtendAnim.SetRepeat(false);

        timeValues[0] = timeValues.back();
        timeValues[1] = timeValues[0] + TIME_PER_SEGMENT;
        armTranslationValues[0] = 0.0;
        armTranslationValues[1] = -DecoBoss::ARM_SCOPING_SEG3_Y_RETRACT_TRANSLATION;

        this->armSeg3AttackExtendAnim.SetLerp(timeValues, armTranslationValues);
        this->armSeg3AttackExtendAnim.SetRepeat(false);

        timeValues[0] = timeValues.back();
        timeValues[1] = timeValues[0] + TIME_PER_SEGMENT;
        armTranslationValues[0] = 0.0;
        armTranslationValues[1] = -DecoBoss::ARM_SCOPING_SEG4_Y_RETRACT_TRANSLATION;

        this->armSeg4AttackExtendAnim.SetLerp(timeValues, armTranslationValues);
        this->armSeg4AttackExtendAnim.SetRepeat(false);
    }

    this->leftArmShakeAnim  = Boss::BuildLimbShakeAnim(DecoBoss::ARM_WIDTH/4.0f);
    this->rightArmShakeAnim = Boss::BuildLimbShakeAnim(DecoBoss::ARM_WIDTH/4.0f);
}

DecoBossAIState::~DecoBossAIState() {
}

Boss* DecoBossAIState::GetBoss() const {
    return this->boss;
}

Collision::AABB2D DecoBossAIState::GenerateDyingAABB() const {
    return this->boss->GetCore()->GenerateWorldAABB();
}

Point2D DecoBossAIState::GeneratePaddleArmAttackPosition(GameModel* gameModel) const {
    assert(gameModel != NULL);    

    PlayerPaddle* paddle = gameModel->GetPlayerPaddle();
    assert(paddle != NULL);

    const float halfBossWidth = this->boss->GetCurrentWidth() / 2.0f;
    float xPos = paddle->GetCenterPosition()[0];
    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();

    // This depends on whether both arms are currently alive or not...
    bool leftArmIsAlive  = !this->boss->GetLeftArm()->GetIsDestroyed();
    bool rightArmIsAlive = !this->boss->GetRightArm()->GetIsDestroyed();
    bool bothArmsAreAlive = leftArmIsAlive && rightArmIsAlive;
    if (bothArmsAreAlive) {
        // When both arms are still alive we just try to hover over the paddle and attack straight
        // downwards simultaneously with both arms 

        // Choose a reasonably-close-to-the-paddle-x-position, but make sure we keep 
        // it in the movement bounds of the boss
        xPos += Randomizer::GetInstance()->RandomNumNegOneToOne() * 2.0f * paddle->GetHalfWidthTotal();
    }
    else if (leftArmIsAlive) {
        // If only the left arm is alive we want to keep the paddle to the left side of the boss so that
        // the boss can attempt to extend its arm directly at the paddle
        float maxX = DecoBoss::GetMovementMaxXBound() - halfBossWidth;
        xPos += Randomizer::GetInstance()->RandomNumZeroToOne() * (maxX - xPos);
    }
    else if (rightArmIsAlive) {
        // If only the right arm is alive we want to keep the paddle to the right side of the boss so that
        // the boss can attempt to extend its arm directly at the paddle
        float minX = DecoBoss::GetMovementMinXBound() + halfBossWidth;
        xPos -= Randomizer::GetInstance()->RandomNumZeroToOne() * (xPos - minX);
    }

    if (xPos - halfBossWidth < DecoBoss::GetMovementMinXBound()) {
        xPos = DecoBoss::GetMovementMinXBound() + halfBossWidth;
    }
    else if (xPos + halfBossWidth > DecoBoss::GetMovementMaxXBound()) {
        xPos = DecoBoss::GetMovementMaxXBound() - halfBossWidth;
    }

    float minY = DecoBoss::ATTACK_PADDLE_WITH_BOTH_ARMS_MIN_Y_POS;
    float maxY = DecoBoss::ATTACK_PADDLE_WITH_BOTH_ARMS_MAX_Y_POS;
    if (!bothArmsAreAlive) {
        minY = DecoBoss::ATTACK_PADDLE_WITH_ONE_ARM_MIN_Y_POS;
        maxY = DecoBoss::ATTACK_PADDLE_WITH_ONE_ARM_MAX_Y_POS;
    }

    // Check to see if the target position is too close to the current position, if it is then we just
    // return the current position -- unless the boss hasn't moved to a good y position yet
    if (fabs(bossPos[0] - xPos) <= 2.0f*paddle->GetHalfWidthTotal() && bossPos[1] <= maxY && bossPos[1] >= minY) {
        return bossPos;
    }
    
    return Point2D(xPos, this->boss->GetRandomAttackPaddleYPos());
}

Point2D DecoBossAIState::GenerateShotOrigin() const {
    return this->boss->alivePartsRoot->GetTranslationPt2D() + DecoBoss::GetLightningFireVec2D();
}

Vector2D DecoBossAIState::GenerateRandomShotDirTowardsPaddle(const Point2D& shotOrigin, GameModel* gameModel) const {
    PlayerPaddle* paddle = gameModel->GetPlayerPaddle();
    assert(paddle != NULL);

    Vector2D attackDir = paddle->GetCenterPosition() - shotOrigin;
    attackDir.Rotate(Randomizer::GetInstance()->RandomNumNegOneToOne() * SHOOT_AT_PADDLE_RANDOM_ROT_ANGLE_IN_DEGS);  // Add a little bit of random rotation
    attackDir.Normalize();

    return attackDir;
}

void DecoBossAIState::ShootLightningBoltAtPaddle(GameModel* gameModel) {
    assert(gameModel != NULL);

    // Don't shoot if the player is in remote-control rocket mode
    if (gameModel->GetTransformInfo()->GetIsRemoteControlRocketCameraOn()) {
        return;
    }

    Point2D shotOrigin = this->GenerateShotOrigin();
    Vector2D attackDir = this->GenerateRandomShotDirTowardsPaddle(shotOrigin, gameModel);

    // Shoot!
    float spdMultiplier = (1.0f + Randomizer::GetInstance()->RandomNumZeroToOne() * 0.25f);
    gameModel->AddProjectile(new BossLightningBoltProjectile(shotOrigin, attackDir, 
        std::max<float>(BossLightningBoltProjectile::SPD_DEFAULT, spdMultiplier * this->GetMaxXSpeed())));

    // Add an effect for it
    // EVENT: Boss shot a laser out of its eye, add effects for it...
    Vector3D offset = this->boss->GetLightningFireVec3D();
    GameEventManager::Instance()->ActionBossEffect(ExpandingHaloEffectInfo(this->boss->GetLightningRelay(), 0.4, 
        Colour(0.9f, 0.78f, 1.0f), 1.5f, offset));
    GameEventManager::Instance()->ActionBossEffect(SparkBurstEffectInfo(this->boss->GetLightningRelay(), 0.55, 
        Colour(0.9f, 0.78f, 1.0f), offset));
}

AnimationMultiLerp<Vector3D> DecoBossAIState::GenerateArmDeathTranslationAnimation(bool isLeft, double timeInSecs) const {
    float xDist = (isLeft ? -1 : 1) * 3 * DecoBoss::ARM_WIDTH;

    float yDist = -1.15f * (isLeft ? this->boss->GetLeftArm()->GetTranslationPt2D()[1] : 
        this->boss->GetRightArm()->GetTranslationPt2D()[1]);
    yDist -= 2 * DecoBoss::ARM_NOT_EXTENDED_HEIGHT;

    Vector2D animVec(xDist, yDist);
    animVec.Rotate(this->currLevelRotationAmtInDegs);

    return Boss::BuildLimbFallOffTranslationAnim(timeInSecs, animVec[0], animVec[1]);
}

AnimationMultiLerp<float> DecoBossAIState::GenerateArmDeathRotationAnimation(bool isLeft, double timeInSecs) const {
    float randomAngle = Randomizer::GetInstance()->RandomUnsignedInt() % 180;
    return Boss::BuildLimbFallOffZRotationAnim(timeInSecs, isLeft ? (360.0f + randomAngle) : -(360.0f + randomAngle));
}

float DecoBossAIState::CalculateSideToSideDropStateVelocity() const {
    float distance   = this->boss->GetSideToSideDropStateDistance();
    int numItemDrops = this->GenerateNumItemsToDropInSideToSideState();

    double timePerItemDrop = this->itemLoadingAnim.GetFinalTime();
    double totalTime       = numItemDrops * timePerItemDrop;

    // v = d/t
    return static_cast<float>(distance / totalTime);
}

bool DecoBossAIState::RemoteControlRocketCheckAndNecessaryStateChange(GameModel* gameModel) {
    
    if (!gameModel->GetTransformInfo()->GetIsRemoteControlRocketCameraOn()) {
        return false;
    }

    // If the boss is already rotating the level then we don't need to do anything more
    if (this->currState == MoveToCenterForLevelRotAIState || this->currState == RotatingLevelAIState) {
        return false;
    }

    // Check to see if the player is currently using the remote control rocket, and the boss has the ability to rotate the level
    // then we leave this state immediately...
    if (!this->boss->IsRightBodyStillAlive() && !this->boss->IsLeftBodyStillAlive()) {
        this->SetState(MoveToCenterForLevelRotAIState);
        return true;
    }
    else {
        // The player is controlling the RC rocket... the boss can't really do much since it hasn't had its arms exposed yet,
        // so if it isn't doing anything important, just move it around and make it look like it's doing stuff
        if (this->currState == StationaryAttackAIState || this->currState == MovingAttackAndItemDropAIState ||
            this->currState == LeftToRightItemDropAIState || this->currState == RightToLeftItemDropAIState ||
            this->currState == MoveToFarLeftSideAIState || this->currState == MoveToFarRightSideAIState) {

            this->SetState(MovingAttackAIState);
        }
    }

    return false;
}

void DecoBossAIState::UpdateBossUpDownSideToSideMotion() {
    // Update the bosses' movement
    Point2D bossPos  = this->boss->alivePartsRoot->GetTranslationPt2D();
    float bossHalfHeight = this->boss->GetCurrentHeight() / 2.0f;
    float bossHalfWidth  = this->boss->GetCurrentWidth()  / 2.0f;

    const float maxXSpeed   = this->GetMaxXSpeed();
    const float maxYSpeed   = this->GetMaxYSpeed();
    const float xBoundBuffer = (maxXSpeed * maxXSpeed) / (2.0f * this->GetAccelerationMagnitude());
    const float yBoundBuffer = (maxYSpeed * maxYSpeed) / (2.0f * this->GetAccelerationMagnitude());

    // Up and down movement
    if (bossPos[1] - bossHalfHeight <= (boss->GetMovementMinYBound() + yBoundBuffer)) {
        this->desiredVel[1] = maxYSpeed;
    }
    else if (bossPos[1] + bossHalfHeight >= (boss->GetMovementMaxYBound() - yBoundBuffer)) {
        this->desiredVel[1] = -maxYSpeed;
    }
    if (fabs(this->desiredVel[1]) < EPSILON) {
        this->desiredVel[1] = maxYSpeed;
    }

    // Side-to-side movement
    if (bossPos[0] - bossHalfWidth <= boss->GetMovementMinXBound() + xBoundBuffer) {
        this->desiredVel[0] = maxXSpeed;
    }
    else if (bossPos[0] + bossHalfWidth >= boss->GetMovementMaxXBound() - xBoundBuffer) {
        this->desiredVel[0] = -maxXSpeed;
    }
    if (fabs(this->desiredVel[0]) < EPSILON) {
        this->desiredVel[0] = Randomizer::GetInstance()->RandomNegativeOrPositive() * maxXSpeed;
    }
}


/// <summary>
/// Updates the shoot countdown and performs the action of shooting at the paddle when necessary.
/// Also updates the state once the number of shots until the next state is reached.
/// </summary>
/// <returns> true if there has been a state change, false if not. </returns>
bool DecoBossAIState::UpdateShootAtPaddleWhileMoving(double dT, GameModel* gameModel) {
    if (this->shootCountdown <= 0.0) {
        // Shoot a bolt at the paddle...
        this->ShootLightningBoltAtPaddle(gameModel);

        // Reset the countdown and decrement the shot count
        this->shootCountdown = this->GenerateShootCountdownWhileMoving();
        this->numShotsUntilNextState--;

        if (this->numShotsUntilNextState <= 0) {
            // Go to the next state if we've exhausted all the shots for this attack state
            this->GoToNextRandomAttackState(gameModel);
            return true;
        }
    }
    else {
        this->shootCountdown -= dT;
    }

    return false;
}

bool DecoBossAIState::UpdateArmAnimation(double dT, AnimationMultiLerp<float>& armSeg1Anim, AnimationMultiLerp<float>& armSeg2Anim, 
                                         AnimationMultiLerp<float>& armSeg3Anim, AnimationMultiLerp<float>& armSeg4Anim) {

    bool allDoneArmAnimation = true;

    allDoneArmAnimation &= armSeg1Anim.Tick(dT);
    allDoneArmAnimation &= armSeg2Anim.Tick(dT);
    allDoneArmAnimation &= armSeg3Anim.Tick(dT);
    allDoneArmAnimation &= armSeg4Anim.Tick(dT);

    // Update the arms with those translations...
    if (!this->boss->GetLeftArm()->GetIsDestroyed()) {
        BossBodyPart* leftArmScoping1 = this->boss->GetLeftArmScopingSeg1Editable();
        BossBodyPart* leftArmScoping2 = this->boss->GetLeftArmScopingSeg2Editable();
        BossBodyPart* leftArmScoping3 = this->boss->GetLeftArmScopingSeg3Editable();
        BossBodyPart* leftArmScoping4 = this->boss->GetLeftArmScopingSeg4Editable();
        BossBodyPart* leftHand = this->boss->GetLeftArmHandEditable();

        leftArmScoping1->SetLocalTranslation(Vector3D(0, armSeg1Anim.GetInterpolantValue(), 0));
        float translation2 = armSeg1Anim.GetInterpolantValue() + armSeg2Anim.GetInterpolantValue();
        leftArmScoping2->SetLocalTranslation(Vector3D(0, translation2, 0));
        float translation3 = translation2 + armSeg3Anim.GetInterpolantValue();
        leftArmScoping3->SetLocalTranslation(Vector3D(0, translation3, 0));
        float translation4 = translation3 + armSeg4Anim.GetInterpolantValue();
        leftArmScoping4->SetLocalTranslation(Vector3D(0, translation4, 0));
        leftHand->SetLocalTranslation(Vector3D(0, translation4, 0));

        Vector2D leftArmOriVec(0,-1);
        leftArmOriVec.Rotate(this->boss->GetLeftArm()->GetLocalZRotation());

        float dxDt = armSeg1Anim.GetDxDt();
        leftArmScoping1->SetExternalAnimationVelocity(dxDt * leftArmOriVec);
        dxDt += armSeg2Anim.GetDxDt();
        leftArmScoping2->SetExternalAnimationVelocity(dxDt * leftArmOriVec);
        dxDt += armSeg3Anim.GetDxDt();
        leftArmScoping3->SetExternalAnimationVelocity(dxDt * leftArmOriVec);
        dxDt += armSeg4Anim.GetDxDt();
        leftArmScoping4->SetExternalAnimationVelocity(dxDt * leftArmOriVec);
        leftHand->SetExternalAnimationVelocity(dxDt * leftArmOriVec);
    }

    if (!this->boss->GetRightArm()->GetIsDestroyed()) {
        BossBodyPart* rightArmScoping1 = this->boss->GetRightArmScopingSeg1Editable();
        BossBodyPart* rightArmScoping2 = this->boss->GetRightArmScopingSeg2Editable();
        BossBodyPart* rightArmScoping3 = this->boss->GetRightArmScopingSeg3Editable();
        BossBodyPart* rightArmScoping4 = this->boss->GetRightArmScopingSeg4Editable();
        BossBodyPart* rightHand = this->boss->GetRightArmHandEditable();

        rightArmScoping1->SetLocalTranslation(Vector3D(0, armSeg1Anim.GetInterpolantValue(), 0));
        float translation2 = armSeg1Anim.GetInterpolantValue() + armSeg2Anim.GetInterpolantValue();
        rightArmScoping2->SetLocalTranslation(Vector3D(0, translation2, 0));
        float translation3 = translation2 + armSeg3Anim.GetInterpolantValue();
        rightArmScoping3->SetLocalTranslation(Vector3D(0, translation3, 0));
        float translation4 = translation3 + armSeg4Anim.GetInterpolantValue();
        rightArmScoping4->SetLocalTranslation(Vector3D(0, translation4, 0));
        rightHand->SetLocalTranslation(Vector3D(0, translation4, 0));

        // Update the animation velocity for all arm pieces (for collision purposes)
        Vector2D rightArmOriVec(0,-1);
        rightArmOriVec.Rotate(this->boss->GetRightArm()->GetLocalZRotation());

        float dxDt = armSeg1Anim.GetDxDt();
        rightArmScoping1->SetExternalAnimationVelocity(dxDt * rightArmOriVec);
        dxDt += armSeg2Anim.GetDxDt();
        rightArmScoping2->SetExternalAnimationVelocity(dxDt * rightArmOriVec);
        dxDt += armSeg3Anim.GetDxDt();
        rightArmScoping3->SetExternalAnimationVelocity(dxDt * rightArmOriVec);
        dxDt += armSeg4Anim.GetDxDt();
        rightArmScoping4->SetExternalAnimationVelocity(dxDt * rightArmOriVec);
        rightHand->SetExternalAnimationVelocity(dxDt * rightArmOriVec);
    }

    return allDoneArmAnimation;
}

bool DecoBossAIState::RotateArmsToDefaultPosition(double dT) {
    GameModel* gameModel = this->boss->GetGameModel();
    GameSound* sound = gameModel->GetSound();

    // Rotate arms back down so that they are facing the paddle again
    bool leftArmFullyRotated  = true;
    bool rightArmFullyRotated = true;

    BossCompositeBodyPart* leftArm = this->boss->GetLeftArmEditable();
    BossCompositeBodyPart* rightArm = this->boss->GetRightArmEditable();

    bool isLeftArmAlive  = !leftArm->GetIsDestroyed();
    bool isRightArmAlive = !rightArm->GetIsDestroyed();

    if (isLeftArmAlive) {
        if (this->currLeftArmRotInDegs < 0.0f) {
            this->currLeftArmRotInDegs += dT * DEFAULT_ARM_ROTATION_SPEED_DEGS_PER_SEC;
            leftArmFullyRotated = false;

            sound->AttachAndPlaySound(leftArm, GameSound::DecoBossArmRotateLoop, true, 
                gameModel->GetCurrentLevelTranslation());
        }
        else {
            this->currLeftArmRotInDegs = 0.0f;
            sound->DetachAndStopSound(leftArm, GameSound::DecoBossArmRotateLoop);
        }
        leftArm->SetLocalZRotation(this->currLeftArmRotInDegs);
    }

    if (isRightArmAlive) {
        if (this->currRightArmRotInDegs > 0.0f) {
            this->currRightArmRotInDegs -= dT * DEFAULT_ARM_ROTATION_SPEED_DEGS_PER_SEC;
            rightArmFullyRotated = false;

            sound->AttachAndPlaySound(rightArm, GameSound::DecoBossArmRotateLoop, true, 
                gameModel->GetCurrentLevelTranslation());
        }
        else {
            this->currRightArmRotInDegs = 0.0f;
            sound->DetachAndStopSound(rightArm, GameSound::DecoBossArmRotateLoop);
        }
        rightArm->SetLocalZRotation(this->currRightArmRotInDegs);
    }

    return (leftArmFullyRotated && rightArmFullyRotated);
}


void DecoBossAIState::DropLoadedItem(GameModel* gameModel) {
    assert(gameModel != NULL);

    gameModel->AddItemDrop(this->boss->GetItemDropPosition(), this->nextDropItemType);
    this->nextDropItemType = this->GenerateRandomItemDropType(gameModel);
    this->itemLoadingAnim.ResetToStart();
}

void DecoBossAIState::InitStationaryAttackState() {
    this->desiredVel = Vector2D(0,0);
    this->currVel    = Vector2D(0,0);

    this->shootCountdown         = this->GenerateShootCountdownWhileStationary();
    this->numShotsUntilNextState = this->GenerateNumShotsWhileStationary();

    assert(this->shootCountdown > 0.0);
    assert(this->numShotsUntilNextState > 0);
}
void DecoBossAIState::InitMovingAttackState() {
    this->shootCountdown         = this->GenerateShootCountdownWhileMoving();
    this->numShotsUntilNextState = this->GenerateNumShotsWhileMoving();

    assert(this->shootCountdown > 0.0);
    assert(this->numShotsUntilNextState > 0);
}
void DecoBossAIState::InitMovingAttackAndItemDropState() {
    this->dropItemCountdown      = this->GenerateItemDropCountdown();
    this->shootCountdown         = this->GenerateShootCountdownWhileMoving();
    this->numShotsUntilNextState = this->GenerateNumShotsWhileMoving();

    assert(this->dropItemCountdown > 0.0);
    assert(this->shootCountdown > 0.0);
    assert(this->numShotsUntilNextState > 0);

    this->itemLoadingAnim.ResetToStart();
}
void DecoBossAIState::InitMoveToFarLeftSideState() {
    this->itemLoadingAnim.ResetToStart();
    this->SetMoveToTargetPosition(this->boss->alivePartsRoot->GetTranslationPt2D(), 
        this->boss->GetFarLeftDropStatePosition());
}
void DecoBossAIState::InitMoveToFarRightSideState() {
    this->itemLoadingAnim.ResetToStart();
    this->SetMoveToTargetPosition(this->boss->alivePartsRoot->GetTranslationPt2D(), 
        this->boss->GetFarRightDropStatePosition());
}
void DecoBossAIState::InitLeftToRightItemDropState() {
    this->SetMoveToTargetPosition(this->boss->alivePartsRoot->GetTranslationPt2D(), 
        this->boss->GetFarRightDropStatePosition());
    this->sideToSideNumDropCountdown = this->GenerateNumItemsToDropInSideToSideState();
}
void DecoBossAIState::InitRightToLeftItemDropState() {
    this->SetMoveToTargetPosition(this->boss->alivePartsRoot->GetTranslationPt2D(), 
        this->boss->GetFarLeftDropStatePosition());
    this->sideToSideNumDropCountdown = this->GenerateNumItemsToDropInSideToSideState();
}
void DecoBossAIState::InitMoveToCenterForLevelRotState() {
    this->SetMoveToTargetPosition(this->boss->alivePartsRoot->GetTranslationPt2D(), 
        this->boss->GetBossPositionForLevelRotation(), 0.0001f);
}
void DecoBossAIState::InitMoveToPaddleArmAttackPosState() {
    this->shootCountdown = this->GenerateTimeToFollowPaddleBeforeShootingArms();
    Point2D currPos = this->boss->alivePartsRoot->GetTranslationPt2D();
    this->SetMoveToTargetPosition(currPos, currPos); 
}
void DecoBossAIState::InitFiringArmsAtPaddleState() {
    this->armSeg1AttackExtendAnim.ResetToStart();
    this->armSeg2AttackExtendAnim.ResetToStart();
    this->armSeg3AttackExtendAnim.ResetToStart();
    this->armSeg4AttackExtendAnim.ResetToStart();

    // Play sound(s) for arm(s) extending
    GameSound* sound = this->boss->GetGameModel()->GetSound();
    if (!this->boss->GetLeftArm()->GetIsDestroyed()) {
        sound->PlaySoundAtPosition(GameSound::DecoBossArmExtendEvent, false, this->boss->GetLeftArm()->GetTranslationPt3D(), true, true, true);
    }
    if (!this->boss->GetRightArm()->GetIsDestroyed()) {
        sound->PlaySoundAtPosition(GameSound::DecoBossArmExtendEvent, false, this->boss->GetRightArm()->GetTranslationPt3D(), true, true, true);
    }
}
void DecoBossAIState::InitFinishedFiringArmsAtPaddleState() {
    this->armSeg1RetractAnim.ResetToStart();
    this->armSeg2RetractAnim.ResetToStart();
    this->armSeg3RetractAnim.ResetToStart();
    this->armSeg4RetractAnim.ResetToStart();

    // Play sound(s) for arm(s) retracting
    GameSound* sound = this->boss->GetGameModel()->GetSound();
    if (!this->boss->GetLeftArm()->GetIsDestroyed()) {
        sound->PlaySoundAtPosition(GameSound::DecoBossArmRetractEvent, false, this->boss->GetLeftArm()->GetTranslationPt3D(), true, true, true);
    }
    if (!this->boss->GetRightArm()->GetIsDestroyed()) {
        sound->PlaySoundAtPosition(GameSound::DecoBossArmRetractEvent, false, this->boss->GetRightArm()->GetTranslationPt3D(), true, true, true);
    }
}
void DecoBossAIState::InitFiringArmsAtLevelState() {
    this->armSeg1RotateExtendAnim.ResetToStart();
    this->armSeg2RotateExtendAnim.ResetToStart();
    this->armSeg3RotateExtendAnim.ResetToStart();
    this->armSeg4RotateExtendAnim.ResetToStart();
}
void DecoBossAIState::InitRotatingLevelState() {
    this->rotateShakeCountdown = this->GenerateRotateShakeCountdown();
    if (this->currLevelRotationAmtInDegs != 0.0) {

        // When the level is already partially rotated, we pick the rotation direction that will
        // get us back to the default orientation the fastest
        if (this->currLevelRotationAmtInDegs < 0) {
            rotationDir = (this->currLevelRotationAmtInDegs < -180.0f) ? 1 : -1;
        }
        else {
            rotationDir = (this->currLevelRotationAmtInDegs > 180.0f) ? 1 : -1;
        }

    }
    else {
        // ... otherwise just choose a random rotation direction
        rotationDir = Randomizer::GetInstance()->RandomNegativeOrPositive();
    }

    // Play the level rotation sound...
    GameSound* sound = this->boss->GetGameModel()->GetSound();
    if (this->rotateLevelSoundID == INVALID_SOUND_ID) {
        this->rotateLevelSoundID = sound->PlaySound(GameSound::DecoBossLevelRotatingLoop, true, true);
    }

}
void DecoBossAIState::InitFinishRotatingLevelState() {
    this->armSeg1RetractAnim.ResetToStart();
    this->armSeg2RetractAnim.ResetToStart();
    this->armSeg3RetractAnim.ResetToStart();
    this->armSeg4RetractAnim.ResetToStart();

    // Play sound(s) for arm(s) retracting
    GameSound* sound = this->boss->GetGameModel()->GetSound();
    if (!this->boss->GetLeftArm()->GetIsDestroyed()) {
        sound->PlaySoundAtPosition(GameSound::DecoBossArmRetractEvent, false, this->boss->GetLeftArm()->GetTranslationPt3D(), true, true, true);
    }
    if (!this->boss->GetRightArm()->GetIsDestroyed()) {
        sound->PlaySoundAtPosition(GameSound::DecoBossArmRetractEvent, false, this->boss->GetRightArm()->GetTranslationPt3D(), true, true, true);
    }
}

void DecoBossAIState::InitElectrifiedState() {
    this->itemLoadingAnim.ResetToStart();
    this->desiredVel = Vector2D(0,0);
    this->currVel    = Vector2D(0,0);

    // Stop all arm sounds and other sounds that might be playing/looped
    static const double SOUND_FADEOUT_TIME = 0.25;
    GameSound* sound = this->boss->GetGameModel()->GetSound();
    sound->DetachAndStopAllSounds(this->boss->GetLeftArm(),  SOUND_FADEOUT_TIME);
    sound->DetachAndStopAllSounds(this->boss->GetRightArm(), SOUND_FADEOUT_TIME);
    sound->StopSound(this->rotateLevelSoundID, SOUND_FADEOUT_TIME);

    // The boss is turned on its side slightly by the sudden jolt
    float rotationAmt = Randomizer::GetInstance()->RandomNegativeOrPositive() * (15.0f + Randomizer::GetInstance()->RandomNumZeroToOne() * 15.0f);
    this->boss->alivePartsRoot->SetLocalZRotation(rotationAmt);

    double animationTime = this->IsFinalAIStage() ? DecoBossAIState::TOTAL_ELECTRIFIED_TIME_IN_SECS : DecoBossAIState::TOTAL_ELECTRIFIED_AND_RETALIATION_TIME_IN_SECS;

    // The boss will shake violently while being electrified
    this->electrifiedHurtAnim = Boss::BuildBossHurtMoveAnim(
        ((rotationAmt < 0) ? Vector2D(1, 0) : Vector2D(-1, 0)), DecoBoss::CORE_WIDTH / 3.0f, animationTime);
    this->boss->alivePartsRoot->AnimateColourRGBA(
        Boss::BuildBossElectrifiedColourAnim(animationTime, 
        GameModelConstants::GetInstance()->TESLA_LIGHTNING_BRIGHT_COLOUR,
        GameModelConstants::GetInstance()->TESLA_LIGHTNING_MEDIUM_COLOUR,
        GameModelConstants::GetInstance()->TESLA_LIGHTNING_DARK_COLOUR));

    // Show effects for the boss being electrified
    GameEventManager::Instance()->ActionBossEffect(ElectrifiedEffectInfo(
        this->boss->alivePartsRoot->GetTranslationPt3D(), DecoBoss::CORE_HEIGHT, animationTime,
        GameModelConstants::GetInstance()->TESLA_LIGHTNING_BRIGHT_COLOUR));
}
void DecoBossAIState::InitElectrificationRetaliationState() {
    this->itemLoadingAnim.ResetToStart();
    this->desiredVel = Vector2D(0,0);
    this->currVel    = Vector2D(0,0);

    // Execute the retaliatory strike to turn off the Tesla blocks...
    GameEventManager::Instance()->ActionBossEffect(ShockwaveEffectInfo(
        this->boss->alivePartsRoot->GetTranslationPt2D(), DecoBoss::CORE_HEIGHT, 
        DecoBossAIState::TOTAL_RETALIATION_TIME_IN_SECS));
}
void DecoBossAIState::InitAngryState() {
    this->itemLoadingAnim.ResetToStart();
    this->desiredVel = Vector2D(0,0);
    this->currVel    = Vector2D(0,0);
    this->boss->alivePartsRoot->AnimateColourRGBA(Boss::BuildBossAngryFlashAnim());
    this->angryMoveAnim.ResetToStart();

    // EVENT: Boss is angry! Rawr.
    GameEventManager::Instance()->ActionBossAngry(this->boss, this->boss->GetCore());
}

void DecoBossAIState::ExecuteStationaryAttackState(double dT, GameModel* gameModel) {
    
    this->currVel    = Vector2D(0,0);
    this->desiredVel = Vector2D(0,0);

    if (this->RemoteControlRocketCheckAndNecessaryStateChange(gameModel)) {
        return;
    }
    
    if (this->shootCountdown <= 0.0) {
    
        // Shoot a bolt at the paddle...
        this->ShootLightningBoltAtPaddle(gameModel);

        // Reset the countdown and decrement the shot count
        this->shootCountdown = this->GenerateShootCountdownWhileStationary();
        this->numShotsUntilNextState--;

        if (this->numShotsUntilNextState <= 0) {
            // Go to the next state if we've exhausted all the shots for this attack state
            this->GoToNextRandomAttackState(gameModel);
            return;
        }
    }
    else {
        this->shootCountdown -= dT;
    }
}

void DecoBossAIState::ExecuteMovingAttackState(double dT, GameModel* gameModel) {
    
    if (this->RemoteControlRocketCheckAndNecessaryStateChange(gameModel)) {
        return;
    }

    // Shoot update
    if (this->UpdateShootAtPaddleWhileMoving(dT, gameModel)) {
        return;
    }

    // Update the bosses motion so that it moves around while it shoots at the paddle
    this->UpdateBossUpDownSideToSideMotion();
}

void DecoBossAIState::ExecuteMovingAttackAndItemDropState(double dT, GameModel* gameModel) {
    if (this->RemoteControlRocketCheckAndNecessaryStateChange(gameModel)) {
        this->itemLoadingAnim.ResetToStart();
        return;
    }

    // Item drop update
    if (this->dropItemCountdown <= 0.0) {

        if (gameModel->GetCurrentStateType() != GameState::BallInPlayStateType) {
            this->dropItemCountdown = this->GenerateItemDropCountdown();
            this->itemLoadingAnim.ResetToStart();
        }
        else {
            // Stop the boss momentarily while the item is being prepared
            this->desiredVel = Vector2D(0,0);

            // Update the item preparation animation...
            if (this->itemLoadingAnim.IsAtStart()) {
                this->nextDropItemType = this->GenerateRandomItemDropType(gameModel);
            }
            bool isItemDoneLoading = this->itemLoadingAnim.Tick(dT);
            if (isItemDoneLoading) {
                // The item is fully prepared for dropping, drop it
                this->DropLoadedItem(gameModel);
                this->dropItemCountdown = this->GenerateItemDropCountdown();
            }

            return;
        }
    }
    else {
        this->dropItemCountdown -= dT;
    }

    // Shoot update
    if (this->UpdateShootAtPaddleWhileMoving(dT, gameModel)) {
        return;
    }

    // Update the bosses motion so that it moves around while it shoots at the paddle
    this->UpdateBossUpDownSideToSideMotion();
}

void DecoBossAIState::ExecuteMoveToFarLeftSideState(double dT, GameModel* gameModel) {
    if (this->RemoteControlRocketCheckAndNecessaryStateChange(gameModel)) {
        this->itemLoadingAnim.ResetToStart();
        return;
    }

    // Leave this state immediately if the ball is not in play
    if (gameModel->GetCurrentStateType() != GameState::BallInPlayStateType) {
        this->itemLoadingAnim.ResetToStart();
        this->SetState(MovingAttackAIState);
        return;
    }    
    
    this->itemLoadingAnim.Tick(dT);
    if (this->MoveToTargetPosition(this->GetMaxXSpeed())) {
        // Done moving to the target, go to the next state
        this->SetState(LeftToRightItemDropAIState);
    }
}

void DecoBossAIState::ExecuteMoveToFarRightSideState(double dT, GameModel* gameModel) {
    if (this->RemoteControlRocketCheckAndNecessaryStateChange(gameModel)) {
        this->itemLoadingAnim.ResetToStart();
        return;
    }

    // Leave this state immediately if the ball is not in play
    if (gameModel->GetCurrentStateType() != GameState::BallInPlayStateType) {
        this->itemLoadingAnim.ResetToStart();
        this->SetState(MovingAttackAIState);
        return;
    }

    this->itemLoadingAnim.Tick(dT);
    if (this->MoveToTargetPosition(this->GetMaxXSpeed())) {
        // Done moving to the target, go to the next state
        this->SetState(RightToLeftItemDropAIState);
    }
}

void DecoBossAIState::ExecuteMoveToCenterForLevelRotState(double dT, GameModel* gameModel) {
    UNUSED_PARAMETER(dT);

    if (this->MoveToTargetPosition(0.8f * this->GetMaxXSpeed())) {
        // Done moving to the target, go to the next state

        // If the ball is not in play or in the lower part of the level, then don't fire the arms and just go back to attacking
        const PlayerPaddle* paddle = gameModel->GetPlayerPaddle();
        assert(paddle != NULL);
        const GameBall* ball = gameModel->GetGameBalls().front();
        assert(ball != NULL);

        if ((gameModel->GetCurrentStateType() == GameState::BallInPlayStateType &&
             ball->GetCenterPosition2D()[1] <= this->boss->GetYBallMaxForLevelRotate()) ||
             gameModel->GetTransformInfo()->GetIsRemoteControlRocketCameraOn() ||
             this->currLevelRotationAmtInDegs != 0.0f) {

            this->SetState(FiringArmsAtLevelAIState);
        }
        else {
            this->GoToNextRandomAttackState(gameModel);
        }
    }
}

void DecoBossAIState::ExecuteFiringArmsAtLevelState(double dT, GameModel* gameModel) {

    GameSound* sound = gameModel->GetSound();

    // Shake arms...
    BossCompositeBodyPart* leftArm = this->boss->GetLeftArmEditable();
    BossCompositeBodyPart* rightArm = this->boss->GetRightArmEditable();

    // Make sure arm(s) are shaking and pointed towards the sides of the level
    // (left arm -90 degrees, right arm +90 degrees)
    
    bool armsFullyRotated = true;
    bool isLeftArmAlive  = !leftArm->GetIsDestroyed();
    bool isRightArmAlive = !rightArm->GetIsDestroyed();

    if (isLeftArmAlive) {

        if (this->currLeftArmRotInDegs > DecoBoss::LEFT_ARM_HORIZ_ORIENT_ROT_ANGLE_IN_DEGS) {
            this->currLeftArmRotInDegs -= dT * DEFAULT_ARM_ROTATION_SPEED_DEGS_PER_SEC;
            armsFullyRotated &= false;

            sound->AttachAndPlaySound(leftArm, GameSound::DecoBossArmRotateLoop, true, 
                gameModel->GetCurrentLevelTranslation());

            float shakeAmt = this->leftArmShakeAnim.GetInterpolantValue();
            this->leftArmShakeAnim.Tick(dT);
            leftArm->SetLocalTranslation(Vector3D(shakeAmt, 0.0f, 0.0f));
        }
        else {
            // Left arm is now fully rotated
            this->currLeftArmRotInDegs = DecoBoss::LEFT_ARM_HORIZ_ORIENT_ROT_ANGLE_IN_DEGS;
            leftArm->SetLocalTranslation(Vector3D(0.0f, 0.0f, 0.0f));
            sound->DetachAndStopSound(leftArm, GameSound::DecoBossArmRotateLoop);
        }

        leftArm->SetLocalZRotation(this->currLeftArmRotInDegs);

    }
    if (isRightArmAlive) {

        if (this->currRightArmRotInDegs < DecoBoss::RIGHT_ARM_HORIZ_ORIENT_ROT_ANGLE_IN_DEGS) {
            this->currRightArmRotInDegs += dT * DEFAULT_ARM_ROTATION_SPEED_DEGS_PER_SEC;
            armsFullyRotated &= false;

            sound->AttachAndPlaySound(rightArm, GameSound::DecoBossArmRotateLoop, true, 
                gameModel->GetCurrentLevelTranslation());

            float shakeAmt = this->rightArmShakeAnim.GetInterpolantValue();
            this->rightArmShakeAnim.Tick(dT);
            rightArm->SetLocalTranslation(Vector3D(shakeAmt, 0.0f, 0.0f));
        }
        else {
            // Right arm is now fully rotated
            this->currRightArmRotInDegs = DecoBoss::RIGHT_ARM_HORIZ_ORIENT_ROT_ANGLE_IN_DEGS;
            rightArm->SetLocalTranslation(Vector3D(0.0f, 0.0f, 0.0f));
            sound->DetachAndStopSound(rightArm, GameSound::DecoBossArmRotateLoop);
        }

        rightArm->SetLocalZRotation(this->currRightArmRotInDegs);
    }

    // If both arms are now properly rotated to be pointing at the sides of the level,
    // we can fire both arms at the sides
    if (armsFullyRotated) {

        sound->DetachAndStopSound(rightArm, GameSound::DecoBossArmRotateLoop);
        sound->DetachAndStopSound(leftArm, GameSound::DecoBossArmRotateLoop);

        // Play sound for each segment of the arm extending
        if ((this->armSeg1RotateExtendAnim.GetInterpolantValue() == 0.0f) ||
            (this->armSeg1RotateExtendAnim.GetInterpolantValue() == this->armSeg1RotateExtendAnim.GetFinalInterpolationValue() && this->armSeg2RotateExtendAnim.GetInterpolantValue() == 0.0f) ||
            (this->armSeg2RotateExtendAnim.GetInterpolantValue() == this->armSeg2RotateExtendAnim.GetFinalInterpolationValue() && this->armSeg3RotateExtendAnim.GetInterpolantValue() == 0.0f) ||
            (this->armSeg3RotateExtendAnim.GetInterpolantValue() == this->armSeg3RotateExtendAnim.GetFinalInterpolationValue() && this->armSeg4RotateExtendAnim.GetInterpolantValue() == 0.0f)) {
            
            if (isLeftArmAlive) {
                sound->PlaySoundAtPosition(GameSound::DecoBossArmExtendEvent, false, leftArm->GetPosition3D(), true, true, true);
            }
            if (isRightArmAlive) {
                sound->PlaySoundAtPosition(GameSound::DecoBossArmExtendEvent, false, rightArm->GetPosition3D(), true, true, true);
            }
        }

        bool allDoneArmAnimation = this->UpdateArmAnimation(dT, this->armSeg1RotateExtendAnim, 
            this->armSeg2RotateExtendAnim, this->armSeg3RotateExtendAnim, this->armSeg4RotateExtendAnim);

        if (allDoneArmAnimation) {
            // Shake the level (the arms have now reached the sides and are digging into them)
            GameEventManager::Instance()->ActionGeneralEffect(LevelShakeEffectInfo(1.0, Vector3D(0.7f, 0.1f, 0.0f), 100));

            // Play a sound for the level getting hit by the arm(s)
            if (isLeftArmAlive) {
                Point3D collisionPt = this->boss->GetLeftArmHand()->GetPosition3D();
                collisionPt[0] -= DecoBoss::ARM_ORIGIN_TO_HAND_END;
                sound->PlaySoundAtPosition(GameSound::DecoBossArmLevelCollisionEvent, false, collisionPt, true, true, true);
                GameEventManager::Instance()->ActionGeneralEffect(StarSmashEffectInfo(collisionPt.ToPoint2D(), Vector2D(-1,0), DecoBoss::HAND_WIDTH / 1.75f, 
                    2.0, Onomatoplex::SUPER_AWESOME));
            }
            if (isRightArmAlive) {
                Point3D collisionPt = this->boss->GetRightArmHand()->GetPosition3D();
                collisionPt[0] += DecoBoss::ARM_ORIGIN_TO_HAND_END;
                sound->PlaySoundAtPosition(GameSound::DecoBossArmLevelCollisionEvent, false, collisionPt, true, true, true);
                GameEventManager::Instance()->ActionGeneralEffect(StarSmashEffectInfo(collisionPt.ToPoint2D(), Vector2D(1,0), DecoBoss::HAND_WIDTH / 1.75f, 
                    2.0, Onomatoplex::SUPER_AWESOME));
            }

            // Start rotating!
            this->SetState(RotatingLevelAIState);
        }
    }
}

void DecoBossAIState::ExecuteMoveToPaddleArmAttackPosState(double dT, GameModel* gameModel) {
    GameSound* sound = gameModel->GetSound();

    BossCompositeBodyPart* leftArm = this->boss->GetLeftArmEditable();
    BossCompositeBodyPart* rightArm = this->boss->GetRightArmEditable();

    if (this->RemoteControlRocketCheckAndNecessaryStateChange(gameModel)) {
        // Reset the translations on both arms
        leftArm->SetLocalTranslation(Vector3D(0.0f, 0.0f, 0.0f));
        rightArm->SetLocalTranslation(Vector3D(0.0f, 0.0f, 0.0f));
        return;
    }

    bool isLeftArmAlive  = !leftArm->GetIsDestroyed();
    bool isRightArmAlive = !rightArm->GetIsDestroyed();

    if (!isLeftArmAlive || !isRightArmAlive) {

        // Only one of the arms is still alive, it will rotate to try and hit the paddle
        const PlayerPaddle* paddle = gameModel->GetPlayerPaddle();
        assert(paddle != NULL);

        Point2D rotCenterPt = this->boss->GetLeftArmGear()->GetTranslationPt2D();
        const Point2D& paddleCenterPt = paddle->GetCenterPosition();

        Vector2D attackVec = paddleCenterPt - rotCenterPt;
        assert(!attackVec.IsZero());

        float atan2Angle = atan2(attackVec[1], attackVec[0]);

        if (isLeftArmAlive) {
            // atan2 returns a negative angle when in the half plane of y < 0, in [-pi, 0]. For the left arm,
            // that angle must be in the [-pi, -pi/2] range for the arm to be able to attack
            if (atan2Angle <= -M_PI_DIV2 && atan2Angle >= -M_PI) {

                // Add 90 degrees (pi/2 radians) to the angle to get the required rotation of the left arm
                float targetLeftArmRotAngle = atan2Angle + M_PI_DIV2;
                // Left arm rotation should be in [-pi/2, 0]
                assert(targetLeftArmRotAngle <= 0.0f && targetLeftArmRotAngle >= -M_PI_DIV2);

                // Start rotating the left arm towards the target angle
                targetLeftArmRotAngle = Trig::radiansToDegrees(targetLeftArmRotAngle);
                float dA = dT * DEFAULT_ARM_ROTATION_SPEED_DEGS_PER_SEC;
                if (fabs(this->currLeftArmRotInDegs - targetLeftArmRotAngle) < dA) {
                    // No more rotating needs to be done
                }
                else {
                    if (this->currLeftArmRotInDegs < targetLeftArmRotAngle) {
                        this->currLeftArmRotInDegs += dA;
                    }
                    else {
                        this->currLeftArmRotInDegs -= dA;
                    }

                    sound->AttachAndPlaySound(leftArm, GameSound::DecoBossArmRotateLoop, true, 
                        gameModel->GetCurrentLevelTranslation());

                    leftArm->SetLocalZRotation(this->currLeftArmRotInDegs);
                }
            }

            float shakeAmt = this->leftArmShakeAnim.GetInterpolantValue();
            this->leftArmShakeAnim.Tick(dT);
            leftArm->SetLocalTranslation(Vector3D(shakeAmt, 0.0f, 0.0f));
        }
        else if (isRightArmAlive) {
            // atan2 returns a negative angle when in the half plane of y < 0, in [-pi, 0]. For the right arm,
            // that angle must be in the [-pi/2, 0] range for the arm to be able to attack
            if (atan2Angle >= -M_PI_DIV2 && atan2Angle <= 0.0f) {
                // Add 90 degrees (pi/2 radians) to the angle to get the required rotation of the right arm
                float targetRightArmRotAngle = atan2Angle + M_PI_DIV2;
                // Left arm rotation should be in [0, pi/2]
                assert(targetRightArmRotAngle >= 0.0f && targetRightArmRotAngle <= M_PI_DIV2);

                // Start rotating the right arm towards the target angle
                targetRightArmRotAngle = Trig::radiansToDegrees(targetRightArmRotAngle);
                float dA = dT * DEFAULT_ARM_ROTATION_SPEED_DEGS_PER_SEC;
                if (fabs(this->currRightArmRotInDegs - targetRightArmRotAngle) < dA) {
                    // No more rotating needs to be done
                }
                else {
                    if (this->currRightArmRotInDegs < targetRightArmRotAngle) {
                        this->currRightArmRotInDegs += dA;
                    }
                    else {
                        this->currRightArmRotInDegs -= dA;
                    }

                    sound->AttachAndPlaySound(rightArm, GameSound::DecoBossArmRotateLoop, true, 
                        gameModel->GetCurrentLevelTranslation());

                    rightArm->SetLocalZRotation(this->currRightArmRotInDegs);
                }
            }
            float shakeAmt = this->rightArmShakeAnim.GetInterpolantValue();
            this->rightArmShakeAnim.Tick(dT);
            rightArm->SetLocalTranslation(Vector3D(shakeAmt, 0.0f, 0.0f));
        }
        else {
            // This shouldn't happen -- boss will always have at least one arm to attack with
            assert(false);
            this->GoToNextRandomAttackState(gameModel);
            return;
        }
    }
    else {
        float shakeAmt = this->rightArmShakeAnim.GetInterpolantValue();
        this->rightArmShakeAnim.Tick(dT);
        rightArm->SetLocalTranslation(Vector3D(shakeAmt, 0.0f, 0.0f));
        shakeAmt = this->leftArmShakeAnim.GetInterpolantValue();
        this->leftArmShakeAnim.Tick(dT);
        leftArm->SetLocalTranslation(Vector3D(shakeAmt, 0.0f, 0.0f));

        Vector3D levelTranslation = gameModel->GetCurrentLevelTranslation();
        sound->AttachAndPlaySound(leftArm, GameSound::DecoBossArmRotateLoop, true, levelTranslation);
        sound->AttachAndPlaySound(rightArm, GameSound::DecoBossArmRotateLoop, true, levelTranslation);
    }

    this->shootCountdown -= dT;
    if (this->MoveToTargetPosition(this->GetMaxYSpeed())) {

        // Choose a new attack position near the paddle
        this->SetMoveToTargetPosition(this->boss->alivePartsRoot->GetTranslationPt2D(),
            this->GeneratePaddleArmAttackPosition(gameModel));

        if (this->shootCountdown <= 0.0) {
            // Reset the translations on both arms
            leftArm->SetLocalTranslation(Vector3D(0.0f, 0.0f, 0.0f));
            rightArm->SetLocalTranslation(Vector3D(0.0f, 0.0f, 0.0f));

            // Attack the paddle with the arms of the boss
            gameModel->GetPlayerPaddle()->ResetLastEntityThatHurtPaddle();
            this->SetState(FiringArmsAtPaddleAIState);
        }
    }
}

void DecoBossAIState::ExecuteFiringArmsAtPaddleState(double dT, GameModel* gameModel) {

    // Arm(s) should no longer sound like they're rotating
    GameSound* sound = gameModel->GetSound();
    sound->DetachAndStopSound(this->boss->GetLeftArm(),  GameSound::DecoBossArmRotateLoop, 0.5);
    sound->DetachAndStopSound(this->boss->GetRightArm(), GameSound::DecoBossArmRotateLoop, 0.5);

    // Fire the arms downwards
    bool allDoneArmAnimation = this->UpdateArmAnimation(dT, this->armSeg1AttackExtendAnim, 
        this->armSeg2AttackExtendAnim, this->armSeg3AttackExtendAnim, this->armSeg4AttackExtendAnim);

    if (allDoneArmAnimation) {
        this->SetState(FinishedFiringArmsAtPaddleAIState);
    }
}

void DecoBossAIState::ExecuteFinishedFiringArmsAtPaddleState(double dT, GameModel* gameModel) {

    bool doneRetractAnim = this->UpdateArmAnimation(dT, this->armSeg1RetractAnim, 
        this->armSeg2RetractAnim, this->armSeg3RetractAnim, this->armSeg4RetractAnim);

    // If one of the arms is dead then we need to rotate the arm back to the default position as well
    bool isLeftArmDead  = this->boss->GetLeftArm()->GetIsDestroyed();
    bool isRightArmDead = this->boss->GetRightArm()->GetIsDestroyed();
    if (isLeftArmDead || isRightArmDead) {
        // If we're done retracting the arms, rotate them to default position
        if (doneRetractAnim) {
            if (this->RotateArmsToDefaultPosition(dT)) {
                this->GoToNextRandomAttackState(gameModel);
            }
        }
    }
    else if (doneRetractAnim) {
        this->GoToNextRandomAttackState(gameModel);
    }
}

void DecoBossAIState::ExecuteRotatingLevelState(double dT, GameModel* gameModel) {
    assert(gameModel != NULL);
    
    GameSound* sound = gameModel->GetSound();

    // If the ball is dying then we stop rotating for a brief period of time
    if (gameModel->GetCurrentStateType() == GameState::BallDeathStateType) {
        return;
    }

    GameTransformMgr* transformMgr = gameModel->GetTransformInfo();
    assert(transformMgr != NULL);
    transformMgr->SetGameZRotation(this->currLevelRotationAmtInDegs, *gameModel->GetCurrentLevel());

    // The condition for stopping rotation is based on the direction
    bool rotationComplete = this->rotationDir > 0 ? 
        (this->currLevelRotationAmtInDegs >= DEFAULT_LEVEL_ROTATION_AMT_IN_DEGS) :
        (this->currLevelRotationAmtInDegs <= -DEFAULT_LEVEL_ROTATION_AMT_IN_DEGS);

    if (rotationComplete) {

        this->currLevelRotationAmtInDegs = 0.0;
        if (gameModel->GetTransformInfo()->GetIsRemoteControlRocketCameraOn()) {
            // Keep rotating but maybe switch the direction...
            this->rotationDir = Randomizer::GetInstance()->RandomNegativeOrPositive();
        }
        else {
            // Shake the level to indicate the boss is done with rotating, also the sudden halt is 
            // emphasized by the shake
            GameEventManager::Instance()->ActionGeneralEffect(LevelShakeEffectInfo(1.0, Vector3D(0.7f, 0.2f, 0.0f), 100));

            // Stop the rotating sound
            sound->StopSound(this->rotateLevelSoundID, 0.5);
            this->SetState(FinishRotatingLevelAIState);
            return;
        }
    }
    else {

        // If the remote control rocket camera isn't active and the ball is stuck above the boss 
        // then we speed up the rotation...
        float currLevelRotSpd = this->GetLevelRotationSpeed();
        const GameBall* ball = gameModel->GetGameBalls().front();
        if (!gameModel->GetTransformInfo()->GetIsRemoteControlRocketCameraOn() &&
            ball->GetCenterPosition2D()[1] > this->boss->GetYBallMaxForLevelRotate()) {

            currLevelRotSpd *= 1.5f;
        }

        // Rotate the level...
        this->currLevelRotationAmtInDegs += dT * this->rotationDir * currLevelRotSpd;
    }

    if (this->rotateShakeCountdown <= 0.0) {
        // Shake the level a little bit while rotating...
        float xDir = Randomizer::GetInstance()->RandomNegativeOrPositive() * 0.7f;
        float yDir = Randomizer::GetInstance()->RandomNegativeOrPositive() * 0.15f;
        float magnitude = 50 + Randomizer::GetInstance()->RandomNumZeroToOne() * 25;
        GameEventManager::Instance()->ActionGeneralEffect(LevelShakeEffectInfo(0.25, Vector3D(xDir, yDir, 0.0f), magnitude));

        this->rotateShakeCountdown = this->GenerateRotateShakeCountdown();
    }
    else {
        this->rotateShakeCountdown -= dT;
    }
}

void DecoBossAIState::ExecuteFinishRotatingLevelState(double dT, GameModel* gameModel) {
    
    bool allDoneArmAnimation = this->UpdateArmAnimation(dT, this->armSeg1RetractAnim, 
        this->armSeg2RetractAnim, this->armSeg3RetractAnim, this->armSeg4RetractAnim);

    if (allDoneArmAnimation) {
        // Rotate arms back down so that they are facing the paddle again
        if (this->RotateArmsToDefaultPosition(dT)) {
            this->GoToNextRandomAttackState(gameModel);
        }
    }
}

void DecoBossAIState::ExecuteSideToSideItemDropState(double dT, GameModel* gameModel) {

    if (this->RemoteControlRocketCheckAndNecessaryStateChange(gameModel)) {
        this->itemLoadingAnim.ResetToStart();
        return;
    }

    // Leave this state immediately if the ball is not in play
    if (gameModel->GetCurrentStateType() != GameState::BallInPlayStateType) {
        this->itemLoadingAnim.ResetToStart();
        this->SetState(MovingAttackAIState);
        return;
    }
    
    bool isMovementFinished = this->MoveToTargetPosition(this->CalculateSideToSideDropStateVelocity());

    // Drop an item every chance that we can get, constrained by the item animation
    if (this->sideToSideNumDropCountdown > 0) {
        if (this->itemLoadingAnim.IsAtStart()) {
            this->nextDropItemType = this->GenerateRandomItemDropType(gameModel);
        }
        bool isItemDoneLoading = this->itemLoadingAnim.Tick(dT);
        if (isItemDoneLoading) {
            // Drop an item and reset the animation
            this->DropLoadedItem(gameModel);
            this->itemLoadingAnim.ResetToStart();
            this->sideToSideNumDropCountdown--;
        }
    }
    else if (isMovementFinished) {
        // We're done with this state, move on to the next one
        this->GoToNextRandomAttackState(gameModel);
    }
}

void DecoBossAIState::ExecuteElectrifiedState(double dT, GameModel* gameModel) {
    UNUSED_PARAMETER(gameModel);

    this->currVel = Vector2D(0,0);
    this->desiredVel = Vector2D(0,0);

    // Retract the arms (if they're currently extended)
    bool armAnimIsFinished = this->UpdateArmAnimation(dT, this->armSeg1RetractAnim, this->armSeg1RetractAnim, 
        this->armSeg1RetractAnim, this->armSeg1RetractAnim);
    bool isFinishedHurtAnim = this->electrifiedHurtAnim.Tick(dT);

    this->boss->alivePartsRoot->SetLocalTranslation(this->electrifiedHurtAnim.GetInterpolantValue());
    if (this->electrifiedHurtAnim.GetCurrentTimeValue() >= TOTAL_ELECTRIFIED_TIME_IN_SECS || isFinishedHurtAnim && armAnimIsFinished) {

        if (this->IsFinalAIStage() && isFinishedHurtAnim) {

            // Stop any colour animation that was set for this state
            this->boss->alivePartsRoot->ResetColourRGBAAnimation();
            // Clean up all the translations and rotations on the body of the boss for this state
            this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0,0,0));

            this->SetState(FinalDeathThroesAIState);
        }
        else {
            this->SetState(ElectrificationRetaliationAIState);
        }
    }
}

void DecoBossAIState::ExecuteElectrifiedRetaliationState(double dT, GameModel* gameModel) {

    this->currVel = Vector2D(0,0);
    this->desiredVel = Vector2D(0,0);
    
    bool isFinished = this->electrifiedHurtAnim.Tick(dT);
    this->boss->alivePartsRoot->SetLocalTranslation(this->electrifiedHurtAnim.GetInterpolantValue());
    if (isFinished) {

        // Stop any colour animation that was set for this state
        this->boss->alivePartsRoot->ResetColourRGBAAnimation();
        // Clean up all the translations and rotations on the body of the boss for this state
        this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0,0,0));

        static const double ROTATE_BACK_TO_UPRIGHT_TIME_IN_SECS = 1.0;
        AnimationMultiLerp<float> rotAnim;
        rotAnim.SetInterpolantValue(this->boss->alivePartsRoot->GetLocalZRotation());
        rotAnim.SetLerp(ROTATE_BACK_TO_UPRIGHT_TIME_IN_SECS, 0.0);
        rotAnim.SetRepeat(false);
        this->boss->alivePartsRoot->AnimateLocalZRotation(rotAnim);

        // Turn off the two Tesla blocks that have been electrifying the boss
        assert(gameModel != NULL);
        
        GameLevel* currLevel = gameModel->GetCurrentLevel();
        assert(currLevel != NULL);
        
        TeslaBlock* leftTeslaBlock  = DecoBoss::GetLeftTeslaBlock(*currLevel);
        TeslaBlock* rightTeslaBlock = DecoBoss::GetRightTeslaBlock(*currLevel);
        assert(leftTeslaBlock != NULL && rightTeslaBlock != NULL);

        if (leftTeslaBlock->GetIsElectricityActive() && this->ShutsOffLeftTeslaBlockOnRetaliation()) {
            const Point2D& center = leftTeslaBlock->GetCenter();
            leftTeslaBlock->ToggleElectricity(*gameModel, *currLevel, true);
            GameEventManager::Instance()->ActionGeneralEffect(ShortCircuitEffectInfo(
                center, GameModelConstants::GetInstance()->TESLA_LIGHTNING_BRIGHT_COLOUR, 
                GameModelConstants::GetInstance()->TESLA_LIGHTNING_MEDIUM_COLOUR, 
                GameModelConstants::GetInstance()->TESLA_LIGHTNING_DARK_COLOUR,
                LevelPiece::PIECE_WIDTH, 2.0));
        }
        if (rightTeslaBlock->GetIsElectricityActive() && this->ShutsOffRightTeslaBlockOnRetaliation()) {
            const Point2D& center = rightTeslaBlock->GetCenter();
            rightTeslaBlock->ToggleElectricity(*gameModel, *currLevel, true);
            GameEventManager::Instance()->ActionGeneralEffect(ShortCircuitEffectInfo(
                center, GameModelConstants::GetInstance()->TESLA_LIGHTNING_BRIGHT_COLOUR, 
                GameModelConstants::GetInstance()->TESLA_LIGHTNING_MEDIUM_COLOUR, 
                GameModelConstants::GetInstance()->TESLA_LIGHTNING_DARK_COLOUR,
                LevelPiece::PIECE_WIDTH, 2.0));
        }

        this->SetState(AngryAIState);
    }
}

void DecoBossAIState::ExecuteAngryState(double dT) {
    this->currVel = Vector2D(0,0);
    this->desiredVel = Vector2D(0,0);

    bool isFinished = this->angryMoveAnim.Tick(dT);
    this->boss->alivePartsRoot->SetLocalTranslation(this->angryMoveAnim.GetInterpolantValue());
    if (isFinished) {

        // Stop any colour animation that was set for this state
        this->boss->alivePartsRoot->ResetColourRGBAAnimation();
        // Clean up all the translations on the body of the boss for this state
        this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0,0,0));

        // Go to the next top-level AI state...
        DecoBossAIState* nextAIState = this->BuildNextAIState();
        assert(nextAIState != NULL);
        this->boss->SetNextAIState(nextAIState);
    }
}

void DecoBossAIState::ExecuteFinalDeathThroesState() {
    this->desiredVel = Vector2D(0,0);
    this->currVel    = Vector2D(0,0);

    // The boss is dead dead dead.
}

void DecoBossAIState::TeslaLightningArcHitOccurred(GameModel* gameModel, const TeslaBlock* block1, const TeslaBlock* block2) {
    UNUSED_PARAMETER(gameModel);
    UNUSED_PARAMETER(block1);
    UNUSED_PARAMETER(block2);

    // Ignore if we've already been through this (shouldn't happen, but just for robustness)
    if (this->currState == ElectrifiedAIState || this->currState == ElectrificationRetaliationAIState ||
        this->currState == AngryAIState || this->currState == FinalDeathThroesAIState) {

        return;
    }

    // Immediately change the boss' state to being electrocuted
    this->SetState(ElectrifiedAIState);
}

void DecoBossAIState::UpdateState(double dT, GameModel* gameModel) {

    switch (this->currState) {
        case StationaryAttackAIState:
            this->ExecuteStationaryAttackState(dT, gameModel);
            break;
        case MovingAttackAIState:
            this->ExecuteMovingAttackState(dT, gameModel);
            break;
        case MovingAttackAndItemDropAIState:
            this->ExecuteMovingAttackAndItemDropState(dT, gameModel);
            break;
       
        case MoveToFarLeftSideAIState:
            this->ExecuteMoveToFarLeftSideState(dT, gameModel);
            break;
        case MoveToFarRightSideAIState:
            this->ExecuteMoveToFarRightSideState(dT, gameModel);
            break;
        case LeftToRightItemDropAIState:
            this->ExecuteSideToSideItemDropState(dT, gameModel);
            break;
        case RightToLeftItemDropAIState:
            this->ExecuteSideToSideItemDropState(dT, gameModel);
            break;

        case MoveToCenterForLevelRotAIState:
            this->ExecuteMoveToCenterForLevelRotState(dT, gameModel);
            break;
        case RotatingLevelAIState:
            this->ExecuteRotatingLevelState(dT, gameModel);
            break;
        case FinishRotatingLevelAIState:
            this->ExecuteFinishRotatingLevelState(dT, gameModel);
            break;
        case FiringArmsAtLevelAIState:
            this->ExecuteFiringArmsAtLevelState(dT, gameModel);
            break;

        case MoveToPaddleArmAttackPosAIState:
            this->ExecuteMoveToPaddleArmAttackPosState(dT, gameModel);
            break;
        case FiringArmsAtPaddleAIState:
            this->ExecuteFiringArmsAtPaddleState(dT, gameModel);
            break;
        case FinishedFiringArmsAtPaddleAIState:
            this->ExecuteFinishedFiringArmsAtPaddleState(dT, gameModel);
            break;

        case ElectrifiedAIState:
            this->ExecuteElectrifiedState(dT, gameModel);
            break;
        case ElectrificationRetaliationAIState:
            this->ExecuteElectrifiedRetaliationState(dT, gameModel);
            break;
        case AngryAIState:
            this->ExecuteAngryState(dT);
            break;

        case FinalDeathThroesAIState:
            this->ExecuteFinalDeathThroesState();
            break;

        default:
            assert(false);
            return;
    }
}

// Stage1AI Functions ********************************************************************

Stage1AI::Stage1AI(DecoBoss* boss) : DecoBossAIState(boss) {
    // Boss starts off by attacking
    this->SetState(MovingAttackAIState);
}

Stage1AI::~Stage1AI() {
}

GameItem::ItemType Stage1AI::GenerateRandomItemDropType(GameModel* gameModel) const {
    assert(gameModel != NULL);
    if (this->nextDropItemType != GameItem::PaddleShrinkItem &&
        gameModel->GetPlayerPaddle()->GetPaddleSize() != PlayerPaddle::SmallestSize) {
        if (Randomizer::GetInstance()->RandomUnsignedInt() % 4 == 0) {
            return GameItem::PaddleShrinkItem;
        }
    }

    switch (Randomizer::GetInstance()->RandomUnsignedInt() % 5) {
        case 0: case 1: default:
            if (gameModel->GetPlayerPaddle()->GetPaddleSize() != PlayerPaddle::SmallestSize &&
                this->nextDropItemType != GameItem::PaddleShrinkItem) {
                return GameItem::PaddleShrinkItem;
            }
            else {
                switch (Randomizer::GetInstance()->RandomUnsignedInt() % 3) {
                    case 0:
                        return GameItem::BallSpeedUpItem;
                    case 1:
                        return GameItem::InvisiPaddleItem;
                    case 2:
                    default:
                        return GameItem::InvisiBallItem;
                }
            }
        case 2:
            return GameItem::BallSpeedUpItem;
        case 3:
            return GameItem::InvisiPaddleItem;
        case 4:
            return GameItem::InvisiBallItem;
    }
}

DecoBossAIState* Stage1AI::BuildNextAIState() const {
    return new Stage2AI(this->boss);
}

void Stage1AI::SetState(DecoBossAIState::AIState newState) {
    switch (newState) {
        case StationaryAttackAIState:
            this->InitStationaryAttackState();
            break;
        case MovingAttackAIState:
            this->InitMovingAttackState();
            break;
        case MovingAttackAndItemDropAIState:
            this->InitMovingAttackAndItemDropState();
            break;
        case MoveToFarLeftSideAIState:
            this->InitMoveToFarLeftSideState();
            break;
        case MoveToFarRightSideAIState:
            this->InitMoveToFarRightSideState();
            break;
        case LeftToRightItemDropAIState:
            this->InitLeftToRightItemDropState();
            break;
        case RightToLeftItemDropAIState:
            this->InitRightToLeftItemDropState();
            break;

        case ElectrifiedAIState: 
            this->InitElectrifiedState();
            break;
        case ElectrificationRetaliationAIState: {
            this->InitElectrificationRetaliationState();

            // By blowing off the body, we will be exposing the arms, generate bounding lines for them so that the ball doesn't get
            // caught in them while we're animating the arms blowing off
            boss->GenerateArmAndRemainingBoundingLines();

            // Blow off the left and right body parts of the boss...
            static const double TOTAL_BLOWUP_TIME = DecoBossAIState::TOTAL_RETALIATION_TIME_IN_SECS + 0.25;
            BossBodyPart* leftBodyPart = this->boss->GetLeftBodyEditable();
            assert(!leftBodyPart->GetIsDestroyed());
            leftBodyPart->AnimateColourRGBA(Boss::BuildBossHurtFlashAndFadeAnim(TOTAL_BLOWUP_TIME));
            leftBodyPart->AnimateLocalTranslation(this->GenerateSideBodyPartDeathTranslationAnimation(true, TOTAL_BLOWUP_TIME));
            leftBodyPart->AnimateLocalZRotation(this->GenerateSideBodyPartDeathRotationAnimation(true, TOTAL_BLOWUP_TIME));
            this->boss->ConvertAliveBodyPartToDeadBodyPart(leftBodyPart);

            BossBodyPart* rightBodyPart = this->boss->GetRightBodyEditable();
            assert(!rightBodyPart->GetIsDestroyed());
            rightBodyPart->AnimateColourRGBA(Boss::BuildBossHurtFlashAndFadeAnim(TOTAL_BLOWUP_TIME));
            rightBodyPart->AnimateLocalTranslation(this->GenerateSideBodyPartDeathTranslationAnimation(false, TOTAL_BLOWUP_TIME));
            rightBodyPart->AnimateLocalZRotation(this->GenerateSideBodyPartDeathRotationAnimation(false, TOTAL_BLOWUP_TIME));
            this->boss->ConvertAliveBodyPartToDeadBodyPart(rightBodyPart);
            
            break;
        }

        case AngryAIState:
            this->InitAngryState();
            break;

        default:
            assert(false);
            return;
    }

    this->currState = newState;
}

void Stage1AI::GoToNextRandomAttackState(GameModel* gameModel) {

    DecoBossAIState::AIState nextAIState = this->currState;

    // If the paddle has a shield active but doesn't have a loaded rc rocket,
    // then we try to stick to attack states that involve shooting reflect-able projectiles
    PlayerPaddle* paddle = gameModel->GetPlayerPaddle();
    assert(paddle != NULL);
    if (paddle->HasPaddleType(PlayerPaddle::ShieldPaddle) && !paddle->HasPaddleType(PlayerPaddle::RemoteControlRocketPaddle)) {
        switch (Randomizer::GetInstance()->RandomUnsignedInt() % 5) {
            case 0:
                nextAIState = StationaryAttackAIState;
                break;
            case 1: case 2: case 3: default:
                nextAIState = MovingAttackAIState;
                break;
            case 4:
                nextAIState = MovingAttackAndItemDropAIState;
                break;
        }

        this->SetState(nextAIState);
        return;
    }

    switch (this->currState) {

        case StationaryAttackAIState:
            switch (Randomizer::GetInstance()->RandomUnsignedInt() % 6) {
                case 0: case 1: default:
                    nextAIState = MovingAttackAIState;
                    break;
                case 2: case 3:
                    nextAIState = MovingAttackAndItemDropAIState;
                    break;
                case 4:
                    nextAIState = MoveToFarLeftSideAIState;
                    break;
                case 5:
                    nextAIState = MoveToFarRightSideAIState;
                    break;
            }
            break;

        default:
            // All other states are erroneous -- we shouldn't be trying to attack if we're in them!
            assert(false);

        case MovingAttackAIState:
        case MovingAttackAndItemDropAIState:
            switch (Randomizer::GetInstance()->RandomUnsignedInt() % 9) {
                case 0: case 1:
                    nextAIState = StationaryAttackAIState;
                    break;
                case 2: case 3: case 4: default:
                    nextAIState = MovingAttackAIState;
                    break;
                case 5: case 6:
                    nextAIState = MovingAttackAndItemDropAIState;
                    break;
                case 7:
                    nextAIState = MoveToFarLeftSideAIState;
                    break;
                case 8:
                    nextAIState = MoveToFarRightSideAIState;
                    break;
            }
            break;

        case LeftToRightItemDropAIState:
        case RightToLeftItemDropAIState:
            switch (Randomizer::GetInstance()->RandomUnsignedInt() % 7) {
                case 0: case 1: case 2: default:
                    nextAIState = MovingAttackAIState;
                    break;
                case 3: case 4: case 5:
                    nextAIState = MovingAttackAndItemDropAIState;
                    break;
                case 6:
                    nextAIState = StationaryAttackAIState;
                    break;
            }
            break;
    }

    this->SetState(nextAIState);
}

AnimationMultiLerp<Vector3D> Stage1AI::GenerateSideBodyPartDeathTranslationAnimation(bool isLeft, double timeInSecs) const {
    float xDist = (isLeft ? -1 : 1) * 2.5f * DecoBoss::SIDE_BODY_PART_WIDTH;

    float yDist = -(isLeft ? this->boss->GetLeftBody()->GetTranslationPt2D()[1] : 
        this->boss->GetRightBody()->GetTranslationPt2D()[1]);
    yDist -= 1.5f * DecoBoss::SIDE_BODY_PART_HEIGHT;

    return Boss::BuildLimbFallOffTranslationAnim(timeInSecs, xDist, yDist);
}

AnimationMultiLerp<float> Stage1AI::GenerateSideBodyPartDeathRotationAnimation(bool isLeft, double timeInSecs) const {
    float randomAngle = Randomizer::GetInstance()->RandomUnsignedInt() % 51;
    return Boss::BuildLimbFallOffZRotationAnim(timeInSecs, isLeft ? (140.0f + randomAngle) : -(140.0f + randomAngle));
}

// Stage2AI Functions **********************************************************************************
const float Stage2AI::SPEED_COEFF = 1.05f;

Stage2AI::Stage2AI(DecoBoss* boss) : DecoBossAIState(boss), stayAliveArm(NULL), becomeDeadArm(NULL) {
    
    // Choose an arm that will stay alive after this state and the other that won't
    BossCompositeBodyPart* leftArm  = boss->GetLeftArmEditable();
    BossCompositeBodyPart* rightArm = boss->GetRightArmEditable();
    if (Randomizer::GetInstance()->RandomTrueOrFalse()) {
        this->stayAliveArm  = leftArm;
        this->becomeDeadArm = rightArm;
    }
    else {
        this->stayAliveArm  = rightArm;
        this->becomeDeadArm = leftArm;
    }

    this->SetState(MoveToCenterForLevelRotAIState);
}

Stage2AI::~Stage2AI() {
}

GameItem::ItemType Stage2AI::GenerateRandomItemDropType(GameModel* gameModel) const {
    assert(gameModel != NULL);
    if (this->nextDropItemType != GameItem::PaddleShrinkItem &&
        gameModel->GetPlayerPaddle()->GetPaddleSize() != PlayerPaddle::SmallestSize) {

        if (Randomizer::GetInstance()->RandomUnsignedInt() % 5 == 0) {
            return GameItem::PaddleShrinkItem;
        }
    }

    switch (Randomizer::GetInstance()->RandomUnsignedInt() % 8) {
        case 0: case 1: case 2: default:
            if (gameModel->GetPlayerPaddle()->GetPaddleSize() != PlayerPaddle::SmallestSize) {
                return GameItem::PaddleShrinkItem;
            }
            else {
                switch (Randomizer::GetInstance()->RandomUnsignedInt() % 5) {
                    case 0:
                        return GameItem::InvisiPaddleItem;
                    case 1:
                        return GameItem::InvisiBallItem;
                    case 2:
                        return GameItem::PoisonPaddleItem;
                    case 3:
                        return GameItem::UpsideDownItem;
                    case 4:
                    default:
                        return GameItem::BallSpeedUpItem;
                }
            }
        case 3:
            return GameItem::InvisiPaddleItem;
        case 4:
            return GameItem::InvisiBallItem;
        case 5:
            return GameItem::PoisonPaddleItem;
        case 6:
            return GameItem::UpsideDownItem;
        case 7:
            return GameItem::BallSpeedUpItem;
    }
}

DecoBossAIState* Stage2AI::BuildNextAIState() const {

    return new Stage3AI(this->boss, this->stayAliveArm, this->currLevelRotationAmtInDegs,
        (this->stayAliveArm == this->boss->GetLeftArm()) ? this->currLeftArmRotInDegs : this->currRightArmRotInDegs);
}

void Stage2AI::SetState(DecoBossAIState::AIState newState) {
    switch (newState) {
        case StationaryAttackAIState:
            this->InitStationaryAttackState();
            break;
        case MovingAttackAIState:
            this->InitMovingAttackState();
            break;
        case MovingAttackAndItemDropAIState:
            this->InitMovingAttackAndItemDropState();
            break;

        case MoveToFarLeftSideAIState:
            this->InitMoveToFarLeftSideState();
            break;
        case MoveToFarRightSideAIState:
            this->InitMoveToFarRightSideState();
            break;
        case LeftToRightItemDropAIState:
            this->InitLeftToRightItemDropState();
            break;
        case RightToLeftItemDropAIState:
            this->InitRightToLeftItemDropState();
            break;

        case MoveToCenterForLevelRotAIState:
            this->InitMoveToCenterForLevelRotState();
            break;
        case FiringArmsAtLevelAIState:
            this->InitFiringArmsAtLevelState();
            break;
        case RotatingLevelAIState:
            this->InitRotatingLevelState();
            break;
        case FinishRotatingLevelAIState:
            this->InitFinishRotatingLevelState();
            break;

        case MoveToPaddleArmAttackPosAIState:
            this->InitMoveToPaddleArmAttackPosState();
            break;
        case FiringArmsAtPaddleAIState:
            this->InitFiringArmsAtPaddleState();
            break;
        case FinishedFiringArmsAtPaddleAIState:
            this->InitFinishedFiringArmsAtPaddleState();
            break;

        case ElectrifiedAIState: 
            this->InitElectrifiedState();
            break;
        case ElectrificationRetaliationAIState: {
            this->InitElectrificationRetaliationState();

            // Blow off one of the arms...
            static const float TOTAL_BLOWUP_TIME = DecoBossAIState::TOTAL_RETALIATION_TIME_IN_SECS + 0.25;
            bool deathToLeftArm = (this->becomeDeadArm == this->boss->GetLeftArm());
            assert(!this->becomeDeadArm->GetIsDestroyed());
            this->becomeDeadArm->AnimateColourRGBA(Boss::BuildBossHurtFlashAndFadeAnim(TOTAL_BLOWUP_TIME));
            this->becomeDeadArm->AnimateLocalTranslation(this->GenerateArmDeathTranslationAnimation(deathToLeftArm, TOTAL_BLOWUP_TIME));
            this->becomeDeadArm->AnimateLocalZRotation(this->GenerateArmDeathRotationAnimation(deathToLeftArm, TOTAL_BLOWUP_TIME));
            this->boss->ConvertAliveBodyPartToDeadBodyPart(this->becomeDeadArm);
            
            GameSound* sound = this->boss->GetGameModel()->GetSound();
            sound->DetachAndStopSound(this->becomeDeadArm, GameSound::DecoBossArmRotateLoop);
            sound->DetachAndStopSound(this->becomeDeadArm, GameSound::DecoBossArmExtendEvent);

            break;
        }

        case AngryAIState:
            this->InitAngryState();
            break;

        default:
            assert(false);
            return;
    }

    this->currState = newState;
}

void Stage2AI::GoToNextRandomAttackState(GameModel* gameModel) {

    DecoBossAIState::AIState nextAIState = this->currState;

    // If the paddle has a shield active but doesn't have a loaded rc rocket,
    // then we try to stick to attack states that involve shooting reflect-able projectiles...
    // Or to a state that will cause the boss to attack the paddle with its arms, thereby canceling
    // the shield effect
    PlayerPaddle* paddle = gameModel->GetPlayerPaddle();
    assert(paddle != NULL);
    if (paddle->HasPaddleType(PlayerPaddle::ShieldPaddle) && 
        !paddle->HasPaddleType(PlayerPaddle::RemoteControlRocketPaddle)) {

        switch (Randomizer::GetInstance()->RandomUnsignedInt() % 8) {
            case 0:
                nextAIState = StationaryAttackAIState;
                break;
            case 1: case 2: 
                nextAIState = MovingAttackAIState;
                break;
            case 3: case 4: 
                nextAIState = MovingAttackAndItemDropAIState;
                break;
            case 5: case 6: case 7: default:
                nextAIState = MoveToPaddleArmAttackPosAIState;
                break;
        }

        this->SetState(nextAIState);
        return;
    }

    // If the remote control rocket is active in any capacity then the boss will almost always
    // rotate the level to make it harder for the player to activate the switches
    if (paddle->HasPaddleType(PlayerPaddle::RemoteControlRocketPaddle) &&
        this->currState != FinishRotatingLevelAIState) {

        // Check to see if the ball is in the lower part of the level
        const GameBall* ball = gameModel->GetGameBalls().front();
        assert(ball != NULL);
        if (ball->GetCenterPosition2D()[1] <= this->boss->GetYBallMaxForLevelRotate()) {
            this->SetState(MoveToCenterForLevelRotAIState);
            return; 
        }

    }
    if (gameModel->GetTransformInfo()->GetIsRemoteControlRocketCameraOn()) {
        this->SetState(MoveToCenterForLevelRotAIState);
        return; 
    }

    switch (this->currState) {

        case StationaryAttackAIState:
            switch (Randomizer::GetInstance()->RandomUnsignedInt() % 8) {
                case 0: 
                    nextAIState = MovingAttackAIState;
                    break;
                case 1: case 2: default: 
                    nextAIState = MovingAttackAndItemDropAIState;
                    break;
                case 3:
                    nextAIState = MoveToFarLeftSideAIState;
                    break;
                case 4:
                    nextAIState = MoveToFarRightSideAIState;
                    break;
                case 5: case 6: case 7:
                    nextAIState = MoveToPaddleArmAttackPosAIState;
                    break;
            }
            break;

        case MoveToCenterForLevelRotAIState:
        case MovingAttackAIState:
        case MovingAttackAndItemDropAIState:
        case FinishRotatingLevelAIState:
            switch (Randomizer::GetInstance()->RandomUnsignedInt() % 12) {
                case 0: 
                    nextAIState = StationaryAttackAIState;
                    break;
                case 1: case 2: case 3:
                    nextAIState = MovingAttackAIState;
                    break;
                case 4: case 5: case 6: default:
                    nextAIState = MovingAttackAndItemDropAIState;
                    break;
                case 7:
                    nextAIState = MoveToFarLeftSideAIState;
                    break;
                case 8:
                    nextAIState = MoveToFarRightSideAIState;
                    break;
                case 9: case 10: case 11:
                    nextAIState = MoveToPaddleArmAttackPosAIState;
                    break;
             }
             break;

        case LeftToRightItemDropAIState:
        case RightToLeftItemDropAIState:
            switch (Randomizer::GetInstance()->RandomUnsignedInt() % 10) {
                case 0: case 1: case 2: case 3: default:
                    nextAIState = MovingAttackAIState;
                    break;
                 case 4: case 5:
                    nextAIState = MovingAttackAndItemDropAIState;
                    break;
                case 6:
                    nextAIState = StationaryAttackAIState;
                    break;
                case 7: case 8: case 9:
                    nextAIState = MoveToPaddleArmAttackPosAIState;
                    break;
            }
            break;
        
        default:
            // All other states are erroneous -- we shouldn't be trying to attack if we're in them!        
            assert(false);

        case FinishedFiringArmsAtPaddleAIState:
            switch (Randomizer::GetInstance()->RandomUnsignedInt() % 11) {
                case 0: case 10:
                    nextAIState = StationaryAttackAIState;
                    break;
                case 1: case 2: case 3:
                    nextAIState = MovingAttackAIState;
                    break;
                case 4: case 5: case 6: default:
                    nextAIState = MovingAttackAndItemDropAIState;
                    break;
                case 7:
                    nextAIState = MoveToFarLeftSideAIState;
                    break;
                case 8:
                    nextAIState = MoveToFarRightSideAIState;
                    break;
                case 9:
                    nextAIState = MoveToPaddleArmAttackPosAIState;
                    break;
            }
            break;
    }

    this->SetState(nextAIState);
}

// Stage3AI Functions **********************************************************************

const float Stage3AI::SPEED_COEFF = 1.1f;

Stage3AI::Stage3AI(DecoBoss* boss, BossCompositeBodyPart* remainingArm, 
                   float currLevelRotationAmtInDegs, float remainingArmRot) : 
    DecoBossAIState(boss), remainingArm(remainingArm) {

    assert(remainingArm != NULL);

    if (remainingArm == boss->GetLeftArm()) {
        this->currLeftArmRotInDegs = remainingArmRot;
    }
    else {
        this->currRightArmRotInDegs = remainingArmRot;
    }

    this->currLevelRotationAmtInDegs = currLevelRotationAmtInDegs;
    this->SetState(MoveToCenterForLevelRotAIState);
}

Stage3AI::~Stage3AI() {
}

GameItem::ItemType Stage3AI::GenerateRandomItemDropType(GameModel* gameModel) const {
    assert(gameModel != NULL);
    if (this->nextDropItemType != GameItem::PaddleShrinkItem &&
        gameModel->GetPlayerPaddle()->GetPaddleSize() != PlayerPaddle::SmallestSize) {

            if (Randomizer::GetInstance()->RandomUnsignedInt() % 4 == 0) {
                return GameItem::PaddleShrinkItem;
            }
    }

    switch (Randomizer::GetInstance()->RandomUnsignedInt() % 12) {
        case 0: case 1: case 2: case 3: case 4: default:
            if (gameModel->GetPlayerPaddle()->GetPaddleSize() != PlayerPaddle::SmallestSize) {
                return GameItem::PaddleShrinkItem;
            }
            else {
                switch (Randomizer::GetInstance()->RandomUnsignedInt() % 7) {
                    case 0:
                        return GameItem::InvisiBallItem;
                    case 1:
                        return GameItem::PoisonPaddleItem;
                    case 2:
                        return GameItem::UpsideDownItem;
                    case 3:
                        return GameItem::BallSpeedUpItem;
                    case 4:
                        return GameItem::CrazyBallItem;
                    case 5:
                        return GameItem::GravityBallItem;
                    case 6:
                    default:
                        return GameItem::InvisiPaddleItem;
                }
            }
            
        case 5:
            return GameItem::InvisiBallItem;
        case 6:
            return GameItem::PoisonPaddleItem;
        case 7:
            return GameItem::UpsideDownItem;
        case 8:
            return GameItem::BallSpeedUpItem;
        case 9:
            return GameItem::CrazyBallItem;
        case 10:
            return GameItem::GravityBallItem;
        case 11:
            return GameItem::InvisiPaddleItem;
    }
}

void Stage3AI::SetState(DecoBossAIState::AIState newState) {
    switch (newState) {
        case StationaryAttackAIState:
            this->InitStationaryAttackState();
            break;
        case MovingAttackAIState:
            this->InitMovingAttackState();
            break;
        case MovingAttackAndItemDropAIState:
            this->InitMovingAttackAndItemDropState();
            break;

        case MoveToFarLeftSideAIState:
            this->InitMoveToFarLeftSideState();
            break;
        case MoveToFarRightSideAIState:
            this->InitMoveToFarRightSideState();
            break;
        case LeftToRightItemDropAIState:
            this->InitLeftToRightItemDropState();
            break;
        case RightToLeftItemDropAIState:
            this->InitRightToLeftItemDropState();
            break;

        case MoveToCenterForLevelRotAIState:
            this->InitMoveToCenterForLevelRotState();
            break;
        case FiringArmsAtLevelAIState:
            this->InitFiringArmsAtLevelState();
            break;
        case RotatingLevelAIState:
            this->InitRotatingLevelState();
            break;
        case FinishRotatingLevelAIState:
            this->InitFinishRotatingLevelState();
            break;

        case MoveToPaddleArmAttackPosAIState:
            this->InitMoveToPaddleArmAttackPosState();
            break;
        case FiringArmsAtPaddleAIState:
            this->InitFiringArmsAtPaddleState();
            break;
        case FinishedFiringArmsAtPaddleAIState:
            this->InitFinishedFiringArmsAtPaddleState();
            break;

        case ElectrifiedAIState: 
            this->InitElectrifiedState();
            break;

        case FinalDeathThroesAIState: {
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);

            // Final fade out for what's left of the boss
            this->boss->alivePartsRoot->AnimateColourRGBA(Boss::BuildBossFinalDeathFlashAnim());

            // Blow off the last remaining arm of the boss
            bool deathToLeftArm = (this->remainingArm == this->boss->GetLeftArm());
            assert(!this->remainingArm->GetIsDestroyed());
            this->remainingArm->AnimateColourRGBA(Boss::BuildBossHurtFlashAndFadeAnim(Boss::TOTAL_DEATH_ANIM_TIME));
            this->remainingArm->AnimateLocalTranslation(this->GenerateArmDeathTranslationAnimation(
                deathToLeftArm, Boss::TOTAL_DEATH_ANIM_TIME));
            this->remainingArm->AnimateLocalZRotation(this->GenerateArmDeathRotationAnimation(
                deathToLeftArm, Boss::TOTAL_DEATH_ANIM_TIME));
            this->boss->ConvertAliveBodyPartToDeadBodyPart(this->remainingArm);

            GameSound* sound = this->boss->GetGameModel()->GetSound();
            sound->DetachAndStopSound(this->remainingArm, GameSound::DecoBossArmRotateLoop);
            sound->DetachAndStopSound(this->remainingArm, GameSound::DecoBossArmExtendEvent);

            break;
        }

        default:
            assert(false);
            return;
    }

    this->currState = newState;
}

void Stage3AI::GoToNextRandomAttackState(GameModel* gameModel) {
    DecoBossAIState::AIState nextAIState = this->currState;

    // If the paddle has a shield active but doesn't have a loaded rc rocket,
    // then we try to stick to attack states that involve shooting reflect-able projectiles...
    // Or to a state that will cause the boss to attack the paddle with its arms, thereby canceling
    // the shield effect
    PlayerPaddle* paddle = gameModel->GetPlayerPaddle();
    assert(paddle != NULL);
    if (paddle->HasPaddleType(PlayerPaddle::ShieldPaddle) && 
        !paddle->HasPaddleType(PlayerPaddle::RemoteControlRocketPaddle)) {

            switch (Randomizer::GetInstance()->RandomUnsignedInt() % 11) {
                case 0:
                    nextAIState = StationaryAttackAIState;
                    break;
                case 1: case 2: 
                    nextAIState = MovingAttackAIState;
                    break;
                case 3: case 4: case 5:
                    nextAIState = MovingAttackAndItemDropAIState;
                    break;
                case 6: case 7: case 8: case 9: case 10: default:
                    nextAIState = MoveToPaddleArmAttackPosAIState;
                    break;
            }

            this->SetState(nextAIState);
            return;
    }

    // If the remote control rocket is active in any capacity then the boss will almost always
    // rotate the level to make it harder for the player to activate the switches
    if (gameModel->GetTransformInfo()->GetIsRemoteControlRocketCameraOn() ||
        (paddle->HasPaddleType(PlayerPaddle::RemoteControlRocketPaddle) && this->currState != FinishRotatingLevelAIState)) {
        this->SetState(MoveToCenterForLevelRotAIState);
        return; 
    }

    switch (this->currState) {
        case StationaryAttackAIState:
            switch (Randomizer::GetInstance()->RandomUnsignedInt() % 9) {
                case 0: 
                    nextAIState = MovingAttackAIState;
                    break;
                case 1: case 2: default: 
                    nextAIState = MovingAttackAndItemDropAIState;
                    break;
                case 3:
                    nextAIState = MoveToFarLeftSideAIState;
                    break;
                case 4:
                    nextAIState = MoveToFarRightSideAIState;
                    break;
                case 5: case 6: case 7:
                    nextAIState = MoveToPaddleArmAttackPosAIState;
                    break;
                case 8:
                    nextAIState = MoveToCenterForLevelRotAIState;
                    break;
            }
            break;

        case MoveToCenterForLevelRotAIState:
        case FinishRotatingLevelAIState:
            switch (Randomizer::GetInstance()->RandomUnsignedInt() % 9) {
                case 0: 
                    nextAIState = MovingAttackAIState;
                    break;
                case 1: case 2: default: 
                    nextAIState = MovingAttackAndItemDropAIState;
                    break;
                case 3:
                    nextAIState = MoveToFarLeftSideAIState;
                    break;
                case 4:
                    nextAIState = MoveToFarRightSideAIState;
                    break;
                case 5: case 6: case 7:
                    nextAIState = MoveToPaddleArmAttackPosAIState;
                    break;
                case 8: 
                    nextAIState = StationaryAttackAIState;
                    break;
            }
            break;

        
        case MovingAttackAIState:
        case MovingAttackAndItemDropAIState:
            switch (Randomizer::GetInstance()->RandomUnsignedInt() % 12) {
                case 0: 
                    nextAIState = StationaryAttackAIState;
                    break;
                case 1: case 2: case 3:
                    nextAIState = MovingAttackAIState;
                    break;
                case 4: case 5: case 6: default:
                    nextAIState = MovingAttackAndItemDropAIState;
                    break;
                case 7:
                    nextAIState = MoveToFarLeftSideAIState;
                    break;
                case 8:
                    nextAIState = MoveToFarRightSideAIState;
                    break;
                case 9: case 10: case 11:
                    nextAIState = MoveToPaddleArmAttackPosAIState;
                    break;
            }
            break;

        case LeftToRightItemDropAIState:
        case RightToLeftItemDropAIState:
            switch (Randomizer::GetInstance()->RandomUnsignedInt() % 10) {
                case 0: case 1: case 2: case 3: default:
                    nextAIState = MovingAttackAIState;
                    break;
                case 4: case 5:
                    nextAIState = MovingAttackAndItemDropAIState;
                    break;
                case 6:
                    nextAIState = StationaryAttackAIState;
                    break;
                case 7: case 8: case 9:
                    nextAIState = MoveToPaddleArmAttackPosAIState;
                    break;
            }
            break;

        default:
            // All other states are erroneous -- we shouldn't be trying to attack if we're in them!        
            assert(false);

        case FinishedFiringArmsAtPaddleAIState:
            switch (Randomizer::GetInstance()->RandomUnsignedInt() % 11) {
                case 0: case 10:
                    nextAIState = StationaryAttackAIState;
                    break;
                case 1: case 2: case 3:
                    nextAIState = MovingAttackAIState;
                    break;
                case 4: case 5: case 6: default:
                    nextAIState = MovingAttackAndItemDropAIState;
                    break;
                case 7:
                    nextAIState = MoveToFarLeftSideAIState;
                    break;
                case 8:
                    nextAIState = MoveToFarRightSideAIState;
                    break;
                case 9:
                    nextAIState = MoveToPaddleArmAttackPosAIState;
                    break;
            }
            break;
    }

    this->SetState(nextAIState);
}
