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
#include "../GameControl/GameControl.h"

class Texture;
class TextLabel2DFixedWidth;
class TextLabel2D;

class OverlayPaneEventHandler {
public:
    virtual void OptionSelected(const std::string& optionText) = 0;
};

class DecoratorOverlayPane {
public:
    DecoratorOverlayPane(OverlayPaneEventHandler* handler, size_t width);
    ~DecoratorOverlayPane();

    void AddText(const std::string& text);
    void AddImage(size_t width, size_t height, Texture* image);
    void SetSelectableOptions(const std::list<std::string>& options, int defaultIdx);

    void Draw(size_t windowWidth, size_t windowHeight);

	void ButtonPressed(const GameControl::ActionButton& pressedButton);

private:
    static const float X_BORDER;
    static const float Y_BORDER;
    static const float ITEM_SPACING;

    class Image {
    public:
        Image(size_t topLeftX, size_t topLeftY, size_t width, size_t height, Texture* texture);
        void Draw(float alpha);
    private:
        size_t topLeftX, topLeftY;
        size_t width, height;
        Texture* texture;
    };

    size_t width;
    OverlayPaneEventHandler* eventHandler;
    std::vector<TextLabel2DFixedWidth*> textLabels;
    std::vector<Image*> images;
    std::vector<TextLabel2D*> selectableOptions;
    int selectedIdx;
    size_t currYPos;

    void ClearTextLabels();
    void ClearImages();
    void ClearSelectableOptions();

    DISALLOW_COPY_AND_ASSIGN(DecoratorOverlayPane);
};

#endif // __DECORATOROVERLAYPANE_H__