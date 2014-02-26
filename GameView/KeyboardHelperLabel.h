/**
 * KeyboardHelperLabel.h
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

#ifndef __KEYBOARDHELPERLABEL_H__
#define __KEYBOARDHELPERLABEL_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/TextLabel.h"
#include "GameFontAssetsManager.h"

class Texture;

/**
 * A class for drawing a keyboard label - a label with text on either side of a keyboard key graphic
 * that displays a key on the keyboard. This is useful for labels that help the user out and tell them
 * what key to hit in order to perform a certain action in the game or menu system.
 * e.g., "Hit the esc key to exit".
 */
class KeyboardHelperLabel {
public:
    KeyboardHelperLabel(const GameFontAssetsManager::FontStyle& fontStyle, 
                        const GameFontAssetsManager::FontSize& fontSize,
                        const char* beforeTxt, const char* keyName, const char* afterTxt);
    ~KeyboardHelperLabel();

    float GetAlpha() const;

    float GetTextHeight() const;
    float GetHeight() const;
    float GetWidth() const;

    void SetKeyName(const char* keyName);
    void SetBeforeKeyText(const char* label);
    void SetAfterKeyText(const char* label);

    void SetTopLeftCorner(const Point2D& p);
    void SetTopLeftCorner(float x, float y);

    void SetAlpha(float alpha);
    void SetBeforeAndAfterTextColour(const Colour& c);

    void SetScale(float scale);

    void ShowXBoxButton(bool show);

    void Draw();

private:
    static const float TEXT_KEY_PIC_GAP;

    float alpha;

    bool xboxButtonDisplayOn;

    Texture* keyTexture;
    Texture* xboxButtonTexture;

    TextLabel2D* keyLbl;
    TextLabel2D* buttonLbl;
    TextLabel2D* orLbl;
    TextLabel2D* beforeLbl;
    TextLabel2D* afterLbl;

    DISALLOW_COPY_AND_ASSIGN(KeyboardHelperLabel);
};

inline float KeyboardHelperLabel::GetAlpha() const {
    return this->alpha;
}

inline float KeyboardHelperLabel::GetTextHeight() const {
    return std::max<float>(this->beforeLbl->GetHeight(), this->afterLbl->GetHeight());
}

inline float KeyboardHelperLabel::GetHeight() const {
    return 2.15f * this->beforeLbl->GetHeight();
}

inline float KeyboardHelperLabel::GetWidth() const {
    return this->beforeLbl->GetLastRasterWidth() + this->orLbl->GetLastRasterWidth() + this->afterLbl->GetLastRasterWidth() +
        1.9f * 2.0f * this->beforeLbl->GetHeight() + 2*TEXT_KEY_PIC_GAP;
}

inline void KeyboardHelperLabel::SetKeyName(const char* keyName) {
    this->keyLbl->SetText(keyName);
}

inline void KeyboardHelperLabel::SetBeforeKeyText(const char* label) {
    this->beforeLbl->SetText(label);
}

inline void KeyboardHelperLabel::SetAfterKeyText(const char* label) {
    this->afterLbl->SetText(label);
}

inline void KeyboardHelperLabel::SetTopLeftCorner(const Point2D& p) {
    this->beforeLbl->SetTopLeftCorner(p);
}

inline void KeyboardHelperLabel::SetTopLeftCorner(float x, float y) {
    this->beforeLbl->SetTopLeftCorner(x,y);
}

inline void KeyboardHelperLabel::SetAlpha(float alpha) {
    this->alpha = alpha;
}

inline void KeyboardHelperLabel::ShowXBoxButton(bool show) {
    this->xboxButtonDisplayOn = show;
}

inline void KeyboardHelperLabel::SetBeforeAndAfterTextColour(const Colour& c) {
    this->beforeLbl->SetColour(c);
    this->afterLbl->SetColour(c);
    this->orLbl->SetColour(c);
}

#endif // __KEYBOARDHELPERLABEL_H__