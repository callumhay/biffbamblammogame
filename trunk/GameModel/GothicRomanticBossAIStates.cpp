/**
 * GothicRomanticBossAIStates.cpp
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

#include "GothicRomanticBossAIStates.h"
#include "GothicRomanticBoss.h"
#include "BossWeakpoint.h"
#include "GameModel.h"
#include "PlayerPaddle.h"
#include "Projectile.h"
#include "BossOrbProjectile.h"
#include "BossRocketProjectile.h"
#include "PaddleLaserProjectile.h"
#include "PowerChargeEffectInfo.h"
#include "ElectricitySpasmEffectInfo.h"
#include "PuffOfSmokeEffectInfo.h"
#include "ShockwaveEffectInfo.h"
#include "ExpandingHaloEffectInfo.h"
#include "FullscreenFlashEffectInfo.h"
#include "SparkBurstEffectInfo.h"
#include "EnumBossEffectInfo.h"

#include "../GameSound/GameSound.h"

using namespace gothicromanticbossai;

// Begin GothicRomanticBossAI *****************************************************

// Constant defining the border from confines/border blocks to the boss
const float GothicRomanticBossAI::MOVE_X_BORDER = LevelPiece::HALF_PIECE_WIDTH;
const float GothicRomanticBossAI::MOVE_Y_BORDER = LevelPiece::HALF_PIECE_HEIGHT;

const float GothicRomanticBossAI::DEFAULT_LASER_SPIN_DEGREES_PER_SEC  = 360.0f;
const float GothicRomanticBossAI::DEFAULT_ROCKET_SPIN_DEGREES_PER_SEC = 400.0f;

const float GothicRomanticBossAI::DEFAULT_DROP_Y_POS = 10;

const double GothicRomanticBossAI::GLITCH_TIME_IN_SECS = 2.25;

const GothicRomanticBossAI::ConfinedMovePos GothicRomanticBossAI::CORNER_POSITIONS[] = { 
    // Make sure these are in the same order as they are in the enumeration!!!
    GothicRomanticBossAI::TopLeftCorner,
    GothicRomanticBossAI::TopRightCorner,
    GothicRomanticBossAI::BottomLeftCorner,
    GothicRomanticBossAI::BottomRightCorner
};

GothicRomanticBossAI::GothicRomanticBossAI(GothicRomanticBoss* boss) :
BossAIState(), boss(boss), attacksSinceLastSummon(0), itemSummoningSoundID(INVALID_SOUND_ID) {

    assert(boss != NULL);
    this->angryMoveAnim = Boss::BuildBossAngryShakeAnim(1.0f);
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
Point2D GothicRomanticBossAI::GetConfinedBossCenterMovePosition(const ConfinedMovePos& pos) const {

    switch (pos) {
        case TopLeftCorner:
            return Point2D(
                this->boss->GetMinXOfConfines() + GothicRomanticBossAI::MOVE_X_BORDER + GothicRomanticBoss::TOTAL_WIDTH_WITH_LEGS/2.0f,
                this->boss->GetMaxYOfConfines() - (GothicRomanticBossAI::MOVE_Y_BORDER + GothicRomanticBoss::TOTAL_HEIGHT / 2.0f));

        case TopRightCorner:
            return Point2D(
                this->boss->GetMaxXOfConfines() - (GothicRomanticBossAI::MOVE_X_BORDER + GothicRomanticBoss::TOTAL_WIDTH_WITH_LEGS/2.0f),
                this->boss->GetMaxYOfConfines() - (GothicRomanticBossAI::MOVE_Y_BORDER + GothicRomanticBoss::TOTAL_HEIGHT / 2.0f));

        case BottomLeftCorner:
            return Point2D(
                this->boss->GetMinXOfConfines() + GothicRomanticBossAI::MOVE_X_BORDER + GothicRomanticBoss::TOTAL_WIDTH_WITH_LEGS/2.0f,
                this->boss->GetMinYOfConfines() + GothicRomanticBossAI::MOVE_Y_BORDER + GothicRomanticBoss::TOTAL_HEIGHT / 2.0f);

        case BottomRightCorner:
            return Point2D(
                this->boss->GetMaxXOfConfines() - (GothicRomanticBossAI::MOVE_X_BORDER + GothicRomanticBoss::TOTAL_WIDTH_WITH_LEGS/2.0f),
                this->boss->GetMinYOfConfines() + GothicRomanticBossAI::MOVE_Y_BORDER + GothicRomanticBoss::TOTAL_HEIGHT / 2.0f);

        case Center:
            return Point2D(
                this->boss->GetMinXOfConfines() + (this->boss->GetMaxXOfConfines()  - this->boss->GetMinXOfConfines()) / 2.0f,
                this->boss->GetMinYOfConfines() + (this->boss->GetMaxYOfConfines() - this->boss->GetMinYOfConfines()) / 2.0f);
            
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
/// given probability for executing the rocket attack. </summary>
/// <param name="pos"> The confined move position to base the returned AI state on. </param>
/// <param name="probabilityOfRocket"> The base probability of a rocket attack [0,1]. </param>
/// <returns> The AI state to go to next based on the given info. </returns>
GothicRomanticBossAI::AIState GothicRomanticBossAI::GetAIStateForConfinedMovePos(const GothicRomanticBossAI::ConfinedMovePos& pos,
                                                                                 float probabilityOfTargetCannonRocket,
                                                                                 float probabilityOfTargetPaddleRocket) {

    float randomNum1 = Randomizer::GetInstance()->RandomNumZeroToOne() + EPSILON;
    float randomNum2 = Randomizer::GetInstance()->RandomNumZeroToOne() + EPSILON;

    switch (pos) {

        case TopLeftCorner:
        case TopRightCorner:
            
            if (randomNum1 <= 1.1f * probabilityOfTargetCannonRocket) {
                if (randomNum2 <= 1.1f * probabilityOfTargetPaddleRocket) {
                    return RocketToCannonAndPaddleAttackAIState;
                }
                return RocketToCannonAttackAIState;
            }
            else if (randomNum2 <= 1.1f * probabilityOfTargetPaddleRocket) {
                return RocketToPaddleAttackAIState;
            }

            return SpinLaserAttackAIState;
            
        case BottomLeftCorner:
        case BottomRightCorner:

            if (randomNum1 <= probabilityOfTargetCannonRocket) {
                if (randomNum2 <= probabilityOfTargetPaddleRocket) {
                    return RocketToCannonAndPaddleAttackAIState;
                }
                return RocketToCannonAttackAIState;
            }
            else if (randomNum2 <= probabilityOfTargetPaddleRocket) {
                return RocketToPaddleAttackAIState;
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

/**
 * Get the probability of the boss executing a summon attack based on the number of attacks that have happened
 * since the last summon attack.
 */
float GothicRomanticBossAI::GenerateSummonProbability() const {
    switch (this->attacksSinceLastSummon) {
        case 0:
            return 0.0f;
        case 1:
            return 0.5f;
        case 2:
            return 0.8f;
        default:
            break;
    }

    return 1.0f;
}

void GothicRomanticBossAI::ShootOrbFromLegPoint(int legIdx, GameModel* gameModel) const {
    assert(gameModel != NULL);
    
    Point3D spawnPos = this->boss->GetLegPointPos(legIdx);
    Point3D paddlePos(gameModel->GetPlayerPaddle()->GetCenterPosition(), 0.0f);
    Vector3D toPaddleVec = paddlePos - spawnPos;
    toPaddleVec.Normalize();

    Vector2D toPaddleVec2D(toPaddleVec[0], toPaddleVec[1]);
    toPaddleVec2D.Rotate(Randomizer::GetInstance()->RandomNumNegOneToOne() * 25.0f);

    gameModel->AddProjectile(new BossOrbProjectile(Point2D(spawnPos[0], spawnPos[1]), toPaddleVec2D));
    
    // Add an effect for the orb being shot...
    Vector3D offset = spawnPos - this->boss->GetLeg(legIdx)->GetTranslationPt3D();

    GameEventManager::Instance()->ActionBossEffect(ExpandingHaloEffectInfo(this->boss->GetLeg(legIdx), 0.4, 
        Colour(0.8627f, 0.1f, 0.24f), 2.5f, offset));
    GameEventManager::Instance()->ActionBossEffect(SparkBurstEffectInfo(this->boss->GetLeg(legIdx), 0.3, 
        Colour(0.8627f, 0.1f, 0.24f), offset));
}

void GothicRomanticBossAI::ShootOrbFromBody(GameModel* gameModel) const {
    assert(gameModel != NULL);

    Point3D spawnPos = this->boss->GetBottomPointTipPos();
    Point3D paddlePos(gameModel->GetPlayerPaddle()->GetCenterPosition(), 0.0f);
    Vector3D toPaddleVec = paddlePos - spawnPos;
    toPaddleVec.Normalize();

    Vector2D toPaddleVec2D(toPaddleVec[0], toPaddleVec[1]);
    toPaddleVec2D.Rotate(Randomizer::GetInstance()->RandomNumNegOneToOne() * 12.0f);

    gameModel->AddProjectile(new BossOrbProjectile(Point2D(spawnPos[0], spawnPos[1]), toPaddleVec2D));

    Vector3D offset(0, GothicRomanticBoss::BOTTOM_POINT_TIP_Y, 0);
    GameEventManager::Instance()->ActionBossEffect(ExpandingHaloEffectInfo(this->boss->GetBottomPoint(), 0.5, 
        Colour(0.8627f, 0.1f, 0.24f), 1.0f, offset));
    GameEventManager::Instance()->ActionBossEffect(SparkBurstEffectInfo(this->boss->GetBottomPoint(), 0.4, 
        Colour(0.8627f, 0.1f, 0.24f), offset));
}

void GothicRomanticBossAI::ShootRocket(const Point2D& rocketTarget, GameModel* gameModel) const {
    Point2D  bossPos = this->boss->GetBody()->GetTranslationPt2D();
    Vector2D rocketVec = rocketTarget - this->boss->GetBody()->GetTranslationPt2D();
    rocketVec.Normalize();

    gameModel->AddProjectile(new BossRocketProjectile(
        bossPos + (GothicRomanticBoss::BODY_WIDTH - BossRocketProjectile::BOSS_ROCKET_HEIGHT_DEFAULT) * rocketVec, rocketVec, 0.0f));
}

void GothicRomanticBossAI::ShootRocketAtPaddle(GameModel* gameModel) const {
    const PlayerPaddle* paddle = gameModel->GetPlayerPaddle();
    const Point2D& paddlePos = paddle->GetCenterPosition();
    Point2D bossPos = this->boss->GetBody()->GetTranslationPt2D();

    // From the boss to the paddle...
    Vector2D toPaddleVec = paddlePos - bossPos;
    toPaddleVec.Normalize();
    toPaddleVec.Rotate(NumberFuncs::SignOf(paddle->GetLastMovingDirection()) * Randomizer::GetInstance()->RandomNumZeroToOne() * 5.0f);

    gameModel->AddProjectile(new BossRocketProjectile(
        bossPos + (GothicRomanticBoss::BODY_WIDTH - BossRocketProjectile::BOSS_ROCKET_HEIGHT_DEFAULT) * toPaddleVec,
        toPaddleVec, 0.0f));
}

void GothicRomanticBossAI::GetItemDropPositions(int numItemsToDrop, const GameLevel& level, std::vector<Point2D>& positions) {
    positions.resize(numItemsToDrop);

    float minX = 0.0f;
    float maxX = level.GetLevelUnitWidth();

    // In cases where the number of items isn't to large, we confine it to the smaller, enclosed level space.
    // This was tested empirically.
    if (numItemsToDrop <= 5) {
        minX = level.GetMinXPaddleBoundPiece(0)->GetCenter()[0] + LevelPiece::HALF_PIECE_WIDTH;
        maxX = level.GetMaxXPaddleBoundPiece(0)->GetCenter()[0] - LevelPiece::HALF_PIECE_WIDTH;
    }

    float axisLengthToDropIn = maxX - minX;
    assert(axisLengthToDropIn > 0);
    float spaceBetween = axisLengthToDropIn / static_cast<float>(numItemsToDrop + 1);

    Point2D currDropPt(minX, DEFAULT_DROP_Y_POS);
    for (int i = 0; i < numItemsToDrop; i++) {
        currDropPt[0] += spaceBetween;
        positions[i] = currDropPt;
    }
}

AnimationMultiLerp<float> GothicRomanticBossAI::GenerateRocketSpinAnimation(int numRockets) {
    AnimationMultiLerp<float> animation;
    
    static const double DEFAULT_TIME_BETWEEN_SPINS = 1.0;

    std::vector<double> timeVals;
    timeVals.reserve(numRockets + 2);
    timeVals.push_back(0.0);
    for (int i = 0; i <= numRockets; i++) {
        timeVals.push_back(timeVals.back() + DEFAULT_TIME_BETWEEN_SPINS + Randomizer::GetInstance()->RandomNumZeroToOne() * 0.5);
    }

    int negOrPos = Randomizer::GetInstance()->RandomNegativeOrPositive();
    double dT;

    std::vector<float> rotVals;
    rotVals.reserve(timeVals.size());
    rotVals.push_back(0.0);
    for (int i = 0; i < numRockets; i++) {
        dT = timeVals[i+1] - timeVals[i];
        assert(dT > 0);
        rotVals.push_back(rotVals.back() + (negOrPos * DEFAULT_ROCKET_SPIN_DEGREES_PER_SEC * dT));
        negOrPos *= -1;
    }
    rotVals.push_back(0.0);

    animation.SetLerp(timeVals, rotVals);
    animation.SetRepeat(false);
    animation.SetInterpolantValue(0.0f);
    return animation;
}

AnimationMultiLerp<Vector2D> GothicRomanticBossAI::GenerateGlitchShakeAnimation() {
    AnimationMultiLerp<Vector2D> result;
    
    static const int NUM_SHAKES = 50;
    static const double SHAKE_TIME_INC = GLITCH_TIME_IN_SECS / static_cast<double>(NUM_SHAKES);
    
    std::vector<double> timeVals;
    timeVals.reserve(NUM_SHAKES + 1);
    timeVals.push_back(0.0);
    for (int i = 0; i < NUM_SHAKES; i++) {
        timeVals.push_back(timeVals.back() + SHAKE_TIME_INC);
    }

    static const float SHAKE_RADIUS = std::min<float>(MOVE_X_BORDER, MOVE_Y_BORDER) / 2.0f;

    std::vector<Vector2D> shakeVals;
    shakeVals.reserve(timeVals.size());
    shakeVals.push_back(Vector2D(0,0));
    for (int i = 0; i < NUM_SHAKES-1; i++) {
        shakeVals.push_back(Vector2D(
            Randomizer::GetInstance()->RandomNumNegOneToOne() * SHAKE_RADIUS,
            Randomizer::GetInstance()->RandomNumNegOneToOne() * SHAKE_RADIUS));
    }
    shakeVals.push_back(Vector2D(0,0));

    result.SetLerp(timeVals, shakeVals);
    result.SetInterpolantValue(Vector2D(0,0));
    result.SetRepeat(false);

    return result;
}

/**
 * Generates a set of randomly sequenced positions for where the cannons are in the boss level. This
 * provides the boss with a list of these positions to shoot rockets at.
 */
void GothicRomanticBossAI::GenerateRocketTargetCannonPositions(int numRockets, std::vector<Point2D>& positions) {
    assert(numRockets > 0);

    positions.clear();
    positions.reserve(numRockets);

    static const Point2D CANNON_POSITIONS[] = {
        Point2D(2*LevelPiece::PIECE_WIDTH + LevelPiece::HALF_PIECE_WIDTH, 28*LevelPiece::PIECE_HEIGHT + LevelPiece::HALF_PIECE_HEIGHT),  // top-left
        Point2D(2*LevelPiece::PIECE_WIDTH + LevelPiece::HALF_PIECE_WIDTH, 12*LevelPiece::PIECE_HEIGHT + LevelPiece::HALF_PIECE_HEIGHT),  // bottom-left
        Point2D(18*LevelPiece::PIECE_WIDTH + LevelPiece::HALF_PIECE_WIDTH, 28*LevelPiece::PIECE_HEIGHT + LevelPiece::HALF_PIECE_HEIGHT), // top-right
        Point2D(18*LevelPiece::PIECE_WIDTH + LevelPiece::HALF_PIECE_WIDTH, 12*LevelPiece::PIECE_HEIGHT + LevelPiece::HALF_PIECE_HEIGHT)  // bottom-right
    };

    int count = numRockets;
    while (count > 0) {
        
        int lastIdxChoice = 0;
        int numIterations = std::min<int>(4, count);
        for (int j = 0; j < numIterations; j++) {
            lastIdxChoice = (lastIdxChoice + (1 + Randomizer::GetInstance()->RandomUnsignedInt() % (4-j))) % 4;
            positions.push_back(CANNON_POSITIONS[lastIdxChoice]);
        }

        count -= numIterations;
    }
}

/**
 * Generates a list of the positions where the regen blocks are located in the boss level so that
 * the boss can target them.
 */
void GothicRomanticBossAI::GenerateTargetRegenBlockPositions(std::vector<Point2D>& positions) {
    positions.clear();
    positions.reserve(3);

    static const float Y_POS = 29*LevelPiece::PIECE_HEIGHT + LevelPiece::HALF_PIECE_HEIGHT;
    positions.push_back(Point2D(7*LevelPiece::PIECE_WIDTH + LevelPiece::HALF_PIECE_WIDTH, Y_POS));
    positions.push_back(Point2D(10*LevelPiece::PIECE_WIDTH + LevelPiece::HALF_PIECE_WIDTH, Y_POS));
    positions.push_back(Point2D(13*LevelPiece::PIECE_WIDTH + LevelPiece::HALF_PIECE_WIDTH, Y_POS));
}

void GothicRomanticBossAI::DoSummonEffect(double timeInSecs) {
    EnumBossEffectInfo summonEffect(EnumBossEffectInfo::GothicRomanticBossSummon);
    summonEffect.SetTimeInSecs(timeInSecs);
    summonEffect.SetBodyPart(this->boss->GetBody());
    summonEffect.SetSize1D(GothicRomanticBoss::BODY_WIDTH);

    GameEventManager::Instance()->ActionBossEffect(summonEffect);
}

// End GothicRomanticBossAI *****************************************************

// Begin ConfinedAI **************************************************************

ConfinedAI::ConfinedAI(GothicRomanticBoss* boss, int numItemsPerSummoning) : GothicRomanticBossAI(boss),
summonsSinceLastSpecialItemDrop(0), numItemsPerSummoning(numItemsPerSummoning) {
    assert(numItemsPerSummoning > 0);

    // Setup any animations specific to this state...
    this->laserSpinRotAnim.SetInterpolantValue(0.0f);
    this->laserSpinRotAnim.SetRepeat(false);
}

float ConfinedAI::GetAccelerationMagnitude() const {
    return 3*LevelPiece::PIECE_WIDTH;
}

/**
 * Get the probability of the boss summoning a special item when summoning items.
 */
float ConfinedAI::GenerateSpecialItemSummonProbability() const {
    return 1.0f;
}

void ConfinedAI::SetupNextAttackStateAndMove(GameModel* gameModel) {

    // Check to see if we're going into a summoning state or not
    float summonProb = this->GenerateSummonProbability();
    if (Randomizer::GetInstance()->RandomNumZeroToOne() <= summonProb &&
        gameModel->GetCurrentStateType() == GameState::BallInPlayStateType) {
        // Summoning attack...
        this->nextAtkAIState = GothicRomanticBossAI::SummonItemsAIState;
        this->nextMovePos    = GothicRomanticBossAI::Center;
    }
    else {
        // Not summoning... doing another type of attack...
        this->nextMovePos    = GothicRomanticBossAI::GetRandomConfinedCornerMovePosition(this->nextMovePos);
        this->nextAtkAIState = this->GetAIStateForConfinedMovePos(this->nextMovePos,
            this->GenerateRocketCannonAttackProbability(this->GetRocketCannonAtkProbabilityMultiplier()),
            this->GenerateRocketPaddleAttackProbability(this->GetRocketPaddleAtkProbabilityMultiplier()));
    }

    this->SetState(GothicRomanticBossAI::BasicMoveAndShootState);
}

/**
 * In the BasicMoveAndShootState the boss chooses a location to move to and moves there
 * while shooting at the paddle with lasers.
 */
void ConfinedAI::ExecuteBasicMoveAndShootState(double dT, GameModel* gameModel) {
    assert(gameModel != NULL);

    // Shoot if the timer has counted-down
    if (this->shootCountdown <= 0.0) {
        // SHOOT!
        this->ShootOrbFromBody(gameModel);
        this->shootCountdown = this->GenerateShootCountdownAmtForMoving();
    }
    else {
        this->shootCountdown -= dT;
    }

    if (this->MoveToTargetPosition(this->GetSlowestConfinedMoveSpeed())) {
        this->SetState(this->nextAtkAIState);
        return;
    }
}

void ConfinedAI::ExecuteSpinLaserAttackState(double dT, GameModel* gameModel) {
    bool isFinished = this->laserSpinRotAnim.Tick(dT);
    if (isFinished) {
        // Make sure we reset the rotation!!!
        this->boss->alivePartsRoot->SetLocalYRotation(0.0f);
        // Move somewhere else...
        this->SetupNextAttackStateAndMove(gameModel);
        return;
    }

    this->boss->alivePartsRoot->SetLocalYRotation(this->laserSpinRotAnim.GetInterpolantValue());

    // Shoot if the timer has counted-down
    if (this->shootCountdown <= 0.0) {
        // SHOOT!
        this->ShootOrbFromLegPoint(Randomizer::GetInstance()->RandomUnsignedInt() % GothicRomanticBoss::NUM_LEGS, gameModel);
        this->shootCountdown = this->GenerateShootCountdownAmtForSpinning();
    }
    else {
        this->shootCountdown -= dT;
    }
}

void ConfinedAI::ExecuteRocketToCannonAttackState(double dT, GameModel* gameModel) {

    // Shoot the rockets as the timer ticks down...
    if (!this->rocketTargetPositions.empty()) {
        if (this->shootCountdown <= 0.0) {
            
            // SHOOT ze missile!
            Point2D rocketTarget = this->rocketTargetPositions.back();
            this->rocketTargetPositions.pop_back();

            this->ShootRocket(rocketTarget, gameModel);
            this->shootCountdown = this->GenerateTimeBetweenRockets();
        }
        else {
            this->shootCountdown -= dT;
        }
    }

    bool isFinished = this->rocketSpinRotAnim.Tick(dT);
    if (isFinished) {
        // Make sure we reset the rotation!!!
        this->boss->alivePartsRoot->SetLocalYRotation(0.0f);

        // Move somewhere else...
        this->SetupNextAttackStateAndMove(gameModel);
        return;
    }

    this->boss->alivePartsRoot->SetLocalYRotation(this->rocketSpinRotAnim.GetInterpolantValue());
}

void ConfinedAI::ExecuteSummonItemsState(double dT, GameModel* gameModel) {

    // Check to see if the game is in a suitable state for items to be dropped...
    if (gameModel->GetCurrentStateType() != GameState::BallInPlayStateType) {
        // Cancel the summoning sound(s)
        gameModel->GetSound()->StopSound(this->itemSummoningSoundID, 0.5);
        
        // If the ball isn't in play then we go to another state...
        this->SetupNextAttackStateAndMove(gameModel);
        return;
    }

    if (this->summonItemsDelayCountdown <= 0.0) {

        const GameLevel* level = gameModel->GetCurrentLevel();
        assert(level != NULL);

        // Summon the items...
       
        // Figure out where to drop the items that will be summoned
        std::vector<Point2D> itemDropPositions;
        GothicRomanticBossAI::GetItemDropPositions(this->numItemsPerSummoning, *level, itemDropPositions);

        // Generate a random set of items with the possibility of dropping the fireball (which is used to
        // hurt the boss at this stage) -- the allowable drops have been custom tailored to ensure they are all awful for the player
        std::vector<GameItem::ItemType> allowableItemDrops = level->GetAllowableItemDropTypes();
        bool allowLevelFlip = this->AllowLevelFlipItem();
        if (!allowLevelFlip) {
            for (std::vector<GameItem::ItemType>::iterator iter = allowableItemDrops.begin(); iter != allowableItemDrops.end(); ++iter) {
                if (*iter == GameItem::UpsideDownItem) {
                    allowableItemDrops.erase(iter);
                    break;
                }
            }
        }

        std::vector<GameItem::ItemType> dropTypes;
        dropTypes.reserve(this->numItemsPerSummoning);
        for (int i = 0; i < this->numItemsPerSummoning; i++) {
            dropTypes.push_back(allowableItemDrops[Randomizer::GetInstance()->RandomUnsignedInt() % allowableItemDrops.size()]);
        }

        // Determine whether we will be dropping a special item, if so, we place it at a random spot in the drop
        if (Randomizer::GetInstance()->RandomNumZeroToOne() <= this->GenerateSpecialItemSummonProbability()) {
            dropTypes[Randomizer::GetInstance()->RandomUnsignedInt() % this->numItemsPerSummoning] = this->GetSpecialItemDropType();
            this->summonsSinceLastSpecialItemDrop = 0;
        }
        else {
            this->summonsSinceLastSpecialItemDrop++;
        }

        assert(static_cast<int>(itemDropPositions.size()) == this->numItemsPerSummoning);
        assert(dropTypes.size() == itemDropPositions.size());

        for (int i = 0; i < this->numItemsPerSummoning; i++) {
            gameModel->AddItemDrop(itemDropPositions[i], dropTypes[i]);

            // EVENT: Effects for each of the items being magically spawned
            GameEventManager::Instance()->ActionBossEffect(ShockwaveEffectInfo(itemDropPositions[i], GameItem::ITEM_WIDTH, 1.0));
            GameEventManager::Instance()->ActionBossEffect(PuffOfSmokeEffectInfo(itemDropPositions[i], 0.5f*GameItem::ITEM_WIDTH, Colour(1,1,1)));
        }

        // EVENT: Effects for the boss itself
        GameEventManager::Instance()->ActionBossEffect(SparkBurstEffectInfo(this->boss->GetBody(), 1.0, Colour(1.0f, 1.0f, 1.0f)));

        // Go to the next state...
        this->SetupNextAttackStateAndMove(gameModel);
    }
    else {
        this->summonItemsDelayCountdown -= dT;
    }
}

// End ConfinedAI **************************************************************

// Begin FireBallAI **************************************************************

// This number doesn't really matter too much, just incase we want to make it more robust later
const float FireBallAI::TOP_POINT_LIFE_POINTS = 300.0f;

// The time between firing the rockets that destroy the regen blocks (that drop the fire globs)
const double FireBallAI::TIME_BETWEEN_REGEN_BLOCK_DESTROYING_ROCKETS = 0.35;

const double FireBallAI::TOP_FADE_TIME = 2.25;

FireBallAI::FireBallAI(GothicRomanticBoss* boss) : ConfinedAI(boss, FireBallAI::NUM_ITEMS_PER_SUMMONING),
topPointWeakpt(NULL) {

    boss->ConvertAliveBodyPartToWeakpoint(boss->topPointIdx, FireBallAI::TOP_POINT_LIFE_POINTS, 0);
    assert(dynamic_cast<BossWeakpoint*>(boss->bodyParts[boss->topPointIdx]) != NULL);
    this->topPointWeakpt = static_cast<BossWeakpoint*>(boss->bodyParts[boss->topPointIdx]);

    {
        std::vector<double> timeVals;
        timeVals.reserve(3);
        timeVals.push_back(0.0);
        timeVals.push_back(BossWeakpoint::DEFAULT_INVULNERABLE_TIME_IN_SECS / 10.0f);
        timeVals.push_back(BossWeakpoint::DEFAULT_INVULNERABLE_TIME_IN_SECS / 1.25f);

        std::vector<float> moveVals;
        moveVals.reserve(timeVals.size());
        moveVals.push_back(0.0f);
        moveVals.push_back(-GothicRomanticBossAI::MOVE_Y_BORDER / 1.25f);
        moveVals.push_back(0.0f);

        this->hitOnTopMoveDownAnim.SetLerp(timeVals, moveVals);
        this->hitOnTopMoveDownAnim.SetInterpolantValue(0.0f);
        this->hitOnTopMoveDownAnim.SetRepeat(false);
    }

    this->nextMovePos    = this->GetRandomConfinedMovePosition(Center);
    this->nextAtkAIState = GothicRomanticBossAI::SpinLaserAttackAIState;
    this->SetState(GothicRomanticBossAI::BasicMoveAndShootState);

    // Set the shoot countdown a large number so the boss doesn't attack the first time it moves
    // (to accommodate the light fade-in, without cheap shots)
    this->shootCountdown = DBL_MAX;
}

FireBallAI::~FireBallAI() {
}

void FireBallAI::CollisionOccurred(GameModel* gameModel, Projectile* projectile,
                                   BossBodyPart* collisionPart) {

    UNUSED_PARAMETER(gameModel);

    // We only care if the projectile hits the weakpoint on the boss
    if (collisionPart != this->topPointWeakpt) {
        return;
    }

    // Only fire globs should be able to hit the boss' weakpoint in this state
    if (projectile->GetType() != Projectile::FireGlobProjectile) {
        assert(false);
        return;
    }

    // If the boss is currently animating for being hurt then we can't hurt it again until it's done
    if (this->currState == GothicRomanticBossAI::HurtTopAIState ||
        this->currState == GothicRomanticBossAI::VeryHurtAndAngryAIState ||
        this->currState == GothicRomanticBossAI::DestroyRegenBlocksAIState) {
        return;
    }

    static const float FIRE_GLOB_DMG_AMT = FireBallAI::TOP_POINT_LIFE_POINTS / static_cast<float>(FireBallAI::NUM_OF_TOP_POINT_HITS);
    this->topPointWeakpt->Diminish(FIRE_GLOB_DMG_AMT);
    
    // Make sure we reset the rotation (in case we're in the middle of a state that was manipulating it)!!!
    this->boss->alivePartsRoot->SetLocalYRotation(0.0f);
    // Do damage animations...
    // Tilt the boss a bit (rotate on z-axis) like it got hit by something
    this->hitOnTopTiltAnim = this->GenerateHitOnTopTiltAnim(projectile->GetPosition());
    
    // Check to see if the top is dead, if it is then we change its state in the body of the boss
    if (this->topPointWeakpt->GetIsDestroyed()) {

        bool fallLeft = this->topPointWeakpt->GetTranslationPt2D()[0] < projectile->GetPosition()[0];
        this->boss->ConvertAliveBodyPartToDeadBodyPart(this->boss->topPointIdx);
        
        // Animate the top to fade out and "fall off" the body
        AbstractBossBodyPart* topPart = this->boss->bodyParts[this->boss->topPointIdx];
        topPart->AnimateColourRGBA(Boss::BuildBossHurtFlashAndFadeAnim(TOP_FADE_TIME));
        topPart->AnimateLocalTranslation(this->GenerateTopDeathTranslationAnimation(fallLeft));
        topPart->AnimateLocalZRotation(this->GenerateTopDeathRotationAnimation(fallLeft));
    }

    this->SetState(GothicRomanticBossAI::HurtTopAIState);
}

AnimationMultiLerp<float> FireBallAI::GenerateHitOnTopTiltAnim(const Point2D& hitPos) const {
    // Figure out which way and how much we should tilt the boss based on the location of the
    // hit relative to the apex of the bosses' head
    Point3D bossApexPos = this->boss->GetTopPointTipPos();
    
    float hitAmt = std::max<float>(-GothicRomanticBoss::HALF_TOP_POINT_WIDTH,
        std::min<float>(GothicRomanticBoss::HALF_TOP_POINT_WIDTH, hitPos[0] - bossApexPos[0]));
    float tiltAmtInDegs = NumberFuncs::LerpOverFloat<float>(-GothicRomanticBoss::HALF_TOP_POINT_WIDTH, 
        GothicRomanticBoss::HALF_TOP_POINT_WIDTH, -20.0f, 20.0f, hitAmt);

    AnimationMultiLerp<float> result;

    std::vector<double> timeVals;
    timeVals.reserve(3);
    timeVals.push_back(0.0);
    timeVals.push_back(BossWeakpoint::DEFAULT_INVULNERABLE_TIME_IN_SECS / 5.0);
    timeVals.push_back(BossWeakpoint::DEFAULT_INVULNERABLE_TIME_IN_SECS / 1.25);

    std::vector<float> rotVals;
    rotVals.reserve(timeVals.size());
    rotVals.push_back(0.0f);
    rotVals.push_back(tiltAmtInDegs);
    rotVals.push_back(0.0f);
    
    result.SetLerp(timeVals, rotVals);
    result.SetInterpolantValue(0.0f);
    result.SetRepeat(false);
    return result;
}

AnimationMultiLerp<Vector3D> FireBallAI::GenerateTopDeathTranslationAnimation(bool fallLeft) const {
    return Boss::BuildLimbFallOffTranslationAnim(TOP_FADE_TIME, (fallLeft ? -1 : 1) * 4 * GothicRomanticBoss::TOP_POINT_WIDTH, 
        -3.0f * GothicRomanticBoss::TOP_POINT_WIDTH - this->boss->alivePartsRoot->GetTranslationPt2D()[1]);
}

AnimationMultiLerp<float> FireBallAI::GenerateTopDeathRotationAnimation(bool fallLeft) const {
    return Boss::BuildLimbFallOffZRotationAnim(TOP_FADE_TIME, fallLeft ? 600.0f : -600.0f);
}

void FireBallAI::SetState(GothicRomanticBossAI::AIState newState) {

    switch (newState) {
        case BasicMoveAndShootState:
            // NOTE: nextAtkAIState and nextMovePos must be set
            this->shootCountdown = this->GenerateShootCountdownAmtForMoving();
            this->SetMoveToTargetPosition(this->boss->alivePartsRoot->GetTranslationPt2D(), 
                this->GetConfinedBossCenterMovePosition(this->nextMovePos));
            break;

        case SpinLaserAttackAIState: {
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->shootCountdown = this->GenerateShootCountdownAmtForSpinning();
            double laserSpinTime = this->GetLaserSpinTime();
            this->laserSpinRotAnim.SetLerp(0.0, laserSpinTime, 0.0f, DEFAULT_LASER_SPIN_DEGREES_PER_SEC * laserSpinTime);
            this->attacksSinceLastSummon++;
            break;
        }

        case RocketToCannonAttackAIState:
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->attacksSinceLastSummon++;
            this->numRocketsToFire = this->GenerateNumRocketsToFire();
            this->rocketSpinRotAnim = this->GenerateRocketSpinAnimation(this->numRocketsToFire);
            this->shootCountdown = this->GenerateTimeBetweenRockets();
            GothicRomanticBossAI::GenerateRocketTargetCannonPositions(this->numRocketsToFire, this->rocketTargetPositions);
            break;

        case SummonItemsAIState:
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->attacksSinceLastSummon = 0;

            this->summonItemsDelayCountdown = GothicRomanticBoss::DELAY_BEFORE_SUMMONING_ITEMS_IN_SECS;
            
            // EVENT: Summon items power charge
            this->DoSummonEffect(this->summonItemsDelayCountdown);

            this->itemSummoningSoundID = this->boss->GetGameModel()->GetSound()->PlaySound(GameSound::GothicBossSummonItemChargeEvent, false);

            break;

        case HurtTopAIState:
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->boss->alivePartsRoot->AnimateColourRGBA(Boss::BuildBossHurtAndInvulnerableColourAnim(
                BossWeakpoint::DEFAULT_INVULNERABLE_TIME_IN_SECS));
            this->hitOnTopMoveDownAnim.ResetToStart();
            
            // Stop any summoning sound that might be happening
            this->boss->GetGameModel()->GetSound()->StopSound(this->itemSummoningSoundID, 0.25);
            this->itemSummoningSoundID = INVALID_SOUND_ID;

            break;

        case VeryHurtAndAngryAIState:
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);

            this->boss->alivePartsRoot->AnimateColourRGBA(Boss::BuildBossAngryFlashAnim());
            this->angryMoveAnim.ResetToStart();

            // EVENT: Boss is angry! Rawr.
            GameEventManager::Instance()->ActionBossAngry(this->boss, this->boss->GetBody());
            break;

        case DestroyRegenBlocksAIState:
            this->shootCountdown = 0.0;
            GothicRomanticBossAI::GenerateTargetRegenBlockPositions(this->rocketTargetPositions);
            this->numRocketsToFire = static_cast<int>(this->rocketTargetPositions.size());
            this->shootCountdown = TIME_BETWEEN_REGEN_BLOCK_DESTROYING_ROCKETS;
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

        case RocketToCannonAttackAIState:
            this->ExecuteRocketToCannonAttackState(dT, gameModel);
            break;

        case SummonItemsAIState:
            this->ExecuteSummonItemsState(dT, gameModel);
            break;

        case HurtTopAIState:
            this->ExecuteHurtTopState(dT, gameModel);
            break;

        case VeryHurtAndAngryAIState:
            this->ExecuteVeryHurtAndAngryState(dT, gameModel);
            break;

        case DestroyRegenBlocksAIState:
            this->ExecuteDestroyRegenBlocksState(dT, gameModel);
            break;

        default:
            assert(false);
            return;
    }
}

float FireBallAI::GetTotalLifePercent() const {
    return this->topPointWeakpt->GetCurrentLifePercentage();
}

void FireBallAI::ExecuteHurtTopState(double dT, GameModel* gameModel) {
    UNUSED_PARAMETER(gameModel);

    this->currVel    = Vector2D(0,0);
    this->desiredVel = Vector2D(0,0);

    bool isFinished = true;
    isFinished &= this->hitOnTopMoveDownAnim.Tick(dT);
    isFinished &= this->hitOnTopTiltAnim.Tick(dT);

    // Animate the boss so that it looks like it got hurt
    this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0, this->hitOnTopMoveDownAnim.GetInterpolantValue(), 0));
    this->boss->alivePartsRoot->SetLocalZRotation(this->hitOnTopTiltAnim.GetInterpolantValue());

    if (isFinished) {
        // Be sure to reset the local transforms of the boss
        this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0,0,0));
        this->boss->alivePartsRoot->SetLocalZRotation(0.0f);

        if (this->topPointWeakpt->GetIsDestroyed()) {
            this->SetState(GothicRomanticBossAI::VeryHurtAndAngryAIState);
        }
        else {
            this->SetupNextAttackStateAndMove(gameModel);
        }
    }
}

// This is called when the life of the weakpoint has been exhausted
void FireBallAI::ExecuteVeryHurtAndAngryState(double dT, GameModel* gameModel) {
    UNUSED_PARAMETER(gameModel);

    assert(this->topPointWeakpt->GetIsDestroyed());

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

        // The final state for this high-level class state is the one where the boss blows up the
        // regen blocks that hurt him via fire globs
        this->SetState(GothicRomanticBossAI::DestroyRegenBlocksAIState);
    }
}

void FireBallAI::ExecuteDestroyRegenBlocksState(double dT, GameModel* gameModel) {

    if (this->shootCountdown <= 0.0) {

        // Shoot the rockets as the timer ticks down...
        if (!this->rocketTargetPositions.empty()) {

            Point2D rocketTarget = this->rocketTargetPositions.back();
            this->rocketTargetPositions.pop_back();

            this->ShootRocket(rocketTarget, gameModel);
            this->shootCountdown = TIME_BETWEEN_REGEN_BLOCK_DESTROYING_ROCKETS;
        }
        else {
            // Go to the next high-level AI state
            this->boss->SetNextAIState(new IceBallAI(this->boss, this->nextMovePos));
        }
    }
    else {
        this->shootCountdown -= dT;
    }
}

// End FireBallAI *******************************************************************************

// Begin IceBallAI ******************************************************************************

const float IceBallAI::BOTTOM_POINT_LIFE_POINTS = PaddleLaserProjectile::DAMAGE_DEFAULT * IceBallAI::NUM_LASER_HITS;
const double IceBallAI::BOTTOM_FADE_TIME = 2.25;

const double IceBallAI::DESTROY_CONFINES_TIME_IN_SECS           = 6.0;
const float IceBallAI::DESTROY_CONFINES_SPIN_ROT_SPD            = 500.0f;
const double IceBallAI::TIME_TO_CHARGE_CONFINE_DESTROYER_BLAST  = 2.0;
const double IceBallAI::TIME_BEFORE_DESTROY_CONFINES_FLASH      = 0.5;

IceBallAI::IceBallAI(GothicRomanticBoss* boss, ConfinedMovePos prevMovePos) : 
ConfinedAI(boss, IceBallAI::NUM_ITEMS_PER_SUMMONING), bottomPointWeakpt(NULL),
numRocketsToFireAtPaddle(0), transitionSoundID(INVALID_SOUND_ID) {

    // Make the bottom point of the boss the new weak point...
    boss->ConvertAliveBodyPartToWeakpoint(boss->bottomPointIdx, IceBallAI::BOTTOM_POINT_LIFE_POINTS, 0);
    assert(dynamic_cast<BossWeakpoint*>(boss->bodyParts[this->boss->bottomPointIdx]) != NULL);
    this->bottomPointWeakpt = static_cast<BossWeakpoint*>(boss->bodyParts[this->boss->bottomPointIdx]);

    // Setup any animations for this state
    this->glitchShakeAnim = GothicRomanticBossAI::GenerateGlitchShakeAnimation();
    {
        std::vector<double> timeVals;
        timeVals.reserve(3);
        timeVals.push_back(0.0);
        timeVals.push_back(BossWeakpoint::DEFAULT_INVULNERABLE_TIME_IN_SECS / 10.0f);
        timeVals.push_back(BossWeakpoint::DEFAULT_INVULNERABLE_TIME_IN_SECS / 1.25f);

        std::vector<float> moveVals;
        moveVals.reserve(timeVals.size());
        moveVals.push_back(0.0f);
        moveVals.push_back(GothicRomanticBossAI::MOVE_Y_BORDER / 1.25f);
        moveVals.push_back(0.0f);

        this->hitOnBottomMoveUpAnim.SetLerp(timeVals, moveVals);
        this->hitOnBottomMoveUpAnim.SetInterpolantValue(0.0f);
        this->hitOnBottomMoveUpAnim.SetRepeat(false);
    }

    this->spinningDestroyConfinesAnim.SetLerp(0.0, DESTROY_CONFINES_TIME_IN_SECS, 0.0f, 
        DESTROY_CONFINES_SPIN_ROT_SPD*DESTROY_CONFINES_TIME_IN_SECS);
    this->spinningDestroyConfinesAnim.SetInterpolantValue(0.0f);
    this->spinningDestroyConfinesAnim.SetRepeat(false);

    this->nextMovePos    = this->GetRandomConfinedMovePosition(prevMovePos);
    this->nextAtkAIState = GothicRomanticBossAI::SpinLaserAttackAIState;
    this->SetState(GothicRomanticBossAI::BasicMoveAndShootState);
}

IceBallAI::~IceBallAI() {
    this->boss->GetGameModel()->GetSound()->StopSound(this->transitionSoundID, 3.0);
}

void IceBallAI::CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart) {
    UNUSED_PARAMETER(gameModel);

    // We only care if the projectile hits the weakpoint on the boss
    if (collisionPart != this->bottomPointWeakpt) {
        return;
    }

    // Only the paddle's lasers will hurt the boss
    if (projectile->GetType() != Projectile::PaddleLaserBulletProjectile) {
        return;
    }

    // NOTE: there's no need to diminish the weakpoint, that will already be done for us

    // Make sure we reset the rotation and translation (in case we're in the middle of a state that was manipulating it)!!!
    this->boss->alivePartsRoot->SetLocalYRotation(0.0f);
    this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0.0f, 0.0f, 0.0f));

    // Check to see if the bottom is dead, if it is then we change its state in the body of the boss
    if (this->bottomPointWeakpt->GetIsDestroyed()) {

        bool fallLeft = this->bottomPointWeakpt->GetTranslationPt2D()[0] < projectile->GetPosition()[0];
        this->boss->ConvertAliveBodyPartToDeadBodyPart(this->boss->bottomPointIdx);
        
        // Animate the top to fade out and "fall off" the body
        this->bottomPointWeakpt->AnimateColourRGBA(Boss::BuildBossHurtFlashAndFadeAnim(BOTTOM_FADE_TIME));
        this->bottomPointWeakpt->AnimateLocalTranslation(this->GenerateBottomDeathTranslationAnimation(fallLeft));
        this->bottomPointWeakpt->AnimateLocalZRotation(this->GenerateBottomDeathRotationAnimation(fallLeft));
    }

    if (this->currState != GothicRomanticBossAI::HurtBottomAIState) {
        // Tilt the boss a bit (rotate on z-axis) like it got hit by something
        this->hitOnBottomTiltAnim = this->GenerateHitOnBottomTiltAnim(projectile->GetPosition());
        this->SetState(GothicRomanticBossAI::HurtBottomAIState);
    }
}

AnimationMultiLerp<float> IceBallAI::GenerateHitOnBottomTiltAnim(const Point2D& hitPos) const {
    // Figure out which way and how much we should tilt the boss based on the location of the
    // hit relative to the apex of the bosses' butt
    Point3D bossPos = this->boss->GetBottomPointTipPos();
    
    float hitAmt = std::max<float>(-GothicRomanticBoss::HALF_BOTTOM_POINT_WIDTH,
        std::min<float>(GothicRomanticBoss::HALF_BOTTOM_POINT_WIDTH, hitPos[0] - bossPos[0]));
    float tiltAmtInDegs = NumberFuncs::LerpOverFloat<float>(-GothicRomanticBoss::HALF_BOTTOM_POINT_WIDTH, 
        GothicRomanticBoss::HALF_BOTTOM_POINT_WIDTH, 20.0f, -20.0f, hitAmt);

    AnimationMultiLerp<float> result;

    std::vector<double> timeVals;
    timeVals.reserve(3);
    timeVals.push_back(0.0);
    timeVals.push_back(BossWeakpoint::DEFAULT_INVULNERABLE_TIME_IN_SECS / 5.0);
    timeVals.push_back(BossWeakpoint::DEFAULT_INVULNERABLE_TIME_IN_SECS / 1.25);

    std::vector<float> rotVals;
    rotVals.reserve(timeVals.size());
    rotVals.push_back(0.0f);
    rotVals.push_back(tiltAmtInDegs);
    rotVals.push_back(0.0f);
    
    result.SetLerp(timeVals, rotVals);
    result.SetInterpolantValue(0.0f);
    result.SetRepeat(false);
    return result;
}

AnimationMultiLerp<Vector3D> IceBallAI::GenerateBottomDeathTranslationAnimation(bool fallLeft) const {
    return Boss::BuildLimbFallOffTranslationAnim(BOTTOM_FADE_TIME, (fallLeft ? -1 : 1) * 4 * GothicRomanticBoss::BOTTOM_POINT_WIDTH, 
        -2.0f * GothicRomanticBoss::BOTTOM_POINT_WIDTH - this->boss->alivePartsRoot->GetTranslationPt2D()[1]);
}

AnimationMultiLerp<float> IceBallAI::GenerateBottomDeathRotationAnimation(bool fallLeft) const {
    return Boss::BuildLimbFallOffZRotationAnim(BOTTOM_FADE_TIME, fallLeft ? 620.0f : -620.0f);
}

void IceBallAI::BlowUpLegs() {
    Point2D bossCenterPos = this->boss->alivePartsRoot->GetTranslationPt2D();

    for (int i = 0; i < GothicRomanticBoss::NUM_LEGS; i++) {
        AbstractBossBodyPart* leg = this->boss->bodyParts[this->boss->legIdxs[i]];
        bool fallLeft = leg->GetTranslationPt2D()[0] < bossCenterPos[0];

        this->boss->ConvertAliveBodyPartToDeadBodyPart(this->boss->legIdxs[i]);
        
        // Animate the leg to fade and fall off the body
        double animationTime = 0.75*DESTROY_CONFINES_TIME_IN_SECS;
        leg->AnimateColourRGBA(Boss::BuildBossHurtFlashAndFadeAnim(animationTime));
        leg->AnimateLocalTranslation(
            Boss::BuildLimbFallOffTranslationAnim(animationTime, (fallLeft ? -1 : 1) * 6 * GothicRomanticBoss::BOTTOM_POINT_WIDTH, 
            -5*GothicRomanticBoss::LEG_HEIGHT - this->boss->alivePartsRoot->GetTranslationPt2D()[1]));
        leg->AnimateLocalZRotation(Boss::BuildLimbFallOffZRotationAnim(animationTime, fallLeft ? 1080.0f : -1080.0f));
    }
}

void IceBallAI::SetState(GothicRomanticBossAI::AIState newState) {

    switch (newState) {
        case BasicMoveAndShootState:
            // NOTE: nextAtkAIState and nextMovePos must be set
            this->shootCountdown = this->GenerateShootCountdownAmtForMoving();
            this->SetMoveToTargetPosition(this->boss->alivePartsRoot->GetTranslationPt2D(), 
                this->GetConfinedBossCenterMovePosition(this->nextMovePos));
            break;

        case SpinLaserAttackAIState: {
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->shootCountdown = this->GenerateShootCountdownAmtForSpinning();
            double laserSpinTime = this->GetLaserSpinTime();
            this->laserSpinRotAnim.SetLerp(0.0, laserSpinTime, 0.0f, DEFAULT_LASER_SPIN_DEGREES_PER_SEC * laserSpinTime);
            this->attacksSinceLastSummon++;
            break;
        }

        case RocketToPaddleAttackAIState:
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->numRocketsToFire  = this->GenerateNumRocketsToFire();
            this->rocketSpinRotAnim = this->GenerateRocketSpinAnimation(this->numRocketsToFire);
            this->shootCountdown    = this->GenerateTimeBetweenRockets();
            this->attacksSinceLastSummon++;
            break;
        
        case RocketToCannonAndPaddleAttackAIState:
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->numRocketsToFire  = this->GenerateNumRocketsToFire();
            this->numRocketsToFireAtPaddle = this->numRocketsToFire;
            this->rocketSpinRotAnim = this->GenerateRocketSpinAnimation(this->numRocketsToFire);
            this->shootCountdown = this->GenerateTimeBetweenRockets();
            GothicRomanticBossAI::GenerateRocketTargetCannonPositions(this->numRocketsToFire, this->rocketTargetPositions);
            this->attacksSinceLastSummon++;
            break;

        case RocketToCannonAttackAIState:
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->attacksSinceLastSummon++;
            this->numRocketsToFireAtPaddle = 0;
            this->numRocketsToFire = this->numRocketsToFireAtPaddle = this->GenerateNumRocketsToFire();
            this->rocketSpinRotAnim = this->GenerateRocketSpinAnimation(this->numRocketsToFire);
            this->shootCountdown = this->GenerateTimeBetweenRockets();
            GothicRomanticBossAI::GenerateRocketTargetCannonPositions(this->numRocketsToFire, this->rocketTargetPositions);
            break;

        case SummonItemsAIState:
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->attacksSinceLastSummon = 0;

            this->summonItemsDelayCountdown = GothicRomanticBoss::DELAY_BEFORE_SUMMONING_ITEMS_IN_SECS;
            
            // EVENT: Summon items power charge
            this->DoSummonEffect(this->summonItemsDelayCountdown);

            this->itemSummoningSoundID = this->boss->GetGameModel()->GetSound()->PlaySound(GameSound::GothicBossSummonItemChargeEvent, false);

            break;

        case GlitchAIState:
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->glitchSummonCountdown = GLITCH_TIME_IN_SECS / 1.5;
            this->glitchShakeAnim.ResetToStart();
            // EVENT: Boss effect for glitch (electricity spasm)
            GameEventManager::Instance()->ActionBossEffect(ElectricitySpasmEffectInfo(this->boss->GetBody(), 1.2f * GLITCH_TIME_IN_SECS, Colour(1,1,1)));
            break;

        case HurtBottomAIState:
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->boss->alivePartsRoot->AnimateColourRGBA(Boss::BuildBossHurtAndInvulnerableColourAnim(
                BossWeakpoint::DEFAULT_INVULNERABLE_TIME_IN_SECS));
            this->hitOnBottomMoveUpAnim.ResetToStart();

            // Stop any summoning sound that might be happening
            this->boss->GetGameModel()->GetSound()->StopSound(this->itemSummoningSoundID, 0.25);
            this->itemSummoningSoundID = INVALID_SOUND_ID;

            break;

        case VeryHurtAndAngryAIState:
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);

            this->boss->alivePartsRoot->AnimateColourRGBA(Boss::BuildBossAngryFlashAnim());
            this->angryMoveAnim.ResetToStart();

            // EVENT: Boss is angry! Rawr.
            GameEventManager::Instance()->ActionBossAngry(this->boss, this->boss->GetBody());
            break;

        case MoveToCenterAIState: {
            this->nextMovePos = GothicRomanticBossAI::Center;
            this->SetMoveToTargetPosition(this->boss->alivePartsRoot->GetTranslationPt2D(), 
                this->GetConfinedBossCenterMovePosition(GothicRomanticBossAI::Center));

            GameSound* sound = this->boss->GetGameModel()->GetSound();
            // Stop the current boss background music
            sound->StopAllSoundsWithType(GameSound::BossBackgroundLoop, 1.0);
            // ... and play the transition sound in a loop
            sound->PlaySound(GameSound::BossBackgroundLoopTransitionSingleHitEvent, false, false);
            this->transitionSoundID = sound->PlaySound(GameSound::BossBackgroundLoopTransition, true, false);
            
            break;
        }

        case DestroyConfinesAIState: {
            this->spinningDestroyConfinesAnim.ResetToStart();

            this->chargeDestroyerBlastCountdown = TIME_TO_CHARGE_CONFINE_DESTROYER_BLAST;
            this->destroyConfinesCountdown      = this->chargeDestroyerBlastCountdown + TIME_BEFORE_DESTROY_CONFINES_FLASH;
            assert(this->chargeDestroyerBlastCountdown < this->destroyConfinesCountdown);
            assert(this->destroyConfinesCountdown <= this->spinningDestroyConfinesAnim.GetFinalTime());

            // EVENT: Huge charge-up for the destruction of the confines
            EnumBossEffectInfo chargeEffect(EnumBossEffectInfo::GothicRomanticBossDestroyCharge);
            chargeEffect.SetTimeInSecs(TIME_TO_CHARGE_CONFINE_DESTROYER_BLAST);
            chargeEffect.SetBodyPart(this->boss->GetBody());
            chargeEffect.SetSize1D(GothicRomanticBoss::BODY_HEIGHT);

            GameEventManager::Instance()->ActionBossEffect(chargeEffect);

            // Play sound for charging up to destroy the confines...
            this->boss->GetGameModel()->GetSound()->PlaySound(GameSound::GothicBossChargeShockwaveEvent, false);

            break;
        }

        default:
            assert(false);
            return;
    }

    this->currState = newState;
}

void IceBallAI::UpdateState(double dT, GameModel* gameModel) {

    switch (this->currState) {
        case BasicMoveAndShootState:
            this->ExecuteBasicMoveAndShootState(dT, gameModel);
            break;

        case SpinLaserAttackAIState:
            this->ExecuteSpinLaserAttackState(dT, gameModel);
            break;

        case RocketToPaddleAttackAIState:
            this->ExecuteRocketToPaddleAttackState(dT, gameModel);
            break;
        
        case RocketToCannonAndPaddleAttackAIState:
            this->ExecuteRocketToCannonAndPaddleAttackState(dT, gameModel);
            break;

        case RocketToCannonAttackAIState:
            this->ExecuteRocketToCannonAttackState(dT, gameModel);
            break;

        case SummonItemsAIState:
            this->ExecuteSummonItemsState(dT, gameModel);
            break;

        case GlitchAIState:
            this->ExecuteGlitchState(dT, gameModel);
            break;

        case HurtBottomAIState:
            this->ExecuteHurtBottomState(dT, gameModel);
            break;

        case VeryHurtAndAngryAIState:
            this->ExecuteVeryHurtAndAngryState(dT, gameModel);
            break;

        case MoveToCenterAIState:
            this->ExecuteMoveToCenterState(dT, gameModel);
            break;

        case DestroyConfinesAIState:
            this->ExecuteDestroyConfinesState(dT, gameModel);
            break;

        default:
            assert(false);
            return;
    }
}

void IceBallAI::SetupNextAttackStateAndMove(GameModel* gameModel) {
    // Check to see if we should go into a glitch state:
    // We do this when the boss has just summoned the special item required to solve the puzzle
    // to hurt it, and it hasn't just been in a glitch state... also see if the game is in a suitable state for items to be dropped...
    if (this->summonsSinceLastSpecialItemDrop == 0 &&
        this->currState == GothicRomanticBossAI::SummonItemsAIState &&
        gameModel->GetCurrentStateType() == GameState::BallInPlayStateType) {

        // Do the glitch...
        this->SetState(GothicRomanticBossAI::GlitchAIState);
        return;
    }

    ConfinedAI::SetupNextAttackStateAndMove(gameModel);
}

float IceBallAI::GetTotalLifePercent() const {
    return this->bottomPointWeakpt->GetCurrentLifePercentage();
}

void IceBallAI::ExecuteRocketToPaddleAttackState(double dT, GameModel* gameModel) {
    assert(gameModel != NULL);

    // Shoot the rockets as the timer ticks down...
    if (this->numRocketsToFireAtPaddle > 0) {
        if (this->shootCountdown <= 0.0) {
            
            // SHOOT ze missile at ze paddle!
            this->ShootRocketAtPaddle(gameModel);
            this->shootCountdown = this->GenerateTimeBetweenRockets();
            this->numRocketsToFireAtPaddle--;

        }
        else {
            this->shootCountdown -= dT;
        }
    }
    
    bool isFinished = this->rocketSpinRotAnim.Tick(dT);
    if (isFinished) {
        // Make sure we reset the rotation!!!
        this->boss->alivePartsRoot->SetLocalYRotation(0.0f);

        // Move somewhere else...
        this->SetupNextAttackStateAndMove(gameModel);
        return;
    }

    this->boss->alivePartsRoot->SetLocalYRotation(this->rocketSpinRotAnim.GetInterpolantValue());
}

void IceBallAI::ExecuteRocketToCannonAndPaddleAttackState(double dT, GameModel* gameModel) {
    // Shoot the rockets as the timer ticks down...
    if (!this->rocketTargetPositions.empty()) {
        if (this->shootCountdown <= 0.0) {
            
            // SHOOT the missiles: 1 at a cannon and the other at the paddle...
            Point2D rocketTarget = this->rocketTargetPositions.back();
            this->rocketTargetPositions.pop_back();

            this->ShootRocket(rocketTarget, gameModel);
            this->ShootRocketAtPaddle(gameModel);

            this->shootCountdown = this->GenerateTimeBetweenRockets();
        }
        else {
            this->shootCountdown -= dT;
        }
    }

    bool isFinished = this->rocketSpinRotAnim.Tick(dT);
    if (isFinished) {
        // Make sure we reset the rotation!!!
        this->boss->alivePartsRoot->SetLocalYRotation(0.0f);

        // Move somewhere else...
        this->SetupNextAttackStateAndMove(gameModel);
        return;
    }

    this->boss->alivePartsRoot->SetLocalYRotation(this->rocketSpinRotAnim.GetInterpolantValue());
}

void IceBallAI::ExecuteGlitchState(double dT, GameModel* gameModel) {

    if (this->glitchSummonCountdown <= 0.0) {

        // Summon the laser item for the paddle - this will help to defeat the boss when
        // coupled with the ice ball

        const GameLevel* level = gameModel->GetCurrentLevel();
        float midX = (level->GetMaxXPaddleBoundPiece(0)->GetCenter()[0] - level->GetMinXPaddleBoundPiece(0)->GetCenter()[0]) / 2.0f;

        // Generate a fairly item drop position that is random and reasonably close to or at the center of the level...
        static const float X_POS_VARIATION = 3*LevelPiece::PIECE_WIDTH;
        
        Point2D dropPos(midX + Randomizer::GetInstance()->RandomNumNegOneToOne() * X_POS_VARIATION, GothicRomanticBossAI::DEFAULT_DROP_Y_POS);
        gameModel->AddItemDrop(dropPos, GameItem::LaserBulletPaddleItem);
        
        // EVENT: Effect the item being magically spawned
        GameEventManager::Instance()->ActionBossEffect(ShockwaveEffectInfo(dropPos, GameItem::ITEM_WIDTH, 1.0));
        GameEventManager::Instance()->ActionBossEffect(PuffOfSmokeEffectInfo(dropPos, 0.5f*GameItem::ITEM_WIDTH, Colour(1,1,1)));

        this->glitchSummonCountdown = GLITCH_TIME_IN_SECS + 1; // Make sure we only summon once in the glitch state
    }
    else {
        this->glitchSummonCountdown -= dT;
    }

    // Play the glitch sound at the start of the animation...
    GameSound* sound = gameModel->GetSound();
    if (this->glitchShakeAnim.GetCurrentTimeValue() == 0.0) {
        sound->AttachAndPlaySound(this->boss->GetBody(), GameSound::BossElectricitySpasmLoop, false, 
            gameModel->GetCurrentLevelTranslation());
    }

    bool isFinished = this->glitchShakeAnim.Tick(dT);
    if (isFinished) {
        sound->DetachAndStopSound(this->boss->GetBody(), GameSound::BossElectricitySpasmLoop);
        this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0.0f, 0.0f, 0.0f));
        this->attacksSinceLastSummon = 0;
        this->SetupNextAttackStateAndMove(gameModel);
        return;
    }

    this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(this->glitchShakeAnim.GetInterpolantValue(), 0.0f));
}

void IceBallAI::ExecuteHurtBottomState(double dT, GameModel* gameModel) {
    UNUSED_PARAMETER(gameModel);

    this->currVel    = Vector2D(0,0);
    this->desiredVel = Vector2D(0,0);

    bool isFinished = true;
    isFinished &= this->hitOnBottomMoveUpAnim.Tick(dT);
    isFinished &= this->hitOnBottomTiltAnim.Tick(dT);

    // Animate the boss so that it looks like it got hurt
    this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0, this->hitOnBottomMoveUpAnim.GetInterpolantValue(), 0));
    this->boss->alivePartsRoot->SetLocalZRotation(this->hitOnBottomTiltAnim.GetInterpolantValue());

    if (isFinished) {
        // Be sure to reset the local transforms of the boss
        this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0,0,0));
        this->boss->alivePartsRoot->SetLocalZRotation(0.0f);

        if (this->bottomPointWeakpt->GetIsDestroyed()) {
            this->SetState(GothicRomanticBossAI::VeryHurtAndAngryAIState);
        }
        else {
            this->SetupNextAttackStateAndMove(gameModel);
        }
    }
}

// This is called when the life of the weakpoint has been exhausted
void IceBallAI::ExecuteVeryHurtAndAngryState(double dT, GameModel* gameModel) {
    UNUSED_PARAMETER(gameModel);

    assert(this->bottomPointWeakpt->GetIsDestroyed());

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

        // Move the boss to the center in prep for blowing up the confines
        this->SetState(GothicRomanticBossAI::MoveToCenterAIState);
    }
}

void IceBallAI::ExecuteMoveToCenterState(double dT, GameModel* gameModel) {
    UNUSED_PARAMETER(dT);
    UNUSED_PARAMETER(gameModel);

    if (this->MoveToTargetPosition(this->GetSlowestConfinedMoveSpeed())) {

        // The final state for this high-level class state is the one where the boss blows up the
        // its confines (all of the blocks that surround it and keep it 'safe')
        this->SetState(GothicRomanticBossAI::DestroyConfinesAIState);
        return;
    }
}

void IceBallAI::ExecuteDestroyConfinesState(double dT, GameModel* gameModel) {
    
    if (this->chargeDestroyerBlastCountdown <= 0.0) {
        // EVENT: Huge energy wave that will disintegrate the confines
        GameEventManager::Instance()->ActionBossEffect(ShockwaveEffectInfo(
            this->boss->alivePartsRoot->GetTranslationPt2D(), 1.75f*GothicRomanticBoss::BODY_HEIGHT, 
            2.0*TIME_BEFORE_DESTROY_CONFINES_FLASH));
        GameEventManager::Instance()->ActionBossEffect(ExpandingHaloEffectInfo(
            this->boss->GetBody(), 2.0*TIME_BEFORE_DESTROY_CONFINES_FLASH, Colour(1,1,1), 8));

        // Play a sound for the shockwave
        GameSound* sound = gameModel->GetSound();
        assert(sound != NULL);
        sound->PlaySound(GameSound::GothicBossMassiveShockwaveEvent, false, false);

        // Completely blow-off all of the legs...
        this->BlowUpLegs();

        // Make sure this doesn't happen again
        this->chargeDestroyerBlastCountdown = DESTROY_CONFINES_TIME_IN_SECS + 1;
    }
    else {
        this->chargeDestroyerBlastCountdown -= dT;
    }

    if (this->destroyConfinesCountdown <= 0.0) {
        // Destroy the confine blocks...
        GameLevel* level = gameModel->GetCurrentLevel();
        
        // EVENT: Big flash for the destruction...
        GameEventManager::Instance()->ActionBossEffect(FullscreenFlashEffectInfo(0.66, 1.0f));

        // Top row of confine blocks
        for (int i = 0; i < 13; i++) {
            LevelPiece* confinePiece = level->GetLevelPieceFromCurrentLayout(25, 4 + i);
            assert(confinePiece != NULL);
            assert(confinePiece->GetType() == LevelPiece::OneWay || confinePiece->GetType() == LevelPiece::NoEntry);
            confinePiece->Destroy(gameModel, LevelPiece::DisintegrationDestruction);
        }
        // Left column of confine blocks
        for (int i = 0; i < 13; i++) {
            LevelPiece* confinePiece = level->GetLevelPieceFromCurrentLayout(24 - i, 4);
            assert(confinePiece != NULL);
            assert(confinePiece->GetType() == LevelPiece::OneWay || confinePiece->GetType() == LevelPiece::NoEntry);
            confinePiece->Destroy(gameModel, LevelPiece::DisintegrationDestruction);
        }
        // Right column of confine blocks
        for (int i = 0; i < 13; i++) {
            LevelPiece* confinePiece = level->GetLevelPieceFromCurrentLayout(24 - i, 16);
            assert(confinePiece != NULL);
            assert(confinePiece->GetType() == LevelPiece::OneWay || confinePiece->GetType() == LevelPiece::NoEntry);
            confinePiece->Destroy(gameModel, LevelPiece::DisintegrationDestruction);
        }
        // Bottom row of confine blocks
        for (int i = 0; i < 13; i++) {
            LevelPiece* confinePiece = level->GetLevelPieceFromCurrentLayout(11, 4 + i);
            assert(confinePiece != NULL);
            assert(confinePiece->GetType() == LevelPiece::OneWay || confinePiece->GetType() == LevelPiece::NoEntry);
            confinePiece->Destroy(gameModel, LevelPiece::DisintegrationDestruction);
        }

        // Make sure this doesn't happen again
        this->destroyConfinesCountdown = DESTROY_CONFINES_TIME_IN_SECS + 1;
    }
    else {
        this->destroyConfinesCountdown -= dT;
    }

    bool isFinished = this->spinningDestroyConfinesAnim.Tick(dT);
    if (isFinished) {
        this->boss->alivePartsRoot->SetLocalYRotation(0.0);
        
        this->boss->GetGameModel()->GetSound()->StopSound(this->transitionSoundID, 3.0);
        this->transitionSoundID = INVALID_SOUND_ID;

        // Go to the next high-level AI state
        this->boss->SetNextAIState(new FreeMovingAttackAI(this->boss));
    }

    this->boss->alivePartsRoot->SetLocalYRotation(this->spinningDestroyConfinesAnim.GetInterpolantValue());
}

// End IceBallAI ********************************************************************************


// Begin FreeMovingAttackAI *********************************************************************

const float FreeMovingAttackAI::BODY_LIFE_POINTS = 500.0f;
const float FreeMovingAttackAI::MAX_SPEED        = PlayerPaddle::DEFAULT_MAX_SPEED / 1.75f;

FreeMovingAttackAI::FreeMovingAttackAI(GothicRomanticBoss* boss) : 
GothicRomanticBossAI(boss), bodyWeakpt(NULL), numItemsPerSummoning(6), glitchCounter(0) {

    this->currVel = Vector2D(0,0);
    this->desiredVel = Vector2D(0,0);

    // The whole body is now a weakpoint
    boss->ConvertAliveBodyPartToWeakpoint(boss->bodyIdx,
        FreeMovingAttackAI::BODY_LIFE_POINTS, FreeMovingAttackAI::BODY_LIFE_POINTS / static_cast<float>(NUM_BALL_HITS));
    assert(dynamic_cast<BossWeakpoint*>(boss->bodyParts[this->boss->bodyIdx]) != NULL);
    this->bodyWeakpt = static_cast<BossWeakpoint*>(boss->bodyParts[this->boss->bodyIdx]);

    // Setup animations
    this->glitchShakeAnim = GothicRomanticBossAI::GenerateGlitchShakeAnimation();

    // The boss will twirl around throughout this state...
    this->twirlAnim.SetLerp(0.0, 1.5, 0.0f, 360.0f);
    this->twirlAnim.SetInterpolantValue(0.0f);
    this->twirlAnim.SetRepeat(true);

    // Start by just moving and shooting
    this->SetState(GothicRomanticBossAI::BasicMoveAndShootState);

    // Play the faster background music
    boss->GetGameModel()->GetSound()->PlaySound(GameSound::BossAngryBackgroundLoop, true);
}

FreeMovingAttackAI::~FreeMovingAttackAI() {
}

bool FreeMovingAttackAI::IsStateMachineFinished() const {
    return (this->currState == GothicRomanticBossAI::FinalDeathThroesAIState) ||
           (this->currState == GothicRomanticBossAI::HurtBodyAIState && this->bodyWeakpt->GetIsDestroyed());
}

float FreeMovingAttackAI::GetAccelerationMagnitude() const {
    return GothicRomanticBoss::DEFAULT_ACCELERATION;
}

void FreeMovingAttackAI::SetNextAttackState() {
    if (this->attacksSinceLastSummon == 0 && this->currState == GothicRomanticBossAI::SummonItemsAIState) {
        if (this->glitchCounter % 2 == 1) {
            this->SetState(GothicRomanticBossAI::GlitchAIState);
            return;
        }
        this->glitchCounter++;
    }

    float summonProb = this->GenerateSummonProbability();
    if (Randomizer::GetInstance()->RandomNumZeroToOne() <= summonProb) {
        this->SetState(GothicRomanticBossAI::SummonItemsAIState);
    }
    else {
        if (this->currState == GothicRomanticBossAI::BasicMoveAndShootState ||
            Randomizer::GetInstance()->RandomUnsignedInt() % 3 == 0) {
            this->SetState(GothicRomanticBossAI::RocketToCannonAndPaddleAttackAIState);
        }
        else {
            this->SetState(GothicRomanticBossAI::BasicMoveAndShootState);
        }
    }
}

void FreeMovingAttackAI::DoCollisionOccurredStuff(const Point2D& collisionPos) {
    // If the boss is currently in a hurt state then ignore the collision
    if (this->currState == GothicRomanticBossAI::HurtBodyAIState) {
        return;
    }

    // Make sure the body is not oddly transformed locally
    this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0.0f, 0.0f, 0.0f));

    // Apply animations if the bosses' body has been hurt but not destroyed
    if (!this->bodyWeakpt->GetIsDestroyed()) {
        this->bodyWeakpt->AnimateColourRGBA(Boss::BuildBossHurtAndInvulnerableColourAnim(
            BossWeakpoint::DEFAULT_INVULNERABLE_TIME_IN_SECS));

        Vector2D collisionToBodyVec = this->bodyWeakpt->GetTranslationPt2D() - collisionPos;
        collisionToBodyVec.Normalize();
        this->hurtMoveAnim = Boss::BuildBossHurtMoveAnim(collisionToBodyVec, GothicRomanticBoss::BODY_WIDTH,
            BossWeakpoint::DEFAULT_INVULNERABLE_TIME_IN_SECS);
    }

    this->SetState(GothicRomanticBossAI::HurtBodyAIState);
}

void FreeMovingAttackAI::SetState(GothicRomanticBossAI::AIState newState) {
    switch (newState) {

        case BasicMoveAndShootState:
            this->UpdateBasicMovement();
            this->moveToNextStateCountdown = this->GenerateBasicMoveAndShootTime();
            this->shootCountdown = this->GenerateTimeBetweenLaserShots();
            this->attacksSinceLastSummon++;
            break;

        case SummonItemsAIState: {
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->attacksSinceLastSummon = 0;
            this->summonItemsDelayCountdown = GothicRomanticBoss::DELAY_BEFORE_SUMMONING_ITEMS_IN_SECS / 1.5;

            // EVENT: Summon items power charge
            this->DoSummonEffect(this->summonItemsDelayCountdown);

            this->itemSummoningSoundID = this->boss->GetGameModel()->GetSound()->PlaySound(GameSound::GothicBossSummonItemChargeEvent, false);
            break;
        }

        case RocketToCannonAndPaddleAttackAIState:
            this->UpdateBasicMovement();
            this->numRocketsToFire  = this->GenerateNumRocketsToFire();
            this->shootCountdown = this->GenerateTimeBetweenRockets();
            GothicRomanticBossAI::GenerateRocketTargetCannonPositions(this->numRocketsToFire, this->rocketTargetPositions);
            this->attacksSinceLastSummon++;
            break;

        case GlitchAIState:
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->glitchSummonCountdown = GLITCH_TIME_IN_SECS / 1.5;
            this->glitchShakeAnim.ResetToStart();
            // EVENT: Boss effect for glitch (electricity spasm)
            GameEventManager::Instance()->ActionBossEffect(ElectricitySpasmEffectInfo(this->boss->GetBody(), GLITCH_TIME_IN_SECS, Colour(1,1,1)));
            break;

        case HurtBodyAIState:
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);

            // Stop any summoning sound that might be happening
            this->boss->GetGameModel()->GetSound()->StopSound(this->itemSummoningSoundID, 0.25);
            this->itemSummoningSoundID = INVALID_SOUND_ID;

            break;

        case FinalDeathThroesAIState:
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->boss->alivePartsRoot->AnimateColourRGBA(Boss::BuildBossFinalDeathFlashAnim());
            break;

        default:
            assert(false);
            return;
    }

    this->currState = newState;
}

void FreeMovingAttackAI::UpdateState(double dT, GameModel* gameModel) {

    switch (this->currState) {

        case BasicMoveAndShootState:
            this->ExecuteBasicMoveAndShootState(dT, gameModel);
            break;

        case SummonItemsAIState:
            this->ExecuteSummonItemsState(dT, gameModel);
            break;

        case RocketToCannonAndPaddleAttackAIState:
            this->ExecuteRocketToCannonAndPaddleAttackState(dT, gameModel);
            break;

        case GlitchAIState:
            this->ExecuteGlitchState(dT, gameModel);
            break;
        
        case HurtBodyAIState:
            this->ExecuteHurtBodyState(dT, gameModel);
            break;

        case FinalDeathThroesAIState:
            this->ExecuteFinalDeathThroesState();
            break;

        default:
            assert(false);
            return;
    }
}

float FreeMovingAttackAI::GetTotalLifePercent() const {
    return this->bodyWeakpt->GetCurrentLifePercentage();
}

void FreeMovingAttackAI::ExecuteBasicMoveAndShootState(double dT, GameModel* gameModel) {
    this->twirlAnim.Tick(dT);
    this->bodyWeakpt->SetLocalYRotation(this->twirlAnim.GetInterpolantValue());

    GameLevel* level = gameModel->GetCurrentLevel();
    Point2D bodyPos = this->bodyWeakpt->GetTranslationPt2D();
    this->PerformBasicMovement(bodyPos, level);

    if (this->shootCountdown <= 0.0) {
        // Fire ze laser!
        this->ExecuteOrbSpray(gameModel);
        this->shootCountdown = this->GenerateTimeBetweenLaserShots();
    }
    else {
        this->shootCountdown -= dT;
    }

    if (this->moveToNextStateCountdown <= 0.0) {
        this->SetNextAttackState();
    }
    else {
        this->moveToNextStateCountdown -= dT;
    }
}

void FreeMovingAttackAI::ExecuteSummonItemsState(double dT, GameModel* gameModel) {
    this->twirlAnim.Tick(dT);
    this->bodyWeakpt->SetLocalYRotation(this->twirlAnim.GetInterpolantValue());

    // Check to see if the game is in a suitable state for items to be dropped...
    if (gameModel->GetCurrentStateType() != GameState::BallInPlayStateType) {
        
        // Cancel the summoning sound(s)
        gameModel->GetSound()->StopSound(this->itemSummoningSoundID, 0.5);

        // If the ball isn't in play then we go to another state...
        this->SetNextAttackState();
        return;
    }

    if (this->summonItemsDelayCountdown <= 0.0) {
        const GameLevel* level = gameModel->GetCurrentLevel();
        assert(level != NULL);

        // Summon the items...
        // Figure out where to drop the items that will be summoned
        std::vector<Point2D> itemDropPositions;
        GothicRomanticBossAI::GetItemDropPositions(this->numItemsPerSummoning, *level, itemDropPositions);

        // Generate a random set of items with the possibility of dropping the fireball (which is used to
        // hurt the boss at this stage) -- the allowable drops have been custom tailored to ensure they are all awful for the player
        const std::vector<GameItem::ItemType>& allowableItemDrops = level->GetAllowableItemDropTypes();
        std::vector<GameItem::ItemType> dropTypes;
        dropTypes.reserve(this->numItemsPerSummoning);
        for (int i = 0; i < this->numItemsPerSummoning; i++) {
            dropTypes.push_back(allowableItemDrops[Randomizer::GetInstance()->RandomUnsignedInt() % allowableItemDrops.size()]);
        }

        assert(static_cast<int>(itemDropPositions.size()) == this->numItemsPerSummoning);
        assert(dropTypes.size() == itemDropPositions.size());

        for (int i = 0; i < this->numItemsPerSummoning; i++) {
            gameModel->AddItemDrop(itemDropPositions[i], dropTypes[i]);

            // EVENT: Effects for each of the items being magically spawned
            GameEventManager::Instance()->ActionBossEffect(ShockwaveEffectInfo(itemDropPositions[i], GameItem::ITEM_WIDTH, 1.0));
            GameEventManager::Instance()->ActionBossEffect(PuffOfSmokeEffectInfo(itemDropPositions[i], 0.5f*GameItem::ITEM_WIDTH, Colour(1,1,1)));
        }

        // EVENT: Effects for the boss itself
        GameEventManager::Instance()->ActionBossEffect(SparkBurstEffectInfo(this->boss->GetBody(), 1.0, Colour(0.0f, 0.0f, 0.0f)));

        // Go to the next state...
        this->SetNextAttackState();
    }
    else {
        this->summonItemsDelayCountdown -= dT;
    }
}

void FreeMovingAttackAI::ExecuteRocketToCannonAndPaddleAttackState(double dT, GameModel* gameModel) {
    this->twirlAnim.Tick(dT);
    this->bodyWeakpt->SetLocalYRotation(this->twirlAnim.GetInterpolantValue());    

    GameLevel* level = gameModel->GetCurrentLevel();
    Point2D bodyPos = this->bodyWeakpt->GetTranslationPt2D();
    this->PerformBasicMovement(bodyPos, level);

    // Shoot the rockets as the timer ticks down...
    if (this->rocketTargetPositions.empty()) {
        this->SetNextAttackState();
    }
    else {
        if (this->shootCountdown <= 0.0) {
            
            // SHOOT the missiles: 1 at a cannon and the other at the paddle...
            Point2D rocketTarget = this->rocketTargetPositions.back();
            this->rocketTargetPositions.pop_back();

            this->ShootRocket(rocketTarget, gameModel);
            this->ShootRocketAtPaddle(gameModel);

            this->shootCountdown = this->GenerateTimeBetweenRockets();
        }
        else {
            this->shootCountdown -= dT;
        }
    }
}

void FreeMovingAttackAI::ExecuteGlitchState(double dT, GameModel* gameModel) {

    if (this->glitchSummonCountdown <= 0.0) {

        // Summon a helpful item for the paddle - this will help to defeat the boss when
        // coupled with the ice ball

        const GameLevel* level = gameModel->GetCurrentLevel();
        float midX = (level->GetMaxXPaddleBoundPiece(0)->GetCenter()[0] - level->GetMinXPaddleBoundPiece(0)->GetCenter()[0]) / 2.0f;

        // Generate an item drop position that is random and reasonably close to or at the center of the level...
        static const float X_POS_VARIATION = 3*LevelPiece::PIECE_WIDTH;
        Point2D dropPos(midX + Randomizer::GetInstance()->RandomNumNegOneToOne() * X_POS_VARIATION, GothicRomanticBossAI::DEFAULT_DROP_Y_POS);

        // Determine a random and reasonably-good item to drop
        static const GameItem::ItemType REASONABLY_GOOD_ITEMS[] = {
            GameItem::BallSlowDownItem,
            GameItem::BallGrowItem,
            GameItem::BallSafetyNetItem
        };

        gameModel->AddItemDrop(dropPos, REASONABLY_GOOD_ITEMS[Randomizer::GetInstance()->RandomUnsignedInt() % 3]);
        
        // EVENT: Effect the item being magically spawned
        GameEventManager::Instance()->ActionBossEffect(ShockwaveEffectInfo(dropPos, GameItem::ITEM_WIDTH, 1.0));
        GameEventManager::Instance()->ActionBossEffect(PuffOfSmokeEffectInfo(dropPos, 0.5f*GameItem::ITEM_WIDTH, Colour(1,1,1)));

        this->glitchSummonCountdown = GLITCH_TIME_IN_SECS + 1; // Make sure we only summon once in the glitch state
    }
    else {
        this->glitchSummonCountdown -= dT;
    }

    bool isFinished = this->glitchShakeAnim.Tick(dT);
    if (isFinished) {
        this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0.0f, 0.0f, 0.0f));
        this->attacksSinceLastSummon = 0;
        this->SetNextAttackState();
        return;
    }

    this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(this->glitchShakeAnim.GetInterpolantValue(), 0.0f));
}

void FreeMovingAttackAI::ExecuteHurtBodyState(double dT, GameModel* gameModel) {
    UNUSED_PARAMETER(gameModel);
    this->desiredVel = Vector2D(0,0);
    this->currVel = Vector2D(0,0);

    bool isFinished = this->hurtMoveAnim.Tick(dT);
    this->bodyWeakpt->SetLocalTranslation(this->hurtMoveAnim.GetInterpolantValue());

    if (isFinished) {
        this->bodyWeakpt->SetLocalTranslation(Vector3D(0,0,0));
        if (this->bodyWeakpt->GetIsDestroyed()) {
            this->SetState(GothicRomanticBossAI::FinalDeathThroesAIState);
        }
        else {
            this->SetNextAttackState();
        }
    }
}

void FreeMovingAttackAI::ExecuteFinalDeathThroesState() {
    this->currVel = Vector2D(0,0);
    this->desiredVel = Vector2D(0,0);

    // The boss is dead dead dead.
}

void FreeMovingAttackAI::PerformBasicMovement(const Point2D& pos, const GameLevel* level) {
    
    // Up-down movement
    float maxHeight = level->GetLevelUnitHeight() - 4*LevelPiece::PIECE_HEIGHT - GothicRomanticBoss::HALF_BODY_HEIGHT;
    float minHeight = 12*LevelPiece::PIECE_HEIGHT + GothicRomanticBoss::HALF_BODY_HEIGHT;
    assert(minHeight < maxHeight);

    if (pos[1] <= minHeight) {
        this->desiredVel[1] = MAX_SPEED;
    }
    else if (pos[1] >= maxHeight) {
        this->desiredVel[1] = -MAX_SPEED;
    }

    // Side-to-side movement
    float maxWidth = level->GetLevelUnitWidth() - 4.5f*LevelPiece::PIECE_WIDTH - GothicRomanticBoss::HALF_BODY_WIDTH;
    float minWidth = 4.5f*LevelPiece::PIECE_WIDTH + GothicRomanticBoss::HALF_BODY_WIDTH;
    if (pos[0] <= minWidth) {
        this->desiredVel[0] = MAX_SPEED;
    }
    else if (pos[0] >= maxWidth) {
        this->desiredVel[0] = -MAX_SPEED;
    }
}

void FreeMovingAttackAI::UpdateBasicMovement() {
    if (fabs(this->currVel[1]) < EPSILON) {
        this->desiredVel[1] = Randomizer::GetInstance()->RandomNegativeOrPositive() * MAX_SPEED;
    }
    if (fabs(this->currVel[0]) < EPSILON) {
        this->desiredVel[0] = Randomizer::GetInstance()->RandomNegativeOrPositive() * MAX_SPEED;
    }
}

void FreeMovingAttackAI::ExecuteOrbSpray(GameModel* gameModel) {
    assert(gameModel != NULL);
    
    const PlayerPaddle* paddle = gameModel->GetPlayerPaddle();
    assert(paddle != NULL);
    
    static const float ANGLE_BETWEEN_LASERS_IN_DEGS = 15.0f;

    // The boss will fire a spray of laser bullets from its eye, in the general direction of the paddle...
    Point2D shootPos  = this->boss->GetBody()->GetTranslationPt2D() - Vector2D(0.0f, GothicRomanticBoss::HALF_BODY_HEIGHT);
    Point2D paddlePos = paddle->GetCenterPosition();

    Vector2D initLaserDir = Vector2D::Normalize(paddlePos - shootPos);
    Vector2D currLaserDir = initLaserDir;

    // Middle laser shot directly towards the paddle...
    gameModel->AddProjectile(new BossOrbProjectile(shootPos, currLaserDir));

    // Right fan-out of lasers
    currLaserDir.Rotate(ANGLE_BETWEEN_LASERS_IN_DEGS);
    gameModel->AddProjectile(new BossOrbProjectile(shootPos, currLaserDir));
    currLaserDir.Rotate(ANGLE_BETWEEN_LASERS_IN_DEGS);
    gameModel->AddProjectile(new BossOrbProjectile(shootPos, currLaserDir));

    // Left fan-out of lasers
    currLaserDir = initLaserDir;
    currLaserDir.Rotate(-ANGLE_BETWEEN_LASERS_IN_DEGS);
    gameModel->AddProjectile(new BossOrbProjectile(shootPos, currLaserDir));
    currLaserDir.Rotate(-ANGLE_BETWEEN_LASERS_IN_DEGS);
    gameModel->AddProjectile(new BossOrbProjectile(shootPos, currLaserDir));

    // EVENT: Boss shot a laser out of its eye, add effects for it...
    GameEventManager::Instance()->ActionBossEffect(ExpandingHaloEffectInfo(this->boss->GetBody(), 0.5, Colour(1.0f, 0.2f, 0.2f)));
    GameEventManager::Instance()->ActionBossEffect(SparkBurstEffectInfo(this->boss->GetBody(), 0.65, Colour(1.0f, 0.2f, 0.2f)));
}

// End FreeMovingAttackAI ***********************************************************************