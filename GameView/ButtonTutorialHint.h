/**
 * ButtonTutorialHint.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __BUTTONTUTORIALHINT_H__
#define __BUTTONTUTORIALHINT_H__

#include "BasicTutorialHint.h"
#include "GameViewConstants.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/TextLabel.h"

class Texture;
class GameTutorialAssets;

class ButtonTutorialHint : public BasicTutorialHint {
public:
    ButtonTutorialHint(const GameTutorialAssets* tutorialAssets, const std::string& action);
    ~ButtonTutorialHint();

    float GetHeight() const;
    float GetWidth() const;

    void SetColour(const Colour& colour);

    void SetActionNameWithSeparator(const std::string& action, const std::string& separator = "");

    void SetXBoxButton(GameViewConstants::XBoxButtonType buttonType, const std::string& buttonText,
        const Colour& buttonColour);
    void SetXBoxButtons(const std::list<GameViewConstants::XBoxButtonType>& buttonTypes,
        const std::list<std::string>& buttonTexts, const std::list<Colour>& buttonColours);

    void SetKeyboardButton(GameViewConstants::KeyboardButtonType buttonType, const std::string& buttonText);
    void SetKeyboardButtons(const std::list<GameViewConstants::KeyboardButtonType>& buttonTypes,
        const std::list<std::string>& buttonTexts);

    void SetMouseButton(GameViewConstants::MouseButtonType buttonType, const std::string& buttonText);

    void Draw(const Camera& camera, bool drawWithDepth = false, float depth = 0.0f);

private:
    static const float BUTTON_SCALE_MULTIPLIER;
        
    class ButtonGlyphLabel {
    public:
        ButtonGlyphLabel(const Texture* buttonTexture, const std::string& buttonText,
                         const Colour& buttonColour, float height, float offsetX, float offsetY);
        ~ButtonGlyphLabel();

        float GetLabelWidth() const { return this->buttonLabel.GetLastRasterWidth(); }
        float GetLabelHeight() const { return this->buttonLabel.GetHeight(); }

        float GetWidth() const { 
            float widthHeightRatio = static_cast<float>(this->buttonTexture->GetWidth()) / static_cast<float>(this->buttonTexture->GetHeight());
            return std::max<float>(this->GetLabelWidth(), widthHeightRatio*this->height);
        }
        float GetHeight() const { return this->height; }

        void SetDimensions(float height, float offsetX, float offsetY);
        void Draw(float centerX, float centerY, float scale, float alpha, bool drawWithDepth, float depth);
        
    private:
        float height;
        float labelOffsetX, labelOffsetY;
        const Texture* buttonTexture;
        TextLabel2D buttonLabel;
        Colour buttonColour;

        DISALLOW_COPY_AND_ASSIGN(ButtonGlyphLabel);
    };

    const GameTutorialAssets* tutorialAssets;

    std::vector<ButtonGlyphLabel*> keyboardKeyLabels;
    std::vector<ButtonGlyphLabel*> xboxLabels;
    ButtonGlyphLabel* mouseLabel;

    TextLabel2D orLabel;
    TextLabel2D commaLabel;

    void ClearKeyboardKeyLabels();
    void ClearXBoxLabels();
    void ClearMouseLabel();

    DISALLOW_COPY_AND_ASSIGN(ButtonTutorialHint);
};

inline void ButtonTutorialHint::SetColour(const Colour& colour) {
    BasicTutorialHint::SetColour(colour);
    this->orLabel.SetColour(colour);
    this->commaLabel.SetColour(colour);
}

inline float ButtonTutorialHint::GetHeight() const {
    return this->actionLabel.GetHeight() * BUTTON_SCALE_MULTIPLIER;
}

#endif // __BUTTONTUTORIALHINT_H__