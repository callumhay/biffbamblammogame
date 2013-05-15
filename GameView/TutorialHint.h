/**
 * TutorialHint.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __TUTORIALHINT_H__
#define __TUTORIALHINT_H__

#include "../BlammoEngine/Animation.h"
#include "../GameControl/GameControl.h"

class GameModel;
class TutorialHintListener;
class Camera;

/**
 * Abstract super-class for all tutorial hints - graphical hints that pop-up
 * during the tutorial level of the game to guide and teach the user.
 */
class TutorialHint {
public:
    TutorialHint();
    virtual ~TutorialHint();

    void SetListener(TutorialHintListener* listener);
    bool GetIsVisible() const { return this->isShown; }

    // Triggers - these can do various things to the tooltip depending on how they are
    // overriden in child classes
    virtual void ButtonPressed(const GameControl::ActionButton& pressedButton)   { UNUSED_PARAMETER(pressedButton);  }
    virtual void ButtonReleased(const GameControl::ActionButton& releasedButton) { UNUSED_PARAMETER(releasedButton); }
    virtual void MousePressed(const GameControl::MouseButton& pressedButton)     { UNUSED_PARAMETER(pressedButton);  }
    virtual void MouseReleased(const GameControl::MouseButton& releasedButton)   { UNUSED_PARAMETER(releasedButton); }

    virtual void Show(double delayInSeconds, double fadeInTimeInSeconds) = 0;
    virtual void Unshow(double delayInSeconds, double fadeOutTimeInSeconds, bool overridePrevUnshow = false) = 0;

    virtual void Tick(double dT) = 0;
    virtual void Draw(const Camera& camera, bool drawWithDepth = false, float depth = 0.0f) = 0;

protected:
    static const float FADE_IN_SCALE_START;
    static const float FADE_OUT_SCALE_END;

    TutorialHintListener* listener;
    bool isShown;

private:
    DISALLOW_COPY_AND_ASSIGN(TutorialHint);
};

#endif // __TUTORIALHINT_H__