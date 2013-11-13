/**
 * BasicTutorialHint.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __BASICTUTORIALHINT_H__
#define __BASICTUTORIALHINT_H__

#include "EmbededTutorialHint.h"
#include "GameViewConstants.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/TextLabel.h"

class BasicTutorialHint : public EmbededTutorialHint {
public:
    explicit BasicTutorialHint(const std::string& action, float scale = 1.0f, float screenSpaceWidth = -1);
    virtual ~BasicTutorialHint();

    virtual float GetHeight() const;
    virtual float GetWidth() const;

    virtual void SetColour(const Colour& colour);

    void SetTopLeftCorner(float x, float y);
    void SetActionName(const std::string& action);

    void SetFlashing(bool on);
    void SetAlphaWhenShowing(float alpha) { this->alphaWhenShowing = alpha; };
    float GetAlpha() const { return this->fadeAnim.GetInterpolantValue(); }

    void Show(double delayInSeconds, double fadeInTimeInSeconds);
    void Unshow(double delayInSeconds, double fadeOutTimeInSeconds, bool overridePrevUnshow = false);

    void Tick(double dT);
    virtual void Draw(const Camera& camera, bool drawWithDepth = false, float depth = 0.0f);

protected:
    AnimationLerp<float> fadeAnim;
    AnimationMultiLerp<Colour>* flashAnim;
    float alphaWhenShowing;

    Colour actionLabelDefaultColour;
    TextLabel2DFixedWidth actionLabel;

private:
    DISALLOW_COPY_AND_ASSIGN(BasicTutorialHint);
};

inline float BasicTutorialHint::GetHeight() const {
    return this->actionLabel.GetHeight();
}

inline float BasicTutorialHint::GetWidth() const {
    return this->actionLabel.GetWidth();
}

inline void BasicTutorialHint::SetColour(const Colour& colour) {
    this->actionLabel.SetColour(colour);
    this->actionLabelDefaultColour = colour;
}

inline void BasicTutorialHint::SetTopLeftCorner(float x, float y) {
    this->actionLabel.SetTopLeftCorner(x, y);
}

inline void BasicTutorialHint::SetActionName(const std::string& action) {
    this->actionLabel.SetText(action);
}

#endif // __BASICTUTORIALHINT_H__