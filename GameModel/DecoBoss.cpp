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


const float DecoBoss::CORE_HEIGHT      = 8.552f;
const float DecoBoss::HALF_CORE_HEIGHT = CORE_HEIGHT / 2.0f;

const float DecoBoss::ARM_POS_X_OFFSET = 2.475f;
const float DecoBoss::ARM_Y_OFFSET     = -0.973f;
const float DecoBoss::ARM_Z_OFFSET     = 0.148f;

DecoBoss::DecoBoss() {
}

DecoBoss::~DecoBoss() {
}

bool DecoBoss::ProjectilePassesThrough(const Projectile* projectile) const {
    switch (projectile->GetType()) {
        case Projectile::BossLaserBulletProjectile:
        case Projectile::BossRocketBulletProjectile:
        case Projectile::BossOrbBulletProjectile:
            //case Projectile::BossShockOrbBulletProjectile:
            //case Projectile::BossLightningBurstBulletProjectile:
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
                    Point2D PT0(-1.172f, 1.526f);
                    Point2D PT1(-1.172f, 2.901f);
                    Point2D PT2(-0.410f, 4.276f);
                    Point2D PT3(0.410f, 4.276f);
                    Point2D PT4(1.172f, 2.489f);
                    Point2D PT5(1.172f, 1.526f);

                    Point2D BTM_PT0(-1.782f, -4.166f);
                    Point2D BTM_PT1(1.782f, -4.166f);

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
                    Point2D PT0(-1.409f, -4.276f);
                    Point2D PT1(-0.749f, -4.881f);
                    Point2D PT2(0.749f, -4.881f);
                    Point2D PT3(1.409f, -4.276f);

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
                Point2D PT0(2.824f, 4.729f);
                Point2D PT1(1.776f, 4.729f);
                Point2D PT2(1.008f, 3.316f);
                Point2D PT3(-0.877f, 3.316f);
                Point2D PT4(-2.168f, 3.035f);
                Point2D PT5(-2.790f, 1.849f);
                Point2D PT6(-2.790f, -2.161f);
                Point2D PT7(-2.020f, -2.931f);
                Point2D PT8(-1.470f, -2.931f);
                Point2D PT9(-0.590f, -2.051f);
                Point2D PT10(2.215f, -2.051f);

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

                // Left Body
                {
                    BossBodyPart* leftBody = new BossBodyPart(sideBodyBounds);
                    alivePartsRoot->AddBodyPart(leftBody);
                    this->leftBodyIdx = this->bodyParts.size();
                    this->bodyParts.push_back(leftBody);
                }

                sideBodyBounds.ReflectX();
                sideBodyBounds.TranslateBounds(Vector2D(0.034f, 0.0f));

                // Right Body
                {
                    BossBodyPart* rightBody = new BossBodyPart(sideBodyBounds);
                    alivePartsRoot->AddBodyPart(rightBody);
                    this->rightBodyIdx = this->bodyParts.size();
                    this->bodyParts.push_back(rightBody);
                }
            }
        }
    }

    // Move the boss to its starting location
    this->root->Translate(Vector3D(startingX, startingY, 0.0f));

    //this->SetNextAIState(new Stage1AI(this));
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