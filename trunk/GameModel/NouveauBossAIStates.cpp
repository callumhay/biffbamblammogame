/**
 * NouveauBossAIStates.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#include "NouveauBossAIStates.h"
#include "NouveauBoss.h"
#include "GameModel.h"
#include "BossWeakpoint.h"
#include "MineProjectile.h"
#include "BossLaserProjectile.h"
#include "PaddleLaserProjectile.h"
#include "BossLaserBeam.h"
#include "DebrisEffectInfo.h"
#include "FullscreenFlashEffectInfo.h"
#include "LaserBeamSightsEffectInfo.h"
#include "PowerChargeEffectInfo.h"

using namespace nouveaubossai;

NouveauBossAI::NouveauBossAI(NouveauBoss* boss) : BossAIState(), boss(boss), currState() {
    assert(boss != NULL);
    this->angryMoveAnim = Boss::BuildBossAngryShakeAnim(1.0f);
}

NouveauBossAI::~NouveauBossAI() {
}

Boss* NouveauBossAI::GetBoss() const {
    return this->boss;
}

Collision::AABB2D NouveauBossAI::GenerateDyingAABB() const {
    return this->boss->GetBody()->GenerateWorldAABB();
}

void NouveauBossAI::ExecuteLaserArcSpray(const Point2D& originPos, GameModel* gameModel) {
    assert(gameModel != NULL);

    const PlayerPaddle* paddle = gameModel->GetPlayerPaddle();
    assert(paddle != NULL);

    static const float ANGLE_BETWEEN_LASERS_IN_DEGS = 15.0f;

    // The boss will fire a spray of laser bullets from its eye, in the general direction of the paddle...
    Vector2D initLaserDir = Vector2D::Normalize(paddle->GetCenterPosition() - originPos);
    Vector2D currLaserDir = initLaserDir;

    // Middle laser shot directly towards the paddle...
    gameModel->AddProjectile(new BossLaserProjectile(originPos, currLaserDir));

    // Right fan-out of lasers
    currLaserDir.Rotate(ANGLE_BETWEEN_LASERS_IN_DEGS);
    gameModel->AddProjectile(new BossLaserProjectile(originPos, currLaserDir));
    currLaserDir.Rotate(ANGLE_BETWEEN_LASERS_IN_DEGS);
    gameModel->AddProjectile(new BossLaserProjectile(originPos, currLaserDir));

    // Left fan-out of lasers
    currLaserDir = initLaserDir;
    currLaserDir.Rotate(-ANGLE_BETWEEN_LASERS_IN_DEGS);
    gameModel->AddProjectile(new BossLaserProjectile(originPos, currLaserDir));
    currLaserDir.Rotate(-ANGLE_BETWEEN_LASERS_IN_DEGS);
    gameModel->AddProjectile(new BossLaserProjectile(originPos, currLaserDir));
}

Point2D NouveauBossAI::ChooseBossPositionForPlayerToHitDomeWithLasers(bool inMiddleXOfLevel) const {

    // Get the furthest laser target position...
    Point2D laserTargetPos1;
    Point2D laserTargetPos2;

    if (inMiddleXOfLevel) {
        laserTargetPos1 = Point2D(NouveauBoss::GetLevelMidX(), NouveauBoss::Y_POS_FOR_LASER_BEAMS1);
        laserTargetPos2 = Point2D(NouveauBoss::GetLevelMidX(), NouveauBoss::Y_POS_FOR_LASER_BEAMS2);
    }
    else {
        Collision::AABB2D bossMovementAABB = this->boss->GetMovementAABB();
        float randomXLoc = bossMovementAABB.GetMin()[0] + Randomizer::GetInstance()->RandomNumZeroToOne() * bossMovementAABB.GetWidth();
        laserTargetPos1 = Point2D(randomXLoc, NouveauBoss::Y_POS_FOR_LASER_BEAMS1);
        laserTargetPos2 = Point2D(randomXLoc, NouveauBoss::Y_POS_FOR_LASER_BEAMS2);
    }

    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();
    float sqrDistToTargetPos1 = (bossPos - laserTargetPos1).SqrMagnitude();
    float sqrDistToTargetPos2 = (bossPos - laserTargetPos2).SqrMagnitude();

    if (sqrDistToTargetPos1 > sqrDistToTargetPos2) {
        return laserTargetPos1;
    }
    return laserTargetPos2;
}

Point2D NouveauBossAI::ChooseTargetPosition(const Point2D& startPos) const {
    Collision::AABB2D movementAABB = this->boss->GetMovementAABB();

    BoundingLines movementBoundingLines(movementAABB);

    // Cast 4 rays from the startPos in reasonably-uniformly distributed, but random directions
    // intersect those rays with the boundaries of the movement bounding box
    std::vector<Collision::Ray2D> rays;
    rays.reserve(8);
    rays.push_back(Collision::Ray2D(startPos, Vector2D::Rotate(0.0f   + Randomizer::GetInstance()->RandomNumZeroToOne()*45.0f, Vector2D(0,1))));
    rays.push_back(Collision::Ray2D(startPos, Vector2D::Rotate(45.0f  + Randomizer::GetInstance()->RandomNumZeroToOne()*45.0f, Vector2D(0,1))));
    rays.push_back(Collision::Ray2D(startPos, Vector2D::Rotate(90.0f  + Randomizer::GetInstance()->RandomNumZeroToOne()*45.0f, Vector2D(0,1))));
    rays.push_back(Collision::Ray2D(startPos, Vector2D::Rotate(135.0f + Randomizer::GetInstance()->RandomNumZeroToOne()*45.0f, Vector2D(0,1))));
    rays.push_back(Collision::Ray2D(startPos, Vector2D::Rotate(180.0f + Randomizer::GetInstance()->RandomNumZeroToOne()*45.0f, Vector2D(0,1))));
    rays.push_back(Collision::Ray2D(startPos, Vector2D::Rotate(225.0f + Randomizer::GetInstance()->RandomNumZeroToOne()*45.0f, Vector2D(0,1))));
    rays.push_back(Collision::Ray2D(startPos, Vector2D::Rotate(270.0f + Randomizer::GetInstance()->RandomNumZeroToOne()*45.0f, Vector2D(0,1))));
    rays.push_back(Collision::Ray2D(startPos, Vector2D::Rotate(315.0f + Randomizer::GetInstance()->RandomNumZeroToOne()*45.0f, Vector2D(0,1))));

    // Intersect each of the rays with the bounding lines and store the corresponding intersection distances...
    std::vector<std::pair<float, const Collision::Ray2D*> > intersectionTRayArray;
    intersectionTRayArray.reserve(rays.size());
    float rayT;
    for (int i = 0; i < static_cast<int>(rays.size()); i++) {
        if (!movementBoundingLines.CollisionCheck(rays[i], rayT)) {
            // Handle the case where somehow the starting position was outside of the movement bounds
            // and we shot a ray away from the bounds (we DONT want to choose that ray!!)
            rayT = -1.0f;
        }
        intersectionTRayArray.push_back(std::make_pair(rayT, &rays[i]));
    }
    // Sort by the distances (Ts) from the ray origins to the edges of the movement boundaries...
    std::sort(intersectionTRayArray.begin(), intersectionTRayArray.end());

    // Choose at random from the last distances
    static const int NUM_RAYS_TO_ELIMINATE = 6;
    int randomIndex = NUM_RAYS_TO_ELIMINATE +
        Randomizer::GetInstance()->RandomUnsignedInt() % (intersectionTRayArray.size() - NUM_RAYS_TO_ELIMINATE);

    float furthestT = intersectionTRayArray[randomIndex].first;
    while (furthestT == -1 && randomIndex < static_cast<int>(intersectionTRayArray.size()) - 1) {
        randomIndex++;
        furthestT = intersectionTRayArray[randomIndex].first;
    }
    
    // Choose a point along the chosen ray from the midpoint of its intersection all the way to its intersection
    float fractT    = 0.75f * furthestT;
    float randomT   = fractT + Randomizer::GetInstance()->RandomNumZeroToOne() * (furthestT - fractT);

    const Collision::Ray2D* randomRay = intersectionTRayArray[randomIndex].second;
    return randomRay->GetPointAlongRayFromOrigin(randomT);
}

void NouveauBossAI::ShootRandomLaserBullet(GameModel* gameModel) {

    // Shoot a laser!
    if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
        this->ShootRandomBottomSphereLaserBullet(gameModel);
    }
    else {
        this->ShootRandomSideLaserBullet(gameModel);
    }
}

void NouveauBossAI::ShootRandomBottomSphereLaserBullet(GameModel* gameModel) {
    PlayerPaddle* paddle = gameModel->GetPlayerPaddle();
    assert(paddle != NULL);
    const GameLevel* currLevel = gameModel->GetCurrentLevel();
    assert(currLevel != NULL);

    // Shoot at the paddle
    Point3D shotOrigin3D = this->boss->GetBottomSphereShootPt(); 
    Point2D shotOrigin(shotOrigin3D[0], shotOrigin3D[1]);
    const Point2D& paddlePos = paddle->GetCenterPosition();
    Vector2D laserDir = paddlePos - shotOrigin;
    laserDir.Normalize();

    // If the laser is anywhere near to colliding with either of the splitter prisms then just shoot
    // at the apex of that splitter prism...
    PrismBlock* splitterPrism = NouveauBoss::GetLeftSplitterPrism(*currLevel);
    Vector2D splitterTargetDir = (splitterPrism->GetCenter() + Vector2D(0, LevelPiece::HALF_PIECE_HEIGHT)) - shotOrigin;
    splitterTargetDir.Normalize();
    if (Trig::radiansToDegrees(acos(std::max<float>(-1.0f, std::min<float>(1.0f, 
        Vector2D::Dot(splitterTargetDir, laserDir))))) <= 30.0f && IsWorthwhileShotAtSplitterPrism(splitterTargetDir)) {

            laserDir = splitterTargetDir;
    }
    else {
        splitterPrism = NouveauBoss::GetRightSplitterPrism(*currLevel);
        splitterTargetDir = (splitterPrism->GetCenter() + Vector2D(0, LevelPiece::HALF_PIECE_HEIGHT)) - shotOrigin;
        splitterTargetDir.Normalize();

        if (Trig::radiansToDegrees(acos(std::max<float>(-1.0f, std::min<float>(1.0f, 
            Vector2D::Dot(splitterTargetDir, laserDir))))) <= 30.0f  && IsWorthwhileShotAtSplitterPrism(splitterTargetDir)) {

                laserDir = splitterTargetDir;
        }
        else {
            // Rotate a small, random amount if we're shooting directly at the paddle
            laserDir.Rotate(Randomizer::GetInstance()->RandomNumNegOneToOne() * 15.0f);
        }
    }   

    gameModel->AddProjectile(new BossLaserProjectile(shotOrigin, laserDir));
}

void NouveauBossAI::ShootRandomSideLaserBullet(GameModel* gameModel) {

    // Figure out what prisms will reflect the lasers towards the paddle...
    if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
        this->ShootRandomLeftSideLaserBullet(gameModel);
    }
    else {
        this->ShootRandomRightSideLaserBullet(gameModel);
    }
}

void NouveauBossAI::ShootRandomLeftSideLaserBullet(GameModel* gameModel) {
    const GameLevel* currLevel = gameModel->GetCurrentLevel();
    assert(currLevel != NULL);
    PlayerPaddle* paddle = gameModel->GetPlayerPaddle();
    assert(paddle != NULL);

    // Shoot at a prism that gives us a decent shot at the paddle...
    Point2D shootPos;
    Vector2D laserDir, shootPosToPaddleDir;
    PrismTriangleBlock* currPrism = NULL;
    int prismIdx;
    std::list<Collision::Ray2D> collisionRays;

    // Left side prisms...
    if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
        Point3D temp = this->boss->GetLeftSideCurlShootTopPt();
        shootPos[0] = temp[0]; shootPos[1] = temp[1];
    }
    else {
        Point3D temp = this->boss->GetLeftSideCurlShootBottomPt();
        shootPos[0] = temp[0]; shootPos[1] = temp[1];
    }

    std::vector<PrismTriangleBlock*> prisms = NouveauBoss::GetBestSidePrismCandidates(*currLevel, *paddle, shootPos, true, false);
    if (!prisms.empty()) {
        // Choose random left-side prisms to shoot at 
        prismIdx = Randomizer::GetInstance()->RandomUnsignedInt() % prisms.size();
        currPrism = prisms[prismIdx];
        assert(currPrism != NULL);
        laserDir = currPrism->GetCenter() - shootPos;
        laserDir.Normalize();
        gameModel->AddProjectile(new BossLaserProjectile(shootPos, laserDir));
    }
}

void NouveauBossAI::ShootRandomRightSideLaserBullet(GameModel* gameModel) {
    const GameLevel* currLevel = gameModel->GetCurrentLevel();
    assert(currLevel != NULL);
    PlayerPaddle* paddle = gameModel->GetPlayerPaddle();
    assert(paddle != NULL);

    // Shoot at a prism that gives us a decent shot at the paddle...
    Point2D shootPos;
    Vector2D laserDir, shootPosToPaddleDir;
    PrismTriangleBlock* currPrism = NULL;
    int prismIdx;
    std::list<Collision::Ray2D> collisionRays;

    // Right side prisms...
    if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
        Point3D temp = this->boss->GetRightSideCurlShootTopPt();
        shootPos[0] = temp[0]; shootPos[1] = temp[1];
    }
    else {
        Point3D temp = this->boss->GetRightSideCurlShootBottomPt();
        shootPos[0] = temp[0]; shootPos[1] = temp[1];
    }
    std::vector<PrismTriangleBlock*> prisms = NouveauBoss::GetBestSidePrismCandidates(*currLevel, *paddle, shootPos, false, true);
    if (!prisms.empty()) {
        prismIdx = Randomizer::GetInstance()->RandomUnsignedInt() % prisms.size();
        currPrism = prisms[prismIdx];
        assert(currPrism != NULL);
        laserDir = currPrism->GetCenter() - shootPos;
        laserDir.Normalize();
        gameModel->AddProjectile(new BossLaserProjectile(shootPos, laserDir));
    }
}

// TODO: GENERALIZE THIS FOR ALL BOSSES SO THAT MOVEMENT WORKS PROPERLY FOR EVERY BOSS!!!!
/// <summary> Move the boss to the currently set target position. </summary>
/// <returns> true if the boss made it to the target position, false if the boss is still moving to the target. </returns>
bool NouveauBossAI::MoveToTargetPosition() {

    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();

    Vector2D startToTargetVec   = this->targetPosition - this->startPosition;
    Vector2D currPosToTargetVec = this->targetPosition - bossPos;

    if (currPosToTargetVec.SqrMagnitude() < 0.5f*LevelPiece::PIECE_HEIGHT ||
        Vector2D::Dot(startToTargetVec, currPosToTargetVec) <= 0.0f) {

        // Stop the boss immediately
        this->desiredVel = Vector2D(0,0);
        this->currVel    = Vector2D(0,0);
        return true;
    }

    // We're not at or near the target position yet, get the velocity to move there
    // (unless we're getting close then we want to slow down and stop)

    // With a bit of kinematics we figure out when to start stopping...
    float distToTargetToStartStopping = -this->currVel.SqrMagnitude() / (2 * -this->GetAccelerationMagnitude());
    assert(distToTargetToStartStopping >= 0.0);
    if (currPosToTargetVec.SqrMagnitude() <= distToTargetToStartStopping * distToTargetToStartStopping) {
        this->desiredVel = Vector2D(0,0);
    }
    else {
        this->desiredVel = this->GetMaxMoveSpeedBetweenTargets() * Vector2D::Normalize(currPosToTargetVec);
    }

    return false;
}

void NouveauBossAI::OnSetStateMoveToTargetStopAndShoot() {
    this->startPosition = this->targetPosition = this->boss->alivePartsRoot->GetTranslationPt2D();
    this->numLinearMovements = this->GenerateNumMovements();
    this->waitingAtTargetCountdown = 0.0;
    this->timeUntilNextLaserWhileWaitingAtTarget = 0.0;
}

void NouveauBossAI::OnSetRapidFireSweep() {
    this->rapidFireSweepLaserFireCountdown = EPSILON;
    this->totalRapidFireAngleChange = 0.0;
    this->rapidFireSweepDir = static_cast<SweepDirection>(Randomizer::GetInstance()->RandomUnsignedInt() % NumSweepDirections);
    switch (this->rapidFireSweepDir) {
        case LeftToRightDir:
            this->currRapidFireShootDir = Point2D(this->boss->GetMinXOfConfines() - 1.5f*LevelPiece::PIECE_WIDTH, 0) - this->boss->GetBottomSphereShootPt().ToPoint2D();
            break;
        case RightToLeftDir:
            this->currRapidFireShootDir = Point2D(this->boss->GetMaxXOfConfines() + 1.5f*LevelPiece::PIECE_WIDTH, 0) - this->boss->GetBottomSphereShootPt().ToPoint2D();
            break;
        default:
            assert(false);
            return;
    }

    this->currRapidFireShootDir.Normalize();
}

void NouveauBossAI::OnSetMoveToLaserTargetableLocation() {
    this->startPosition  = this->boss->alivePartsRoot->GetTranslationPt2D();
    this->targetPosition = this->ChooseBossPositionForPlayerToHitDomeWithLasers(true);
}

void NouveauBossAI::OnSetStatePrepLaserBeamAttack() {
    this->currVel    = Vector2D(0,0);
    this->desiredVel = Vector2D(0,0);

    int numLaserBeams = this->GenerateNumLaserBeamsToFire();
    this->firingLocations.push_back(CenterBottom);
    if (numLaserBeams > 1) {
        if (numLaserBeams > 2) {
            if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
                this->firingLocations.push_back(LeftSideBottom);
                this->firingLocations.push_back(RightSideBottom);
            }
            else {
                this->firingLocations.push_back(RightSideBottom);
                this->firingLocations.push_back(LeftSideBottom);
            }
        }
        else {
            this->firingLocations.push_back(Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0 ? LeftSideBottom : RightSideBottom);
        }
    }
    if (numLaserBeams > 3) {
        if (numLaserBeams > 4) {
            if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
                this->firingLocations.push_back(LeftSideTop);
                this->firingLocations.push_back(RightSideTop);
            }
            else {
                this->firingLocations.push_back(RightSideTop);
                this->firingLocations.push_back(LeftSideTop);
            }
        }
        else {
            this->firingLocations.push_back(Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0 ? LeftSideTop : RightSideTop);
        }
    }

    this->timeBetweenLaserBeamPrepsCountdown = this->GetTimeBetweenLaserBeamPreps();
    this->laserBeamRaysToFire.clear();
    this->timeBetweenLastPrepAndFireCountdown = this->GenerateTimeBetweenLaserBeamLastPrepAndFire();
}

void NouveauBossAI::OnSetStateLaserBeamAttack() {
    this->currVel    = Vector2D(0,0);
    this->desiredVel = Vector2D(0,0);

    this->countdownToNextLaserBeamFiring   = 0.0;
    this->countdownWaitForLastBeamToFinish = BossLaserBeam::BEAM_EXPIRE_TIME_IN_SECONDS + 1.5; 
}

void NouveauBossAI::ExecuteMoveToTargetStopAndShootState(double dT, GameModel* gameModel) {

    // Check to see if we're really close to the target or if we've overshot it...
    if (this->MoveToTargetPosition()) {

        // Wait at the target for a bit of time before moving on...
        if (this->waitingAtTargetCountdown <= 0.0) {
            // Decrement the number of movements
            this->numLinearMovements--;
            if (this->numLinearMovements <= 0) {

                // We're done with all the movements, go to the next state...
                this->GoToNextRandomAttackState();
            }
            else {
                // Choose a new target, continue moving between targets
                this->startPosition = this->boss->alivePartsRoot->GetTranslationPt2D();

                if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
                    this->targetPosition = this->ChooseBossPositionForPlayerToHitDomeWithLasers(false);
                }
                else {
                    this->targetPosition = this->ChooseTargetPosition(this->startPosition);
                }

                this->waitingAtTargetCountdown = this->GenerateWaitAtTargetTime();
                this->timeUntilNextLaserWhileWaitingAtTarget = 0.0;
            }
        }
        else {
            this->waitingAtTargetCountdown -= dT;

            // Shoot a whole bunch of lasers (some that will bounce off prisms) at the paddle
            if (this->timeUntilNextLaserWhileWaitingAtTarget <= 0.0) {
                this->ShootRandomLaserBullet(gameModel);
                this->timeUntilNextLaserWhileWaitingAtTarget = this->GenerateTimeBetweenLasersWhileWaitingAtTarget();
            }
            else {
                this->timeUntilNextLaserWhileWaitingAtTarget -= dT;
            }
        }
    }
}

void NouveauBossAI::ExecuteRapidFireSweepState(double dT, GameModel* gameModel) {

    // Check to see if the sweep is complete...
    if (this->totalRapidFireAngleChange > 100.0f) {
        this->GoToNextRandomAttackState();
        return;
    }

    if (this->rapidFireSweepLaserFireCountdown <= 0.0) {

        float randomDegs = Randomizer::GetInstance()->RandomNumNegOneToOne() * 30.0f;
        Vector2D currShootDir = Vector2D::Rotate(randomDegs, this->currRapidFireShootDir);

        // Fire the laser
        gameModel->AddProjectile(new BossLaserProjectile(this->boss->GetBottomSphereShootPt().ToPoint2D(), currShootDir));

        this->rapidFireSweepLaserFireCountdown = this->GenerateTimeBetweenRapidFireSweepLasers();
    }
    else {
        this->rapidFireSweepLaserFireCountdown -= dT;
    }

    float dAngle = this->GetRapidFireAngleSpeedInDegsPerSec() * dT;
    this->totalRapidFireAngleChange += dAngle;
    if (this->rapidFireSweepDir == RightToLeftDir) {
        dAngle *= -1.0f;
    }

    this->currRapidFireShootDir.Rotate(dAngle);
}

void NouveauBossAI::ExecuteMoveToLaserTargetableLocationState() {
    if (this->MoveToTargetPosition()) {        
        // Go to the next state!
        this->SetState(NouveauBossAI::PrepLaserBeamAttackAIState);
    }
}

void NouveauBossAI::ExecutePrepLaserBeamAttackState(double dT, GameModel* gameModel) {
    
    if (this->firingLocations.empty()) {
        
        // All the laser beams are prepared wait a brief amount of time and then FIRE!!!
        if (this->timeBetweenLastPrepAndFireCountdown <= 0.0) {
            // Fire ze lazor Beeamsssss!!?!
            this->SetState(NouveauBossAI::LaserBeamAttackAIState);
        }
        else {
            this->timeBetweenLastPrepAndFireCountdown -= dT;
        }
        return;
    }
    else {

        // We're in the process of calculating all of the places to fire the laser beams...
        if (this->timeBetweenLaserBeamPrepsCountdown <= 0.0) {

            // Calculate the next place where lasers will be fired...
            LaserBeamFireLocation nextFiringLocation = this->firingLocations.front();
            this->firingLocations.pop_front();

            Point2D nextFiringPt;
            Point2D nextTargetPt;
            bool isLeft = false;
            Vector2D chargeOffset;
            const BossBodyPart* firingBodyPart = NULL;

            switch (nextFiringLocation) {
                case LeftSideTop:
                    isLeft = true;
                    firingBodyPart = this->boss->GetLeftSideCurls();
                    nextFiringPt = this->boss->GetLeftSideCurlShootTopPt().ToPoint2D();
                    chargeOffset = this->boss->GetLeftSideCurlShootTopPtOffset().ToVector2D();
                    break;
                case LeftSideBottom:
                    isLeft = true;
                    firingBodyPart = this->boss->GetLeftSideCurls();
                    nextFiringPt = this->boss->GetLeftSideCurlShootBottomPt().ToPoint2D();
                    chargeOffset = this->boss->GetLeftSideCurlShootBottomPtOffset().ToVector2D();
                    break;
                case CenterBottom:
                    firingBodyPart = this->boss->GetBottomHexSphere();
                    nextFiringPt = this->boss->GetBottomSphereBeamShootPt().ToPoint2D();
                    chargeOffset = this->boss->GetBottomSphereBeamShootPtOffset().ToVector2D();
                    nextTargetPt = Point2D(nextFiringPt[0], 0.0f);
                    break;
                case RightSideTop:
                    isLeft = false;
                    firingBodyPart = this->boss->GetRightSideCurls();
                    nextFiringPt = this->boss->GetRightSideCurlShootTopPt().ToPoint2D();
                    chargeOffset = this->boss->GetRightSideCurlShootTopPtOffset().ToVector2D();
                    break;
                case RightSideBottom:
                    isLeft = false;
                    firingBodyPart = this->boss->GetRightSideCurls();
                    nextFiringPt = this->boss->GetRightSideCurlShootBottomPt().ToPoint2D();
                    chargeOffset = this->boss->GetRightSideCurlShootBottomPtOffset().ToVector2D();
                    break;
                default:
                    assert(false);
                    return;
            }
            
            if (nextFiringLocation != CenterBottom) {
                const GameLevel* currLevel = gameModel->GetCurrentLevel();
                assert(currLevel != NULL);

                std::vector<PrismTriangleBlock*> prisms = 
                    NouveauBoss::GetBestSidePrismCandidates(*currLevel, *gameModel->GetPlayerPaddle(), 
                    nextFiringPt, isLeft, !isLeft);
                
                if (prisms.empty()) {

                    // Try shooting at the splitter prisms...
                    const PrismBlock* leftSplitterPrism  = NouveauBoss::GetLeftSplitterPrism(*currLevel);
                    const PrismBlock* rightSplitterPrism = NouveauBoss::GetRightSplitterPrism(*currLevel);

                    Point2D leftPrismTop  = leftSplitterPrism->GetCenter() + Vector2D(0, LevelPiece::HALF_PIECE_HEIGHT);
                    Point2D rightPrismTop = rightSplitterPrism->GetCenter() + Vector2D(0, LevelPiece::HALF_PIECE_HEIGHT);

                    Vector2D leftPrismTopDir  = Vector2D::Normalize(leftPrismTop - nextFiringPt);
                    Vector2D rightPrismTopDir = Vector2D::Normalize(rightPrismTop - nextFiringPt);

                    bool isLeftPrismWorthShooting  = IsWorthwhileShotAtSplitterPrism(leftPrismTopDir);
                    bool isRightPrismWorthShooting = IsWorthwhileShotAtSplitterPrism(rightPrismTopDir);

                    if (isLeftPrismWorthShooting && isRightPrismWorthShooting) {
                        nextTargetPt = Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0 ? leftPrismTop : rightPrismTop;
                    }
                    else if (isLeftPrismWorthShooting) {
                        nextTargetPt = leftPrismTop;
                    }
                    else if (isRightPrismWorthShooting) {
                        nextTargetPt = rightPrismTop;
                    }
                    else {
                        // If all else fails then just fire the laser straight downwards
                        nextTargetPt = Point2D(nextFiringPt[0], 0.0f);
                    }
                }
                else {
                    // Choose a random prism on the side to shoot at...
                    const PrismTriangleBlock* chosenPrism = prisms[Randomizer::GetInstance()->RandomUnsignedInt() % prisms.size()];
                    assert(chosenPrism != NULL);
                    nextTargetPt = chosenPrism->GetCenter();
                }
            }


            // Inform the visuals of the beam that is about to be fired...
            double timeUntilFiring = this->timeBetweenLastPrepAndFireCountdown +
                (this->GetTimeBetweenLaserBeamPreps() * this->firingLocations.size()) + 
                (this->GetTimeBetweenLaserBeamFirings() * this->laserBeamRaysToFire.size());
            assert(timeUntilFiring > 0.0);

            // EVENT: Boss is setting its sights
            GameEventManager::Instance()->ActionEffect(
                LaserBeamSightsEffectInfo(nextFiringPt, nextTargetPt, timeUntilFiring));

            // EVENT: Energy gathering until the beam fires
            GameEventManager::Instance()->ActionEffect(
                PowerChargeEffectInfo(firingBodyPart, timeUntilFiring, 
                GameModelConstants::GetInstance()->BOSS_LASER_BEAM_COLOUR, 0.75f, chargeOffset));

            Vector2D nextTargetDir = Vector2D::Normalize(nextTargetPt - nextFiringPt);
            Collision::Ray2D beamRay(nextFiringPt, nextTargetDir);
            this->laserBeamRaysToFire.push_back(std::make_pair(nextFiringLocation, beamRay));

            // Keep preparing beams while there are still locations available
            if (!this->firingLocations.empty()) {
                this->timeBetweenLaserBeamPrepsCountdown = this->GetTimeBetweenLaserBeamPreps();
            }
        }
        else {
            this->timeBetweenLaserBeamPrepsCountdown -= dT;
        }
    }
}
void NouveauBossAI::ExecuteLaserBeamAttackState(double dT, GameModel* gameModel) {

    // When we've finished firing all the laser beams we wait for the final beam to stop firing and then we move on
    if (this->laserBeamRaysToFire.empty()) {
        this->countdownWaitForLastBeamToFinish -= dT;
        if (this->countdownWaitForLastBeamToFinish <= 0.0) {
            // All done shooting the laser beams, time to go to another state
            this->GoToNextRandomAttackState();
        }
        return;
    }

    if (this->countdownToNextLaserBeamFiring <= 0.0) {
        
        // Fire a laser beam from the next position on the boss!
        assert(!this->laserBeamRaysToFire.empty());
        LaserBeamFireLocation beamLoc = this->laserBeamRaysToFire.begin()->first;
        Collision::Ray2D beamRay = this->laserBeamRaysToFire.begin()->second;
        this->laserBeamRaysToFire.pop_front();
        
        float beamRadius = 0.0f;
        switch (beamLoc) {
            case LeftSideTop:
            case RightSideTop:
                beamRadius = NouveauBoss::SIDE_CURL_TOP_LASER_BEAM_RADIUS;
                break;

            case LeftSideBottom:
            case RightSideBottom:
                beamRadius = NouveauBoss::SIDE_CURL_BOTTOM_LASER_BEAM_RADIUS;
                break;
            
            case CenterBottom:
                beamRadius = NouveauBoss::BOTTOM_SPHERE_LASER_BEAM_RADIUS;
                break;

            default:
                assert(false);
                return;
        }

        BossLaserBeam* bossBeam = new BossLaserBeam(beamRay, beamRadius, gameModel);
        gameModel->AddBeam(bossBeam);
        
        // EVENT: Flash for the laser beam
        GameEventManager::Instance()->ActionEffect(FullscreenFlashEffectInfo(0.2, 0.0f));

        this->countdownToNextLaserBeamFiring = this->GetTimeBetweenLaserBeamFirings();
    }
    else {
        this->countdownToNextLaserBeamFiring -= dT;
    }
}

// SideSphereAI Functions ***********************************************************************

const float SideSphereAI::SIDE_SPHERE_LIFE_POINTS  = 10.0f;

const float SideSphereAI::MAX_MOVE_SPEED = 0.4f * PlayerPaddle::DEFAULT_MAX_SPEED;
const float SideSphereAI::DEFAULT_ACCELERATION = 0.25f * PlayerPaddle::DEFAULT_ACCELERATION;

const float SideSphereAI::RAPID_FIRE_ANGLE_CHANGE_SPEED_DEGS_PER_SECOND = 18.0f;

const double SideSphereAI::SIDE_FRILL_FADEOUT_TIME = 0.1;
const double SideSphereAI::ARM_FADE_TIME           = 2.5;

SideSphereAI::SideSphereAI(NouveauBoss* boss) : NouveauBossAI(boss), 
leftSideSphereWeakpt(NULL), rightSideSphereWeakpt(NULL),
leftSideSphereFrill(NULL), rightSideSphereFrill(NULL),
leftSideSphereHoldingArm(NULL), rightSideSphereHoldingArm(NULL),
leftSideSphereHolderCurl(NULL), rightSideSphereHolderCurl(NULL) {

    // Get pointers to the composite 'arm-like' entities that hold the weak points in this state
    assert(dynamic_cast<BossCompositeBodyPart*>(boss->bodyParts[boss->leftSideSphereHolderIdx]) != NULL);
    this->leftSideSphereHoldingArm  = static_cast<BossCompositeBodyPart*>(boss->bodyParts[boss->leftSideSphereHolderIdx]);
    assert(dynamic_cast<BossCompositeBodyPart*>(boss->bodyParts[boss->rightSideSphereHolderIdx]) != NULL);
    this->rightSideSphereHoldingArm = static_cast<BossCompositeBodyPart*>(boss->bodyParts[boss->rightSideSphereHolderIdx]);

    // Convert and keep pointers to the two side sphere weak points on the boss for this AI state
    boss->ConvertAliveBodyPartToWeakpoint(boss->leftSideSphereIdx, SideSphereAI::SIDE_SPHERE_LIFE_POINTS, SideSphereAI::SIDE_SPHERE_LIFE_POINTS);
    assert(dynamic_cast<BossWeakpoint*>(boss->bodyParts[boss->leftSideSphereIdx]) != NULL);
    this->leftSideSphereWeakpt = static_cast<BossWeakpoint*>(boss->bodyParts[boss->leftSideSphereIdx]);
    this->leftSideSphereWeakpt->SetCollisionsDisabled(true);

    boss->ConvertAliveBodyPartToWeakpoint(boss->rightSideSphereIdx, SideSphereAI::SIDE_SPHERE_LIFE_POINTS, SideSphereAI::SIDE_SPHERE_LIFE_POINTS);
    assert(dynamic_cast<BossWeakpoint*>(boss->bodyParts[boss->rightSideSphereIdx]) != NULL);
    this->rightSideSphereWeakpt = static_cast<BossWeakpoint*>(boss->bodyParts[boss->rightSideSphereIdx]);
    this->rightSideSphereWeakpt->SetCollisionsDisabled(true);

    // Get pointers to the side frills that protect the side spheres
    assert(dynamic_cast<BossBodyPart*>(boss->bodyParts[boss->leftSideSphereFrillsIdx]) != NULL);
    this->leftSideSphereFrill  = static_cast<BossBodyPart*>(boss->bodyParts[boss->leftSideSphereFrillsIdx]);
    assert(dynamic_cast<BossBodyPart*>(boss->bodyParts[boss->rightSideSphereFrillsIdx]) != NULL);
    this->rightSideSphereFrill = static_cast<BossBodyPart*>(boss->bodyParts[boss->rightSideSphereFrillsIdx]);

    // Get pointers the the side curls that form the main support of each of the 'arms'
    assert(dynamic_cast<BossBodyPart*>(boss->bodyParts[boss->leftSideHolderCurlIdx]) != NULL);
    this->leftSideSphereHolderCurl  = static_cast<BossBodyPart*>(boss->bodyParts[boss->leftSideHolderCurlIdx]);
    assert(dynamic_cast<BossBodyPart*>(boss->bodyParts[boss->rightSideHolderCurlIdx]) != NULL);
    this->rightSideSphereHolderCurl = static_cast<BossBodyPart*>(boss->bodyParts[boss->rightSideHolderCurlIdx]);

    // Setup any special animations
    {
        static const double FINAL_JITTER_TIME = 0.3;
        static const double SHAKE_INC_TIME = 0.075;
        static const int NUM_SHAKES = (BossWeakpoint::DEFAULT_INVULNERABLE_TIME_IN_SECS - FINAL_JITTER_TIME) / (2*SHAKE_INC_TIME + SHAKE_INC_TIME);

        std::vector<double> timeValues;
        timeValues.clear();
        timeValues.reserve(3 + NUM_SHAKES + 1);
        timeValues.push_back(0.0);
        timeValues.push_back(2.0 * FINAL_JITTER_TIME / 3.0);
        timeValues.push_back(FINAL_JITTER_TIME);
        for (int i = 0; i <= NUM_SHAKES*2; i++) {
            timeValues.push_back(timeValues.back() + SHAKE_INC_TIME);
        }
        assert(timeValues.back() <= BossWeakpoint::DEFAULT_INVULNERABLE_TIME_IN_SECS);

        float hurtXPos = NouveauBoss::BODY_CORE_BOTTOM_WIDTH/5.0f;
        std::vector<Vector3D> moveValues;
        moveValues.reserve(timeValues.size());
        moveValues.push_back(Vector3D(0,0,0));
        moveValues.push_back(Vector3D(hurtXPos, LevelPiece::PIECE_HEIGHT, 0.0f));
        moveValues.push_back(Vector3D(0,0,0));
        for (int i = 0; i < NUM_SHAKES; i++) {
            moveValues.push_back(Vector3D(NouveauBoss::BODY_CORE_BOTTOM_WIDTH/30.0f, 0, 0));
            moveValues.push_back(Vector3D(-NouveauBoss::BODY_CORE_BOTTOM_WIDTH/30.0f, 0, 0));
        }
        moveValues.push_back(Vector3D(0.0f, 0.0f, 0.0f));

        this->lostLeftArmAnim.SetLerp(timeValues, moveValues);
        this->lostLeftArmAnim.SetRepeat(false);

        for (int i = 0; i < static_cast<int>(moveValues.size()); i++) {
            moveValues[i][0] *= -1;
        }
        this->lostRightArmAnim.SetLerp(timeValues, moveValues);
        this->lostRightArmAnim.SetRepeat(false);
    }

    this->SetState(NouveauBossAI::MoveToTargetStopAndShootAIState);
}

SideSphereAI::~SideSphereAI() {
}

void SideSphereAI::CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart) {
    UNUSED_PARAMETER(gameModel);
    UNUSED_PARAMETER(ball);

    assert(collisionPart != NULL);

    // Do nothing if the we're still animating a previous arm loss state
    if (this->currState == NouveauBossAI::LostLeftArmAIState ||
        this->currState == NouveauBossAI::LostRightArmAIState ||
        this->currState == NouveauBossAI::BothArmsLostAIState) {
        return;
    }

    if (collisionPart == this->leftSideSphereWeakpt && this->leftSideSphereFrill->GetIsDestroyed()) {
        // The ball collided with the left side sphere and the frill protecting it has already been destroyed...
        // Destroy the left side sphere
        this->KillLeftArm();
    }
    else if (collisionPart == this->rightSideSphereWeakpt && this->rightSideSphereFrill->GetIsDestroyed()) {
        // The ball collided with the right side sphere and the frill protecting it has already been destroyed...
        // Destroy the right side sphere
        this->KillRightArm();
    }
}

void SideSphereAI::CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart) {
    assert(collisionPart != NULL);
    UNUSED_PARAMETER(gameModel);

    // Only accept laser bullets, since that's the only player-controlled 
    // projectile that can do direct damage in this boss fight
    if (projectile->GetType() != Projectile::PaddleLaserBulletProjectile) {
        return;
    }
    // Do nothing if the we're still animating a previous arm loss state
    if (this->currState == NouveauBossAI::LostLeftArmAIState ||
        this->currState == NouveauBossAI::LostRightArmAIState ||
        this->currState == NouveauBossAI::BothArmsLostAIState) {
            return;
    }

    if (collisionPart == this->leftSideSphereWeakpt && this->leftSideSphereFrill->GetIsDestroyed()) {
        // Projectile collided with the left side sphere and the frill protecting it has already been destroyed...
        // Destroy the left side sphere
        this->KillLeftArm();
    }
    else if (collisionPart == this->rightSideSphereWeakpt && this->rightSideSphereFrill->GetIsDestroyed()) {
        // Projectile collided with the right side sphere and the frill protecting it has already been destroyed...
        // Destroy the right side sphere
        this->KillRightArm();
    }
}

void SideSphereAI::MineExplosionOccurred(GameModel* gameModel, const MineProjectile* mine) {
    UNUSED_PARAMETER(gameModel);
    assert(mine != NULL);

    const Point2D& minePosition = mine->GetPosition();
    float explosionRadius       = mine->GetExplosionRadius();
    Collision::Circle2D explosionCircle(minePosition, explosionRadius);

    static const double DEBRIS_MIN_LIFE = 1.0;
    static const double DEBRIS_MAX_LIFE = 1.75;
    static const Colour DEBRIS_COLOUR   = Colour(0.5f, 0.5f, 0.5f);
    static const int NUM_DEBRIS_BITS    = 10;

    // Check to see if the explosion hit the left/right frills (if they haven't been blown up already)
    if (!this->leftSideSphereFrill->GetIsDestroyed()) {
        if (this->leftSideSphereFrill->GetWorldBounds().CollisionCheck(explosionCircle)) {
            
            // Destroy the left frill...
            this->boss->ConvertAliveBodyPartToDeadBodyPart(this->leftSideSphereFrill);
            // Quickly fade out the geometry (it will seem to disappear in the explosion)
            this->leftSideSphereFrill->SetColour(SIDE_FRILL_FADEOUT_TIME, ColourRGBA(1,1,1,0));

            // Change the boundaries under the frill to now be on the outside...
            BoundingLines tempBoundingLines = this->leftSideSphereWeakpt->GetLocalBounds();
            tempBoundingLines.SetAllBoundsInside(false);
            this->leftSideSphereWeakpt->SetLocalBounds(tempBoundingLines);

            BossBodyPart* leftSideSphereHolderCurl = static_cast<BossBodyPart*>(this->boss->bodyParts[this->boss->leftSideHolderCurlIdx]);
            tempBoundingLines = leftSideSphereHolderCurl->GetLocalBounds();
            tempBoundingLines.SetAllBoundsInside(false);
            leftSideSphereHolderCurl->SetLocalBounds(tempBoundingLines);

            // Enable collisions for the left sphere
            this->leftSideSphereWeakpt->SetCollisionsDisabled(false);
            
            // EVENT: Debris flies off the boss...
            GameEventManager::Instance()->ActionEffect(DebrisEffectInfo(this->leftSideSphereFrill, mine->GetPosition(), 
                DEBRIS_COLOUR, DEBRIS_MIN_LIFE, DEBRIS_MAX_LIFE, NUM_DEBRIS_BITS));
            GameEventManager::Instance()->ActionEffect(FullscreenFlashEffectInfo(0.2, 0.0f));

            return;
        }
    }
    if (!this->rightSideSphereFrill->GetIsDestroyed()) {
        if (this->rightSideSphereFrill->GetWorldBounds().CollisionCheck(explosionCircle)) {
            
            // Destroy the right frill...
            this->boss->ConvertAliveBodyPartToDeadBodyPart(this->rightSideSphereFrill);
            // Quickly fade out the geometry (it will seem to disappear in the explosion)
            this->rightSideSphereFrill->SetColour(SIDE_FRILL_FADEOUT_TIME, ColourRGBA(1,1,1,0));
            
            // Change the boundaries under the frill to now be on the outside...
            BoundingLines tempBoundingLines = this->rightSideSphereWeakpt->GetLocalBounds();
            tempBoundingLines.SetAllBoundsInside(false);
            this->rightSideSphereWeakpt->SetLocalBounds(tempBoundingLines);

            BossBodyPart* rightSideSphereHolderCurl = static_cast<BossBodyPart*>(this->boss->bodyParts[this->boss->rightSideHolderCurlIdx]);
            tempBoundingLines = rightSideSphereHolderCurl->GetLocalBounds();
            tempBoundingLines.SetAllBoundsInside(false);
            rightSideSphereHolderCurl->SetLocalBounds(tempBoundingLines);

            // Enable collisions for the right sphere
            this->rightSideSphereWeakpt->SetCollisionsDisabled(false);

            // EVENT: Debris flies off the boss...
            GameEventManager::Instance()->ActionEffect(DebrisEffectInfo(this->rightSideSphereFrill, mine->GetPosition(), 
                DEBRIS_COLOUR, DEBRIS_MIN_LIFE, DEBRIS_MAX_LIFE, NUM_DEBRIS_BITS));
            GameEventManager::Instance()->ActionEffect(FullscreenFlashEffectInfo(0.2, 0.0f));

            return;
        }
    }
    
    // If the frill for a sphere is blown up then the sphere can also be blown up by a mine...
    if (this->leftSideSphereFrill->GetIsDestroyed() && !this->leftSideSphereWeakpt->GetIsDestroyed()) {
        if (this->leftSideSphereWeakpt->GetWorldBounds().CollisionCheck(explosionCircle)) {

            // Destroy the entire left side sphere holder curl-thing along with all its children...
            this->KillLeftArm();
            return;
        }
    }
    if (this->rightSideSphereFrill->GetIsDestroyed() && !this->rightSideSphereWeakpt->GetIsDestroyed()) {
        if (this->rightSideSphereWeakpt->GetWorldBounds().CollisionCheck(explosionCircle)) {

            // Destroy the entire right side sphere holder curl-thing along with all its children...
            this->KillRightArm();
            return;
        }
    }
}

void SideSphereAI::KillLeftArm() {
    assert(!this->leftSideSphereHoldingArm->GetIsDestroyed());

    // Tricky bug: Make sure that the other sphere is invulnerable during the time period where the boss
    // makes itself invulnerable because it got hit, otherwise we can trigger erroneous animations in the view
    if (!this->rightSideSphereWeakpt->GetIsDestroyed()) {
        this->rightSideSphereWeakpt->SetAsInvulnerable(this->leftSideSphereWeakpt->GetInvulnerableTime());
    }

    this->boss->ConvertAliveBodyPartToDeadBodyPart(this->leftSideSphereHoldingArm);

    // Animate the left arm thingy to fade out and "fall off" the body

    // N.B., We don't want to animate the ENTIRE arm's colour or else we'll make the frills visible again!!
    this->leftSideSphereWeakpt->AnimateColourRGBA(Boss::BuildBossHurtFlashAndFadeAnim(ARM_FADE_TIME));
    this->leftSideSphereHolderCurl->AnimateColourRGBA(Boss::BuildBossHurtFlashAndFadeAnim(ARM_FADE_TIME));

    this->leftSideSphereHoldingArm->AnimateLocalTranslation(this->GenerateArmDeathTranslationAnimation(true));
    this->leftSideSphereHoldingArm->AnimateLocalZRotation(this->GenerateArmDeathRotationAnimation(true));

    // Fix up all the inside bounds that the arm used to hide...
    BossBodyPart* sideCurls = static_cast<BossBodyPart*>(this->boss->bodyParts[this->boss->leftSideCurlsIdx]);
    BoundingLines bounds = sideCurls->GetLocalBounds();
    bounds.SetAllBoundsInside(false);
    sideCurls->SetLocalBounds(bounds);

    BossBodyPart* body = static_cast<BossBodyPart*>(this->boss->bodyParts[this->boss->bodyIdx]);
    bounds = body->GetLocalBounds();
    // Left diagonal of the body is the index 1
    bounds.SetBoundInside(1, false);
    body->SetLocalBounds(bounds);

    this->SetState(NouveauBossAI::LostLeftArmAIState);
}

void SideSphereAI::KillRightArm() {
    assert(!this->rightSideSphereHoldingArm->GetIsDestroyed());

    // Tricky bug: Make sure that the other sphere is invulnerable during the time period where the boss
    // makes itself invulnerable because it got hit, otherwise we can trigger erroneous animations in the view
    if (!this->leftSideSphereWeakpt->GetIsDestroyed()) {
        this->leftSideSphereWeakpt->SetAsInvulnerable(this->rightSideSphereWeakpt->GetInvulnerableTime());
    }

    this->boss->ConvertAliveBodyPartToDeadBodyPart(this->rightSideSphereHoldingArm);

    // Animate the right arm thingy to fade out and "fall off" the body

    // N.B., We don't want to animate the ENTIRE arm's colour or else we'll make the frills visible again!!
    this->rightSideSphereWeakpt->AnimateColourRGBA(Boss::BuildBossHurtFlashAndFadeAnim(ARM_FADE_TIME));
    this->rightSideSphereHolderCurl->AnimateColourRGBA(Boss::BuildBossHurtFlashAndFadeAnim(ARM_FADE_TIME));

    this->rightSideSphereHoldingArm->AnimateLocalTranslation(this->GenerateArmDeathTranslationAnimation(false));
    this->rightSideSphereHoldingArm->AnimateLocalZRotation(this->GenerateArmDeathRotationAnimation(false));

    // Fix up all the inside bounds that the arm used to hide...
    BossBodyPart* sideCurls = static_cast<BossBodyPart*>(this->boss->bodyParts[this->boss->rightSideCurlsIdx]);
    BoundingLines bounds = sideCurls->GetLocalBounds();
    bounds.SetAllBoundsInside(false);
    sideCurls->SetLocalBounds(bounds);

    BossBodyPart* body = static_cast<BossBodyPart*>(this->boss->bodyParts[this->boss->bodyIdx]);
    bounds = body->GetLocalBounds();
    // Left diagonal of the body is the index 2
    bounds.SetBoundInside(2, false);
    body->SetLocalBounds(bounds);

    this->SetState(NouveauBossAI::LostRightArmAIState);
}

float SideSphereAI::GetTotalLifePercent() const {
    float totalLifePercent = 0.0f;
    if (this->leftSideSphereWeakpt != NULL) {
        totalLifePercent += this->leftSideSphereWeakpt->GetCurrentLifePercentage();
    }
    if (this->rightSideSphereWeakpt != NULL) {
        totalLifePercent += this->rightSideSphereWeakpt->GetCurrentLifePercentage();
    }

    totalLifePercent /= 2.0f;
    return totalLifePercent;
}

void SideSphereAI::GoToNextRandomAttackState() {
    
    int randomNum = Randomizer::GetInstance()->RandomUnsignedInt() % 10;
    switch (randomNum) {
        case 0: case 1:  {
            if (this->currState != NouveauBossAI::LaserBeamAttackAIState) {
                this->SetState(NouveauBossAI::PrepLaserBeamAttackAIState);
            }
            else {
                this->SetState(NouveauBossAI::ArcSprayFireAIState);
            }
            break;
        }

        case 2: case 3: case 4:
            if (this->currState == NouveauBossAI::ArcSprayFireAIState) {
                this->SetState(NouveauBossAI::MoveToTargetStopAndShootAIState);
            }
            else {
                this->SetState(NouveauBossAI::ArcSprayFireAIState);
            }
            break;

        case 5: case 6: case 7: case 8: case 9:
        default:
            this->SetState(NouveauBossAI::MoveToTargetStopAndShootAIState);
            break;
    }
}

void SideSphereAI::CheckForBothArmsDeadAndChangeState() {
    if (this->leftSideSphereHoldingArm->GetIsDestroyed() &&
        this->rightSideSphereHoldingArm->GetIsDestroyed()) {

        this->SetState(NouveauBossAI::BothArmsLostAIState);
    }
    else {
        // Decide on a new AI state to go to where the boss is attacking the player...
        this->GoToNextRandomAttackState();
    }
}

void SideSphereAI::SetState(NouveauBossAI::AIState newState) {

    switch (newState) {

        case MoveToTargetStopAndShootAIState:
            this->OnSetStateMoveToTargetStopAndShoot();
            break;
        
        case ArcSprayFireAIState:
            this->startPosition  = this->boss->alivePartsRoot->GetTranslationPt2D();
            this->targetPosition = this->ChooseTargetPosition(this->startPosition); 
            this->arcLaserSprayFireCountdown = EPSILON;
            break;

        case RapidFireSweepAIState:
            this->OnSetRapidFireSweep();
            break;

        case MoveToLaserTargetableLocationAIState:
            this->OnSetMoveToLaserTargetableLocation();
            break;
        case PrepLaserBeamAttackAIState:
            this->OnSetStatePrepLaserBeamAttack();
            break;
        case LaserBeamAttackAIState:
            this->OnSetStateLaserBeamAttack();
            break;

        case LostLeftArmAIState:
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->boss->alivePartsRoot->AnimateColourRGBA(Boss::BuildBossHurtAndInvulnerableColourAnim(
                BossWeakpoint::DEFAULT_INVULNERABLE_TIME_IN_SECS));
            this->lostLeftArmAnim.ResetToStart();
            break;

        case LostRightArmAIState:
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->boss->alivePartsRoot->AnimateColourRGBA(Boss::BuildBossHurtAndInvulnerableColourAnim(
                BossWeakpoint::DEFAULT_INVULNERABLE_TIME_IN_SECS));
            this->lostRightArmAnim.ResetToStart();
            break;

        case BothArmsLostAIState:
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->boss->alivePartsRoot->AnimateColourRGBA(Boss::BuildBossAngryFlashAnim());
            this->angryMoveAnim.ResetToStart();

            // EVENT: Boss is angry! Rawr.
            GameEventManager::Instance()->ActionBossAngry(this->boss, this->boss->GetBody());
            break;

        default:
            assert(false);
            return;
    }

    this->currState = newState;
}

void SideSphereAI::UpdateState(double dT, GameModel* gameModel) {

    switch (this->currState) {

        case MoveToTargetStopAndShootAIState:
            this->ExecuteMoveToTargetStopAndShootState(dT, gameModel);
            break;
        case ArcSprayFireAIState:
            this->ExecuteArcSprayFireState(dT, gameModel);
            break;
        //case RapidFireSweepAIState:
        //    this->ExecuteRapidFireSweepState(dT, gameModel);
        //    break;

        case MoveToLaserTargetableLocationAIState:
            this->ExecuteMoveToLaserTargetableLocationState();
            break;
        case PrepLaserBeamAttackAIState:
            this->ExecutePrepLaserBeamAttackState(dT, gameModel);
            break;
        case LaserBeamAttackAIState:
            this->ExecuteLaserBeamAttackState(dT, gameModel);
            break;

        case LostLeftArmAIState:
            this->ExecuteLostLeftArmState(dT);
            break;
        case LostRightArmAIState:
            this->ExecuteLostRightArmState(dT);
            break;
        case BothArmsLostAIState:
            this->ExecuteBothArmsLostState(dT);
            break;

        default:
            assert(false);
            return;
    }
}

void SideSphereAI::ExecuteArcSprayFireState(double dT, GameModel* gameModel) {
    // Shoot lasers in an arc downward at the paddle...
    if (this->arcLaserSprayFireCountdown <= 0.0) {
        this->ExecuteLaserArcSpray(this->boss->GetBottomSphereShootPt().ToPoint2D(), gameModel);
        this->arcLaserSprayFireCountdown = this->GenerateTimeBetweenArcSprayLasers();
    }
    else {
        this->arcLaserSprayFireCountdown -= dT;
    }

    if (this->MoveToTargetPosition()) {
        this->GoToNextRandomAttackState();
    }
}

void SideSphereAI::ExecuteLostLeftArmState(double dT) {
    this->currVel = Vector2D(0,0);
    this->desiredVel = Vector2D(0,0);

    bool isFinished = this->lostLeftArmAnim.Tick(dT);
    this->boss->alivePartsRoot->SetLocalTranslation(this->lostLeftArmAnim.GetInterpolantValue());
    if (isFinished) {

        // Stop any colour animation that was set for this state
        this->boss->alivePartsRoot->ResetColourRGBAAnimation();
        // Clean up all the translations on the body of the boss for this state
        this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0,0,0));

        this->CheckForBothArmsDeadAndChangeState();
    }
}

void SideSphereAI::ExecuteLostRightArmState(double dT) {
    this->currVel = Vector2D(0,0);
    this->desiredVel = Vector2D(0,0);

    bool isFinished = this->lostRightArmAnim.Tick(dT);
    this->boss->alivePartsRoot->SetLocalTranslation(this->lostRightArmAnim.GetInterpolantValue());
    if (isFinished) {

        // Stop any colour animation that was set for this state
        this->boss->alivePartsRoot->ResetColourRGBAAnimation();
        // Clean up all the translations on the body of the boss for this state
        this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0,0,0));

        this->CheckForBothArmsDeadAndChangeState();
    }
}

void SideSphereAI::ExecuteBothArmsLostState(double dT) {
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
        this->boss->SetNextAIState(new GlassDomeAI(this->boss));
    }
}

AnimationMultiLerp<Vector3D> SideSphereAI::GenerateArmDeathTranslationAnimation(bool isLeftArm) const {
    float xDist = (isLeftArm ? -1 : 1) * 3 * NouveauBoss::ARM_SPHERE_HOLDER_CURL_WIDTH;

    float yDist = -(isLeftArm ? this->leftSideSphereHolderCurl->GetTranslationPt2D()[1] : 
        this->rightSideSphereHolderCurl->GetTranslationPt2D()[1]);
    yDist -= 2*NouveauBoss::ARM_SPHERE_HOLDER_CURL_WIDTH;

    return Boss::BuildLimbFallOffTranslationAnim(ARM_FADE_TIME, xDist, yDist);
}

AnimationMultiLerp<float> SideSphereAI::GenerateArmDeathRotationAnimation(bool isLeftArm) const {
    return Boss::BuildLimbFallOffZRotationAnim(ARM_FADE_TIME, isLeftArm ? 180.0f : -180.0f);
}


// GlassDomeAI Functions *************************************************************************

const double GlassDomeAI::INVULNERABLE_TIME_IN_SECS = BossWeakpoint::DEFAULT_INVULNERABLE_TIME_IN_SECS + 0.75;

const float GlassDomeAI::TOP_SPHERE_LIFE_POINTS = PaddleLaserProjectile::DAMAGE_DEFAULT * 3;
const float GlassDomeAI::MAX_MOVE_SPEED = 0.6f * PlayerPaddle::DEFAULT_MAX_SPEED;
const float GlassDomeAI::DEFAULT_ACCELERATION = 0.35f * PlayerPaddle::DEFAULT_ACCELERATION;

const float GlassDomeAI::RAPID_FIRE_ANGLE_CHANGE_SPEED_DEGS_PER_SECOND = 22.0f;

const float GlassDomeAI::TOP_DEATH_FADE_TIME = 4.0f;

GlassDomeAI::GlassDomeAI(NouveauBoss* boss) : NouveauBossAI(boss), 
topDome(NULL), gazebo(NULL), topSphereWeakpt(NULL) {

    assert(dynamic_cast<BossBodyPart*>(this->boss->bodyParts[this->boss->domeIdx]) != NULL);
    this->topDome = static_cast<BossBodyPart*>(this->boss->bodyParts[this->boss->domeIdx]);

    assert(dynamic_cast<BossBodyPart*>(this->boss->bodyParts[this->boss->gazeboIdx]) != NULL);
    this->gazebo = static_cast<BossBodyPart*>(this->boss->bodyParts[this->boss->gazeboIdx]);

    // Convert and keep pointers to the top sphere weak point on the boss for this AI state
    boss->ConvertAliveBodyPartToWeakpoint(boss->topSphereIdx, GlassDomeAI::TOP_SPHERE_LIFE_POINTS, GlassDomeAI::TOP_SPHERE_LIFE_POINTS / 3.0f);
    assert(dynamic_cast<BossWeakpoint*>(boss->bodyParts[boss->topSphereIdx]) != NULL);
    this->topSphereWeakpt = static_cast<BossWeakpoint*>(boss->bodyParts[boss->topSphereIdx]);
    this->topSphereWeakpt->SetInvulnerableTime(INVULNERABLE_TIME_IN_SECS);

    this->topHurtAnim = Boss::BuildBossHurtMoveAnim(Vector2D(0,-1), NouveauBoss::BODY_CORE_BOTTOM_WIDTH/7.0f,
        BossWeakpoint::DEFAULT_INVULNERABLE_TIME_IN_SECS);
        
    this->SetState(NouveauBossAI::ArcSprayFireAIState);
}

GlassDomeAI::~GlassDomeAI() {
}

void GlassDomeAI::CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart) {
    UNUSED_PARAMETER(gameModel);

    // If the projectile isn't colliding with the weakpoint in this state then ignore it
    if (collisionPart != this->topSphereWeakpt) {
        return;
    }    
    // Ignore all other projectiles except the laser bullets from the paddle
    if (projectile->GetType() != Projectile::PaddleLaserBulletProjectile) {
        return;
    }
    // Do nothing if the we're still animating a previous hurt state
    if (this->currState == NouveauBossAI::HurtTopAIState ||
        this->currState == NouveauBossAI::TopLostAIState) {
        return;
    }

    // There's been a collision with the weakpoint, react to it
    if (collisionPart->GetIsDestroyed()) {

        this->boss->ConvertAliveBodyPartToDeadBodyPart(this->topDome);
        this->boss->ConvertAliveBodyPartToDeadBodyPart(this->gazebo);

        bool randomDir = Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0;

        // Blow the top dome and gazebo right off the top of the boss
        this->topDome->AnimateColourRGBA(Boss::BuildBossHurtFlashAndFadeAnim(TOP_DEATH_FADE_TIME));
        AnimationMultiLerp<Vector3D> topDomeDeathTransAnim = this->GenerateTopDeathTranslationAnimation(gameModel, randomDir);
        this->topDome->AnimateLocalTranslation(topDomeDeathTransAnim);
        AnimationMultiLerp<float> topDomeDeathRotAnim = this->GenerateTopDeathRotationAnimation();
        this->topDome->AnimateLocalZRotation(topDomeDeathRotAnim);

        this->gazebo->AnimateColourRGBA(Boss::BuildBossHurtFlashAndFadeAnim(TOP_DEATH_FADE_TIME));
        AnimationMultiLerp<Vector3D> gazeboDeathTransAnim = this->GenerateTopDeathTranslationAnimation(gameModel, !randomDir);
        this->gazebo->AnimateLocalTranslation(gazeboDeathTransAnim);
        AnimationMultiLerp<float> gazeboDeathRotAnim = this->GenerateTopDeathRotationAnimation();
        this->gazebo->AnimateLocalZRotation(gazeboDeathRotAnim);
    }

    this->SetState(NouveauBossAI::HurtTopAIState);
}

float GlassDomeAI::GetTotalLifePercent() const {
    return this->topSphereWeakpt->GetCurrentLifePercentage();
}

AnimationMultiLerp<float> GlassDomeAI::GenerateTopDeathRotationAnimation() {
    std::vector<double> timeValues;
    timeValues.reserve(2);
    timeValues.push_back(0.0);
    timeValues.push_back(TOP_DEATH_FADE_TIME);

    std::vector<float> rotationValues;
    rotationValues.reserve(timeValues.size());
    rotationValues.push_back(0.0f);
    rotationValues.push_back(Randomizer::GetInstance()->RandomNegativeOrPositive() * 1080.0f);

    AnimationMultiLerp<float> topDeathRotAnim;
    topDeathRotAnim.SetLerp(timeValues, rotationValues);
    topDeathRotAnim.SetRepeat(false);
    topDeathRotAnim.SetInterpolantValue(0.0f);

    return topDeathRotAnim;
}

AnimationMultiLerp<Vector3D> GlassDomeAI::GenerateTopDeathTranslationAnimation(GameModel* gameModel, bool randomDir) {
    const GameLevel* currLevel = gameModel->GetCurrentLevel();
    assert(currLevel != NULL);

    std::vector<double> timeValues;
    timeValues.reserve(2);
    timeValues.push_back(0.0);
    timeValues.push_back(TOP_DEATH_FADE_TIME);

    std::vector<Vector3D> moveValues;
    moveValues.reserve(timeValues.size());
    moveValues.push_back(Vector3D(0,0,0));
    moveValues.push_back(Vector3D((randomDir ? 1.0f : -1.0f) * currLevel->GetLevelUnitWidth(), 
        -this->boss->GetTopDome()->GetTranslationPt2D()[1] - 10.0f, 0.0f));

    AnimationMultiLerp<Vector3D> topDeathTransAnim;
    topDeathTransAnim.SetLerp(timeValues, moveValues);
    topDeathTransAnim.SetRepeat(false);
    topDeathTransAnim.SetInterpolantValue(Vector3D(0,0,0));

    return topDeathTransAnim;
}

void GlassDomeAI::GoToNextRandomAttackState() {

    if (this->GetTotalLifePercent() <= 0.75f) {
        if (this->GetTotalLifePercent() <= 0.5f) {
            int randomNum = Randomizer::GetInstance()->RandomUnsignedInt() % 12;
            switch (randomNum) {
                case 0: case 1:
                    this->SetState(NouveauBossAI::MoveToTargetStopAndShootAIState);
                    return;
                case 2: case 3: case 4:
                    this->SetState(NouveauBossAI::ArcSprayFireAIState);
                    return;
                case 5: case 6: case 7:
                    if (this->currState == NouveauBossAI::RapidFireSweepAIState) {
                        this->SetState(NouveauBossAI::ArcSprayFireAIState);
                    }
                    else {
                        this->SetState(NouveauBossAI::RapidFireSweepAIState);
                    }
                    return;
                case 8: case 9: case 10: case 11:
                default:
                    if (this->currState == MoveToLaserTargetableLocationAIState) {
                        this->SetState(NouveauBossAI::MoveToTargetStopAndShootAIState);
                    }
                    else {
                        this->SetState(NouveauBossAI::MoveToLaserTargetableLocationAIState);
                    }
                    return;
            }
        }
        else {
            int randomNum = Randomizer::GetInstance()->RandomUnsignedInt() % 10;
            switch (randomNum) {
                case 0: case 1: 
                    this->SetState(NouveauBossAI::MoveToTargetStopAndShootAIState);
                    return;
                case 2: case 3: case 4: 
                    this->SetState(NouveauBossAI::ArcSprayFireAIState);
                    return;
                case 5: case 6: 
                    if (this->currState == NouveauBossAI::RapidFireSweepAIState) {
                        this->SetState(NouveauBossAI::ArcSprayFireAIState);
                    }
                    else {
                        this->SetState(NouveauBossAI::RapidFireSweepAIState);
                    }
                    return;
                case 7: case 8: case 9:
                default:
                    if (this->currState == MoveToLaserTargetableLocationAIState) {
                        this->SetState(NouveauBossAI::MoveToTargetStopAndShootAIState);
                    }
                    else {
                        this->SetState(NouveauBossAI::MoveToLaserTargetableLocationAIState);
                    }
                    return;
            }
        }
    }
    else {
        int randomNum = Randomizer::GetInstance()->RandomUnsignedInt() % 11;
        switch (randomNum) {
            case 0: case 1: case 2: 
                this->SetState(NouveauBossAI::MoveToTargetStopAndShootAIState);
                return;
            case 3: case 4: case 5: case 6:
                this->SetState(NouveauBossAI::ArcSprayFireAIState);
                return;
            case 7: case 8: 
                if (this->currState == NouveauBossAI::RapidFireSweepAIState) {
                    this->SetState(NouveauBossAI::ArcSprayFireAIState);
                }
                else {
                    this->SetState(NouveauBossAI::RapidFireSweepAIState);
                }
                return;
            case 9: case 10:
            default:
                if (this->currState == MoveToLaserTargetableLocationAIState) {
                    this->SetState(NouveauBossAI::MoveToTargetStopAndShootAIState);
                }
                else {
                    this->SetState(NouveauBossAI::MoveToLaserTargetableLocationAIState);
                }
                return;
        }
    }

    // Should never get here, but just as a fail safe when modifying code, have the boss go to
    // the most 'typical' attack state
    //this->SetState(NouveauBossAI::MoveToTargetStopAndShootAIState);
}

void GlassDomeAI::SetState(NouveauBossAI::AIState newState) {
    switch (newState) {

        case MoveToTargetStopAndShootAIState:
            this->OnSetStateMoveToTargetStopAndShoot();
            break;

        case MoveToLaserTargetableLocationAIState:
            this->OnSetMoveToLaserTargetableLocation();
            break;
        case PrepLaserBeamAttackAIState:
            this->OnSetStatePrepLaserBeamAttack();
            break;
        case LaserBeamAttackAIState:
            this->OnSetStateLaserBeamAttack();
            break;

        case ArcSprayFireAIState:
            this->startPosition = this->boss->alivePartsRoot->GetTranslationPt2D();
            this->targetPosition = this->ChooseTargetPosition(this->startPosition);
            this->numLinearMovements = this->GenerateNumArcLaserFireMovements();
            this->waitingAtTargetCountdown = this->GenerateArcLaserTimeToWaitAtTarget();
            break;

        case RapidFireSweepAIState:
            this->OnSetRapidFireSweep();
            break;

        case HurtTopAIState:
            this->boss->alivePartsRoot->AnimateColourRGBA(
                Boss::BuildBossHurtAndInvulnerableColourAnim(INVULNERABLE_TIME_IN_SECS));
            this->topHurtAnim.ResetToStart();
            break;

        case TopLostAIState:
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->boss->alivePartsRoot->AnimateColourRGBA(Boss::BuildBossAngryFlashAnim());
            this->angryMoveAnim.ResetToStart();

            // EVENT: Boss is angry! Rawr.
            GameEventManager::Instance()->ActionBossAngry(this->boss, this->boss->GetBody());
            break;
        
        default:
            assert(false);
            return;
    }

    this->currState = newState;
}

void GlassDomeAI::UpdateState(double dT, GameModel* gameModel) {
    switch (this->currState) {

        case MoveToTargetStopAndShootAIState:
            this->ExecuteMoveToTargetStopAndShootState(dT, gameModel);
            break;

        case MoveToLaserTargetableLocationAIState:
            this->ExecuteMoveToLaserTargetableLocationState();
            break;
        case PrepLaserBeamAttackAIState:
            this->ExecutePrepLaserBeamAttackState(dT, gameModel);
            break;
        case LaserBeamAttackAIState:
            this->ExecuteLaserBeamAttackState(dT, gameModel);
            break;

        case ArcSprayFireAIState:
            this->ExecuteArcSprayFireState(dT, gameModel);
            break;

        case RapidFireSweepAIState:
            this->ExecuteRapidFireSweepState(dT, gameModel);
            break;

        case HurtTopAIState:
            this->ExecuteHurtTopState(dT, gameModel);
            break;

        case TopLostAIState:
            this->ExecuteTopLostState(dT, gameModel);
            break;

        default:
            assert(false);
            return;
    }
}

void GlassDomeAI::ExecuteArcSprayFireState(double dT, GameModel* gameModel) {
    
    // Check to see if we're really close to the target or if we've overshot it...
    if (this->MoveToTargetPosition()) {

        // Stop the boss immediately
        this->desiredVel = Vector2D(0,0);
        this->currVel    = Vector2D(0,0);

        // Wait at the target for a bit of time before moving on...
        if (this->waitingAtTargetCountdown <= 0.0) {
            
            // Shoot the lasers!
            this->ExecuteLaserArcSpray(this->boss->GetBottomSphereShootPt().ToPoint2D(), gameModel);

            // Decrement the number of movements
            this->numLinearMovements--;
            if (this->numLinearMovements <= 0) {

                // We're done with all the movements, go to the next state...
                this->GoToNextRandomAttackState();
            }
            else {
                // Choose a new target, continue moving between targets
                this->startPosition = this->boss->alivePartsRoot->GetTranslationPt2D();
                this->targetPosition = this->ChooseTargetPosition(this->startPosition);
                this->waitingAtTargetCountdown = this->GenerateArcLaserTimeToWaitAtTarget();
            }
        }
        else {
            this->waitingAtTargetCountdown -= dT;
        }
    }
}

void GlassDomeAI::ExecuteHurtTopState(double dT, GameModel* gameModel) {
    UNUSED_PARAMETER(gameModel);

    this->currVel = Vector2D(0,0);
    this->desiredVel = Vector2D(0,0);

    bool isFinished = this->topHurtAnim.Tick(dT);
    this->boss->alivePartsRoot->SetLocalTranslation(this->topHurtAnim.GetInterpolantValue());
    if (isFinished) {

        // Stop any colour animation that was set for this state
        this->boss->alivePartsRoot->ResetColourRGBAAnimation();
        // Clean up all the translations on the body of the boss for this state
        this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0,0,0));

        if (this->topSphereWeakpt->GetIsDestroyed()) {
            this->SetState(NouveauBossAI::TopLostAIState);
        }
        else {
            this->GoToNextRandomAttackState();
        }
    }
}

void GlassDomeAI::ExecuteTopLostState(double dT, GameModel* gameModel) {
    UNUSED_PARAMETER(gameModel);

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
        this->boss->SetNextAIState(new TopSphereAI(this->boss));
    }
}

// TopSphereAI Functions ***********************************************************************

const float TopSphereAI::TOP_SPHERE_LIFE_POINTS = PaddleLaserProjectile::DAMAGE_DEFAULT * 5;
const float TopSphereAI::TOP_SPHERE_DAMAGE_ON_HIT = TOP_SPHERE_LIFE_POINTS / 5.0f;
const float TopSphereAI::MAX_MOVE_SPEED = 0.7f * PlayerPaddle::DEFAULT_MAX_SPEED;
const float TopSphereAI::DEFAULT_ACCELERATION = 0.45f * PlayerPaddle::DEFAULT_ACCELERATION;
const float TopSphereAI::RAPID_FIRE_ANGLE_CHANGE_SPEED_DEGS_PER_SECOND = 25.0f;

TopSphereAI::TopSphereAI(NouveauBoss* boss) : NouveauBossAI(boss) {

    assert(dynamic_cast<BossWeakpoint*>(boss->bodyParts[boss->topSphereIdx]) != NULL);
    this->topSphereWeakpt = static_cast<BossWeakpoint*>(boss->bodyParts[boss->topSphereIdx]);
    this->topSphereWeakpt->ResetLife(TOP_SPHERE_LIFE_POINTS);
    this->topSphereWeakpt->ResetDamageOnBallHit(TOP_SPHERE_DAMAGE_ON_HIT);

    this->SetState(NouveauBossAI::BrutalMoveAndShootAIState);
}

TopSphereAI::~TopSphereAI() {
}

void TopSphereAI::CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart) {
    UNUSED_PARAMETER(gameModel);

    if (collisionPart != this->topSphereWeakpt) {
        return;
    }
    // Do nothing if the we're still animating a previous hurt state
    if (this->currState == NouveauBossAI::HurtTopAIState ||
        this->currState == NouveauBossAI::FinalDeathThroesAIState) {
        return;
    }
    this->TopSphereWasHurt(ball.GetCenterPosition2D());
}

void TopSphereAI::CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart) {
    UNUSED_PARAMETER(gameModel);
    if (collisionPart != this->topSphereWeakpt || projectile->GetType() != Projectile::PaddleLaserBulletProjectile) {
        return;
    }
    // Do nothing if the we're still animating a previous hurt state
    if (this->currState == NouveauBossAI::HurtTopAIState ||
        this->currState == NouveauBossAI::FinalDeathThroesAIState) {
        return;
    }
    this->TopSphereWasHurt(projectile->GetPosition());
}

void TopSphereAI::MineExplosionOccurred(GameModel* gameModel, const MineProjectile* mine) {
    UNUSED_PARAMETER(gameModel);
    assert(mine != NULL);
    
    // Do nothing if the we're still animating a previous hurt state
    if (this->currState == NouveauBossAI::HurtTopAIState ||
        this->currState == NouveauBossAI::FinalDeathThroesAIState) {
        return;
    }

    const Point2D& minePosition = mine->GetPosition();
    float explosionRadius       = mine->GetExplosionRadius();
    Collision::Circle2D explosionCircle(minePosition, explosionRadius);

    // If the mine explosion took place in the vicinity of the top sphere then damage is done to it...
    if (!this->topSphereWeakpt->GetIsDestroyed()) {
        if (this->topSphereWeakpt->GetWorldBounds().CollisionCheck(explosionCircle)) {
            this->topSphereWeakpt->Diminish(TOP_SPHERE_DAMAGE_ON_HIT);
            this->TopSphereWasHurt(minePosition);
        }
    }
}

void TopSphereAI::TopSphereWasHurt(const Point2D& impactPt) {
    Vector2D hurtDir = this->topSphereWeakpt->GetTranslationPt2D() - impactPt;
    hurtDir.Normalize();

    // Animation for getting hurt...
    this->topHurtAnim = Boss::BuildBossHurtMoveAnim(hurtDir, NouveauBoss::BODY_CORE_BOTTOM_WIDTH/7.0f,
        BossWeakpoint::DEFAULT_INVULNERABLE_TIME_IN_SECS);
    this->SetState(NouveauBossAI::HurtTopAIState);
}

bool TopSphereAI::IsStateMachineFinished() const {
    return (this->currState == NouveauBossAI::FinalDeathThroesAIState) ||
        (this->currState == NouveauBossAI::HurtTopAIState && this->topSphereWeakpt->GetIsDestroyed());
}

float TopSphereAI::GetTotalLifePercent() const {
    return this->topSphereWeakpt->GetCurrentLifePercentage();
}

void TopSphereAI::GoToNextRandomAttackState() {
    int randomNum = Randomizer::GetInstance()->RandomUnsignedInt() % 10;
    switch (randomNum) {
        case 0: case 1: case 2: case 3:
            this->SetState(NouveauBossAI::BrutalMoveAndShootAIState);
            break;
        case 4: case 5: case 6: case 7:
            this->SetState(NouveauBossAI::RapidFireSweepAIState);
            break;
        case 8: case 9:
        default:
            this->SetState(NouveauBossAI::PrepLaserBeamAttackAIState);
            break;
    }
}

void TopSphereAI::SetState(NouveauBossAI::AIState newState) {
    switch (newState) {

        case BrutalMoveAndShootAIState:
            this->OnSetStateMoveToTargetStopAndShoot();
            break;

        case MoveToLaserTargetableLocationAIState:
            this->OnSetMoveToLaserTargetableLocation();
            break;
        case PrepLaserBeamAttackAIState:
            this->OnSetStatePrepLaserBeamAttack();
            break;
        case LaserBeamAttackAIState:
            this->OnSetStateLaserBeamAttack();
            break;

        case RapidFireSweepAIState:
            this->OnSetRapidFireSweep();
            break;

        case HurtTopAIState:
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->boss->alivePartsRoot->AnimateColourRGBA(Boss::BuildBossHurtAndInvulnerableColourAnim(
                BossWeakpoint::DEFAULT_INVULNERABLE_TIME_IN_SECS));
            this->topHurtAnim.ResetToStart();
            break;

        case FinalDeathThroesAIState:
            // Final fade out for what's left of the boss
            this->boss->alivePartsRoot->AnimateColourRGBA(Boss::BuildBossFinalDeathFlashAnim());
            break;

        default:
            assert(false);
            return;
    }

    this->currState = newState;
}

void TopSphereAI::UpdateState(double dT, GameModel* gameModel) {

    switch (this->currState) {

        case BrutalMoveAndShootAIState:
            this->ExecuteBrutalMoveAndShootState(dT, gameModel);
            break;

        case MoveToLaserTargetableLocationAIState:
            this->ExecuteMoveToLaserTargetableLocationState();
            break;
        case PrepLaserBeamAttackAIState:
            this->ExecutePrepLaserBeamAttackState(dT, gameModel);
            break;
        case LaserBeamAttackAIState:
            this->ExecuteLaserBeamAttackState(dT, gameModel);
            break;

        case RapidFireSweepAIState:
            this->ExecuteRapidFireSweepState(dT, gameModel);
            break;

        case HurtTopAIState:
            this->ExecuteHurtTopState(dT, gameModel);
            break;

        case FinalDeathThroesAIState:
            this->ExecuteFinalDeathThroesState();
            break;

        default:
            assert(false);
            return;
    }
}

void TopSphereAI::ExecuteBrutalMoveAndShootState(double dT, GameModel* gameModel) {

    // Check to see if we're really close to the target or if we've overshot it...
    if (this->MoveToTargetPosition()) {

        // Wait at the target for a bit of time before moving on...
        if (this->waitingAtTargetCountdown <= 0.0) {
            // Decrement the number of movements
            this->numLinearMovements--;
            if (this->numLinearMovements <= 0) {

                // We're done with all the movements, go to the next state...
                this->GoToNextRandomAttackState();
            }
            else {
                // Choose a new target, continue moving between targets
                this->startPosition = this->boss->alivePartsRoot->GetTranslationPt2D();
                this->targetPosition = this->ChooseTargetPosition(this->startPosition);
                this->waitingAtTargetCountdown = this->GenerateWaitAtTargetTime();
                this->timeUntilNextLaserWhileWaitingAtTarget = 0.0;

                this->ExecuteLaserArcSpray(this->boss->GetBottomSphereShootPt().ToPoint2D(), gameModel);
            }
        }
        else {
            this->waitingAtTargetCountdown -= dT;

            // Shoot a whole bunch of lasers (some that will bounce off prisms) at the paddle
            if (this->timeUntilNextLaserWhileWaitingAtTarget <= 0.0) {
                
                if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
                    this->ShootRandomLaserBullet(gameModel);
                }
                else {
                    if (Randomizer::GetInstance()->RandomUnsignedInt() % 3 == 0) {
                        this->ShootRandomBottomSphereLaserBullet(gameModel);
                        this->ShootRandomLeftSideLaserBullet(gameModel);
                        this->ShootRandomRightSideLaserBullet(gameModel);
                    }
                    else {
                        this->ShootRandomBottomSphereLaserBullet(gameModel);
                        this->ShootRandomSideLaserBullet(gameModel);
                    }
                }

                this->timeUntilNextLaserWhileWaitingAtTarget = this->GenerateTimeBetweenLasersWhileWaitingAtTarget();
            }
            else {
                this->timeUntilNextLaserWhileWaitingAtTarget -= dT;
            }
        }
    }
}

void TopSphereAI::ExecuteHurtTopState(double dT, GameModel* gameModel) {
    UNUSED_PARAMETER(gameModel);

    this->currVel = Vector2D(0,0);
    this->desiredVel = Vector2D(0,0);

    bool isFinished = this->topHurtAnim.Tick(dT);
    this->boss->alivePartsRoot->SetLocalTranslation(this->topHurtAnim.GetInterpolantValue());
    if (isFinished) {

        // Stop any colour animation that was set for this state
        this->boss->alivePartsRoot->ResetColourRGBAAnimation();
        // Clean up all the translations on the body of the boss for this state
        this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0,0,0));

        if (this->topSphereWeakpt->GetIsDestroyed()) {
            this->SetState(NouveauBossAI::FinalDeathThroesAIState);
        }
        else {
            this->GoToNextRandomAttackState();
        }
    }
}

void TopSphereAI::ExecuteFinalDeathThroesState() {
    this->desiredVel = Vector2D(0,0);
    this->currVel    = Vector2D(0,0);

    // The boss is dead dead dead.
}