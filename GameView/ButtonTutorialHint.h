/**
 * ButtonTutorialHint.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BUTTONTUTORIALHINT_H__
#define __BUTTONTUTORIALHINT_H__

#include "TutorialHint.h"
#include "GameViewConstants.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/TextLabel.h"

class Texture;
class GameTutorialAssets;

class ButtonTutorialHint : public TutorialHint {
public:
    ButtonTutorialHint(const GameTutorialAssets* tutorialAssets, const std::string& action);
    ~ButtonTutorialHint();

    float GetHeight() const;
    float GetWidth() const;

    void SetTopLeftCorner(float x, float y);
    void SetActionName(const std::string& action);

    void SetXBoxButton(GameViewConstants::XBoxButtonType buttonType, const std::string& buttonText,
        const Colour& buttonColour);
    void SetKeyboardButton(GameViewConstants::KeyboardButtonType buttonType, const std::string& buttonText);
    void SetKeyboardButtons(const std::list<GameViewConstants::KeyboardButtonType>& buttonTypes,
        const std::list<std::string>& buttonTexts);

    void Draw(const Camera& camera);

private:
    static const float BUTTON_SCALE_MULTIPLIER;
        
    class ButtonGlyphLabel {
    public:
        ButtonGlyphLabel(const Texture* buttonTexture, const std::string& buttonText,
                         const Colour& buttonColour, float height, float offsetX, float offsetY);
        ~ButtonGlyphLabel();

        float GetLabelWidth() const { return this->buttonLabel.GetLastRasterWidth(); }
        float GetLabelHeight() const { return this->buttonLabel.GetHeight(); }

        float GetWidth() const { return std::max<float>(this->GetLabelWidth(), this->height); }
        float GetHeight() const { return this->height; }

        void SetDimensions(float height, float offsetX, float offsetY);
        void Draw(float centerX, float centerY, float scale, float alpha);
        
    private:
        float height;
        float labelOffsetX, labelOffsetY;
        const Texture* buttonTexture;
        TextLabel2D buttonLabel;
        Colour buttonColour;

        DISALLOW_COPY_AND_ASSIGN(ButtonGlyphLabel);
    };


    const GameTutorialAssets* tutorialAssets;
    ButtonGlyphLabel* xboxButtonLabel;
    std::vector<ButtonGlyphLabel*> keyboardKeyLabels;
    TextLabel2D actionLabel;
    TextLabel2D orLabel;
    TextLabel2D commaLabel;

    void ClearKeyboardKeyLabels();

    DISALLOW_COPY_AND_ASSIGN(ButtonTutorialHint);
};


inline void ButtonTutorialHint::SetTopLeftCorner(float x, float y) {
    this->actionLabel.SetTopLeftCorner(x, y);
}

inline void ButtonTutorialHint::SetActionName(const std::string& action) {
    this->actionLabel.SetText(action + std::string(" ="));
}


#endif // __BUTTONTUTORIALHINT_H__