/**
 * DecoBoss.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "DecoBoss.h"
#include "DecoBossAIStates.h"

const float DecoBoss::SCALE_AMT = 1.4f;

const float DecoBoss::CORE_HEIGHT      = SCALE_AMT * 8.552f;
const float DecoBoss::CORE_WIDTH       = 4.988f;
const float DecoBoss::HALF_CORE_HEIGHT = CORE_HEIGHT / 2.0f;
const float DecoBoss::CORE_HALF_DEPTH  = SCALE_AMT * 1.475f;

const float DecoBoss::SIDE_BODY_PART_WIDTH  = 7.859f;
const float DecoBoss::SIDE_BODY_PART_HEIGHT = 10.723f;

const float DecoBoss::FULL_BODY_TOTAL_HEIGHT = CORE_HEIGHT + 1.771f;
const float DecoBoss::FULL_BODY_TOTAL_WIDTH  = CORE_WIDTH + 2*SIDE_BODY_PART_WIDTH;

const float DecoBoss::CORE_AND_ARMS_WIDTH = 11.264f;

const float DecoBoss::ARM_POS_X_OFFSET = SCALE_AMT * 2.475f;
const float DecoBoss::ARM_Y_OFFSET     = SCALE_AMT * -0.973f;
const float DecoBoss::ARM_Z_OFFSET     = SCALE_AMT * 0.148f;

const float DecoBoss::SIDE_POS_X_OFFSET = SCALE_AMT * 3.962f;
const float DecoBoss::SIDE_Y_OFFSET     = -3.283f;
const float DecoBoss::SIDE_Z_OFFSET     = SCALE_AMT * 0.0f;

const float DecoBoss::LIGHTNING_FIRE_POS_X = 0.0f;
const float DecoBoss::LIGHTNING_FIRE_POS_Y = -6.427f;
const float DecoBoss::LIGHTNING_FIRE_POS_Z = 0.805f;

const float DecoBoss::MOVEMENT_PADDING_X      = LevelPiece::PIECE_WIDTH;
const float DecoBoss::MOVEMENT_PADDING_Y      = 1.5f * LevelPiece::PIECE_HEIGHT;
const float DecoBoss::MOVEMENT_MIN_Y_BOUNDARY = 6.0f * LevelPiece::PIECE_HEIGHT;

const float DecoBoss::ITEM_LOAD_OFFSET_Y = -5.063f - GameItem::ITEM_HEIGHT / 2.0f;

using namespace decobossai;

DecoBoss::DecoBoss() {
}

DecoBoss::~DecoBoss() {
}

bool DecoBoss::ProjectilePassesThrough(const Projectile* projectile) const {
    switch (projectile->GetType()) {
        case Projectile::BossLaserBulletProjectile:
        case Projectile::BossRocketBulletProjectile:
        case Projectile::BossOrbBulletProjectile:
        case Projectile::BossLightningBoltBulletProjectile:
            //case Projectile::BossShockOrbBulletProjectile:
            return true;
        default:
            break;
    }

    return false;
}

void DecoBoss::Init(float startingX, float startingY) {

    // Body Layout for the Deco Boss:
    // -----------------------------------
    // root
    // |____deadPartsRoot
    // |____alivePartsRoot
    //      |____middle_body
    //           |____core
    //           |____lightning_relay
    //           |____left_arm
    //                |____gear
    //                |____scoping_arm1
    //                |____scoping_arm2
    //                |____spikey_hand
    //           |____right_arm
    //                |____gear
    //                |____scoping_arm1
    //                |____scoping_arm2
    //                |____spikey_hand
    //      |____left_body
    //      |____right_body

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

            // Middle Body
            {
                BossCompositeBodyPart* middleBody = new BossCompositeBodyPart();
                this->alivePartsRoot->AddBodyPart(middleBody);
                this->middleBodyIdx = this->bodyParts.size();
                this->bodyParts.push_back(middleBody);
                
                // Core
                {
                    Point2D PT0(-1.641f, 2.137f);
                    Point2D PT1(-1.641f, 4.062f);
                    Point2D PT2(-0.575f, 5.987f);
                    Point2D PT3(0.575f, 5.987f);
                    Point2D PT4(1.641f, 3.485f);
                    Point2D PT5(1.641f, 2.137f);

                    Point2D BTM_PT0(-2.494f, -5.987f);
                    Point2D BTM_PT1(2.494f, -5.987f);

                    BoundingLines coreBounds;
                    coreBounds.AddBound(Collision::LineSeg2D(PT0, PT1), Vector2D(-1, 0), false);
                    coreBounds.AddBound(Collision::LineSeg2D(PT1, PT2), Vector2D(PT1[1] - PT2[1], PT2[0] - PT1[0]), false);
                    coreBounds.AddBound(Collision::LineSeg2D(PT2, PT3), Vector2D(0, 1), false);
                    coreBounds.AddBound(Collision::LineSeg2D(PT3, PT4), Vector2D(PT3[1] - PT4[1], PT4[0] - PT3[0]), false);
                    coreBounds.AddBound(Collision::LineSeg2D(PT4, PT5), Vector2D(1, 0), false);
                    coreBounds.AddBound(Collision::LineSeg2D(BTM_PT0, BTM_PT1), Vector2D(0, -1), false);

                    BossBodyPart* core = new BossBodyPart(coreBounds);
                    middleBody->AddBodyPart(core);
                    this->coreIdx = this->bodyParts.size();
                    this->bodyParts.push_back(core);
                }

                // Lightning Relay
                {
                    Point2D PT0(-2.281f, -5.987f);
                    Point2D PT1(-1.369f, -6.581f);
                    Point2D PT2(1.369f, -6.581f);
                    Point2D PT3(2.281f, -5.987f);

                    BoundingLines relayBounds;
                    relayBounds.AddBound(Collision::LineSeg2D(PT0, PT1), Vector2D(PT1[1] - PT0[1], PT0[0] - PT1[0]), false);
                    relayBounds.AddBound(Collision::LineSeg2D(PT1, PT2), Vector2D(0, -1), false);
                    relayBounds.AddBound(Collision::LineSeg2D(PT2, PT3), Vector2D(PT3[1] - PT2[1], PT2[0] - PT3[0]), false);

                    BossBodyPart* lightningRelay = new BossBodyPart(relayBounds);
                    middleBody->AddBodyPart(lightningRelay);
                    this->lightningRelayIdx = this->bodyParts.size();
                    this->bodyParts.push_back(lightningRelay);
                }

                // Left Arm
                {
                    BossCompositeBodyPart* leftArm = this->BuildArm(middleBody, leftArmIdx, leftArmGearIdx, 
                        leftArmScopingSeg1Idx, leftArmScopingSeg2Idx, leftArmHandIdx);
                    assert(leftArm != NULL);

                    leftArm->Translate(Vector3D(-ARM_POS_X_OFFSET, ARM_Y_OFFSET, ARM_Z_OFFSET));
                }

                // Right Arm
                {
                    BossCompositeBodyPart* rightArm = this->BuildArm(middleBody, rightArmIdx, rightArmGearIdx, 
                        rightArmScopingSeg1Idx, rightArmScopingSeg2Idx, rightArmHandIdx);
                    assert(rightArm != NULL);

                    rightArm->Translate(Vector3D(ARM_POS_X_OFFSET, ARM_Y_OFFSET, ARM_Z_OFFSET));
                }
            }

            // Left and Right Body
            {
                // Initially all points correspond to the left body...
                Point2D PT0(3.946f, 6.249f);
                Point2D PT1(2.479f, 6.249f);
                Point2D PT2(1.404f, 4.272f);
                Point2D PT3(-1.235f, 4.272f);
                Point2D PT4(-2.835f, 3.912f);
                Point2D PT5(-3.913f, 2.218f);
                Point2D PT6(-3.913f, -3.396f);
                Point2D PT7(-2.835f, -4.474f);
                Point2D PT8(-2.065f, -4.474f);
                Point2D PT9(-0.833f, -3.242f);
                Point2D PT10(1.708f, -3.242f);
                Point2D PT11(3.094f, -2.703f);

                BoundingLines sideBodyBounds;
                sideBodyBounds.AddBound(Collision::LineSeg2D(PT0, PT1), Vector2D(0, 1), false);
                sideBodyBounds.AddBound(Collision::LineSeg2D(PT1, PT2), Vector2D(PT2[1] - PT1[1], PT1[0] - PT2[0]), false);
                sideBodyBounds.AddBound(Collision::LineSeg2D(PT2, PT3), Vector2D(0, 1), false);
                sideBodyBounds.AddBound(Collision::LineSeg2D(PT3, PT4), Vector2D(PT4[1] - PT3[1], PT3[0] - PT4[0]), false);
                sideBodyBounds.AddBound(Collision::LineSeg2D(PT4, PT5), Vector2D(PT5[1] - PT4[1], PT4[0] - PT5[0]), false);
                sideBodyBounds.AddBound(Collision::LineSeg2D(PT5, PT6), Vector2D(-1, 0), false);
                sideBodyBounds.AddBound(Collision::LineSeg2D(PT6, PT7), Vector2D(PT7[1] - PT6[1], PT6[0] - PT7[0]), false);
                sideBodyBounds.AddBound(Collision::LineSeg2D(PT7, PT8), Vector2D(0, -1), false);
                sideBodyBounds.AddBound(Collision::LineSeg2D(PT8, PT9), Vector2D(PT9[1] - PT8[1], PT8[0] - PT9[0]), false);
                sideBodyBounds.AddBound(Collision::LineSeg2D(PT9, PT10), Vector2D(0, -1), false);
                sideBodyBounds.AddBound(Collision::LineSeg2D(PT10, PT11), Vector2D(PT11[1] - PT10[1], PT10[0] - PT11[0]), false);

                // Left Body
                {
                    BossBodyPart* leftBody = new BossBodyPart(sideBodyBounds);
                    alivePartsRoot->AddBodyPart(leftBody);
                    this->leftBodyIdx = this->bodyParts.size();
                    this->bodyParts.push_back(leftBody);
                    leftBody->Translate(Vector3D(-SIDE_POS_X_OFFSET, SIDE_Y_OFFSET, SIDE_Z_OFFSET));
                }

                sideBodyBounds.ReflectX();
                sideBodyBounds.TranslateBounds(Vector2D(SCALE_AMT * 0.034f, SCALE_AMT * 0.0f));

                // Right Body
                {
                    BossBodyPart* rightBody = new BossBodyPart(sideBodyBounds);
                    alivePartsRoot->AddBodyPart(rightBody);
                    this->rightBodyIdx = this->bodyParts.size();
                    this->bodyParts.push_back(rightBody);
                    rightBody->Translate(Vector3D(SIDE_POS_X_OFFSET, SIDE_Y_OFFSET, SIDE_Z_OFFSET));
                }
            }
        }
    }

    // Move the boss to its starting location
    //this->root->Transform(Matrix4x4::scaleMatrix(Vector3D(1.4f, 1.4f, 1.4f)));
    this->root->Translate(Vector3D(startingX, startingY, 0.0f));
    

    this->SetNextAIState(new Stage1AI(this));
    // N.B., Bosses are transformed into level space by the GameLevel when they are loaded from file.
}

BossCompositeBodyPart* DecoBoss::BuildArm(BossCompositeBodyPart* middleBody, size_t& armIdx, size_t& gearIdx, 
                                          size_t& scoping1Idx, size_t& scoping2Idx, size_t& handIdx) {
    
    BossCompositeBodyPart* arm = new BossCompositeBodyPart();
    middleBody->AddBodyPart(arm);
    armIdx = this->bodyParts.size();
    this->bodyParts.push_back(arm);

    // Gear
    {
        BossBodyPart* gear = new BossBodyPart(BoundingLines());
        arm->AddBodyPart(gear);
        gearIdx = this->bodyParts.size();
        this->bodyParts.push_back(gear);
    }

    // Scoping Arm Segment 1
    {
        BossBodyPart* scopingArmSeg1 = new BossBodyPart(BoundingLines());
        arm->AddBodyPart(scopingArmSeg1);
        scoping1Idx = this->bodyParts.size();
        this->bodyParts.push_back(scopingArmSeg1);
    }

    // Scoping Arm Segment 2
    {
        BossBodyPart* scopingArmSeg2 = new BossBodyPart(BoundingLines());
        arm->AddBodyPart(scopingArmSeg2);
        scoping2Idx = this->bodyParts.size();
        this->bodyParts.push_back(scopingArmSeg2);
    }

    // Spikey Hand
    {
        BossBodyPart* hand = new BossBodyPart(BoundingLines());
        arm->AddBodyPart(hand);
        handIdx = this->bodyParts.size();
        this->bodyParts.push_back(hand);
    }

    return arm;
}