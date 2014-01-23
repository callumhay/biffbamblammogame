/**
 * DecoratorOverlayPane.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __DECORATOROVERLAYPANE_H__
#define __DECORATOROVERLAYPANE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/TextLabel.h"
#include "../GameControl/GameControl.h"

class Texture;
class TextLabel2DFixedWidth;
class TextLabel2D;

class OverlayPaneEventHandler {
public:
    virtual ~OverlayPaneEventHandler() {}
    virtual void OptionSelected(const std::string& optionText) { UNUSED_PARAMETER(optionText); };
};

class DecoratorOverlayPane {
public:
    DecoratorOverlayPane(OverlayPaneEventHandler* handler, size_t width, const Colour& bgColour);
    ~DecoratorOverlayPane();

    void DecorateAsBlammopediaPane(const Texture2D* texture, const std::string& currName, 
        const std::string& currDesc, const std::string& finePrint, bool imgHasBorder);

    enum LayoutType { Centered, TwoColumn, Left };
    void SetLayoutType(const LayoutType& layout) { this->layout = layout; }

    void AddText(const std::string& text, const Colour& colour = Colour(1,1,1), float scale = 1.0f, bool keepAtSameY = false);
    void AddImage(size_t width, const Texture* image, bool keepAtSameY = false);
    void AddYSpacing(int spacing) { this->currYPos += spacing; }
    void AddXSpacing(int spacing) { this->currXPos += spacing; }
    void SetSelectableOptions(const std::vector<std::string>& options, int defaultIdx);

    void Show(double delayInSecs, double timeInSecs);
    void Hide(double delayInSecs, double timeInSecs);

    void Tick(double dT);
    void Draw(size_t windowWidth, size_t windowHeight);

	void ButtonPressed(const GameControl::ActionButton& pressedButton);

    bool IsOptionSelectedAndActive() { return this->optionActive; }
    bool IsFinished() const { 
        return (this->optionActive && 
            this->fgFadeAnimation.GetInterpolantValue() <= 0.0f &&
            this->bgFadeAnimation.GetInterpolantValue() <= 0.0f);
    }

    int GetSelectedOptionIdx() const { return this->selectedIdx; }
    std::string GetSelectedOptionStr() const { return this->selectableOptions[this->selectedIdx]->GetText(); }

private:
    const float X_BORDER;
    const float Y_BORDER;
    const float ITEM_Y_SPACING;
    const float ITEM_X_SPACING;

    const float X_OPTION_GAP;
    const float COL_GAP;
    
    const Colour OPTION_IDLE_COLOUR;
    const Colour OPTION_SEL_COLOUR;	
    const Colour OPTION_ACTIVE_COLOUR; 

    class Image {
    public:
        Image(int topLeftX, int topLeftY, size_t width, size_t height, const Texture* texture, bool hasBorder = true);
        void Draw(float alpha, float tX, float tY);
    private:
        bool hasBorder;
        int topLeftX, topLeftY;
        size_t width, height;
        const Texture* texture;
    };

    AnimationLerp<float> fgFadeAnimation;
    AnimationLerp<float> bgFadeAnimation;
    AnimationLerp<float> scaleAnimation;
    AnimationMultiLerp<float> arrowPulseAnimation;
    AnimationMultiLerp<float> upAndDownAnimation;

    size_t width;
    OverlayPaneEventHandler* eventHandler;
    std::vector<TextLabel2DFixedWidth*> textLabels;
    std::vector<Image*> images;
    
    std::vector<TextLabel2D*> selectableOptions;
    std::vector<TextLabel2D*> selectedSelectableOptions;

    int selectedIdx;
    int currYPos;
    int currXPos;
    bool optionActive;

    Colour bgColour;

    LayoutType layout;
    int currImgYPos;

    void ClearTextLabels();
    void ClearImages();
    void ClearSelectableOptions();

    void DrawArrow(const Point2D& topLeftCorner, float alpha, 
                   float arrowHeight, bool isLeftPointing);

    DISALLOW_COPY_AND_ASSIGN(DecoratorOverlayPane);
};

#endif // __DECORATOROVERLAYPANE_H__