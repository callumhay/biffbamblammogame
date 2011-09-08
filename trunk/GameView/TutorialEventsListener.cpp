/**
 * TutorialEventsListener.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "TutorialEventsListener.h"

TutorialEventsListener::TutorialEventsListener() : 
numBlocksDestroyed(0), movePaddleHint(NULL), movePaddleHintUnshown(false), shootHint(NULL) {
}

TutorialEventsListener::~TutorialEventsListener() {
}

void TutorialEventsListener::ButtonPressed(const GameControl::ActionButton& pressedButton) {
    switch (pressedButton) {
        case GameControl::LeftButtonAction:
        case GameControl::RightButtonAction:
            if (!this->movePaddleHintUnshown) {
                this->movePaddleHint->Unshow(2.0);
                this->movePaddleHintUnshown = true;
            }
            break;
        default:
            break;
    }
}

void TutorialEventsListener::MousePressed(const GameControl::MouseButton& pressedButton) {
    UNUSED_PARAMETER(pressedButton);
}