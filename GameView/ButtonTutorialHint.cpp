/**
 * ButtonTutorialHint.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "ButtonTutorialHint.h"
#include "GameFontAssetsManager.h"
#include "GameTutorialAssets.h"
#include "TutorialHintListeners.h"

#include "../ResourceManager.h"
#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/GeometryMaker.h"
#include "../BlammoEngine/Camera.h"

const float ButtonTutorialHint::BUTTON_SCALE_MULTIPLIER = 2.0f;

#define ACTION_LABEL_DEFAULT_COLOUR Colour(0.3882f, 0.72157f, 1.0f)

ButtonTutorialHint::ButtonTutorialHint(const GameTutorialAssets* tutorialAssets,
                                       const std::string& action) : 
TutorialHint(), tutorialAssets(tutorialAssets),
actionLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom,
GameFontAssetsManager::Medium), ""),
orLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom,
GameFontAssetsManager::Medium), " or  "),
commaLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom,
GameFontAssetsManager::Medium), ","),
mouseLabel(NULL), flashAnim(NULL), alphaWhenShowing(1.0f) {

    assert(tutorialAssets != NULL);

    this->SetActionName(action);
    this->actionLabel.SetColour(ACTION_LABEL_DEFAULT_COLOUR);
    this->actionLabel.SetDropShadow(Colour(0,0,0), 0.10f);

    orLabel.SetScale(0.75f);
    orLabel.SetColour(ACTION_LABEL_DEFAULT_COLOUR);
    orLabel.SetDropShadow(Colour(0,0,0), 0.10f);
    commaLabel.SetColour(ACTION_LABEL_DEFAULT_COLOUR);
    commaLabel.SetDropShadow(Colour(0,0,0), 0.07f);

    this->fadeAnim.ClearLerp();
    this->fadeAnim.SetInterpolantValue(0.0f);
    this->fadeAnim.SetRepeat(false);
}

ButtonTutorialHint::~ButtonTutorialHint() {
    this->ClearKeyboardKeyLabels();
    this->ClearXBoxLabels();
    this->ClearMouseLabel();

    if (this->flashAnim != NULL) {
        delete this->flashAnim;
        this->flashAnim = NULL;
    }
}

void ButtonTutorialHint::ClearKeyboardKeyLabels() {
    for (std::vector<ButtonGlyphLabel*>::iterator iter = this->keyboardKeyLabels.begin();
         iter != this->keyboardKeyLabels.end(); ++iter) {
        ButtonGlyphLabel* label = *iter;
        delete label;
        label = NULL;
    }
    this->keyboardKeyLabels.clear();
}

void ButtonTutorialHint::ClearXBoxLabels() {
    for (std::vector<ButtonGlyphLabel*>::iterator iter = this->xboxLabels.begin();
         iter != this->xboxLabels.end(); ++iter) {
        ButtonGlyphLabel* label = *iter;
        delete label;
        label = NULL;
    }
    this->xboxLabels.clear();
}

void ButtonTutorialHint::ClearMouseLabel() {
    if (this->mouseLabel != NULL) {
        delete this->mouseLabel;
        this->mouseLabel = NULL;
    }
}

void ButtonTutorialHint::SetXBoxButton(GameViewConstants::XBoxButtonType buttonType,
                                       const std::string& buttonText, const Colour& buttonColour) {

    this->ClearXBoxLabels();
    const Texture* buttonTex = this->tutorialAssets->GetXBox360Texture(buttonType);
    assert(buttonTex != NULL);

    float buttonHeight = this->actionLabel.GetHeight() * BUTTON_SCALE_MULTIPLIER;
    ButtonGlyphLabel* xboxButtonLabel = new ButtonGlyphLabel(buttonTex, buttonText, buttonColour,
        buttonHeight, 0, 0);

    float offsetX, offsetY;
    GameViewConstants::GetInstance()->GetXBoxButtonLabelOffset(buttonType, xboxButtonLabel->GetWidth(),
        xboxButtonLabel->GetHeight(), xboxButtonLabel->GetLabelWidth(), xboxButtonLabel->GetLabelHeight(),
        offsetX, offsetY);
    xboxButtonLabel->SetDimensions(buttonHeight, offsetX, offsetY);
    
    this->xboxLabels.reserve(1);
    this->xboxLabels.push_back(xboxButtonLabel);
}

void ButtonTutorialHint::SetXBoxButtons(const std::list<GameViewConstants::XBoxButtonType>& buttonTypes,
                                        const std::list<std::string>& buttonTexts,
                                        const std::list<Colour>& buttonColours) {

    this->ClearXBoxLabels();
    float buttonHeight = this->actionLabel.GetHeight() * BUTTON_SCALE_MULTIPLIER;

    assert(buttonTypes.size() == buttonTexts.size());
    assert(buttonTypes.size() == buttonColours.size());

    std::list<GameViewConstants::XBoxButtonType>::const_iterator typeIter = buttonTypes.begin();
    std::list<std::string>::const_iterator textIter = buttonTexts.begin();
    std::list<Colour>::const_iterator colourIter = buttonColours.begin();
    
    this->xboxLabels.reserve(buttonTypes.size());
    float offsetX, offsetY;

    for (; typeIter != buttonTypes.end() && textIter != buttonTexts.end() && colourIter != buttonColours.end(); 
        ++typeIter, ++textIter, ++colourIter) {
        
        const Texture* buttonTex = this->tutorialAssets->GetXBox360Texture(*typeIter);
        assert(buttonTex != NULL);
        
        ButtonGlyphLabel* keyLabel = new ButtonGlyphLabel(buttonTex, *textIter, *colourIter,
            buttonHeight, 0, 0);
        GameViewConstants::GetInstance()->GetXBoxButtonLabelOffset(*typeIter, keyLabel->GetWidth(),
            keyLabel->GetHeight(), keyLabel->GetLabelWidth(), keyLabel->GetLabelHeight(), offsetX, offsetY);
        keyLabel->SetDimensions(buttonHeight, offsetX, offsetY);
        
        this->xboxLabels.push_back(keyLabel);
    }
}

void ButtonTutorialHint::SetKeyboardButton(GameViewConstants::KeyboardButtonType buttonType,
                                           const std::string& buttonText) {
#ifndef _XBOX360
    this->ClearKeyboardKeyLabels();
    float buttonHeight = this->actionLabel.GetHeight() * BUTTON_SCALE_MULTIPLIER;

    const Texture* buttonTex = this->tutorialAssets->GetKeyboardTexture(buttonType);
    assert(buttonTex != NULL);
    ButtonGlyphLabel* keyLabel = new ButtonGlyphLabel(buttonTex, buttonText, Colour(1,1,1),
        buttonHeight, 0, 0);
    float offsetX, offsetY;
    GameViewConstants::GetInstance()->GetKeyboardButtonLabelOffset(buttonType, buttonHeight,
        keyLabel->GetLabelWidth(), keyLabel->GetLabelHeight(), offsetX, offsetY);
    keyLabel->SetDimensions(buttonHeight, offsetX, offsetY);
    this->keyboardKeyLabels.reserve(1);
    this->keyboardKeyLabels.push_back(keyLabel);
#else
    UNUSED_PARAMETER(buttonType);
    UNUSED_PARAMETER(buttonText);
#endif
}

void ButtonTutorialHint::SetKeyboardButtons(const std::list<GameViewConstants::KeyboardButtonType>& buttonTypes,
                                            const std::list<std::string>& buttonTexts) {
#ifndef _XBOX360
    this->ClearKeyboardKeyLabels();
    float buttonHeight = this->actionLabel.GetHeight() * BUTTON_SCALE_MULTIPLIER;

    assert(buttonTypes.size() == buttonTexts.size());

    std::list<GameViewConstants::KeyboardButtonType>::const_iterator typeIter = buttonTypes.begin();
    std::list<std::string>::const_iterator textIter = buttonTexts.begin();
    this->keyboardKeyLabels.reserve(buttonTypes.size());
    float offsetX, offsetY;

    for (; typeIter != buttonTypes.end() && textIter != buttonTexts.end(); ++typeIter, ++textIter) {
        
        const Texture* buttonTex = this->tutorialAssets->GetKeyboardTexture(*typeIter);
        assert(buttonTex != NULL);
        
        ButtonGlyphLabel* keyLabel = new ButtonGlyphLabel(buttonTex, *textIter, Colour(1,1,1),
            buttonHeight, 0, 0);
        GameViewConstants::GetInstance()->GetKeyboardButtonLabelOffset(*typeIter, buttonHeight,
            keyLabel->GetLabelWidth(), keyLabel->GetLabelHeight(), offsetX, offsetY);
        keyLabel->SetDimensions(buttonHeight, offsetX, offsetY);
        
        this->keyboardKeyLabels.push_back(keyLabel);
    }
#else
    UNUSED_PARAMETER(buttonTypes);
    UNUSED_PARAMETER(buttonTexts);
#endif
}

void ButtonTutorialHint::SetMouseButton(GameViewConstants::MouseButtonType buttonType, const std::string& buttonText) {
#ifndef _XBOX360
    this->ClearMouseLabel();

    float buttonHeight = this->actionLabel.GetHeight() * BUTTON_SCALE_MULTIPLIER;

    const Texture* buttonTex = this->tutorialAssets->GetMouseTexture(buttonType);
    assert(buttonTex != NULL);
    ButtonGlyphLabel* label = new ButtonGlyphLabel(buttonTex, buttonText, Colour(1,1,1),
        buttonHeight, 0, 0);
    float offsetX, offsetY;
    GameViewConstants::GetInstance()->GetMouseButtonLabelOffset(buttonType, buttonHeight,
        label->GetLabelWidth(), label->GetLabelHeight(), offsetX, offsetY);
    label->SetDimensions(buttonHeight, offsetX, offsetY);
    this->mouseLabel = label;
#else
    UNUSED_PARAMETER(buttonType);
    UNUSED_PARAMETER(buttonText);
#endif
}

void ButtonTutorialHint::SetFlashing(bool on) {
    if (on) {
        if (this->flashAnim == NULL) {
            this->flashAnim = new AnimationMultiLerp<Colour>();

            std::vector<double> timeVals;
            timeVals.reserve(3);
            timeVals.push_back(0.0);
            timeVals.push_back(0.5);
            timeVals.push_back(1.0);
            std::vector<Colour> colourVals;
            colourVals.reserve(3);
            colourVals.push_back(Colour(0.0f, 0.6f, 0.9f));
            colourVals.push_back(Colour(1.0f, 0.8f, 0.0f));
            colourVals.push_back(Colour(0.0f, 0.6f, 0.9f));

            this->flashAnim->SetLerp(timeVals, colourVals);
            this->flashAnim->SetRepeat(true);
        }
    }
    else {
        if (this->flashAnim != NULL) {
            delete this->flashAnim;
            this->flashAnim = NULL;
        }
    }
}

float ButtonTutorialHint::GetHeight() const {
    return this->actionLabel.GetHeight() * BUTTON_SCALE_MULTIPLIER;
}

float ButtonTutorialHint::GetWidth() const {
    float width = this->actionLabel.GetLastRasterWidth();
    for (std::vector<ButtonGlyphLabel*>::const_iterator iter = this->xboxLabels.begin();
         iter != this->xboxLabels.end(); ++iter) {

        const ButtonGlyphLabel* buttonLabel = *iter;
        width += buttonLabel->GetWidth();
    }
    width += std::max<float>(0, (this->xboxLabels.size()-1)) * this->commaLabel.GetLastRasterWidth();

    if (!this->xboxLabels.empty() && !this->keyboardKeyLabels.empty()) {
        width += this->orLabel.GetLastRasterWidth();
    }

    for (std::vector<ButtonGlyphLabel*>::const_iterator iter = this->keyboardKeyLabels.begin();
         iter != this->keyboardKeyLabels.end(); ++iter) {
    
        const ButtonGlyphLabel* buttonLabel = *iter;
        width += buttonLabel->GetWidth();
    }
    width += std::max<float>(0, (static_cast<float>(this->keyboardKeyLabels.size()) - 1.0f)) * this->commaLabel.GetLastRasterWidth();

    if ((!this->xboxLabels.empty() || !this->keyboardKeyLabels.empty()) && this->mouseLabel != NULL) {
        width += this->orLabel.GetLastRasterWidth();
    }

    if (this->mouseLabel != NULL) {
        width += this->mouseLabel->GetWidth();
    }

    return width;
}

void ButtonTutorialHint::Show(double delayInSeconds, double fadeInTimeInSeconds) {
    if (this->isShown) { return; }

    this->fadeAnim.SetLerp(delayInSeconds, delayInSeconds+fadeInTimeInSeconds, this->fadeAnim.GetInterpolantValue(), this->alphaWhenShowing);
    this->fadeAnim.SetRepeat(false);

    if (this->listener != NULL) {
        this->listener->OnTutorialHintShown();
    }

    this->isShown = true;
}

void ButtonTutorialHint::Unshow(double delayInSeconds, double fadeOutTimeInSeconds, bool overridePrevUnshow) {
    if (!this->isShown && !overridePrevUnshow) { return; }

    this->fadeAnim.SetLerp(delayInSeconds, delayInSeconds+fadeOutTimeInSeconds, this->fadeAnim.GetInterpolantValue(), 0.0f);
    this->fadeAnim.SetRepeat(false);

    if (this->listener != NULL) {
        this->listener->OnTutorialHintUnshown();
    }

    this->isShown = false;
}

void ButtonTutorialHint::Tick(double dT) {
    this->fadeAnim.Tick(dT);
    if (this->flashAnim != NULL) {
        this->flashAnim->Tick(dT);
    }
}

void ButtonTutorialHint::Draw(const Camera& camera, bool drawWithDepth, float depth) {
    UNUSED_PARAMETER(camera);

    float alpha = this->fadeAnim.GetInterpolantValue();
    if (alpha <= 0) {
        return;
    }

    if (this->flashAnim != NULL) {
        this->actionLabel.SetColour(this->flashAnim->GetInterpolantValue());
    }
    else {
        this->actionLabel.SetColour(ACTION_LABEL_DEFAULT_COLOUR);
    }

    float scale = 1.0f;
    const Point2D& topLeftCorner = this->actionLabel.GetTopLeftCorner();
    float currX = topLeftCorner[0] + this->actionLabel.GetLastRasterWidth();
    const float actualCenterY = topLeftCorner[1] - this->actionLabel.GetHeight()/2.0f;

    // Draw the action label...
    this->actionLabel.SetAlpha(alpha);
    this->actionLabel.Draw(drawWithDepth, depth);

    float commaHeightDiv2 = this->commaLabel.GetHeight()/2.0f;
    float buttonLabelDiv2 = 0;
    this->commaLabel.SetAlpha(alpha);

    // Draw the XBox Button labels, if any exist
    size_t count = 0;
    for (std::vector<ButtonGlyphLabel*>::iterator iter = this->xboxLabels.begin();
         iter != this->xboxLabels.end(); ++iter) {

        ButtonGlyphLabel* buttonLabel = *iter;
        buttonLabelDiv2 = buttonLabel->GetWidth() / 2.0f;
        currX += buttonLabelDiv2;
        buttonLabel->Draw(currX, actualCenterY, scale, alpha, drawWithDepth, depth);
        currX += buttonLabelDiv2;

        if (count != this->xboxLabels.size()-1 && this->xboxLabels.size() > 1) {
            this->commaLabel.SetTopLeftCorner(currX, actualCenterY + commaHeightDiv2);
            this->commaLabel.Draw(drawWithDepth, depth);
            currX += this->commaLabel.GetLastRasterWidth();
        }
    }

    if (!this->xboxLabels.empty() && !this->keyboardKeyLabels.empty()) {
        // Draw an 'or' label next to the xbox button label...
        this->orLabel.SetTopLeftCorner(currX, actualCenterY + this->orLabel.GetHeight()/2.0f);
        this->orLabel.SetAlpha(alpha);
        this->orLabel.Draw(drawWithDepth, depth);
        currX += this->orLabel.GetLastRasterWidth();
    }

    // Draw the keyboard button label(s) if it/they exist...
    count = 0;
    for (std::vector<ButtonGlyphLabel*>::const_iterator iter = this->keyboardKeyLabels.begin();
         iter != this->keyboardKeyLabels.end(); ++iter, count++) {
    
        ButtonGlyphLabel* buttonLabel = *iter;
        buttonLabelDiv2 = buttonLabel->GetWidth() / 2.0f;
        currX += buttonLabelDiv2;
        buttonLabel->Draw(currX, actualCenterY, scale, alpha, drawWithDepth, depth);
        currX += buttonLabelDiv2;
        
        if (count != this->keyboardKeyLabels.size()-1 && this->keyboardKeyLabels.size() > 1) {
            this->commaLabel.SetTopLeftCorner(currX, actualCenterY + commaHeightDiv2);
            this->commaLabel.Draw(drawWithDepth, depth);
            currX += this->commaLabel.GetLastRasterWidth();
        }
    }

    if (this->mouseLabel != NULL) {
        if ((!this->xboxLabels.empty() || !this->keyboardKeyLabels.empty())) {
            // Draw an 'or' label...
            this->orLabel.SetTopLeftCorner(currX, actualCenterY + this->orLabel.GetHeight()/2.0f);
            this->orLabel.SetAlpha(alpha);
            this->orLabel.Draw(drawWithDepth, depth);
            currX += this->orLabel.GetLastRasterWidth();
        }
        currX += this->mouseLabel->GetWidth() / 2.0f;
        this->mouseLabel->Draw(currX, actualCenterY, scale, alpha, drawWithDepth, depth);
    }
}

ButtonTutorialHint::ButtonGlyphLabel::ButtonGlyphLabel(const Texture* buttonTexture, const std::string& buttonText,
                                                       const Colour& buttonColour, float height, float offsetX,
                                                       float offsetY) : 
buttonTexture(buttonTexture),
buttonLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom,
GameFontAssetsManager::Small), buttonText), buttonColour(buttonColour) {

    assert(buttonTexture != NULL);
    this->buttonLabel.SetColour(Colour(1,1,1));
    this->buttonLabel.SetDropShadow(Colour(0,0,0), 0.05f);
    this->buttonLabel.SetScale(0.9f);
    this->SetDimensions(height, offsetX, offsetY);
}

ButtonTutorialHint::ButtonGlyphLabel::~ButtonGlyphLabel() {
}

void ButtonTutorialHint::ButtonGlyphLabel::SetDimensions(float height, float offsetX, float offsetY) {
    this->height = height;
    this->labelOffsetX = offsetX;
    this->labelOffsetY = offsetY;
}

void ButtonTutorialHint::ButtonGlyphLabel::Draw(float centerX, float centerY, float scale, float alpha,
                                                bool drawWithDepth, float depth) {

    float widthHeightRatio = static_cast<float>(this->buttonTexture->GetWidth()) / static_cast<float>(this->buttonTexture->GetHeight());

    glPushMatrix();
    glTranslatef(centerX, centerY, depth);
    glScalef(widthHeightRatio*scale*this->height, scale*this->height, 1.0f);

    glColor4f(this->buttonColour.R(), this->buttonColour.G(), this->buttonColour.B(), alpha);
    this->buttonTexture->BindTexture();
    GeometryMaker::GetInstance()->DrawQuad();
    this->buttonTexture->UnbindTexture();

    glPopMatrix();

    float topLeftX = centerX - this->height/2;
    float topLeftY = centerY + this->height/2;

    this->buttonLabel.SetTopLeftCorner(topLeftX + this->labelOffsetX, topLeftY + this->labelOffsetY);
    this->buttonLabel.SetAlpha(alpha);
    this->buttonLabel.Draw(drawWithDepth, depth);
}