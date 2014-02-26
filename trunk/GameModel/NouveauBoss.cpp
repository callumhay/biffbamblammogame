/**
 * NouveauBoss.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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

#include "NouveauBoss.h"
#include "NouveauBossAIStates.h"
#include "TargetedRefractiveBossBodyPart.h"

using namespace nouveaubossai;

const float NouveauBoss::BODY_CORE_HEIGHT         = 4.719f;
const float NouveauBoss::BODY_CORE_TOP_WIDTH      = 4.236f;
const float NouveauBoss::BODY_CORE_BOTTOM_WIDTH   = 10.7f;
const float NouveauBoss::BODY_CORE_MID_HALF_DEPTH = 8.463f / 2.0f;

const float NouveauBoss::TOP_ENCLOSURE_GAZEBO_HEIGHT  = 2.480f;
const float NouveauBoss::TOP_ENCLOSURE_GAZEBO_WIDTH   = 3.972f;
const float NouveauBoss::TOP_ENCLOSURE_DOME_HEIGHT    = 1.555f;
const float NouveauBoss::TOP_ENCLOSURE_DOME_TOP_WIDTH = 1.312f;
const float NouveauBoss::TOP_ENCLOSURE_DOME_BOTTOM_WIDTH = 3.611f;
const float NouveauBoss::TOP_SPHERE_WIDTH  = 2.687f;
const float NouveauBoss::TOP_SPHERE_HEIGHT = 2.780f;

const float NouveauBoss::ARM_SPHERE_HOLDER_CURL_WIDTH = 5.485f;
const float NouveauBoss::ARM_SPHERE_HOLDER_CURL_HEIGHT = 1.8f;

const float NouveauBoss::BOTTOM_SPHERE_LASER_BEAM_RADIUS    = 1.168f;
const float NouveauBoss::SIDE_CURL_TOP_LASER_BEAM_RADIUS    = 0.3f;
const float NouveauBoss::SIDE_CURL_BOTTOM_LASER_BEAM_RADIUS = 0.35f;

const float NouveauBoss::MIN_X_BOUNDS_WITH_PADDING = (LEVEL_NUM_PIECES_WIDTH  - 19) * LevelPiece::PIECE_WIDTH  + 3.75f * GameBall::DEFAULT_BALL_RADIUS;
const float NouveauBoss::MAX_X_BOUNDS_WITH_PADDING = (LEVEL_NUM_PIECES_WIDTH  - 6)  * LevelPiece::PIECE_WIDTH  - 3.75f * GameBall::DEFAULT_BALL_RADIUS;
const float NouveauBoss::MIN_Y_BOUNDS_WITH_PADDING = (LEVEL_NUM_PIECES_HEIGHT - 24) * LevelPiece::PIECE_HEIGHT;
const float NouveauBoss::MAX_Y_BOUNDS_WITH_PADDING = (LEVEL_NUM_PIECES_HEIGHT - 3)  * LevelPiece::PIECE_HEIGHT - 2.5f * GameBall::DEFAULT_BALL_RADIUS;

const float NouveauBoss::LEFT_SIDE_CURL_SHOOT1_OFFSET_X = -2.359f;
const float NouveauBoss::LEFT_SIDE_CURL_SHOOT1_OFFSET_Y = -0.366f;
const float NouveauBoss::LEFT_SIDE_CURL_SHOOT2_OFFSET_X = -4.470f;
const float NouveauBoss::LEFT_SIDE_CURL_SHOOT2_OFFSET_Y = -1.267f;

const float NouveauBoss::RIGHT_SIDE_CURL_SHOOT1_OFFSET_X = -NouveauBoss::LEFT_SIDE_CURL_SHOOT1_OFFSET_X;
const float NouveauBoss::RIGHT_SIDE_CURL_SHOOT1_OFFSET_Y =  NouveauBoss::LEFT_SIDE_CURL_SHOOT1_OFFSET_Y;
const float NouveauBoss::RIGHT_SIDE_CURL_SHOOT2_OFFSET_X = -NouveauBoss::LEFT_SIDE_CURL_SHOOT2_OFFSET_X;
const float NouveauBoss::RIGHT_SIDE_CURL_SHOOT2_OFFSET_Y =  NouveauBoss::LEFT_SIDE_CURL_SHOOT2_OFFSET_Y;

const float NouveauBoss::SIDE_CURL_SHOOT_OFFSET_Z = 1.0f;
const float NouveauBoss::BOTTOM_SPHERE_SHOOT_OFFSET_Y = -2.250f;

const float NouveauBoss::ARM_CURL_CENTER_X_OFFSET = 2.931f;
const float NouveauBoss::ARM_CURL_CENTER_Y_OFFSET = 0.995f;

const float NouveauBoss::TOP_SPHERE_CENTER_Y_OFFSET = 0.692f;

const std::pair<int,int> NouveauBoss::SIDE_PRISM_IDX_PAIRS[NUM_SIDE_PRISMS] = {
    std::make_pair(LEVEL_NUM_PIECES_HEIGHT-6,  4), std::make_pair(LEVEL_NUM_PIECES_HEIGHT-6,  LEVEL_NUM_PIECES_WIDTH-5),
    std::make_pair(LEVEL_NUM_PIECES_HEIGHT-11, 4), std::make_pair(LEVEL_NUM_PIECES_HEIGHT-11, LEVEL_NUM_PIECES_WIDTH-5),
    std::make_pair(LEVEL_NUM_PIECES_HEIGHT-16, 4), std::make_pair(LEVEL_NUM_PIECES_HEIGHT-16, LEVEL_NUM_PIECES_WIDTH-5),
    std::make_pair(LEVEL_NUM_PIECES_HEIGHT-21, 4), std::make_pair(LEVEL_NUM_PIECES_HEIGHT-21, LEVEL_NUM_PIECES_WIDTH-5)
};

const std::pair<int,int> NouveauBoss::LEFT_SIDE_PRISM_IDX_PAIRS[NUM_LEFT_SIDE_PRISMS] = {
    std::make_pair(LEVEL_NUM_PIECES_HEIGHT-6,  4),
    std::make_pair(LEVEL_NUM_PIECES_HEIGHT-11, 4),
    std::make_pair(LEVEL_NUM_PIECES_HEIGHT-16, 4),
    std::make_pair(LEVEL_NUM_PIECES_HEIGHT-21, 4)
};

const std::pair<int,int> NouveauBoss::RIGHT_SIDE_PRISM_IDX_PAIRS[NUM_RIGHT_SIDE_PRISMS] = {
    std::make_pair(LEVEL_NUM_PIECES_HEIGHT-6,  LEVEL_NUM_PIECES_WIDTH-5),
    std::make_pair(LEVEL_NUM_PIECES_HEIGHT-11, LEVEL_NUM_PIECES_WIDTH-5),
    std::make_pair(LEVEL_NUM_PIECES_HEIGHT-16, LEVEL_NUM_PIECES_WIDTH-5),
    std::make_pair(LEVEL_NUM_PIECES_HEIGHT-21, LEVEL_NUM_PIECES_WIDTH-5)
};

const float NouveauBoss::Y_POS_FOR_LASER_BEAMS1 = 22.0f;
const float NouveauBoss::Y_POS_FOR_LASER_BEAMS2 = 16.0f;

NouveauBoss::NouveauBoss() {
}

NouveauBoss::~NouveauBoss() {
}

bool NouveauBoss::ProjectilePassesThrough(const Projectile* projectile) const {
    switch (projectile->GetType()) {
        case Projectile::BossLaserBulletProjectile:
        case Projectile::BossRocketBulletProjectile:
        case Projectile::BossOrbBulletProjectile:
        case Projectile::BossLightningBoltBulletProjectile:
            return true;
        default:
            break;
    }

    return false;
}

bool NouveauBoss::ProjectileIsDestroyedOnCollision(const Projectile* projectile, BossBodyPart* collisionPart) const {
    // Handle the special case where the player is shooting a laser at the bosses' glass dome
    if (collisionPart == this->GetTopDome() && projectile->GetType() == Projectile::PaddleLaserBulletProjectile) {
        return false;
    }
    return Boss::ProjectileIsDestroyedOnCollision(projectile, collisionPart);
}

void NouveauBoss::Init(float startingX, float startingY) {

    // Body Layout for the Nouveau Boss:
    // -----------------------------------
    // root
    // |____deadPartsRoot
    // |____alivePartsRoot
    //      |____body (core)
    //      |____topEnclosure
    //           |____gazebo
    //           |____dome
    //      |____topSphere
    //      |____leftSideCurls, rightSideCurls
    //      |____leftSideSphereHolderCurl
    //           |____leftSideHolderCurl
    //           |____leftSideSphere
    //           |____leftSideSphereFrills
    //      |____rightSideSphereHolderCurl
    //           |____rightSideHolderCurl
    //           |____rightSideSphere
    //           |____rightSideSphereFrills
    //      |____bottomCurl1, bottomCurl2, bottomCurl3, bottomCurl4, bottomCurl5, bottomCurl6, bottomCurl7, bottomCurl8
    //      |____bottomHexSphere

    // root
    {
        assert(this->root == NULL);
        this->root = new BossCompositeBodyPart();
        this->bodyParts.push_back(this->root);

        // deadPartsRoot
        {
            assert(this->deadPartsRoot == NULL);
            this->deadPartsRoot  = new BossCompositeBodyPart();
            this->root->AddBodyPart(this->deadPartsRoot);
            this->bodyParts.push_back(this->deadPartsRoot);
        }

        // alivePartsRoot
        {
            assert(this->alivePartsRoot == NULL);
            this->alivePartsRoot = new BossCompositeBodyPart();
            this->root->AddBodyPart(this->alivePartsRoot);
            this->bodyParts.push_back(this->alivePartsRoot);

            // body (core)
            {
                static const float HEIGHT         = BODY_CORE_HEIGHT;
                static const float TOP_WIDTH      = BODY_CORE_TOP_WIDTH;
                static const float HALF_HEIGHT    = HEIGHT / 2.0f; // 
                static const float HALF_TOP_WIDTH = TOP_WIDTH / 2.0f;

                const Point2D leftPt(-3.0f, 0.8f);
                const Point2D rightPt(-leftPt[0], leftPt[1]);

                BoundingLines bodyBounds;
                bodyBounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_TOP_WIDTH, HALF_HEIGHT), 
                    Point2D(HALF_TOP_WIDTH, HALF_HEIGHT)), Vector2D(0, 1), true);  // Top
                bodyBounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_TOP_WIDTH, HALF_HEIGHT), leftPt), 
                    Vector2D(leftPt[1] - HALF_HEIGHT, -leftPt[0] - HALF_TOP_WIDTH), true); // Left diagonal
                bodyBounds.AddBound(Collision::LineSeg2D(Point2D(HALF_TOP_WIDTH, HALF_HEIGHT), rightPt), 
                    Vector2D(HALF_HEIGHT - rightPt[1], rightPt[0] - HALF_TOP_WIDTH), true);  // Right diagonal

                BossBodyPart* body = new BossBodyPart(bodyBounds);
                this->alivePartsRoot->AddBodyPart(body);
                this->bodyIdx = this->bodyParts.size();
                this->bodyParts.push_back(body);
            }

            // topEnclosure
            {
                BossCompositeBodyPart* topEnclosure = new BossCompositeBodyPart();
                this->alivePartsRoot->AddBodyPart(topEnclosure);
                this->bodyParts.push_back(topEnclosure);

                // gazebo
                {
                    static const float HEIGHT = TOP_ENCLOSURE_GAZEBO_HEIGHT;
                    static const float WIDTH  = TOP_ENCLOSURE_GAZEBO_WIDTH;
                    static const float HALF_HEIGHT = HEIGHT / 2.0f;
                    static const float HALF_WIDTH  = WIDTH  / 2.0f;

                    BoundingLines gazeboBounds;
                    gazeboBounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_WIDTH, HALF_HEIGHT), Point2D(-HALF_WIDTH, -HALF_HEIGHT)), Vector2D(-1,0)); // Left
                    gazeboBounds.AddBound(Collision::LineSeg2D(Point2D(HALF_WIDTH, HALF_HEIGHT), Point2D(HALF_WIDTH, -HALF_HEIGHT)), Vector2D(1,0));    // Right

                    BossBodyPart* gazebo = new BossBodyPart(gazeboBounds);
                    topEnclosure->AddBodyPart(gazebo);
                    this->gazeboIdx = this->bodyParts.size();
                    this->bodyParts.push_back(gazebo);
                }
                // dome
                {
                    static const float HEIGHT = TOP_ENCLOSURE_DOME_HEIGHT;
                    static const float LOWER_MID_HEIGHT = 0.692f;
                    static const float UPPER_MID_HEIGHT = 1.246f;

                    static const float HALF_BOTTOM_WIDTH = TOP_ENCLOSURE_GAZEBO_WIDTH / 2.0f;
                    static const float HALF_LOWER_MID_WIDTH = 1.657f;
                    static const float HALF_UPPER_MID_WIDTH = 1.247f;
                    static const float HALF_TOP_WIDTH = TOP_ENCLOSURE_DOME_TOP_WIDTH / 2.0f;

                    BoundingLines domeBounds = BuildSemiCircleBoundingLines(HEIGHT, LOWER_MID_HEIGHT, UPPER_MID_HEIGHT, HALF_BOTTOM_WIDTH,
                        HALF_LOWER_MID_WIDTH, HALF_UPPER_MID_WIDTH, HALF_TOP_WIDTH, false);

                    TargetedRefractiveBossBodyPart* dome = new TargetedRefractiveBossBodyPart(domeBounds, Point2D(0, -2.0f));
                    dome->Translate(Vector3D(0.0f, TOP_ENCLOSURE_GAZEBO_HEIGHT/2.0f, 0.0f));
                    topEnclosure->AddBodyPart(dome);
                    this->domeIdx = this->bodyParts.size();
                    this->bodyParts.push_back(dome);
                }

                topEnclosure->Translate(Vector3D(0.0f, 2.32f + TOP_ENCLOSURE_GAZEBO_HEIGHT/2.0f, 0.0f));

            } // end topEnclosure

            // topSphere
            {
                static const float HEIGHT = 1.321f;
                static const float MID_HEIGHT = 0.948f;

                static const float HALF_BOTTOM_WIDTH = 1.343f;
                static const float HALF_MID_WIDTH = 0.928f;

                static const float START_Y = -0.3f;

                BoundingLines topSphereBounds;
                topSphereBounds.AddBound(Collision::LineSeg2D(
                    Point2D(-HALF_BOTTOM_WIDTH, START_Y), Point2D(-HALF_MID_WIDTH, MID_HEIGHT)), 
                    Vector2D(START_Y - MID_HEIGHT, HALF_BOTTOM_WIDTH - HALF_MID_WIDTH), true); // Left-Bottom
                topSphereBounds.AddBound(Collision::LineSeg2D(
                    Point2D(-HALF_MID_WIDTH, MID_HEIGHT), Point2D(0.0f, HEIGHT)), 
                    Vector2D(MID_HEIGHT - HEIGHT, HALF_MID_WIDTH)); // Left-Top

                topSphereBounds.AddBound(Collision::LineSeg2D(
                    Point2D(HALF_BOTTOM_WIDTH, START_Y), Point2D(HALF_MID_WIDTH, MID_HEIGHT)),
                    Vector2D(MID_HEIGHT - START_Y, HALF_BOTTOM_WIDTH - HALF_MID_WIDTH), true); // Right-Bottom
                topSphereBounds.AddBound(Collision::LineSeg2D(
                    Point2D(HALF_MID_WIDTH, MID_HEIGHT), Point2D(0.0f, HEIGHT)),
                    Vector2D(HEIGHT - MID_HEIGHT, HALF_MID_WIDTH)); // Right-Top

                BossBodyPart* topSphere = new BossBodyPart(topSphereBounds);
                topSphere->Translate(Vector3D(0.0f, 1.597f + 1.070, 0.0f));
                this->alivePartsRoot->AddBodyPart(topSphere);
                this->topSphereIdx = this->bodyParts.size();
                this->bodyParts.push_back(topSphere);
            }

            // Side Curls...
            {
                Point2D PT0(0.0f, 0.0f);
                Point2D PT1(-2.689f, 1.173f);
                Point2D PT2(-5.390f, 0.0f);
                Point2D PT3(-5.773f, -0.902f);
                Point2D PT4(-5.118f, -2.362f);
                Point2D PT5(-4.390f, -2.510f);
                Point2D PT6(-2.5f, -1.9f);

                BoundingLines leftCurlBounds;
                leftCurlBounds.AddBound(Collision::LineSeg2D(PT0, PT1), Vector2D(PT1[1] - PT0[1], PT0[0] - PT1[0]), true);
                leftCurlBounds.AddBound(Collision::LineSeg2D(PT1, PT2), Vector2D(PT2[1] - PT1[1], PT1[0] - PT2[0]), true);
                leftCurlBounds.AddBound(Collision::LineSeg2D(PT2, PT3), Vector2D(PT3[1] - PT2[1], PT2[0] - PT3[0]));
                leftCurlBounds.AddBound(Collision::LineSeg2D(PT3, PT4), Vector2D(PT4[1] - PT3[1], PT3[0] - PT4[0]));
                leftCurlBounds.AddBound(Collision::LineSeg2D(PT4, PT5), Vector2D(PT5[1] - PT4[1], PT4[0] - PT5[0]));
                leftCurlBounds.AddBound(Collision::LineSeg2D(PT5, PT6), Vector2D(PT6[1] - PT5[1], PT5[0] - PT6[0]));

                PT0[0] *= -1; PT1[0] *= -1; PT2[0] *= -1; PT3[0] *= -1; PT4[0] *= -1; PT5[0] *= -1; PT6[0] *= -1;
                BoundingLines rightCurlBounds;
                rightCurlBounds.AddBound(Collision::LineSeg2D(PT0, PT1), Vector2D(-(PT1[1] - PT0[1]), -(PT0[0] - PT1[0])), true);
                rightCurlBounds.AddBound(Collision::LineSeg2D(PT1, PT2), Vector2D(-(PT2[1] - PT1[1]), -(PT1[0] - PT2[0])), true);
                rightCurlBounds.AddBound(Collision::LineSeg2D(PT2, PT3), Vector2D(-(PT3[1] - PT2[1]), -(PT2[0] - PT3[0])));
                rightCurlBounds.AddBound(Collision::LineSeg2D(PT3, PT4), Vector2D(-(PT4[1] - PT3[1]), -(PT3[0] - PT4[0])));
                rightCurlBounds.AddBound(Collision::LineSeg2D(PT4, PT5), Vector2D(-(PT5[1] - PT4[1]), -(PT4[0] - PT5[0])));
                rightCurlBounds.AddBound(Collision::LineSeg2D(PT5, PT6), Vector2D(-(PT6[1] - PT5[1]), -(PT5[0] - PT6[0])));

                // leftSideCurls
                {
                    BossBodyPart* leftSideCurls = new BossBodyPart(leftCurlBounds);
                    leftSideCurls->ToggleSimpleBoundingCalc(true);
                    leftSideCurls->Translate(Vector3D(-2.986f, 0.82f, 0.0f));
                    this->alivePartsRoot->AddBodyPart(leftSideCurls);
                    this->leftSideCurlsIdx = this->bodyParts.size();
                    this->bodyParts.push_back(leftSideCurls);
                }

                // rightSideCurls
                {
                    BossBodyPart* rightSideCurls = new BossBodyPart(rightCurlBounds);
                    rightSideCurls->ToggleSimpleBoundingCalc(true);
                    rightSideCurls->RotateY(180.0f);
                    rightSideCurls->Translate(Vector3D(2.986f, 0.82f, 0.0f));
                    this->alivePartsRoot->AddBodyPart(rightSideCurls);
                    this->rightSideCurlsIdx = this->bodyParts.size();
                    this->bodyParts.push_back(rightSideCurls);
                }
            }

            // Side Sphere Holders...
            {

                BoundingLines leftHolderCurlBounds;
                BoundingLines rightHolderCurlBounds;
                {
                    Point2D PT0(0.35f, 1.714f);
                    Point2D PT1(-5.718f, 1.714f);
                    Point2D PT2(-5.718f, -0.15f);
                    leftHolderCurlBounds.AddBound(Collision::LineSeg2D(PT0, PT1), Vector2D(0, 1), true);
                    leftHolderCurlBounds.AddBound(Collision::LineSeg2D(PT1, PT2), Vector2D(-1, 0), false);

                    PT0[0] *= -1; PT1[0] *= -1; PT2[0] *= -1;
                    rightHolderCurlBounds.AddBound(Collision::LineSeg2D(PT0, PT1), Vector2D(0, 1), true);
                    rightHolderCurlBounds.AddBound(Collision::LineSeg2D(PT1, PT2), Vector2D(1, 0), false);
                }

                BoundingLines sideSphereBounds;
                {
                    static const float HEIGHT           = 1.215f;
                    static const float LOWER_MID_HEIGHT = 0.347f;
                    static const float UPPER_MID_HEIGHT = 0.970f;

                    static const float HALF_BOTTOM_WIDTH    = 0.9f;
                    static const float HALF_LOWER_MID_WIDTH = 0.977f;
                    static const float HALF_UPPER_MID_WIDTH = 0.675f;
                    static const float HALF_TOP_WIDTH       = 0.251f;

                    sideSphereBounds = BuildSemiCircleBoundingLines(HEIGHT, LOWER_MID_HEIGHT, UPPER_MID_HEIGHT, HALF_BOTTOM_WIDTH,
                        HALF_LOWER_MID_WIDTH, HALF_UPPER_MID_WIDTH, HALF_TOP_WIDTH, true);
                }

                BoundingLines leftFrillBounds;
                BoundingLines rightFrillBounds;
                {
                    Point2D PT0(-2.606f, 0.0f);
                    Point2D PT1(-2.606f, 1.407f);
                    Point2D PT2(-0.528f, 2.599f);
                    Point2D PT3(0.528f, 2.599f);
                    Point2D PT4(2.606f, 1.407f);
                    Point2D PT5(3.5f, 1.407f);

                    leftFrillBounds.AddBound(Collision::LineSeg2D(PT0, PT1), Vector2D(-1, 0));
                    leftFrillBounds.AddBound(Collision::LineSeg2D(PT1, PT2), Vector2D(PT1[1] - PT2[1], PT2[0] - PT1[0]));
                    leftFrillBounds.AddBound(Collision::LineSeg2D(PT2, PT3), Vector2D(0, 1));
                    leftFrillBounds.AddBound(Collision::LineSeg2D(PT3, PT4), Vector2D(PT3[1] - PT4[1], PT4[0] - PT3[0]));
                    leftFrillBounds.AddBound(Collision::LineSeg2D(PT4, PT5), Vector2D(0, 1));

                    PT0[0] *= -1; PT1[0] *= -1; PT2[0] *= -1; PT3[0] *= -1; PT4[0] *= -1; PT5[0] *= -1;
                    rightFrillBounds.AddBound(Collision::LineSeg2D(PT0, PT1), Vector2D(1, 0));
                    rightFrillBounds.AddBound(Collision::LineSeg2D(PT1, PT2), Vector2D(-(PT1[1] - PT2[1]), -(PT2[0] - PT1[0])));
                    rightFrillBounds.AddBound(Collision::LineSeg2D(PT2, PT3), Vector2D(0, 1));
                    rightFrillBounds.AddBound(Collision::LineSeg2D(PT3, PT4), Vector2D(-(PT3[1] - PT4[1]), -(PT4[0] - PT3[0])));
                    rightFrillBounds.AddBound(Collision::LineSeg2D(PT4, PT5), Vector2D(0, 1));
                }

                // leftSideSphereHolderCurl
                {
                    BossCompositeBodyPart* leftSideSphereHolder = new BossCompositeBodyPart();
                    this->alivePartsRoot->AddBodyPart(leftSideSphereHolder);
                    this->leftSideSphereHolderIdx = this->bodyParts.size();
                    this->bodyParts.push_back(leftSideSphereHolder);

                    // leftSideHolderCurl
                    {
                        BossBodyPart* leftSideHolderCurl = new BossBodyPart(leftHolderCurlBounds);
                        leftSideSphereHolder->AddBodyPart(leftSideHolderCurl);
                        this->leftSideHolderCurlIdx = this->bodyParts.size();
                        this->bodyParts.push_back(leftSideHolderCurl);
                    }

                    // leftSideSphere
                    {
                        BossBodyPart* leftSideSphere = new BossBodyPart(sideSphereBounds);
                        leftSideSphere->ToggleSimpleBoundingCalc(true);
                        leftSideSphere->Translate(Vector3D(-3.112f , 1.558f, 0.0f));
                        leftSideSphereHolder->AddBodyPart(leftSideSphere);
                        this->leftSideSphereIdx = this->bodyParts.size();
                        this->bodyParts.push_back(leftSideSphere);

                    }
                    // leftSideSphereFrills
                    {
                        BossBodyPart* leftSideSphereFrills = new BossBodyPart(leftFrillBounds);
                        leftSideSphereFrills->ToggleSimpleBoundingCalc(true);
                        leftSideSphereFrills->Translate(Vector3D(-3.112f , 1.558f, 0.0f));
                        leftSideSphereHolder->AddBodyPart(leftSideSphereFrills);
                        this->leftSideSphereFrillsIdx = this->bodyParts.size();
                        this->bodyParts.push_back(leftSideSphereFrills);
                    }

                    leftSideSphereHolder->Translate(Vector3D(-2.386f, 1.07f, 0.0f));
                }

                // rightSideSphereHolderCurl
                {
                    BossCompositeBodyPart* rightSideSphereHolder = new BossCompositeBodyPart();
                    this->alivePartsRoot->AddBodyPart(rightSideSphereHolder);
                    this->rightSideSphereHolderIdx = this->bodyParts.size();
                    this->bodyParts.push_back(rightSideSphereHolder);
                    
                    // rightSideHolderCurl
                    {
                        BossBodyPart* rightSideHolderCurl = new BossBodyPart(rightHolderCurlBounds);
                        rightSideHolderCurl->ToggleSimpleBoundingCalc(true);
                        rightSideHolderCurl->RotateY(180.0f);
                        rightSideSphereHolder->AddBodyPart(rightSideHolderCurl);
                        this->rightSideHolderCurlIdx = this->bodyParts.size();
                        this->bodyParts.push_back(rightSideHolderCurl);
                    }

                    // rightSideSphere
                    {
                        BossBodyPart* rightSideSphere = new BossBodyPart(sideSphereBounds);
                        rightSideSphere->ToggleSimpleBoundingCalc(true);
                        rightSideSphere->Translate(Vector3D(3.112f , 1.558f, 0.0f));
                        rightSideSphereHolder->AddBodyPart(rightSideSphere);
                        this->rightSideSphereIdx = this->bodyParts.size();
                        this->bodyParts.push_back(rightSideSphere);

                    }

                    // rightSideSphereFrills
                    {
                        BossBodyPart* rightSideSphereFrills = new BossBodyPart(rightFrillBounds);
                        rightSideSphereFrills->ToggleSimpleBoundingCalc(true);
                        rightSideSphereFrills->Translate(Vector3D(3.112f , 1.558f, 0.0f));
                        rightSideSphereHolder->AddBodyPart(rightSideSphereFrills);
                        this->rightSideSphereFrillsIdx = this->bodyParts.size();
                        this->bodyParts.push_back(rightSideSphereFrills);
                    }

                    rightSideSphereHolder->Translate(Vector3D(2.386f, 1.07f, 0.0f));
                }

            } // end Side sphere holders

            // Bottom Curls
            {
                static const float Y_TRANSLATION = -1.545f;
                this->BuildBottomCurl(Vector3D(-4.909f, Y_TRANSLATION, 2.034f), 22.5f, this->bottomCurlIdxs[0]);
                this->BuildBottomCurl(Vector3D(-2.034f, Y_TRANSLATION, 4.909f), 67.5f, this->bottomCurlIdxs[1]);
                this->BuildBottomCurl(Vector3D( 2.034f, Y_TRANSLATION, 4.909f), 112.5f, this->bottomCurlIdxs[2]);
                this->BuildBottomCurl(Vector3D( 4.909f, Y_TRANSLATION, 2.034f), 157.5f, this->bottomCurlIdxs[3]);
                this->BuildBottomCurl(Vector3D( 4.909f, Y_TRANSLATION, -2.034f), 202.5f, this->bottomCurlIdxs[4]);
                this->BuildBottomCurl(Vector3D( 2.034f, Y_TRANSLATION, -4.909f), 247.5f, this->bottomCurlIdxs[5]);
                this->BuildBottomCurl(Vector3D(-2.034f, Y_TRANSLATION, -4.909f), 292.5f, this->bottomCurlIdxs[6]);
                this->BuildBottomCurl(Vector3D(-4.909f, Y_TRANSLATION, -2.034f), 337.5f, this->bottomCurlIdxs[7]);
            }
            
            // bottomHexSphere
            {
                const Point2D PT2(-1.94f, -2.250f);
                const Point2D PT3(1.94f, -2.250f);

                BoundingLines hexSphereBounds;
                hexSphereBounds.AddBound(Collision::LineSeg2D(PT2, PT3), Vector2D(0, -1));

                BossBodyPart* bottomHexSphere = new BossBodyPart(hexSphereBounds);
                bottomHexSphere->ToggleSimpleBoundingCalc(true);
                bottomHexSphere->Translate(Vector3D(0.0f, -1.680f, 0.0f));
                this->alivePartsRoot->AddBodyPart(bottomHexSphere);
                this->bottomHexSphereIdx = this->bodyParts.size();
                this->bodyParts.push_back(bottomHexSphere);
            }

        } // end alivePartsRoot
    } // end root

    this->root->Translate(Vector3D(startingX, startingY, 0.0f)); // NOTE: THIS MUST BE DONE BEFORE SETTING THE FIRST AI STATE!

    this->SetCurrentAIStateImmediately(new SideSphereAI(this));
    // N.B., Bosses are transformed into level space by the GameLevel when they are loaded from file.
}

void NouveauBoss::BuildBottomCurl(const Vector3D& translation, float yRotation, size_t& idx) {
    // Curls have no bounds unless they are the ones hanging off the front-sides
    BoundingLines curlBounds;

    Point2D PT0(-0.570f, 0.5f);
    Point2D PT1(-0.570f, -2.275f);
    Point2D PT2(0.505f, -3.549f);
    Point2D PT3(3.034f, -3.549f);
    Point2D PT4(3.034f, -2.35f);

    if (yRotation == 22.5f) {
        curlBounds.AddBound(Collision::LineSeg2D(PT0, PT1), Vector2D(-1, 0));
        curlBounds.AddBound(Collision::LineSeg2D(PT1, PT2), Vector2D(PT2[1] - PT1[1], PT1[0] - PT2[0]));
        curlBounds.AddBound(Collision::LineSeg2D(PT2, PT3), Vector2D(0, -1));
        curlBounds.AddBound(Collision::LineSeg2D(PT3, PT4), Vector2D(1, 0));
    }
    else if (yRotation == 157.5f) {
        PT0[0] *= -1; PT1[0] *= -1; PT2[0] *= -1; PT3[0] *= -1; PT4[0] *= -1; 
        curlBounds.AddBound(Collision::LineSeg2D(PT0, PT1), Vector2D(1, 0));
        curlBounds.AddBound(Collision::LineSeg2D(PT1, PT2), Vector2D(-(PT2[1] - PT1[1]), -(PT1[0] - PT2[0])));
        curlBounds.AddBound(Collision::LineSeg2D(PT2, PT3), Vector2D(0, -1));
        curlBounds.AddBound(Collision::LineSeg2D(PT3, PT4), Vector2D(-1, 0));
    }

    BossBodyPart* bottomCurl = new BossBodyPart(curlBounds);
    bottomCurl->ToggleSimpleBoundingCalc(true);
    bottomCurl->RotateY(yRotation);
    bottomCurl->Translate(translation);

    this->alivePartsRoot->AddBodyPart(bottomCurl);
    idx = this->bodyParts.size();
    this->bodyParts.push_back(bottomCurl);
}

BoundingLines NouveauBoss::BuildSemiCircleBoundingLines(float height, float lowerMidHeight, float upperMidHeight,
                                                        float halfBottomWidth, float halfLowerMidWidth,
                                                        float halfUpperMidWidth, float halfTopWidth, bool doInsideOnSides) {

    BoundingLines domeBounds;

    domeBounds.AddBound(Collision::LineSeg2D(
        Point2D(-halfBottomWidth, 0.0f), 
        Point2D(-halfLowerMidWidth, lowerMidHeight)), 
        Vector2D(0.0f - lowerMidHeight, halfBottomWidth - halfLowerMidWidth), doInsideOnSides); // Left-Bottom
    domeBounds.AddBound(Collision::LineSeg2D(
        Point2D(-halfLowerMidWidth, lowerMidHeight), 
        Point2D(-halfUpperMidWidth, upperMidHeight)), 
        Vector2D(lowerMidHeight - upperMidHeight, halfLowerMidWidth - halfUpperMidWidth), doInsideOnSides); // Left-Middle
    domeBounds.AddBound(Collision::LineSeg2D(
        Point2D(-halfUpperMidWidth, upperMidHeight), 
        Point2D(-halfTopWidth, height)), 
        Vector2D(upperMidHeight - height, halfUpperMidWidth - halfTopWidth)); // Left-Top

    domeBounds.AddBound(Collision::LineSeg2D(Point2D(-halfTopWidth, height), Point2D(halfTopWidth, height)), Vector2D(0,1)); // Top

    domeBounds.AddBound(Collision::LineSeg2D(
        Point2D(halfBottomWidth, 0.0f),
        Point2D(halfLowerMidWidth, lowerMidHeight)),
        Vector2D(lowerMidHeight - 0.0f, halfBottomWidth - halfLowerMidWidth), doInsideOnSides); // Right-Bottom
    domeBounds.AddBound(Collision::LineSeg2D(
        Point2D(halfLowerMidWidth, lowerMidHeight),
        Point2D(halfUpperMidWidth, upperMidHeight)),
        Vector2D(upperMidHeight - lowerMidHeight, halfLowerMidWidth - halfUpperMidWidth), doInsideOnSides); // Right-Middle
    domeBounds.AddBound(Collision::LineSeg2D(
        Point2D(halfUpperMidWidth, upperMidHeight),
        Point2D(halfTopWidth, height)),
        Vector2D(height - upperMidHeight, halfUpperMidWidth - halfTopWidth)); // Right-Top

    return domeBounds;
}

PrismTriangleBlock* NouveauBoss::GetSidePrism(const GameLevel& level, int idx) {
    assert(idx >= 0 && idx < NUM_SIDE_PRISMS);
    assert(LEVEL_NUM_PIECES_HEIGHT == level.GetCurrentLevelLayout().size());
    assert(LEVEL_NUM_PIECES_WIDTH  == level.GetCurrentLevelLayout()[0].size());

    const std::pair<int, int>& prismIdxPair = SIDE_PRISM_IDX_PAIRS[idx];
    LevelPiece* piece = level.GetCurrentLevelLayout()[prismIdxPair.first][prismIdxPair.second];

    assert(piece != NULL);
    assert(piece->GetType() == LevelPiece::PrismTriangle);

    return static_cast<PrismTriangleBlock*>(piece);
}
PrismTriangleBlock* NouveauBoss::GetLeftSidePrism(const GameLevel& level, int idx) {
    assert(idx >= 0 && idx < NUM_LEFT_SIDE_PRISMS);
    assert(LEVEL_NUM_PIECES_HEIGHT == level.GetCurrentLevelLayout().size());
    assert(LEVEL_NUM_PIECES_WIDTH  == level.GetCurrentLevelLayout()[0].size());

    const std::pair<int, int>& prismIdxPair = LEFT_SIDE_PRISM_IDX_PAIRS[idx];
    LevelPiece* piece = level.GetCurrentLevelLayout()[prismIdxPair.first][prismIdxPair.second];

    assert(piece != NULL);
    assert(piece->GetType() == LevelPiece::PrismTriangle);

    return static_cast<PrismTriangleBlock*>(piece);
}
PrismTriangleBlock* NouveauBoss::GetRightSidePrism(const GameLevel& level, int idx) {
    assert(idx >= 0 && idx < NUM_RIGHT_SIDE_PRISMS);
    assert(LEVEL_NUM_PIECES_HEIGHT == level.GetCurrentLevelLayout().size());
    assert(LEVEL_NUM_PIECES_WIDTH  == level.GetCurrentLevelLayout()[0].size());

    const std::pair<int, int>& prismIdxPair = RIGHT_SIDE_PRISM_IDX_PAIRS[idx];
    LevelPiece* piece = level.GetCurrentLevelLayout()[prismIdxPair.first][prismIdxPair.second];

    assert(piece != NULL);
    assert(piece->GetType() == LevelPiece::PrismTriangle);

    return static_cast<PrismTriangleBlock*>(piece);
}

std::vector<PrismTriangleBlock*> NouveauBoss::GetBestSidePrismCandidates(const GameLevel& level,
                                                                         const PlayerPaddle& paddle, 
                                                                         const Point2D& shotOrigin,
                                                                         bool leftSide, bool rightSide) {
    std::vector<PrismTriangleBlock*> result;
    result.reserve(8);
    
    Vector2D laserDir;
    std::list<Collision::Ray2D> collisionRays;
    PrismTriangleBlock* currPrism = NULL;

    Vector2D shootPosToPaddleDir = paddle.GetCenterPosition() - shotOrigin;
    shootPosToPaddleDir.Normalize();
    float rayT;

    // Make a bounding circle around the paddle for aiming rays...
    Collision::Circle2D paddleTargetCircle(paddle.GetCenterPosition(), 3*PlayerPaddle::PADDLE_WIDTH_TOTAL);

    if (leftSide) {
        for (int i = 0; i < NouveauBoss::NUM_LEFT_SIDE_PRISMS; i++) {
            currPrism = NouveauBoss::GetLeftSidePrism(level, i);
            laserDir = currPrism->GetCenter() - shotOrigin;
            laserDir.Normalize();

            Collision::Ray2D ray(shotOrigin, laserDir);
            if (!currPrism->CollisionCheck(ray, rayT)) {
                continue;
            }

            currPrism->GetReflectionRefractionRays(ray.GetPointAlongRayFromOrigin(rayT), laserDir, collisionRays);
            if (collisionRays.empty()) {
                continue;
            }

            // Check to see if the collision ray will hit the paddle target circle...
            if (Collision::IsCollision(*collisionRays.begin(), paddleTargetCircle, rayT)) {
                result.push_back(currPrism);
            } 
        }
    }
    if (rightSide) {
        for (int i = 0; i < NouveauBoss::NUM_RIGHT_SIDE_PRISMS; i++) {
            currPrism = NouveauBoss::GetRightSidePrism(level, i);

            laserDir = currPrism->GetCenter() - shotOrigin;
            laserDir.Normalize();

            Collision::Ray2D ray(shotOrigin, laserDir);
            if (!currPrism->CollisionCheck(ray, rayT)) {
                continue;
            }

            currPrism->GetReflectionRefractionRays(ray.GetPointAlongRayFromOrigin(rayT), laserDir, collisionRays);
            if (collisionRays.empty()) {
                continue;
            }

            // Check to see if the collision ray will hit the paddle target circle...
            if (Collision::IsCollision(*collisionRays.begin(), paddleTargetCircle, rayT)) {
                result.push_back(currPrism);
            } 
        }
    }
    
    return result;
}

PrismBlock* NouveauBoss::GetLeftSplitterPrism(const GameLevel& level) {
    LevelPiece* piece = level.GetCurrentLevelLayout()[9][9];

    assert(piece != NULL);
    assert(piece->GetType() == LevelPiece::Prism);

    return static_cast<PrismBlock*>(piece);
}
PrismBlock* NouveauBoss::GetRightSplitterPrism(const GameLevel& level) {
    LevelPiece* piece = level.GetCurrentLevelLayout()[9][15];

    assert(piece != NULL);
    assert(piece->GetType() == LevelPiece::Prism);

    return static_cast<PrismBlock*>(piece);
}
