/**
 * KeyboardHelperLabel.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "KeyboardHelperLabel.h"
#include "GameViewConstants.h"
#include "../ResourceManager.h"

KeyboardHelperLabel::KeyboardHelperLabel(const GameFontAssetsManager::FontStyle& fontStyle, 
                                         const GameFontAssetsManager::FontSize& fontSize, 
                                         const char* beforeTxt, 
                                         const char* keyName, 
                                         const char* afterTxt) : 
keyTexture(NULL), keyLbl(NULL), beforeLbl(NULL), afterLbl(NULL), alpha(1.0f) {

    this->keyTexture = ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_KEYBOARD_KEY, 
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->keyTexture != NULL);

    this->beforeLbl = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(fontStyle, fontSize), beforeTxt);
    this->beforeLbl->SetColour(Colour(0,0,0));
    this->keyLbl    = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(fontStyle, fontSize), keyName);
    this->keyLbl->SetColour(Colour(0,0,0));
    this->keyLbl->SetScale(0.5f);
    this->afterLbl  = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(fontStyle, fontSize), afterTxt);
    this->afterLbl->SetColour(Colour(0,0,0));
}

KeyboardHelperLabel::~KeyboardHelperLabel() {
    bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->keyTexture);
    assert(success);

    delete this->beforeLbl;
    this->beforeLbl = NULL;
    delete this->keyLbl;
    this->keyLbl = NULL;
    delete this->afterLbl;
    this->afterLbl = NULL;
}

void KeyboardHelperLabel::Draw() {
    static const float TEXT_KEY_PIC_GAP = 10;
    const float X_TEXT_LOC = this->beforeLbl->GetTopLeftCorner()[0];
    const float Y_TEXT_LOC = this->beforeLbl->GetTopLeftCorner()[1];
    const float KEY_HEIGHT = 2 * this->beforeLbl->GetHeight();
    const float X_LEFT_KEY_PIC  = X_TEXT_LOC + this->beforeLbl->GetLastRasterWidth() + TEXT_KEY_PIC_GAP;
    const float X_RIGHT_KEY_PIC = X_LEFT_KEY_PIC + KEY_HEIGHT;

    this->beforeLbl->SetAlpha(this->alpha);
    this->beforeLbl->SetTopLeftCorner(X_TEXT_LOC, Y_TEXT_LOC);
    this->beforeLbl->Draw();

    this->afterLbl->SetAlpha(this->alpha);
    this->afterLbl->SetTopLeftCorner(X_RIGHT_KEY_PIC + TEXT_KEY_PIC_GAP, Y_TEXT_LOC);
    this->afterLbl->Draw();

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
    glVertex2f(X_RIGHT_KEY_PIC, Y_TEXT_LOC + this->beforeLbl->GetHeight()/2);
    glTexCoord2i(0, 1);
    glVertex2f(X_LEFT_KEY_PIC, Y_TEXT_LOC + this->beforeLbl->GetHeight()/2);
    glTexCoord2i(0, 0);
    glVertex2f(X_LEFT_KEY_PIC, Y_TEXT_LOC - 1.75f * this->beforeLbl->GetHeight());
    glEnd();
    
    glPopAttrib();

    this->keyLbl->SetAlpha(this->alpha);
    this->keyLbl->SetTopLeftCorner(X_LEFT_KEY_PIC + this->beforeLbl->GetHeight()/2, Y_TEXT_LOC - this->beforeLbl->GetHeight()/4);
    this->keyLbl->Draw();
}