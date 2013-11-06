/**
 * TutorialHintListeners.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __TUTORIALHINTLISTENERS_H__
#define __TUTORIALHINTLISTENERS_H__

#include "../GameModel/PlayerPaddle.h"
#include "ButtonTutorialHint.h"

/**
 * Superclass Event listener for tutorial hints.
 */
class TutorialHintListener {
public:
    virtual ~TutorialHintListener() {}

    virtual void OnTutorialHintShown() {}
    virtual void OnTutorialHintUnshown() {}

protected:
    TutorialHintListener() {}
};

#endif // __TUTORIALHINTLISTENERS_H__