/**
 * PointsHUD.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "PointsHUD.h"
#include "GameFontAssetsManager.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/TextLabel.h"
#include "../BlammoEngine/GeometryMaker.h"
#include "../BlammoEngine/Texture.h"

#include "../GameModel/GameLevel.h"

#include "../ResourceManager.h"

const int PointsHUD::STAR_SIZE                          = 40;
const int PointsHUD::STAR_GAP                           = 3;
const int PointsHUD::SCREEN_EDGE_VERTICAL_GAP           = 10;
const int PointsHUD::SCREEN_EDGE_HORIZONTAL_GAP         = 15;
const int PointsHUD::STAR_TO_SCORE_VERTICAL_GAP         = 5;
const int PointsHUD::SCORE_TO_MULTIPLER_HORIZONTAL_GAP  = 5;
const int PointsHUD::ALL_STARS_WIDTH = (STAR_GAP * (GameLevel::MAX_STARS_PER_LEVEL-1) + STAR_SIZE * GameLevel::MAX_STARS_PER_LEVEL);

PointsHUD::PointsHUD() : numStars(0), currPtScore(0), 
ptScoreLabel(NULL), starTex(NULL), multiplier(new MultiplierHUD()) {

    this->ptScoreLabel = new TextLabel2D(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium), "0");
    this->ptScoreLabel->SetColour(Colour(1.0f, 0.4f, 0.0f));
    this->ptScoreLabel->SetDropShadow(Colour(1,1,1), 0.05f);
    this->ptScoreLabel->SetScale(0.85f);
    
    this->scoreAnimator = AnimationLerp<long>(&this->currPtScore);
    this->scoreAnimator.SetRepeat(false);

    this->starTex = ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_STAR,
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->starTex != NULL);
}

PointsHUD::~PointsHUD() {
    delete this->ptScoreLabel;
    this->ptScoreLabel = NULL;
    delete this->multiplier;
    this->multiplier = NULL;

    // Clean up any leftover notifications
    this->ClearNotifications();

    // Release textures
    bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->starTex);
    assert(success);
}

void PointsHUD::Draw(int displayWidth, int displayHeight, double dT) {
    this->scoreAnimator.Tick(dT);

    float currentX = displayWidth  - SCREEN_EDGE_HORIZONTAL_GAP;
    float currentY = displayHeight - SCREEN_EDGE_VERTICAL_GAP;
    
	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Camera::PushWindowCoords();
	glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
	glLoadIdentity();

    // Draw the current star score
    this->DrawIdleStars(currentX, currentY, dT);
    currentY -= (STAR_SIZE + STAR_TO_SCORE_VERTICAL_GAP);

    // Draw the current total point score
    std::stringstream ptScoreString;
    ptScoreString << this->currPtScore;
    this->ptScoreLabel->SetText(ptScoreString.str());
    
    currentX = displayWidth - SCREEN_EDGE_HORIZONTAL_GAP - this->ptScoreLabel->GetLastRasterWidth();
    this->ptScoreLabel->SetTopLeftCorner(currentX, currentY);
    this->ptScoreLabel->Draw();

    // Draw the multiplier if it's something greater than 1
    this->multiplier->Draw(displayWidth - SCREEN_EDGE_HORIZONTAL_GAP - ALL_STARS_WIDTH - 
                           SCORE_TO_MULTIPLER_HORIZONTAL_GAP, currentY + STAR_SIZE/2, dT);

    Camera::PopWindowCoords();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    // Draw any point notifications
    for (PointNotifyListIter iter = this->ptNotifications.begin(); iter != this->ptNotifications.end();) {
        PointNotification* ptNotification = *iter;
        if (ptNotification->Draw(displayWidth, dT)) {
            delete ptNotification;
            ptNotification = NULL;
            iter = this->ptNotifications.erase(iter);
        }
        else {
            ++iter;
        }
    }

    glPopAttrib();
}

void PointsHUD::DrawIdleStars(float rightMostX, float topMostY, double dT) {
    UNUSED_PARAMETER(dT);

    static const float STAR_HALF_SIZE  = STAR_SIZE / 2.0f;
    
    const Colour& activeStarColour   = GameViewConstants::GetInstance()->ACTIVE_POINT_STAR_COLOUR;
    const Colour& inactiveStarColour = GameViewConstants::GetInstance()->INACTIVE_POINT_STAR_COLOUR;

    float currentCenterX = rightMostX - ALL_STARS_WIDTH + STAR_HALF_SIZE;
    float centerY = topMostY - STAR_HALF_SIZE;

    this->starTex->BindTexture();

    int currStarIdx = 0;
    glColor4f(activeStarColour.R(), activeStarColour.G(), activeStarColour.B(), 1.0f);
    for (; currStarIdx < this->numStars; currStarIdx++) {
        this->DrawQuad(currentCenterX , centerY, STAR_SIZE);
        currentCenterX += STAR_SIZE + STAR_GAP;
    }
    glColor4f(inactiveStarColour.R(), inactiveStarColour.G(), inactiveStarColour.B(), 1.0f);
    for (; currStarIdx < GameLevel::MAX_STARS_PER_LEVEL; currStarIdx++) {
        this->DrawQuad(currentCenterX , centerY, STAR_SIZE);
        currentCenterX += STAR_SIZE + STAR_GAP;
    }
}

void PointsHUD::DrawQuad(float centerX, float centerY, float size) {
    glPushMatrix();
    glTranslatef(centerX, centerY, 0.0f);
    glScalef(size, size, 1.0f);
    GeometryMaker::GetInstance()->DrawQuad();
    glPopMatrix();
}

/**
 * Call this to reinitialize the points HUD to what you would expect it
 * to look like at the very start of a level.
 */
void PointsHUD::Reinitialize() {
    this->scoreAnimator.ClearLerp();
    this->scoreAnimator.SetInterpolantValue(0);
    this->ClearNotifications();

    this->numStars         = 0;
    this->currPtScore      = 0;
    this->multiplier->Reinitialize();
}

void PointsHUD::SetNumStars(int numStars) {
    assert(numStars >= 0);

    // TODO...
    this->numStars = numStars;
}

void PointsHUD::SetScore(long pointScore) {

    // This will animated and update the score as calls to Draw are made
    if (this->scoreAnimator.GetTargetValue() != pointScore) {
        this->scoreAnimator.SetLerp(0.25, pointScore);
    }
}

void PointsHUD::PostPointNotification(const std::string& name, int pointAmount) {
    // Figure out where the notification is going to appear and move to
    float finalTopY = this->ptScoreLabel->GetTopLeftCorner()[1] - this->ptScoreLabel->GetHeight();
    float initialTopY;
    
    // If there are any other notifications present then we place the new notification underneath the
    // current one
    if (this->ptNotifications.empty()) {
        initialTopY = this->ptScoreLabel->GetTopLeftCorner()[1] - static_cast<float>(this->ptScoreLabel->GetHeight()) -
                      PointsHUD::PointNotification::INITIAL_NOTIFY_VERTICAL_GAP;
    }
    else {
        initialTopY = this->ptNotifications.back()->GetTopLeftCorner()[1] - 
                      static_cast<float>(this->ptNotifications.back()->GetHeight()) - 
                      PointsHUD::PointNotification::NOTIFIER_TO_NOTIFIER_VERTICAL_GAP;
    }
    
    PointNotification* ptNotify = new PointNotification(name, pointAmount, initialTopY, finalTopY);
    this->ptNotifications.push_back(ptNotify);
}

void PointsHUD::ClearNotifications() {
    for (PointNotifyListIter iter = this->ptNotifications.begin(); iter != this->ptNotifications.end(); ++iter) {
        PointNotification* ptNotify = *iter;
        delete ptNotify;
        ptNotify = NULL;
    }
    this->ptNotifications.clear();
}

/**
 * Sets the current alpha (1 for completely visible, 0 for completely invisible) for the entire
 * points HUD.
 */
void PointsHUD::SetAlpha(float alpha) {
    this->ptScoreLabel->SetAlpha(alpha);
    this->multiplier->SetAlpha(alpha);
    for (PointNotifyListIter iter = this->ptNotifications.begin(); iter != this->ptNotifications.end();) {
        PointNotification* ptNotification = *iter;
        ptNotification->SetAlpha(alpha);
    }
}

const int PointsHUD::PointNotification::NOTIFIER_TO_NOTIFIER_VERTICAL_GAP = -2;
const int PointsHUD::PointNotification::INITIAL_NOTIFY_VERTICAL_GAP = 20;
const int PointsHUD::PointNotification::NAME_POINT_VERTICAL_GAP     = 5;

PointsHUD::PointNotification::PointNotification(const std::string& name, int pointAmount, 
                                                float initialTopY, float finalTopY) :
pointLabel(NULL), notificationName(NULL), pointAmount(pointAmount), moveAnimation(0.0f) {
    assert(pointAmount != 0);

    bool isPositive = (pointAmount > 0);
    std::string positiveSign("");
    Colour labelColour;
    if (isPositive) {
        labelColour = GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR;
        positiveSign = "+";
    }
    else {
        labelColour = GameViewConstants::GetInstance()->ITEM_BAD_COLOUR;
    }


    float topY = initialTopY;
    if (!name.empty()) {
        this->notificationName = new TextLabel2D(
            GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium), name);
        this->notificationName->SetDropShadow(Colour(0,0,0), 0.07f);
        this->notificationName->SetColour(labelColour);
        this->notificationName->SetTopLeftCorner(0, topY);
        topY -= (this->notificationName->GetHeight() + NAME_POINT_VERTICAL_GAP);
    }

    std::stringstream pointStr;
    pointStr << positiveSign << pointAmount;
    this->pointLabel = new TextLabel2D(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small),
        pointStr.str());
    this->pointLabel->SetDropShadow(Colour(0,0,0), 0.04f);
    this->pointLabel->SetColour(labelColour);
    this->pointLabel->SetTopLeftCorner(0, topY);

    this->moveAnimation.SetLerp(0.0, 2.0, initialTopY, finalTopY);
    this->moveAnimation.SetRepeat(false);

    std::vector<double> timeVals;
    timeVals.push_back(0.0);
    timeVals.push_back(0.3);
    timeVals.push_back(0.5);
    timeVals.push_back(2.0);
    std::vector<ColourRGBA> colourVals;
    colourVals.push_back(ColourRGBA(labelColour, 0.0f));
    colourVals.push_back(ColourRGBA(1, 1, 1, 1));
    colourVals.push_back(ColourRGBA(labelColour, 1.0f));
    colourVals.push_back(ColourRGBA(labelColour, 0.0f));

    this->rgbaAnimation.SetLerp(timeVals, colourVals);
    this->rgbaAnimation.SetRepeat(false);
}

PointsHUD::PointNotification::~PointNotification() {
    delete this->pointLabel;
    this->pointLabel = NULL;

    if (this->notificationName != NULL) {
        delete this->notificationName;
        this->notificationName = NULL;
    }
}

bool PointsHUD::PointNotification::Draw(int displayWidth, double dT) {
    bool isDone = this->moveAnimation.Tick(dT);
    isDone &= this->rgbaAnimation.Tick(dT);

    float currentY                  = this->moveAnimation.GetInterpolantValue();
    const ColourRGBA& currentColour = this->rgbaAnimation.GetInterpolantValue();

    if (this->notificationName != NULL) {

        this->notificationName->SetTopLeftCorner(
            displayWidth - this->notificationName->GetLastRasterWidth() - 
            PointsHUD::SCREEN_EDGE_HORIZONTAL_GAP, currentY);
        this->notificationName->SetColour(currentColour);
        this->notificationName->Draw();
        currentY -= (this->notificationName->GetHeight() + NAME_POINT_VERTICAL_GAP);

    }
    this->pointLabel->SetTopLeftCorner(displayWidth -
        this->pointLabel->GetLastRasterWidth() - PointsHUD::SCREEN_EDGE_HORIZONTAL_GAP, currentY);
    this->pointLabel->SetColour(currentColour);
    this->pointLabel->Draw();
    
    return isDone;
}

void PointsHUD::PointNotification::SetAlpha(float alpha) {
    this->pointLabel->SetAlpha(alpha);
    if (this->notificationName != NULL) {
        this->notificationName->SetAlpha(alpha);
    }
}

const Point2D& PointsHUD::PointNotification::GetTopLeftCorner() const {
    if (this->notificationName != NULL) {
        return this->notificationName->GetTopLeftCorner();
    }
    return this->pointLabel->GetTopLeftCorner();
}

float PointsHUD::PointNotification::GetHeight() const {
    float totalHeight = this->pointLabel->GetHeight();
    if (this->notificationName != NULL) {
        totalHeight += NAME_POINT_VERTICAL_GAP;
        totalHeight += this->notificationName->GetHeight();
    }
    return totalHeight;
}

// MULTIPLIER HUD FUNCTIONS ***********************************************

const float PointsHUD::MultiplierHUD::BANG_TEXT_BORDER = 5.0f;

PointsHUD::MultiplierHUD::MultiplierHUD() : 
ptMultiplierLabel(NULL), currPtMultiplier(1), multiplierBangTex(NULL), 
size(0), currState(None), scaleAnim(0.0f) {

    ColourRGBA multiplierLabelColour(1.0f, 0.4f, 0.0f, 0.0f);
    this->ptMultiplierLabel = new TextLabel2D(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Big), "");
    this->ptMultiplierLabel->SetColour(multiplierLabelColour);
    this->ptMultiplierLabel->SetDropShadow(Colour(0,0,0), 0.09f);

    this->multiplierBangTex = ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_MULTIPLIER_BANG,
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->multiplierBangTex != NULL);

    this->scaleAnim.SetInterpolantValue(0.0f);
    this->scaleAnim.SetRepeat(false);

    this->rgbaLabelAnim.SetInterpolantValue(multiplierLabelColour);
    this->rgbaLabelAnim.SetRepeat(false);
}

PointsHUD::MultiplierHUD::~MultiplierHUD() {
    delete this->ptMultiplierLabel;
    this->ptMultiplierLabel = NULL;

    bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->multiplierBangTex);
    assert(success);
}

void PointsHUD::MultiplierHUD::Reinitialize() {
    // Clean up all animations...

    this->currPtMultiplier = 1;
}

void PointsHUD::MultiplierHUD::SetAlpha(float alpha) {
    this->ptMultiplierLabel->SetAlpha(alpha);
}

void PointsHUD::MultiplierHUD::SetMultiplier(int multiplierAmt) {
    // Ignore cases where the multiplier is already the same
    if (this->currPtMultiplier == multiplierAmt || multiplierAmt < 1) {
        return;
    }

    int oldMultiplierAmt = this->currPtMultiplier;
    // Change the multiplier amount...
    this->currPtMultiplier = multiplierAmt;

    bool wasMultiplierVisibleBefore = (oldMultiplierAmt >= 2);
    bool isMultiplierVisibleNow     = (multiplierAmt >= 2);

    // Change the label if need be...
    if (isMultiplierVisibleNow) {
        std::stringstream ptMultString;
        ptMultString << multiplierAmt << "x";
        this->ptMultiplierLabel->SetText(ptMultString.str());
        this->ptMultiplierLabel->SetScale(MultiplierHUD::GetMultiplierScale(multiplierAmt));
        this->size = std::max<float>(this->ptMultiplierLabel->GetHeight(), this->ptMultiplierLabel->GetLastRasterWidth()) + 2 * BANG_TEXT_BORDER;

        if (!wasMultiplierVisibleBefore) {
            // Enter the creation state if the multiplier wasn't visible before
            this->SetCurrentAnimationState(CreationAnimation);
        }
        else {
            // In this case the multiplier has either grown or shrunk - for now we only allow
            // growing multipliers for now...
            assert(oldMultiplierAmt < multiplierAmt);
            this->SetCurrentAnimationState(IncreaseAnimation);
        }
    }
    else {
        // The multiplier is no longer visible - in such a case IT MUST HAVE BEEN VISIBLE BEFORE
        assert(wasMultiplierVisibleBefore);
        assert(multiplierAmt == 1);
        // The animation for the multiplier going away is set
        this->SetCurrentAnimationState(LeaveAnimation);
    }
}

void PointsHUD::MultiplierHUD::Draw(float rightMostX, float topMostY, double dT) {

    switch (this->currState) {
        case None:
            // Don't draw the multiplier if the current state is 'None'
            assert(this->currPtMultiplier < 2);
            return;

        case CreationAnimation:
            {
                bool done = this->scaleAnim.Tick(dT);
                done &= this->rgbaLabelAnim.Tick(dT);
                this->DrawHUD(rightMostX, topMostY);
                if (done) {
                    this->SetCurrentAnimationState(IdleAnimation);
                }   
            }
            break;

        case IncreaseAnimation:
            {
                bool done = this->scaleAnim.Tick(dT);
                done &= this->rgbaLabelAnim.Tick(dT);
                this->DrawHUD(rightMostX, topMostY);
                if (done) {
                    this->SetCurrentAnimationState(IdleAnimation);
                }
            }
            break;

        case IdleAnimation:
            this->rgbaLabelAnim.Tick(dT);
            this->DrawHUD(rightMostX, topMostY);
            break;

        case LeaveAnimation:
            {
                bool done = this->scaleAnim.Tick(dT);
                done &= this->rgbaLabelAnim.Tick(dT);
                this->DrawHUD(rightMostX, topMostY);
                if (done) {
                    this->SetCurrentAnimationState(None);
                }
            }
            break;

        default:
            assert(false);
            return;
    }
}

void PointsHUD::MultiplierHUD::DrawHUD(float rightMostX, float topMostY) {
    static const float MAX_SIZE = 70;

    float scaledSize = this->scaleAnim.GetInterpolantValue() * this->size;
    const ColourRGBA& labelColour = this->rgbaLabelAnim.GetInterpolantValue();
    
    this->ptMultiplierLabel->SetTopLeftCorner((rightMostX - MAX_SIZE) + 
        (MAX_SIZE - this->ptMultiplierLabel->GetLastRasterWidth()) / 2.0f, 
        topMostY - (MAX_SIZE - this->ptMultiplierLabel->GetHeight())/2.0f);

    float centerX = rightMostX - MAX_SIZE / 2.0f;
    float centerY = topMostY   - MAX_SIZE / 2.0f;

    // Draw the bang behind the multiplier label...
    this->multiplierBangTex->BindTexture();
    glColor4f(1,1,1,1);
    glPushMatrix();
    glTranslatef(centerX, centerY, 0.0f);
    glScalef(scaledSize, scaledSize, 1.0f);
    GeometryMaker::GetInstance()->DrawQuad();
    glPopMatrix();

    // Draw the multiplier text label...
    this->ptMultiplierLabel->SetColour(labelColour);
    this->ptMultiplierLabel->Draw();
}

void PointsHUD::MultiplierHUD::SetCurrentAnimationState(const AnimationState& state) {
    switch (state) {
        case None:
            assert(this->currPtMultiplier < 2);
            break;

        case CreationAnimation:
            {
                // Creating the multiplier - the size will increase a lot then go down to a normal size
                std::vector<double> timeVals;
                std::vector<float> scaleVals;

                timeVals.push_back(0.0);
                timeVals.push_back(0.8);
                timeVals.push_back(1.2);
                
                scaleVals.push_back(this->scaleAnim.GetInterpolantValue());
                scaleVals.push_back(1.2f);
                scaleVals.push_back(1.0f);

                this->scaleAnim.SetLerp(timeVals, scaleVals);

                // The colour of the label will shine once and go to its typical colour for the current
                // multiplier value
                std::vector<ColourRGBA> colourVals;
                colourVals.push_back(this->rgbaLabelAnim.GetInterpolantValue());
                colourVals.push_back(ColourRGBA(1,1,1,1));
                colourVals.push_back(ColourRGBA(MultiplierHUD::GetMultiplierColour(this->currPtMultiplier), 1));

                this->rgbaLabelAnim.SetLerp(timeVals, colourVals);
                this->rgbaLabelAnim.SetRepeat(false);

            }   
            break;

        case IncreaseAnimation:
            {
                // Creating the multiplier - the size will increase a lot then go down to a normal size
                std::vector<double> timeVals;
                std::vector<float> scaleVals;

                timeVals.push_back(0.0);
                timeVals.push_back(0.5);
                timeVals.push_back(0.8);
                
                scaleVals.push_back(this->scaleAnim.GetInterpolantValue());
                scaleVals.push_back(1.3f);
                scaleVals.push_back(1.0f);

                this->scaleAnim.SetLerp(timeVals, scaleVals);

                // The colour of the label will shine once and go to its typical colour for the current
                // multiplier value
                std::vector<ColourRGBA> colourVals;
                colourVals.push_back(this->rgbaLabelAnim.GetInterpolantValue());
                colourVals.push_back(ColourRGBA(1,1,1,1));
                colourVals.push_back(ColourRGBA(MultiplierHUD::GetMultiplierColour(this->currPtMultiplier), 1));

                this->rgbaLabelAnim.SetLerp(timeVals, colourVals);
                this->rgbaLabelAnim.SetRepeat(false);
            }
            break;

        case IdleAnimation:
            {
                // No scale changes while idleing
                this->scaleAnim.ClearLerp();
                this->scaleAnim.SetInterpolantValue(1.0f);
                
                std::vector<double> timeVals;
                timeVals.reserve(5);
                std::vector<ColourRGBA> colourVals;
                colourVals.reserve(5);

                timeVals.push_back(0.0);
                timeVals.push_back(1.0);
                timeVals.push_back(1.5);
                timeVals.push_back(2.0);
                timeVals.push_back(3.0);
                
                colourVals.push_back(ColourRGBA(MultiplierHUD::GetMultiplierColour(this->currPtMultiplier), 1));
                colourVals.push_back(ColourRGBA(MultiplierHUD::GetMultiplierColour(this->currPtMultiplier), 1));
                colourVals.push_back(ColourRGBA(1,1,1,1));
                colourVals.push_back(ColourRGBA(MultiplierHUD::GetMultiplierColour(this->currPtMultiplier), 1));
                colourVals.push_back(ColourRGBA(MultiplierHUD::GetMultiplierColour(this->currPtMultiplier), 1));
                
                this->rgbaLabelAnim.SetLerp(timeVals, colourVals);
                this->rgbaLabelAnim.SetRepeat(true);
            }
            break;

        case LeaveAnimation:
            {
                // The scale will shrink back to zero as the multiplier disappears
                std::vector<double> timeVals;
                std::vector<float> scaleVals;

                timeVals.push_back(0.0);
                timeVals.push_back(0.5);
                timeVals.push_back(0.8);
                
                scaleVals.push_back(this->scaleAnim.GetInterpolantValue());
                scaleVals.push_back(1.1f);
                scaleVals.push_back(0.0f);
                
                this->scaleAnim.SetLerp(timeVals, scaleVals);

                // The colour of the label will fade out
                this->rgbaLabelAnim.SetLerp(0.7, ColourRGBA(this->rgbaLabelAnim.GetInterpolantValue().GetColour(),0));
                this->rgbaLabelAnim.SetRepeat(false);
            }
            break;

        default:
            assert(false);
            return;
    }

    this->currState = state;
}

Colour PointsHUD::MultiplierHUD::GetMultiplierColour(int multiplier) {
    switch (multiplier) {
        case 1:
            break;
        case 2:
            return GameViewConstants::GetInstance()->TWO_TIMES_MULTIPLIER_COLOUR;
        case 3:
            return GameViewConstants::GetInstance()->THREE_TIMES_MULTIPLIER_COLOUR;
        case 4:
            return GameViewConstants::GetInstance()->FOUR_TIMES_MULTIPLIER_COLOUR;
        default:
            assert(false);
            break;
    }
    return Colour(0,0,0);
}

float PointsHUD::MultiplierHUD::GetMultiplierScale(int multiplier) {
    switch (multiplier) {
        case 1:
            break;
        case 2:
            return 1.0f;
        case 3:
            return 1.1f;
        case 4:
            return 1.2f;
        default:
            assert(false);
            break;
    }
    return 0.0f;
}
