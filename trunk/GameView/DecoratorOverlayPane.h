/**
 * DecoratorOverlayPane.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __DECORATOROVERLAYPANE_H__
#define __DECORATOROVERLAYPANE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Point.h"
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

    enum LayoutType { Centered, TwoColumn };
    void SetLayoutType(const LayoutType& layout) { this->layout = layout; }

    void AddText(const std::string& text, const Colour& colour = Colour(1,1,1), float scale = 1.0f);
    void AddImage(size_t width, const Texture* image);
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

private:
    static const float X_BORDER;
    static const float Y_BORDER;
    static const float ITEM_SPACING;
    static const float X_OPTION_GAP;
    static const float COL_GAP;
    
    const Colour OPTION_IDLE_COLOUR;
    const Colour OPTION_SEL_COLOUR;	
    const Colour OPTION_ACTIVE_COLOUR; 

    class Image {
    public:
        Image(int topLeftX, int topLeftY, size_t width, size_t height, const Texture* texture);
        void Draw(float alpha, float tX, float tY);
    private:
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