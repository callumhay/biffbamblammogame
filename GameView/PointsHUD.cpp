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

PointsHUD::PointsHUD() : numStars(0), currPtScore(0), currPtMultiplier(1), 
ptScoreLabel(NULL), ptMultiplierLabel(NULL), starTex(NULL), multiplierBangTex(NULL) {

    this->ptScoreLabel = new TextLabel2D(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium), "0");
    this->ptScoreLabel->SetColour(Colour(1.0f, 0.4f, 0.0f));
    this->ptScoreLabel->SetDropShadow(Colour(1,1,1), 0.09f);
    this->ptScoreLabel->SetScale(0.85f);

    this->ptMultiplierLabel = new TextLabel2D(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Big), "");
    this->ptMultiplierLabel->SetColour(Colour(1.0f, 0.4f, 0.0f));
    this->ptMultiplierLabel->SetDropShadow(Colour(0,0,0), 0.09f);
    
    this->scoreAnimator = AnimationLerp<long>(&this->currPtScore);
    this->scoreAnimator.SetRepeat(false);

    this->starTex = ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_STAR,
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->starTex != NULL);
    this->multiplierBangTex = ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_MULTIPLIER_BANG,
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->multiplierBangTex != NULL);
}

PointsHUD::~PointsHUD() {
    delete this->ptScoreLabel;
    this->ptScoreLabel = NULL;
    delete this->ptMultiplierLabel;
    this->ptMultiplierLabel = NULL;

    // Clean up any leftover notifications
    this->ClearNotifications();

    // Release textures
    bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->starTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->multiplierBangTex);
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
    if (this->currPtMultiplier > 1) {
        this->DrawMultiplier(displayWidth - SCREEN_EDGE_HORIZONTAL_GAP - ALL_STARS_WIDTH - 
                             SCORE_TO_MULTIPLER_HORIZONTAL_GAP, currentY + STAR_SIZE/2);
    }

    Camera::PopWindowCoords();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    // Draw any point notifications
    for (PointNotifyListIter iter = this->ptNotifications.begin(); iter != this->ptNotifications.end();) {
        PointNotification* ptNotification = *iter;
        if (ptNotification->Draw(dT)) {
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

void PointsHUD::DrawMultiplier(float rightMostX, float topMostY) {
    static const float BANG_TEXT_BORDER = 5.0f;

    std::stringstream ptMultString;
    ptMultString << this->currPtMultiplier << "x";

    this->ptMultiplierLabel->SetText(ptMultString.str());
    float size = std::max<float>(this->ptMultiplierLabel->GetHeight(), this->ptMultiplierLabel->GetLastRasterWidth()) + 2 * BANG_TEXT_BORDER;
    this->ptMultiplierLabel->SetTopLeftCorner((rightMostX - size) + (size - this->ptMultiplierLabel->GetLastRasterWidth()) / 2.0f, 
                                               topMostY - (size - this->ptMultiplierLabel->GetHeight())/2.0f);

    float centerX = rightMostX - size / 2.0f;
    float centerY = topMostY   - size / 2.0f;

    // Draw the bang behind the multiplier label...
    this->multiplierBangTex->BindTexture();
    glColor4f(1,1,1,1);
    glPushMatrix();
    glTranslatef(centerX, centerY, 0.0f);
    glScalef(size, size, 1.0f);
    GeometryMaker::GetInstance()->DrawQuad();
    glPopMatrix();

    // Draw the multiplier text label...
    this->ptMultiplierLabel->Draw();
}

void PointsHUD::DrawIdleStars(float rightMostX, float topMostY, double dT) {
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
    this->currPtMultiplier = 1;
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
    PointNotification* ptNotify = new PointNotification(name, pointAmount);
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
    this->ptMultiplierLabel->SetAlpha(alpha);
    for (PointNotifyListIter iter = this->ptNotifications.begin(); iter != this->ptNotifications.end();) {
        PointNotification* ptNotification = *iter;
        ptNotification->SetAlpha(alpha);
    }
}

PointsHUD::PointNotification::PointNotification(const std::string& name, int pointAmount) :
pointLabel(NULL), notificationName(NULL), pointAmount(pointAmount) {
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

    std::stringstream pointStr;
    pointStr << positiveSign << pointAmount;
    this->pointLabel = new TextLabel2D(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small),
        pointStr.str());
    this->pointLabel->SetDropShadow(Colour(0,0,0), 0.05f);
    this->pointLabel->SetColour(labelColour);

    if (!name.empty()) {
        this->notificationName = new TextLabel2D(
            GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium), name);
        this->notificationName->SetDropShadow(Colour(0,0,0), 0.08f);
        this->notificationName->SetColour(labelColour);
    }
}

PointsHUD::PointNotification::~PointNotification() {
    delete this->pointLabel;
    this->pointLabel = NULL;

    if (this->notificationName != NULL) {
        delete this->notificationName;
        this->notificationName = NULL;
    }
}

bool PointsHUD::PointNotification::Draw(double dT) {
    
    if (this->notificationName != NULL) {
    }

    
    return false;
}

void PointsHUD::PointNotification::SetAlpha(float alpha) {
    this->pointLabel->SetAlpha(alpha);
    if (this->notificationName != NULL) {
        this->notificationName->SetAlpha(alpha);
    }
}