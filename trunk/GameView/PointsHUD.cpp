/**
 * PointsHUD.cpp
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

#include "PointsHUD.h"
#include "GameFontAssetsManager.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/GeometryMaker.h"
#include "../BlammoEngine/Texture.h"

#include "../ESPEngine/ESPPointEmitter.h"

#include "../GameModel/GameLevel.h"
#include "../GameModel/PointAward.h"

#include "../ResourceManager.h"

const int PointsHUD::STAR_SIZE                          = 32;
const float  PointsHUD::STAR_HALF_SIZE                  = STAR_SIZE / 2.0f;
const float PointsHUD::STAR_ALPHA                       = 0.75f;
const int PointsHUD::STAR_GAP                           = 3;
const int PointsHUD::SCREEN_EDGE_VERTICAL_GAP           = 5;
const int PointsHUD::SCREEN_EDGE_HORIZONTAL_GAP         = 5;
const int PointsHUD::STAR_TO_SCORE_VERTICAL_GAP         = 5;
const int PointsHUD::SCORE_TO_MULTIPLER_HORIZONTAL_GAP  = 5;
const int PointsHUD::ALL_STARS_WIDTH                    = (STAR_GAP * (GameLevel::MAX_STARS_PER_LEVEL-1) + STAR_SIZE * GameLevel::MAX_STARS_PER_LEVEL);

PointsHUD::PointsHUD() : numStars(0), currPtScore(0), 
ptScoreLabel(NULL), starTex(NULL), lensFlareTex(NULL), haloTex(NULL), starAlpha(1.0f),
multiplier(new MultiplierHUD()), multiplierGage(new MultiplierGageHUD()),
haloGrower(1.0f, 3.2f), haloFader(1.0f, 0.0f), flareRotator(0, 1, ESPParticleRotateEffector::CLOCKWISE) {

    this->ptScoreLabel = new TextLabel2D(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Big), "0");
    this->ptScoreLabel->SetColour(Colour(0.0f, 0.6f, 1.0f));
    this->ptScoreLabel->SetDropShadow(Colour(0,0,0), 0.05f);
    this->ptScoreLabel->SetScale(0.95f);
    
    this->scoreAnimator = AnimationLerp<long>(&this->currPtScore);
    this->scoreAnimator.SetRepeat(false);

    this->starTex = ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_STAR,
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->starTex != NULL);
    this->lensFlareTex = ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_LENSFLARE,
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->lensFlareTex != NULL);
    this->haloTex = ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_HALO,
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->lensFlareTex != NULL);

    // Setup the star animators
    this->starSizeAnimators.reserve(GameLevel::MAX_STARS_PER_LEVEL);
    this->starColourAnimators.reserve(GameLevel::MAX_STARS_PER_LEVEL);
    for (int i = 0; i < GameLevel::MAX_STARS_PER_LEVEL; i++) {
        AnimationMultiLerp<float>* starSizeAnim = new AnimationMultiLerp<float>();
        starSizeAnim->SetInterpolantValue(1.0f);
        starSizeAnim->SetRepeat(false);
        this->starSizeAnimators.push_back(starSizeAnim);

        AnimationMultiLerp<ColourRGBA>* starColourAnim = new AnimationMultiLerp<ColourRGBA>();
        starColourAnim->SetInterpolantValue(ColourRGBA(GameViewConstants::GetInstance()->INACTIVE_POINT_STAR_COLOUR, STAR_ALPHA));
        starColourAnim->SetRepeat(false);
        this->starColourAnimators.push_back(starColourAnim);
    }

    std::vector<double> timeValues;
    timeValues.reserve(3);
    timeValues.push_back(0.0);
    timeValues.push_back(1.0);
    timeValues.push_back(2.0);

    std::vector<ColourRGBA> colourValues;
    colourValues.reserve(3);
    colourValues.push_back(ColourRGBA(GameViewConstants::GetInstance()->ACTIVE_POINT_STAR_COLOUR, 1.2f * STAR_ALPHA));
    colourValues.push_back(ColourRGBA(GameViewConstants::GetInstance()->BRIGHT_POINT_STAR_COLOUR, 1.0f));
    colourValues.push_back(ColourRGBA(GameViewConstants::GetInstance()->ACTIVE_POINT_STAR_COLOUR, 1.2f * STAR_ALPHA));

    this->flashingStarAnimator.SetInterpolantValue(colourValues.front());
    this->flashingStarAnimator.SetLerp(timeValues, colourValues);
    this->flashingStarAnimator.SetRepeat(true);
}

PointsHUD::~PointsHUD() {
    delete this->ptScoreLabel;
    this->ptScoreLabel = NULL;
    delete this->multiplier;
    this->multiplier = NULL;
    delete this->multiplierGage;
    this->multiplierGage = NULL;

    // Clean up all the star animations and emitters
    for (size_t i = 0; i < this->starSizeAnimators.size(); i++) {
        delete this->starSizeAnimators[i];
    }
    this->starSizeAnimators.clear();
    for (size_t i = 0; i < this->starColourAnimators.size(); i++) {
        delete this->starColourAnimators[i];
    }
    this->starColourAnimators.clear();

    for (std::list<ESPPointEmitter*>::iterator iter = this->starEffectEmitters.begin();
         iter != this->starEffectEmitters.end(); ++iter) {
        ESPPointEmitter* emitter = *iter;
        delete emitter;
        emitter = NULL;
    }
    this->starEffectEmitters.clear();

    // Release textures
    bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->starTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->lensFlareTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->haloTex);
    assert(success);
}

void PointsHUD::Draw(const Camera& camera, int displayWidth, int displayHeight, double dT) {
    this->scoreAnimator.Tick(dT);

	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Camera::PushWindowCoords();
	glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
	glLoadIdentity();

    // Draw the multiplier gage
    this->multiplierGage->Draw(displayWidth - SCREEN_EDGE_HORIZONTAL_GAP, displayHeight - SCREEN_EDGE_VERTICAL_GAP, dT);
    // Draw the multiplier
    this->multiplier->Draw(displayWidth - SCREEN_EDGE_HORIZONTAL_GAP - 20, displayHeight - SCREEN_EDGE_VERTICAL_GAP - 15, dT);

    // Draw the current total point score
    std::stringstream ptScoreString;
    ptScoreString << this->currPtScore;
    this->ptScoreLabel->SetText(ptScoreString.str());

    this->ptScoreLabel->SetTopLeftCorner(displayWidth - SCREEN_EDGE_HORIZONTAL_GAP - 
        MultiplierGageHUD::MULTIPLIER_GAGE_SIZE - this->ptScoreLabel->GetLastRasterWidth(),
        displayHeight - SCREEN_EDGE_VERTICAL_GAP - 20);
    this->ptScoreLabel->Draw();

    // Draw the current star score
    Point2D starStartPos = this->GetStarStartPos(displayWidth);
    this->DrawIdleStars(camera, starStartPos[0], starStartPos[1], dT);

    Camera::PopWindowCoords();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glPopAttrib();
}

void PointsHUD::DrawIdleStars(const Camera& camera, float rightMostX, float topMostY, double dT) {
    float currentCenterX = rightMostX - ALL_STARS_WIDTH + STAR_HALF_SIZE;
    float centerY = topMostY - STAR_HALF_SIZE;

    this->flashingStarAnimator.Tick(dT);

    this->starTex->BindTexture();
    for (int currStarIdx = 0; currStarIdx < GameLevel::MAX_STARS_PER_LEVEL; currStarIdx++) {

        this->starColourAnimators[currStarIdx]->Tick(dT);
        const ColourRGBA* starColour = &this->starColourAnimators[currStarIdx]->GetInterpolantValue();
        if (starColour->GetColour() == GameViewConstants::GetInstance()->ACTIVE_POINT_STAR_COLOUR) {
            starColour = &this->flashingStarAnimator.GetInterpolantValue();
        }
        glColor4f(starColour->R(), starColour->G(), starColour->B(), starColour->A() * this->starAlpha);

        this->starSizeAnimators[currStarIdx]->Tick(dT);
        float sizeMultiplier = this->starSizeAnimators[currStarIdx]->GetInterpolantValue();
        float totalStarSize  = sizeMultiplier * STAR_SIZE;

        this->DrawQuad(currentCenterX , centerY, totalStarSize);
        currentCenterX += STAR_SIZE + STAR_GAP;
    }
    
    for (std::list<ESPPointEmitter*>::iterator iter = this->starEffectEmitters.begin();
         iter != this->starEffectEmitters.end();) {

        ESPPointEmitter* emitter = *iter;
        emitter->SetParticleAlpha(emitter->GetParticleAlpha());
        emitter->Tick(dT);
        emitter->Draw(camera);

        if (emitter->IsDead()) {
            delete emitter;
            emitter = NULL;
            iter = this->starEffectEmitters.erase(iter);
        }
        else {
            ++iter;
        }
    }
}

void PointsHUD::DrawQuad(float centerX, float centerY, float size) {
    glPushMatrix();
    glTranslatef(centerX, centerY, 0.0f);
    glScalef(size, size, 1.0f);
    GeometryMaker::GetInstance()->DrawQuad();
    glPopMatrix();
}

void PointsHUD::SetStarAcquiredAnimation(const Camera& camera, size_t starIdx) {
    UNUSED_PARAMETER(camera);
    assert(starIdx < this->starSizeAnimators.size());

    static const double TOTAL_ANIM_TIME = 1.0;

    // Set up animations for size and colour
    std::vector<double> timeValues;
    timeValues.reserve(3);
    timeValues.push_back(0.0);
    timeValues.push_back(TOTAL_ANIM_TIME * 0.33);
    timeValues.push_back(TOTAL_ANIM_TIME);

    std::vector<float> sizeValues;
    sizeValues.reserve(3);
    sizeValues.push_back(1.0f);
    sizeValues.push_back(2.0f);
    sizeValues.push_back(1.0f);

    std::vector<ColourRGBA> colourValues;
    colourValues.reserve(3);
    colourValues.push_back(ColourRGBA(GameViewConstants::GetInstance()->INACTIVE_POINT_STAR_COLOUR, STAR_ALPHA));
    colourValues.push_back(ColourRGBA(1,1,1,1));
    colourValues.push_back(ColourRGBA(GameViewConstants::GetInstance()->ACTIVE_POINT_STAR_COLOUR, 1.2f * STAR_ALPHA));

    this->starSizeAnimators[starIdx]->SetLerp(timeValues, sizeValues);
    this->starSizeAnimators[starIdx]->SetRepeat(false);
    this->starColourAnimators[starIdx]->SetLerp(timeValues, colourValues);
    this->starColourAnimators[starIdx]->SetRepeat(false);

    Point2D starCenter = this->GetStarStartPos(camera.GetWindowWidth());
    starCenter[0] = starCenter[0] - ALL_STARS_WIDTH + STAR_HALF_SIZE;
    starCenter[1] = starCenter[1] - STAR_HALF_SIZE;
    for (size_t i = 0; i < starIdx; i++) {
        starCenter[0] += STAR_SIZE + STAR_GAP;
    }

    // Setup any effects emitters
    ESPPointEmitter* flareEmitter = new ESPPointEmitter();
    flareEmitter->SetSpawnDelta(ESPEmitter::ONLY_SPAWN_ONCE);
	flareEmitter->SetInitialSpd(ESPInterval(0.0f, 0.0f));
	flareEmitter->SetParticleLife(ESPInterval(TOTAL_ANIM_TIME - EPSILON));
	flareEmitter->SetParticleSize(ESPInterval(2.0f * STAR_SIZE));
	flareEmitter->SetParticleAlignment(ESP::ScreenAligned);
    flareEmitter->SetEmitPosition(Point3D(starCenter + Vector2D(STAR_SIZE*0.15f, STAR_SIZE*0.15f), 0.0f));
    flareEmitter->AddEffector(&this->flareRotator);
    flareEmitter->SetParticles(1, static_cast<Texture2D*>(this->lensFlareTex));
    this->starEffectEmitters.push_back(flareEmitter);

    ESPPointEmitter* haloEmitter = new ESPPointEmitter();
    haloEmitter->SetSpawnDelta(ESPEmitter::ONLY_SPAWN_ONCE);
	haloEmitter->SetInitialSpd(ESPInterval(0.0f, 0.0f));
	haloEmitter->SetParticleLife(ESPInterval(1.5 * TOTAL_ANIM_TIME));
	haloEmitter->SetParticleSize(ESPInterval(1.25f * STAR_SIZE));
	haloEmitter->SetParticleAlignment(ESP::ScreenAligned);
    haloEmitter->SetEmitPosition(Point3D(starCenter, 0.0f));
    haloEmitter->AddEffector(&this->haloGrower);
    haloEmitter->AddEffector(&this->haloFader);
    haloEmitter->SetParticles(1, static_cast<Texture2D*>(this->haloTex));
    this->starEffectEmitters.push_back(haloEmitter);
}

/**
 * Call this to reinitialize the points HUD to what you would expect it
 * to look like at the very start of a level.
 */
void PointsHUD::Reinitialize() {
    this->scoreAnimator.ClearLerp();
    this->scoreAnimator.SetInterpolantValue(0);

    for (size_t i = 0; i < this->starSizeAnimators.size(); i++) {
        this->starSizeAnimators[i]->ClearLerp();
        this->starSizeAnimators[i]->SetInterpolantValue(1.0f);
        this->starSizeAnimators[i]->SetRepeat(false);
    }
    for (size_t i = 0; i < this->starColourAnimators.size(); i++) {
        this->starColourAnimators[i]->ClearLerp();
        this->starColourAnimators[i]->SetInterpolantValue(ColourRGBA(GameViewConstants::GetInstance()->INACTIVE_POINT_STAR_COLOUR, STAR_ALPHA));
        this->starColourAnimators[i]->SetRepeat(false);
    }

    for (std::list<ESPPointEmitter*>::iterator iter = this->starEffectEmitters.begin();
         iter != this->starEffectEmitters.end(); ++iter) {
        ESPPointEmitter* emitter = *iter;
        delete emitter;
        emitter = NULL;
    }
    this->starEffectEmitters.clear();

    this->numStars         = 0;
    this->currPtScore      = 0;
    this->multiplier->Reinitialize();
    this->multiplierGage->Reinitialize();
}

/**
 * Sets the current alpha (1 for completely visible, 0 for completely invisible) for the entire
 * points HUD.
 */
void PointsHUD::SetAlpha(float alpha) {
    this->ptScoreLabel->SetAlpha(alpha);
    this->multiplier->SetAlpha(alpha);
    this->multiplierGage->SetAlpha(alpha);
    
    this->starAlpha = alpha;
    for (std::list<ESPPointEmitter*>::iterator iter = this->starEffectEmitters.begin();
         iter != this->starEffectEmitters.end(); ++iter) {
        ESPPointEmitter* currEmitter = *iter;
        currEmitter->SetParticleAlpha(ESPInterval(alpha)); 
    }
}

// MULTIPLIER HUD FUNCTIONS ***********************************************

const float PointsHUD::MultiplierHUD::BANG_TEXT_BORDER = 7.0f;

PointsHUD::MultiplierHUD::MultiplierHUD() : 
ptMultiplierLabel(NULL), currPtMultiplier(1), multiplierBangTex(NULL), 
size(0), currState(None), scaleAnim(0.0f) {

    ColourRGBA multiplierLabelColour(1.0f, 0.4f, 0.0f, 0.0f);
    this->ptMultiplierLabel = new TextLabel2D(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Big), "");
    this->ptMultiplierLabel->SetColour(multiplierLabelColour);
    this->ptMultiplierLabel->SetDropShadow(Colour(0,0,0), 0.09f);
    this->ptMultiplierLabel->SetScale(1.1f);

    this->multiplierBangTex = ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_MULTIPLIER_BANG,
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->multiplierBangTex != NULL);

    this->scaleAnim.SetInterpolantValue(0.0f);
    this->scaleAnim.SetRepeat(false);

    this->rgbaLabelAnim.SetInterpolantValue(multiplierLabelColour);
    this->rgbaLabelAnim.SetRepeat(false);
}

PointsHUD::MultiplierHUD::~MultiplierHUD() {
    delete this->ptMultiplierLabel;
    this->ptMultiplierLabel = NULL;

    bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->multiplierBangTex);
    UNUSED_VARIABLE(success);
    assert(success);
}

void PointsHUD::MultiplierHUD::Reinitialize() {
    // Clean up all animations...
    ColourRGBA multiplierLabelColour(1.0f, 0.4f, 0.0f, 0.0f);
    
    this->rgbaLabelAnim.ClearLerp();
    this->rgbaLabelAnim.SetInterpolantValue(multiplierLabelColour);
    
    this->scaleAnim.ClearLerp();
    this->scaleAnim.SetInterpolantValue(0.0f);

    this->currPtMultiplier = 1;
    this->ptMultiplierLabel->SetText("");
    this->SetCurrentAnimationState(None);
}

void PointsHUD::MultiplierHUD::SetAlpha(float alpha) {
    this->ptMultiplierLabel->SetAlpha(alpha);
}

void PointsHUD::MultiplierHUD::SetMultiplier(int multiplierAmt) {
    // Ignore cases where the multiplier is already the same
    if (this->currPtMultiplier == multiplierAmt || multiplierAmt < 1) {
        return;
    }

    int oldMultiplierAmt = this->currPtMultiplier;
    // Change the multiplier amount...
    this->currPtMultiplier = multiplierAmt;

    bool wasMultiplierVisibleBefore = (oldMultiplierAmt >= 2);
    bool isMultiplierVisibleNow     = (multiplierAmt >= 2);

    // Change the label if need be...
    if (isMultiplierVisibleNow) {
        std::stringstream ptMultString;
        ptMultString << multiplierAmt << "x";
        this->ptMultiplierLabel->SetText(ptMultString.str());
        this->ptMultiplierLabel->SetScale(MultiplierHUD::GetMultiplierScale(multiplierAmt));
        this->size = std::max<float>(this->ptMultiplierLabel->GetHeight(), this->ptMultiplierLabel->GetLastRasterWidth()) + 2 * BANG_TEXT_BORDER;

        if (!wasMultiplierVisibleBefore) {
            // Enter the creation state if the multiplier wasn't visible before
            this->SetCurrentAnimationState(CreationAnimation);
        }
        else {
            // In this case the multiplier has either grown or shrunk - for now we only allow
            // growing multipliers for now...
            assert(oldMultiplierAmt < multiplierAmt);
            this->SetCurrentAnimationState(IncreaseAnimation);
        }
    }
    else {
        // The multiplier is no longer visible - in such a case IT MUST HAVE BEEN VISIBLE BEFORE
        assert(wasMultiplierVisibleBefore);
        assert(multiplierAmt == 1);
        // The animation for the multiplier going away is set
        this->SetCurrentAnimationState(LeaveAnimation);
    }
}

void PointsHUD::MultiplierHUD::Draw(float rightMostX, float topMostY, double dT) {

    switch (this->currState) {
        case None:
            // Don't draw the multiplier if the current state is 'None'
            assert(this->currPtMultiplier < 2);
            return;

        case CreationAnimation:
            {
                bool done = this->scaleAnim.Tick(dT);
                done &= this->rgbaLabelAnim.Tick(dT);
                this->DrawHUD(rightMostX, topMostY);
                if (done) {
                    this->SetCurrentAnimationState(IdleAnimation);
                }   
            }
            break;

        case IncreaseAnimation:
            {
                bool done = this->scaleAnim.Tick(dT);
                done &= this->rgbaLabelAnim.Tick(dT);
                this->DrawHUD(rightMostX, topMostY);
                if (done) {
                    this->SetCurrentAnimationState(IdleAnimation);
                }
            }
            break;

        case IdleAnimation:
            this->rgbaLabelAnim.Tick(dT);
            this->DrawHUD(rightMostX, topMostY);
            break;

        case LeaveAnimation:
            {
                bool done = this->scaleAnim.Tick(dT);
                done &= this->rgbaLabelAnim.Tick(dT);
                this->DrawHUD(rightMostX, topMostY);
                if (done) {
                    this->SetCurrentAnimationState(None);
                }
            }
            break;

        default:
            assert(false);
            break;
    }
}

void PointsHUD::MultiplierHUD::DrawHUD(float rightMostX, float topMostY) {
    static const float MAX_SIZE = 70;

    float scaledSize = this->scaleAnim.GetInterpolantValue() * this->size;
    const ColourRGBA& labelColour = this->rgbaLabelAnim.GetInterpolantValue();
    
    this->ptMultiplierLabel->SetTopLeftCorner((rightMostX - MAX_SIZE) + 
        (MAX_SIZE - this->ptMultiplierLabel->GetLastRasterWidth()) / 2.0f - 4, 
        topMostY - (MAX_SIZE - this->ptMultiplierLabel->GetHeight())/2.0f + 8);

    float centerX = rightMostX - MAX_SIZE / 2.0f;
    float centerY = topMostY   - MAX_SIZE / 2.0f;

    // Draw the bang behind the multiplier label...
    this->multiplierBangTex->BindTexture();
    glColor4f(1,1,1,1);
    glPushMatrix();
    glTranslatef(centerX, centerY, 0.0f);
    glRotatef(-10.0f, 0, 0, 1.0f);
    glScalef(scaledSize, scaledSize, 1.0f);
    GeometryMaker::GetInstance()->DrawQuad();
    glPopMatrix();

    // Draw the multiplier text label...
    this->ptMultiplierLabel->SetColour(labelColour);
    this->ptMultiplierLabel->Draw(-8.0f);
}

void PointsHUD::MultiplierHUD::SetCurrentAnimationState(const AnimationState& state) {
    static const float MULTIPLIER_NUM_BASE_ALPHA = 1.0f;
    switch (state) {
        case None:
            assert(this->currPtMultiplier < 2);
            break;

        case CreationAnimation:
            {
                // Creating the multiplier - the size will increase a lot then go down to a normal size
                std::vector<double> timeVals;
                std::vector<float> scaleVals;

                timeVals.push_back(0.0);
                timeVals.push_back(0.8);
                timeVals.push_back(1.2);
                
                scaleVals.push_back(this->scaleAnim.GetInterpolantValue());
                scaleVals.push_back(1.2f);
                scaleVals.push_back(1.0f);

                this->scaleAnim.SetLerp(timeVals, scaleVals);

                // The colour of the label will shine once and go to its typical colour for the current
                // multiplier value
                std::vector<ColourRGBA> colourVals;
                colourVals.push_back(this->rgbaLabelAnim.GetInterpolantValue());
                colourVals.push_back(ColourRGBA(1,1,1,1));
                colourVals.push_back(ColourRGBA(GameViewConstants::GetInstance()->GetMultiplierColour(this->currPtMultiplier), MULTIPLIER_NUM_BASE_ALPHA));

                this->rgbaLabelAnim.SetLerp(timeVals, colourVals);
                this->rgbaLabelAnim.SetRepeat(false);

            }   
            break;

        case IncreaseAnimation:
            {
                // Creating the multiplier - the size will increase a lot then go down to a normal size
                std::vector<double> timeVals;
                std::vector<float> scaleVals;

                timeVals.push_back(0.0);
                timeVals.push_back(0.5);
                timeVals.push_back(0.8);
                
                scaleVals.push_back(this->scaleAnim.GetInterpolantValue());
                scaleVals.push_back(1.3f);
                scaleVals.push_back(1.0f);

                this->scaleAnim.SetLerp(timeVals, scaleVals);

                // The colour of the label will shine once and go to its typical colour for the current
                // multiplier value
                std::vector<ColourRGBA> colourVals;
                colourVals.push_back(this->rgbaLabelAnim.GetInterpolantValue());
                colourVals.push_back(ColourRGBA(1,1,1,1));
                colourVals.push_back(ColourRGBA(GameViewConstants::GetInstance()->GetMultiplierColour(this->currPtMultiplier), MULTIPLIER_NUM_BASE_ALPHA));

                this->rgbaLabelAnim.SetLerp(timeVals, colourVals);
                this->rgbaLabelAnim.SetRepeat(false);
            }
            break;

        case IdleAnimation:
            {
                // No scale changes while idleing
                this->scaleAnim.ClearLerp();
                this->scaleAnim.SetInterpolantValue(1.0f);
                
                std::vector<double> timeVals;
                timeVals.reserve(3);
                timeVals.push_back(0.0);
                timeVals.push_back(1.5);
                timeVals.push_back(3.0);
                
                ColourRGBA multiplyColour(GameViewConstants::GetInstance()->GetMultiplierColour(this->currPtMultiplier), MULTIPLIER_NUM_BASE_ALPHA);

                std::vector<ColourRGBA> colourVals;
                colourVals.reserve(timeVals.size());
                colourVals.push_back(multiplyColour);
                colourVals.push_back(ColourRGBA(1,1,1,MULTIPLIER_NUM_BASE_ALPHA));
                colourVals.push_back(multiplyColour);
                
                this->rgbaLabelAnim.SetLerp(timeVals, colourVals);
                this->rgbaLabelAnim.SetRepeat(true);
            }
            break;

        case LeaveAnimation:
            {
                // The scale will shrink back to zero as the multiplier disappears
                std::vector<double> timeVals;
                std::vector<float> scaleVals;

                timeVals.push_back(0.0);
                timeVals.push_back(0.5);
                timeVals.push_back(0.8);
                
                scaleVals.push_back(this->scaleAnim.GetInterpolantValue());
                scaleVals.push_back(1.1f);
                scaleVals.push_back(0.0f);
                
                this->scaleAnim.SetLerp(timeVals, scaleVals);

                // The colour of the label will fade out
                this->rgbaLabelAnim.SetLerp(0.7, ColourRGBA(this->rgbaLabelAnim.GetInterpolantValue().GetColour(),0));
                this->rgbaLabelAnim.SetRepeat(false);
            }
            break;

        default:
            assert(false);
            return;
    }

    this->currState = state;
}

float PointsHUD::MultiplierHUD::GetMultiplierScale(int multiplier) {
    switch (multiplier) {
        case 1:
            break;
        case 2:
            return 1.0f;
        case 3:
            return 1.1f;
        case 4:
            return 1.2f;
        default:
            assert(false);
            break;
    }
    return 0.0f;
}

// MultiplierGageHUD FUNCTIONS ********************************************************

const int PointsHUD::MultiplierGageHUD::MULTIPLIER_GAGE_SIZE = 110;
const int PointsHUD::MultiplierGageHUD::HALF_MULTIPLIER_GAGE_SIZE = MULTIPLIER_GAGE_SIZE/2;

PointsHUD::MultiplierGageHUD::MultiplierGageHUD() : 
currMultiplierCounterIdx(-1), multiplierGaugeOutlineTex(NULL), multiplierGaugeFillTex(NULL), alpha(1.0) {
    
    this->multiplierGaugeOutlineTex = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_MULTIPLIER_GAUGE_OUTLINE,
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->multiplierGaugeOutlineTex != NULL);

    this->multiplierGaugeFillTex = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_MULTIPLIER_GAUGE_FILL,
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->multiplierGaugeFillTex != NULL);

    Texture* tempTex = NULL;
    tempTex = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_MULTIPLIER_GAUGE_FILL_1,
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(tempTex != NULL);
    this->multiplierGageFillTexs.push_back(tempTex);
    tempTex = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_MULTIPLIER_GAUGE_FILL_2,
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(tempTex != NULL);
    this->multiplierGageFillTexs.push_back(tempTex);
    tempTex = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_MULTIPLIER_GAUGE_FILL_3,
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(tempTex != NULL);
    this->multiplierGageFillTexs.push_back(tempTex);
    tempTex = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_MULTIPLIER_GAUGE_FILL_4,
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(tempTex != NULL);
    this->multiplierGageFillTexs.push_back(tempTex);
    tempTex = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_MULTIPLIER_GAUGE_FILL_5,
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(tempTex != NULL);
    this->multiplierGageFillTexs.push_back(tempTex);
    tempTex = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_MULTIPLIER_GAUGE_FILL_6,
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(tempTex != NULL);
    this->multiplierGageFillTexs.push_back(tempTex);
    tempTex = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_MULTIPLIER_GAUGE_FILL_7,
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(tempTex != NULL);
    this->multiplierGageFillTexs.push_back(tempTex);
    tempTex = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_MULTIPLIER_GAUGE_FILL_8,
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(tempTex != NULL);
    this->multiplierGageFillTexs.push_back(tempTex);
    tempTex = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_MULTIPLIER_GAUGE_FILL_9,
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(tempTex != NULL);
    this->multiplierGageFillTexs.push_back(tempTex);
}

PointsHUD::MultiplierGageHUD::~MultiplierGageHUD() {
    bool success = false;
    
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->multiplierGaugeOutlineTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->multiplierGaugeFillTex);
    assert(success);

    for (std::vector<Texture*>::iterator iter = this->multiplierGageFillTexs.begin();
         iter != this->multiplierGageFillTexs.end(); ++iter) {
        Texture* currTex = *iter;
        assert(currTex != NULL);
        success = ResourceManager::GetInstance()->ReleaseTextureResource(currTex);
        assert(success);
    }
}

void PointsHUD::MultiplierGageHUD::Reinitialize() {
    this->currMultiplierCounterIdx = -1;
}

void PointsHUD::MultiplierGageHUD::Draw(float rightMostX, float topMostY, double dT) {
    UNUSED_PARAMETER(dT);

    glPushMatrix();
    glTranslatef(rightMostX - HALF_MULTIPLIER_GAGE_SIZE, topMostY - HALF_MULTIPLIER_GAGE_SIZE, 0.0f);
    glScalef(MULTIPLIER_GAGE_SIZE, MULTIPLIER_GAGE_SIZE, 1.0f);
    glColor4f(1, 1, 1, this->alpha);

    // Draw the background fill of the gauge
    this->multiplierGaugeFillTex->BindTexture();
    GeometryMaker::GetInstance()->DrawQuad();

    // Draw any fills on the multiplier gauge
    if (this->currMultiplierCounterIdx >= 0) {
        this->multiplierGageFillTexs[this->currMultiplierCounterIdx]->BindTexture();
        GeometryMaker::GetInstance()->DrawQuad();
    }

    // Draw the outline of the gauge
    this->multiplierGaugeOutlineTex->BindTexture();
    GeometryMaker::GetInstance()->DrawQuad();

    glPopMatrix();
}