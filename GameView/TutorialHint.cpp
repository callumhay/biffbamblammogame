/**
 * TutorialHint.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "TutorialHint.h"
#include "TutorialHintListeners.h"

const float TutorialHint::FADE_IN_SCALE_START = 2.25f;
const float TutorialHint::FADE_OUT_SCALE_END  = 2.25f;

TutorialHint::TutorialHint() : listener(NULL), isShown(false) {
}

TutorialHint::~TutorialHint() {
    this->SetListener(NULL);
}

void TutorialHint::SetListener(TutorialHintListener* listener) {
    if (this->listener != NULL) {
        delete this->listener;
    }
    this->listener = listener;
}
