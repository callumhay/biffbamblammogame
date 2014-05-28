/**
 * FuturismBoss.cpp
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

#include "FuturismBoss.h"
#include "FuturismBossStage1AIState.h"
#include "FuturismBossStage3AIState.h"
#include "PortalBlock.h"

const float FuturismBoss::FULLY_SHIELDED_BOSS_HEIGHT       = 10.0f;
const float FuturismBoss::FULLY_SHIELDED_BOSS_HALF_HEIGHT  = FuturismBoss::FULLY_SHIELDED_BOSS_HEIGHT / 2.0f;
const float FuturismBoss::FULLY_SHIELDED_BOSS_WIDTH        = 8.80f;
const float FuturismBoss::FULLY_SHIELDED_BOSS_HALF_WIDTH   = FuturismBoss::FULLY_SHIELDED_BOSS_WIDTH / 2.0f;

const float FuturismBoss::CORE_BOSS_SIZE      = 5.037f;
const float FuturismBoss::CORE_BOSS_HALF_SIZE = FuturismBoss::CORE_BOSS_SIZE / 2.0f;
const float FuturismBoss::CORE_EYE_SIZE      = 1.40f;
const float FuturismBoss::CORE_EYE_HALF_SIZE = CORE_EYE_SIZE / 2.0f;
const float FuturismBoss::CORE_SHIELD_SIZE = 3.30f;
const float FuturismBoss::CORE_BULB_SIZE = 1.160f;
const float FuturismBoss::CORE_BULB_HALF_SIZE = CORE_BULB_SIZE / 2.0f;
const float FuturismBoss::CORE_BULB_Z_DIST = -1.732f;
const float FuturismBoss::CORE_BULB_OFFSET_FROM_ORIGIN = 2.326f;
const float FuturismBoss::CORE_ARMS_MOVE_FORWARD_AMT = 0.723f;

const float FuturismBoss::CORE_Z_DIST_FROM_ORIGIN = 0.0f;//1.732f;
const float FuturismBoss::CORE_Z_SHOOT_DIST_FROM_ORIGIN_WITHOUT_SHIELD = EPSILON;
const float FuturismBoss::CORE_Z_SHOOT_DIST_FROM_ORIGIN_WITH_SHIELD    = 0.363f;
const float FuturismBoss::CORE_SHIELD_CORNER_HEIGHT = 1.167f;

const float FuturismBoss::TOP_BOTTOM_SHIELD_EXPLODE_WIDTH  = 1.636f;
const float FuturismBoss::TOP_BOTTOM_SHIELD_EXPLODE_HEIGHT = 3.457f;
const float FuturismBoss::LEFT_RIGHT_SHIELD_EXPLODE_WIDTH  = 3.233f;
const float FuturismBoss::LEFT_RIGHT_SHIELD_EXPLODE_HEIGHT = 2.333f;

const float FuturismBoss::TOP_BOTTOM_SHIELD_X_OFFSET = 0.0f;
const float FuturismBoss::TOP_BOTTOM_SHIELD_Y_OFFSET = 3.272f;
const float FuturismBoss::LEFT_RIGHT_SHIELD_X_OFFSET = 1.6165f + 1.167f;
const float FuturismBoss::LEFT_RIGHT_SHIELD_Y_OFFSET = 0.0f;

const float FuturismBoss::BLOCK_TO_BOSS_X_PADDING = LevelPiece::PIECE_HEIGHT;
const float FuturismBoss::BLOCK_TO_BOSS_Y_PADDING = LevelPiece::PIECE_HEIGHT;

const float FuturismBoss::PORTAL_PROJECTILE_WIDTH       = 1.1f*LevelPiece::PIECE_WIDTH;
const float FuturismBoss::PORTAL_PROJECTILE_HEIGHT      = 1.1f*LevelPiece::PIECE_HEIGHT;
const float FuturismBoss::PORTAL_PROJECTILE_HALF_WIDTH  = FuturismBoss::PORTAL_PROJECTILE_WIDTH / 2.0f;
const float FuturismBoss::PORTAL_PROJECTILE_HALF_HEIGHT = FuturismBoss::PORTAL_PROJECTILE_HEIGHT / 2.0f;

FuturismBoss::FuturismBoss() : Boss(),
currIceRotInDegs(0.0f), currArmMoveForwardOffset(0.0f) {

    // The middle y coordinate is the same for the left and right sub-arenas
    float midY = FuturismBoss::GetLeftSubArenaMinYBossPos(FULLY_SHIELDED_BOSS_HALF_HEIGHT) + 
        (FuturismBoss::GetLeftSubArenaMaxYBossPos(FULLY_SHIELDED_BOSS_HALF_HEIGHT) - 
        FuturismBoss::GetLeftSubArenaMinYBossPos(FULLY_SHIELDED_BOSS_HALF_HEIGHT)) / 2.0f;
    
    {
        float leftSideMidX    = FuturismBoss::GetLeftSubArenaMinXConfines()  + FuturismBoss::GetSubArenaWidth()/2.0f;
        float leftSideLeftX   = FuturismBoss::GetLeftSubArenaMinXBossPos(FULLY_SHIELDED_BOSS_HALF_WIDTH);
        float leftSideRightX  = FuturismBoss::GetLeftSubArenaMaxXBossPos(FULLY_SHIELDED_BOSS_HALF_WIDTH);
        float leftSideTopY    = FuturismBoss::GetLeftSubArenaMaxYBossPos(FULLY_SHIELDED_BOSS_HALF_HEIGHT);
        float leftSideBottomY = FuturismBoss::GetLeftSubArenaMinYBossPos(FULLY_SHIELDED_BOSS_HALF_HEIGHT);

        this->leftSideMovePositions.reserve(9);
        this->leftSideMovePositions.push_back(Point2D(leftSideLeftX, leftSideTopY));     // Top-Left
        this->leftSideMovePositions.push_back(Point2D(leftSideMidX, leftSideTopY));      // Top
        this->leftSideMovePositions.push_back(Point2D(leftSideRightX, leftSideTopY));    // Top-Right
        this->leftSideMovePositions.push_back(Point2D(leftSideLeftX, midY));             // Left
        this->leftSideMovePositions.push_back(Point2D(leftSideMidX, midY));              // Middle
        this->leftSideMovePositions.push_back(Point2D(leftSideRightX, midY));            // Right
        this->leftSideMovePositions.push_back(Point2D(leftSideLeftX, leftSideBottomY));  // Bottom-Left
        this->leftSideMovePositions.push_back(Point2D(leftSideMidX, leftSideBottomY));   // Bottom
        this->leftSideMovePositions.push_back(Point2D(leftSideRightX, leftSideBottomY)); // Bottom-Right
    }
    {
        float rightSideMidX    = FuturismBoss::GetRightSubArenaMinXConfines()  + FuturismBoss::GetSubArenaWidth()/2.0f;
        float rightSideLeftX   = FuturismBoss::GetRightSubArenaMinXBossPos(FULLY_SHIELDED_BOSS_HALF_WIDTH);
        float rightSideRightX  = FuturismBoss::GetRightSubArenaMaxXBossPos(FULLY_SHIELDED_BOSS_HALF_WIDTH);
        float rightSideTopY    = FuturismBoss::GetRightSubArenaMaxYBossPos(FULLY_SHIELDED_BOSS_HALF_HEIGHT);
        float rightSideBottomY = FuturismBoss::GetRightSubArenaMinYBossPos(FULLY_SHIELDED_BOSS_HALF_HEIGHT);

        this->rightSideMovePositions.reserve(9);
        this->rightSideMovePositions.push_back(Point2D(rightSideLeftX, rightSideTopY));     // Top-Left
        this->rightSideMovePositions.push_back(Point2D(rightSideMidX, rightSideTopY));      // Top
        this->rightSideMovePositions.push_back(Point2D(rightSideRightX, rightSideTopY));    // Top-Right
        this->rightSideMovePositions.push_back(Point2D(rightSideLeftX, midY));              // Left
        this->rightSideMovePositions.push_back(Point2D(rightSideMidX, midY));               // Middle
        this->rightSideMovePositions.push_back(Point2D(rightSideRightX, midY));             // Right
        this->rightSideMovePositions.push_back(Point2D(rightSideLeftX, rightSideBottomY));  // Bottom-Left
        this->rightSideMovePositions.push_back(Point2D(rightSideMidX, rightSideBottomY));   // Bottom
        this->rightSideMovePositions.push_back(Point2D(rightSideRightX, rightSideBottomY)); // Bottom-Right
    }
    {
        float midX = FuturismBoss::GetFullArenaCenterPos()[0];
        
        this->fullLevelMovePositions.reserve(19);
        this->fullLevelMovePositions.insert(this->fullLevelMovePositions.end(), this->leftSideMovePositions.begin(), this->leftSideMovePositions.end());
        this->fullLevelMovePositions.insert(this->fullLevelMovePositions.end(), this->rightSideMovePositions.begin(), this->rightSideMovePositions.end());
        this->fullLevelMovePositions.push_back(Point2D(midX, midY));
    }
}

FuturismBoss::~FuturismBoss() {
}

bool FuturismBoss::IsCoreShieldVulnerable() const {
    return static_cast<const FuturismBossAIState*>(this->GetCurrentAIState())->IsCoreShieldVulnerable();
}

bool FuturismBoss::AreBulbsVulnerable() const {
    return static_cast<const FuturismBossAIState*>(this->GetCurrentAIState())->AreBulbsVulnerable();
}

bool FuturismBoss::IsFrozen() const {
    return static_cast<const FuturismBossAIState*>(this->GetCurrentAIState())->IsFrozen();
}

bool FuturismBoss::ProjectilePassesThrough(const Projectile* projectile) const {
    switch (projectile->GetType()) {
        case Projectile::BossLaserBulletProjectile:
        case Projectile::BossRocketBulletProjectile:
        case Projectile::BossOrbBulletProjectile:
        case Projectile::BossLightningBoltBulletProjectile:
        case Projectile::PortalBlobProjectile:
        case Projectile::BossShockOrbBulletProjectile:
            return true;
        default:
            break;
    }

    return false;
}

void FuturismBoss::Init(float startingX, float startingY, 
                        const std::vector<std::vector<LevelPiece*> >& levelPieces) {

    // Body Layout for the Futurism Boss:
    // ----------------------------------
    // root
    // |____deadPartsRoot
    // |____alivePartsRoot
    //      |____core_assembly
    //           |____core_body
    //           |____top_bulb
    //           |____bottom_bulb
    //           |____left_bulb
    //           |____right_bulb
    //           
    //      |____core_shield
    //      |____top_shield
    //      |____bottom_shield
    //      |____left_shield
    //      |____right_shield

    // root
    {
        assert(this->root == NULL);
        this->root = new BossCompositeBodyPart();
        this->bodyParts.push_back(this->root);

        // deadPartsRoot
        {
            assert(this->deadPartsRoot == NULL);
            this->deadPartsRoot = new BossCompositeBodyPart();
            this->root->AddBodyPart(this->deadPartsRoot);
            this->bodyParts.push_back(this->deadPartsRoot);
        }

        // alivePartsRoot
        {
            assert(this->alivePartsRoot == NULL);
            this->alivePartsRoot = new BossCompositeBodyPart();
            this->root->AddBodyPart(this->alivePartsRoot);
            this->bodyParts.push_back(this->alivePartsRoot);

            // core_assembly
            {
                BossCompositeBodyPart* coreAssembly = new BossCompositeBodyPart();
                this->alivePartsRoot->AddBodyPart(coreAssembly);
                this->coreAssemblyIdx = this->bodyParts.size();
                this->bodyParts.push_back(coreAssembly);

                BoundingLines emptyBounds;

                // core_body
                {
                    BossBodyPart* coreBody = new BossBodyPart(emptyBounds);
                    coreAssembly->AddBodyPart(coreBody);
                    this->coreBodyIdx = this->bodyParts.size();
                    this->bodyParts.push_back(coreBody);
                }

                // top_bulb
                {
                    BossBodyPart* bulb = new BossBodyPart(emptyBounds);
                    coreAssembly->AddBodyPart(bulb);
                    this->topBulbIdx = this->bodyParts.size();
                    this->bodyParts.push_back(bulb);
                }

                // left_bulb
                {
                    BossBodyPart* bulb = new BossBodyPart(emptyBounds);
                    coreAssembly->AddBodyPart(bulb);
                    this->leftBulbIdx = this->bodyParts.size();
                    this->bodyParts.push_back(bulb);
                    
                    bulb->RotateZ(90.0f);
                }

                // bottom_bulb
                {
                    BossBodyPart* bulb = new BossBodyPart(emptyBounds);
                    coreAssembly->AddBodyPart(bulb);
                    this->bottomBulbIdx = this->bodyParts.size();
                    this->bodyParts.push_back(bulb);

                    bulb->RotateZ(180.0f);
                }

                // right_bulb
                {
                    BossBodyPart* bulb = new BossBodyPart(emptyBounds);
                    coreAssembly->AddBodyPart(bulb);
                    this->rightBulbIdx = this->bodyParts.size();
                    this->bodyParts.push_back(bulb);

                    bulb->RotateZ(270.0f);
                }

            }

            // core_shield
            {
                BoundingLines coreShieldBounds;
                
                // NOTE: We don't generate the core shield's bounds yet, since it starts hidden by
                // all the shields around it

                BossBodyPart* coreShield = new BossBodyPart(coreShieldBounds);
                this->alivePartsRoot->AddBodyPart(coreShield);
                this->coreShieldIdx = this->bodyParts.size();
                this->bodyParts.push_back(coreShield);
            }

            {
                const Point2D PT0(-2.159f, CORE_SHIELD_CORNER_HEIGHT);
                const Point2D PT1(-0.818f, 4.517f);
                const Point2D PT2(-0.258f, 5.0f);
                const Point2D PT3(0.258f, 5.0f);
                const Point2D PT4(0.818f, 4.517f);
                const Point2D PT5(2.159f, CORE_SHIELD_CORNER_HEIGHT);

                BoundingLines shieldBounds;
                
                shieldBounds.AddBound(Collision::LineSeg2D(PT0, PT1), Vector2D(PT0[1]-PT1[1], PT1[0]-PT0[0]), false);
                shieldBounds.AddBound(Collision::LineSeg2D(PT1, PT2), Vector2D(PT1[1]-PT2[1], PT2[0]-PT1[0]), false);
                shieldBounds.AddBound(Collision::LineSeg2D(PT2, PT3), Vector2D(0,1), false);
                shieldBounds.AddBound(Collision::LineSeg2D(PT3, PT4), Vector2D(PT3[1]-PT4[1], PT4[0]-PT3[0]), false);
                shieldBounds.AddBound(Collision::LineSeg2D(PT4, PT5), Vector2D(PT4[1]-PT5[1], PT5[0]-PT4[0]), false);

                // top_shield
                {
                    BossBodyPart* topShield = new BossBodyPart(shieldBounds);
                    this->alivePartsRoot->AddBodyPart(topShield);
                    this->topShieldIdx = this->bodyParts.size();
                    this->bodyParts.push_back(topShield);
                }

                // bottom_shield
                {
                    BossBodyPart* bottomShield = new BossBodyPart(shieldBounds);
                    this->alivePartsRoot->AddBodyPart(bottomShield);
                    this->bottomShieldIdx = this->bodyParts.size();
                    this->bodyParts.push_back(bottomShield);
                    bottomShield->RotateZ(180.0f);
                }
            }

            {
                static const Vector2D BOUNDS_OFFSET(0.5f, 0.0f);

                const Point2D PT0 = Point2D(2.159f, 1.167f);
                const Point2D PT1 = Point2D(2.909f, 1.417f);
                const Point2D PT2_BASE = Point2D(4.4f, 1.417f);
                const Point2D PT3_BASE = Point2D(4.4f, -1.417f);
                const Point2D PT4 = Point2D(2.909f, -1.417f);
                const Point2D PT5 = Point2D(2.159f, -1.167f);


                
                
                #define BUILD_SHIELD_BOUNDS() shieldBounds.Clear(); \
                    shieldBounds.AddBound(Collision::LineSeg2D(PT0, PT1), Vector2D(PT0[1]-PT1[1], PT1[0]-PT0[0]), false); \
                    shieldBounds.AddBound(Collision::LineSeg2D(PT1, PT2), Vector2D(0,1), false); \
                    shieldBounds.AddBound(Collision::LineSeg2D(PT2, PT3), Vector2D(1,0), false); \
                    shieldBounds.AddBound(Collision::LineSeg2D(PT3, PT4), Vector2D(0,-1), false); \
                    shieldBounds.AddBound(Collision::LineSeg2D(PT4, PT5), Vector2D(PT4[1]-PT5[1], PT5[0]-PT4[0]), false);
            
                // left_shield
                {
                    const Point2D PT2 = PT2_BASE - BOUNDS_OFFSET;
                    const Point2D PT3 = PT3_BASE - BOUNDS_OFFSET;

                    BoundingLines shieldBounds;
                    BUILD_SHIELD_BOUNDS();

                    BossBodyPart* leftShield = new BossBodyPart(shieldBounds);
                    this->alivePartsRoot->AddBodyPart(leftShield);
                    this->leftShieldIdx = this->bodyParts.size();
                    this->bodyParts.push_back(leftShield);
                    leftShield->RotateZ(180.0f);
                }

                // right_shield
                {
                    const Point2D PT2 = PT2_BASE + BOUNDS_OFFSET;
                    const Point2D PT3 = PT3_BASE + BOUNDS_OFFSET;

                    BoundingLines shieldBounds;
                    BUILD_SHIELD_BOUNDS();

                    BossBodyPart* rightShield = new BossBodyPart(shieldBounds);
                    this->alivePartsRoot->AddBodyPart(rightShield);
                    this->rightShieldIdx = this->bodyParts.size();
                    this->bodyParts.push_back(rightShield);
                }
                #undef BUILD_SHIELD_BOUNDS


            }
        }
    }

    // Initialize the cache of level pieces in the left and right sub-arenas
    static const int numPiecesToEndOfLeftSubArena = LEVEL_X_OUTER_PADDING_NUM_PIECES+LEVEL_SUBARENA_WIDTH_NUM_PIECES;
    for (int row = LEVEL_Y_OUTER_PADDING_NUM_PIECES+1; row < LEVEL_SUBARENA_HEIGHT_NUM_PIECES-1; row++) {
        for (int col = LEVEL_X_OUTER_PADDING_NUM_PIECES+1; col < numPiecesToEndOfLeftSubArena-1; col++) {
            this->leftSubArenaPieces.insert(levelPieces[row][col]);
        }
    }
    for (int row = LEVEL_Y_OUTER_PADDING_NUM_PIECES+1; row < LEVEL_SUBARENA_HEIGHT_NUM_PIECES-1; row++) {
        for (int col = numPiecesToEndOfLeftSubArena+2; col < numPiecesToEndOfLeftSubArena+LEVEL_SUBARENA_WIDTH_NUM_PIECES; col++) {
            this->rightSubArenaPieces.insert(levelPieces[row][col]);
        }
    }
    
    // For the full arena, insert pieces for both the left and right sub-arenas
    this->fullArenaPieces.insert(this->leftSubArenaPieces.begin(), this->leftSubArenaPieces.end());
    this->fullArenaPieces.insert(this->rightSubArenaPieces.begin(), this->rightSubArenaPieces.end());
    
    // NOTE: WHEN THE BOSS BLOWS UP THE MID PIECES WE'LL NEED TO INSERT THE RESULTING PIECES, UNTIL THEN DONT.
    // ... also insert the middle pieces...
    //int midCol = static_cast<int>(levelPieces.front().size()) / 2;
    //for (int row = LEVEL_Y_OUTER_PADDING_NUM_PIECES+1; row < LEVEL_SUBARENA_HEIGHT_NUM_PIECES-1; row++) {
    //    this->fullArenaPieces.insert(levelPieces[row][midCol]);
    //}


    // Setup the portal projectile colours...
    static const int NUM_PORTAL_COLOURS = 5;
    this->portalProjectileColours.reserve(NUM_PORTAL_COLOURS);
    for (int i = 0; i < NUM_PORTAL_COLOURS; i++) {
        this->portalProjectileColours.push_back(PortalBlock::GeneratePortalColour());
    }
    this->currPortalProjectileColourIdx = Randomizer::GetInstance()->RandomUnsignedInt() % 
        static_cast<int>(this->portalProjectileColours.size());


    // Move the boss to its starting location
    // NOTE: We override the starting position because we want the boss to start on one side of the level...
    UNUSED_PARAMETER(startingX);
    UNUSED_PARAMETER(startingY);
    Point2D rightArenaCenter = FuturismBoss::GetRightSubArenaCenterPos();
    this->root->Translate(Vector3D(rightArenaCenter[0], rightArenaCenter[1], 0.0f));

    this->SetCurrentAIStateImmediately(new FuturismBossStage1AIState(this));
}


void FuturismBoss::AddSpecialCoreBounds(ShieldLimbType sectorToAdd) {

    // These points represent the bounds for the top part of the shield
    const Point2D PT0(-CORE_SHIELD_CORNER_HEIGHT, CORE_SHIELD_CORNER_HEIGHT);
    const Point2D PT1(-0.435f, 2.529f);
    const Point2D PT2(0.435f, 2.529f);
    const Point2D PT3(CORE_SHIELD_CORNER_HEIGHT, CORE_SHIELD_CORNER_HEIGHT);

    Collision::LineSeg2D leftLine(PT0, PT1);
    Vector2D leftLineNormal(PT0[1] - PT1[1], PT1[0] - PT0[0]);
    Collision::LineSeg2D topLine(PT1, PT2);
    Vector2D topLineNormal(0, 1);
    Collision::LineSeg2D rightLine(PT2, PT3);
    Vector2D rightLineNormal(PT2[1] - PT3[1], PT3[0] - PT2[0]);

    BossBodyPart* core = static_cast<BossBodyPart*>(this->bodyParts[this->coreBodyIdx]);
    BoundingLines shieldBounds = core->GetLocalBounds();

#define ROTATE_LINES_AND_NORMALS(degs) \
    leftLine.Rotate(degs, Point2D(0,0)); leftLineNormal.Rotate(degs); \
    topLine.Rotate(degs, Point2D(0,0)); topLineNormal.Rotate(degs); \
    rightLine.Rotate(degs, Point2D(0,0)); rightLineNormal.Rotate(degs)

#define ADD_BOUNDS() \
    shieldBounds.AddBound(leftLine, leftLineNormal, false); \
    shieldBounds.AddBound(topLine, topLineNormal, false); \
    shieldBounds.AddBound(rightLine, rightLineNormal, false)

    switch (sectorToAdd) {
        case TopShield:
            break;

        case BottomShield:
            ROTATE_LINES_AND_NORMALS(180);
            break;

        case LeftShield:
            ROTATE_LINES_AND_NORMALS(90);
            break;

        case RightShield:
            ROTATE_LINES_AND_NORMALS(-90);
            break;

        case CoreShield:
        default:
            return;

    }
    ADD_BOUNDS();

#undef ADD_BOUNDS
#undef ROTATE_LINES_AND_NORMALS

    assert(shieldBounds.GetNumLines() <= 12);
    core->SetLocalBounds(shieldBounds);
}

// When a particular shielded limb of the boss is destroyed, this function should be called to
// update the bounds of all other existing body parts
void FuturismBoss::UpdateBoundsForDestroyedShieldLimb(ShieldLimbType type) {
    
    switch (type) {
        case TopShield:
            this->TopShieldUpdate();
            break;
        case BottomShield:
            this->BottomShieldUpdate();
            break;
        case LeftShield:
            this->LeftShieldUpdate();
            break;
        case RightShield:
            this->RightShieldUpdate();
            break;
        
        case CoreShield:
            this->RegenerateBoundsForFinalCore();
            break;

        default:
            assert(false);
            return;
    }

    // If all limb shields are destroyed the core shield 
    // bounds should already be properly generated
}

static const float SIDE_SHIELD_EXTRA_LINE_Y = 1.167f;
static const float SIDE_SHIELD_EXTRA_LINE_X0 = 1.167f;
static const float SIDE_SHIELD_EXTRA_LINE_X1 = 2.159f;

void FuturismBoss::TopShieldUpdate() {
    if (this->GetLeftShield()->GetIsDestroyed()) {
        // Update the relevant part of the left shield
        this->LeftShieldUpdate();
    }
    else {
        const Point2D PT0(SIDE_SHIELD_EXTRA_LINE_X0, -SIDE_SHIELD_EXTRA_LINE_Y);
        const Point2D PT1(SIDE_SHIELD_EXTRA_LINE_X1, -SIDE_SHIELD_EXTRA_LINE_Y);

        // Add top bound line to left shield (since the left shield is rotated by 180, we actually add the bottom)
        BossBodyPart* leftShield = static_cast<BossBodyPart*>(this->bodyParts[this->leftShieldIdx]);
        BoundingLines bounds = leftShield->GetLocalBounds();
        bounds.AddBound(Collision::LineSeg2D(PT0, PT1), Vector2D(0,-1), false);
        leftShield->SetLocalBounds(bounds);
    }
    if (this->GetRightShield()->GetIsDestroyed()) {
        this->RightShieldUpdate();
    }
    else {
        // Add top bound line to right shield
        const Point2D PT0(SIDE_SHIELD_EXTRA_LINE_X0, SIDE_SHIELD_EXTRA_LINE_Y);
        const Point2D PT1(SIDE_SHIELD_EXTRA_LINE_X1, SIDE_SHIELD_EXTRA_LINE_Y);

        BossBodyPart* rightShield = static_cast<BossBodyPart*>(this->bodyParts[this->rightShieldIdx]);
        BoundingLines bounds = rightShield->GetLocalBounds();
        bounds.AddBound(Collision::LineSeg2D(PT0, PT1), Vector2D(0,1), false);
        rightShield->SetLocalBounds(bounds);
    }
    this->AddSpecialCoreBounds(TopShield);
}

void FuturismBoss::BottomShieldUpdate() {
    if (this->GetLeftShield()->GetIsDestroyed()) {
        this->LeftShieldUpdate();
    }
    else {
        // Add bottom bound line to left shield
        const Point2D PT0(SIDE_SHIELD_EXTRA_LINE_X0, SIDE_SHIELD_EXTRA_LINE_Y);
        const Point2D PT1(SIDE_SHIELD_EXTRA_LINE_X1, SIDE_SHIELD_EXTRA_LINE_Y);

        // Add top bound line to left shield (since the left shield is rotated by 180, we actually add the top)
        BossBodyPart* leftShield = static_cast<BossBodyPart*>(this->bodyParts[this->leftShieldIdx]);
        BoundingLines bounds = leftShield->GetLocalBounds();
        bounds.AddBound(Collision::LineSeg2D(PT0, PT1), Vector2D(0,1), false);
        leftShield->SetLocalBounds(bounds);
    }
    if (this->GetRightShield()->GetIsDestroyed()) {
        this->RightShieldUpdate();
    }
    else {
        // Add bottom bound line to right shield
        const Point2D PT0(SIDE_SHIELD_EXTRA_LINE_X0, -SIDE_SHIELD_EXTRA_LINE_Y);
        const Point2D PT1(SIDE_SHIELD_EXTRA_LINE_X1, -SIDE_SHIELD_EXTRA_LINE_Y);

        BossBodyPart* rightShield = static_cast<BossBodyPart*>(this->bodyParts[this->rightShieldIdx]);
        BoundingLines bounds = rightShield->GetLocalBounds();
        bounds.AddBound(Collision::LineSeg2D(PT0, PT1), Vector2D(0,-1), false);
        rightShield->SetLocalBounds(bounds);
    }

    this->AddSpecialCoreBounds(BottomShield);
}

void FuturismBoss::LeftShieldUpdate() {

    const Point2D PT0(-2.159f, 1.167f);
    const Point2D PT1(-2.519f, 0.384f);
    const Point2D PT2(-2.519f, -0.384f);
    const Point2D PT3(-2.159f, -1.167f);

    const Point2D TOP_CORNER_PT(-CORE_SHIELD_CORNER_HEIGHT, CORE_SHIELD_CORNER_HEIGHT);
    const Point2D BOTTOM_CORNER_PT(-CORE_SHIELD_CORNER_HEIGHT, -CORE_SHIELD_CORNER_HEIGHT);

    const Collision::LineSeg2D line0(PT0, PT1);
    const Collision::LineSeg2D line1(PT1, PT2);
    const Collision::LineSeg2D line2(PT2, PT3);

    const Collision::LineSeg2D topCornerLine(PT1, TOP_CORNER_PT);
    const Collision::LineSeg2D bottomCornerLine(PT2, BOTTOM_CORNER_PT);

    const Vector2D line0Norm(PT1[1]-PT0[1], PT0[0]-PT1[0]);
    const Vector2D line1Norm(-1,0);
    const Vector2D line2Norm(PT3[1]-PT2[1], PT2[0]-PT3[0]);

    // Make sure special bounds are removed...
    BossBodyPart* core = static_cast<BossBodyPart*>(this->bodyParts[this->coreBodyIdx]);
    BoundingLines bounds = core->GetLocalBounds();
    bounds.RemoveBound(line0);
    bounds.RemoveBound(line1);
    bounds.RemoveBound(line2);
    bounds.RemoveBound(topCornerLine);
    bounds.RemoveBound(bottomCornerLine);

    if (this->GetTopShield()->GetIsDestroyed()) {
        if (this->GetBottomShield()->GetIsDestroyed()) {
            core->SetLocalBounds(bounds);
            this->AddSpecialCoreBounds(LeftShield);
        }
        else {
            // Top shield destroyed, bottom shield not destroyed
            bounds.AddBound(line1, line1Norm, false);
            bounds.AddBound(line2, line2Norm, false);
            bounds.AddBound(topCornerLine, Vector2D(PT1[1]-TOP_CORNER_PT[1], TOP_CORNER_PT[0]-PT1[0]), false);
            
            core->SetLocalBounds(bounds);
        }
    }
    else {
        if (this->GetBottomShield()->GetIsDestroyed()) {
            // Top shield not destroyed, bottom shield destroyed
            bounds.AddBound(line0, line0Norm, false);
            bounds.AddBound(line1, line1Norm, false);
            bounds.AddBound(bottomCornerLine, Vector2D(BOTTOM_CORNER_PT[1]-PT2[1], PT2[0]-BOTTOM_CORNER_PT[0]), false);

            core->SetLocalBounds(bounds);
        }
        else {
            // Top and bottom shields are not destroyed
            bounds.AddBound(line0, line0Norm, false);
            bounds.AddBound(line1, line1Norm, false);
            bounds.AddBound(line2, line2Norm, false);
            
            core->SetLocalBounds(bounds);
        }
    }
}

void FuturismBoss::RightShieldUpdate() {

    const Point2D PT0(2.159f, 1.167f);
    const Point2D PT1(2.519f, 0.384f);
    const Point2D PT2(2.519f, -0.384f);
    const Point2D PT3(2.159f, -1.167f);

    const Point2D TOP_CORNER_PT(CORE_SHIELD_CORNER_HEIGHT, CORE_SHIELD_CORNER_HEIGHT);
    const Point2D BOTTOM_CORNER_PT(CORE_SHIELD_CORNER_HEIGHT, -CORE_SHIELD_CORNER_HEIGHT);

    const Collision::LineSeg2D line0(PT0, PT1);
    const Collision::LineSeg2D line1(PT1, PT2);
    const Collision::LineSeg2D line2(PT2, PT3);

    const Collision::LineSeg2D topCornerLine(PT1, TOP_CORNER_PT);
    const Collision::LineSeg2D bottomCornerLine(PT2, BOTTOM_CORNER_PT);

    const Vector2D line0Norm(PT0[1]-PT1[1], PT1[0]-PT0[0]);
    const Vector2D line1Norm(1,0);
    const Vector2D line2Norm(PT2[1]-PT3[1], PT3[0]-PT2[0]);

    // Make sure special bounds are removed...
    BossBodyPart* core = static_cast<BossBodyPart*>(this->bodyParts[this->coreBodyIdx]);
    BoundingLines bounds = core->GetLocalBounds();
    bounds.RemoveBound(line0);
    bounds.RemoveBound(line1);
    bounds.RemoveBound(line2);
    bounds.RemoveBound(topCornerLine);
    bounds.RemoveBound(bottomCornerLine);

    if (this->GetTopShield()->GetIsDestroyed()) {
        if (this->GetBottomShield()->GetIsDestroyed()) {
            core->SetLocalBounds(bounds);
            this->AddSpecialCoreBounds(RightShield);
        }
        else {
            // Top shield destroyed, bottom shield not destroyed
            bounds.AddBound(line1, line1Norm, false);
            bounds.AddBound(line2, line2Norm, false);
            bounds.AddBound(topCornerLine, Vector2D(TOP_CORNER_PT[1]-PT1[1],PT1[0]- TOP_CORNER_PT[0]), false);
            
            core->SetLocalBounds(bounds);
        }
    }
    else {
        if (this->GetBottomShield()->GetIsDestroyed()) {
            // Top shield not destroyed, bottom shield destroyed
            bounds.AddBound(line0, line0Norm, false);
            bounds.AddBound(line1, line1Norm, false);
            bounds.AddBound(bottomCornerLine, Vector2D(PT2[1]-BOTTOM_CORNER_PT[1], BOTTOM_CORNER_PT[0]-PT2[0]), false);
            
            core->SetLocalBounds(bounds);
        }
        else {
            // Top and bottom shields are not destroyed
            bounds.AddBound(line0, line0Norm, false);
            bounds.AddBound(line1, line1Norm, false);
            bounds.AddBound(line2, line2Norm, false);

            core->SetLocalBounds(bounds);
        }
    }
}

void FuturismBoss::RegenerateBoundsForCoreWithShield() {


    {
        static const Point2D PT0(-0.722f, 1.351f);
        static const Point2D PT1(-0.435f, 2.562f);
        static const Point2D PT2(0.435f,  2.562f);
        static const Point2D PT3(0.722f,  1.351f);

        Collision::LineSeg2D lineSeg0(PT0, PT1);
        Collision::LineSeg2D lineSeg1(PT1, PT2);
        Collision::LineSeg2D lineSeg2(PT2, PT3);
        Vector2D norm0(PT0[1]-PT1[1], PT1[0]-PT0[0]); norm0.Normalize();
        Vector2D norm1(0, 1);
        Vector2D norm2(PT2[1]-PT3[1], PT3[0]-PT2[0]); norm2.Normalize();

#define ROTATE_BY_90() lineSeg0.Rotate(90, Point2D(0,0)); lineSeg1.Rotate(90, Point2D(0,0)); lineSeg2.Rotate(90, Point2D(0,0)); \
    norm0.Rotate(90); norm1.Rotate(90); norm2.Rotate(90)

        BoundingLines bounds;
        bounds.AddBound(lineSeg0, norm0, false);
        bounds.AddBound(lineSeg1, norm1, false);
        bounds.AddBound(lineSeg2, norm2, false);
        ROTATE_BY_90();
        bounds.AddBound(lineSeg0, norm0, false);
        bounds.AddBound(lineSeg1, norm1, false);
        bounds.AddBound(lineSeg2, norm2, false);
        ROTATE_BY_90();
        bounds.AddBound(lineSeg0, norm0, false);
        bounds.AddBound(lineSeg1, norm1, false);
        bounds.AddBound(lineSeg2, norm2, false);
        ROTATE_BY_90();
        bounds.AddBound(lineSeg0, norm0, false);
        bounds.AddBound(lineSeg1, norm1, false);
        bounds.AddBound(lineSeg2, norm2, false);

#undef ROTATE_BY_90
        BossBodyPart* coreBody   = static_cast<BossBodyPart*>(this->bodyParts[this->coreBodyIdx]);
        coreBody->SetLocalBounds(bounds);
    }

    {
        static const float PT0_X = 0.722f;
        static const float PT0_Y = 1.351f;
        static const float PT1_X = 1.167f;
        static const float PT1_Y = 1.167f;
        static const float PT2_X = 1.351f;
        static const float PT2_Y = 0.722f;
        
        const Point2D PT3(0, 1.650f);
        const Point2D PT4(1.650f, 0);


        Collision::LineSeg2D lineSeg0(Point2D(PT0_X, PT0_Y), Point2D(PT1_X, PT1_Y));
        Collision::LineSeg2D lineSeg1(Point2D(PT1_X, PT1_Y), Point2D(PT2_X, PT2_Y));
        Collision::LineSeg2D lineSeg2(Point2D(PT0_X, PT0_Y), PT3);
        Collision::LineSeg2D lineSeg3(Point2D(PT2_X, PT2_Y), PT4);
        Vector2D norm0(PT0_Y - PT1_Y, PT1_X - PT0_X); norm0.Normalize();
        Vector2D norm1(PT1_Y - PT2_Y, PT2_X - PT1_X); norm1.Normalize();
        Vector2D norm2(PT3[1]-PT0_Y, PT0_X-PT3[0]); norm2.Normalize();
        Vector2D norm3(PT2_Y-PT4[1], PT4[0]-PT2_X); norm3.Normalize();
#define ROTATE_BY_90() lineSeg0.Rotate(90, Point2D(0,0)); lineSeg1.Rotate(90, Point2D(0,0)); \
    lineSeg2.Rotate(90, Point2D(0,0)); lineSeg3.Rotate(90, Point2D(0,0)); \
    norm0.Rotate(90); norm1.Rotate(90); norm2.Rotate(90); norm3.Rotate(90)

        BoundingLines bounds;

        bounds.AddBound(lineSeg0, norm0, false);
        bounds.AddBound(lineSeg1, norm1, false);
        bounds.AddBound(lineSeg2, norm2, true);
        bounds.AddBound(lineSeg3, norm3, true);
        ROTATE_BY_90();
        bounds.AddBound(lineSeg0, norm0, false);
        bounds.AddBound(lineSeg1, norm1, false);
        bounds.AddBound(lineSeg2, norm2, true);
        bounds.AddBound(lineSeg3, norm3, true);
        ROTATE_BY_90();
        bounds.AddBound(lineSeg0, norm0, false);
        bounds.AddBound(lineSeg1, norm1, false);
        bounds.AddBound(lineSeg2, norm2, true);
        bounds.AddBound(lineSeg3, norm3, true);
        ROTATE_BY_90();
        bounds.AddBound(lineSeg0, norm0, false);
        bounds.AddBound(lineSeg1, norm1, false);
        bounds.AddBound(lineSeg2, norm2, true);
        bounds.AddBound(lineSeg3, norm3, true);

#undef ROTATE_BY_90
        BossBodyPart* coreShield = static_cast<BossBodyPart*>(this->bodyParts[this->coreShieldIdx]);
        coreShield->SetLocalBounds(bounds);
    }
}

void FuturismBoss::RegenerateBoundsForFinalCore() {
    {
        const Point2D PT0(-2.137f, 0.340f);
        const Point2D PT1(-0.386f, 2.090f);

        const Point2D PT2(0.386f, 2.090f);
        const Point2D PT3(2.137f, 0.340f);

        const Point2D PT4(2.137f, -0.340f);
        const Point2D PT5(0.386f, -2.090f);

        const Point2D PT6(-0.386f, -2.090f);
        const Point2D PT7(-2.137f, -0.340f);

        BoundingLines coreBodyBounds;

        coreBodyBounds.AddBound(Collision::LineSeg2D(PT0, PT1), Vector2D(PT0[1]-PT1[1], PT1[0]-PT0[0]), true); // Top-left
        coreBodyBounds.AddBound(Collision::LineSeg2D(PT2, PT3), Vector2D(PT2[1]-PT3[1], PT3[0]-PT2[0]), true); // Top-right
        coreBodyBounds.AddBound(Collision::LineSeg2D(PT4, PT5), Vector2D(PT4[1]-PT5[1], PT5[0]-PT4[0]), true); // Bottom-right
        coreBodyBounds.AddBound(Collision::LineSeg2D(PT6, PT7), Vector2D(PT6[1]-PT7[1], PT7[0]-PT6[0]), true); // Bottom-left

        BossBodyPart* coreBody = static_cast<BossBodyPart*>(this->bodyParts[this->coreBodyIdx]);
        coreBody->SetLocalBounds(coreBodyBounds);
    }

    {
        const Point2D PT0(-0.386f, 2.090f);
        const Point2D PT1(-0.580f, 2.219f);
        const Point2D PT2(-0.435f, 2.562f);
        const Point2D PT3(0.435f,  2.562f);
        const Point2D PT4(0.580f,  2.219f);
        const Point2D PT5(0.386f,  2.090f);

        BoundingLines bulbBounds;

        bulbBounds.AddBound(Collision::LineSeg2D(PT0, PT1), Vector2D(PT0[1]-PT1[1], PT1[0]-PT0[0]), true);  // Left-under
        bulbBounds.AddBound(Collision::LineSeg2D(PT1, PT2), Vector2D(PT1[1]-PT2[1], PT2[0]-PT1[0]), false); // Left-side
        bulbBounds.AddBound(Collision::LineSeg2D(PT2, PT3), Vector2D(0, 1), false);                         // Top
        bulbBounds.AddBound(Collision::LineSeg2D(PT3, PT4), Vector2D(PT3[1]-PT4[1], PT4[0]-PT3[0]), false); // Right-side
        bulbBounds.AddBound(Collision::LineSeg2D(PT4, PT5), Vector2D(PT4[1]-PT5[1], PT5[0]-PT4[0]), true);  // Right-under

        BossBodyPart* topBulb = static_cast<BossBodyPart*>(this->bodyParts[this->topBulbIdx]);
        topBulb->SetLocalBounds(bulbBounds);
        BossBodyPart* leftBulb = static_cast<BossBodyPart*>(this->bodyParts[this->leftBulbIdx]);
        leftBulb->SetLocalBounds(bulbBounds);
        BossBodyPart* bottomBulb = static_cast<BossBodyPart*>(this->bodyParts[this->bottomBulbIdx]);
        bottomBulb->SetLocalBounds(bulbBounds);
        BossBodyPart* rightBulb = static_cast<BossBodyPart*>(this->bodyParts[this->rightBulbIdx]);
        rightBulb->SetLocalBounds(bulbBounds);
    }
}

size_t FuturismBoss::GetShieldIndex(ShieldLimbType type) const {
    switch (type) {
        case TopShield:
            return this->topShieldIdx;
        case BottomShield:
            return this->bottomShieldIdx;
        case LeftShield:
            return this->leftShieldIdx;
        case RightShield:
            return this->rightShieldIdx;
        case CoreShield:
            return this->coreShieldIdx;
        default:
            assert(false);
            return 0;
    }
}

void FuturismBoss::GetRocketStrategyPortalSearchPieces(const GameLevel& level, std::set<LevelPiece*>& pieces) {
    const std::vector<std::vector<LevelPiece*> >& levelPieces = level.GetCurrentLevelLayout();
    for (int row = 15; row <= 21; row++) {
        for (int col = static_cast<int>(levelPieces[row].size()-3); col < static_cast<int>(levelPieces[row].size()-1); col++) {
            pieces.insert(levelPieces[row][col]);
        }
    }
}

void FuturismBoss::GetIceStrategyPortalSearchPieces(const GameLevel& level, std::set<LevelPiece*>& pieces) {
    const std::vector<std::vector<LevelPiece*> >& levelPieces = level.GetCurrentLevelLayout();
    for (int row = 11; row <= 17; row++) {
        for (int col = 0; col <= 2; col++) {
            pieces.insert(levelPieces[row][col]);
        }
    }
}

void FuturismBoss::GetBarrierPiecesTopToBottom(const GameLevel& level, std::vector<LevelPiece*>& pieces) {

    const std::vector<std::vector<LevelPiece*> >& levelPieces = level.GetCurrentLevelLayout();
    assert(static_cast<int>(levelPieces.size()) > LEVEL_Y_OUTER_PADDING_NUM_PIECES);
    assert(!levelPieces.front().empty());
    assert(levelPieces.front().size() % 2 == 1);

    int midColIdx = static_cast<int>(levelPieces.front().size() / 2);
    int startingTopIdx = static_cast<int>(levelPieces.size()) - 1 - LEVEL_Y_OUTER_PADDING_NUM_PIECES;
    
    pieces.clear();
    pieces.reserve(levelPieces.size() - 2*LEVEL_Y_OUTER_PADDING_NUM_PIECES);
    for (int row = startingTopIdx; row >= LEVEL_Y_OUTER_PADDING_NUM_PIECES; row--) {
        pieces.push_back(levelPieces[row][midColIdx]);
    }
}

#ifdef _DEBUG
void FuturismBoss::DebugDraw() const {
    Boss::DebugDraw();

    glPushAttrib(GL_POINT_BIT | GL_CURRENT_BIT);
    glPointSize(10.0f);

    // Draw movement positions for the various AI routines
    glBegin(GL_POINTS);
    glColor4f(1, 0, 0, 1);
    for (int i = 0; i < static_cast<int>(this->leftSideMovePositions.size()); i++) {
        const Point2D& pt = this->leftSideMovePositions[i];
        glVertex2f(pt[0], pt[1]);
    }

    glColor4f(0, 1, 0, 1);
    for (int i = 0; i < static_cast<int>(this->rightSideMovePositions.size()); i++) {
        const Point2D& pt = this->rightSideMovePositions[i];
        glVertex2f(pt[0], pt[1]);
    }

    glColor4f(0, 0, 1, 1);
    for (int i = 0; i < static_cast<int>(this->fullLevelMovePositions.size()); i++) {
        const Point2D& pt = this->fullLevelMovePositions[i];
        glVertex2f(pt[0], pt[1]);
    }

    glEnd();
/*
#define DRAW_LEVEL_PIECE_LINES(piece) \
    Point2D center = piece->GetCenter(); \
    glVertex2f(center[0] - LevelPiece::HALF_PIECE_WIDTH, center[1] + LevelPiece::HALF_PIECE_HEIGHT); \
    glVertex2f(center[0] - LevelPiece::HALF_PIECE_WIDTH, center[1] - LevelPiece::HALF_PIECE_HEIGHT); \
    glVertex2f(center[0] - LevelPiece::HALF_PIECE_WIDTH, center[1] - LevelPiece::HALF_PIECE_HEIGHT); \
    glVertex2f(center[0] + LevelPiece::HALF_PIECE_WIDTH, center[1] - LevelPiece::HALF_PIECE_HEIGHT); \
    glVertex2f(center[0] + LevelPiece::HALF_PIECE_WIDTH, center[1] - LevelPiece::HALF_PIECE_HEIGHT); \
    glVertex2f(center[0] + LevelPiece::HALF_PIECE_WIDTH, center[1] + LevelPiece::HALF_PIECE_HEIGHT); \
    glVertex2f(center[0] + LevelPiece::HALF_PIECE_WIDTH, center[1] + LevelPiece::HALF_PIECE_HEIGHT); \
    glVertex2f(center[0] - LevelPiece::HALF_PIECE_WIDTH, center[1] + LevelPiece::HALF_PIECE_HEIGHT)

    glBegin(GL_LINES);
    glColor4f(1,1,0,1);
    for (std::set<LevelPiece*>::const_iterator iter = this->leftSubArenaPieces.begin(); iter != this->leftSubArenaPieces.end(); ++iter) {
        const LevelPiece* currPiece = *iter;
        DRAW_LEVEL_PIECE_LINES(currPiece);
    }
    glColor4f(0,1,1,1);
    for (std::set<LevelPiece*>::const_iterator iter = this->rightSubArenaPieces.begin(); iter != this->rightSubArenaPieces.end(); ++iter) {
        const LevelPiece* currPiece = *iter;
        DRAW_LEVEL_PIECE_LINES(currPiece);
    }
    glEnd();
*/
    glPopAttrib();
}
#endif