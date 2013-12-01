/**
 * BasicTutorialHint.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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
