/**
 * DecoratorOverlayPane.h
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