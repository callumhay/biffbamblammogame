/**
 * BasicTutorialHint.h
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