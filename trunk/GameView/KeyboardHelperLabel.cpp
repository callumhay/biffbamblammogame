/**
 * KeyboardHelperLabel.cpp
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

#include "KeyboardHelperLabel.h"
#include "GameViewConstants.h"
#include "../BlammoEngine/StringHelper.h"
#include "../ResourceManager.h"

const float KeyboardHelperLabel::TEXT_KEY_PIC_GAP = 10;

KeyboardHelperLabel::KeyboardHelperLabel(const GameFontAssetsManager::FontStyle& fontStyle, 
                                         const GameFontAssetsManager::FontSize& fontSize, 
                                         const char* beforeTxt, 
                                         const char* keyName, 
                                         const char* afterTxt) : 
keyTexture(NULL), xboxButtonTexture(NULL), keyLbl(NULL), buttonLbl(NULL), orLbl(NULL), beforeLbl(NULL),
afterLbl(NULL), alpha(1.0f), xboxButtonDisplayOn(true) {

    std::string keyNameStr(keyName);
    std::transform(keyNameStr.begin(), keyNameStr.end(), keyNameStr.begin(), ::toupper);

    if (keyNameStr == "SPACE") {
        this->keyTexture = ResourceManager::GetInstance()->GetImgTextureResource(
            GameViewConstants::GetInstance()->TEXTURE_LONG_KEYBOARD_KEY, Texture::Trilinear, GL_TEXTURE_2D);
    }
    else {
        this->keyTexture = ResourceManager::GetInstance()->GetImgTextureResource(
            GameViewConstants::GetInstance()->TEXTURE_SHORT_KEYBOARD_KEY, Texture::Trilinear, GL_TEXTURE_2D);
    }
    assert(this->keyTexture != NULL);

    this->xboxButtonTexture = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_XBOX_CONTROLLER_BUTTON, Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->xboxButtonTexture != NULL); 

    this->beforeLbl = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(fontStyle, fontSize), beforeTxt);
    this->beforeLbl->SetColour(Colour(0,0,0));
    
    this->keyLbl = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(fontStyle, fontSize), keyName);
    this->keyLbl->SetColour(Colour(0,0,0));
    this->keyLbl->SetScale(0.5f);

    if (keyNameStr == "ESC") {
        this->buttonLbl = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(fontStyle, fontSize), "B");
    }
    else {
        this->buttonLbl = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(fontStyle, fontSize), "A");
    }

    this->buttonLbl->SetColour(Colour(1,1,1));
    this->buttonLbl->SetScale(0.8f);

    this->orLbl = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(fontStyle, fontSize), "or");
    this->orLbl->SetColour(Colour(0,0,0));
    this->orLbl->SetScale(0.75f);

    this->afterLbl  = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(fontStyle, fontSize), afterTxt);
    this->afterLbl->SetColour(Colour(0,0,0));
}

KeyboardHelperLabel::~KeyboardHelperLabel() {
    bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->keyTexture);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->xboxButtonTexture);
    assert(success);

    delete this->beforeLbl;
    this->beforeLbl = NULL;
    delete this->keyLbl;
    this->keyLbl = NULL;
    delete this->buttonLbl;
    this->buttonLbl = NULL;
    delete this->orLbl;
    this->orLbl = NULL;
    delete this->afterLbl;
    this->afterLbl = NULL;
}

void KeyboardHelperLabel::SetScale(float scale) {
    this->keyLbl->SetScale(0.5f*scale);
    this->buttonLbl->SetScale(0.8f*scale);
    this->orLbl->SetScale(0.75f*scale);
    this->beforeLbl->SetScale(scale);
    this->afterLbl->SetScale(scale);
}

void KeyboardHelperLabel::Draw() {
    const float X_TEXT_LOC = this->beforeLbl->GetTopLeftCorner()[0];
    const float Y_TEXT_LOC = this->beforeLbl->GetTopLeftCorner()[1];
    const float KEY_SIZE = 2 * this->beforeLbl->GetHeight();
    const float X_LEFT_KEY_PIC  = X_TEXT_LOC + this->beforeLbl->GetLastRasterWidth() + TEXT_KEY_PIC_GAP;
    const float X_RIGHT_KEY_PIC = X_LEFT_KEY_PIC + KEY_SIZE;

    this->beforeLbl->SetAlpha(this->alpha);
    this->beforeLbl->SetTopLeftCorner(X_TEXT_LOC, Y_TEXT_LOC);
    this->beforeLbl->Draw();
    
    const float HALF_BEFORE_LBL_HEIGHT =  this->beforeLbl->GetHeight()/2;

    glPushAttrib(GL_CURRENT_BIT | GL_TEXTURE_BIT | GL_ENABLE_BIT | GL_LINE_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw the key...
    this->keyTexture->BindTexture();
    glColor4f(1, 1, 1, this->alpha);
    glBegin(GL_QUADS);
    glTexCoord2i(1, 0);
    glVertex2f(X_RIGHT_KEY_PIC, Y_TEXT_LOC - 1.75f * this->beforeLbl->GetHeight());
    glTexCoord2i(1, 1);
    glVertex2f(X_RIGHT_KEY_PIC, Y_TEXT_LOC + HALF_BEFORE_LBL_HEIGHT);
    glTexCoord2i(0, 1);
    glVertex2f(X_LEFT_KEY_PIC, Y_TEXT_LOC + HALF_BEFORE_LBL_HEIGHT);
    glTexCoord2i(0, 0);
    glVertex2f(X_LEFT_KEY_PIC, Y_TEXT_LOC - 1.75f * this->beforeLbl->GetHeight());
    glEnd();
    
    this->keyLbl->SetAlpha(this->alpha);
    this->keyLbl->SetTopLeftCorner(X_LEFT_KEY_PIC + this->beforeLbl->GetHeight()/2, Y_TEXT_LOC - this->beforeLbl->GetHeight()/4);
    this->keyLbl->Draw();

    float nextXPos = X_RIGHT_KEY_PIC;
    // If the XBox controller button has been enabled then draw it too
    if (this->xboxButtonDisplayOn) {
        const float BUTTON_SIZE = KEY_SIZE * 0.9f;
        nextXPos += TEXT_KEY_PIC_GAP;

        this->orLbl->SetAlpha(this->alpha);
        this->orLbl->SetTopLeftCorner(nextXPos, Y_TEXT_LOC - this->beforeLbl->GetHeight()*0.2f);
        this->orLbl->Draw();
        nextXPos += this->orLbl->GetLastRasterWidth() + TEXT_KEY_PIC_GAP;

        const Colour& B_BUTTON_COLOUR = GameViewConstants::GetInstance()->XBOX_CONTROLLER_B_BUTTON_COLOUR;

        // Draw the XBox 360 controller button
        this->xboxButtonTexture->BindTexture();
        glColor4f(B_BUTTON_COLOUR.R(), B_BUTTON_COLOUR.G(), B_BUTTON_COLOUR.B(), this->alpha);
        glBegin(GL_QUADS);
        glTexCoord2i(1, 0);
        glVertex2f(nextXPos+BUTTON_SIZE, Y_TEXT_LOC - BUTTON_SIZE*0.8f);
        glTexCoord2i(1, 1);
        glVertex2f(nextXPos+BUTTON_SIZE, Y_TEXT_LOC + BUTTON_SIZE*0.2f);
        glTexCoord2i(0, 1);
        glVertex2f(nextXPos, Y_TEXT_LOC + BUTTON_SIZE*0.2f);
        glTexCoord2i(0, 0);
        glVertex2f(nextXPos, Y_TEXT_LOC - BUTTON_SIZE*0.8f);
        glEnd();

        this->buttonLbl->SetAlpha(this->alpha);
        this->buttonLbl->SetTopLeftCorner(nextXPos + (BUTTON_SIZE - this->buttonLbl->GetLastRasterWidth())/2,
            Y_TEXT_LOC + BUTTON_SIZE*0.2f - (BUTTON_SIZE - this->buttonLbl->GetHeight())/2);
        this->buttonLbl->Draw();

        nextXPos += BUTTON_SIZE + TEXT_KEY_PIC_GAP;


    }

    glPopAttrib();

    this->afterLbl->SetAlpha(this->alpha);
    this->afterLbl->SetTopLeftCorner(nextXPos, Y_TEXT_LOC);
    this->afterLbl->Draw();
}