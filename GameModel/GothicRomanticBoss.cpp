/**
 * GothicRomanticBoss.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GothicRomanticBoss.h"
#include "GothicRomanticBossAIStates.h"

using namespace gothicromanticbossai;

const float GothicRomanticBoss::BODY_HEIGHT = 7.0f;
const float GothicRomanticBoss::BODY_WIDTH  = 3.45f;

const float GothicRomanticBoss::TOP_POINT_WIDTH  = 2.644f;
const float GothicRomanticBoss::TOP_POINT_HEIGHT = 0.742f;

const float GothicRomanticBoss::BOTTOM_POINT_WIDTH  = 3.75f;
const float GothicRomanticBoss::BOTTOM_POINT_HEIGHT = 1.016f;

const float GothicRomanticBoss::DEFAULT_ACCELERATION = 1.25f * PlayerPaddle::DEFAULT_ACCELERATION;

GothicRomanticBoss::GothicRomanticBoss() : Boss(), bodyIdx(0), topPointIdx(0), bottomPointIdx(0) {
    for (int i = 0; i < NUM_LEGS; i++) {
        this->legIdxs[i] = 0;
    }
}

GothicRomanticBoss::~GothicRomanticBoss() {
}

void GothicRomanticBoss::Init() {

    // Body Layout for the Gothic-Romantic Boss:
    // -----------------------------------
    // root
    // |____deadPartsRoot
    // |____alivePartsRoot
    //      |____body
    //      |____topPoint
    //      |____bottomPoint
    //      |____leg1, leg2, leg3, leg4, leg5, leg6, leg7, leg8


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

            // Body
            {
                static const float HEIGHT = BODY_HEIGHT;
                static const float WIDTH  = BODY_WIDTH;
                static const float HALF_HEIGHT = HEIGHT / 2.0f;
                static const float HALF_WIDTH  = WIDTH  / 2.0f;

                BoundingLines bodyBounds;
                bodyBounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_WIDTH, HALF_HEIGHT), Point2D(HALF_WIDTH, HALF_HEIGHT)), Vector2D(0, 1));    // Top
                bodyBounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_WIDTH, -HALF_HEIGHT), Point2D(HALF_WIDTH, -HALF_HEIGHT)), Vector2D(0, -1)); // Bottom
                bodyBounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_WIDTH, HALF_HEIGHT), Point2D(-HALF_WIDTH, -HALF_HEIGHT)), Vector2D(-1, 0)); // Left
                bodyBounds.AddBound(Collision::LineSeg2D(Point2D(HALF_WIDTH, HALF_HEIGHT), Point2D(HALF_WIDTH, -HALF_HEIGHT)), Vector2D(1, 0));    // Right

                BossBodyPart* body = new BossBodyPart(bodyBounds);

                this->alivePartsRoot->AddBodyPart(body);
                this->bodyIdx = this->bodyParts.size();
                this->bodyParts.push_back(body);
            }

            // Top Point
            {
                static const float WIDTH  = TOP_POINT_WIDTH;
                static const float HALF_WIDTH = WIDTH / 2.0f;

                static const float TOP_Y    = 0.458f;
                static const float BOTTOM_Y = -0.284f;

                BoundingLines topPtBounds;
                topPtBounds.AddBound(Collision::LineSeg2D(Point2D(0, TOP_Y), Point2D(-HALF_WIDTH, BOTTOM_Y)), Vector2D(TOP_Y - BOTTOM_Y, HALF_WIDTH));
                topPtBounds.AddBound(Collision::LineSeg2D(Point2D(0, TOP_Y), Point2D(HALF_WIDTH,  BOTTOM_Y)), Vector2D(BOTTOM_Y - TOP_Y, HALF_WIDTH));

                BossBodyPart* topPt = new BossBodyPart(topPtBounds);
                topPt->Translate(Vector3D(0, 3.542f, 0));

                this->alivePartsRoot->AddBodyPart(topPt);
                this->topPointIdx = this->bodyParts.size();
                this->bodyParts.push_back(topPt);
            }

            // Bottom Point
            {
                static const float WIDTH  = BOTTOM_POINT_WIDTH;
                static const float HALF_WIDTH = WIDTH / 2.0f;

                static const float TOP_Y    = 0.447f;
                static const float BOTTOM_Y = -0.569f;

                BoundingLines bottomPtBounds;
                bottomPtBounds.AddBound(Collision::LineSeg2D(Point2D(0, BOTTOM_Y), Point2D(-HALF_WIDTH, TOP_Y)), Vector2D(BOTTOM_Y - TOP_Y, -HALF_WIDTH));
                bottomPtBounds.AddBound(Collision::LineSeg2D(Point2D(0, BOTTOM_Y), Point2D(HALF_WIDTH,  TOP_Y)), Vector2D(TOP_Y - BOTTOM_Y, -HALF_WIDTH));

                BossBodyPart* bottomPt = new BossBodyPart(bottomPtBounds);
                bottomPt->Translate(Vector3D(0, -3.431f, 0));

                this->alivePartsRoot->AddBodyPart(bottomPt);
                this->bottomPointIdx = this->bodyParts.size();
                this->bodyParts.push_back(bottomPt);
            }

            // Legs
            {
                this->BuildLeg(Vector3D(0.0f, -1.859f, -1.876f), 0.0f, this->legIdxs[0]);
                this->BuildLeg(Vector3D(1.326f, -1.859f, -1.327f), 45.0f, this->legIdxs[1]);
                this->BuildLeg(Vector3D(1.875f, -1.859f, 0.0f), 90.0f, this->legIdxs[2]);
                this->BuildLeg(Vector3D(1.326f, -1.859f, 1.325f), 135.0f, this->legIdxs[3]);
                this->BuildLeg(Vector3D(0.0f, -1.859f, 1.876f), 180.0f, this->legIdxs[4]);
                this->BuildLeg(Vector3D(-1.326f, -1.859f, 1.325f), 225.0f, this->legIdxs[5]);
                this->BuildLeg(Vector3D(-1.875f, -1.859f, 0.0f), 270.0f, this->legIdxs[6]);
                this->BuildLeg(Vector3D(-1.326f, -1.859f, -1.327f), 315.0f, this->legIdxs[7]);
            }

        } // end alivePartsRoot
    } // end root

    this->SetNextAIState(new FireBallAI(this));
    // N.B., Bosses are transformed into level space by the GameLevel when they are loaded from file.
}

void GothicRomanticBoss::BuildLeg(const Vector3D& legTranslation, float legYRotation, size_t& legIdx) {
    // Legs have no bounds
    BoundingLines legBounds;

    BossBodyPart* leg = new BossBodyPart(legBounds);
    leg->RotateY(legYRotation);
    leg->Translate(legTranslation);

    this->alivePartsRoot->AddBodyPart(leg);
    legIdx = this->bodyParts.size();
    this->bodyParts.push_back(leg);
}