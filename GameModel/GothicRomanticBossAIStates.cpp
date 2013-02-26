/**
 * GothicRomanticBossAIStates.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#include "GothicRomanticBossAIStates.h"
#include "GothicRomanticBoss.h"
#include "BossWeakpoint.h"
#include "GameModel.h"
#include "PlayerPaddle.h"
#include "Projectile.h"
#include "BossLaserProjectile.h"

using namespace gothicromanticbossai;

// Begin GothicRomanticBossAI *****************************************************

// Constant defining the border from confines/border blocks to the boss
const float GothicRomanticBossAI::MOVE_X_BORDER = LevelPiece::HALF_PIECE_WIDTH;
const float GothicRomanticBossAI::MOVE_Y_BORDER = LevelPiece::HALF_PIECE_HEIGHT;

const float GothicRomanticBossAI::DEFAULT_LASER_SPIN_DEGREES_PER_SEC = 360.0f;

const GothicRomanticBossAI::ConfinedMovePos GothicRomanticBossAI::CORNER_POSITIONS[] = { 
    // Make sure these are in the same order as they are in the enumeration!!!
    GothicRomanticBossAI::TopLeftCorner,
    GothicRomanticBossAI::TopRightCorner,
    GothicRomanticBossAI::BottomLeftCorner,
    GothicRomanticBossAI::BottomRightCorner
};

GothicRomanticBossAI::GothicRomanticBossAI(GothicRomanticBoss* boss) : BossAIState(), boss(boss) {
    assert(boss != NULL);
}

GothicRomanticBossAI::~GothicRomanticBossAI() {
}

Boss* GothicRomanticBossAI::GetBoss() const {
    return this->boss;
}

Collision::AABB2D GothicRomanticBossAI::GenerateDyingAABB() const {
    return this->boss->GetBody()->GenerateWorldAABB();
}

/**
 * Gets the center position of the boss for a enumerated move position within the confines
 * in the first 2 states of the battle.
 */
Point2D GothicRomanticBossAI::GetConfinedBossCenterMovePosition(float levelWidth, float levelHeight,
                                                                const ConfinedMovePos& pos) const {

    switch (pos) {
        case TopLeftCorner:
            return Point2D(
                this->boss->GetMinXOfConfines() + GothicRomanticBossAI::MOVE_X_BORDER + GothicRomanticBoss::TOTAL_WIDTH_WITH_LEGS/2.0f,
                this->boss->GetMaxYOfConfines(levelHeight) - (GothicRomanticBossAI::MOVE_Y_BORDER + GothicRomanticBoss::TOTAL_HEIGHT / 2.0f));

        case TopRightCorner:
            return Point2D(
                this->boss->GetMaxXOfConfines(levelWidth) - (GothicRomanticBossAI::MOVE_X_BORDER + GothicRomanticBoss::TOTAL_WIDTH_WITH_LEGS/2.0f),
                this->boss->GetMaxYOfConfines(levelHeight) - (GothicRomanticBossAI::MOVE_Y_BORDER + GothicRomanticBoss::TOTAL_HEIGHT / 2.0f));

        case BottomLeftCorner:
            return Point2D(
                this->boss->GetMinXOfConfines() + GothicRomanticBossAI::MOVE_X_BORDER + GothicRomanticBoss::TOTAL_WIDTH_WITH_LEGS/2.0f,
                this->boss->GetMinYOfConfines() + GothicRomanticBossAI::MOVE_Y_BORDER + GothicRomanticBoss::TOTAL_HEIGHT / 2.0f);

        case BottomRightCorner:
            return Point2D(
                this->boss->GetMaxXOfConfines(levelWidth) - (GothicRomanticBossAI::MOVE_X_BORDER + GothicRomanticBoss::TOTAL_WIDTH_WITH_LEGS/2.0f),
                this->boss->GetMinYOfConfines() + GothicRomanticBossAI::MOVE_Y_BORDER + GothicRomanticBoss::TOTAL_HEIGHT / 2.0f);

        case Center:
            return Point2D(
                this->boss->GetMinXOfConfines() + (this->boss->GetMaxXOfConfines(levelWidth)  - this->boss->GetMinXOfConfines()) / 2.0f,
                this->boss->GetMinYOfConfines() + (this->boss->GetMaxYOfConfines(levelHeight) - this->boss->GetMinYOfConfines()) / 2.0f);
            
        default:
            assert(false);
            break;
    }

    return Point2D(0,0);
}

GothicRomanticBossAI::ConfinedMovePos GothicRomanticBossAI::GetRandomConfinedCornerMovePosition(const ConfinedMovePos& currPos) {
    if (currPos == Center) {
        return GothicRomanticBossAI::CORNER_POSITIONS[Randomizer::GetInstance()->RandomUnsignedInt() % 4];
    }
    return GothicRomanticBossAI::CORNER_POSITIONS[(currPos + 1 + (Randomizer::GetInstance()->RandomUnsignedInt() % 3)) % 4];
}

/// <summary> 
/// Determine the next AI state based on the position of the boss from the set of
/// enumerated, confined movement positions. The state is also determined by a
/// given probability for executing the orb attack. </summary>
/// <param name="pos"> The confined move position to base the returned AI state on. </param>
/// <param name="probabilityOfOrb"> The base probability of an orb attack [0,1]. </param>
/// <returns> The AI state to go to next based on the given info. </returns>
GothicRomanticBossAI::AIState GothicRomanticBossAI::GetAIStateForConfinedMovePos(const GothicRomanticBossAI::ConfinedMovePos& pos,
                                                                                 float probabilityOfOrb) {
    switch (pos) {

        case TopLeftCorner:
        case TopRightCorner:
            if (Randomizer::GetInstance()->RandomNumZeroToOne() <= 1.25f * probabilityOfOrb) {
                return OrbProjectileAttackAIState;
            }
            return SpinLaserAttackAIState;
            
        case BottomLeftCorner:
        case BottomRightCorner:
            if (Randomizer::GetInstance()->RandomNumZeroToOne() <= probabilityOfOrb) {
                return OrbProjectileAttackAIState;
            }
            return SpinLaserAttackAIState;

        case Center:
            return SummonItemsAIState;

        default:
            assert(false);
            break;
    }

    return SpinLaserAttackAIState;
}

float GothicRomanticBossAI::GetAccelerationMagnitude() const {
    return GothicRomanticBoss::DEFAULT_ACCELERATION;
}

void GothicRomanticBossAI::ShootLaserFromLegPoint(int legIdx, GameModel* gameModel) const {
    assert(gameModel != NULL);
    
    Point3D spawnPos = this->boss->GetLegPointPos(legIdx);
    Point3D paddlePos(gameModel->GetPlayerPaddle()->GetCenterPosition(), 0.0f);
    Vector3D toPaddleVec = paddlePos - spawnPos;
    toPaddleVec.Normalize();

    Vector2D toPaddleVec2D(toPaddleVec[0], toPaddleVec[1]);
    toPaddleVec2D.Rotate(Randomizer::GetInstance()->RandomNumNegOneToOne() * 30.0f);

    gameModel->AddProjectile(new BossLaserProjectile(Point2D(spawnPos[0], spawnPos[1]), toPaddleVec2D));
}

void GothicRomanticBossAI::ShootLaserFromBody(GameModel* gameModel) const {
    assert(gameModel != NULL);

    Point3D spawnPos = this->boss->GetBottomPointTipPos();
    Point3D paddlePos(gameModel->GetPlayerPaddle()->GetCenterPosition(), 0.0f);
    Vector3D toPaddleVec = paddlePos - spawnPos;
    toPaddleVec.Normalize();

    Vector2D toPaddleVec2D(toPaddleVec[0], toPaddleVec[1]);
    toPaddleVec2D.Rotate(Randomizer::GetInstance()->RandomNumNegOneToOne() * 22.5f);

    gameModel->AddProjectile(new BossLaserProjectile(Point2D(spawnPos[0], spawnPos[1]), Vector2D(toPaddleVec[0], toPaddleVec[1])));
}

// End GothicRomanticBossAI *****************************************************

// Begin FireBallAI **************************************************************

// This number doesn't really matter too much, just incase we want to make it more robust later
const float FireBallAI::TOP_POINT_LIFE_POINTS = 300.0f;

FireBallAI::FireBallAI(GothicRomanticBoss* boss) : GothicRomanticBossAI(boss),
topPointWeakpt(NULL), attacksSinceLastSummon(0) {

    boss->ConvertAliveBodyPartToWeakpoint(boss->topPointIdx, FireBallAI::TOP_POINT_LIFE_POINTS, 0);
    assert(dynamic_cast<BossWeakpoint*>(boss->bodyParts[boss->topPointIdx]) != NULL);
    this->topPointWeakpt = static_cast<BossWeakpoint*>(boss->bodyParts[boss->topPointIdx]);
  
    this->laserSpinRotAnim.SetInterpolantValue(0.0f);
    this->laserSpinRotAnim.SetRepeat(false);

    this->nextMovePos    = this->GetRandomConfinedMovePosition(Center);
    this->nextAtkAIState = GothicRomanticBossAI::SpinLaserAttackAIState;
    this->SetState(GothicRomanticBossAI::BasicMoveAndShootState);

    // Set the shoot countdown a large number so the boss doesn't attack the first time it moves
    // (to accomodate the light fade-in, without cheap shots)
    this->shootCountdown = DBL_MAX;
}

FireBallAI::~FireBallAI() {
}

void FireBallAI::CollisionOccurred(GameModel* gameModel, Projectile* projectile,
                                   BossBodyPart* collisionPart) {

    // We only care if the projectile hits the weakpoint on the boss
    if (collisionPart != this->topPointWeakpt) {
        return;
    }

    // Only fire globs should be able to hit the boss' weakpoint in this state
    if (projectile->GetType() != Projectile::FireGlobProjectile) {
        assert(false);
        return;
    }

    static const float FIRE_GLOB_DMG_AMT = FireBallAI::TOP_POINT_LIFE_POINTS / static_cast<float>(FireBallAI::NUM_OF_TOP_POINT_HITS);
    this->topPointWeakpt->Diminish(FIRE_GLOB_DMG_AMT, gameModel);
    
    // Make sure we reset the rotation (in case we're in the middle of a state that was manipulating it)!!!
    this->boss->alivePartsRoot->SetLocalYRotation(0.0f);
    // Do damage animations...
    // TODO
    
    this->SetState(GothicRomanticBossAI::HurtTopAIState);
}

/**
 * Get the probability of the boss executing a summon attack based on the number of attacks that have happened
 * since the last summon attack.
 */
float FireBallAI::GenerateSummonProbability() const {
    switch (this->attacksSinceLastSummon) {
        case 0:
            return 0.0f;
        case 1:
            return 0.5f;
        case 2:
            return 0.7f;
        default:
            break;
    }

    return 1.0f;
}

void FireBallAI::SetupNextAttackStateAndMove() {
    // Check to see if we're going into a summoning state or not
    float summonProb = this->GenerateSummonProbability();
    if (Randomizer::GetInstance()->RandomNumZeroToOne() <= summonProb) {
        // Summoning attack...
        this->nextAtkAIState = GothicRomanticBossAI::SummonItemsAIState;
        this->nextMovePos    = GothicRomanticBossAI::Center;
    }
    else {
        // Not summoning... doing another type of attack...
        this->nextMovePos    = GothicRomanticBossAI::GetRandomConfinedCornerMovePosition(this->nextMovePos);
        this->nextAtkAIState = this->GetAIStateForConfinedMovePos(this->nextMovePos, this->GenerateOrbAttackProbability());
    }

    this->SetState(GothicRomanticBossAI::BasicMoveAndShootState);
}

void FireBallAI::SetState(GothicRomanticBossAI::AIState newState) {
    switch (newState) {

        case BasicMoveAndShootState:
            // NOTE: nextAtkAIState and nextMovePos must be set
            this->shootCountdown = this->GenerateShootCountdownAmtForMoving();
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            break;

        case SpinLaserAttackAIState: {
            this->shootCountdown = this->GenerateShootCountdownAmtForSpinning();
            double laserSpinTime = this->GetLaserSpinTime();
            this->laserSpinRotAnim.SetLerp(0.0, laserSpinTime, 0.0f, DEFAULT_LASER_SPIN_DEGREES_PER_SEC * laserSpinTime);
            this->attacksSinceLastSummon++;
            break;
        }

        case OrbProjectileAttackAIState:
            this->attacksSinceLastSummon++;
            break;

        case SummonItemsAIState:
            this->attacksSinceLastSummon = 0;
            break;

        case HurtTopAIState:
            break;

        default:
            assert(false);
            return;
    }

    this->currState = newState;
}

void FireBallAI::UpdateState(double dT, GameModel* gameModel) {
    switch (this->currState) {

        case BasicMoveAndShootState:
            this->ExecuteBasicMoveAndShootState(dT, gameModel);
            break;

        case SpinLaserAttackAIState:
            this->ExecuteSpinLaserAttackState(dT, gameModel);
            break;

        case OrbProjectileAttackAIState:
            break;

        case SummonItemsAIState:
            break;

        case HurtTopAIState:
            break;

        default:
            assert(false);
            return;
    }
}

float FireBallAI::GetTotalLifePercent() const {
    return this->topPointWeakpt->GetCurrentLifePercentage();
}

/**
 * In the BasicMoveAndShootState the boss chooses a location to move to and moves there
 * while shooting at the paddle with lasers.
 */
void FireBallAI::ExecuteBasicMoveAndShootState(double dT, GameModel* gameModel) {
    assert(gameModel != NULL);

    // Shoot if the timer has counted-down
    if (this->shootCountdown <= 0.0) {
        // SHOOT!
        this->ShootLaserFromBody(gameModel);
        this->shootCountdown = this->GenerateShootCountdownAmtForMoving();
    }
    else {
        this->shootCountdown -= dT;
    }

    const GameLevel* level = gameModel->GetCurrentLevel();
    assert(level != NULL);

    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();
    Point2D moveToPos = this->GetConfinedBossCenterMovePosition(
        level->GetLevelUnitWidth(), level->GetLevelUnitHeight(), this->nextMovePos);

    float dist = Point2D::Distance(bossPos, moveToPos);
    if (dist < LevelPiece::HALF_PIECE_WIDTH) {
        // Close enough. Go to the appropriate attack state
        this->desiredVel = Vector2D(0,0);
        this->currVel    = Vector2D(0,0);
        this->SetState(this->nextAtkAIState);
        return;
    }

    Vector2D moveDir = moveToPos - bossPos;
    moveDir.Normalize();
    this->desiredVel = this->GetSlowestConfinedMoveSpeed() * moveDir;
}

void FireBallAI::ExecuteSpinLaserAttackState(double dT, GameModel* gameModel) {
    bool isFinished = this->laserSpinRotAnim.Tick(dT);
    if (isFinished) {
        // Make sure we reset the rotation!!!
        this->boss->alivePartsRoot->SetLocalYRotation(0.0f);
        // Move somewhere else...
        this->SetupNextAttackStateAndMove();
        return;
    }

    this->boss->alivePartsRoot->SetLocalYRotation(this->laserSpinRotAnim.GetInterpolantValue());

    // Shoot if the timer has counted-down
    if (this->shootCountdown <= 0.0) {
        // SHOOT!
        this->ShootLaserFromLegPoint(Randomizer::GetInstance()->RandomUnsignedInt() % GothicRomanticBoss::NUM_LEGS, gameModel);
        this->shootCountdown = this->GenerateShootCountdownAmtForSpinning();
    }
    else {
        this->shootCountdown -= dT;
    }
}