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

const float DecoBoss::LIGHTNING_RELAY_GLOW_SIZE = 1.6f;

const float DecoBoss::ARM_WIDTH               = 4.334f;
const float DecoBoss::ARM_NOT_EXTENDED_HEIGHT = 6.382f;

const float DecoBoss::ARM_POS_X_OFFSET = 3.465f;
const float DecoBoss::ARM_Y_OFFSET     = SCALE_AMT * -0.973f;
const float DecoBoss::ARM_Z_OFFSET     = SCALE_AMT * 0.148f;

const float DecoBoss::ARM_SCOPING_SEG1_Y_RETRACT_TRANSLATION = 3.098f;
const float DecoBoss::ARM_SCOPING_SEG2_Y_RETRACT_TRANSLATION = ARM_SCOPING_SEG1_Y_RETRACT_TRANSLATION;
const float DecoBoss::ARM_SCOPING_SEG3_Y_RETRACT_TRANSLATION = ARM_SCOPING_SEG1_Y_RETRACT_TRANSLATION;
const float DecoBoss::ARM_SCOPING_SEG4_Y_RETRACT_TRANSLATION = 2.218f;

const float DecoBoss::ARM_ORIGIN_TO_HAND_END = 15.285f;
const float DecoBoss::HAND_WIDTH = 3.025f;

const float DecoBoss::SIDE_POS_X_OFFSET = SCALE_AMT * 3.962f;
const float DecoBoss::SIDE_Y_OFFSET     = -3.283f;
const float DecoBoss::SIDE_Z_OFFSET     = SCALE_AMT * 0.0f;

const float DecoBoss::LIGHTNING_FIRE_POS_X = 0.0f;
const float DecoBoss::LIGHTNING_FIRE_POS_Y = -6.427f;
const float DecoBoss::LIGHTNING_FIRE_POS_Z = 0.805f;

const float DecoBoss::MOVEMENT_PADDING_X      = LevelPiece::PIECE_WIDTH;
const float DecoBoss::MOVEMENT_PADDING_Y      = 1.5f * LevelPiece::PIECE_HEIGHT;
const float DecoBoss::MOVEMENT_MIN_Y_BOUNDARY = 7.0f * LevelPiece::PIECE_HEIGHT;

const float DecoBoss::ROTATION_Y_POSITION = 19.0f * LevelPiece::PIECE_HEIGHT;

const float DecoBoss::LEFT_ARM_HORIZ_ORIENT_ROT_ANGLE_IN_DEGS  = -90.0f;
const float DecoBoss::RIGHT_ARM_HORIZ_ORIENT_ROT_ANGLE_IN_DEGS = 90.0f;

const float DecoBoss::ITEM_LOAD_OFFSET_Y = -5.063f - GameItem::ITEM_HEIGHT / 2.0f;

const float DecoBoss::EYE1_OFFSET_X   = 0.185f;
const float DecoBoss::EYE2_OFFSET_X   = -0.185f;
const float DecoBoss::EYE1_2_OFFSET_Y = 0.8f;
const float DecoBoss::EYE3_OFFSET_Y   = 0.4f;
const float DecoBoss::EYE4_OFFSET_Y   = 0.0f;
const float DecoBoss::EYE5_OFFSET_Y   = -0.4f;
const float DecoBoss::EYE6_OFFSET_Y   = -0.8f;

const float DecoBoss::ATTACK_PADDLE_WITH_BOTH_ARMS_MIN_Y_POS = 14.0f * LevelPiece::PIECE_HEIGHT - (2.0f * LevelPiece::PIECE_HEIGHT);
const float DecoBoss::ATTACK_PADDLE_WITH_BOTH_ARMS_MAX_Y_POS = 14.0f * LevelPiece::PIECE_HEIGHT + (2.0f * LevelPiece::PIECE_HEIGHT);

const float DecoBoss::ATTACK_PADDLE_WITH_ONE_ARM_MIN_Y_POS = 12.0f * LevelPiece::PIECE_HEIGHT - (2.0f * LevelPiece::PIECE_HEIGHT);;
const float DecoBoss::ATTACK_PADDLE_WITH_ONE_ARM_MAX_Y_POS = 12.0f * LevelPiece::PIECE_HEIGHT + (1.0f * LevelPiece::PIECE_HEIGHT);

using namespace decobossai;

DecoBoss::DecoBoss() {
    this->eyeOffsets.reserve(NUM_EYES);
    this->eyeOffsets.push_back(Vector2D(EYE1_OFFSET_X, EYE1_2_OFFSET_Y));
    this->eyeOffsets.push_back(Vector2D(EYE2_OFFSET_X, EYE1_2_OFFSET_Y));
    this->eyeOffsets.push_back(Vector2D(0, EYE3_OFFSET_Y));
    this->eyeOffsets.push_back(Vector2D(0, EYE4_OFFSET_Y));
    this->eyeOffsets.push_back(Vector2D(0, EYE5_OFFSET_Y));
    this->eyeOffsets.push_back(Vector2D(0, EYE6_OFFSET_Y));
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
    //                |____scoping_arm3
    //                |____scoping_arm4
    //                |____hand
    //           |____right_arm
    //                |____gear
    //                |____scoping_arm1
    //                |____scoping_arm2
    //                |____scoping_arm3
    //                |____scoping_arm4
    //                |____hand
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
                        leftArmScopingSeg1Idx, leftArmScopingSeg2Idx, leftArmScopingSeg3Idx, leftArmScopingSeg4Idx, leftArmHandIdx);
                    assert(leftArm != NULL);

                    leftArm->Translate(Vector3D(-ARM_POS_X_OFFSET, ARM_Y_OFFSET, ARM_Z_OFFSET));
                }

                // Right Arm
                {
                    BossCompositeBodyPart* rightArm = this->BuildArm(middleBody, rightArmIdx, rightArmGearIdx, 
                        rightArmScopingSeg1Idx, rightArmScopingSeg2Idx, rightArmScopingSeg3Idx, rightArmScopingSeg4Idx, rightArmHandIdx);
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
    this->root->Translate(Vector3D(startingX, startingY, 0.0f));
    
    // Retract the bosses' arms so that they initially hidden by the two side parts of the bosses' body
    AbstractBossBodyPart* leftArmSeg1 = this->bodyParts[leftArmScopingSeg1Idx];
    AbstractBossBodyPart* leftArmSeg2 = this->bodyParts[leftArmScopingSeg2Idx];
    AbstractBossBodyPart* leftArmSeg3 = this->bodyParts[leftArmScopingSeg3Idx];
    AbstractBossBodyPart* leftArmSeg4 = this->bodyParts[leftArmScopingSeg4Idx];
    AbstractBossBodyPart* leftArmHand = this->bodyParts[leftArmHandIdx];
    leftArmSeg1->Translate(Vector3D(0, ARM_SCOPING_SEG1_Y_RETRACT_TRANSLATION, 0));
    leftArmSeg2->Translate(Vector3D(0, ARM_SCOPING_SEG1_Y_RETRACT_TRANSLATION + ARM_SCOPING_SEG2_Y_RETRACT_TRANSLATION, 0));
    leftArmSeg3->Translate(Vector3D(0, ARM_SCOPING_SEG1_Y_RETRACT_TRANSLATION + ARM_SCOPING_SEG2_Y_RETRACT_TRANSLATION + ARM_SCOPING_SEG3_Y_RETRACT_TRANSLATION, 0));
    leftArmSeg4->Translate(Vector3D(0, ARM_SCOPING_SEG1_Y_RETRACT_TRANSLATION + ARM_SCOPING_SEG2_Y_RETRACT_TRANSLATION + 
        ARM_SCOPING_SEG3_Y_RETRACT_TRANSLATION + ARM_SCOPING_SEG4_Y_RETRACT_TRANSLATION, 0));
    leftArmHand->Translate(Vector3D(0, ARM_SCOPING_SEG1_Y_RETRACT_TRANSLATION + ARM_SCOPING_SEG2_Y_RETRACT_TRANSLATION + 
        ARM_SCOPING_SEG3_Y_RETRACT_TRANSLATION + ARM_SCOPING_SEG4_Y_RETRACT_TRANSLATION, 0));

    AbstractBossBodyPart* rightArmSeg1 = this->bodyParts[rightArmScopingSeg1Idx];
    AbstractBossBodyPart* rightArmSeg2 = this->bodyParts[rightArmScopingSeg2Idx];
    AbstractBossBodyPart* rightArmSeg3 = this->bodyParts[rightArmScopingSeg3Idx];
    AbstractBossBodyPart* rightArmSeg4 = this->bodyParts[rightArmScopingSeg4Idx];
    AbstractBossBodyPart* rightArmHand = this->bodyParts[rightArmHandIdx];
    rightArmSeg1->Translate(Vector3D(0, ARM_SCOPING_SEG1_Y_RETRACT_TRANSLATION, 0));
    rightArmSeg2->Translate(Vector3D(0, ARM_SCOPING_SEG1_Y_RETRACT_TRANSLATION + ARM_SCOPING_SEG2_Y_RETRACT_TRANSLATION, 0));
    rightArmSeg3->Translate(Vector3D(0, ARM_SCOPING_SEG1_Y_RETRACT_TRANSLATION + ARM_SCOPING_SEG2_Y_RETRACT_TRANSLATION + ARM_SCOPING_SEG3_Y_RETRACT_TRANSLATION, 0));
    rightArmSeg4->Translate(Vector3D(0, ARM_SCOPING_SEG1_Y_RETRACT_TRANSLATION + ARM_SCOPING_SEG2_Y_RETRACT_TRANSLATION + 
        ARM_SCOPING_SEG3_Y_RETRACT_TRANSLATION + ARM_SCOPING_SEG4_Y_RETRACT_TRANSLATION, 0));
    rightArmHand->Translate(Vector3D(0, ARM_SCOPING_SEG1_Y_RETRACT_TRANSLATION + ARM_SCOPING_SEG2_Y_RETRACT_TRANSLATION + 
        ARM_SCOPING_SEG3_Y_RETRACT_TRANSLATION + ARM_SCOPING_SEG4_Y_RETRACT_TRANSLATION, 0));

    this->SetNextAIState(new Stage1AI(this));
    // N.B., Bosses are transformed into level space by the GameLevel when they are loaded from file.
}

BossCompositeBodyPart* DecoBoss::BuildArm(BossCompositeBodyPart* middleBody, size_t& armIdx, size_t& gearIdx, 
                                          size_t& scoping1Idx, size_t& scoping2Idx, size_t& scoping3Idx, 
                                          size_t& scoping4Idx, size_t& handIdx) {
    
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

    // Scoping Arm Segment 3
    {
        BossBodyPart* scopingArmSeg3 = new BossBodyPart(BoundingLines());
        arm->AddBodyPart(scopingArmSeg3);
        scoping3Idx = this->bodyParts.size();
        this->bodyParts.push_back(scopingArmSeg3);
    }

    // Scoping Arm Segment 4
    {
        BossBodyPart* scopingArmSeg4 = new BossBodyPart(BoundingLines());
        arm->AddBodyPart(scopingArmSeg4);
        scoping4Idx = this->bodyParts.size();
        this->bodyParts.push_back(scopingArmSeg4);
    }

    // Hand
    {
        BossBodyPart* hand = new BossBodyPart(BoundingLines());
        arm->AddBodyPart(hand);
        handIdx = this->bodyParts.size();
        this->bodyParts.push_back(hand);
    }

    return arm;
}

void DecoBoss::GenerateArmAndRemainingBoundingLines() {
    static const float GEAR_MIN_Y = -2.890f;
    // Arm Gears
    {
        const Point2D PT0(0.824f, GEAR_MIN_Y);
        const Point2D PT1(0.824f, -1.687f);
        const Point2D PT2(1.877f, -1.084f);
        const Point2D PT3(1.877f, 1.084f);
        const Point2D PT4(0.0f, 2.177f);
        const Point2D PT5(-1.877f, 1.084f);
        const Point2D PT6(-1.877f, -1.084f);
        const Point2D PT7(-0.824f, -1.687f);
        const Point2D PT8(-0.824f, GEAR_MIN_Y);

        BoundingLines gearBoundingLines;
        gearBoundingLines.AddBound(Collision::LineSeg2D(PT0, PT1), Vector2D(1, 0), false);
        gearBoundingLines.AddBound(Collision::LineSeg2D(PT1, PT2), Vector2D(PT2[1] - PT1[1], PT1[0] - PT2[0]), false);
        gearBoundingLines.AddBound(Collision::LineSeg2D(PT2, PT3), Vector2D(1, 0), false);
        gearBoundingLines.AddBound(Collision::LineSeg2D(PT3, PT4), Vector2D(PT4[1] - PT3[1], PT3[0] - PT4[0]), false);
        gearBoundingLines.AddBound(Collision::LineSeg2D(PT4, PT5), Vector2D(PT5[1] - PT4[1], PT4[0] - PT5[0]), false);
        gearBoundingLines.AddBound(Collision::LineSeg2D(PT5, PT6), Vector2D(-1, 0), false);
        gearBoundingLines.AddBound(Collision::LineSeg2D(PT6, PT7), Vector2D(PT7[1] - PT6[1], PT6[0] - PT7[0]), false);
        gearBoundingLines.AddBound(Collision::LineSeg2D(PT7, PT8), Vector2D(-1, 0), false);

        BossBodyPart* leftArmGear  = static_cast<BossBodyPart*>(this->bodyParts[this->leftArmGearIdx]);
        BossBodyPart* rightArmGear = static_cast<BossBodyPart*>(this->bodyParts[this->rightArmGearIdx]);
        leftArmGear->SetLocalBounds(gearBoundingLines);
        rightArmGear->SetLocalBounds(gearBoundingLines);
    }

    static const float SEG1_MIN_Y = GEAR_MIN_Y - ARM_SCOPING_SEG1_Y_RETRACT_TRANSLATION;
    // Arm Scoping Segments (1)
    {
        const Point2D P0L(-0.785f, GEAR_MIN_Y);
        const Point2D P1L(-0.785f, SEG1_MIN_Y);
        const Point2D P0R(0.785f, GEAR_MIN_Y);
        const Point2D P1R(0.785f, SEG1_MIN_Y);

        BoundingLines seg1BoundingLines;
        seg1BoundingLines.AddBound(Collision::LineSeg2D(P0L, P1L), Vector2D(-1, 0), true);
        seg1BoundingLines.AddBound(Collision::LineSeg2D(P0R, P1R), Vector2D(1, 0), true);

        BossBodyPart* leftArmSeg1  = static_cast<BossBodyPart*>(this->bodyParts[leftArmScopingSeg1Idx]);
        BossBodyPart* rightArmSeg1 = static_cast<BossBodyPart*>(this->bodyParts[rightArmScopingSeg1Idx]);
        leftArmSeg1->SetLocalBounds(seg1BoundingLines);
        rightArmSeg1->SetLocalBounds(seg1BoundingLines);
    }
    
    static const float SEG2_MIN_Y = SEG1_MIN_Y - ARM_SCOPING_SEG2_Y_RETRACT_TRANSLATION;
    // Arm Scoping Segments (2) 
    {
        const Point2D P0L(-0.751f, SEG1_MIN_Y);
        const Point2D P1L(-0.751f, SEG2_MIN_Y);
        const Point2D P0R(0.751f,  SEG1_MIN_Y);
        const Point2D P1R(0.751f,  SEG2_MIN_Y);

        BoundingLines seg2BoundingLines;
        seg2BoundingLines.AddBound(Collision::LineSeg2D(P0L, P1L), Vector2D(-1, 0), true);
        seg2BoundingLines.AddBound(Collision::LineSeg2D(P0R, P1R), Vector2D(1, 0), true);

        BossBodyPart* leftArmSeg2  = static_cast<BossBodyPart*>(this->bodyParts[leftArmScopingSeg2Idx]);
        BossBodyPart* rightArmSeg2 = static_cast<BossBodyPart*>(this->bodyParts[rightArmScopingSeg2Idx]);
        leftArmSeg2->SetLocalBounds(seg2BoundingLines);
        rightArmSeg2->SetLocalBounds(seg2BoundingLines);
    }
    
    static const float SEG3_MIN_Y = SEG2_MIN_Y - ARM_SCOPING_SEG3_Y_RETRACT_TRANSLATION;
    // Arm Scoping Segments (3) 
    {
        const Point2D P0L(-0.701f, SEG2_MIN_Y);
        const Point2D P1L(-0.701f, SEG3_MIN_Y);
        const Point2D P0R(0.701f,  SEG2_MIN_Y);
        const Point2D P1R(0.701f,  SEG3_MIN_Y);

        BoundingLines seg3BoundingLines;
        seg3BoundingLines.AddBound(Collision::LineSeg2D(P0L, P1L), Vector2D(-1, 0), true);
        seg3BoundingLines.AddBound(Collision::LineSeg2D(P0R, P1R), Vector2D(1, 0), true);

        BossBodyPart* leftArmSeg3  = static_cast<BossBodyPart*>(this->bodyParts[leftArmScopingSeg3Idx]);
        BossBodyPart* rightArmSeg3 = static_cast<BossBodyPart*>(this->bodyParts[rightArmScopingSeg3Idx]);
        leftArmSeg3->SetLocalBounds(seg3BoundingLines);
        rightArmSeg3->SetLocalBounds(seg3BoundingLines);
    }

    static const float SEG4_MIN_Y = SEG3_MIN_Y - ARM_SCOPING_SEG4_Y_RETRACT_TRANSLATION;
    // Arm Scoping Segments (3) 
    {
        const Point2D P0L(-0.651f, SEG3_MIN_Y);
        const Point2D P1L(-0.651f, SEG4_MIN_Y);
        const Point2D P0R(0.651f,  SEG3_MIN_Y);
        const Point2D P1R(0.651f,  SEG4_MIN_Y);

        BoundingLines seg4BoundingLines;
        seg4BoundingLines.AddBound(Collision::LineSeg2D(P0L, P1L), Vector2D(-1, 0), true);
        seg4BoundingLines.AddBound(Collision::LineSeg2D(P0R, P1R), Vector2D(1, 0), true);

        BossBodyPart* leftArmSeg4  = static_cast<BossBodyPart*>(this->bodyParts[leftArmScopingSeg4Idx]);
        BossBodyPart* rightArmSeg4 = static_cast<BossBodyPart*>(this->bodyParts[rightArmScopingSeg4Idx]);
        leftArmSeg4->SetLocalBounds(seg4BoundingLines);
        rightArmSeg4->SetLocalBounds(seg4BoundingLines);
    }

    // Arm Hands
    {
        const Point2D P0(-0.750f, -14.401f);
        const Point2D P1(-1.399f, -15.285f);
        const Point2D P2(1.399f, -15.285f);
        const Point2D P3(0.750f, -14.401f);

        BoundingLines handBoundingLines;
        handBoundingLines.AddBound(Collision::LineSeg2D(P0, P1), Vector2D(P1[1] - P0[1], P0[0] - P1[0]), false);
        handBoundingLines.AddBound(Collision::LineSeg2D(P1, P2), Vector2D(0, -1), false);
        handBoundingLines.AddBound(Collision::LineSeg2D(P2, P3), Vector2D(P3[1] - P2[1], P2[0] - P3[0]), false);

        BossBodyPart* leftArmHand  = static_cast<BossBodyPart*>(this->bodyParts[leftArmHandIdx]);
        BossBodyPart* rightArmHand = static_cast<BossBodyPart*>(this->bodyParts[rightArmHandIdx]);
        leftArmHand->SetLocalBounds(handBoundingLines);
        rightArmHand->SetLocalBounds(handBoundingLines);
    }

    // Remaining body bounding lines
    {
        Point2D PT0L(-1.641f, 2.137f);
        Point2D PT1L(-2.494f, 2.137f);
        Point2D PT2L(-2.494f, -5.987f);

        Point2D PT0R(1.641f, 2.137f);
        Point2D PT1R(2.494f, 2.137f);
        Point2D PT2R(2.494f, -5.987f);
            
        BossBodyPart* core = this->GetCoreEditable();
        BoundingLines coreBoundingLines = core->GetLocalBounds();

        coreBoundingLines.AddBound(Collision::LineSeg2D(PT0L, PT1L), Vector2D(0, 1), false);
        coreBoundingLines.AddBound(Collision::LineSeg2D(PT1L, PT2L), Vector2D(-1, 0), false);
        coreBoundingLines.AddBound(Collision::LineSeg2D(PT0R, PT1R), Vector2D(0, 1), false);
        coreBoundingLines.AddBound(Collision::LineSeg2D(PT1R, PT2R), Vector2D(1, 0), false);

        core->SetLocalBounds(coreBoundingLines);
    }
}

float DecoBoss::GetRandomAttackPaddleYPos() const {
    // Depends on whether we're attacking with both arms or not...
    bool isLeftArmAlive  = !this->GetLeftArm()->GetIsDestroyed();
    bool isRightArmAlive = !this->GetRightArm()->GetIsDestroyed();

    if (isLeftArmAlive && isRightArmAlive) {
        return ATTACK_PADDLE_WITH_BOTH_ARMS_MIN_Y_POS + Randomizer::GetInstance()->RandomNumZeroToOne() *
            (ATTACK_PADDLE_WITH_BOTH_ARMS_MAX_Y_POS - ATTACK_PADDLE_WITH_BOTH_ARMS_MIN_Y_POS);
    }
    
    // One of the arms is dead, lower the y position of the attack...
    return ATTACK_PADDLE_WITH_ONE_ARM_MIN_Y_POS + Randomizer::GetInstance()->RandomNumZeroToOne() *
        (ATTACK_PADDLE_WITH_ONE_ARM_MAX_Y_POS - ATTACK_PADDLE_WITH_ONE_ARM_MIN_Y_POS);
}