/**
 * MalfunctionTextHUD.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#include "MalfunctionTextHUD.h"
#include "PointsHUD.h"
#include "GameFontAssetsManager.h"
#include "GameViewConstants.h"
#include "GameDisplay.h"

#include "../BlammoEngine/Texture2D.h"
#include "../GameSound/GameSound.h"
#include "../ResourceManager.h"

const float MalfunctionTextHUD::ICON_TEXT_X_GAP = 5.0f;
const double MalfunctionTextHUD::TIME_UNTIL_BOOST_MALFUNCTON_DEACTIVATION = 4.0;
const float MalfunctionTextHUD::STATIC_SCALE = 0.8f;

MalfunctionTextHUD::MalfunctionTextHUD(const char* text, GameSound* sound) : sound(sound),
malfunctionIconTex(NULL), isActive(false), malfunctionText(text),
timeSinceLastBoostMalfunction(TIME_UNTIL_BOOST_MALFUNCTON_DEACTIVATION),
malfunctionLabel(GameFontAssetsManager::GetInstance()->GetFont(
                 GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Big), 
                 600.0f, text) {

    assert(sound != NULL);

    this->malfunctionIconTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_BOOST_MALFUNCTION,  Texture::Trilinear, GL_TEXTURE_2D));
    assert(this->malfunctionIconTex != NULL);

    this->malfunctionLabel.SetScale(STATIC_SCALE * GameDisplay::GetTextScalingFactor(), this->malfunctionText);
    this->fullTextWidth  = this->malfunctionLabel.GetWidth();
    this->fullTextHeight = this->malfunctionLabel.GetHeight();

    std::vector<double> timeVals;
    timeVals.reserve(3);
    timeVals.push_back(0.0);
    timeVals.push_back(1.0);
    timeVals.push_back(2.0);

    std::vector<ColourRGBA> colourVals;
    colourVals.reserve(timeVals.size());
    colourVals.push_back(ColourRGBA(1.0f, 0.0f, 0.0f, 1.0f));
    colourVals.push_back(ColourRGBA(1.0f, 1.0f, 1.0f, 1.0f));
    colourVals.push_back(ColourRGBA(1.0f, 0.0f, 0.0f, 1.0f));

    this->flashAnim.SetLerp(timeVals, colourVals);
    this->flashAnim.SetRepeat(true);
    this->flashAnim.SetInterpolantValue(colourVals.front());

    this->Reset();
}

MalfunctionTextHUD::~MalfunctionTextHUD() {
    bool success = false;
    
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->malfunctionIconTex);
    assert(success);

    UNUSED_VARIABLE(success);
}

void MalfunctionTextHUD::Draw(double dT, const Camera& camera) {
    UNUSED_PARAMETER(camera);

    static const float Y_DIST_FROM_TOP = 150;

    float currAlpha = this->fadeAnim.GetInterpolantValue();
    const Colour& flashColour = this->flashAnim.GetInterpolantValue().GetColour();

    this->fadeAnim.Tick(dT);

    if (currAlpha <= 0.0f) {
        return;
    }
    
    this->flashAnim.Tick(dT);
    this->iconScaleAnim.Tick(dT);
    this->textAnim.Tick(dT);

    float scalingFactor     = STATIC_SCALE * GameDisplay::GetTextScalingFactor();
    float finalIconSize     = scalingFactor * 128.0f;
    float halfFinalIconSize = finalIconSize / 2.0f;

    int textLength = static_cast<int>(this->textAnim.GetInterpolantValue());
    this->malfunctionLabel.SetText(this->malfunctionText.substr(0, textLength));
    this->malfunctionLabel.SetTopLeftCorner(
        Camera::GetWindowWidth() - this->fullTextWidth - 4*PointsHUD::SCREEN_EDGE_HORIZONTAL_GAP,
        Camera::GetWindowHeight() - Y_DIST_FROM_TOP);

    float iconYOffset = (finalIconSize - static_cast<float>(this->fullTextHeight)) / 2.0f;
    float iconXPos = this->malfunctionLabel.GetTopLeftCorner()[0] - MalfunctionTextHUD::ICON_TEXT_X_GAP*scalingFactor - halfFinalIconSize;
    float iconYPos = this->malfunctionLabel.GetTopLeftCorner()[1] - halfFinalIconSize + iconYOffset;

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Camera::PushWindowCoords();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor4f(flashColour.R(), flashColour.G(), flashColour.B(), currAlpha);
    glPushMatrix();
    glTranslatef(iconXPos, iconYPos, 0.0f);
    float iconScale = this->iconScaleAnim.GetInterpolantValue() * finalIconSize;
    glScalef(iconScale, iconScale, 1.0f);
    this->malfunctionIconTex->BindTexture();
    GeometryMaker::GetInstance()->DrawQuad();
    this->malfunctionIconTex->UnbindTexture();
    glPopMatrix();
        
    this->malfunctionLabel.SetColour(ColourRGBA(this->flashAnim.GetInterpolantValue().GetColour(), 
        currAlpha * this->flashAnim.GetInterpolantValue().A()));
    this->malfunctionLabel.Draw();

    glPopMatrix();
    Camera::PopWindowCoords();

    glPopAttrib();

    if (this->timeSinceLastBoostMalfunction >= TIME_UNTIL_BOOST_MALFUNCTON_DEACTIVATION) {
        this->Deactivate();
    }
    this->timeSinceLastBoostMalfunction += dT;
}

void MalfunctionTextHUD::Activate() {

    if (this->isActive) {
        this->timeSinceLastBoostMalfunction = 0.0;
        return;
    }

    this->fadeAnim.SetLerp(0.33, 1.0f);
    this->fadeAnim.SetRepeat(false);
    this->iconScaleAnim.SetLerp(0.66, 1.0f);
    this->iconScaleAnim.SetRepeat(false);
    this->textAnim.SetLerp(1.075, this->malfunctionText.size());
    this->textAnim.SetRepeat(false);
    this->isActive = true;
    this->timeSinceLastBoostMalfunction = 0.0;

    // Play the activation sound
    this->sound->PlaySound(GameSound::BoostMalfunctionPromptEvent, false, true, 1.0f);
}

void MalfunctionTextHUD::Deactivate() {

    if (!this->isActive) {
        return;
    }

    this->fadeAnim.SetLerp(0.33, 0.0f);
    this->fadeAnim.SetRepeat(false);
    this->iconScaleAnim.SetLerp(0.66, 0.01f);
    this->iconScaleAnim.SetRepeat(false);
    this->textAnim.SetLerp(0.2, 0);
    this->textAnim.SetRepeat(false);
    this->isActive = false;
    this->timeSinceLastBoostMalfunction = TIME_UNTIL_BOOST_MALFUNCTON_DEACTIVATION;
}

void MalfunctionTextHUD::Reset() {
    this->fadeAnim.ClearLerp();
    this->fadeAnim.SetInterpolantValue(0.0f);
    this->iconScaleAnim.ClearLerp();
    this->iconScaleAnim.SetInterpolantValue(0.01f);
    this->textAnim.ClearLerp();
    this->textAnim.SetInterpolantValue(0.0f);
    this->malfunctionLabel.SetText("");
    this->isActive = false;
    this->timeSinceLastBoostMalfunction = TIME_UNTIL_BOOST_MALFUNCTON_DEACTIVATION;
}
