/**
 * ClassicalBoss.cpp
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

#include "ClassicalBoss.h"
#include "BossCompositeBodyPart.h"
#include "BossBodyPart.h"
#include "BossWeakpoint.h"

#include "ClassicalBossAIStates.h"

using namespace classicalbossai;

const float ClassicalBoss::ARM_X_TRANSLATION_FROM_CENTER = 10.3f;
const float ClassicalBoss::ARM_X_TRANSLATION_TO_INNER_EDGE_FROM_CENTER = 8.797f;
const float ClassicalBoss::ARM_WIDTH = 2.4f;
const float ClassicalBoss::HALF_ARM_WIDTH = ARM_WIDTH / 2.0f;
const float ClassicalBoss::ARM_HEIGHT = 9.66f;
const float ClassicalBoss::HALF_ARM_HEIGHT = ARM_HEIGHT / 2.0f;

const float ClassicalBoss::COLUMN_WIDTH  = 1.704f;
const float ClassicalBoss::COLUMN_HEIGHT = 4.373f;

const float ClassicalBoss::BASE_WIDTH  = 18.355f;
const float ClassicalBoss::BASE_HEIGHT = 2.150f;

const float ClassicalBoss::TABLATURE_WIDTH  = 7.621f;
const float ClassicalBoss::TABLATURE_HEIGHT = 1.334f;

const float ClassicalBoss::PEDIMENT_WIDTH  = 17.823f;
const float ClassicalBoss::PEDIMENT_HEIGHT = 3.16f;

const float ClassicalBoss::EYE_WIDTH  = 3.150f;
const float ClassicalBoss::EYE_HEIGHT = 1.575f;
const float ClassicalBoss::EYE_DEPTH  = 0.788f;

const float ClassicalBoss::ARMS_BODY_HEAD_MAX_SPEED     = 0.8f * PlayerPaddle::DEFAULT_MAX_SPEED / 1.75f;
const float ClassicalBoss::ARMS_BODY_HEAD_ACCELERATION  = 0.8f * PlayerPaddle::DEFAULT_ACCELERATION / 2.25f;

const float ClassicalBoss::BODY_HEAD_MAX_SPEED          = 0.8f * PlayerPaddle::DEFAULT_MAX_SPEED + 5.0f;
const float ClassicalBoss::BODY_HEAD_ACCELERATION       = 0.8f * PlayerPaddle::DEFAULT_ACCELERATION;

const float ClassicalBoss::PEDIMENT_MAX_SPEED    = 0.8f * PlayerPaddle::DEFAULT_MAX_SPEED;
const float ClassicalBoss::PEDIMENT_ACCELERATION = 0.8f * PlayerPaddle::DEFAULT_ACCELERATION;

const float ClassicalBoss::EYE_MAX_SPEED    = 0.8f * PlayerPaddle::DEFAULT_MAX_SPEED + 8.0f;
const float ClassicalBoss::EYE_ACCELERATION = 0.8f * PlayerPaddle::DEFAULT_ACCELERATION + 12.0f;


ClassicalBoss::ClassicalBoss() : Boss(),
leftArmIdx(0), leftArmSquareIdx(0), rightArmIdx(0), rightArmSquareIdx(0)
{
}

ClassicalBoss::~ClassicalBoss() {
}

void ClassicalBoss::Init(float startingX, float startingY) {
    
    
    // Body Layout for the Classical Boss:
    // -----------------------------------
    // root
    // |____deadPartsRoot
    // |____alivePartsRoot
    //      |____eye
    //      |____pediment
    //      |____topleftTablature, topRightTablature, bottomLeftTablature, bottomRightTablature
    //      |____leftBodyColumn1, leftBodyColumn2, leftBodyColumn3, rightBodyColumn1, rightBodyColumn2, rightBodyColumn3
    //      |____base
    //      |____leftArm
    //           |____square, restOfArm
    //      |____rightArm
    //           |____square, restOfArm

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

            // eye
            {
                // Eye has no bounds to begin with (since it's inside the pediment and we don't want it to
                // mess with ball collisions)
                BoundingLines eyeBounds;
                BossBodyPart* eye = new BossBodyPart(eyeBounds);

                // Translation from the local-space center of the entire boss body
                Vector3D eyeTranslation(0.0f, 5.081f, 1.053f);
                eye->Translate(eyeTranslation);

                this->alivePartsRoot->AddBodyPart(eye);
                this->eyeIdx = this->bodyParts.size();
                this->bodyParts.push_back(eye);
            }

            // pediment
            {
                static const float HEIGHT = PEDIMENT_HEIGHT;
                static const float WIDTH  = PEDIMENT_WIDTH;
                static const float HALF_HEIGHT = HEIGHT / 2.0f;
                static const float HALF_WIDTH  = WIDTH  / 2.0f;

                BoundingLines pedimentBounds;
                pedimentBounds.AddBound(Collision::LineSeg2D(Point2D(0.0f, HALF_HEIGHT), Point2D(-HALF_WIDTH, -HALF_HEIGHT)), Vector2D(-HEIGHT, HALF_WIDTH)); // top-left diagonal
                pedimentBounds.AddBound(Collision::LineSeg2D(Point2D(0.0f, HALF_HEIGHT), Point2D(HALF_WIDTH, -HALF_HEIGHT)), Vector2D(HEIGHT, HALF_WIDTH));   // top-right diagonal

                BossBodyPart* pediment = new BossBodyPart(pedimentBounds);

                // Translation from the local-space center of the entire boss body
                Vector3D pedimentTranslation(0.0f, 5.0905f, 0.0f);
                pediment->Translate(pedimentTranslation);

                this->alivePartsRoot->AddBodyPart(pediment);
                this->pedimentIdx = this->bodyParts.size();
                this->bodyParts.push_back(pediment);
            }

            // *Tablature
            {
                static const float HEIGHT = TABLATURE_HEIGHT;
                static const float BOTTOM_WIDTH  = TABLATURE_WIDTH;
                static const float TOP_WIDTH = 6.933f;
                static const float HALF_HEIGHT = HEIGHT / 2.0f;
                static const float HALF_BOTTOM_WIDTH  = BOTTOM_WIDTH  / 2.0f;
                static const float HALF_TOP_WIDTH = TOP_WIDTH / 2.0f;
                
                BoundingLines tablatureBounds;
                tablatureBounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_TOP_WIDTH, HALF_HEIGHT), Point2D(HALF_TOP_WIDTH, HALF_HEIGHT)), Vector2D(0, 1)); // Top
                tablatureBounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_TOP_WIDTH, HALF_HEIGHT), Point2D(-HALF_BOTTOM_WIDTH, -HALF_HEIGHT)), Vector2D(-HEIGHT, HALF_BOTTOM_WIDTH - HALF_TOP_WIDTH)); // Left diagonal
                tablatureBounds.AddBound(Collision::LineSeg2D(Point2D(HALF_TOP_WIDTH, HALF_HEIGHT), Point2D(HALF_BOTTOM_WIDTH, -HALF_HEIGHT)), Vector2D(HEIGHT, HALF_BOTTOM_WIDTH - HALF_TOP_WIDTH));    // Right diagonal

                // topLeftTablature
                {
                    BossBodyPart* topLeftTablature = new BossBodyPart(tablatureBounds);
                    topLeftTablature->RotateZ(180);
                    topLeftTablature->Translate(Vector3D(-5.101f, 2.854f, 0.0f));
                    this->alivePartsRoot->AddBodyPart(topLeftTablature);
                    this->topLeftTablatureIdx = this->bodyParts.size();
                    this->bodyParts.push_back(topLeftTablature);
                }

                // topRightTablature
                {
                    BossBodyPart* topRightTablature = new BossBodyPart(tablatureBounds);
                    topRightTablature->RotateZ(180);
                    topRightTablature->Translate(Vector3D(5.101f, 2.854f, 0.0f));
                    this->alivePartsRoot->AddBodyPart(topRightTablature);
                    this->topRightTablatureIdx = this->bodyParts.size();
                    this->bodyParts.push_back(topRightTablature);
                }

                // bottomLeftTablature
                {
                    BossBodyPart* bottomLeftTablature = new BossBodyPart(tablatureBounds);
                    bottomLeftTablature->Translate(Vector3D(-5.101f, -2.854f, 0.0f));
                    this->alivePartsRoot->AddBodyPart(bottomLeftTablature);
                    this->bottomLeftTablatureIdx = this->bodyParts.size();
                    this->bodyParts.push_back(bottomLeftTablature);
                }

                // bottomRightTablature
                {
                    BossBodyPart* bottomRightTablature = new BossBodyPart(tablatureBounds);
                    bottomRightTablature->Translate(Vector3D(5.101f, -2.854f, 0.0f));
                    this->alivePartsRoot->AddBodyPart(bottomRightTablature);
                    this->bottomRightTablatureIdx = this->bodyParts.size();
                    this->bodyParts.push_back(bottomRightTablature);
                }
            }

            // *BodyColumnx
            {
                BoundingLines emptyBounds;
                BoundingLines columnBounds;
                this->BuildColumnBounds(columnBounds);

                // leftBodyColumn1
                {
                    BossBodyPart* leftBodyColumn1 = new BossBodyPart(columnBounds);
                    leftBodyColumn1->Translate(Vector3D(-7.275f, 0.0f, 0.0f));
                    this->alivePartsRoot->AddBodyPart(leftBodyColumn1);
                    this->leftCol1Idx = this->bodyParts.size();
                    this->bodyParts.push_back(leftBodyColumn1);
                }
                // leftBodyColumn2
                {
                    BossBodyPart* leftBodyColumn2 = new BossBodyPart(emptyBounds);
                    leftBodyColumn2->Translate(Vector3D(-5.101f, 0.0f, 0.0f));
                    this->alivePartsRoot->AddBodyPart(leftBodyColumn2);
                    this->leftCol2Idx = this->bodyParts.size();
                    this->bodyParts.push_back(leftBodyColumn2);
                }
                // leftBodyColumn3
                {
                    BossBodyPart* leftBodyColumn3 = new BossBodyPart(emptyBounds);
                    leftBodyColumn3->Translate(Vector3D(-2.927f, 0.0f, 0.0f));
                    this->alivePartsRoot->AddBodyPart(leftBodyColumn3);
                    this->leftCol3Idx = this->bodyParts.size();
                    this->bodyParts.push_back(leftBodyColumn3);
                }

                // rightBodyColumn1
                {
                    BossBodyPart* rightBodyColumn1 = new BossBodyPart(columnBounds);
                    rightBodyColumn1->Translate(Vector3D(7.275f, 0.0f, 0.0f));
                    this->alivePartsRoot->AddBodyPart(rightBodyColumn1);
                    this->rightCol1Idx = this->bodyParts.size();
                    this->bodyParts.push_back(rightBodyColumn1);
                }
                // rightBodyColumn2
                {
                    BossBodyPart* rightBodyColumn2 = new BossBodyPart(emptyBounds);
                    rightBodyColumn2->Translate(Vector3D(5.101f, 0.0f, 0.0f));
                    this->alivePartsRoot->AddBodyPart(rightBodyColumn2);
                    this->rightCol2Idx = this->bodyParts.size();
                    this->bodyParts.push_back(rightBodyColumn2);
                }
                // rightBodyColumn3
                {
                    BossBodyPart* rightBodyColumn3 = new BossBodyPart(emptyBounds);
                    rightBodyColumn3->Translate(Vector3D(2.927f, 0.0f, 0.0f));
                    this->alivePartsRoot->AddBodyPart(rightBodyColumn3);
                    this->rightCol3Idx = this->bodyParts.size();
                    this->bodyParts.push_back(rightBodyColumn3);
                }
            }

            // base
            {
                static const float WIDTH  = BASE_WIDTH;
                static const float HEIGHT = BASE_HEIGHT;
                static const float HALF_HEIGHT = HEIGHT / 2.0f;
                static const float HALF_WIDTH  = WIDTH  / 2.0f;

                BoundingLines baseBounds;
                baseBounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_WIDTH, HALF_HEIGHT), Point2D(HALF_WIDTH, HALF_HEIGHT)), Vector2D(0, 1));    // Top
                baseBounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_WIDTH, -HALF_HEIGHT), Point2D(HALF_WIDTH, -HALF_HEIGHT)), Vector2D(0, -1)); // Bottom
                baseBounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_WIDTH, HALF_HEIGHT), Point2D(-HALF_WIDTH, -HALF_HEIGHT)), Vector2D(-1, 0)); // Left
                baseBounds.AddBound(Collision::LineSeg2D(Point2D(HALF_WIDTH, HALF_HEIGHT), Point2D(HALF_WIDTH, -HALF_HEIGHT)), Vector2D(1, 0));    // Right
                
                BossBodyPart* base = new BossBodyPart(baseBounds);
                base->Translate(Vector3D(0.0f, -4.596f, 0.0f));
                this->alivePartsRoot->AddBodyPart(base);
                this->baseIdx = this->bodyParts.size();
                this->bodyParts.push_back(base);
            }

            // leftArm 
            this->BuildArm(true, Vector3D(-ARM_X_TRANSLATION_FROM_CENTER, 0.834f, 0.0f), this->leftArmIdx, this->leftRestOfArmIdx, this->leftArmSquareIdx);

            // rightArm
            this->BuildArm(false, Vector3D(ARM_X_TRANSLATION_FROM_CENTER, 0.834f, 0.0f), this->rightArmIdx, this->rightRestOfArmIdx, this->rightArmSquareIdx);
 
        } // end alivePartsRoot
    } // end root

    this->root->Translate(Vector3D(startingX, startingY, 0.0f));
    this->SetCurrentAIStateImmediately(new ArmsBodyHeadAI(this));

    // N.B., Bosses are transformed into level space by the GameLevel when they are loaded from file.
}

void ClassicalBoss::BuildArm(bool isLeftArm, const Vector3D& armTranslation, size_t& armIdx, size_t& restOfArmIdx, size_t& squareIdx) {

    static const float FAVOUR_SQUARE_SIZE = 0.25f;

    static const float SQUARE_SIZE      = 2.445f + FAVOUR_SQUARE_SIZE;
    static const float HALF_SQUARE_SIZE = SQUARE_SIZE / 2.0f;

    static const float SUPPORT_WIDTH  = SQUARE_SIZE;
    static const float SUPPORT_HEIGHT = 0.652f - FAVOUR_SQUARE_SIZE;
    static const float HALF_SUPPORT_WIDTH = SUPPORT_WIDTH / 2.0f;
    static const float HALF_SUPPORT_HEIGHT = SUPPORT_HEIGHT / 2.0f;

    BossCompositeBodyPart* arm = new BossCompositeBodyPart();
    this->alivePartsRoot->AddBodyPart(arm);
    armIdx = this->bodyParts.size();
    this->bodyParts.push_back(arm);

    // restOfArm
    {
        BoundingLines bounds;

        // Top rectangular support piece
        Vector2D translation(0.0f, 3.097f + HALF_SUPPORT_HEIGHT);
        bounds.AddBound(Collision::LineSeg2D(translation + Point2D(-HALF_SUPPORT_WIDTH, HALF_SUPPORT_HEIGHT),  translation + Point2D(HALF_SUPPORT_WIDTH, HALF_SUPPORT_HEIGHT)),   Vector2D(0, 1));  // Top
        bounds.AddBound(Collision::LineSeg2D(translation + Point2D(-HALF_SUPPORT_WIDTH, HALF_SUPPORT_HEIGHT),  translation + Point2D(-HALF_SUPPORT_WIDTH, -HALF_SUPPORT_HEIGHT)), Vector2D(-1, 0)); // Left
        bounds.AddBound(Collision::LineSeg2D(translation + Point2D(HALF_SUPPORT_WIDTH, HALF_SUPPORT_HEIGHT),   translation + Point2D(HALF_SUPPORT_WIDTH, -HALF_SUPPORT_HEIGHT)),  Vector2D(1, 0));  // Right
        //bounds.AddBound(Collision::LineSeg2D(translation + Point2D(-HALF_SUPPORT_WIDTH, -HALF_SUPPORT_HEIGHT), translation + Point2D(HALF_SUPPORT_WIDTH, -HALF_SUPPORT_HEIGHT)),  Vector2D(0, -1)); // Bottom

        // Bottom rectangular support piece
        translation[1] = HALF_SUPPORT_HEIGHT;
        //bounds.AddBound(Collision::LineSeg2D(translation + Point2D(-HALF_SUPPORT_WIDTH, HALF_SUPPORT_HEIGHT),  translation + Point2D(HALF_SUPPORT_WIDTH, HALF_SUPPORT_HEIGHT)),   Vector2D(0, 1));  // Top
        //bounds.AddBound(Collision::LineSeg2D(translation + Point2D(-HALF_SUPPORT_WIDTH, -HALF_SUPPORT_HEIGHT), translation + Point2D(HALF_SUPPORT_WIDTH, -HALF_SUPPORT_HEIGHT)),  Vector2D(0, -1)); // Bottom
        bounds.AddBound(Collision::LineSeg2D(translation + Point2D(-HALF_SUPPORT_WIDTH, HALF_SUPPORT_HEIGHT),  translation + Point2D(-HALF_SUPPORT_WIDTH, -HALF_SUPPORT_HEIGHT)), Vector2D(-1, 0), !isLeftArm); // Left
        bounds.AddBound(Collision::LineSeg2D(translation + Point2D(HALF_SUPPORT_WIDTH, HALF_SUPPORT_HEIGHT),   translation + Point2D(HALF_SUPPORT_WIDTH, -HALF_SUPPORT_HEIGHT)),  Vector2D(1, 0), isLeftArm);  // Right
    
        // Column
        static const float COLUMN_TOP_BASE_WIDTH = 2.375f;
        static const float COLUMN_TOP_BASE_HEIGHT = 0.292f;
        static const float HALF_COLUMN_TOP_BASE_WIDTH  = COLUMN_TOP_BASE_WIDTH / 2.0f;
        static const float HALF_COLUMN_TOP_BASE_HEIGHT = COLUMN_TOP_BASE_HEIGHT / 2.0f;

        static const float COLUMN_TAPERED_TOP_TOP_WIDTH  = COLUMN_TOP_BASE_WIDTH;
        static const float COLUMN_TAPERED_TOP_HEIGHT = 0.207f;
        static const float COLUMN_TAPERED_TOP_BOTTOM_WIDTH = 1.599f;
        static const float HALF_COLUMN_TAPERED_TOP_TOP_WIDTH = COLUMN_TAPERED_TOP_TOP_WIDTH / 2.0f;
        static const float HALF_COLUMN_TAPERED_TOP_BOTTOM_WIDTH = COLUMN_TAPERED_TOP_BOTTOM_WIDTH / 2.0f;
        static const float HALF_COLUMN_TAPERED_TOP_HEIGHT = COLUMN_TAPERED_TOP_HEIGHT / 2.0f;

        static const float COLUMN_HEIGHT = 4.792f;
        static const float COLUMN_TOP_WIDTH     = COLUMN_TAPERED_TOP_BOTTOM_WIDTH;
        static const float COLUMN_BOTTOM_WIDTH  = 2.375f;
        static const float HALF_COLUMN_HEIGHT = COLUMN_HEIGHT / 2.0f;
        static const float HALF_COLUMN_TOP_WIDTH  = COLUMN_TOP_WIDTH  / 2.0f;
        static const float HALF_COLUMN_BOTTOM_WIDTH  = COLUMN_BOTTOM_WIDTH  / 2.0f;

        static const float COLUMN_BOTTOM_BASE_WIDTH = COLUMN_BOTTOM_WIDTH;
        static const float COLUMN_BOTTOM_BASE_HEIGHT = 0.621f;
        static const float HALF_COLUMN_BOTTOM_BASE_WIDTH = COLUMN_BOTTOM_BASE_WIDTH / 2.0f;
        static const float HALF_COLUMN_BOTTOM_BASE_HEIGHT = COLUMN_BOTTOM_BASE_HEIGHT / 2.0f;
        
        // Top part of the column
        translation[1] = -HALF_COLUMN_TOP_BASE_HEIGHT;
        bounds.AddBound(Collision::LineSeg2D(translation + Point2D(-HALF_COLUMN_TOP_BASE_WIDTH, HALF_COLUMN_TOP_BASE_HEIGHT), translation + Point2D(-HALF_COLUMN_TOP_BASE_WIDTH, -HALF_COLUMN_TOP_BASE_HEIGHT)), Vector2D(-1, 0), !isLeftArm); // Left
        bounds.AddBound(Collision::LineSeg2D(translation + Point2D(HALF_COLUMN_TOP_BASE_WIDTH, HALF_COLUMN_TOP_BASE_HEIGHT),  translation + Point2D(HALF_COLUMN_TOP_BASE_WIDTH, -HALF_COLUMN_TOP_BASE_HEIGHT)),  Vector2D(1, 0), isLeftArm);  // Right
    
        // Tapered part of the column
        translation[1] = -(COLUMN_TOP_BASE_HEIGHT + HALF_COLUMN_TAPERED_TOP_HEIGHT);
        bounds.AddBound(Collision::LineSeg2D(translation + Point2D(-HALF_COLUMN_TAPERED_TOP_TOP_WIDTH, HALF_COLUMN_TAPERED_TOP_HEIGHT), translation + Point2D(-HALF_COLUMN_TAPERED_TOP_BOTTOM_WIDTH, -HALF_COLUMN_TAPERED_TOP_HEIGHT)), Vector2D(-COLUMN_TAPERED_TOP_HEIGHT, HALF_COLUMN_TAPERED_TOP_BOTTOM_WIDTH - HALF_COLUMN_TAPERED_TOP_TOP_WIDTH), !isLeftArm); // Left diagonal
        bounds.AddBound(Collision::LineSeg2D(translation + Point2D(HALF_COLUMN_TAPERED_TOP_TOP_WIDTH, HALF_COLUMN_TAPERED_TOP_HEIGHT),  translation + Point2D(HALF_COLUMN_TAPERED_TOP_BOTTOM_WIDTH, -HALF_COLUMN_TAPERED_TOP_HEIGHT)),  Vector2D(COLUMN_TAPERED_TOP_HEIGHT, HALF_COLUMN_TAPERED_TOP_BOTTOM_WIDTH - HALF_COLUMN_TAPERED_TOP_TOP_WIDTH), isLeftArm);    // Right diagonal

        // Middle part of the column
        translation[1] = -(COLUMN_TOP_BASE_HEIGHT + COLUMN_TAPERED_TOP_HEIGHT + HALF_COLUMN_HEIGHT);
        bounds.AddBound(Collision::LineSeg2D(translation + Point2D(-HALF_COLUMN_TOP_WIDTH, HALF_COLUMN_HEIGHT), translation + Point2D(-HALF_COLUMN_BOTTOM_WIDTH, -HALF_COLUMN_HEIGHT)), Vector2D(-COLUMN_HEIGHT, HALF_COLUMN_BOTTOM_WIDTH - HALF_COLUMN_TOP_WIDTH), !isLeftArm); // Left diagonal
        bounds.AddBound(Collision::LineSeg2D(translation + Point2D(HALF_COLUMN_TOP_WIDTH, HALF_COLUMN_HEIGHT),  translation + Point2D(HALF_COLUMN_BOTTOM_WIDTH, -HALF_COLUMN_HEIGHT)),  Vector2D(COLUMN_HEIGHT, HALF_COLUMN_BOTTOM_WIDTH - HALF_COLUMN_TOP_WIDTH), isLeftArm);    // Right diagonal

        // Bottom part of the column
        translation[1] = -(COLUMN_TOP_BASE_HEIGHT + COLUMN_TAPERED_TOP_HEIGHT + COLUMN_HEIGHT + HALF_COLUMN_BOTTOM_BASE_HEIGHT);
        bounds.AddBound(Collision::LineSeg2D(translation + Point2D(-HALF_COLUMN_BOTTOM_BASE_WIDTH, -HALF_COLUMN_BOTTOM_BASE_HEIGHT), translation + Point2D(HALF_COLUMN_BOTTOM_BASE_WIDTH, -HALF_COLUMN_BOTTOM_BASE_HEIGHT)),  Vector2D(0, -1)); // Bottom
        bounds.AddBound(Collision::LineSeg2D(translation + Point2D(-HALF_COLUMN_BOTTOM_BASE_WIDTH, HALF_COLUMN_BOTTOM_BASE_HEIGHT),  translation + Point2D(-HALF_COLUMN_BOTTOM_BASE_WIDTH, -HALF_COLUMN_BOTTOM_BASE_HEIGHT)), Vector2D(-1, 0), !isLeftArm); // Left
        bounds.AddBound(Collision::LineSeg2D(translation + Point2D(HALF_COLUMN_BOTTOM_BASE_WIDTH, HALF_COLUMN_BOTTOM_BASE_HEIGHT),   translation + Point2D(HALF_COLUMN_BOTTOM_BASE_WIDTH, -HALF_COLUMN_BOTTOM_BASE_HEIGHT)),  Vector2D(1, 0), isLeftArm);  // Right
    
        BossBodyPart* armBase = new BossBodyPart(bounds);
        arm->AddBodyPart(armBase);
        restOfArmIdx = this->bodyParts.size();
        this->bodyParts.push_back(armBase);
    }

    // square
    {
        BoundingLines bounds;
        bounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_SQUARE_SIZE, HALF_SQUARE_SIZE),  Point2D(-HALF_SQUARE_SIZE, -HALF_SQUARE_SIZE)), Vector2D(-1, 0), !isLeftArm); // Left
        bounds.AddBound(Collision::LineSeg2D(Point2D(HALF_SQUARE_SIZE, HALF_SQUARE_SIZE),   Point2D(HALF_SQUARE_SIZE, -HALF_SQUARE_SIZE)),  Vector2D(1, 0), isLeftArm);  // Right
    
        BossBodyPart* square = new BossBodyPart(bounds);
        square->Translate(Vector3D(0.0f, SUPPORT_HEIGHT + HALF_SQUARE_SIZE, 0.0f));
        arm->AddBodyPart(square);
        squareIdx = this->bodyParts.size();
        this->bodyParts.push_back(square);
    }

    arm->Translate(armTranslation);
}

void ClassicalBoss::GenerateFullColumnPedimentBounds() {
    
    // Add the bottom bound for the pediment...
    {
        static const float HEIGHT = PEDIMENT_HEIGHT;
        static const float WIDTH  = PEDIMENT_WIDTH;
        static const float HALF_HEIGHT = HEIGHT / 2.0f;
        static const float HALF_WIDTH  = WIDTH  / 2.0f;
    
        BossBodyPart* pediment = static_cast<BossBodyPart*>(this->bodyParts[this->pedimentIdx]);
        BoundingLines pedimentBounds = pediment->GetLocalBounds();
        pedimentBounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_WIDTH, -HALF_HEIGHT), Point2D(HALF_WIDTH, -HALF_HEIGHT)), Vector2D(0, -1));        // bottom
        pediment->SetLocalBounds(pedimentBounds);
    }

    // Add the remaining column bounds
    {
        BoundingLines columnBounds;
        this->BuildColumnBounds(columnBounds);

        BossBodyPart* leftCol2 = static_cast<BossBodyPart*>(this->bodyParts[this->leftCol2Idx]);
        leftCol2->SetLocalBounds(columnBounds);
        BossBodyPart* leftCol3 = static_cast<BossBodyPart*>(this->bodyParts[this->leftCol3Idx]);
        leftCol3->SetLocalBounds(columnBounds);
        
        BossBodyPart* rightCol2 = static_cast<BossBodyPart*>(this->bodyParts[this->rightCol2Idx]);
        rightCol2->SetLocalBounds(columnBounds);
        BossBodyPart* rightCol3 = static_cast<BossBodyPart*>(this->bodyParts[this->rightCol3Idx]);
        rightCol3->SetLocalBounds(columnBounds);
    }

}

void ClassicalBoss::GenerateEyeBounds() {
    BoundingLines eyeBounds;
    this->BuildEyeBounds(eyeBounds);
    BossBodyPart* eye = static_cast<BossBodyPart*>(this->bodyParts[this->eyeIdx]);
    eye->SetLocalBounds(eyeBounds);
}

void ClassicalBoss::BuildEyeBounds(BoundingLines& eyeBounds) {
    static const float HEIGHT = EYE_HEIGHT;
    static const float WIDTH  = EYE_WIDTH;
    static const float HALF_HEIGHT = HEIGHT / 2.0f;
    static const float HALF_WIDTH  = WIDTH  / 2.0f;

    eyeBounds.Clear();
    eyeBounds.AddBound(Collision::LineSeg2D(Point2D(0.0f, HALF_HEIGHT), Point2D(-HALF_WIDTH, 0.0f)), Vector2D(-HALF_HEIGHT, HALF_WIDTH));   // top-left diagonal
    eyeBounds.AddBound(Collision::LineSeg2D(Point2D(0.0f, HALF_HEIGHT), Point2D(HALF_WIDTH, 0.0f)), Vector2D(HALF_HEIGHT, HALF_WIDTH));     // top-right diagonal
    eyeBounds.AddBound(Collision::LineSeg2D(Point2D(0.0f, -HALF_HEIGHT), Point2D(-HALF_WIDTH, 0.0f)), Vector2D(-HALF_HEIGHT, -HALF_WIDTH)); // bottom-left diagonal
    eyeBounds.AddBound(Collision::LineSeg2D(Point2D(0.0f, -HALF_HEIGHT), Point2D(HALF_WIDTH, 0.0f)), Vector2D(HALF_HEIGHT, -HALF_WIDTH));   // bottom-right diagonal
}

void ClassicalBoss::BuildColumnBounds(BoundingLines& columnBounds) {
    static const float HEIGHT = COLUMN_HEIGHT;
    static const float WIDTH  = COLUMN_WIDTH;
    static const float HALF_HEIGHT = HEIGHT / 2.0f;
    static const float HALF_WIDTH  = WIDTH  / 2.0f;

    columnBounds.Clear();
    columnBounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_WIDTH, HALF_HEIGHT), Point2D(-HALF_WIDTH, -HALF_HEIGHT)), Vector2D(-1, 0)); // Left
    columnBounds.AddBound(Collision::LineSeg2D(Point2D(HALF_WIDTH, HALF_HEIGHT), Point2D(HALF_WIDTH, -HALF_HEIGHT)), Vector2D(1, 0));    // Right
}

std::vector<const BossBodyPart*> ClassicalBoss::GetBodyColumns() const {
    std::vector<const BossBodyPart*> columns;
    columns.reserve(6);
    
    columns.push_back(static_cast<const BossBodyPart*>(this->bodyParts[this->leftCol1Idx]));
    columns.push_back(static_cast<const BossBodyPart*>(this->bodyParts[this->leftCol2Idx]));
    columns.push_back(static_cast<const BossBodyPart*>(this->bodyParts[this->leftCol3Idx]));
    columns.push_back(static_cast<const BossBodyPart*>(this->bodyParts[this->rightCol1Idx]));
    columns.push_back(static_cast<const BossBodyPart*>(this->bodyParts[this->rightCol2Idx]));
    columns.push_back(static_cast<const BossBodyPart*>(this->bodyParts[this->rightCol3Idx]));

    return columns;
}
