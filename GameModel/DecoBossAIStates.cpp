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

using namespace decobossai;

const float DecoBossAIState::DEFAULT_ACCELERATION = 0.25f * PlayerPaddle::DEFAULT_ACCELERATION;
const float DecoBossAIState::DEFAULT_MAX_X_SPEED = 8.0f;
const float DecoBossAIState::DEFAULT_MAX_Y_SPEED = 14.0f;
const float DecoBossAIState::SHOOT_AT_PADDLE_RANDOM_ROT_ANGLE_IN_DEGS = 13.25f;

const double DecoBossAIState::TOTAL_ELECTRIFIED_TIME_IN_SECS = 2.5;
const double DecoBossAIState::TOTAL_RETALIATION_TIME_IN_SECS = 2.0;
const double DecoBossAIState::TOTAL_ELECTRIFIED_AND_RETALIATION_TIME_IN_SECS = 
    TOTAL_ELECTRIFIED_TIME_IN_SECS + TOTAL_RETALIATION_TIME_IN_SECS;

DecoBossAIState::DecoBossAIState(DecoBoss* boss) : BossAIState(), boss(boss), shootCountdown(0.0),
numShotsUntilNextState(0), dropItemCountdown(0.0), nextDropItemType(GameItem::PaddleShrinkItem) {
    assert(boss != NULL);
    this->angryMoveAnim = Boss::BuildBossAngryShakeAnim(1.0f);
    this->itemLoadingAnim.SetLerp(0.0, 0.8, 0.0, DecoBoss::GetItemDropAnimDisplacement());
    this->itemLoadingAnim.SetRepeat(false);
    this->itemLoadingAnim.SetInterpolantValue(0.0);
}

DecoBossAIState::~DecoBossAIState() {
}

Boss* DecoBossAIState::GetBoss() const {
    return this->boss;
}

Collision::AABB2D DecoBossAIState::GenerateDyingAABB() const {
    return this->boss->GetCore()->GenerateWorldAABB();
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
        std::max<float>(BossLightningBoltProjectile::SPD_DEFAULT, spdMultiplier * this->GetMaxYSpeed())));
}

bool DecoBossAIState::RemoteControlRocketCheckAndNecessaryStateChange(GameModel* gameModel) {
    
    if (!gameModel->GetTransformInfo()->GetIsRemoteControlRocketCameraOn()) {
        return false;
    }

    // Check to see if the player is currently using the remote control rocket, and the boss has the ability to rotate the level
    // then we leave this state immediately...
    if (!this->boss->IsRightBodyStillAlive() || !this->boss->IsLeftBodyStillAlive()) {
        this->SetState(FiringArmsAtLevelAIState);
        return true;
    }
    else {
        // The player is controlling the RC rocket... the boss can't really do much since it hasn't had its arms exposed yet,
        // so if it isn't doing anything important, just move it around and make it look like it's doing stuff
        if (this->currState == StationaryAttackAIState) {
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
            this->SetState(MovingAttackAndItemDropAIState);
            //this->GoToNextRandomAttackState();
            return true;
        }
    }
    else {
        this->shootCountdown -= dT;
    }

    return false;
}

void DecoBossAIState::DropLoadedItem(GameModel* gameModel) {
    assert(gameModel != NULL);

    gameModel->AddItemDrop(this->boss->GetItemDropPosition(), this->nextDropItemType);
    this->nextDropItemType = this->GenerateRandomItemDropType(gameModel);
    this->itemLoadingAnim.ResetToStart();
}

void DecoBossAIState::ExecuteStationaryAttackState(double dT, GameModel* gameModel) {
    
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
            this->GoToNextRandomAttackState();
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

void DecoBossAIState::ExecuteElectrifiedState(double dT, GameModel* gameModel) {
    UNUSED_PARAMETER(gameModel);

    this->currVel = Vector2D(0,0);
    this->desiredVel = Vector2D(0,0);

    bool isFinished = this->electrifiedHurtAnim.Tick(dT);
    this->boss->alivePartsRoot->SetLocalTranslation(this->electrifiedHurtAnim.GetInterpolantValue());
    if (this->electrifiedHurtAnim.GetCurrentTimeValue() >= TOTAL_ELECTRIFIED_TIME_IN_SECS || isFinished) {

        if (this->IsFinalAIStage() && isFinished) {

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

        if (leftTeslaBlock->GetIsElectricityActive()) {
            const Point2D& center = leftTeslaBlock->GetCenter();
            leftTeslaBlock->ToggleElectricity(*gameModel, *currLevel, true);
            GameEventManager::Instance()->ActionGeneralEffect(ShortCircuitEffectInfo(
                center, GameModelConstants::GetInstance()->TESLA_LIGHTNING_BRIGHT_COLOUR, 
                GameModelConstants::GetInstance()->TESLA_LIGHTNING_MEDIUM_COLOUR, 
                GameModelConstants::GetInstance()->TESLA_LIGHTNING_DARK_COLOUR,
                LevelPiece::PIECE_WIDTH, 2.0));
        }
        if (rightTeslaBlock->GetIsElectricityActive()) {
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

// Stage1AI Functions ********************************************************************

Stage1AI::Stage1AI(DecoBoss* boss) : DecoBossAIState(boss) {
    // Boss starts off by attacking
    this->SetState(MovingAttackAndItemDropAIState);
}

Stage1AI::~Stage1AI() {
}

GameItem::ItemType Stage1AI::GenerateRandomItemDropType(GameModel* gameModel) const {
    assert(gameModel != NULL);
    if (this->nextDropItemType != GameItem::PaddleShrinkItem &&
        gameModel->GetPlayerPaddle()->GetPaddleSize() != PlayerPaddle::SmallestSize) {
        if (Randomizer::GetInstance()->RandomUnsignedInt() % 3 == 0) {
            return GameItem::PaddleShrinkItem;
        }
    }

    switch (Randomizer::GetInstance()->RandomUnsignedInt() % 6) {
        case 0: case 1: case 2: default:
            if (gameModel->GetPlayerPaddle()->GetPaddleSize() != PlayerPaddle::SmallestSize) {
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
        case 3:
            return GameItem::BallSpeedUpItem;
        case 4:
            return GameItem::InvisiPaddleItem;
        case 5:
            return GameItem::InvisiBallItem;
    }
}

DecoBossAIState* Stage1AI::BuildNextAIState() const {
    return new Stage2AI(this->boss);
}

void Stage1AI::SetState(DecoBossAIState::AIState newState) {
    switch (newState) {
        case StationaryAttackAIState:
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);

            this->shootCountdown         = this->GenerateShootCountdownWhileStationary();
            this->numShotsUntilNextState = this->GenerateNumShotsWhileStationary();

            assert(this->shootCountdown > 0.0);
            assert(this->numShotsUntilNextState > 0);

            break;
        
        case MovingAttackAIState:
            this->shootCountdown         = this->GenerateShootCountdownWhileMoving();
            this->numShotsUntilNextState = this->GenerateNumShotsWhileMoving();

            assert(this->shootCountdown > 0.0);
            assert(this->numShotsUntilNextState > 0);

            break;

        case MovingAttackAndItemDropAIState:
            this->dropItemCountdown      = this->GenerateItemDropCountdown();
            this->shootCountdown         = this->GenerateShootCountdownWhileMoving();
            this->numShotsUntilNextState = this->GenerateNumShotsWhileMoving();

            assert(this->dropItemCountdown > 0.0);
            assert(this->shootCountdown > 0.0);
            assert(this->numShotsUntilNextState > 0);

            this->itemLoadingAnim.ResetToStart();

            break;

        case MoveToFarLeftSideAIState:
            break;

        case MoveToFarRightSideAIState:
            break;

        case LeftToRightItemDropAIState:
            this->itemLoadingAnim.ResetToStart();
            
            break;

        case RightToLeftItemDropAIState:
            this->itemLoadingAnim.ResetToStart();

            break;

        case ElectrifiedAIState: {
            this->itemLoadingAnim.ResetToStart();
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);

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

            break;
        }

        case ElectrificationRetaliationAIState: {
            this->itemLoadingAnim.ResetToStart();
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);

            // Execute the retaliatory strike to turn off the Tesla blocks...
            GameEventManager::Instance()->ActionBossEffect(ShockwaveEffectInfo(
                this->boss->alivePartsRoot->GetTranslationPt2D(), DecoBoss::CORE_HEIGHT, 
                DecoBossAIState::TOTAL_RETALIATION_TIME_IN_SECS));

            // Blow off the left and right body parts of the boss...
            BossBodyPart* leftBodyPart = this->boss->GetLeftBodyEditable();
            assert(!leftBodyPart->GetIsDestroyed());
            leftBodyPart->AnimateColourRGBA(Boss::BuildBossHurtFlashAndFadeAnim(DecoBossAIState::TOTAL_RETALIATION_TIME_IN_SECS));
            leftBodyPart->AnimateLocalTranslation(this->GenerateSideBodyPartDeathTranslationAnimation(
                true, DecoBossAIState::TOTAL_RETALIATION_TIME_IN_SECS));
            leftBodyPart->AnimateLocalZRotation(this->GenerateSideBodyPartDeathRotationAnimation(
                true, DecoBossAIState::TOTAL_RETALIATION_TIME_IN_SECS));
            leftBodyPart->SetDestroyed(true);

            BossBodyPart* rightBodyPart = this->boss->GetRightBodyEditable();
            assert(!rightBodyPart->GetIsDestroyed());
            rightBodyPart->AnimateColourRGBA(Boss::BuildBossHurtFlashAndFadeAnim(DecoBossAIState::TOTAL_RETALIATION_TIME_IN_SECS));
            rightBodyPart->AnimateLocalTranslation(this->GenerateSideBodyPartDeathTranslationAnimation(
                false, DecoBossAIState::TOTAL_RETALIATION_TIME_IN_SECS));
            rightBodyPart->AnimateLocalZRotation(this->GenerateSideBodyPartDeathRotationAnimation(
                false, DecoBossAIState::TOTAL_RETALIATION_TIME_IN_SECS));
            rightBodyPart->SetDestroyed(true);

            break;
        }

        case AngryAIState:
            this->itemLoadingAnim.ResetToStart();
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->boss->alivePartsRoot->AnimateColourRGBA(Boss::BuildBossAngryFlashAnim());
            this->angryMoveAnim.ResetToStart();

            // EVENT: Boss is angry! Rawr.
            GameEventManager::Instance()->ActionBossAngry(this->boss, this->boss->GetCore());
            break;

        default:
            assert(false);
            return;
    }

    this->currState = newState;
}

void Stage1AI::UpdateState(double dT, GameModel* gameModel) {
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
            break;

        case MoveToFarRightSideAIState:
            break;

        case LeftToRightItemDropAIState:
            break;

        case RightToLeftItemDropAIState:
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

        default:
            assert(false);
            return;
    }
}

void Stage1AI::GoToNextRandomAttackState() {

    DecoBossAIState::AIState nextAIState = this->currState;

    switch (this->currState) {

        case StationaryAttackAIState:
            break;
        case MovingAttackAIState:
            break;
        case MovingAttackAndItemDropAIState:
            break;
        case MoveToFarLeftSideAIState:
            break;
        case MoveToFarRightSideAIState:
            break;
        case LeftToRightItemDropAIState:
            break;
        case RightToLeftItemDropAIState:
            break;

        case ElectrifiedAIState:
        case ElectrificationRetaliationAIState:
        case AngryAIState:
        default:
            assert(false);
            return;
    }

    this->SetState(nextAIState);
}

AnimationMultiLerp<Vector3D> Stage1AI::GenerateSideBodyPartDeathTranslationAnimation(bool isLeft, double timeInSecs) const {
    float xDist = (isLeft ? -1 : 1) * 3 * DecoBoss::SIDE_BODY_PART_WIDTH;

    float yDist = -(isLeft ? this->boss->GetLeftBody()->GetTranslationPt2D()[1] : 
        this->boss->GetRightBody()->GetTranslationPt2D()[1]);
    yDist -= 2 * DecoBoss::SIDE_BODY_PART_HEIGHT;

    return Boss::BuildLimbFallOffTranslationAnim(timeInSecs, xDist, yDist);
}

AnimationMultiLerp<float> Stage1AI::GenerateSideBodyPartDeathRotationAnimation(bool isLeft, double timeInSecs) const {
    float randomAngle = Randomizer::GetInstance()->RandomUnsignedInt() % 51;
    return Boss::BuildLimbFallOffZRotationAnim(timeInSecs, isLeft ? (100.0f + randomAngle) : -(100.0f + randomAngle));
}

// Stage2AI Functions **********************************************************************************
const float Stage2AI::SPEED_COEFF = 1.15f;

Stage2AI::Stage2AI(DecoBoss* boss) : DecoBossAIState(boss) {
    this->SetState(MovingAttackAIState);
}

Stage2AI::~Stage2AI() {
}

GameItem::ItemType Stage2AI::GenerateRandomItemDropType(GameModel* gameModel) const {
    assert(gameModel != NULL);
    if (this->nextDropItemType != GameItem::PaddleShrinkItem &&
        gameModel->GetPlayerPaddle()->GetPaddleSize() != PlayerPaddle::SmallestSize) {

        if (Randomizer::GetInstance()->RandomUnsignedInt() % 4 == 0) {
            return GameItem::PaddleShrinkItem;
        }
    }

    switch (Randomizer::GetInstance()->RandomUnsignedInt() % 9) {
        case 0: case 1: case 2: case 3: default:
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
        case 4:
            return GameItem::InvisiPaddleItem;
        case 5:
            return GameItem::InvisiBallItem;
        case 6:
            return GameItem::PoisonPaddleItem;
        case 7:
            return GameItem::UpsideDownItem;
        case 8:
            return GameItem::BallSpeedUpItem;
    }
}

DecoBossAIState* Stage2AI::BuildNextAIState() const {
    return new Stage3AI(this->boss);
}

void Stage2AI::SetState(DecoBossAIState::AIState newState) {
    
    /*
    switch (newState) {
        case 
    }
    */
}

void Stage2AI::UpdateState(double dT, GameModel* gameModel) {
    switch (this->currState) {
        case StationaryAttackAIState:
            this->ExecuteStationaryAttackState(dT, gameModel);
            break;

        case MovingAttackAIState:
            break;

        case MovingAttackAndItemDropAIState:
            break;

        case MoveToFarLeftSideAIState:
            break;

        case MoveToFarRightSideAIState:
            break;

        case LeftToRightItemDropAIState:
            break;

        case RightToLeftItemDropAIState:
            break;

        case ElectrifiedAIState:
            //this->ExecuteElectrifiedState(dT, gameModel);
            break;
        case ElectrificationRetaliationAIState:
            //this->ExecuteElectrifiedRetaliationState(dT, gameModel);
            break;
        case AngryAIState:
            //this->ExecuteAngryState(dT);
            break;

        default:
            assert(false);
            return;
    }
}

void Stage2AI::GoToNextRandomAttackState() {

}

// Stage3AI Functions **********************************************************************

const float Stage3AI::SPEED_COEFF = 1.3f;

Stage3AI::Stage3AI(DecoBoss* boss) : DecoBossAIState(boss) {
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

}

void Stage3AI::UpdateState(double dT, GameModel* gameModel) {

}

void Stage3AI::GoToNextRandomAttackState() {

}