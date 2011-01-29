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

    float GetTextHeight() const;
    float GetHeight() const;

    void SetKeyName(const char* keyName);
    void SetBeforeKeyText(const char* label);
    void SetAfterKeyText(const char* label);

    void SetTopLeftCorner(const Point2D& p);
    void SetTopLeftCorner(float x, float y);

    void SetAlpha(float alpha);
    void SetBeforeAndAfterTextColour(const Colour& c);

    void Draw();

private:
    float alpha;

    Texture* keyTexture;
    TextLabel2D* keyLbl;
    TextLabel2D* beforeLbl;
    TextLabel2D* afterLbl;

    DISALLOW_COPY_AND_ASSIGN(KeyboardHelperLabel);
};

inline float KeyboardHelperLabel::GetTextHeight() const {
    return std::max<float>(this->beforeLbl->GetHeight(), this->afterLbl->GetHeight());
}

inline float KeyboardHelperLabel::GetHeight() const {
    return 2.15f * this->beforeLbl->GetHeight();
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

inline void KeyboardHelperLabel::SetBeforeAndAfterTextColour(const Colour& c) {
    this->beforeLbl->SetColour(c);
    this->afterLbl->SetColour(c);
}

#endif // __KEYBOARDHELPERLABEL_H__