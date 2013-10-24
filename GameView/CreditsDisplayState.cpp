
/**
 * CreditsDisplayState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "CreditsDisplayState.h"
#include "GameFontAssetsManager.h"
#include "GameDisplay.h"
#include "MainMenuDisplayState.h"

#include "../ResourceManager.h"

const char* CreditsDisplayState::CREDITS_1_TITLE_TEXT  = "Designed, Created, Programmed, and Directed By";
const char* CreditsDisplayState::CREDITS_1_NAMES_TEXT  = "Callum Hay";
const char* CreditsDisplayState::CREDITS_2_TITLE_TEXT  = "Music By";
const char* CreditsDisplayState::CREDITS_2_NAMES_TEXT  = "Adam Axbey";

const char* CreditsDisplayState::SPECIAL_THANKS_TITLE_TEXT = "Special Thanks To";
const char* CreditsDisplayState::SPECIAL_THANKS_NAMES_TEXT = "Sara Vinten\nMikhail St-Denis\nBlake Withers\nAndre Lowy";

CreditsDisplayState::CreditsDisplayState(GameDisplay* display) : DisplayState(display), starryBG(NULL),
creditLabel1(CREDITS_1_TITLE_TEXT, CREDITS_1_NAMES_TEXT, Camera::GetWindowWidth()*0.75f), 
creditLabel2(CREDITS_2_TITLE_TEXT, CREDITS_2_NAMES_TEXT, Camera::GetWindowWidth()*0.75f),
specialThanksLabel(SPECIAL_THANKS_TITLE_TEXT, SPECIAL_THANKS_NAMES_TEXT, Camera::GetWindowWidth()*0.75f),
exitState(false), bbbLogoTex(NULL) {
    
    this->starryBG = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_STARFIELD, Texture::Trilinear));
    assert(this->starryBG != NULL);

    this->bbbLogoTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_BBB_LOGO, Texture::Trilinear));
    assert(this->bbbLogoTex != NULL);

    static const double TIME_BETWEEN_LABEL_FADEINS = 1.0;
    double lastFadeInTime = 0.0;
    this->logoFadeinAnim.SetLerp(lastFadeInTime, lastFadeInTime + TIME_BETWEEN_LABEL_FADEINS, 0.0f, 1.0f);
    lastFadeInTime = this->logoFadeinAnim.GetFinalTime();
    this->fadeInCreditLabel1Anim.SetLerp(lastFadeInTime, lastFadeInTime + TIME_BETWEEN_LABEL_FADEINS, 0.0f, 1.0f);
    lastFadeInTime = this->fadeInCreditLabel1Anim.GetFinalTime();
    this->fadeInCreditLabel2Anim.SetLerp(lastFadeInTime, lastFadeInTime + TIME_BETWEEN_LABEL_FADEINS, 0.0f, 1.0f);
    lastFadeInTime= this->fadeInCreditLabel2Anim.GetFinalTime();
    this->fadeInSpecialThanksAnim.SetLerp(lastFadeInTime, lastFadeInTime + TIME_BETWEEN_LABEL_FADEINS, 0.0f, 1.0f);

    this->fadeInCreditLabel1Anim.SetRepeat(false);
    this->fadeInCreditLabel2Anim.SetRepeat(false);
    this->fadeInSpecialThanksAnim.SetRepeat(false);
    this->logoFadeinAnim.SetRepeat(false);

    this->fadeoutAnim.SetLerp(0.0, 1.0, 1.0f, 0.0f);
    this->fadeoutAnim.SetRepeat(false);

    float scalingFactor    = GameDisplay::GetTextScalingFactor();
    float gapBetweenLabels = GAP_BETWEEN_CREDITS_LABELS * scalingFactor;
    float extraTopGap = 50 * scalingFactor;

    float verticalGap = (Camera::GetWindowHeight() - this->creditLabel1.GetHeight() - 
        this->creditLabel2.GetHeight() - this->specialThanksLabel.GetHeight() - 2*gapBetweenLabels) / 2.0f;

    float currYPos = Camera::GetWindowHeight() - verticalGap - extraTopGap;
    this->creditLabel1.SetYPos(currYPos);
    currYPos -= (gapBetweenLabels + this->creditLabel1.GetHeight());
    this->creditLabel2.SetYPos(currYPos);
    currYPos -= (gapBetweenLabels + this->creditLabel2.GetHeight());
    this->specialThanksLabel.SetYPos(currYPos);
}

CreditsDisplayState::~CreditsDisplayState() {
    bool success = false;
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->starryBG);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->bbbLogoTex);
    assert(success);
    UNUSED_VARIABLE(success);
}

void CreditsDisplayState::RenderFrame(double dT) {

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw the starry background...
    this->starryBG->BindTexture();
    GeometryMaker::GetInstance()->DrawTiledFullScreenQuad(Camera::GetWindowWidth(), Camera::GetWindowHeight(), 
        GameViewConstants::STARRY_BG_TILE_MULTIPLIER * static_cast<float>(Camera::GetWindowWidth()) / static_cast<float>(this->starryBG->GetWidth()),
        GameViewConstants::STARRY_BG_TILE_MULTIPLIER * static_cast<float>(Camera::GetWindowHeight()) / static_cast<float>(this->starryBG->GetHeight()));
    this->starryBG->UnbindTexture();

    float credit1LabelAlpha = 0.0f;
    float credit2LabelAlpha = 0.0f;
    float specialCreditLabelAlpha = 0.0f;
    float logoAlpha = 0.0f;

    bool fadeOutIsFinished = false;

    if (this->exitState) {
        fadeOutIsFinished = this->fadeoutAnim.Tick(dT);

        credit1LabelAlpha = this->fadeoutAnim.GetInterpolantValue();
        credit2LabelAlpha = this->fadeoutAnim.GetInterpolantValue();
        specialCreditLabelAlpha = this->fadeoutAnim.GetInterpolantValue();
        logoAlpha = this->fadeoutAnim.GetInterpolantValue();
    }
    else {
        this->fadeInCreditLabel1Anim.Tick(dT);
        this->fadeInCreditLabel2Anim.Tick(dT);
        this->fadeInSpecialThanksAnim.Tick(dT);
        this->logoFadeinAnim.Tick(dT);

        credit1LabelAlpha = this->fadeInCreditLabel1Anim.GetInterpolantValue();
        credit2LabelAlpha = this->fadeInCreditLabel2Anim.GetInterpolantValue();
        specialCreditLabelAlpha = this->fadeInSpecialThanksAnim.GetInterpolantValue();
        logoAlpha = this->logoFadeinAnim.GetInterpolantValue();
    }

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_LINE_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Camera::PushWindowCoords();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    float scaleFactor = GameDisplay::GetTextScalingFactor();
    const float VERT_DIST_FROM_EDGE = 20 * scaleFactor;
    
    float logoXSize = 512.0f * scaleFactor;
    float logoYSize = 256.0f * scaleFactor;
    float logoXSizeDiv2 = logoXSize / 2.0f;
    float logoYSizeDiv2 = logoYSize / 2.0f;

    glTranslatef((Camera::GetWindowWidth() - logoXSize) / 2.0f + logoXSizeDiv2, Camera::GetWindowHeight() - logoYSizeDiv2 - VERT_DIST_FROM_EDGE, 0);
    glScalef(logoXSize, logoYSize, 1.0f);
    
    glColor4f(1,1,1,logoAlpha);
    this->bbbLogoTex->BindTexture();
    GeometryMaker::GetInstance()->DrawQuad();
    this->bbbLogoTex->UnbindTexture();

    glPopMatrix();
    Camera::PopWindowCoords();

    glPopAttrib();

    this->creditLabel1.SetAlpha(credit1LabelAlpha);
    this->creditLabel2.SetAlpha(credit2LabelAlpha);
    this->specialThanksLabel.SetAlpha(specialCreditLabelAlpha);

    this->creditLabel1.Draw();
    this->creditLabel2.Draw();
    this->specialThanksLabel.Draw();

    if (fadeOutIsFinished) {
        this->display->SetCurrentState(new MainMenuDisplayState(this->display));
        return;
    }
}

void CreditsDisplayState::ButtonPressed(const GameControl::ActionButton& pressedButton,
                                        const GameControl::ActionMagnitude& magnitude) {
    UNUSED_PARAMETER(pressedButton);
    UNUSED_PARAMETER(magnitude);

    if (this->fadeInSpecialThanksAnim.GetInterpolantValue() == this->fadeInSpecialThanksAnim.GetTargetValue()) {
        this->exitState = true;
    }
}

CreditsDisplayState::CreditLabel::CreditLabel(const char* title, const char* names, float maxWidth) :
titleLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), maxWidth, title),
namesLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), maxWidth, names) {

    this->titleLabel.SetAlignment(TextLabel2DFixedWidth::CenterAligned);
    this->titleLabel.SetScale(GameDisplay::GetTextScalingFactor() * 1.10f, false);
    this->titleLabel.SetColour(Colour(0.85f, 0.85f, 0.85f));

    this->namesLabel.SetAlignment(TextLabel2DFixedWidth::CenterAligned);
    this->namesLabel.SetScale(GameDisplay::GetTextScalingFactor(), false);
    this->namesLabel.SetColour(Colour(1,1,1));
}

CreditsDisplayState::CreditLabel::~CreditLabel() {
}

void CreditsDisplayState::CreditLabel::SetYPos(float y) {
    this->titleLabel.SetTopLeftCorner((Camera::GetWindowWidth() - this->titleLabel.GetWidth()) / 2.0f, y);
    this->namesLabel.SetTopLeftCorner((Camera::GetWindowWidth() - this->namesLabel.GetWidth()) / 2.0f, y - this->titleLabel.GetHeight() - this->GetTitleToNameGap());
}

void CreditsDisplayState::CreditLabel::SetAlpha(float alpha) {
    this->titleLabel.SetAlpha(alpha);
    this->namesLabel.SetAlpha(alpha);
}

void CreditsDisplayState::CreditLabel::Draw() {
    this->titleLabel.Draw();
    this->namesLabel.Draw();
}

float CreditsDisplayState::CreditLabel::GetTitleToNameGap() const {
    return GameDisplay::GetTextScalingFactor() * TITLE_NAMES_V_GAP;
}

float CreditsDisplayState::CreditLabel::GetHeight() const {
    return this->titleLabel.GetHeight() + this->GetTitleToNameGap() + this->namesLabel.GetHeight();
}