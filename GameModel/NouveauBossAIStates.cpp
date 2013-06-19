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

using namespace nouveaubossai;

NouveauBossAI::NouveauBossAI(NouveauBoss* boss) : BossAIState(), boss(boss), currState() {
    assert(boss != NULL);
}

NouveauBossAI::~NouveauBossAI() {
}

Boss* NouveauBossAI::GetBoss() const {
    return this->boss;
}

Collision::AABB2D NouveauBossAI::GenerateDyingAABB() const {
    return this->boss->GetBody()->GenerateWorldAABB();
}

float NouveauBossAI::GetAccelerationMagnitude() const {
    return NouveauBoss::DEFAULT_ACCELERATION;
}

// SideSphereAI Functions ***********************************************************************

const float SideSphereAI::SIDE_SPHERE_LIFE_POINTS  = 10.0f;

const float SideSphereAI::MAX_MOVE_SPEED = 0.75f * PlayerPaddle::DEFAULT_MAX_SPEED;

const double SideSphereAI::SIDE_FRILL_FADEOUT_TIME = 0.1;
const double SideSphereAI::ARM_FADE_TIME           = 3.0;

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

    this->SetState(NouveauBossAI::BasicMoveAndShootAIState);
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

    // Check to see if the explosion hit the left/right frills (if they haven't been blown up already)
    if (!this->leftSideSphereFrill->GetIsDestroyed()) {
        if (this->leftSideSphereFrill->GetWorldBounds().CollisionCheck(explosionCircle)) {
            
            // Destroy the left frill...
            this->boss->ConvertAliveBodyPartToDeadBodyPart(this->leftSideSphereFrill);
            // Quickly fade out the geometry (it will seem to disappear in the explosion)
            this->leftSideSphereFrill->SetColour(SIDE_FRILL_FADEOUT_TIME, ColourRGBA(1,1,1,0));
            
            return;
        }
    }
    if (!this->rightSideSphereFrill->GetIsDestroyed()) {
        if (this->rightSideSphereFrill->GetWorldBounds().CollisionCheck(explosionCircle)) {
            
            // Destroy the right frill...
            this->boss->ConvertAliveBodyPartToDeadBodyPart(this->rightSideSphereFrill);
            // Quickly fade out the geometry (it will seem to disappear in the explosion)
            this->rightSideSphereFrill->SetColour(SIDE_FRILL_FADEOUT_TIME, ColourRGBA(1,1,1,0));

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

Point2D SideSphereAI::ChooseTargetPosition(const Point2D& startPos) const {
    Collision::AABB2D movementAABB = this->boss->GetMovementAABB();
    
    BoundingLines movementBoundingLines(movementAABB);
    
    // Cast 4 rays from the startPos in reasonably-uniformly distributed, but random directions
    // intersect those rays with the boundaries of the movement bounding box
    std::vector<Collision::Ray2D> rays;
    rays.reserve(4);
    rays.push_back(Collision::Ray2D(startPos, Vector2D::Rotate(0.0f   + Randomizer::GetInstance()->RandomNumZeroToOne()*90.0f, Vector2D(0,1))));
    rays.push_back(Collision::Ray2D(startPos, Vector2D::Rotate(90.0f  + Randomizer::GetInstance()->RandomNumZeroToOne()*90.0f, Vector2D(0,1))));
    rays.push_back(Collision::Ray2D(startPos, Vector2D::Rotate(180.0f + Randomizer::GetInstance()->RandomNumZeroToOne()*90.0f, Vector2D(0,1))));
    rays.push_back(Collision::Ray2D(startPos, Vector2D::Rotate(270.0f + Randomizer::GetInstance()->RandomNumZeroToOne()*90.0f, Vector2D(0,1))));

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
    static const int NUM_RAYS_TO_ELIMINATE = 1;
    int randomIndex = NUM_RAYS_TO_ELIMINATE +
        Randomizer::GetInstance()->RandomUnsignedInt() % (intersectionTRayArray.size() - NUM_RAYS_TO_ELIMINATE);
    
    // Choose a point along the chosen ray from the midpoint of its intersection all the way to its intersection
    float furthestT = intersectionTRayArray[randomIndex].first;
    float midT = furthestT / 2.0f;
    float randomT = midT + Randomizer::GetInstance()->RandomNumZeroToOne() * (furthestT - midT);
    
    const Collision::Ray2D* randomRay = intersectionTRayArray[randomIndex].second;
    return randomRay->GetPointAlongRayFromOrigin(randomT);
}

void SideSphereAI::SetState(NouveauBossAI::AIState newState) {

    switch (newState) {
        case BasicMoveAndShootAIState:
            this->startPosition = this->targetPosition = this->boss->alivePartsRoot->GetTranslationPt2D();
            this->numLinearMovements = this->GenerateNumMovements();
            this->waitingAtTargetCountdown = this->GenerateWaitAtTargetTime();
            break;
        case RapidFireAIState:
            break;
        case PrepLaserBeamAttackAIState:
            break;
        case LaserBeamAttackAIState:
            break;
        default:
            assert(false);
            return;
    }

    this->currState = newState;
}

void SideSphereAI::UpdateState(double dT, GameModel* gameModel) {

    switch (this->currState) {
        case BasicMoveAndShootAIState:
            this->ExecuteBasicMoveAndShootState(dT, gameModel);
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
        default:
            assert(false);
            return;
    }
}

void SideSphereAI::ExecuteBasicMoveAndShootState(double dT, GameModel* gameModel) {
    
    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();

    // Shoot lasers when possible...
    // TODO

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
                // TODO
            }
            else {
                // Choose a new target, continue moving between targets
                this->startPosition = bossPos;
                this->targetPosition = this->ChooseTargetPosition(this->startPosition);
                this->waitingAtTargetCountdown = this->GenerateWaitAtTargetTime();
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
        this->desiredVel = MAX_MOVE_SPEED * Vector2D::Normalize(currPosToTargetVec);
    }
}

void SideSphereAI::ExecuteRapidFireState(double dT, GameModel* gameModel) {

}

void SideSphereAI::ExecutePrepLaserBeamAttackState(double dT, GameModel* gameModel) {

}

void SideSphereAI::ExecuteLaserBeamAttackState(double dT, GameModel* gameModel) {

}

AnimationMultiLerp<Vector3D> SideSphereAI::GenerateArmDeathTranslationAnimation(bool isLeftArm) const {
    float xDist = (isLeftArm ? -1 : 1) * 5 * NouveauBoss::ARM_SPHERE_HOLDER_CURL_WIDTH;

    float yDist = -(isLeftArm ? this->leftSideSphereHolderCurl->GetTranslationPt2D()[1] : 
        this->rightSideSphereHolderCurl->GetTranslationPt2D()[1]);
    yDist -= 2*NouveauBoss::ARM_SPHERE_HOLDER_CURL_WIDTH;

    return Boss::BuildLimbFallOffTranslationAnim(ARM_FADE_TIME, xDist, yDist);
}

AnimationMultiLerp<float> SideSphereAI::GenerateArmDeathRotationAnimation(bool isLeftArm) const {
    return Boss::BuildLimbFallOffZRotationAnim(ARM_FADE_TIME, isLeftArm ? 100.0f : -100.0f);
}