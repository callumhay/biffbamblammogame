/**
 * BasicMultiTutorialHint.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "BasicMultiTutorialHint.h"
#include "TutorialHintListeners.h"

BasicMultiTutorialHint::BasicMultiTutorialHint(double timeBetweenLabels, 
                                               double fadeOutTime, double fadeInTime) : 
EmbededTutorialHint(), currLabelIdx(-1), labelCountdown(0.0), timeBetweenLabels(timeBetweenLabels),
fadeInTime(fadeInTime), fadeOutTime(fadeOutTime) {
    
    assert(timeBetweenLabels > 0.0);
    assert(fadeInTime >= 0.0);
    assert(fadeOutTime >= 0.0);

    this->Reset();
}

BasicMultiTutorialHint::~BasicMultiTutorialHint() {
    // Clean up the labels
    for (std::vector<BasicTutorialHint*>::iterator iter = this->labels.begin(); iter != this->labels.end(); ++iter) {
        BasicTutorialHint* label = *iter;
        delete label;
    }
    this->labels.clear();
}

void BasicMultiTutorialHint::Reset() {
    this->currLabelIdx = -1;
    this->labelCountdown = 0;
}

void BasicMultiTutorialHint::SetTopLeftCorner(float x, float y) {
    for (std::vector<BasicTutorialHint*>::iterator iter = this->labels.begin(); iter != this->labels.end(); ++iter) {
        BasicTutorialHint* label = *iter;
        label->SetTopLeftCorner(x, y);
    }
}

void BasicMultiTutorialHint::SetAlphaWhenShowing(float alpha) {
    for (std::vector<BasicTutorialHint*>::iterator iter = this->labels.begin(); iter != this->labels.end(); ++iter) {
        BasicTutorialHint* label = *iter;
        label->SetAlphaWhenShowing(alpha);
    }
}

void BasicMultiTutorialHint::Show(double delayInSeconds, double fadeInTimeInSeconds) {
    if (this->isShown) { return; }

    // Show the current label...
    if (this->currLabelIdx >= 0 && this->currLabelIdx < static_cast<int>(this->labels.size())-1) {
        this->labelCountdown = this->timeBetweenLabels + delayInSeconds;
        this->labels[this->currLabelIdx]->Show(delayInSeconds, fadeInTimeInSeconds);
    }

    if (this->listener != NULL) {
        this->listener->OnTutorialHintShown();
    }

    this->isShown = true;
}

void BasicMultiTutorialHint::Unshow(double delayInSeconds, double fadeOutTimeInSeconds, bool overridePrevUnshow) {
    if (!this->isShown && !overridePrevUnshow) { return; }

    // Unshow the current label...
    if (this->currLabelIdx >= 0 && this->currLabelIdx < static_cast<int>(this->labels.size())-1) {
        this->labels[this->currLabelIdx]->Unshow(delayInSeconds, fadeOutTimeInSeconds, overridePrevUnshow);
    }

    if (this->listener != NULL) {
        this->listener->OnTutorialHintUnshown();
    }

    this->isShown = false;
}

void BasicMultiTutorialHint::Tick(double dT) {

    // Tick all the labels...
    for (std::vector<BasicTutorialHint*>::iterator iter = this->labels.begin(); iter != this->labels.end(); ++iter) {
        BasicTutorialHint* label = *iter;
        label->Tick(dT);
    }

    if (this->currLabelIdx >= static_cast<int>(this->labels.size()) || !this->isShown) {
        return;
    }

    if (this->labelCountdown <= 0.0) {

        if (this->currLabelIdx >= 0) {
            this->labels[this->currLabelIdx]->Unshow(0.0, this->fadeOutTime);
        }

        this->currLabelIdx++;
        
        if (this->currLabelIdx < 0 || this->currLabelIdx >= static_cast<int>(this->labels.size())) {
            return;
        }

        this->labels[this->currLabelIdx]->Show(this->fadeOutTime, this->fadeInTime);
        this->labelCountdown = this->timeBetweenLabels;
    }
    else {
        this->labelCountdown -= dT;
    }
}
