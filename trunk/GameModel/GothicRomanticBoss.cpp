/**
 * GothicRomanticBoss.cpp
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

#include "GothicRomanticBoss.h"
#include "GothicRomanticBossAIStates.h"

using namespace gothicromanticbossai;

const float GothicRomanticBoss::TOTAL_WIDTH_WITH_LEGS = 10.25f;
const float GothicRomanticBoss::TOTAL_HEIGHT          = 9.258f;

const float GothicRomanticBoss::BODY_HEIGHT = 7.0f;
const float GothicRomanticBoss::BODY_WIDTH  = 3.45f;
const float GothicRomanticBoss::HALF_BODY_WIDTH  = BODY_WIDTH / 2.0f;
const float GothicRomanticBoss::HALF_BODY_HEIGHT = BODY_HEIGHT / 2.0f;

const float GothicRomanticBoss::TOP_POINT_WIDTH  = 2.644f;
const float GothicRomanticBoss::HALF_TOP_POINT_WIDTH = TOP_POINT_WIDTH / 2.0f;
const float GothicRomanticBoss::TOP_POINT_HEIGHT = 1.492f;
const float GothicRomanticBoss::TOP_POINT_TIP_Y  = 1.208f;

const float GothicRomanticBoss::BOTTOM_POINT_WIDTH  = 3.75f;
const float GothicRomanticBoss::HALF_BOTTOM_POINT_WIDTH = BOTTOM_POINT_WIDTH / 2.0f;
const float GothicRomanticBoss::BOTTOM_POINT_HEIGHT = 1.516f;
const float GothicRomanticBoss::BOTTOM_POINT_TIP_Y  = -1.069f;

const float GothicRomanticBoss::LEG_WIDTH  = 3.25f;
const float GothicRomanticBoss::LEG_HEIGHT = 3.0f;

const float GothicRomanticBoss::DEFAULT_ACCELERATION = 1.5f * PlayerPaddle::DEFAULT_ACCELERATION;

const double GothicRomanticBoss::DELAY_BEFORE_SUMMONING_ITEMS_IN_SECS = 2.0;

const float GothicRomanticBoss::LEVEL_WIDTH  = 21 * LevelPiece::PIECE_WIDTH;
const float GothicRomanticBoss::LEVEL_HEIGHT = 31 * LevelPiece::PIECE_HEIGHT;


GothicRomanticBoss::GothicRomanticBoss() : Boss(), bodyIdx(0), topPointIdx(0), bottomPointIdx(0) {
    for (int i = 0; i < NUM_LEGS; i++) {
        this->legIdxs[i] = 0;
    }
}

GothicRomanticBoss::~GothicRomanticBoss() {
}

bool GothicRomanticBoss::ProjectilePassesThrough(const Projectile* projectile) const {
    switch (projectile->GetType()) {
        case Projectile::BossLaserBulletProjectile:
        case Projectile::BossRocketBulletProjectile:
        case Projectile::BossOrbBulletProjectile:
        case Projectile::BossLightningBoltBulletProjectile:
        case Projectile::BossShockOrbBulletProjectile:
            return true;
        default:
            break;
    }

    return false;
}

Point3D GothicRomanticBoss::GetLegPointPos(int idx) const {
    const BossBodyPart* leg = this->GetLeg(idx);
    assert(leg != NULL);

    return leg->GetWorldTransform() * Point3D(0.0f, -1.875f, 3.0f);
}

Point3D GothicRomanticBoss::GetBottomPointTipPos() const {
    const BossBodyPart* bottomPt = this->GetBottomPoint();
    assert(bottomPt != NULL);

    return bottomPt->GetTranslationPt3D() + Vector3D(0.0f, BOTTOM_POINT_TIP_Y, 0.0f);
}

Point3D GothicRomanticBoss::GetTopPointTipPos() const {
    const BossBodyPart* topPt = this->GetTopPoint();
    assert(topPt != NULL);

    return topPt->GetTranslationPt3D() + Vector3D(0.0f, TOP_POINT_TIP_Y, 0.0f);
}

void GothicRomanticBoss::Init(float startingX, float startingY, 
                              const std::vector<std::vector<LevelPiece*> >& levelPieces) {
    UNUSED_PARAMETER(levelPieces);

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

            // Body
            {
                static const float HEIGHT = BODY_HEIGHT;
                static const float WIDTH  = BODY_WIDTH;
                static const float HALF_HEIGHT = HEIGHT / 2.0f;
                static const float HALF_WIDTH  = WIDTH  / 2.0f;
                static const float Y_NEG = -2.984f;

                BoundingLines bodyBounds;
                bodyBounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_WIDTH, HALF_HEIGHT), Point2D(HALF_WIDTH, HALF_HEIGHT)), Vector2D(0, 1)); // Top
                bodyBounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_WIDTH, Y_NEG), Point2D(HALF_WIDTH, Y_NEG)), Vector2D(0, -1));            // Bottom
                bodyBounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_WIDTH, HALF_HEIGHT), Point2D(-HALF_WIDTH, Y_NEG)), Vector2D(-1, 0));     // Left
                bodyBounds.AddBound(Collision::LineSeg2D(Point2D(HALF_WIDTH, HALF_HEIGHT), Point2D(HALF_WIDTH, Y_NEG)), Vector2D(1, 0));        // Right

                BossBodyPart* body = new BossBodyPart(bodyBounds);
                body->ToggleSimpleBoundingCalc(true);

                this->alivePartsRoot->AddBodyPart(body);
                this->bodyIdx = this->bodyParts.size();
                this->bodyParts.push_back(body);
            }

            // Top Point
            {
                static const float WIDTH  = TOP_POINT_WIDTH;
                static const float HALF_WIDTH = WIDTH / 2.0f;

                static const float TOP_Y    = TOP_POINT_TIP_Y;
                static const float BOTTOM_Y = -0.284f;

                BoundingLines topPtBounds;
                topPtBounds.AddBound(Collision::LineSeg2D(Point2D(0, TOP_Y), Point2D(-HALF_WIDTH, BOTTOM_Y)), Vector2D(BOTTOM_Y - TOP_Y, HALF_WIDTH));
                topPtBounds.AddBound(Collision::LineSeg2D(Point2D(0, TOP_Y), Point2D(HALF_WIDTH,  BOTTOM_Y)), Vector2D(TOP_Y - BOTTOM_Y, HALF_WIDTH));

                BossBodyPart* topPt = new BossBodyPart(topPtBounds);
                topPt->Translate(Vector3D(0, 3.542f, 0));
                topPt->ToggleSimpleBoundingCalc(true);

                this->alivePartsRoot->AddBodyPart(topPt);
                this->topPointIdx = this->bodyParts.size();
                this->bodyParts.push_back(topPt);
            }

            // Bottom Point
            {
                static const float WIDTH  = BOTTOM_POINT_WIDTH;
                static const float HALF_WIDTH = WIDTH / 2.0f;

                static const float TOP_Y    = 0.447f;
                static const float BOTTOM_Y = BOTTOM_POINT_TIP_Y;

                BoundingLines bottomPtBounds;
                bottomPtBounds.AddBound(Collision::LineSeg2D(Point2D(0, BOTTOM_Y), Point2D(-HALF_WIDTH, TOP_Y)), Vector2D(BOTTOM_Y - TOP_Y, -HALF_WIDTH));
                bottomPtBounds.AddBound(Collision::LineSeg2D(Point2D(0, BOTTOM_Y), Point2D(HALF_WIDTH,  TOP_Y)), Vector2D(TOP_Y - BOTTOM_Y, -HALF_WIDTH));

                BossBodyPart* bottomPt = new BossBodyPart(bottomPtBounds);
                bottomPt->Translate(Vector3D(0, -3.431f, 0));
                bottomPt->ToggleSimpleBoundingCalc(true);

                this->alivePartsRoot->AddBodyPart(bottomPt);
                this->bottomPointIdx = this->bodyParts.size();
                this->bodyParts.push_back(bottomPt);
            }

            // Legs
            {
                this->BuildLeg(Vector3D(0.0f, -1.859f, 1.876f), 0.0f, this->legIdxs[0]);
                this->BuildLeg(Vector3D(1.326f, -1.859f, 1.327f), 45.0f, this->legIdxs[1]);
                this->BuildLeg(Vector3D(1.875f, -1.859f, 0.0f), 90.0f, this->legIdxs[2]);
                this->BuildLeg(Vector3D(1.326f, -1.859f, -1.325f), 135.0f, this->legIdxs[3]);
                this->BuildLeg(Vector3D(0.0f, -1.859f, -1.876f), 180.0f, this->legIdxs[4]);
                this->BuildLeg(Vector3D(-1.326f, -1.859f, -1.325f), 225.0f, this->legIdxs[5]);
                this->BuildLeg(Vector3D(-1.875f, -1.859f, 0.0f), 270.0f, this->legIdxs[6]);
                this->BuildLeg(Vector3D(-1.326f, -1.859f, 1.327f), 315.0f, this->legIdxs[7]);
            }

        } // end alivePartsRoot
    } // end root

    // Move the boss up a bit to start off
    this->root->Translate(Vector3D(startingX, startingY + 2*LevelPiece::PIECE_HEIGHT, 0.0f));

    this->SetCurrentAIStateImmediately(new FireBallAI(this));
    // N.B., Bosses are transformed into level space by the GameLevel when they are loaded from file.
}

void GothicRomanticBoss::BuildLeg(const Vector3D& legTranslation, float legYRotation, size_t& legIdx) {
    // Legs have no bounds unless they are the ones hanging off the sides on the x/y plane
    BoundingLines legBounds;

    BossBodyPart* leg = new BossBodyPart(legBounds);
    leg->RotateY(legYRotation);
    leg->Translate(legTranslation);

    this->alivePartsRoot->AddBodyPart(leg);
    legIdx = this->bodyParts.size();
    this->bodyParts.push_back(leg);
}