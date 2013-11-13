/**
 * EmbededTutorialHint.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __EMBEDEDTUTORIALHINT_H__
#define __EMBEDEDTUTORIALHINT_H__

#include "TutorialHint.h"
#include "GameViewConstants.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/TextLabel.h"

class EmbededTutorialHint : public TutorialHint {
public:
    EmbededTutorialHint();
    virtual ~EmbededTutorialHint();

    virtual float GetHeight() const = 0;
    virtual float GetWidth() const = 0;

    virtual void SetColour(const Colour& colour) = 0;

    virtual void SetTopLeftCorner(float x, float y) = 0;

    virtual void SetAlphaWhenShowing(float alpha) = 0;

    virtual void Show(double delayInSeconds, double fadeInTimeInSeconds) = 0;
    virtual void Unshow(double delayInSeconds, double fadeOutTimeInSeconds, bool overridePrevUnshow = false) = 0;

    virtual void Tick(double dT) = 0;
    virtual void Draw(const Camera& camera, bool drawWithDepth = false, float depth = 0.0f) = 0;

protected:


private:
    DISALLOW_COPY_AND_ASSIGN(EmbededTutorialHint);
};

#endif // __EMBEDEDTUTORIALHINT_H__