/**
 * ButtonTutorialHint.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "ButtonTutorialHint.h"
#include "GameFontAssetsManager.h"
#include "GameTutorialAssets.h"

#include "../ResourceManager.h"
#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/GeometryMaker.h"

const float ButtonTutorialHint::BUTTON_SCALE_MULTIPLIER = 2.0f;

ButtonTutorialHint::ButtonTutorialHint(const GameTutorialAssets* tutorialAssets,
                                       const std::string& action) : 
TutorialHint(), tutorialAssets(tutorialAssets),
actionLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom,
GameFontAssetsManager::Medium), ""),
orLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom,
GameFontAssetsManager::Medium), " or  "),
commaLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom,
GameFontAssetsManager::Medium), ","),
xboxButtonLabel(NULL) {

    assert(tutorialAssets != NULL);

    this->SetActionName(action);
    this->actionLabel.SetColour(Colour(0.3882f, 0.72157f, 1.0f));
    this->actionLabel.SetDropShadow(Colour(0,0,0), 0.10f);

    orLabel.SetScale(0.75f);
    orLabel.SetColour(Colour(0.3882f, 0.72157f, 1.0f));
    orLabel.SetDropShadow(Colour(0,0,0), 0.10f);
    commaLabel.SetColour(Colour(0.3882f, 0.72157f, 1.0f));
    commaLabel.SetDropShadow(Colour(0,0,0), 0.07f);
}

ButtonTutorialHint::~ButtonTutorialHint() {
    if (this->xboxButtonLabel != NULL) {
        delete this->xboxButtonLabel;
        this->xboxButtonLabel = NULL;
    }

    this->ClearKeyboardKeyLabels();
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

void ButtonTutorialHint::SetXBoxButton(GameViewConstants::XBoxButtonType buttonType,
                                       const std::string& buttonText, const Colour& buttonColour) {

    
    const Texture* buttonTex = this->tutorialAssets->GetXBox360Texture(buttonType);
    assert(buttonTex != NULL);

    if (this->xboxButtonLabel != NULL) {
        delete this->xboxButtonLabel;
        this->xboxButtonLabel = NULL;
    }

    float buttonHeight = this->actionLabel.GetHeight() * BUTTON_SCALE_MULTIPLIER;
    this->xboxButtonLabel = new ButtonGlyphLabel(buttonTex, buttonText, buttonColour,
        buttonHeight, 0, 0);

    float offsetX, offsetY;
    GameViewConstants::GetInstance()->GetXBoxButtonLabelOffset(buttonType, buttonHeight,
        this->xboxButtonLabel->GetLabelWidth(), this->xboxButtonLabel->GetLabelHeight(),
        offsetX, offsetY);
    this->xboxButtonLabel->SetDimensions(buttonHeight, offsetX, offsetY);
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
#endif
}


float ButtonTutorialHint::GetHeight() const {
    return this->actionLabel.GetHeight() * BUTTON_SCALE_MULTIPLIER;
}

float ButtonTutorialHint::GetWidth() const {
    float width = this->actionLabel.GetLastRasterWidth();
    if (this->xboxButtonLabel != NULL) {
        width += this->xboxButtonLabel->GetWidth();
    }
    for (std::vector<ButtonGlyphLabel*>::const_iterator iter = this->keyboardKeyLabels.begin();
         iter != this->keyboardKeyLabels.end(); ++iter) {
    
        const ButtonGlyphLabel* buttonLabel = *iter;
        width += buttonLabel->GetWidth();
    }
    return width;
}

void ButtonTutorialHint::Draw(const Camera& camera) {
    UNUSED_PARAMETER(camera);

    float alpha = this->fadeAnim.GetInterpolantValue();
    if (alpha <= 0) {
        return;
    }

    float scale = 1.0f;//this->scaleAnim.GetInterpolantValue();
    const Point2D& topLeftCorner = this->actionLabel.GetTopLeftCorner();
    float currX = topLeftCorner[0] + this->actionLabel.GetLastRasterWidth();
    const float actualCenterY = topLeftCorner[1] - this->actionLabel.GetHeight()/2.0f;

    // Draw the action label...
    this->actionLabel.SetAlpha(alpha);
    this->actionLabel.Draw();

    // Draw the XBox Button label, if it exists
    if (this->xboxButtonLabel != NULL) {
        currX += this->xboxButtonLabel->GetWidth() / 2.0f;
        this->xboxButtonLabel->Draw(currX, actualCenterY, scale, alpha);
        currX += this->xboxButtonLabel->GetWidth() / 2.0f;

        if (!this->keyboardKeyLabels.empty()) {
            // Draw an 'or' label next to the xbox button label...
            this->orLabel.SetTopLeftCorner(currX, actualCenterY + this->orLabel.GetHeight()/2.0f);
            this->orLabel.SetAlpha(alpha);
            this->orLabel.Draw();
            currX += this->orLabel.GetLastRasterWidth();
        }
    }

    float commaHeightDiv2 = this->commaLabel.GetHeight()/2.0f;
    float buttonLabelDiv2 = 0;
    this->commaLabel.SetAlpha(alpha);

    // Draw the keyboard button label(s) if it/they exist...
    size_t count = 0;
    for (std::vector<ButtonGlyphLabel*>::const_iterator iter = this->keyboardKeyLabels.begin();
         iter != this->keyboardKeyLabels.end(); ++iter, count++) {
    
        ButtonGlyphLabel* buttonLabel = *iter;
        buttonLabelDiv2 = buttonLabel->GetWidth() / 2.0f;
        currX += buttonLabelDiv2;
        buttonLabel->Draw(currX, actualCenterY, scale, alpha);
        currX += buttonLabelDiv2;
        
        if (count != this->keyboardKeyLabels.size()-1) {
            this->commaLabel.SetTopLeftCorner(currX, actualCenterY + commaHeightDiv2);
            this->commaLabel.Draw();
            currX += this->commaLabel.GetLastRasterWidth();
        }
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

void ButtonTutorialHint::ButtonGlyphLabel::Draw(float centerX, float centerY, float scale, float alpha) {
    float widthHeightRatio = static_cast<float>(this->buttonTexture->GetWidth()) / static_cast<float>(this->buttonTexture->GetHeight());

    glPushMatrix();
    glTranslatef(centerX, centerY, 0.0f);
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
    this->buttonLabel.Draw();
}