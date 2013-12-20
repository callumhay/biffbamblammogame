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
fadeInTime(fadeInTime), fadeOutTime(fadeOutTime), isPaused(false), labelColour(1,1,1) {
    
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
    this->isPaused = false;
    for (std::vector<BasicTutorialHint*>::iterator iter = this->labels.begin(); iter != this->labels.end(); ++iter) {
        BasicTutorialHint* label = *iter;
        label->Unshow(0, 0, true);
    }
}

void BasicMultiTutorialHint::Pause(double pauseFadeOutTime) {
    if (this->isPaused || !this->isShown) { return; }

    if (this->currLabelIdx >= 0 && this->currLabelIdx < static_cast<int>(this->labels.size())) {
        this->labels[this->currLabelIdx]->Unshow(0.0, pauseFadeOutTime, true);
    }

    if (this->listener != NULL) {
        this->listener->OnTutorialHintUnshown();
    }

    this->isPaused = true;
}

void BasicMultiTutorialHint::Resume(double pauseFadeInTime) {
    if (!this->isPaused || !this->isShown) { return; }

    if (this->currLabelIdx >= 0 && this->currLabelIdx < static_cast<int>(this->labels.size())) {
        this->labels[this->currLabelIdx]->Show(0.0, pauseFadeInTime);
        if (this->listener != NULL) {
            this->listener->OnTutorialHintShown();
        }
    }
    this->isPaused = false;
}

float BasicMultiTutorialHint::GetHeight() const {
    float maxHeight = 0.0f;
    if (this->currLabelIdx >= 1 && this->currLabelIdx <= static_cast<int>(this->labels.size())) {
        if (this->labels[this->currLabelIdx-1]->GetAlpha() > 0.0f) {
            maxHeight = this->labels[this->currLabelIdx-1]->GetHeight();
        }
    }
    if (this->currLabelIdx >= 0 && this->currLabelIdx < static_cast<int>(this->labels.size())) {
        if (this->labels[this->currLabelIdx]->GetAlpha() > 0.0f) {
            maxHeight = std::max<float>(maxHeight, this->labels[this->currLabelIdx]->GetHeight());
        }
    }
    return maxHeight;
}

float BasicMultiTutorialHint::GetWidth() const {
    float maxWidth = 0.0f;
    if (this->currLabelIdx >= 1 && this->currLabelIdx <= static_cast<int>(this->labels.size())) {
        if (this->labels[this->currLabelIdx-1]->GetAlpha() > 0.0f) {
            maxWidth = this->labels[this->currLabelIdx-1]->GetWidth();
        }
    }
    if (this->currLabelIdx >= 0 && this->currLabelIdx < static_cast<int>(this->labels.size())) {
        if (this->labels[this->currLabelIdx]->GetAlpha() > 0.0f) {
            maxWidth = std::max<float>(maxWidth, this->labels[this->currLabelIdx]->GetWidth());
        }
    }
    return maxWidth;
}

void BasicMultiTutorialHint::SetColour(const Colour& colour) {
    for (std::vector<BasicTutorialHint*>::iterator iter = this->labels.begin(); iter != this->labels.end(); ++iter) {
        BasicTutorialHint* label = *iter;
        label->SetColour(colour);
    }
    this->labelColour = colour;
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

    if (this->isPaused || !this->isShown || this->currLabelIdx >= static_cast<int>(this->labels.size())) {
        return;
    }

    if (this->labelCountdown <= 0.0) {

        double showDelayTime = 0.0;
        if (this->currLabelIdx >= 0) {
            this->labels[this->currLabelIdx]->Unshow(0.0, this->fadeOutTime);
            showDelayTime = this->fadeOutTime;
        }

        this->currLabelIdx++;
        
        if (this->currLabelIdx < 0 || this->currLabelIdx >= static_cast<int>(this->labels.size())) {
            if (this->listener != NULL) {
                this->listener->OnTutorialHintUnshown();
            }
            this->isShown  = false;
            this->isPaused = false;
            return;
        }

        this->labels[this->currLabelIdx]->Show(showDelayTime, this->fadeInTime);
        this->labelCountdown = this->timeBetweenLabels;
    }
    else {
        this->labelCountdown -= dT;
    }
}
