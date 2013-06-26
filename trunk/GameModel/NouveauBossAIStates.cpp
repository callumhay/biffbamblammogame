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
#include "BossWeakpoint.h"
#include "MineProjectile.h"
#include "GameModel.h"
#include "BossLaserProjectile.h"
#include "DebrisEffectInfo.h"
#include "FullscreenFlashEffectInfo.h"

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
    for (int i = 0; i < static_cast<int>(rays.size()); i++) {
        float rayT;
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

    // Choose a point along the chosen ray from the midpoint of its intersection all the way to its intersection
    float furthestT = intersectionTRayArray[randomIndex].first;
    float fractT    = 0.75f * furthestT;
    float randomT   = fractT + Randomizer::GetInstance()->RandomNumZeroToOne() * (furthestT - fractT);

    const Collision::Ray2D* randomRay = intersectionTRayArray[randomIndex].second;
    return randomRay->GetPointAlongRayFromOrigin(randomT);
}

void NouveauBossAI::ShootRandomLaserBullet(GameModel* gameModel) {

    PlayerPaddle* paddle = gameModel->GetPlayerPaddle();
    assert(paddle != NULL);
    const GameLevel* currLevel = gameModel->GetCurrentLevel();
    assert(currLevel != NULL);

    // Shoot a laser!
    if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
        
        // Shoot at the paddle
        Point2D shotOrigin = this->boss->GetBottomSphereShootPt(); 
        const Point2D& paddlePos = paddle->GetCenterPosition();
        Vector2D laserDir = paddlePos - shotOrigin;
        laserDir.Normalize();

        // If the laser is anywhere near to colliding with either of the splitter prisms then just shoot
        // at the apex of that splitter prism...
        PrismBlock* splitterPrism = NouveauBoss::GetLeftSplitterPrism(*currLevel);
        Vector2D splitterTargetDir = (splitterPrism->GetCenter() + Vector2D(0, LevelPiece::HALF_PIECE_HEIGHT)) - shotOrigin;
        splitterTargetDir.Normalize();
        if (Trig::radiansToDegrees(acos(std::max<float>(-1.0f, std::min<float>(1.0f, 
            Vector2D::Dot(splitterTargetDir, laserDir))))) <= 15.0f) {
            laserDir = splitterTargetDir;
        }
        else {
            splitterPrism = NouveauBoss::GetRightSplitterPrism(*currLevel);
            splitterTargetDir = (splitterPrism->GetCenter() + Vector2D(0, LevelPiece::HALF_PIECE_HEIGHT)) - shotOrigin;
            splitterTargetDir.Normalize();

            if (Trig::radiansToDegrees(acos(std::max<float>(-1.0f, std::min<float>(1.0f, 
                Vector2D::Dot(splitterTargetDir, laserDir))))) <= 15.0f) {
                    laserDir = splitterTargetDir;
            }
            else {
                // Rotate a tiny random amount if we're shooting directly at the paddle
                laserDir.Rotate(Randomizer::GetInstance()->RandomNumNegOneToOne() * 5.0f);
            }
        }   
        
        gameModel->AddProjectile(new BossLaserProjectile(shotOrigin, laserDir));
    }
    else {
        // Shoot at prism that gives us a decent shot at the paddle...
        Point2D shootPos;
        Vector2D laserDir, shootPosToPaddleDir;
        PrismTriangleBlock* currPrism = NULL;
        int prismIdx;
        std::list<Collision::Ray2D> collisionRays;

        // Figure out what prisms will reflect the lasers towards the paddle...
        if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
            // Left side prisms...
            if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
                shootPos = this->boss->GetLeftSideCurlShootPt1();
            }
            else {
                shootPos = this->boss->GetLeftSideCurlShootPt2();
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
        else {
            // Right side prisms...
            if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
                shootPos = this->boss->GetRightSideCurlShootPt1();
            }
            else {
                shootPos = this->boss->GetRightSideCurlShootPt2();
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
    }
}

void NouveauBossAI::OnSetStateMoveToTargetStopAndShoot() {
    this->startPosition = this->targetPosition = this->boss->alivePartsRoot->GetTranslationPt2D();
    this->numLinearMovements = this->GenerateNumMovements();
    this->waitingAtTargetCountdown = 0.0;
    this->timeUntilNextLaserWhileWaitingAtTarget = 0.0;
}

void NouveauBossAI::ExecuteMoveToTargetStopAndShootState(double dT, GameModel* gameModel) {

    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();

    Vector2D startToTargetVec   = this->targetPosition - this->startPosition;
    Vector2D currPosToTargetVec = this->targetPosition - bossPos;

    // Check to see if we're really close to the target or if we've overshot it...
    if (currPosToTargetVec.SqrMagnitude() < EPSILON ||
        Vector2D::Dot(startToTargetVec, currPosToTargetVec) < 0.0f) {

            // Stop the boss immediately
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);

            // Wait at the target for a bit of time before moving on...
            this->waitingAtTargetCountdown -= dT;
            if (this->waitingAtTargetCountdown <= 0.0) {
                // Decrement the number of movements
                this->numLinearMovements--;
                if (this->numLinearMovements <= 0) {
                    // We're done with all the movements, go to the next state...
                    this->GoToNextRandomAttackState();
                }
                else {
                    // Choose a new target, continue moving between targets
                    this->startPosition = bossPos;
                    this->targetPosition = this->ChooseTargetPosition(this->startPosition);
                    this->waitingAtTargetCountdown = this->GenerateWaitAtTargetTime();
                    this->timeUntilNextLaserWhileWaitingAtTarget = 0.0;
                }
            }
            else {
                // Shoot a whole bunch of lasers (some that will bounce off prisms) at the paddle
                if (this->timeUntilNextLaserWhileWaitingAtTarget <= 0.0) {
                    this->ShootRandomLaserBullet(gameModel);
                    this->timeUntilNextLaserWhileWaitingAtTarget = this->GetTimeBetweenLasersWhileWaitingAtTarget();
                }
                else {
                    this->timeUntilNextLaserWhileWaitingAtTarget -= dT;
                }
            }

            return;
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
}

// SideSphereAI Functions ***********************************************************************

const float SideSphereAI::SIDE_SPHERE_LIFE_POINTS  = 10.0f;

const float SideSphereAI::MAX_MOVE_SPEED = 0.4f * PlayerPaddle::DEFAULT_MAX_SPEED;
const float SideSphereAI::DEFAULT_ACCELERATION = 0.25f * PlayerPaddle::DEFAULT_ACCELERATION;

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

    boss->ConvertAliveBodyPartToWeakpoint(boss->rightSideSphereIdx, SideSphereAI::SIDE_SPHERE_LIFE_POINTS, SideSphereAI::SIDE_SPHERE_LIFE_POINTS);
    assert(dynamic_cast<BossWeakpoint*>(boss->bodyParts[boss->rightSideSphereIdx]) != NULL);
    this->rightSideSphereWeakpt = static_cast<BossWeakpoint*>(boss->bodyParts[boss->rightSideSphereIdx]);

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
    // TODO...
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
        
        case RapidFireAIState:
            break;
        case PrepLaserBeamAttackAIState:
            break;
        case LaserBeamAttackAIState:
            break;

        case LostLeftArmAIState:
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->boss->alivePartsRoot->AnimateColourRGBA(Boss::BuildBossHurtAndInvulnerableColourAnim());
            this->lostLeftArmAnim.ResetToStart();
            break;

        case LostRightArmAIState:
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            this->boss->alivePartsRoot->AnimateColourRGBA(Boss::BuildBossHurtAndInvulnerableColourAnim());
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
        case RapidFireAIState:
            this->ExecuteRapidFireState(dT, gameModel);
            break;
        case PrepLaserBeamAttackAIState:
            this->ExecutePrepLaserBeamAttackState(dT, gameModel);
            break;
        case LaserBeamAttackAIState:
            this->ExecuteLaserBeamAttackState(dT, gameModel);
            break;
        case LostLeftArmAIState:
            this->ExecuteLostLeftArmState(dT, gameModel);
            break;
        case LostRightArmAIState:
            this->ExecuteLostRightArmState(dT, gameModel);
            break;
        case BothArmsLostAIState:
            this->ExecuteBothArmsLostState(dT, gameModel);
            break;
        default:
            assert(false);
            return;
    }
}



void SideSphereAI::ExecuteRapidFireState(double dT, GameModel* gameModel) {

}

void SideSphereAI::ExecutePrepLaserBeamAttackState(double dT, GameModel* gameModel) {

}

void SideSphereAI::ExecuteLaserBeamAttackState(double dT, GameModel* gameModel) {

}

void SideSphereAI::ExecuteLostLeftArmState(double dT, GameModel* gameModel) {
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

void SideSphereAI::ExecuteLostRightArmState(double dT, GameModel* gameModel) {

}

void SideSphereAI::ExecuteBothArmsLostState(double dT, GameModel* gameModel) {
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

const float GlassDomeAI::TOP_SPHERE_LIFE_POINTS = 300.0f;
const float GlassDomeAI::MAX_MOVE_SPEED = 0.6f * PlayerPaddle::DEFAULT_MAX_SPEED;
const float GlassDomeAI::DEFAULT_ACCELERATION = 0.35f * PlayerPaddle::DEFAULT_ACCELERATION;

GlassDomeAI::GlassDomeAI(NouveauBoss* boss) : NouveauBossAI(boss), topSphereWeakpt(NULL) {

    // Convert and keep pointers to the top sphere weak point on the boss for this AI state
    boss->ConvertAliveBodyPartToWeakpoint(boss->topSphereIdx, GlassDomeAI::TOP_SPHERE_LIFE_POINTS, GlassDomeAI::TOP_SPHERE_LIFE_POINTS / 3.0f);
    assert(dynamic_cast<BossWeakpoint*>(boss->bodyParts[boss->topSphereIdx]) != NULL);
    this->topSphereWeakpt = static_cast<BossWeakpoint*>(boss->bodyParts[boss->topSphereIdx]);

    this->SetState(NouveauBossAI::MoveToTargetStopAndShootAIState);
}

GlassDomeAI::~GlassDomeAI() {

}

void GlassDomeAI::CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart) {
    
    // If the projectile isn't colliding with the weakpoint in this state then ignore it
    if (collisionPart != this->topSphereWeakpt) {
        return;
    }    
    // Ignore all other projectiles except the laser bullets from the paddle
    if (projectile->GetType() != Projectile::PaddleLaserBulletProjectile) {
        return;
    }

    // There's been a collision with the weakpoint, react to it
    if (!collisionPart->GetIsDestroyed()) {
        // TODO

        this->SetState(NouveauBossAI::HurtTopAIState);
    }
}

float GlassDomeAI::GetTotalLifePercent() const {
    return this->topSphereWeakpt->GetCurrentLifePercentage();
}

void GlassDomeAI::GoToNextRandomAttackState() {
    assert(false); // TODO!!
}

void GlassDomeAI::SetState(NouveauBossAI::AIState newState) {
    switch (newState) {

        case MoveToTargetStopAndShootAIState:
            break;

        case PrepLaserBeamAttackAIState:
            break;
        case LaserBeamAttackAIState:
            break;

        // TODO... other attack states?

        case HurtTopAIState:
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
            break;

        case PrepLaserBeamAttackAIState:
            break;
        case LaserBeamAttackAIState:
            break;

            // TODO... other attack states?

        case HurtTopAIState:
            break;

        default:
            assert(false);
            return;
    }
}