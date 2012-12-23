/**
 * ClassicalBoss.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "ClassicalBoss.h"
#include "BossCompositeBodyPart.h"
#include "BossBodyPart.h"
#include "BossWeakpoint.h"

#include "ClassicalBossAIStates.h"

using namespace classicalbossai;

ClassicalBoss::ClassicalBoss() : Boss(), deadPartsRoot(NULL), alivePartsRoot(NULL),
leftArmIdx(0), leftArmSquareIdx(0), rightArmIdx(0), rightArmSquareIdx(0)
{
}

ClassicalBoss::~ClassicalBoss() {
}

void ClassicalBoss::Init() {
    
    
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
                static const float HEIGHT = 1.575f;
                static const float WIDTH  = 3.150f;
                static const float HALF_HEIGHT = HEIGHT / 2.0f;
                static const float HALF_WIDTH  = WIDTH  / 2.0f;

                BoundingLines eyeBounds;
                eyeBounds.AddBound(Collision::LineSeg2D(Point2D(0.0f, HALF_HEIGHT), Point2D(-HALF_WIDTH, 0.0f)), Vector2D(-HALF_HEIGHT, HALF_WIDTH));   // top-left diagonal
                eyeBounds.AddBound(Collision::LineSeg2D(Point2D(0.0f, HALF_HEIGHT), Point2D(HALF_WIDTH, 0.0f)), Vector2D(HALF_HEIGHT, HALF_WIDTH));     // top-right diagonal
                eyeBounds.AddBound(Collision::LineSeg2D(Point2D(0.0f, -HALF_HEIGHT), Point2D(-HALF_WIDTH, 0.0f)), Vector2D(-HALF_HEIGHT, -HALF_WIDTH)); // bottom-left diagonal
                eyeBounds.AddBound(Collision::LineSeg2D(Point2D(0.0f, -HALF_HEIGHT), Point2D(HALF_WIDTH, 0.0f)), Vector2D(HALF_HEIGHT, -HALF_WIDTH));   // bottom-right diagonal

                BossBodyPart* eye = new BossBodyPart(eyeBounds);

                // Translation from the local-space center of the entire boss body
                Vector3D eyeTranslation(0.0f, 5.081f, 1.053f);
                eye->Translate(eyeTranslation);

                this->alivePartsRoot->AddBodyPart(eye);
                this->bodyParts.push_back(eye);
            }

            // pediment
            {
                static const float HEIGHT = 3.16f;
                static const float WIDTH  = 17.823f;
                static const float HALF_HEIGHT = HEIGHT / 2.0f;
                static const float HALF_WIDTH  = WIDTH  / 2.0f;

                BoundingLines pedimentBounds;
                pedimentBounds.AddBound(Collision::LineSeg2D(Point2D(0.0f, HALF_HEIGHT), Point2D(-HALF_WIDTH, -HALF_HEIGHT)), Vector2D(-HEIGHT, HALF_WIDTH)); // top-left diagonal
                pedimentBounds.AddBound(Collision::LineSeg2D(Point2D(0.0f, HALF_HEIGHT), Point2D(HALF_WIDTH, -HALF_HEIGHT)), Vector2D(HEIGHT, HALF_WIDTH));   // top-right diagonal
                pedimentBounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_WIDTH, -HALF_HEIGHT), Point2D(HALF_WIDTH, -HALF_HEIGHT)), Vector2D(0, -1));        // bottom

                BossBodyPart* pediment = new BossBodyPart(pedimentBounds);

                // Translation from the local-space center of the entire boss body
                Vector3D pedimentTranslation(0.0f, 5.0905f, 0.0f);
                pediment->Translate(pedimentTranslation);

                this->alivePartsRoot->AddBodyPart(pediment);
                this->bodyParts.push_back(pediment);
            }

            // *Tablature
            {
                static const float HEIGHT = 1.334f;
                static const float BOTTOM_WIDTH  = 7.621f;
                static const float TOP_WIDTH = 6.933f;
                static const float HALF_HEIGHT = HEIGHT / 2.0f;
                static const float HALF_BOTTOM_WIDTH  = BOTTOM_WIDTH  / 2.0f;
                static const float HALF_TOP_WIDTH = TOP_WIDTH / 2.0f;
                
                BoundingLines tablatureBounds;
                tablatureBounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_TOP_WIDTH, HALF_HEIGHT), Point2D(HALF_TOP_WIDTH, HALF_HEIGHT)), Vector2D(0, 1));            // Top
                tablatureBounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_BOTTOM_WIDTH, -HALF_HEIGHT), Point2D(HALF_BOTTOM_WIDTH, -HALF_HEIGHT)), Vector2D(0, -1));   // Bottom
                tablatureBounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_TOP_WIDTH, HALF_HEIGHT), Point2D(-HALF_BOTTOM_WIDTH, -HALF_HEIGHT)), Vector2D(-HEIGHT, HALF_BOTTOM_WIDTH - HALF_TOP_WIDTH)); // Left diagonal
                tablatureBounds.AddBound(Collision::LineSeg2D(Point2D(HALF_TOP_WIDTH, HALF_HEIGHT), Point2D(HALF_BOTTOM_WIDTH, -HALF_HEIGHT)), Vector2D(HEIGHT, HALF_BOTTOM_WIDTH - HALF_TOP_WIDTH));    // Right diagonal

                // topLeftTablature
                {
                    BossBodyPart* topLeftTablature = new BossBodyPart(tablatureBounds);
                    topLeftTablature->RotateZ(180);
                    topLeftTablature->Translate(Vector3D(-5.101f, 2.854f, 0.0f));
                    this->alivePartsRoot->AddBodyPart(topLeftTablature);
                    this->bodyParts.push_back(topLeftTablature);
                }

                // topRightTablature
                {
                    BossBodyPart* topRightTablature = new BossBodyPart(tablatureBounds);
                    topRightTablature->RotateZ(180);
                    topRightTablature->Translate(Vector3D(5.101f, 2.854f, 0.0f));
                    this->alivePartsRoot->AddBodyPart(topRightTablature);
                    this->bodyParts.push_back(topRightTablature);
                }

                // bottomLeftTablature
                {
                    BossBodyPart* bottomLeftTablature = new BossBodyPart(tablatureBounds);
                    bottomLeftTablature->Translate(Vector3D(-5.101f, -2.854f, 0.0f));
                    this->alivePartsRoot->AddBodyPart(bottomLeftTablature);
                    this->bodyParts.push_back(bottomLeftTablature);
                }

                // bottomRightTablature
                {
                    BossBodyPart* bottomLeftTablature = new BossBodyPart(tablatureBounds);
                    bottomLeftTablature->Translate(Vector3D(5.101f, -2.854f, 0.0f));
                    this->alivePartsRoot->AddBodyPart(bottomLeftTablature);
                    this->bodyParts.push_back(bottomLeftTablature);
                }
            }

            // *BodyColumnx
            {
                static const float HEIGHT = 4.373f;
                static const float WIDTH  = 1.704f;
                static const float HALF_HEIGHT = HEIGHT / 2.0f;
                static const float HALF_WIDTH  = WIDTH  / 2.0f;

                BoundingLines columnBounds;
                columnBounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_WIDTH, HALF_HEIGHT), Point2D(HALF_WIDTH, HALF_HEIGHT)), Vector2D(0, 1));    // Top
                columnBounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_WIDTH, -HALF_HEIGHT), Point2D(HALF_WIDTH, -HALF_HEIGHT)), Vector2D(0, -1)); // Bottom
                columnBounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_WIDTH, HALF_HEIGHT), Point2D(-HALF_WIDTH, -HALF_HEIGHT)), Vector2D(-1, 0)); // Left
                columnBounds.AddBound(Collision::LineSeg2D(Point2D(HALF_WIDTH, HALF_HEIGHT), Point2D(HALF_WIDTH, -HALF_HEIGHT)), Vector2D(1, 0));    // Right

                // leftBodyColumn1
                {
                    BossBodyPart* leftBodyColumn1 = new BossBodyPart(columnBounds);
                    leftBodyColumn1->Translate(Vector3D(-7.275f, 0.0f, 0.0f));
                    this->alivePartsRoot->AddBodyPart(leftBodyColumn1);
                    this->bodyParts.push_back(leftBodyColumn1);
                }
                // leftBodyColumn2
                {
                    BossBodyPart* leftBodyColumn2 = new BossBodyPart(columnBounds);
                    leftBodyColumn2->Translate(Vector3D(-5.101f, 0.0f, 0.0f));
                    this->alivePartsRoot->AddBodyPart(leftBodyColumn2);
                    this->bodyParts.push_back(leftBodyColumn2);
                }
                // leftBodyColumn3
                {
                    BossBodyPart* leftBodyColumn3 = new BossBodyPart(columnBounds);
                    leftBodyColumn3->Translate(Vector3D(-2.927f, 0.0f, 0.0f));
                    this->alivePartsRoot->AddBodyPart(leftBodyColumn3);
                    this->bodyParts.push_back(leftBodyColumn3);
                }

                // rightBodyColumn1
                {
                    BossBodyPart* rightBodyColumn1 = new BossBodyPart(columnBounds);
                    rightBodyColumn1->Translate(Vector3D(7.275f, 0.0f, 0.0f));
                    this->alivePartsRoot->AddBodyPart(rightBodyColumn1);
                    this->bodyParts.push_back(rightBodyColumn1);
                }
                // rightBodyColumn2
                {
                    BossBodyPart* rightBodyColumn2 = new BossBodyPart(columnBounds);
                    rightBodyColumn2->Translate(Vector3D(5.101f, 0.0f, 0.0f));
                    this->alivePartsRoot->AddBodyPart(rightBodyColumn2);
                    this->bodyParts.push_back(rightBodyColumn2);
                }
                // rightBodyColumn3
                {
                    BossBodyPart* rightBodyColumn3 = new BossBodyPart(columnBounds);
                    rightBodyColumn3->Translate(Vector3D(2.927f, 0.0f, 0.0f));
                    this->alivePartsRoot->AddBodyPart(rightBodyColumn3);
                    this->bodyParts.push_back(rightBodyColumn3);
                }
            }

            // base
            {
                static const float WIDTH  = 18.355f;
                static const float HEIGHT = 2.150f;
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
                this->bodyParts.push_back(base);
            }

            // leftArm
            this->BuildArm(Vector3D(-10.246f, 0.0f, 0.0f), this->leftArmIdx, this->leftArmSquareIdx);

            // rightArm
            this->BuildArm(Vector3D(10.246f, 0.0f, 0.0f), this->rightArmIdx, this->rightArmSquareIdx);

        } // end alivePartsRoot
    } // end root

    this->SetNextAIState(new ArmsBodyHeadAI(this));
}

void ClassicalBoss::BuildArm(const Vector3D& armTranslation, size_t& armIdx, size_t& squareIdx) {

    static const float SQUARE_SIZE      = 2.445f;
    static const float HALF_SQUARE_SIZE = SQUARE_SIZE / 2.0f;

    static const float SUPPORT_WIDTH  = 3.097f;
    static const float SUPPORT_HEIGHT = 0.652f;
    static const float HALF_SUPPORT_WIDTH = SUPPORT_WIDTH / 2.0f;
    static const float HALF_SUPPORT_HEIGHT = SUPPORT_HEIGHT / 2.0f;

    static const float ARM_COLUMN_WIDTH  = 2.589f;
    static const float ARM_COLUMN_HEIGHT = 5.911f;
    static const float HALF_ARM_COLUMN_WIDTH  = ARM_COLUMN_WIDTH / 2.0f;
    static const float HALF_ARM_COLUMN_HEIGHT = ARM_COLUMN_HEIGHT / 2.0f;

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
        bounds.AddBound(Collision::LineSeg2D(translation + Point2D(-HALF_SUPPORT_WIDTH, -HALF_SUPPORT_HEIGHT), translation + Point2D(HALF_SUPPORT_WIDTH, -HALF_SUPPORT_HEIGHT)),  Vector2D(0, -1)); // Bottom
        bounds.AddBound(Collision::LineSeg2D(translation + Point2D(-HALF_SUPPORT_WIDTH, HALF_SUPPORT_HEIGHT),  translation + Point2D(-HALF_SUPPORT_WIDTH, -HALF_SUPPORT_HEIGHT)), Vector2D(-1, 0)); // Left
        bounds.AddBound(Collision::LineSeg2D(translation + Point2D(HALF_SUPPORT_WIDTH, HALF_SUPPORT_HEIGHT),   translation + Point2D(HALF_SUPPORT_WIDTH, -HALF_SUPPORT_HEIGHT)),  Vector2D(1, 0));  // Right
    
        // Bottom rectangular support piece
        translation[1] = HALF_SUPPORT_HEIGHT;
        bounds.AddBound(Collision::LineSeg2D(translation + Point2D(-HALF_SUPPORT_WIDTH, HALF_SUPPORT_HEIGHT),  translation + Point2D(HALF_SUPPORT_WIDTH, HALF_SUPPORT_HEIGHT)),   Vector2D(0, 1));  // Top
        bounds.AddBound(Collision::LineSeg2D(translation + Point2D(-HALF_SUPPORT_WIDTH, -HALF_SUPPORT_HEIGHT), translation + Point2D(HALF_SUPPORT_WIDTH, -HALF_SUPPORT_HEIGHT)),  Vector2D(0, -1)); // Bottom
        bounds.AddBound(Collision::LineSeg2D(translation + Point2D(-HALF_SUPPORT_WIDTH, HALF_SUPPORT_HEIGHT),  translation + Point2D(-HALF_SUPPORT_WIDTH, -HALF_SUPPORT_HEIGHT)), Vector2D(-1, 0)); // Left
        bounds.AddBound(Collision::LineSeg2D(translation + Point2D(HALF_SUPPORT_WIDTH, HALF_SUPPORT_HEIGHT),   translation + Point2D(HALF_SUPPORT_WIDTH, -HALF_SUPPORT_HEIGHT)),  Vector2D(1, 0));  // Right
    
        // Column
        translation[1] = -HALF_ARM_COLUMN_HEIGHT;
        bounds.AddBound(Collision::LineSeg2D(translation + Point2D(-HALF_ARM_COLUMN_WIDTH, HALF_ARM_COLUMN_HEIGHT),  translation + Point2D(HALF_ARM_COLUMN_WIDTH, HALF_ARM_COLUMN_HEIGHT)),   Vector2D(0, 1));  // Top
        bounds.AddBound(Collision::LineSeg2D(translation + Point2D(-HALF_ARM_COLUMN_WIDTH, -HALF_ARM_COLUMN_HEIGHT), translation + Point2D(HALF_ARM_COLUMN_WIDTH, -HALF_ARM_COLUMN_HEIGHT)),  Vector2D(0, -1)); // Bottom
        bounds.AddBound(Collision::LineSeg2D(translation + Point2D(-HALF_ARM_COLUMN_WIDTH, HALF_ARM_COLUMN_HEIGHT),  translation + Point2D(-HALF_ARM_COLUMN_WIDTH, -HALF_ARM_COLUMN_HEIGHT)), Vector2D(-1, 0)); // Left
        bounds.AddBound(Collision::LineSeg2D(translation + Point2D(HALF_ARM_COLUMN_WIDTH, HALF_ARM_COLUMN_HEIGHT),   translation + Point2D(HALF_ARM_COLUMN_WIDTH, -HALF_ARM_COLUMN_HEIGHT)),  Vector2D(1, 0));  // Right
    
        BossBodyPart* armBase = new BossBodyPart(bounds);
        arm->AddBodyPart(armBase);
        this->bodyParts.push_back(armBase);
    }

    // square
    {
        BoundingLines bounds;
        bounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_SQUARE_SIZE, HALF_SQUARE_SIZE),  Point2D(HALF_SQUARE_SIZE, HALF_SQUARE_SIZE)),   Vector2D(0, 1));  // Top
        bounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_SQUARE_SIZE, -HALF_SQUARE_SIZE), Point2D(HALF_SQUARE_SIZE, -HALF_SQUARE_SIZE)),  Vector2D(0, -1)); // Bottom
        bounds.AddBound(Collision::LineSeg2D(Point2D(-HALF_SQUARE_SIZE, HALF_SQUARE_SIZE),  Point2D(-HALF_SQUARE_SIZE, -HALF_SQUARE_SIZE)), Vector2D(-1, 0)); // Left
        bounds.AddBound(Collision::LineSeg2D(Point2D(HALF_SQUARE_SIZE, HALF_SQUARE_SIZE),   Point2D(HALF_SQUARE_SIZE, -HALF_SQUARE_SIZE)),  Vector2D(1, 0));  // Right
    
        BossBodyPart* square = new BossBodyPart(bounds);
        square->Translate(Vector3D(0.0f, SUPPORT_HEIGHT + HALF_SQUARE_SIZE, 0.0f));
        arm->AddBodyPart(square);
        squareIdx = this->bodyParts.size();
        this->bodyParts.push_back(square);
    }

    arm->Translate(armTranslation);
}

bool ClassicalBoss::GetIsDead() const {
    return false; // TODO
}

void ClassicalBoss::CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart) {
}

void ClassicalBoss::CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart) {
}

void ClassicalBoss::CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle, BossBodyPart* collisionPart) {
}
