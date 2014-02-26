/**
 * BasicTutorialHint.cpp
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

#include "BasicTutorialHint.h"
#include "GameFontAssetsManager.h"
#include "GameTutorialAssets.h"
#include "TutorialHintListeners.h"

BasicTutorialHint::BasicTutorialHint(const std::string& action, float scale, float screenSpaceWidth) : 
EmbededTutorialHint(), flashAnim(NULL), alphaWhenShowing(1.0f), actionLabelDefaultColour(Colour(1,1,1)),
actionLabel(GameFontAssetsManager::GetInstance()->GetFont(
            GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium), 
            screenSpaceWidth <= 0 ? Camera::GetWindowWidth() : screenSpaceWidth, "") {

    this->SetActionName(action);
    this->actionLabel.SetColour(this->actionLabelDefaultColour);
    this->actionLabel.SetDropShadow(Colour(0,0,0), 0.10f);
    this->actionLabel.SetScale(scale);

    this->fadeAnim.ClearLerp();
    this->fadeAnim.SetInterpolantValue(0.0f);
    this->fadeAnim.SetRepeat(false);
}

BasicTutorialHint::~BasicTutorialHint() {
    if (this->flashAnim != NULL) {
        delete this->flashAnim;
        this->flashAnim = NULL;
    }
}

void BasicTutorialHint::SetFlashing(bool on) {
    if (on) {
        if (this->flashAnim == NULL) {
            this->flashAnim = new AnimationMultiLerp<Colour>(
                GameViewConstants::GetInstance()->BuildFlashingColourAnimation());
        }
    }
    else {
        if (this->flashAnim != NULL) {
            delete this->flashAnim;
            this->flashAnim = NULL;
        }
    }
}

void BasicTutorialHint::Show(double delayInSeconds, double fadeInTimeInSeconds) {
    if (this->isShown) { return; }

    this->fadeAnim.SetLerp(delayInSeconds, delayInSeconds+fadeInTimeInSeconds, this->fadeAnim.GetInterpolantValue(), this->alphaWhenShowing);
    this->fadeAnim.SetRepeat(false);

    if (this->listener != NULL) {
        this->listener->OnTutorialHintShown();
    }

    this->isShown = true;
}

void BasicTutorialHint::Unshow(double delayInSeconds, double fadeOutTimeInSeconds, bool overridePrevUnshow) {
    if (!this->isShown && !overridePrevUnshow) { return; }

    if (delayInSeconds == 0 && fadeOutTimeInSeconds == 0) {
        this->fadeAnim.ClearLerp();
        this->fadeAnim.SetInterpolantValue(0.0f);
    }
    else {
        this->fadeAnim.SetLerp(delayInSeconds, delayInSeconds+fadeOutTimeInSeconds, this->fadeAnim.GetInterpolantValue(), 0.0f);
    }
    this->fadeAnim.SetRepeat(false);

    if (this->listener != NULL) {
        this->listener->OnTutorialHintUnshown();
    }

    this->isShown = false;
}

void BasicTutorialHint::Tick(double dT) {
    this->fadeAnim.Tick(dT);
    if (this->flashAnim != NULL) {
        this->flashAnim->Tick(dT);
    }
}

void BasicTutorialHint::Draw(const Camera& camera, bool drawWithDepth, float depth) {
    UNUSED_PARAMETER(camera);
    UNUSED_PARAMETER(drawWithDepth);
    UNUSED_PARAMETER(depth);

    float alpha = this->fadeAnim.GetInterpolantValue();
    if (alpha <= 0) {
        return;
    }

    if (this->flashAnim != NULL) {
        this->actionLabel.SetColour(this->flashAnim->GetInterpolantValue());
    }
    else {
        this->actionLabel.SetColour(this->actionLabelDefaultColour);
    }

    // Draw the action label...
    this->actionLabel.SetAlpha(alpha);
    this->actionLabel.Draw();
}
