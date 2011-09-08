/**
 * TutorialHint.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __TUTORIALHINT_H__
#define __TUTORIALHINT_H__

#include "../BlammoEngine/Animation.h"
#include "../GameControl/GameControl.h"

class GameModel;
class Camera;

/**
 * Abstract super-class for all tutorial hints - graphical hints that pop-up
 * during the tutorial level of the game to guide and teach the user.
 */
class TutorialHint {
public:
    TutorialHint();
    virtual ~TutorialHint();

    // Triggers - these can do various things to the tooltip depending on how they are
    // overriden in child classes
    virtual void ButtonPressed(const GameControl::ActionButton& pressedButton)   { UNUSED_PARAMETER(pressedButton);  }
    virtual void ButtonReleased(const GameControl::ActionButton& releasedButton) { UNUSED_PARAMETER(releasedButton); }
    virtual void MousePressed(const GameControl::MouseButton& pressedButton)     { UNUSED_PARAMETER(pressedButton);  }
    virtual void MouseReleased(const GameControl::MouseButton& releasedButton)   { UNUSED_PARAMETER(releasedButton); }

    void Show(double fadeInTimeInSeconds);
    void Unshow(double fadeOutTimeInSeconds);

    virtual void Tick(double dT);
    virtual void Draw(const Camera& camera) = 0;    

protected:
    static const float FADE_IN_SCALE_START;
    static const float FADE_OUT_SCALE_END;

    //enum State { TurningOnState, OnState, TurningOffState, OffState };
    //State currState;

    AnimationLerp<float> fadeAnim;
    AnimationLerp<float> scaleAnim;
    //double lifeTimeInSecs;

private:
    DISALLOW_COPY_AND_ASSIGN(TutorialHint);
};

inline void TutorialHint::Tick(double dT) {
    fadeAnim.Tick(dT);
    scaleAnim.Tick(dT);
}

#endif // __TUTORIALHINT_H__