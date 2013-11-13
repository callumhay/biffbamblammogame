/**
 * BasicMultiTutorialHint.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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