/**
 * TutorialHint.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "TutorialHint.h"
#include "TutorialHintListeners.h"

const float TutorialHint::FADE_IN_SCALE_START = 2.25f;
const float TutorialHint::FADE_OUT_SCALE_END  = 2.25f;

TutorialHint::TutorialHint() : listener(NULL), isShown(false) {
    this->fadeAnim.ClearLerp();
    this->fadeAnim.SetInterpolantValue(0.0f);
    this->fadeAnim.SetRepeat(false);
    //this->scaleAnim.ClearLerp();
    //this->scaleAnim.SetInterpolantValue(0.0f);
    //this->scaleAnim.SetRepeat(false);
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

void TutorialHint::Show(double delayInSeconds, double fadeInTimeInSeconds) {
    if (this->isShown) { return; }

    this->fadeAnim.SetLerp(delayInSeconds, delayInSeconds+fadeInTimeInSeconds, this->fadeAnim.GetInterpolantValue(), 1.0f);
    this->fadeAnim.SetRepeat(false);
    //this->scaleAnim.SetLerp(delayInSeconds, delayInSeconds+fadeInTimeInSeconds, TutorialHint::FADE_IN_SCALE_START, 1.0f);
    //this->scaleAnim.SetRepeat(false);

    if (this->listener != NULL) {
        this->listener->OnTutorialHintShown();
    }

    this->isShown = true;
}

void TutorialHint::Unshow(double delayInSeconds, double fadeOutTimeInSeconds, bool overridePrevUnshow) {
    if (!this->isShown && !overridePrevUnshow) { return; }

    this->fadeAnim.SetLerp(delayInSeconds, delayInSeconds+fadeOutTimeInSeconds, this->fadeAnim.GetInterpolantValue(), 0.0f);
    this->fadeAnim.SetRepeat(false);
    //this->scaleAnim.SetLerp(delayInSeconds, fadeOutTimeInSeconds, this->scaleAnim.GetInterpolantValue(), TutorialHint::FADE_OUT_SCALE_END);
    //this->scaleAnim.SetRepeat(false);

    if (this->listener != NULL) {
        this->listener->OnTutorialHintUnshown();
    }

    this->isShown = false;
}