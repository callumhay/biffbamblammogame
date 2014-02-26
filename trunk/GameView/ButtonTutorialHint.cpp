/**
 * ButtonTutorialHint.cpp
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

#include "ButtonTutorialHint.h"
#include "GameFontAssetsManager.h"
#include "GameTutorialAssets.h"
#include "TutorialHintListeners.h"

#include "../ResourceManager.h"
#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/GeometryMaker.h"
#include "../BlammoEngine/Camera.h"

const float ButtonTutorialHint::BUTTON_SCALE_MULTIPLIER = 2.0f;


ButtonTutorialHint::ButtonTutorialHint(const GameTutorialAssets* tutorialAssets,
                                       const std::string& action) : 
BasicTutorialHint(""), tutorialAssets(tutorialAssets),
orLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom,
GameFontAssetsManager::Medium), " or  "),
commaLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom,
GameFontAssetsManager::Medium), ","),
mouseLabel(NULL) {

    assert(tutorialAssets != NULL);

    this->SetActionNameWithSeparator(action, ":");
    this->SetColour(Colour(0.3882f, 0.72157f, 1.0f));
    this->actionLabel.SetDropShadow(Colour(0,0,0), 0.10f);

    this->orLabel.SetScale(0.75f);
    this->orLabel.SetDropShadow(Colour(0,0,0), 0.10f);
    this->commaLabel.SetDropShadow(Colour(0,0,0), 0.07f);

    this->fadeAnim.ClearLerp();
    this->fadeAnim.SetInterpolantValue(0.0f);
    this->fadeAnim.SetRepeat(false);
}

ButtonTutorialHint::~ButtonTutorialHint() {
    this->ClearKeyboardKeyLabels();
    this->ClearXBoxLabels();
    this->ClearMouseLabel();
}

void ButtonTutorialHint::SetActionNameWithSeparator(const std::string& action, const std::string& separator) {
    this->actionLabel.SetText(action + separator + " ");
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

float ButtonTutorialHint::GetWidth() const {
    float width = this->actionLabel.GetWidth();
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

void ButtonTutorialHint::Draw(const Camera& camera, bool drawWithDepth, float depth) {
    UNUSED_PARAMETER(camera);
    UNUSED_PARAMETER(drawWithDepth);
    UNUSED_PARAMETER(depth);

    float alpha = this->fadeAnim.GetInterpolantValue();
    if (alpha <= 0) {
        return;
    }

    if (this->flashAnim != NULL) {
        this->actionLabel.SetColour(this->flashAnim->GetInterpolantValue());
    }
    else {
        this->actionLabel.SetColour(this->actionLabelDefaultColour);
    }

    float scale = 1.0f;
    const Point2D& topLeftCorner = this->actionLabel.GetTopLeftCorner();
    float currX = topLeftCorner[0] + this->actionLabel.GetWidth();
    const float actualCenterY = topLeftCorner[1] - this->actionLabel.GetHeight()/2.0f;

    // Draw the action label...
    this->actionLabel.SetAlpha(alpha);
    this->actionLabel.Draw();

    float commaHeightDiv2 = this->commaLabel.GetHeight()/2.0f;
    float buttonLabelDiv2 = 0;
    this->commaLabel.SetAlpha(alpha);

    glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Camera::PushWindowCoords();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

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

    glPopMatrix();
    Camera::PopWindowCoords();
    glPopAttrib();
    debug_opengl_state();
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