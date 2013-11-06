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
    void PushHint(const std::string& hintStr, float scale, float width);
    bool IsDoneShowingAllHints() const;

    float GetHeight() const;
    float GetWidth() const;

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

    std::vector<BasicTutorialHint*> labels;
    int currLabelIdx;
    double labelCountdown;

    DISALLOW_COPY_AND_ASSIGN(BasicMultiTutorialHint);
};

inline void BasicMultiTutorialHint::PushHint(const std::string& hintStr, float scale, float width) {
    this->labels.push_back(new BasicTutorialHint(hintStr, scale, width));
}

inline bool BasicMultiTutorialHint::IsDoneShowingAllHints() const {
    return this->currLabelIdx >= static_cast<int>(this->labels.size())-1;
}

inline float BasicMultiTutorialHint::GetHeight() const {
    if (this->currLabelIdx >= 0 && this->currLabelIdx < static_cast<int>(this->labels.size())) {
        return this->labels[this->currLabelIdx]->GetHeight();
    }
    return 0.0f;
}
inline float BasicMultiTutorialHint::GetWidth() const {
    if (this->currLabelIdx >= 0 && this->currLabelIdx < static_cast<int>(this->labels.size())) {
        return this->labels[this->currLabelIdx]->GetWidth();
    }
    return 0.0f;
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