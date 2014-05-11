/**
 * CreditsDisplayState.cpp
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

#include "CreditsDisplayState.h"
#include "GameFontAssetsManager.h"
#include "GameDisplay.h"
#include "MainMenuDisplayState.h"
#include "MenuBackgroundRenderer.h"

#include "../ResourceManager.h"

const double CreditsDisplayState::FADE_OUT_TIME_ON_BUTTON_PRESS = 1.0;

const float CreditsDisplayState::CREDIT_SCROLLING_VELOCITY = 24.0f;

const char* CreditsDisplayState::CORE_CREDITS_1_TITLE_TEXT  = "Design, Art, and Programming By";
const char* CreditsDisplayState::CORE_CREDITS_1_NAMES_TEXT  = "Callum Hay";
const char* CreditsDisplayState::CORE_CREDITS_2_TITLE_TEXT  = "Music By";
const char* CreditsDisplayState::CORE_CREDITS_2_NAMES_TEXT  = "Adam Axbey";
const char* CreditsDisplayState::CORE_CREDITS_SPECIAL_THANKS_TITLE_TEXT = "Special Thanks To";
const char* CreditsDisplayState::CORE_CREDITS_SPECIAL_THANKS_NAMES_TEXT = "Sara Vinten\nMikhail St-Denis\nBlake Withers\nAndre Lowy";

const char* CreditsDisplayState::FONT_CREDITS_CATEGORY_TITLE_TEXT = "- Font Credits -";
const char* CreditsDisplayState::FONT_CREDITS_1_TITLE_TEXT = "\"ExplosionBoom\" Font";
const char* CreditsDisplayState::FONT_CREDITS_1_NAMES_TEXT = "Created by Callum Hay\nInspired by \"Badaboom\", a font created by Blambot (www.blambot.com)";
const char* CreditsDisplayState::FONT_CREDITS_2_TITLE_TEXT = "Boris Black Bloxx Font";
const char* CreditsDisplayState::FONT_CREDITS_2_NAMES_TEXT = "Created by Manfred Klein (www.manfredklein.de)";
const char* CreditsDisplayState::FONT_CREDITS_3_TITLE_TEXT = "Double Feature Font";
const char* CreditsDisplayState::FONT_CREDITS_3_NAMES_TEXT = "Created by David Shetterly";
const char* CreditsDisplayState::FONT_CREDITS_4_TITLE_TEXT = "ElectricZap Font";
const char* CreditsDisplayState::FONT_CREDITS_4_NAMES_TEXT = "Created by Callum Hay";

const char* CreditsDisplayState::THIRD_PARTY_SW_CATEGORY_TITLE_TEXT = "- Third Party Software Credits -";
const char* CreditsDisplayState::THIRD_PARTY_SW_1_TITLE_TEXT = "OpenGL (www.opengl.org)";
const char* CreditsDisplayState::THIRD_PARTY_SW_1_NAMES_TEXT = "Registered Trademark of SGI, part of The Khronos Group consortium";
const char* CreditsDisplayState::THIRD_PARTY_SW_2_TITLE_TEXT = "NVIDIA Cg Toolkit, CgFx (developer.nvidia.com/cg-toolkit)";
const char* CreditsDisplayState::THIRD_PARTY_SW_2_NAMES_TEXT = "Created by NVIDIA Corporation";
const char* CreditsDisplayState::THIRD_PARTY_SW_3_TITLE_TEXT = "irrKlang Pro Indie Sound Engine (www.ambiera.com/irrklang)";
const char* CreditsDisplayState::THIRD_PARTY_SW_3_NAMES_TEXT = "Created by Nikolaus Gebhardt / Ambiera";
const char* CreditsDisplayState::THIRD_PARTY_SW_4_TITLE_TEXT = "SDL (Simple DirectMedia Layer) v1.2 (www.libsdl.org)";
const char* CreditsDisplayState::THIRD_PARTY_SW_4_NAMES_TEXT = "Licensed under the GNU LGPL";
const char* CreditsDisplayState::THIRD_PARTY_SW_5_TITLE_TEXT = "FreeType Font Rendering Library (www.freetype.org)";
const char* CreditsDisplayState::THIRD_PARTY_SW_5_NAMES_TEXT = "Created by David Turner, Robert Wilhelm, and Werner Lemberg\nLicensed under the FreeType Project License";
const char* CreditsDisplayState::THIRD_PARTY_SW_6_TITLE_TEXT = "Microsoft XInput Library (msdn.microsoft.com)";
const char* CreditsDisplayState::THIRD_PARTY_SW_6_NAMES_TEXT = "Created by Microsoft Corporation";
const char* CreditsDisplayState::THIRD_PARTY_SW_7_TITLE_TEXT = "PhysicsFS (icculus.org/physfs)";
const char* CreditsDisplayState::THIRD_PARTY_SW_7_NAMES_TEXT = "Created by Ryan C. Gordon, Licensed under the ZLib License";
const char* CreditsDisplayState::THIRD_PARTY_SW_8_TITLE_TEXT = "DevIL - A full featured cross-platform Image Library (openil.sourceforge.net)";
const char* CreditsDisplayState::THIRD_PARTY_SW_8_NAMES_TEXT = "Licensed under the GNU LGPL";
const char* CreditsDisplayState::THIRD_PARTY_SW_9_TITLE_TEXT = "Mersenne Twister Pseudo-random Number Generator";
const char* CreditsDisplayState::THIRD_PARTY_SW_9_NAMES_TEXT = "Coded by Takuji Nishimura and Makoto Matsumoto\nPorted to C++ by Jasper Bedaux (www.bedaux.net/mtrand)";

CreditsDisplayState::CreditsDisplayState(GameDisplay* display) : DisplayState(display),
exitState(false), bbbLogoTex(NULL) {
   
    CreditLabel::SetCreditsYVelocity(0.0f);

    this->bbbLogoTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_BBB_LOGO, Texture::Trilinear));
    assert(this->bbbLogoTex != NULL);

    static const double TIME_BETWEEN_LABEL_FADEINS = 1.0;
    double lastFadeInTime = 0.0;
    
    this->logoFadeAnim.SetLerp(lastFadeInTime, lastFadeInTime + TIME_BETWEEN_LABEL_FADEINS, 0.0f, 1.0f);
    this->logoFadeAnim.SetRepeat(false);
    lastFadeInTime = this->logoFadeAnim.GetFinalTime();
    
    float scalingFactor    = GameDisplay::GetTextScalingFactor();
    float gapBetweenLabels = GAP_BETWEEN_CREDITS_LABELS * scalingFactor;
    float extraTopGap = 50 * scalingFactor;

    const float MAX_LABEL_WIDTH = Camera::GetWindowWidth()*0.75f;

    // Add the credits...
    CreditLabel* creditLabel1       = NULL;
    CreditLabel* creditLabel2       = NULL;
    CreditLabel* specialThanksLabel = NULL;

    {
        CreditLabel* tempCreditLabel = NULL;

        tempCreditLabel = new CreditLabel(CORE_CREDITS_1_TITLE_TEXT, CORE_CREDITS_1_NAMES_TEXT, MAX_LABEL_WIDTH);
        tempCreditLabel->StartFadeIn(lastFadeInTime, lastFadeInTime + TIME_BETWEEN_LABEL_FADEINS);
        creditLabel1 = tempCreditLabel;
        lastFadeInTime += TIME_BETWEEN_LABEL_FADEINS;
        this->creditLabels.push_front(tempCreditLabel);

        tempCreditLabel = new CreditLabel(CORE_CREDITS_2_TITLE_TEXT, CORE_CREDITS_2_NAMES_TEXT, MAX_LABEL_WIDTH);
        tempCreditLabel->StartFadeIn(lastFadeInTime, lastFadeInTime + TIME_BETWEEN_LABEL_FADEINS);
        creditLabel2 = tempCreditLabel;
        lastFadeInTime += TIME_BETWEEN_LABEL_FADEINS;
        this->creditLabels.push_front(tempCreditLabel);

        tempCreditLabel = new CreditLabel(CORE_CREDITS_SPECIAL_THANKS_TITLE_TEXT, CORE_CREDITS_SPECIAL_THANKS_NAMES_TEXT, MAX_LABEL_WIDTH);
        tempCreditLabel->StartFadeIn(lastFadeInTime, lastFadeInTime + TIME_BETWEEN_LABEL_FADEINS);
        specialThanksLabel = tempCreditLabel;
        lastFadeInTime += TIME_BETWEEN_LABEL_FADEINS;
        this->creditLabels.push_front(tempCreditLabel);

        this->countdownToScrollCredits = lastFadeInTime + 1.0;
    }

    float verticalGap = (Camera::GetWindowHeight() - creditLabel1->GetHeight() - 
        creditLabel2->GetHeight() - specialThanksLabel->GetHeight() - 2*gapBetweenLabels) / 2.0f;
    float currYPos = Camera::GetWindowHeight() - verticalGap - extraTopGap;

    this->topYCoordinateForFadeOut = (Camera::GetWindowHeight() - verticalGap - 0.75f*extraTopGap);
    
    creditLabel1->SetYPos(currYPos);
    currYPos -= (gapBetweenLabels + creditLabel1->GetHeight());
    creditLabel2->SetYPos(currYPos);
    currYPos -= (gapBetweenLabels + creditLabel2->GetHeight());
    specialThanksLabel->SetYPos(currYPos);
    currYPos -= (gapBetweenLabels + specialThanksLabel->GetHeight());

    this->bottomYCoordinateForFadeIn = currYPos;
    currYPos -= gapBetweenLabels;

    {
        const TextureFontSet* categoryFont = GameFontAssetsManager::GetInstance()->GetFont(
            GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium);
        const TextureFontSet* defaultFont = GameFontAssetsManager::GetInstance()->GetFont(
            GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small);

#define ADD_CREDIT_LABEL(titleFont, titleText, nameText) \
    tempCreditLabel = new CreditLabel(titleFont, titleText, nameText, MAX_LABEL_WIDTH); \
    tempCreditLabel->SetYPos(currYPos); \
    currYPos -= (0.5f*gapBetweenLabels + tempCreditLabel->GetHeight()); \
    this->creditLabels.push_front(tempCreditLabel)

        CreditLabel* tempCreditLabel = NULL;
       
        ADD_CREDIT_LABEL(categoryFont, FONT_CREDITS_CATEGORY_TITLE_TEXT, "");
        ADD_CREDIT_LABEL(GameFontAssetsManager::GetInstance()->GetFont(
            GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium), FONT_CREDITS_1_TITLE_TEXT, FONT_CREDITS_1_NAMES_TEXT);
        ADD_CREDIT_LABEL(GameFontAssetsManager::GetInstance()->GetFont(
            GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), FONT_CREDITS_2_TITLE_TEXT, FONT_CREDITS_2_NAMES_TEXT);
        ADD_CREDIT_LABEL(GameFontAssetsManager::GetInstance()->GetFont(
            GameFontAssetsManager::SadBadGoo, GameFontAssetsManager::Small), FONT_CREDITS_3_TITLE_TEXT, FONT_CREDITS_3_NAMES_TEXT);
        ADD_CREDIT_LABEL(GameFontAssetsManager::GetInstance()->GetFont(
            GameFontAssetsManager::ElectricZap, GameFontAssetsManager::Small), FONT_CREDITS_4_TITLE_TEXT, FONT_CREDITS_4_NAMES_TEXT);

        currYPos -= gapBetweenLabels;
        ADD_CREDIT_LABEL(categoryFont, THIRD_PARTY_SW_CATEGORY_TITLE_TEXT, "");
        ADD_CREDIT_LABEL(defaultFont, THIRD_PARTY_SW_1_TITLE_TEXT, THIRD_PARTY_SW_1_NAMES_TEXT);
        ADD_CREDIT_LABEL(defaultFont, THIRD_PARTY_SW_2_TITLE_TEXT, THIRD_PARTY_SW_2_NAMES_TEXT);
        ADD_CREDIT_LABEL(defaultFont, THIRD_PARTY_SW_3_TITLE_TEXT, THIRD_PARTY_SW_3_NAMES_TEXT);
        ADD_CREDIT_LABEL(defaultFont, THIRD_PARTY_SW_4_TITLE_TEXT, THIRD_PARTY_SW_4_NAMES_TEXT);
        ADD_CREDIT_LABEL(defaultFont, THIRD_PARTY_SW_5_TITLE_TEXT, THIRD_PARTY_SW_5_NAMES_TEXT);
        ADD_CREDIT_LABEL(defaultFont, THIRD_PARTY_SW_6_TITLE_TEXT, THIRD_PARTY_SW_6_NAMES_TEXT);
        ADD_CREDIT_LABEL(defaultFont, THIRD_PARTY_SW_7_TITLE_TEXT, THIRD_PARTY_SW_7_NAMES_TEXT);
        ADD_CREDIT_LABEL(defaultFont, THIRD_PARTY_SW_8_TITLE_TEXT, THIRD_PARTY_SW_8_NAMES_TEXT);
        ADD_CREDIT_LABEL(defaultFont, THIRD_PARTY_SW_9_TITLE_TEXT, THIRD_PARTY_SW_9_NAMES_TEXT);


#undef ADD_CREDIT_LABEL  
    }
}

CreditsDisplayState::~CreditsDisplayState() {
    bool success = false;
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->bbbLogoTex);
    assert(success);
    UNUSED_VARIABLE(success);

    for (CreditLabelContainerIter iter = this->creditLabels.begin(); iter != this->creditLabels.end(); ++iter) {
        CreditLabel* label = *iter;
        delete label;
        label = NULL;
    }
    this->creditLabels.clear();
}

void CreditsDisplayState::RenderFrame(double dT) {
    const Camera& camera = this->display->GetCamera();
    MenuBackgroundRenderer* bgRenderer = this->display->GetMenuBGRenderer();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw the background...
    bgRenderer->DrawBG(camera);

    this->logoFadeAnim.Tick(dT);
    float logoAlpha = this->logoFadeAnim.GetInterpolantValue();

    
    if (this->countdownToScrollCredits <= 0.0) {
        CreditLabel::SetCreditsYVelocity(CREDIT_SCROLLING_VELOCITY);
    }
    else {
        this->countdownToScrollCredits -= dT;
    }

    for (CreditLabelContainerIter iter = this->creditLabels.begin(); iter != this->creditLabels.end();) {
        CreditLabel* currLabel = *iter;
        currLabel->Draw(this, dT);
        if (currLabel->GetIsNeverToBeVisibleAgain()) {
            delete currLabel;
            currLabel = NULL;
            iter = this->creditLabels.erase(iter);
        }
        else {
            ++iter;
        }
    }

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_LINE_BIT);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Camera::PushWindowCoords();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    float scaleFactor = GameDisplay::GetTextScalingFactor();
    const float VERT_DIST_FROM_EDGE = 20 * scaleFactor;
    
    float logoXSize = 512.0f * scaleFactor;
    float logoYSize = 256.0f * scaleFactor;
    float logoXSizeDiv2 = logoXSize / 2.0f;
    float logoYSizeDiv2 = logoYSize / 2.0f;

    glTranslatef((Camera::GetWindowWidth() - logoXSize) / 2.0f + logoXSizeDiv2, Camera::GetWindowHeight() - logoYSizeDiv2 - VERT_DIST_FROM_EDGE, 0);
    glScalef(logoXSize, logoYSize, 1.0f);
    
    glColor4f(1,1,1,logoAlpha);
    this->bbbLogoTex->BindTexture();
    GeometryMaker::GetInstance()->DrawQuad();
    this->bbbLogoTex->UnbindTexture();

    glPopMatrix();
    Camera::PopWindowCoords();

    glPopAttrib();

    if (!this->exitState && this->creditLabels.empty()) {
        this->logoFadeAnim.SetLerp(FADE_OUT_TIME_ON_BUTTON_PRESS, 0.0f);
        this->exitState = true;
    }

    if (this->exitState && this->logoFadeAnim.GetInterpolantValue() <= 0.0f) {
        this->display->SetCurrentState(DisplayState::BuildDisplayStateFromType(DisplayState::MainMenu, 
            DisplayStateInfo::BuildMainMenuInfo(true), this->display));
        return;
    }
}

void CreditsDisplayState::ButtonPressed(const GameControl::ActionButton& pressedButton,
                                        const GameControl::ActionMagnitude& magnitude) {
    UNUSED_PARAMETER(pressedButton);
    UNUSED_PARAMETER(magnitude);

    if (!this->exitState) {

        this->exitState = true;

        for (CreditLabelContainerIter iter = this->creditLabels.begin(); iter != this->creditLabels.end(); ++iter) {
            CreditLabel* currLabel = *iter;
            currLabel->StartFadeOut(0.0, FADE_OUT_TIME_ON_BUTTON_PRESS);
        }
        this->logoFadeAnim.SetLerp(FADE_OUT_TIME_ON_BUTTON_PRESS, 0.0f);
    }
}

float CreditsDisplayState::CreditLabel::creditsYVelocity = 0.0f;

CreditsDisplayState::CreditLabel::CreditLabel(const char* title, const char* names, float maxWidth) :
titleLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), maxWidth, title),
namesLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), maxWidth, names),
currState(NotVisibleState), currYPos(0.0f) {

    this->titleLabel.SetAlignment(TextLabel2DFixedWidth::CenterAligned);
    this->titleLabel.SetScale(GameDisplay::GetTextScalingFactor() * 1.10f, false);
    this->titleLabel.SetColour(Colour(0.8f, 0.8f, 0.8f));

    this->namesLabel.SetAlignment(TextLabel2DFixedWidth::CenterAligned);
    this->namesLabel.SetScale(GameDisplay::GetTextScalingFactor(), false);
    this->namesLabel.SetColour(Colour(1,1,1));

    this->SetState(NotVisibleState);
}

CreditsDisplayState::CreditLabel::CreditLabel(const TextureFontSet* titleFont, const char* title,
                                              const char* names, float maxWidth) :
titleLabel(titleFont, maxWidth, title), currState(NotVisibleState), currYPos(0.0f),
namesLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), maxWidth, names) {

    this->titleLabel.SetAlignment(TextLabel2DFixedWidth::CenterAligned);
    this->titleLabel.SetScale(GameDisplay::GetTextScalingFactor() * 1.10f, false);
    this->titleLabel.SetColour(Colour(0.8f, 0.8f, 0.8f));

    this->namesLabel.SetAlignment(TextLabel2DFixedWidth::CenterAligned);
    this->namesLabel.SetScale(GameDisplay::GetTextScalingFactor(), false);
    this->namesLabel.SetColour(Colour(1,1,1));

    this->SetState(NotVisibleState);
}

CreditsDisplayState::CreditLabel::~CreditLabel() {
}

void CreditsDisplayState::CreditLabel::StartFadeIn(double startTime, double endTime) {
    if (this->currState != NotVisibleState) {
        return;
    }

    this->fadeAnim.SetLerp(startTime, endTime, this->titleLabel.GetAlpha(), 1.0f);
    this->fadeAnim.SetInterpolantValue(this->titleLabel.GetAlpha());
    this->fadeAnim.SetRepeat(false);

    this->SetState(FadingInState);
}

void CreditsDisplayState::CreditLabel::StartFadeOut(double startTime, double endTime) {
    if (this->currState != VisibleState && this->currState != FadingInState) {
        return;
    }

    this->fadeAnim.SetLerp(startTime, endTime, this->titleLabel.GetAlpha(), 0.0f);
    this->fadeAnim.SetInterpolantValue(this->titleLabel.GetAlpha());
    this->fadeAnim.SetRepeat(false);

    this->SetState(FadingOutState);
}

void CreditsDisplayState::CreditLabel::SetYPos(float yPos) {

    this->titleLabel.SetTopLeftCorner((Camera::GetWindowWidth() - this->titleLabel.GetWidth()) / 2.0f, yPos);
    this->namesLabel.SetTopLeftCorner((Camera::GetWindowWidth() - this->namesLabel.GetWidth()) / 2.0f, 
        yPos - this->titleLabel.GetHeight() - this->GetTitleToNameGap());

    this->currYPos = yPos;
}

void CreditsDisplayState::CreditLabel::Draw(CreditsDisplayState* state, double dT) {

    switch (this->currState) {

        case NotVisibleState:
            this->SetYPos(this->currYPos + CreditLabel::creditsYVelocity * dT);

            // If the y coordinate is above a certain amount then we start fading in the credit...
            if (this->currYPos >= state->GetBottomYCoordinateToStartFadingInAt()) {
                this->StartFadeIn(0.0, 2.0);
            }
            return;

        case FadingInState: {
            this->SetYPos(this->currYPos + CreditLabel::creditsYVelocity * dT);

            bool finishedFadingIn = this->fadeAnim.Tick(dT);
            float currAlpha = this->fadeAnim.GetInterpolantValue();
            if (finishedFadingIn) {
                this->SetState(VisibleState);
                currAlpha = 1.0f;
            }
            this->SetAlpha(currAlpha);
            this->titleLabel.Draw();
            this->namesLabel.Draw();
            break;
        }

        case VisibleState:
            this->SetYPos(this->currYPos + CreditLabel::creditsYVelocity * dT);

            // If the y coordinate is above a certain amount then we start fading out the credit...
            if (this->currYPos >= state->GetTopYCoordinateToStartFadingOutAt()) {
                this->StartFadeOut(0.0, 2.0);
            }

            this->titleLabel.Draw();
            this->namesLabel.Draw();
            break;

        case FadingOutState: {
            this->SetYPos(this->currYPos + CreditLabel::creditsYVelocity * dT);

            bool finishedFadingOut = this->fadeAnim.Tick(dT);
            float currAlpha = this->fadeAnim.GetInterpolantValue();
            if (finishedFadingOut) {
                this->SetState(NeverToBeVisibleAgain);
                currAlpha = 0.0f;
            }
            this->SetAlpha(currAlpha);
            this->titleLabel.Draw();
            this->namesLabel.Draw();
            break;
        }

        case NeverToBeVisibleAgain:
            break;

        default:
            assert(false);
            return;
    }
}

float CreditsDisplayState::CreditLabel::GetTitleToNameGap() const {
    return GameDisplay::GetTextScalingFactor() * TITLE_NAMES_V_GAP;
}

float CreditsDisplayState::CreditLabel::GetHeight() const {
    return this->titleLabel.GetHeight() + this->GetTitleToNameGap() + this->namesLabel.GetHeight();
}

void CreditsDisplayState::CreditLabel::SetState(State newState) {
    switch (newState) {

        case NotVisibleState:
            this->SetAlpha(0.0f);
            break;

        case FadingInState:
            break;

        case VisibleState:
            this->SetAlpha(0.0f);
            break;

        case FadingOutState:
            break;

        case NeverToBeVisibleAgain:
            this->SetAlpha(0.0f);
            this->SetYPos(Camera::GetWindowHeight() + 2*this->GetHeight());
            break;

        default:
            assert(false);
            return;
    }

    this->currState = newState;
}

void CreditsDisplayState::CreditLabel::SetAlpha(float alpha) {
    this->titleLabel.SetAlpha(alpha);
    this->namesLabel.SetAlpha(alpha);
}
