/**
 * BasicMultiTutorialHint.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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

#ifndef __BASICMULTITUTORIALHINT_H__
#define __BASICMULTITUTORIALHINT_H__

#include "BasicTutorialHint.h"

class BasicMultiTutorialHint : public EmbededTutorialHint {
public:
    BasicMultiTutorialHint(double timeBetweenLabels, double fadeOutTime, double fadeInTime);
    ~BasicMultiTutorialHint();

    void Reset();
    void Pause(double pauseFadeOutTime);
    void Resume(double pauseFadeInTime);
    void PushHint(const std::string& hintStr, float scale, float width);
    bool IsDoneShowingAllHints() const;

    float GetHeight() const;
    float GetWidth() const;

    void SetColour(const Colour& colour);

    void SetTopLeftCorner(float x, float y);
    void SetAlphaWhenShowing(float alpha);

    void Show(double delayInSeconds, double fadeInTimeInSeconds);
    void Unshow(double delayInSeconds, double fadeOutTimeInSeconds, bool overridePrevUnshow = false);

    void Tick(double dT);
    void Draw(const Camera& camera, bool drawWithDepth = false, float depth = 0.0f);

private:
    double timeBetweenLabels;
    double fadeOutTime;
    double fadeInTime;

    bool isPaused;
    Colour labelColour;

    std::vector<BasicTutorialHint*> labels;
    int currLabelIdx;
    double labelCountdown;

    DISALLOW_COPY_AND_ASSIGN(BasicMultiTutorialHint);
};

inline void BasicMultiTutorialHint::PushHint(const std::string& hintStr, float scale, float width) {
    BasicTutorialHint* newHint = new BasicTutorialHint(hintStr, scale, width);
    newHint->SetColour(this->labelColour);
    this->labels.push_back(newHint);
}

inline bool BasicMultiTutorialHint::IsDoneShowingAllHints() const {
    bool lastLabelNotShowing = !this->labels.empty() && (this->labels.back()->GetAlpha() <= 0.0f);
    return this->currLabelIdx >= static_cast<int>(this->labels.size())-1 && !this->isShown && lastLabelNotShowing;
}

inline void BasicMultiTutorialHint::Draw(const Camera& camera, bool drawWithDepth, float depth) {
    if (this->currLabelIdx >= 1 && this->currLabelIdx <= static_cast<int>(this->labels.size())) {
        this->labels[this->currLabelIdx-1]->Draw(camera, drawWithDepth, depth);
    }
    if (this->currLabelIdx >= 0 && this->currLabelIdx < static_cast<int>(this->labels.size())) {
        this->labels[this->currLabelIdx]->Draw(camera, drawWithDepth, depth);
    }
}

#endif // __BASICMULTITUTORIALHINT_H__