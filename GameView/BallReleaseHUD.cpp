/**
 * BallReleaseHUD.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "BallReleaseHUD.h"
#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/GeometryMaker.h"
#include "../GameModel/GameModel.h"

BallReleaseHUD::BallReleaseHUD() : lastKnownPercentElapsedAmt(1.0f) {
    this->SetState(BallReleaseHUD::NOT_STARTED);
}

BallReleaseHUD::~BallReleaseHUD() {
}

void BallReleaseHUD::Draw(double dT, const Camera& camera, const GameModel& gameModel) {
    switch (this->currState) {
        case BallReleaseHUD::NOT_STARTED:
            // Do nothing.
            return;

        case BallReleaseHUD::STARTING:
            {
                bool finished = this->fadeAnim.Tick(dT);
                this->scaleAnim.Tick(dT);
                this->DrawReleaseTimer(camera, gameModel, gameModel.GetPlayerPaddle()->GetCenterPosition(),
                    this->scaleAnim.GetInterpolantValue(), this->fadeAnim.GetInterpolantValue(), 0.0f, 
                    gameModel.GetPercentBallReleaseTimerElapsed());
                if (finished) {
                    this->SetState(BallReleaseHUD::TICKING);
                }
            }
            break;

        case BallReleaseHUD::TICKING:
            {
                this->DrawReleaseTimer(camera, gameModel, gameModel.GetPlayerPaddle()->GetCenterPosition(),
                    1.0f, 1.0f, 0.0f, gameModel.GetPercentBallReleaseTimerElapsed());
            
                if (gameModel.GetCurrentStateType() != GameState::BallOnPaddleStateType) {
                    this->SetState(BallReleaseHUD::RELEASED);
                }
                else if (gameModel.GetPercentBallReleaseTimerElapsed() > 0.8f) {
                    this->SetState(BallReleaseHUD::ENDING);
                }
            }
            break;

        case BallReleaseHUD::ENDING:
            this->scaleAnim.Tick(dT);
            this->flashAnim.Tick(dT);
            this->DrawReleaseTimer(camera, gameModel, gameModel.GetPlayerPaddle()->GetCenterPosition(),
                this->scaleAnim.GetInterpolantValue(), 
                1.0f, this->flashAnim.GetInterpolantValue(), gameModel.GetPercentBallReleaseTimerElapsed());
            if (gameModel.GetPercentBallReleaseTimerElapsed() >= 1.0f) {
                this->SetState(BallReleaseHUD::RELEASED);
            }

            break;

        case BallReleaseHUD::RELEASED:
            {
                bool finished = this->fadeAnim.Tick(dT);
                this->scaleAnim.Tick(dT);
                this->DrawReleaseTimer(camera, gameModel, this->lastKnownPaddlePos, this->scaleAnim.GetInterpolantValue(), 
                    this->fadeAnim.GetInterpolantValue(), 0.0f, this->lastKnownPercentElapsedAmt);
                if (finished) {
                    this->SetState(BallReleaseHUD::NOT_STARTED);
                }   
            }
            break;

        default:
            assert(false);
            return;
    }
}

void BallReleaseHUD::Reinitialize() {
    this->SetState(BallReleaseHUD::NOT_STARTED);
}

void BallReleaseHUD::SetState(const BallReleaseHUD::State& state) {
    switch (state) {
        case BallReleaseHUD::NOT_STARTED:

            // Clean up everything...
            this->fadeAnim.ClearLerp();
            this->fadeAnim.SetInterpolantValue(0.0f);
            this->scaleAnim.ClearLerp();
            this->scaleAnim.SetInterpolantValue(0.0f);
            this->flashAnim.ClearLerp();
            this->flashAnim.SetInterpolantValue(0.0f);
            this->lastKnownPercentElapsedAmt = 1.0f;
            
            break;

        case BallReleaseHUD::STARTING:
            // The timer will fade in from a larger size...
            this->fadeAnim.SetLerp(0.5, 1.0f);
            this->fadeAnim.SetRepeat(false);

            this->scaleAnim.SetInterpolantValue(2.25f);
            this->scaleAnim.SetLerp(0.5, 1.0f);
            this->scaleAnim.SetRepeat(false);
            break;

        case BallReleaseHUD::TICKING:
            // The timer will stay the same size and tick away until it's close to ending
            this->fadeAnim.ClearLerp();
            this->fadeAnim.SetInterpolantValue(1.0f);
            this->fadeAnim.SetRepeat(false);
            this->scaleAnim.ClearLerp();
            this->scaleAnim.SetInterpolantValue(1.0f);
            this->scaleAnim.SetRepeat(false);
            break;
        
        case BallReleaseHUD::ENDING:
            {
                // The timer will start to flash and pulse until it expires
                std::vector<double> timeValues;
                timeValues.reserve(3);
                timeValues.push_back(0.0);
                timeValues.push_back(0.4);
                timeValues.push_back(0.8);

                std::vector<float> scaleValues;
                scaleValues.reserve(3);
                scaleValues.push_back(1.0f);
                scaleValues.push_back(1.1f);
                scaleValues.push_back(1.0f);

                this->scaleAnim.SetInterpolantValue(1.0f);
                this->scaleAnim.SetLerp(timeValues, scaleValues);
                this->scaleAnim.SetRepeat(true);

                timeValues.clear();
                timeValues.reserve(3);
                timeValues.push_back(0.0);
                timeValues.push_back(0.1);
                timeValues.push_back(0.2);

                std::vector<float> flashValues;
                flashValues.reserve(3);
                flashValues.push_back(0.0f);
                flashValues.push_back(0.75f);
                flashValues.push_back(0.0f);

                this->flashAnim.SetInterpolantValue(0.0f);
                this->flashAnim.SetLerp(timeValues, flashValues);
                this->flashAnim.SetRepeat(true);
            }

            break;

        case BallReleaseHUD::RELEASED:
            // The timer will fade out and grow larger in size as it fades...
            this->fadeAnim.SetLerp(0.35, 0.0f);
            this->fadeAnim.SetRepeat(false);
            this->scaleAnim.SetLerp(0.35, 2.25f);
            this->scaleAnim.SetRepeat(false);
            break;

        default:
            assert(false);
            return;
    }

    this->currState = state;
}

void BallReleaseHUD::DrawReleaseTimer(const Camera& camera, const GameModel& gameModel, const Point2D& paddlePos,
                                      float scale, float alpha, float flashAmt, float percentElapsed) {

    static const Vector3D EXPIRE_COLOUR(1,0,0);
    static const Vector3D MID_COLOUR(1,1,0);
    static const Vector3D FULL_COLOUR(0,1,0);

    alpha *= 0.75f;

    float fillPercent = 1.0f - percentElapsed;
    const PlayerPaddle* paddle = gameModel.GetPlayerPaddle();
    Vector2D negHalfLevelDim = -0.5 * gameModel.GetLevelUnitDimensions();

    float paddleHeight = 2*paddle->GetHalfHeight();
    float paddleWidth  = 2*paddle->GetHalfWidthTotal();
    float halfBarHeight = 0.9f * paddle->GetHalfHeight();

    Matrix4x4 screenAlignMatrix = camera.GenerateScreenAlignMatrix();
    Vector3D barColour = FULL_COLOUR + percentElapsed * (EXPIRE_COLOUR - FULL_COLOUR);
    barColour = barColour + Vector3D(flashAmt,flashAmt,flashAmt);
    
	glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT | GL_LINE_BIT);
	glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
    glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], 0.0f);
    glTranslatef(paddlePos[0], paddlePos[1] - (1.2f*paddleHeight + halfBarHeight), 0.0f);
    glMultMatrixf(screenAlignMatrix.begin());

    glPushMatrix();
    glColor4f(barColour[0], barColour[1], barColour[2], alpha);
	glScalef(scale, scale, 1.0f);
	glPolygonMode(GL_FRONT, GL_FILL);

    // Draw the fill quad based on the percentage of fill...
    glBegin(GL_QUADS);
    
    glVertex2f(-paddle->GetHalfWidthTotal(), -halfBarHeight);
    glVertex2f(-paddle->GetHalfWidthTotal() + fillPercent*paddleWidth,  -halfBarHeight);
    glVertex2f(-paddle->GetHalfWidthTotal() + fillPercent*paddleWidth,   halfBarHeight);
    glVertex2f(-paddle->GetHalfWidthTotal(),  halfBarHeight);
    glEnd();
    glPopMatrix();

    // Draw outline...
    glPushMatrix();
    glPolygonMode(GL_FRONT, GL_LINE);
    glColor4f(0,0,0,alpha);
    glLineWidth(2.0f);
    glScalef(scale*paddleWidth, scale*2*halfBarHeight, 1.0f);
    GeometryMaker::GetInstance()->DrawQuad();
	glPopMatrix();

    glPopMatrix();
	glPopAttrib();

    this->lastKnownPercentElapsedAmt = percentElapsed;
    this->lastKnownPaddlePos = paddlePos;
}