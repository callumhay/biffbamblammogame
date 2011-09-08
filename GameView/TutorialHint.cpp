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

const float TutorialHint::FADE_IN_SCALE_START = 2.25f;
const float TutorialHint::FADE_OUT_SCALE_END  = 2.25f;

TutorialHint::TutorialHint() {
    this->fadeAnim.ClearLerp();
    this->fadeAnim.SetInterpolantValue(0.0f);
    this->fadeAnim.SetRepeat(false);
    this->scaleAnim.ClearLerp();
    this->scaleAnim.SetInterpolantValue(0.0f);
    this->scaleAnim.SetRepeat(false);
}

TutorialHint::~TutorialHint() {
}

void TutorialHint::Show(double fadeInTimeInSeconds) {
    this->fadeAnim.SetLerp(0.9 * fadeInTimeInSeconds, 1.0f);
    this->fadeAnim.SetRepeat(false);
    this->scaleAnim.SetLerp(0.0, fadeInTimeInSeconds, TutorialHint::FADE_IN_SCALE_START, 1.0f);
    this->scaleAnim.SetRepeat(false);
}

void TutorialHint::Unshow(double fadeOutTimeInSeconds) {
    this->fadeAnim.SetLerp(fadeOutTimeInSeconds, 0.0f);
    this->fadeAnim.SetRepeat(false);
    this->scaleAnim.SetLerp(fadeOutTimeInSeconds, TutorialHint::FADE_OUT_SCALE_END);
    this->scaleAnim.SetRepeat(false);
}
