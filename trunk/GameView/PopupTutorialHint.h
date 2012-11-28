/**
 * PopupTutorialHint.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __POPUPTUTORIALHINT_H__
#define __POPUPTUTORIALHINT_H__

#include "TutorialHint.h"
#include "DecoratorOverlayPane.h"

class PopupTutorialHint : public TutorialHint {
public:
    PopupTutorialHint(size_t width);
    ~PopupTutorialHint();

    DecoratorOverlayPane* GetPane() const {
        return this->pane;
    }

    void ButtonPressed(const GameControl::ActionButton& pressedButton);
    //void ButtonReleased(const GameControl::ActionButton& releasedButton) { UNUSED_PARAMETER(releasedButton); }
    //void MousePressed(const GameControl::MouseButton& pressedButton)     { UNUSED_PARAMETER(pressedButton);  }
    //void MouseReleased(const GameControl::MouseButton& releasedButton)   { UNUSED_PARAMETER(releasedButton); }

    void Show(double delayInSeconds, double fadeInTimeInSeconds);
    void Unshow(double delayInSeconds, double fadeOutTimeInSeconds, bool overridePrevUnshow = false);

    void JustTick(double dT);
    void Draw(double dT, const Camera& camera, bool drawWithDepth = false, float depth = 0.0f);

private:
    static const double UNSHOW_TIME;

    class PaneHandler : public OverlayPaneEventHandler {
    public:
        PaneHandler(PopupTutorialHint* popupHint) : popupHint(popupHint) {
            assert(popupHint != NULL);
        }
        void OptionSelected(const std::string& optionText);
    private:
        PopupTutorialHint* popupHint;
    };

    DecoratorOverlayPane* pane;
    OverlayPaneEventHandler* paneHandler;

    double timeToUnshow;
    DISALLOW_COPY_AND_ASSIGN(PopupTutorialHint);
};


#endif // __POPUPTUTORIALHINT_H__