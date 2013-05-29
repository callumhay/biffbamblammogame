/**
 * NouveauBoss.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "NouveauBoss.h"
#include "NouveauBossAIStates.h"

using namespace nouveaubossai;

const float NouveauBoss::BODY_CORE_HEIGHT       = 4.719f;
const float NouveauBoss::BODY_CORE_TOP_WIDTH    = 4.236f;
const float NouveauBoss::BODY_CORE_BOTTOM_WIDTH = 9.705f;

const float NouveauBoss::TOP_ENCLOSURE_GAZEBO_HEIGHT  = 2.480f;
const float NouveauBoss::TOP_ENCLOSURE_GAZEBO_WIDTH   = 3.972f;
const float NouveauBoss::TOP_ENCLOSURE_DOME_HEIGHT    = 1.555f;
const float NouveauBoss::TOP_ENCLOSURE_DOME_TOP_WIDTH = 1.312f;

const float NouveauBoss::ARM_SPHERE_HOLDER_CURL_WIDTH = 5.485f;

const float NouveauBoss::DEFAULT_ACCELERATION = PlayerPaddle::DEFAULT_ACCELERATION;

const float NouveauBoss::MIN_X_BOUNDS_WITH_PADDING = LevelPiece::PIECE_WIDTH * 5  + LevelPiece::PIECE_WIDTH;
const float NouveauBoss::MAX_X_BOUNDS_WITH_PADDING = LevelPiece::PIECE_WIDTH * 18 - LevelPiece::PIECE_WIDTH;
const float NouveauBoss::MIN_Y_BOUNDS_WITH_PADDING = LevelPiece::PIECE_HEIGHT * 12;
const float NouveauBoss::MAX_Y_BOUNDS_WITH_PADDING = LevelPiece::PIECE_HEIGHT * 25;

NouveauBoss::NouveauBoss() {
}

NouveauBoss::~NouveauBoss() {
}

bool NouveauBoss::ProjectilePassesThrough(const Projectile* projectile) const {
    switch (projectile->GetType()) {
        case Projectile::BossLaserBulletProjectile:
        case Projectile::BossRocketBulletProjectile:
        case Projectile::BossOrbBulletProjectile:
            return true;
        default:
            break;
    }

    return false;
}

void NouveauBoss::Init() {

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
                static const float HEIGHT        = BODY_CORE_HEIGHT;
                static const float BOTTOM_WIDTH  = BODY_CORE_BOTTOM_WIDTH;
                static const float TOP_WIDTH     = BODY_CORE_TOP_WIDTH;
                static const float HALF_HEIGHT       = HEIGHT / 2.0f;
                static const float HALF_BOTTOM_WIDTH = BOTTOM_WIDTH  / 2.0f;
                static const float HALF_TOP_WIDTH    = TOP_WIDTH / 2.0f;

                BoundingLines bodyBounds;
                bodyBounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_TOP_WIDTH, HALF_HEIGHT), 
                    Point2D(HALF_TOP_WIDTH, HALF_HEIGHT)), Vector2D(0, 1));  // Top
                bodyBounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_TOP_WIDTH, HALF_HEIGHT), 
                    Point2D(-HALF_BOTTOM_WIDTH, -HALF_HEIGHT)), Vector2D(-HEIGHT, HALF_BOTTOM_WIDTH - HALF_TOP_WIDTH)); // Left diagonal
                bodyBounds.AddBound(Collision::LineSeg2D(Point2D(HALF_TOP_WIDTH, HALF_HEIGHT), 
                    Point2D(HALF_BOTTOM_WIDTH, -HALF_HEIGHT)), Vector2D(HEIGHT, HALF_BOTTOM_WIDTH - HALF_TOP_WIDTH));   // Right diagonal

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
                    gazeboBounds.AddBound(Collision::LineSeg2D(Point2D(HALF_WIDTH, HALF_HEIGHT), Point2D(HALF_WIDTH, HALF_HEIGHT)), Vector2D(1,0));     // Right

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
                        HALF_LOWER_MID_WIDTH, HALF_UPPER_MID_WIDTH, HALF_TOP_WIDTH);

                    BossBodyPart* dome = new BossBodyPart(domeBounds);
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

                BoundingLines topSphereBounds;
                topSphereBounds.AddBound(Collision::LineSeg2D(
                    Point2D(-HALF_BOTTOM_WIDTH, 0.0f), Point2D(-HALF_MID_WIDTH, MID_HEIGHT)), 
                    Vector2D(0.0f - MID_HEIGHT, HALF_BOTTOM_WIDTH - HALF_MID_WIDTH)); // Left-Bottom
                topSphereBounds.AddBound(Collision::LineSeg2D(
                    Point2D(-HALF_MID_WIDTH, MID_HEIGHT), Point2D(0.0f, HEIGHT)), 
                    Vector2D(MID_HEIGHT - HEIGHT, HALF_MID_WIDTH)); // Left-Top
                topSphereBounds.AddBound(Collision::LineSeg2D(
                    Point2D(HALF_BOTTOM_WIDTH, 0.0f), Point2D(HALF_MID_WIDTH, MID_HEIGHT)),
                    Vector2D(MID_HEIGHT, HALF_BOTTOM_WIDTH - HALF_MID_WIDTH)); // Right-Middle
                topSphereBounds.AddBound(Collision::LineSeg2D(
                    Point2D(HALF_MID_WIDTH, MID_HEIGHT), Point2D(0.0f, HEIGHT)),
                    Vector2D(MID_HEIGHT - HEIGHT, HALF_MID_WIDTH)); // Right-Top

                BossBodyPart* topSphere = new BossBodyPart(topSphereBounds);
                topSphere->Translate(Vector3D(0.0f, 1.597f + 1.070, 0.0f));
                this->alivePartsRoot->AddBodyPart(topSphere);
                this->topSphereIdx = this->bodyParts.size();
                this->bodyParts.push_back(topSphere);
            }

            // Side Curls...
            {
                const Point2D PT0(0.0f, 0.0f);
                const Point2D PT1(-2.689f, 1.173f);
                const Point2D PT2(-5.390f, 0.0f);
                const Point2D PT3(-5.773f, -0.902f);
                const Point2D PT4(-5.118f, -2.362f);
                const Point2D PT5(-4.390f, -2.510f);
                const Point2D PT6(-0.750f, -1.361f);

                BoundingLines curlBounds;
                curlBounds.AddBound(Collision::LineSeg2D(PT0, PT1), Vector2D(PT1[1] - PT0[1], PT0[0] - PT1[0]));
                curlBounds.AddBound(Collision::LineSeg2D(PT1, PT2), Vector2D(PT2[1] - PT1[1], PT1[0] - PT2[0]));
                curlBounds.AddBound(Collision::LineSeg2D(PT2, PT3), Vector2D(PT3[1] - PT2[1], PT2[0] - PT3[0]));
                curlBounds.AddBound(Collision::LineSeg2D(PT3, PT4), Vector2D(PT4[1] - PT3[1], PT3[0] - PT4[0]));
                curlBounds.AddBound(Collision::LineSeg2D(PT4, PT5), Vector2D(PT5[1] - PT4[1], PT4[0] - PT5[0]));
                curlBounds.AddBound(Collision::LineSeg2D(PT5, PT6), Vector2D(PT6[1] - PT5[1], PT5[0] - PT6[0]));

                // leftSideCurls
                {
                    BossBodyPart* leftSideCurls = new BossBodyPart(curlBounds);
                    leftSideCurls->Translate(Vector3D(-2.986f, 0.82f, 0.0f));
                    this->alivePartsRoot->AddBodyPart(leftSideCurls);
                    this->leftSideCurlsIdx = this->bodyParts.size();
                    this->bodyParts.push_back(leftSideCurls);
                }

                // rightSideCurls
                {
                    BossBodyPart* rightSideCurls = new BossBodyPart(curlBounds);
                    rightSideCurls->RotateY(180.0f);
                    rightSideCurls->Translate(Vector3D(2.986f, 0.82f, 0.0f));
                    this->alivePartsRoot->AddBodyPart(rightSideCurls);
                    this->rightSideCurlsIdx = this->bodyParts.size();
                    this->bodyParts.push_back(rightSideCurls);
                }
            }

            // Side Sphere Holders...
            {

                BoundingLines holderCurlBounds;
                {
                    const Point2D PT0(0.15f, 1.961f);
                    const Point2D PT1(-5.485f, 1.961f);
                    const Point2D PT2(-5.485f, 0.154f);
                    holderCurlBounds.AddBound(Collision::LineSeg2D(PT0, PT1), Vector2D(0, 1));
                    holderCurlBounds.AddBound(Collision::LineSeg2D(PT1, PT2), Vector2D(-1, 0));
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
                        HALF_LOWER_MID_WIDTH, HALF_UPPER_MID_WIDTH, HALF_TOP_WIDTH);
                }

                BoundingLines frillBounds;
                {
                    const Point2D PT0(-2.606f, 0.0f);
                    const Point2D PT1(-2.606f, 1.407f);
                    const Point2D PT2(-0.984f, 2.599f);
                    const Point2D PT3(0.984f, 2.599f);
                    const Point2D PT4(2.606f, 1.407f);
                    const Point2D PT5(2.606f, 0.0f);

                    frillBounds.AddBound(Collision::LineSeg2D(PT0, PT1), Vector2D(-1, 0));
                    frillBounds.AddBound(Collision::LineSeg2D(PT1, PT2), Vector2D(PT1[1] - PT2[1], PT2[0] - PT1[0]));
                    frillBounds.AddBound(Collision::LineSeg2D(PT2, PT3), Vector2D(0, 1));
                    frillBounds.AddBound(Collision::LineSeg2D(PT3, PT4), Vector2D(PT3[1] - PT4[1], PT4[0] - PT3[0]));
                    frillBounds.AddBound(Collision::LineSeg2D(PT4, PT5), Vector2D(1, 0));
                }

                // leftSideSphereHolderCurl
                {
                    BossCompositeBodyPart* leftSideSphereHolder = new BossCompositeBodyPart();
                    this->alivePartsRoot->AddBodyPart(leftSideSphereHolder);
                    this->leftSideSphereHolderIdx = this->bodyParts.size();
                    this->bodyParts.push_back(leftSideSphereHolder);

                    // leftSideHolderCurl
                    {
                        BossBodyPart* leftSideHolderCurl = new BossBodyPart(holderCurlBounds);
                        leftSideSphereHolder->AddBodyPart(leftSideHolderCurl);
                        this->leftSideHolderCurlIdx = this->bodyParts.size();
                        this->bodyParts.push_back(leftSideHolderCurl);
                    }

                    // leftSideSphere
                    {
                        BossBodyPart* leftSideSphere = new BossBodyPart(sideSphereBounds);
                        leftSideSphere->Translate(Vector3D(-3.112f , 1.558f, 0.0f));
                        leftSideSphereHolder->AddBodyPart(leftSideSphere);
                        this->leftSideSphereIdx = this->bodyParts.size();
                        this->bodyParts.push_back(leftSideSphere);

                    }
                    // leftSideSphereFrills
                    {
                        BossBodyPart* leftSideSphereFrills = new BossBodyPart(frillBounds);
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
                        BossBodyPart* rightSideHolderCurl = new BossBodyPart(holderCurlBounds);
                        rightSideHolderCurl->RotateY(180.0f);
                        rightSideSphereHolder->AddBodyPart(rightSideHolderCurl);
                        this->rightSideHolderCurlIdx = this->bodyParts.size();
                        this->bodyParts.push_back(rightSideHolderCurl);
                    }

                    // rightSideSphere
                    {
                        BossBodyPart* rightSideSphere = new BossBodyPart(sideSphereBounds);
                        rightSideSphere->Translate(Vector3D(3.112f , 1.558f, 0.0f));
                        rightSideSphereHolder->AddBodyPart(rightSideSphere);
                        this->rightSideSphereIdx = this->bodyParts.size();
                        this->bodyParts.push_back(rightSideSphere);

                    }

                    // rightSideSphereFrills
                    {
                        BossBodyPart* rightSideSphereFrills = new BossBodyPart(frillBounds);
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
                const Point2D PT0(-2.456f, 0.0f);
                const Point2D PT1(-1.616f, -1.250f);
                const Point2D PT2(-0.512f, -2.250f);
                const Point2D PT3(0.512f, -2.250f);
                const Point2D PT4(1.616f, -1.250f);
                const Point2D PT5(2.456f, 0.0f);

                BoundingLines hexSphereBounds;
                hexSphereBounds.AddBound(Collision::LineSeg2D(PT0, PT1), Vector2D(PT1[1] - PT0[1], PT0[0] - PT1[0]));
                hexSphereBounds.AddBound(Collision::LineSeg2D(PT1, PT2), Vector2D(PT2[1] - PT1[1], PT1[0] - PT2[0]));
                hexSphereBounds.AddBound(Collision::LineSeg2D(PT2, PT3), Vector2D(0, -1));
                hexSphereBounds.AddBound(Collision::LineSeg2D(PT3, PT4), Vector2D(PT4[1] - PT3[1], PT3[0] - PT4[0]));
                hexSphereBounds.AddBound(Collision::LineSeg2D(PT4, PT5), Vector2D(PT5[1] - PT4[1], PT4[0] - PT5[0]));

                BossBodyPart* bottomHexSphere = new BossBodyPart(hexSphereBounds);
                bottomHexSphere->Translate(Vector3D(0.0f, -1.680f, 0.0f));
                this->alivePartsRoot->AddBodyPart(bottomHexSphere);
                this->bottomHexSphereIdx = this->bodyParts.size();
                this->bodyParts.push_back(bottomHexSphere);
            }

        } // end alivePartsRoot
    } // end root

    // Move the boss up a bit to start off
    //this->Translate(Vector3D(0.0f, 2*LevelPiece::PIECE_HEIGHT, 0.0f));

    this->SetNextAIState(new SideSphereAI(this));
    // N.B., Bosses are transformed into level space by the GameLevel when they are loaded from file.
}

void NouveauBoss::BuildBottomCurl(const Vector3D& translation, float yRotation, size_t& idx) {
    // Curls have no bounds unless they are the ones hanging off the front-sides
    BoundingLines curlBounds;

    static const Point2D PT0(-0.570f, 0.0f);
    static const Point2D PT1(-0.570f, -2.275f);
    static const Point2D PT2(0.505f, -3.549f);
    static const Point2D PT3(3.034f, -3.549f);
    static const Point2D PT4(3.034f, 0.0f);

    if (yRotation == 22.5f || yRotation == 157.5f) {
        curlBounds.AddBound(Collision::LineSeg2D(PT0, PT1), Vector2D(-1, 0));
        curlBounds.AddBound(Collision::LineSeg2D(PT1, PT2), Vector2D(PT2[1] - PT1[1], PT1[0] - PT2[0]));
        curlBounds.AddBound(Collision::LineSeg2D(PT2, PT3), Vector2D(0, -1));
        curlBounds.AddBound(Collision::LineSeg2D(PT3, PT4), Vector2D(1, 0));
    }

    BossBodyPart* bottomCurl = new BossBodyPart(curlBounds);
    bottomCurl->RotateY(yRotation);
    bottomCurl->Translate(translation);

    this->alivePartsRoot->AddBodyPart(bottomCurl);
    idx = this->bodyParts.size();
    this->bodyParts.push_back(bottomCurl);
}

BoundingLines NouveauBoss::BuildSemiCircleBoundingLines(float height, float lowerMidHeight, float upperMidHeight,
                                                        float halfBottomWidth, float halfLowerMidWidth,
                                                        float halfUpperMidWidth, float halfTopWidth) {

    BoundingLines domeBounds;

    domeBounds.AddBound(Collision::LineSeg2D(
        Point2D(-halfBottomWidth, 0.0f), 
        Point2D(-halfLowerMidWidth, lowerMidHeight)), 
        Vector2D(0.0f - lowerMidHeight, halfBottomWidth - halfLowerMidWidth)); // Left-Bottom
    domeBounds.AddBound(Collision::LineSeg2D(
        Point2D(-halfLowerMidWidth, lowerMidHeight), 
        Point2D(-halfUpperMidWidth, upperMidHeight)), 
        Vector2D(lowerMidHeight - upperMidHeight, halfLowerMidWidth - halfUpperMidWidth)); // Left-Middle
    domeBounds.AddBound(Collision::LineSeg2D(
        Point2D(-halfUpperMidWidth, upperMidHeight), 
        Point2D(-halfTopWidth, height)), 
        Vector2D(upperMidHeight - halfTopWidth, halfUpperMidWidth - halfTopWidth)); // Left-Top

    domeBounds.AddBound(Collision::LineSeg2D(Point2D(-halfTopWidth, height), Point2D(halfTopWidth, height)), Vector2D(0,1)); // Top

    domeBounds.AddBound(Collision::LineSeg2D(
        Point2D(halfBottomWidth, 0.0f),
        Point2D(halfLowerMidWidth, lowerMidHeight)),
        Vector2D(lowerMidHeight - 0.0f, halfBottomWidth - halfLowerMidWidth)); // Right-Bottom
    domeBounds.AddBound(Collision::LineSeg2D(
        Point2D(halfLowerMidWidth, lowerMidHeight),
        Point2D(halfUpperMidWidth, upperMidHeight)),
        Vector2D(upperMidHeight - lowerMidHeight, halfLowerMidWidth - halfUpperMidWidth)); // Right-Middle
    domeBounds.AddBound(Collision::LineSeg2D(
        Point2D(halfUpperMidWidth, upperMidHeight),
        Point2D(halfTopWidth, height)),
        Vector2D(upperMidHeight - height, halfUpperMidWidth - halfTopWidth)); // Right-Top

    return domeBounds;
}