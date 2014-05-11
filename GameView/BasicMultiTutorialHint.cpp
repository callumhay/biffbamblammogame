/**
 * BasicMultiTutorialHint.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
