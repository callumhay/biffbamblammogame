/**
 * SelectLevelMenuState.cpp
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

#include "SelectLevelMenuState.h"
#include "SelectWorldMenuState.h"
#include "KeyboardHelperLabel.h"
#include "GameFontAssetsManager.h"
#include "GameViewConstants.h"
#include "GameDisplay.h"
#include "MenuBackgroundRenderer.h"
#include "PersistentTextureManager.h"

#include "../BlammoEngine/TextLabel.h"
#include "../BlammoEngine/FBObj.h"
#include "../BlammoEngine/StringHelper.h"

#include "../GameModel/GameModel.h"
#include "../GameModel/GameWorld.h"
#include "../GameModel/GameProgressIO.h"

static const float BORDER_GAP = 10;

SelectLevelMenuState::SelectLevelMenuState(GameDisplay* display, const DisplayStateInfo& info, SoundID bgSoundLoopID) : 
DisplayState(display), worldLabel(NULL), bgSoundLoopID(bgSoundLoopID), world(display->GetModel()->GetWorldByIndex(info.GetWorldSelectionIndex())), 
pressEscAlphaAnim(0.0f), goBackToWorldSelectMenu(false), goToStartLevel(false), goBackMenuMoveAnim(0.0f), goBackMenuAlphaAnim(1.0f), starTexture(NULL),
bossIconTexture(NULL), starGlowTexture(NULL), arrowTexture(NULL), nextPgArrowEmitter(NULL), prevPgArrowEmitter(NULL), snapTexture(NULL),
autoUnlockAnimCountdown(0.25), playAutoStarUnlockAnim(false), playAutoBasicUnlockAnim(false), totalNumGameStarsLabel(NULL), totalLabel(NULL),
padlockTexture(NULL), padlockBrokenLeftTexture(NULL), padlockBrokenRightTexture(NULL),
freezePlayerInput(false), explosionTex(NULL), sphereNormalsTex(NULL), menuFBO(NULL), postMenuFBObj(NULL),
selectionAlphaOrangeAnim(0.0f), selectionAlphaYellowAnim(0.0f), selectionBorderAddAnim(0.0f), totalNumWorldStarsLabel(NULL),
particleFader(1, 0), particleMediumGrowth(1.0f, 2.0f), particleSuperGrowth(1.0f, 10.0f), particleSmallGrowth(1.0f, 1.3f),
lockBreakGravityEffector(Vector3D(0, -125, 0)),
particleFireFastColourFader(ColourRGBA(1.0f, 1.0f, 0.1f, 0.8f), ColourRGBA(0.5f, 0.0f, 0.0f, 0.0f)), levelWasUnlockedViaStarCost(false),
smokeRotatorCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.25f, ESPParticleRotateEffector::CLOCKWISE), 
smokeRotatorCCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.25f, ESPParticleRotateEffector::COUNTER_CLOCKWISE),
fiveStarFGSparkleEmitter(NULL), fiveStarBGGlowEmitter(NULL) {

    if (this->bgSoundLoopID == INVALID_SOUND_ID) {
        this->bgSoundLoopID = this->display->GetSound()->PlaySound(GameSound::WorldMenuBackgroundLoop, true);
    }

    float scalingFactor = this->display->GetTextScalingFactor();

    this->starTexture = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_STAR, Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->starTexture != NULL);
    this->starTexture->SetWrapParams(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    this->bossIconTexture = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_BOSS_ICON, Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->bossIconTexture != NULL);
    this->bossIconTexture->SetWrapParams(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    this->arrowTexture = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_UP_ARROW, Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->arrowTexture != NULL);
    this->arrowTexture->SetWrapParams(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    this->starGlowTexture = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_CIRCLE_GRADIENT, Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->starGlowTexture != NULL);
    this->starGlowTexture->SetWrapParams(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    this->padlockTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
    GameViewConstants::GetInstance()->TEXTURE_PADLOCK, Texture::Trilinear));
    assert(this->padlockTexture != NULL);
    this->padlockTexture->SetWrapParams(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    this->padlockBrokenLeftTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_PADLOCK_BROKEN_LEFT, Texture::Trilinear));
    assert(this->padlockBrokenLeftTexture != NULL);
    this->padlockBrokenLeftTexture->SetWrapParams(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    this->padlockBrokenRightTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_PADLOCK_BROKEN_RIGHT, Texture::Trilinear));
    assert(this->padlockBrokenRightTexture != NULL);
    this->padlockBrokenRightTexture->SetWrapParams(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    this->snapTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_BANG1, Texture::Trilinear));
    assert(this->snapTexture != NULL);

    this->sphereNormalsTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_SPHERE_NORMALS, Texture::Trilinear));
    assert(this->sphereNormalsTex != NULL);

    this->explosionTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_BUBBLY_EXPLOSION, Texture::Trilinear));
    assert(this->explosionTex != NULL);

    // Initialize the smoke textures...
    if (this->smokeTextures.empty()) {
        this->smokeTextures.reserve(6);
        Texture2D* temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE1, Texture::Trilinear));
        assert(temp != NULL);
        this->smokeTextures.push_back(temp);
        temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE2, Texture::Trilinear));
        assert(temp != NULL);
        this->smokeTextures.push_back(temp);
        temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE3, Texture::Trilinear));
        assert(temp != NULL);
        this->smokeTextures.push_back(temp);
        temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE4, Texture::Trilinear));
        assert(temp != NULL);
        this->smokeTextures.push_back(temp);
        temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE5, Texture::Trilinear));
        assert(temp != NULL);
        this->smokeTextures.push_back(temp);
        temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE6, Texture::Trilinear));
        assert(temp != NULL);
        this->smokeTextures.push_back(temp);	
    }

    this->fiveStarFGSparkleEmitter = new ESPPointEmitter();
    this->fiveStarFGSparkleEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
    this->fiveStarFGSparkleEmitter->SetInitialSpd(ESPInterval(0.0f, 0.0f));
    this->fiveStarFGSparkleEmitter->SetParticleLife(ESPParticle::INFINITE_PARTICLE_LIFETIME);
    this->fiveStarFGSparkleEmitter->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
    this->fiveStarFGSparkleEmitter->SetParticleAlignment(ESP::NoAlignment);
    this->fiveStarFGSparkleEmitter->SetParticleRotation(ESPInterval(0));
    this->fiveStarFGSparkleEmitter->SetParticleColour(ESPInterval(1), ESPInterval(1), 
        ESPInterval(0.8f), ESPInterval(0.85f));
    this->fiveStarFGSparkleEmitter->SetParticleSize(ESPInterval(1));
    this->fiveStarFGSparkleEmitter->AddCopiedEffector(ESPParticleRotateEffector(45.0f, ESPParticleRotateEffector::CLOCKWISE));
    this->fiveStarFGSparkleEmitter->AddCopiedEffector(ESPParticleScaleEffector(ScaleEffect(1.0f, 1.5f)));
    this->fiveStarFGSparkleEmitter->SetParticles(1, 
        PersistentTextureManager::GetInstance()->GetLoadedTexture(GameViewConstants::GetInstance()->TEXTURE_SPARKLE));

    const Colour STAR_COLOUR_BG = 1.2f * GameViewConstants::GetInstance()->ACTIVE_POINT_STAR_COLOUR;
    this->fiveStarBGGlowEmitter = new ESPPointEmitter();
    this->fiveStarBGGlowEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
    this->fiveStarBGGlowEmitter->SetInitialSpd(ESPInterval(0.0f, 0.0f));
    this->fiveStarBGGlowEmitter->SetParticleLife(ESPParticle::INFINITE_PARTICLE_LIFETIME);
    this->fiveStarBGGlowEmitter->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
    this->fiveStarBGGlowEmitter->SetParticleAlignment(ESP::NoAlignment);
    this->fiveStarBGGlowEmitter->SetParticleRotation(ESPInterval(0));
    this->fiveStarBGGlowEmitter->SetParticleColour(ESPInterval(STAR_COLOUR_BG.R()), ESPInterval(STAR_COLOUR_BG.G()), ESPInterval(STAR_COLOUR_BG.B()), ESPInterval(1));
    this->fiveStarBGGlowEmitter->SetParticleSize(ESPInterval(1));
    this->fiveStarBGGlowEmitter->AddCopiedEffector(ESPParticleRotateEffector(45.0f, ESPParticleRotateEffector::CLOCKWISE));
    this->fiveStarBGGlowEmitter->AddCopiedEffector(ESPParticleScaleEffector(ScaleEffect(1.0f, 1.2f)));
    this->fiveStarBGGlowEmitter->SetParticles(1, static_cast<Texture2D*>(this->starGlowTexture));

    std::vector<ColourRGBA> arrowColours;
    arrowColours.reserve(3);
    arrowColours.push_back(ColourRGBA(0,1,0,0));
    arrowColours.push_back(ColourRGBA(0,1,0,1));
    arrowColours.push_back(ColourRGBA(0,1,0,0));
    this->nextArrowFader.SetColours(arrowColours);

    static const int NUM_ARROWS = 10;
    static const float ARROW_LIFE_TIME_MIN = 2.2f;
    static const float ARROW_LIFE_TIME_MAX = 3.0f;
    static const float ARROW_SPAWN_DELTA   = 0.45f;
    static const float ARROW_WIDTH  = 25;
    static const float ARROW_HEIGHT = 35;
    static const float ARROW_SPD    = 30.0f;

    this->nextPgArrowEmitter = new ESPVolumeEmitter();
	this->nextPgArrowEmitter->SetSpawnDelta(ESPInterval(ARROW_SPAWN_DELTA));
	this->nextPgArrowEmitter->SetInitialSpd(ESPInterval(ARROW_SPD));
	this->nextPgArrowEmitter->SetParticleLife(ESPInterval(ARROW_LIFE_TIME_MIN, ARROW_LIFE_TIME_MAX));
    this->nextPgArrowEmitter->SetNumParticleLives(ESPParticle::INFINITE_PARTICLE_LIVES);
	this->nextPgArrowEmitter->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	this->nextPgArrowEmitter->SetParticleAlignment(ESP::ScreenAligned);
	this->nextPgArrowEmitter->SetParticleRotation(ESPInterval(90));
    this->nextPgArrowEmitter->SetEmitDirection(Vector3D(1,0,0));
	this->nextPgArrowEmitter->SetParticleSize(ESPInterval(ARROW_WIDTH), ESPInterval(ARROW_HEIGHT));
    this->nextPgArrowEmitter->SetParticleColour(ESPInterval(0), ESPInterval(1), ESPInterval(0), ESPInterval(1));
    this->nextPgArrowEmitter->SetParticles(NUM_ARROWS, static_cast<Texture2D*>(this->arrowTexture));
    this->nextPgArrowEmitter->AddEffector(&nextArrowFader);

    this->prevPgArrowEmitter = new ESPVolumeEmitter();
	this->prevPgArrowEmitter->SetSpawnDelta(ESPInterval(ARROW_SPAWN_DELTA));
	this->prevPgArrowEmitter->SetInitialSpd(ESPInterval(ARROW_SPD));
	this->prevPgArrowEmitter->SetParticleLife(ESPInterval(ARROW_LIFE_TIME_MIN, ARROW_LIFE_TIME_MAX));
    this->prevPgArrowEmitter->SetNumParticleLives(ESPParticle::INFINITE_PARTICLE_LIVES);
	this->prevPgArrowEmitter->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	this->prevPgArrowEmitter->SetParticleAlignment(ESP::ScreenAligned);
	this->prevPgArrowEmitter->SetParticleRotation(ESPInterval(-90));
    this->prevPgArrowEmitter->SetEmitDirection(Vector3D(-1,0,0));
	this->prevPgArrowEmitter->SetParticleSize(ESPInterval(ARROW_WIDTH), ESPInterval(ARROW_HEIGHT));
    this->prevPgArrowEmitter->SetParticleColour(ESPInterval(0), ESPInterval(1), ESPInterval(0), ESPInterval(1));
    this->prevPgArrowEmitter->SetParticles(NUM_ARROWS, static_cast<Texture2D*>(this->arrowTexture));
    this->prevPgArrowEmitter->AddEffector(&nextArrowFader);

    // Build a framebuffer object for the menu
	this->menuFBO = new FBObj(Camera::GetWindowWidth(), Camera::GetWindowHeight(), Texture::Nearest, FBObj::NoAttachment);
    this->postMenuFBObj = new FBObj(Camera::GetWindowWidth(), Camera::GetWindowHeight(), Texture::Nearest, FBObj::NoAttachment);

    Colour titleColour(0.4f, 0.6f, 0.8f); // Steel blue
    std::stringstream worldLabelTxt;
    worldLabelTxt << this->world->GetName();
    this->worldLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, 
        GameFontAssetsManager::Huge), worldLabelTxt.str());
    this->worldLabel->SetColour(titleColour);
    this->worldLabel->SetDropShadow(Colour(1,1,1), 0.05f);
    this->worldLabel->SetScale(1.4f*scalingFactor);

    this->keyEscLabel = new KeyboardHelperLabel(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium, "Press", "Esc", "to Return");
    this->keyEscLabel->SetBeforeAndAfterTextColour(Colour(1,1,1));
    this->keyEscLabel->SetScale(scalingFactor);

    this->totalLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(
        GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Big), "Total");
    this->totalLabel->SetColour(Colour(1,1,1));
    this->totalLabel->SetScale(scalingFactor);

    {
        std::vector<float> alphaVals;
        alphaVals.push_back(0.15f);
        alphaVals.push_back(1.0f);
        alphaVals.push_back(0.15f);
        std::vector<double> timeVals;
        timeVals.push_back(0.0);
        timeVals.push_back(2.0);
        timeVals.push_back(4.0);
        this->pressEscAlphaAnim.SetLerp(timeVals, alphaVals);
        this->pressEscAlphaAnim.SetRepeat(true);
    }

    {
        std::vector<double> timeVals(3);
        timeVals[0] = 0.0; timeVals[1] = 0.5; timeVals[2] = 0.51;
        std::vector<float> alphaVals(timeVals.size());
        alphaVals[0] = 1.0f; alphaVals[1] = alphaVals[2] = 0.0f;
        this->fadeAnimation.SetLerp(timeVals, alphaVals);
        this->fadeAnimation.SetRepeat(false);
        this->fadeAnimation.SetInterpolantValue(1.0f);
    }

    this->normalTexRefractEffect.SetTechnique(CgFxPostRefract::NORMAL_TEXTURE_TECHNIQUE_NAME);
    this->normalTexRefractEffect.SetWarpAmountParam(27.0f);
    this->normalTexRefractEffect.SetIndexOfRefraction(1.2f);
    this->normalTexRefractEffect.SetNormalTexture(this->sphereNormalsTex);

    {
        
        std::vector<double> timeVals;
        timeVals.reserve(3);
        timeVals.push_back(0.0);
        timeVals.push_back(0.1);
        timeVals.push_back(0.199);
        
        std::vector<float> alphaVals;
        alphaVals.reserve(timeVals.size());
        alphaVals.push_back(1);
        alphaVals.push_back(0);
        alphaVals.push_back(1);

        AnimationMultiLerp<float> flickerAlphaAnim;
        flickerAlphaAnim.SetLerp(timeVals, alphaVals);
        flickerAlphaAnim.SetRepeat(true);
        flickerAlphaAnim.SetInterpolantValue(1.0f);

        timeVals.clear();
        timeVals.reserve(3);
        timeVals.push_back(0.0);
        timeVals.push_back(1.25);
        timeVals.push_back(1.5);
        
        alphaVals.clear();
        alphaVals.reserve(timeVals.size());
        alphaVals.push_back(1.0f);
        alphaVals.push_back(1.0f);
        alphaVals.push_back(0.0f);

        AnimationMultiLerp<float> fadeAnim;
        fadeAnim.SetLerp(timeVals, alphaVals);
        fadeAnim.SetRepeat(false);
        fadeAnim.SetInterpolantValue(1.0f);

        this->particleFlickerFader.SetAnimation0(flickerAlphaAnim);
        this->particleFlickerFader.SetAnimation1(fadeAnim);
    }

    {
        std::vector<double> timeVals;
        timeVals.clear();
        timeVals.reserve(3);
        timeVals.push_back(0.0);
        timeVals.push_back(1.0);
        timeVals.push_back(2.0);

        std::vector<Colour> colourVals;
        colourVals.reserve(3);
        colourVals.push_back(GameViewConstants::GetInstance()->ACTIVE_POINT_STAR_COLOUR);
        colourVals.push_back(GameViewConstants::GetInstance()->BRIGHT_POINT_STAR_COLOUR);
        colourVals.push_back(GameViewConstants::GetInstance()->ACTIVE_POINT_STAR_COLOUR);

        this->starGlowColourAnim.SetInterpolantValue(GameViewConstants::GetInstance()->ACTIVE_POINT_STAR_COLOUR);
        this->starGlowColourAnim.SetLerp(timeVals, colourVals);
        this->starGlowColourAnim.SetRepeat(true);
    }

    this->SetupLevelPages(info);
}

SelectLevelMenuState::~SelectLevelMenuState() {
    delete this->menuFBO;
    this->menuFBO = NULL;
    delete this->postMenuFBObj;
    this->postMenuFBObj = NULL;

    delete this->worldLabel;
    this->worldLabel = NULL;

    delete this->totalLabel;
    this->totalLabel = NULL;

    delete this->totalNumWorldStarsLabel;
    this->totalNumWorldStarsLabel = NULL;

    delete this->totalNumGameStarsLabel;
    this->totalNumGameStarsLabel = NULL;

    delete this->keyEscLabel;
    this->keyEscLabel = NULL;

    this->ClearLevelPages();

    bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->starTexture);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->bossIconTexture);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->arrowTexture);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->padlockTexture);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->padlockBrokenLeftTexture);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->padlockBrokenRightTexture);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->snapTexture);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->starGlowTexture);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->sphereNormalsTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->explosionTex);
    assert(success);

    for (std::vector<Texture2D*>::iterator iter = this->smokeTextures.begin();
        iter != this->smokeTextures.end(); ++iter) {

        success = ResourceManager::GetInstance()->ReleaseTextureResource(*iter);
        assert(success);	
    }
    this->smokeTextures.clear();

    UNUSED_VARIABLE(success);

    delete this->nextPgArrowEmitter;
    this->nextPgArrowEmitter = NULL;
    delete this->prevPgArrowEmitter;
    this->prevPgArrowEmitter = NULL;

    delete this->fiveStarFGSparkleEmitter;
    this->fiveStarFGSparkleEmitter = NULL;
    delete this->fiveStarBGGlowEmitter;
    this->fiveStarBGGlowEmitter = NULL;
}


void SelectLevelMenuState::RenderFrame(double dT) {
    this->pressEscAlphaAnim.Tick(dT);
    this->starGlowColourAnim.Tick(dT);
    this->fiveStarFGSparkleEmitter->Tick(dT);
    this->fiveStarBGGlowEmitter->Tick(dT);
    this->fiveStarBGGlowEmitter->SetParticleColour(this->starGlowColourAnim.GetInterpolantValue());

    const Camera& camera = this->display->GetCamera();
    MenuBackgroundRenderer* bgRenderer = this->display->GetMenuBGRenderer();

    // Bind the menu's FrameBufferObject - we use this to do Bloom on the menu
	this->menuFBO->BindFBObj();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw the starry background...
    if (!this->goBackToWorldSelectMenu) {
        bgRenderer->DrawBG(camera);
    }

	// Draw in window coordinates
	Camera::PushWindowCoords();
	glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
	glLoadIdentity();

    // Draw the title...
    this->worldLabel->SetTopLeftCorner(HORIZONTAL_TITLE_GAP, Camera::GetWindowHeight() - VERTICAL_TITLE_GAP);
    this->worldLabel->Draw();

    // Draw the title strip
    //this->DrawTitleStrip(camera);

    // Draw the "Press esc to go back" stuff
    this->keyEscLabel->SetAlpha(this->pressEscAlphaAnim.GetInterpolantValue());
    this->keyEscLabel->SetTopLeftCorner(20, this->keyEscLabel->GetHeight()); 
    this->keyEscLabel->Draw();

    // Draw the menu
    this->DrawLevelSelectMenu(camera, dT);

    // Draw the label showing how many stars have been acquired for the world...
    this->DrawStarTotalLabels(camera, dT);

    // Draw the page selection squares
    this->DrawPageSelection(camera);

	// Draw a fade overlay if necessary
    bool fadeDone = this->fadeAnimation.Tick(dT);
    if (!fadeDone) {
        if (this->goToStartLevel) {
            bgRenderer->DrawNonAnimatedFadeBG(this->fadeAnimation.GetInterpolantValue());
        }
        else {
            bgRenderer->DrawBG(camera, this->fadeAnimation.GetInterpolantValue());
        }
    }

    // Special case for when the player enters the level select screen and the next level is unlock-able by the
    // number of stars that they have
    if (fadeDone) {
        if (this->playAutoStarUnlockAnim) {
            if (this->autoUnlockAnimCountdown <= 0.0) {
                this->pages[this->selectedPage]->GetSelectedItem()->ExecuteUnlockStarsPaidForAnimation();
                this->playAutoStarUnlockAnim = false;
            }
            else {
                this->autoUnlockAnimCountdown -= dT;
            }
        }
        else if (this->playAutoBasicUnlockAnim) {
            if (this->autoUnlockAnimCountdown <= 0.0) {
                this->pages[this->selectedPage]->GetSelectedItem()->ExecuteBasicUnlockAnimation();
                this->playAutoBasicUnlockAnim = false;
            }
            else {
                this->autoUnlockAnimCountdown -= dT;
            }
        }
    }

    glPopMatrix();
    Camera::PopWindowCoords();
    this->menuFBO->UnbindFBObj();

    if (this->goBackToWorldSelectMenu) {

        bool animDone = this->goBackMenuAlphaAnim.Tick(dT);
        animDone &= this->goBackMenuMoveAnim.Tick(dT);
        
        float moveAmt  = this->goBackMenuMoveAnim.GetInterpolantValue();  
        float alphaAmt = this->goBackMenuAlphaAnim.GetInterpolantValue();

	    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Camera::PushWindowCoords();
	    glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
	    glLoadIdentity();

        glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT | GL_ENABLE_BIT);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glPolygonMode(GL_FRONT, GL_FILL);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);

        Texture* bgTexture = bgRenderer->GetMenuBGTexture();

        float screenWidth = Camera::GetWindowWidth();
        float screenHeight = Camera::GetWindowHeight();
        float totalTexU = (GameViewConstants::STARRY_BG_TILE_MULTIPLIER * screenWidth / static_cast<float>(bgTexture->GetWidth()));
        float totalTexV = (GameViewConstants::STARRY_BG_TILE_MULTIPLIER * screenHeight / static_cast<float>(bgTexture->GetHeight()));

        bgTexture->BindTexture();
        glColor4f(1,1,1,1);
        glBegin(GL_QUADS);
            glTexCoord2f(0, 0);
            glVertex2f(-Camera::GetWindowWidth() + moveAmt, 0);
            glTexCoord2f(2*totalTexU, 0);
	        glVertex2f(Camera::GetWindowWidth() + moveAmt, 0);
            glTexCoord2f(2*totalTexU, totalTexV);
            glVertex2f(Camera::GetWindowWidth() + moveAmt, Camera::GetWindowHeight());
	        glTexCoord2f(0, totalTexV);
            glVertex2f(-Camera::GetWindowWidth() + moveAmt, Camera::GetWindowHeight());
        glEnd();

        this->menuFBO->GetFBOTexture()->BindTexture();
        glColor4f(1,1,1,alphaAmt);
        glBegin(GL_QUADS);
	        glTexCoord2i(1, 0);
            glVertex2i(Camera::GetWindowWidth() + moveAmt, 0);
	        glTexCoord2i(1, 1);
            glVertex2i(Camera::GetWindowWidth() + moveAmt, Camera::GetWindowHeight());
	        glTexCoord2i(0, 1);
	        glVertex2i(moveAmt, Camera::GetWindowHeight());
            glTexCoord2i(0, 0);
	        glVertex2i(moveAmt, 0);
        glEnd();
        this->menuFBO->GetFBOTexture()->UnbindTexture();
        
        glPopAttrib();
        glPopMatrix();
        Camera::PopWindowCoords();

        if (animDone) {

            // Switch states back to the world select menu...
            this->display->SetCurrentState(new SelectWorldMenuState(this->display, 
                DisplayStateInfo::BuildSelectWorldInfo(this->world->GetWorldNumber()-1), this->bgSoundLoopID));

            debug_opengl_state();
            return;
        }
    }
    else {
        this->postMenuFBObj->BindFBObj();
        this->menuFBO->GetFBOTexture()->RenderTextureToFullscreenQuad();
        this->postMenuFBObj->UnbindFBObj();
        this->postMenuFBObj->GetFBOTexture()->RenderTextureToFullscreenQuad();

        this->normalTexRefractEffect.SetFBOTexture(this->postMenuFBObj->GetFBOTexture());
    }

    // Handle the case where the player selected a level
    if (this->goToStartLevel && fadeDone) {
        
        // Start the currently selected level
        {
            AbstractLevelMenuItem* selectedLevelItem = this->pages[this->selectedPage]->GetSelectedItem();
            GameLevel* selectedLevel = selectedLevelItem->GetLevel();

		    // Load all the initial stuffs for the game - this will queue up the next states that we need to go to
            this->display->GetModel()->StartGameAtWorldAndLevel(this->world->GetWorldIndex(), selectedLevel->GetLevelIndex());
            // WARNING: the selectedLevel will be an invalid pointer after the call to StartGameAtWorldAndLevel
		}
        
        // Place the view into the proper state to play the game	
		this->display->SetCurrentStateAsNextQueuedState();
    }

    debug_opengl_state();
}

void SelectLevelMenuState::ButtonPressed(const GameControl::ActionButton& pressedButton,
                                         const GameControl::ActionMagnitude& magnitude) {

    UNUSED_PARAMETER(magnitude);

    if (this->goBackToWorldSelectMenu || this->goToStartLevel || this->freezePlayerInput) {
        return;
    }

    switch (pressedButton) {
        case GameControl::EscapeButtonAction:
            this->GoBackToWorldSelectMenu();
            break;

        case GameControl::EnterButtonAction:
            this->GoToStartLevel();
            break;

        case GameControl::LeftButtonAction:
            this->MoveSelectionX(false);
            break;

        case GameControl::RightButtonAction:
            this->MoveSelectionX(true);
            break;

        case GameControl::UpButtonAction:
            this->MoveSelectionY(true);
            break;

        case GameControl::DownButtonAction:
            this->MoveSelectionY(false);
            break;

#ifdef _DEBUG
        case GameControl::SpecialCheatButtonAction:
            // Unlock all levels in the current movement...
            // (You need to restart the game for this to take effect)
            GameProgressIO::SaveFullProgressOfWorld(this->display->GetModel(), this->world->GetStyle());
            break;
#endif

        default:
            break;
    }

}

void SelectLevelMenuState::ButtonReleased(const GameControl::ActionButton& releasedButton) {
    UNUSED_PARAMETER(releasedButton);
}

// Event called whenever the selection of a level in the menu changes
void SelectLevelMenuState::LevelSelectionChanged() {
    this->display->GetSound()->PlaySound(GameSound::LevelMenuItemChangedSelectionEvent, false);
}
void SelectLevelMenuState::PageSelectionChanged() {
    this->nextPgArrowEmitter->Reset();
    this->prevPgArrowEmitter->Reset();
}

void SelectLevelMenuState::DrawStarTotalLabels(const Camera& camera, double dT) {
    UNUSED_PARAMETER(camera);

    float worldTotalLabelTopCornerX = this->worldLabel->GetTopLeftCorner()[0] + this->worldLabel->GetLastRasterWidth() + HORIZONTAL_TITLE_GAP;
    float worldTotalLabelTopCornerY = this->worldLabel->GetTopLeftCorner()[1] - 
        this->worldLabel->GetHeight() + this->totalNumWorldStarsLabel->GetHeight();

    this->totalNumWorldStarsLabel->SetTopLeftCorner(worldTotalLabelTopCornerX, worldTotalLabelTopCornerY);
    this->totalNumWorldStarsLabel->Draw();

    float gameTotalLabelTopCornerX = Camera::GetWindowWidth() - this->totalNumGameStarsLabel->GetLastRasterWidth() - HORIZONTAL_TITLE_GAP;
    float gameTotalLabelTopCornerY = this->totalNumGameStarsLabel->GetHeight() + VERTICAL_TITLE_GAP/2;
    this->totalNumGameStarsLabel->SetTopLeftCorner(gameTotalLabelTopCornerX, gameTotalLabelTopCornerY);
    this->totalNumGameStarsLabel->Draw();

    float diff     = 0.25f * this->totalNumWorldStarsLabel->GetHeight();
    float starSize = this->totalNumWorldStarsLabel->GetHeight() + 2*diff;
    float centerX  = worldTotalLabelTopCornerX - diff - starSize / 2.0f;
    float centerY  = worldTotalLabelTopCornerY + 1.5f * diff - starSize / 2.0f;

    glPushAttrib(GL_CURRENT_BIT | GL_TEXTURE_BIT | GL_ENABLE_BIT);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const Colour& starColour = this->starGlowColourAnim.GetInterpolantValue();
    
    // Star in the world star total
    this->starTexture->BindTexture();
    glColor4f(starColour.R(), starColour.G(), starColour.B(), 1.0f);
    glPushMatrix();
    glTranslatef(centerX, centerY, 0.0f);
    glScalef(starSize, starSize, 1.0f);
    GeometryMaker::GetInstance()->DrawQuad();
    glPopMatrix();
    
    // Star in the game star total
    diff     = 0.25f * this->totalNumGameStarsLabel->GetHeight();
    starSize = this->totalNumGameStarsLabel->GetHeight() + 2*diff;
    centerX  = gameTotalLabelTopCornerX - diff - starSize / 2.0f;
    centerY  = gameTotalLabelTopCornerY + 1.5f * diff - starSize / 2.0f;
    
    glPushMatrix();
    glTranslatef(centerX, centerY, 0.0f);
    glScalef(starSize, starSize, 1.0f);

    glPushMatrix();
    this->totalGameStarGlowPulseAnim.Tick(dT);
    float pulseScale = this->totalGameStarGlowPulseAnim.GetInterpolantValue();
    glScalef(pulseScale, pulseScale, 1.0f);
    glColor4f(starColour.R(), starColour.G(), starColour.B(), 0.5f);
    this->starGlowTexture->BindTexture();
    GeometryMaker::GetInstance()->DrawQuad();
    glPopMatrix();

    this->starTexture->BindTexture();
    glColor4f(starColour.R(), starColour.G(), starColour.B(), 1.0f);
    GeometryMaker::GetInstance()->DrawQuad();    
    glPopMatrix();

    glPopAttrib();

    this->totalLabel->SetTopLeftCorner(centerX - starSize / 2.0f - diff - this->totalLabel->GetLastRasterWidth(), gameTotalLabelTopCornerY);
    this->totalLabel->Draw();
}

void SelectLevelMenuState::DrawTitleStrip(const Camera& camera) const {
    UNUSED_PARAMETER(camera);

    static const float STRIP_HEIGHT      = 10;
    static const float DROP_AMT          = STRIP_HEIGHT / 5.0f;
    static const float TEXT_TO_STRIP_GAP = 20;
    static const float SIDE_TO_STRIP_GAP = 20;

    Point2D stripStartPt = this->worldLabel->GetTopLeftCorner() + 
        Vector2D(this->worldLabel->GetLastRasterWidth() + TEXT_TO_STRIP_GAP, -static_cast<int>(this->worldLabel->GetHeight()/2));


    // Drop shadow of the title strip
    glPushMatrix();
    glTranslatef(DROP_AMT, -DROP_AMT, 0);
    glBegin(GL_QUADS);
    glColor4f(1, 1, 1, 1);
    glVertex3f(stripStartPt[0], stripStartPt[1], 0.0f);
    glVertex3f(stripStartPt[0], stripStartPt[1] - STRIP_HEIGHT, 0.0f);
    glVertex3f(Camera::GetWindowWidth()-SIDE_TO_STRIP_GAP, stripStartPt[1] - STRIP_HEIGHT, 0.0f);
    glVertex3f(Camera::GetWindowWidth()-SIDE_TO_STRIP_GAP, stripStartPt[1], 0.0f);
    glEnd();
    glPopMatrix();

    glBegin(GL_QUADS);
    glColor4f(0.4f, 0.6f, 0.8f, 1.0f);
    glVertex3f(stripStartPt[0], stripStartPt[1], 0.0f);
    glVertex3f(stripStartPt[0], stripStartPt[1] - STRIP_HEIGHT, 0.0f);
    glVertex3f(Camera::GetWindowWidth()-SIDE_TO_STRIP_GAP, stripStartPt[1] - STRIP_HEIGHT, 0.0f);
    glVertex3f(Camera::GetWindowWidth()-SIDE_TO_STRIP_GAP, stripStartPt[1], 0.0f);
    glEnd();
}

void SelectLevelMenuState::DrawPageSelection(const Camera& camera) const {
    UNUSED_PARAMETER(camera);

    // Don't bother drawing anything if there's only one page
    if (this->pages.size() <= 1) {
        return;
    }

    static const int SQUARE_SIZE         = 16;
    static const int SQUARE_GAP_SIZE     = 5;
    static const int SQUARE_BOTTOM_Y_POS = VERTICAL_TITLE_GAP;
    static const int SQUARE_TOP_Y_POS    = SQUARE_BOTTOM_Y_POS + SQUARE_SIZE;
    
    int totalWidth = SQUARE_SIZE * this->pages.size() + SQUARE_GAP_SIZE * (this->pages.size()-1);
    int currX = (Camera::GetWindowWidth() - totalWidth) / 2;

    glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT | GL_ENABLE_BIT);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();

    // Draw a square for each page, the selected page is coloured in, the rest are grey
    glBegin(GL_QUADS);
    for (size_t i = 0; i < this->pages.size(); i++) {
        if (i == static_cast<size_t>(this->selectedPage)) {
            glColor4f(1, 0.65f, 0, 1);
        }
        else {
            glColor4f(0.66f, 0.66f, 0.66f, 1);
        }

        glVertex2f(currX, SQUARE_TOP_Y_POS);
        glVertex2f(currX, SQUARE_BOTTOM_Y_POS);
        glVertex2f(currX+SQUARE_SIZE, SQUARE_BOTTOM_Y_POS);
        glVertex2f(currX+SQUARE_SIZE, SQUARE_TOP_Y_POS);

        currX += SQUARE_SIZE + SQUARE_GAP_SIZE;
    }
    glEnd();
    
    // Draw outlines...
    glLineWidth(1.0f);
    glColor4f(0, 0, 0, 1);
    currX = (Camera::GetWindowWidth() - totalWidth) / 2;
    for (size_t i = 0; i < this->pages.size(); i++) {
        
        glBegin(GL_LINE_LOOP);
        glVertex2f(currX, SQUARE_TOP_Y_POS);
        glVertex2f(currX, SQUARE_BOTTOM_Y_POS);
        glVertex2f(currX+SQUARE_SIZE, SQUARE_BOTTOM_Y_POS);
        glVertex2f(currX+SQUARE_SIZE, SQUARE_TOP_Y_POS);
        glEnd();

        currX += SQUARE_SIZE + SQUARE_GAP_SIZE;
    }
    
    glPopMatrix();
    glPopAttrib();
}

/**
 * Draw the menu of level selection items, including the selected item which has a glowing border
 * drawn around it.
 */
void SelectLevelMenuState::DrawLevelSelectMenu(const Camera& camera, double dT) {
    glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT | GL_ENABLE_BIT);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    AbstractLevelMenuItem* selectedItem = this->pages[this->selectedPage]->GetSelectedItem();
    assert(selectedItem != NULL);
    
    float leftX  = selectedItem->GetTopLeftCorner()[0] - BORDER_GAP;
    float rightX = selectedItem->GetTopLeftCorner()[0] + selectedItem->GetWidth() + BORDER_GAP;
    float topY = selectedItem->GetTopLeftCorner()[1] + BORDER_GAP/2;
    float bottomY = selectedItem->GetTopLeftCorner()[1] - selectedItem->GetHeight() - BORDER_GAP;

    float centerX = (leftX + rightX) / 2.0f;
    float centerY = (topY + bottomY) / 2.0f;
    float selectionWidth  = selectedItem->GetWidth() + 2*BORDER_GAP;
    float selectionHeight = selectedItem->GetHeight() + 1.5*BORDER_GAP;

    this->selectionAlphaOrangeAnim.Tick(dT);
    this->selectionAlphaYellowAnim.Tick(dT);
    this->selectionBorderAddAnim.Tick(dT);

    float yellowAlpha  = this->selectionAlphaYellowAnim.GetInterpolantValue();
    float orangeAlpha  = this->selectionAlphaOrangeAnim.GetInterpolantValue();
    float selectionAdd = this->selectionBorderAddAnim.GetInterpolantValue();

    // Draw any indicator arrows to show that the selection can be moved to another page
    if (this->selectedPage != static_cast<int>(this->pages.size())-1) {
        const AbstractLevelMenuItem* lastItem = this->pages[this->selectedPage]->GetLastItem();
        this->nextPgArrowEmitter->SetEmitVolume(Point3D(lastItem->GetTopLeftCorner()[0] + lastItem->GetWidth(), 
                                                        lastItem->GetTopLeftCorner()[1] - lastItem->GetHeight()+5, 0),
                                                Point3D(lastItem->GetTopLeftCorner()[0] + lastItem->GetWidth(),
                                                        lastItem->GetTopLeftCorner()[1]-5, 0));

        this->nextPgArrowEmitter->Tick(dT);
        this->nextPgArrowEmitter->Draw(camera);
    }

    if (this->selectedPage != 0) {
        const AbstractLevelMenuItem* firstItem = this->pages[this->selectedPage]->GetFirstItem();
        this->prevPgArrowEmitter->SetEmitVolume(Point3D(firstItem->GetTopLeftCorner()[0], 
                                                        firstItem->GetTopLeftCorner()[1] - firstItem->GetHeight()+5, 0),
                                                Point3D(firstItem->GetTopLeftCorner()[0],
                                                        firstItem->GetTopLeftCorner()[1]-5, 0));
        this->prevPgArrowEmitter->Tick(dT);
        this->prevPgArrowEmitter->Draw(camera);
    }

     // Draw the currently selected level item's selection border
    glPushMatrix();
    glTranslatef(centerX, centerY, 0.0f);
    
    glPushMatrix();
    // Draw the yellow border
    glColor4f(1, 1, 0, yellowAlpha);
    glScalef(selectionWidth + selectionAdd, selectionHeight + selectionAdd, 1.0f);
    GeometryMaker::GetInstance()->DrawQuad();
    glPopMatrix();

    glPushMatrix();
    // Draw the orange border
    glColor4f(1, 0.65f, 0, orangeAlpha);
    glScalef(selectionWidth + selectionAdd/2, selectionHeight + selectionAdd/2, 1.0f);
    GeometryMaker::GetInstance()->DrawQuad();
    glPopMatrix();

    glPopMatrix();

    // Draw the yellowish highlight background
    glColor4f(0.98f, 1.0f, 0.66f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(leftX, topY);
    glVertex2f(leftX, bottomY);
    glVertex2f(rightX, bottomY);
    glVertex2f(rightX, topY);
    glEnd();

    // Draw the black outline around the selection
    glColor4f(0, 0, 0, 1);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(leftX, topY);
    glVertex2f(leftX, bottomY);
    glVertex2f(rightX, bottomY);
    glVertex2f(rightX, topY);
    glEnd();

    // Draw the current page of level items
    this->pages[this->selectedPage]->Draw(camera, dT);

    glPopAttrib();
}

/** 
 * Call this to indicate that we will be going back to the world select menu.
 */
void SelectLevelMenuState::GoBackToWorldSelectMenu() {
    
    GameSound* sound = this->display->GetSound();
    sound->PlaySound(GameSound::MenuItemCancelEvent, false);

    this->goBackMenuMoveAnim.SetLerp(0.5, Camera::GetWindowWidth());
    this->goBackMenuMoveAnim.SetRepeat(false);
    this->goBackMenuAlphaAnim.SetLerp(0.5, 0.0f);
    this->goBackMenuAlphaAnim.SetRepeat(false);

    this->goBackToWorldSelectMenu = true;
}

void SelectLevelMenuState::GoToStartLevel() {
    GameSound* sound = this->display->GetSound();
    
    AbstractLevelMenuItem* selectedItem = this->pages[this->selectedPage]->GetSelectedItem();
    assert(selectedItem != NULL);

    if (selectedItem->GetIsEnabled()) {

        sound->StopSound(this->bgSoundLoopID, 0.5);
        sound->PlaySound(GameSound::LevelMenuItemSelectEvent, false);
        
        // Finishing animation for starting the level
        std::vector<double> timeVals(3);
        timeVals[0] = 0.0; timeVals[1] = 0.5; timeVals[2] = 0.51;
        std::vector<float> alphaVals(timeVals.size());
        alphaVals[0] = this->fadeAnimation.GetInterpolantValue();
        alphaVals[1] = alphaVals[2] = 1.0f;

        this->fadeAnimation.SetLerp(timeVals, alphaVals);
	    this->fadeAnimation.SetRepeat(false);

        this->goToStartLevel = true;
    }
    else {

        // Check to see if the item is locked with a star total and whether it is the closest level to being unlocked
        const GameLevel* selectedItemLevel = selectedItem->GetLevel();
        assert(selectedItemLevel != NULL);
        
        bool isProgressUpToSelectedLevel = selectedItemLevel->GetLevelIndex() == 0 || 
            this->world->GetLastLevelIndexPassed() >= static_cast<int>(selectedItemLevel->GetLevelIndex())-1;
        bool canBeUnlockedWithStars = !selectedItemLevel->GetAreUnlockStarsPaidFor() && 
            (selectedItemLevel->GetNumStarsRequiredToUnlock() > 0) && isProgressUpToSelectedLevel;
        bool isLocked = true;
        if (canBeUnlockedWithStars) {
            // Does the player have enough stars to unlock the level?
            int totalAcquiredStars = this->display->GetModel()->GetTotalStarsCollectedInGame();
            if (totalAcquiredStars >= selectedItemLevel->GetNumStarsRequiredToUnlock()) {
                
                // Unlock the level... play the animation for the lock exploding 
                // and freeze player control until it's done
                selectedItem->ExecuteUnlockStarsPaidForAnimation();
                isLocked = false;
            }
        }

        if (isLocked) {
            sound->PlaySound(GameSound::LevelMenuItemLockedEvent, false);
            // Play animation to indicate/show that the level is disabled
            selectedItem->ExecuteLockedAnimation();
        }

    }
}

void SelectLevelMenuState::SetupLevelPages(const DisplayStateInfo& info) {
    this->ClearLevelPages();

    static const int TITLE_TO_ITEM_Y_GAP_SIZE = 40;
    static const int SIDE_TO_ITEM_GAP_SIZE    = HORIZONTAL_TITLE_GAP;
    static const int ITEM_X_GAP_SIZE          = 40;
    static const int ITEM_Y_GAP_SIZE          = 40;
    static const int MIN_ITEM_SIZE            = 200;

    float scalingFactor = this->display->GetTextScalingFactor();
    float amtForItems = Camera::GetWindowWidth() - 2 * SIDE_TO_ITEM_GAP_SIZE;
    
    float itemWidth = 0;
    this->numItemsPerRow = 5; 
    while (itemWidth < MIN_ITEM_SIZE && numItemsPerRow >= 2) {
        this->numItemsPerRow--;
        itemWidth = static_cast<float>(amtForItems - (ITEM_X_GAP_SIZE * (numItemsPerRow-1))) / 
                    static_cast<float>(this->numItemsPerRow);
    }
    assert(this->numItemsPerRow >= 2);
    
    float itemX = SIDE_TO_ITEM_GAP_SIZE;
    float itemY = Camera::GetWindowHeight() - this->worldLabel->GetHeight() - VERTICAL_TITLE_GAP - TITLE_TO_ITEM_Y_GAP_SIZE;

    // Build the menu items for level selection
    int totalNumStarsCollected = this->world->GetNumStarsCollectedInWorld();
    int totalNumStars = this->world->GetTotalAchievableStarsInWorld();

    const std::vector<GameLevel*>& levels = this->world->GetAllLevelsInWorld();
    this->pages.push_back(new LevelMenuPage());
    AbstractLevelMenuItem* levelItem   = NULL;
    int numRows = static_cast<int>(ceil(static_cast<float>(levels.size()) / static_cast<float>(numItemsPerRow)));
    bool noScoreEncountered = false;
    this->selectedPage = 0;

    float standardHeight = 0.0f;
    {
        LevelMenuItem* temp = new LevelMenuItem(this, 1, levels.at(0), itemWidth, Point2D(0,0), true);
        standardHeight = temp->GetHeight();
        delete temp;
        temp = NULL;
    }

    int rowsOnCurrPage = 0;
    for (int row = 0; row < numRows; row++) {
        for (int col = 0; col < numItemsPerRow; col++) {

            int currLevelIdx = row * numItemsPerRow + col;
            if (currLevelIdx >= static_cast<int>(levels.size())) {
                break;
            }

            GameLevel* currLevel = levels.at(currLevelIdx);
            assert(currLevel != NULL);
            if (noScoreEncountered) {
                currLevel->SetAreUnlockStarsPaidFor(false);
            }

            if (currLevel->GetHasBoss()) {
                levelItem = new BossLevelMenuItem(this, currLevel, itemWidth, standardHeight,
                    Point2D(itemX, itemY), !noScoreEncountered && currLevel->GetAreUnlockStarsPaidFor(), this->bossIconTexture);
            }
            else {
                levelItem = new LevelMenuItem(this, currLevelIdx+1, currLevel, itemWidth, 
                    Point2D(itemX, itemY), !noScoreEncountered && currLevel->GetAreUnlockStarsPaidFor());
            }

            LevelMenuPage* currPage = this->pages.back();
            currPage->AddLevelItem(levelItem);

            if (!currLevel->GetIsLevelPassedWithScore()) {
                
                if (!noScoreEncountered) {
                    this->selectedPage = this->pages.size()-1;
                    currPage->SetSelectedItemIndex(currPage->GetNumLevelItems()-1);
                }

                noScoreEncountered = true;
            }

            itemX += itemWidth + ITEM_X_GAP_SIZE;
        }

        if (!noScoreEncountered) {
            // If all levels are passed then we set the level select to be the final level of the movement
            this->selectedPage = this->pages.size()-1;
            LevelMenuPage* lastPage = this->pages[this->pages.size()-1];
            lastPage->SetSelectedItemIndex(lastPage->GetNumLevelItems()-1);
        }

        assert(levelItem != NULL);
        itemX  = SIDE_TO_ITEM_GAP_SIZE;
        itemY -= ITEM_Y_GAP_SIZE + standardHeight;

        // Check to see if we've exceeded the size of the page so that we make the next page...
        rowsOnCurrPage++;
        if ((itemY - standardHeight) <= (this->keyEscLabel->GetHeight() + VERTICAL_TITLE_GAP)) {

            this->pages.back()->SetNumRows(rowsOnCurrPage);

            // As long as this isn't the last page we keep making more pages...
            if (row != numRows-1) {
                itemY = Camera::GetWindowHeight() - this->worldLabel->GetHeight() - VERTICAL_TITLE_GAP - TITLE_TO_ITEM_Y_GAP_SIZE;
                this->pages.push_back(new LevelMenuPage());
                rowsOnCurrPage = 0;
            }
        }
    }

    if (info.GetLevelSelectionIndex() >= 0) {
        int selectionIdx = info.GetLevelSelectionIndex();
        for (int i = 0; i < static_cast<int>(this->pages.size()) && selectionIdx >= 0; i++) {
            LevelMenuPage* currPage = this->pages[i];
            if (selectionIdx < static_cast<int>(currPage->GetNumLevelItems())) {
                assert(selectionIdx >= 0);
                currPage->SetSelectedItemIndex(selectionIdx);
                this->selectedPage = i;
                break;
            }
            selectionIdx -= currPage->GetNumLevelItems();
        }
    }

    // Now we know the height of the item selection grid, we can center it on the y-axis
    const float totalAllowableGridYSpace = Camera::GetWindowHeight() - this->worldLabel->GetHeight() - VERTICAL_TITLE_GAP - 
        TITLE_TO_ITEM_Y_GAP_SIZE - this->keyEscLabel->GetHeight() - VERTICAL_TITLE_GAP;
    
    LevelMenuPage* firstPage = this->pages.front();
    float gridHeight = firstPage->GetNumRows() * standardHeight + (firstPage->GetNumRows()-1) * ITEM_Y_GAP_SIZE;
    
    float centeredYBorderSpace = (totalAllowableGridYSpace - gridHeight) / 2.0f;
    if (centeredYBorderSpace > 0) {
        float itemTopY = Camera::GetWindowHeight() - this->worldLabel->GetHeight() - VERTICAL_TITLE_GAP - 
            TITLE_TO_ITEM_Y_GAP_SIZE - centeredYBorderSpace;

        // Go through all pages and readjust the y position of each item
        for (int i = 0; i < static_cast<int>(this->pages.size()); i++) {
            LevelMenuPage* currPage = this->pages[i];
            AbstractLevelMenuItem* firstItem = currPage->GetItemAt(0);
            float yDiff = itemTopY - firstItem->GetTopLeftCorner()[1];

            for (int j = 0; j < static_cast<int>(currPage->GetNumLevelItems()); j++) {
                AbstractLevelMenuItem* currItem = currPage->GetItemAt(j);
                currItem->RebuildItem(currItem->GetIsEnabled(), currItem->GetTopLeftCorner() + Vector2D(0, yDiff));
            }
        }
    }
    assert(!this->pages.empty());

    // Setup the selection animation
    std::vector<double> timeVals;
    timeVals.push_back(0.0);
    timeVals.push_back(0.5);
    timeVals.push_back(1.0);
    std::vector<float> alphaVals;
    alphaVals.push_back(0.0f);
    alphaVals.push_back(1.0f);
    alphaVals.push_back(0.0f);
    this->selectionAlphaOrangeAnim.SetLerp(timeVals, alphaVals);
    this->selectionAlphaOrangeAnim.SetRepeat(true);

    timeVals.clear();
    timeVals.push_back(0.0);
    timeVals.push_back(1.0);
    alphaVals.clear();
    alphaVals.push_back(1.0f);
    alphaVals.push_back(0.0f);
    this->selectionAlphaYellowAnim.SetLerp(timeVals, alphaVals);
    this->selectionAlphaYellowAnim.SetRepeat(true);

    std::vector<float> scaleVals;
    scaleVals.push_back(0.0f);
    scaleVals.push_back(2*ITEM_X_GAP_SIZE);
    this->selectionBorderAddAnim.SetLerp(timeVals, scaleVals);
    this->selectionBorderAddAnim.SetRepeat(true);

    std::stringstream totalNumGameStarsTxt;
    totalNumGameStarsTxt << this->display->GetModel()->GetTotalStarsCollectedInGame();
    this->totalNumGameStarsLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, 
        GameFontAssetsManager::Big), totalNumGameStarsTxt.str());
    this->totalNumGameStarsLabel->SetColour(Colour(1,1,1));
    this->totalNumGameStarsLabel->SetScale(scalingFactor);

    timeVals.clear();
    timeVals.reserve(3);
    timeVals.push_back(0.0); timeVals.push_back(1.0); timeVals.push_back(2.0);
    std::vector<float> pulseVals;
    pulseVals.push_back(1.0f); pulseVals.push_back(1.25f); pulseVals.push_back(1.0f);
    this->totalGameStarGlowPulseAnim.SetLerp(timeVals, pulseVals);
    this->totalGameStarGlowPulseAnim.SetRepeat(true);

    std::stringstream totalNumWorldStarsTxt;
    totalNumWorldStarsTxt << totalNumStarsCollected << "/" << totalNumStars;
    this->totalNumWorldStarsLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, 
        GameFontAssetsManager::Medium), totalNumWorldStarsTxt.str());
    this->totalNumWorldStarsLabel->SetColour(Colour(1,1,1));
    this->totalNumWorldStarsLabel->SetScale(0.75f * scalingFactor);

    // If we can unlock the currently selected level with stars then we do...
    AbstractLevelMenuItem* selectedItem = this->pages[this->selectedPage]->GetSelectedItem();
    assert(selectedItem != NULL);
    const GameLevel* selectedItemLevel = selectedItem->GetLevel();
    assert(selectedItemLevel != NULL);

    // REMOVE THIS*******************************
    //this->freezePlayerInput = true;
    //this->playAutoBasicUnlockAnim = true;
    //this->autoUnlockAnimCountdown = 0.1;
    //selectedItem->SetEnabled(false);
    //return;
    // *******************************************

    bool isProgressUpToSelectedLevel = selectedItemLevel->GetLevelIndex() == 0 || 
        this->world->GetLastLevelIndexPassed() >= static_cast<int>(selectedItemLevel->GetLevelIndex())-1;
    if (isProgressUpToSelectedLevel) {

        if (selectedItemLevel->GetNumStarsRequiredToUnlock() > 0) {
            if (!selectedItemLevel->GetAreUnlockStarsPaidFor()) {
                // Does the player have enough stars to unlock the level?
                int totalAcquiredStars = this->display->GetModel()->GetTotalStarsCollectedInGame();
                if (totalAcquiredStars >= selectedItemLevel->GetNumStarsRequiredToUnlock()) {

                    // Unlock the level... play the animation for the lock exploding and freeze player control until it's done
                    this->freezePlayerInput  = true;
                    this->playAutoStarUnlockAnim = true;
                    this->autoUnlockAnimCountdown = 0.25;
                }
            }
        }
        else if (info.GetDoBasicLevelUnlockAnimation()) {
            this->freezePlayerInput = true;
            this->playAutoBasicUnlockAnim = true;
            this->autoUnlockAnimCountdown = 0.05;
            selectedItem->SetEnabled(false);
        }
    }
}

void SelectLevelMenuState::ClearLevelPages() {
    for (size_t i = 0; i < this->pages.size(); i++) {
        LevelMenuPage* page = this->pages[i];
        delete page;
        page = NULL;
    }
    this->pages.clear();
}

void SelectLevelMenuState::MoveSelectionX(bool right) {
    int x = right ? 1 : -1;

    // Check to see if we're spilling over the edge of the current page - if there's a page to either
    // side then the selection will move to that new page...
    size_t selectedItem = this->pages[this->selectedPage]->GetSelectedItemIndex();
    
    size_t prevSelectedItem = selectedItem;
    int prevSelectedPg   = this->selectedPage;

    assert(this->numItemsPerRow-1 > 0);

    bool isFirstItem      = selectedItem == 0;
    bool isLastItem       = selectedItem == (this->pages[this->selectedPage]->GetNumLevelItems()-1);

    bool fallThrough = false;
    if (this->pages.size() > 1 && (isFirstItem || isLastItem)) {
        if (isFirstItem && !right) {
            // The user is moving the selection off the page to the left
            size_t newPageNum = (this->pages.size() + (static_cast<int>(this->selectedPage) - 1)) % this->pages.size();
            this->selectedPage = newPageNum;
            LevelMenuPage* currPage = this->pages[this->selectedPage];
            currPage->SetSelectedItemIndex(currPage->GetNumLevelItems()-1);
        }
        else if (isLastItem && right) {
            // The user is moving the selection off the page to the right
            size_t newPageNum = (this->selectedPage + 1) % this->pages.size();
            this->selectedPage = newPageNum;
            LevelMenuPage* currPage = this->pages[this->selectedPage];
            currPage->SetSelectedItemIndex(0);
        }
        else {
            fallThrough = true;
        }
    }
    else {
        fallThrough = true;
    }

    if (fallThrough) {
        int currRowIndex  = selectedItem / this->numItemsPerRow;
        int wrapAroundX   = (this->numItemsPerRow + (selectedItem - currRowIndex * this->numItemsPerRow) + x) % this->numItemsPerRow;

        bool isFirstItemInRow = (selectedItem % this->numItemsPerRow) == 0;
        bool isLastItemInRow  = (selectedItem % this->numItemsPerRow) == static_cast<size_t>(this->numItemsPerRow-1);

        if (right && !isLastItem && isLastItemInRow) {
            this->pages[this->selectedPage]->SetSelectedItemIndex(this->numItemsPerRow * (currRowIndex+1) + wrapAroundX);
        }
        else if(!right && currRowIndex != 0 && isFirstItemInRow) {
            this->pages[this->selectedPage]->SetSelectedItemIndex(this->numItemsPerRow * (currRowIndex-1) + wrapAroundX);
        }
        else {
            this->pages[this->selectedPage]->SetSelectedItemIndex(this->numItemsPerRow * currRowIndex + wrapAroundX);
        }
    }

    // Check to see if we should raise a level selection changed event
    if (this->selectedPage != prevSelectedPg) {
        this->PageSelectionChanged();
        this->LevelSelectionChanged();
    }
    else if (this->pages[this->selectedPage]->GetSelectedItemIndex() != prevSelectedItem) {
        this->LevelSelectionChanged();
    }
}

void SelectLevelMenuState::MoveSelectionY(bool up) {
    int y = up ? 1 : -1;

    int selectedItem  = this->pages[this->selectedPage]->GetSelectedItemIndex();
    size_t numLevelItems = this->pages[this->selectedPage]->GetNumLevelItems();

    int numRows = (numLevelItems / this->numItemsPerRow) + 1;
    int rowsAllFilledNumItems = (numRows * this->numItemsPerRow);
    int newSelectedIndex = (rowsAllFilledNumItems + selectedItem - y * this->numItemsPerRow) % rowsAllFilledNumItems;
    if (newSelectedIndex >= static_cast<int>(numLevelItems)) {
        if (up) {
            newSelectedIndex -= this->numItemsPerRow;
        }
        else {
            newSelectedIndex %= this->numItemsPerRow;
        }
    }
    this->pages[this->selectedPage]->SetSelectedItemIndex(newSelectedIndex);

    // Check to see if we should raise a level selection changed event
    if (selectedItem != newSelectedIndex) {
        this->LevelSelectionChanged();
    }
}

int SelectLevelMenuState::GetNumItemsOnRow(int rowIdx) {
    size_t numLevelItems = this->pages[this->selectedPage]->GetNumLevelItems();
    int temp = static_cast<int>(numLevelItems) - (static_cast<int>(this->numItemsPerRow) * rowIdx);
    return std::min<int>(temp, this->numItemsPerRow);
}

#define DISABLED_COLOUR Colour(SelectLevelMenuState::AbstractLevelMenuItem::DISABLED_GREY_AMT, \
    SelectLevelMenuState::AbstractLevelMenuItem::DISABLED_GREY_AMT, \
    SelectLevelMenuState::AbstractLevelMenuItem::DISABLED_GREY_AMT)

const float SelectLevelMenuState::AbstractLevelMenuItem::NUM_TO_NAME_GAP            = 8;
const float SelectLevelMenuState::AbstractLevelMenuItem::DISABLED_GREY_AMT          = 0.78f;
const float SelectLevelMenuState::AbstractLevelMenuItem::PADLOCK_SCALE              = 0.85f;
const float SelectLevelMenuState::AbstractLevelMenuItem::STAR_LOCKED_PADLOCK_SCALE  = 1.32f;

const double SelectLevelMenuState::AbstractLevelMenuItem::TOTAL_STAR_UNLOCK_TIME = 3.0;

SelectLevelMenuState::AbstractLevelMenuItem::AbstractLevelMenuItem(SelectLevelMenuState* state,
                                                                   GameLevel* level, float width,
                                                                   const Point2D& topLeftCorner, bool isEnabled) :
state(state), level(level), topLeftCorner(topLeftCorner), width(width), starShrinkAnim(NULL), starAlphaAnim(NULL),
isEnabled(isEnabled), nameLabel(NULL), unlockNumStarsLabel(NULL), unlockStarColourAnim(NULL), unlockStarGlowPulseAnim(NULL),
lockShakeRotateAnim(NULL), lockShakeTranslateAnim(NULL), lockShrinkAnim(NULL), 
isStarUnlockAnimPlaying(false), isBasicUnlockAnimPlaying(false),
explosionEmitter(NULL), explosionOnoEmitter(NULL), shockwaveEffect(NULL), fireSmokeEmitter1(NULL),
fireSmokeEmitter2(NULL), starLockFadeAnim(NULL), 
lockSnapEmitter(NULL), lockSnapOnoEmitter(NULL), leftLockHalfEmitter(NULL), rightLockHalfEmitter(NULL) {

    assert(state != NULL);
    assert(level != NULL);

    this->lockedAnim.ClearLerp();
    this->lockedAnim.SetInterpolantValue(0);
}

SelectLevelMenuState::AbstractLevelMenuItem::~AbstractLevelMenuItem() {

    delete this->nameLabel;
    this->nameLabel = NULL;

    if (this->unlockNumStarsLabel != NULL) {
        delete this->unlockNumStarsLabel;
        this->unlockNumStarsLabel = NULL;
    }
    if (this->unlockStarColourAnim != NULL) {
        delete this->unlockStarColourAnim;
        this->unlockStarColourAnim = NULL;
    }
    if (this->unlockStarGlowPulseAnim != NULL) {
        delete this->unlockStarGlowPulseAnim;
        this->unlockStarGlowPulseAnim = NULL;
    }

    if (this->starShrinkAnim != NULL) {
        delete this->starShrinkAnim;
        this->starShrinkAnim = NULL;
    }
    if (this->starAlphaAnim != NULL) {
        delete this->starAlphaAnim;
        this->starAlphaAnim = NULL;
    }
    if (this->lockShakeRotateAnim != NULL) {
        delete this->lockShakeRotateAnim;
        this->lockShakeRotateAnim = NULL;
    }
    if (this->lockShakeTranslateAnim != NULL) {
        delete this->lockShakeTranslateAnim;
        this->lockShakeTranslateAnim = NULL;
    }
    if (this->lockShrinkAnim != NULL) {
        delete this->lockShrinkAnim;
        this->lockShrinkAnim = NULL;
    }
    if (this->explosionEmitter != NULL) {
        delete this->explosionEmitter;
        this->explosionEmitter = NULL;
    }
    if (this->explosionOnoEmitter != NULL) {
        delete this->explosionOnoEmitter;
        this->explosionOnoEmitter = NULL;
    }
    if (this->shockwaveEffect != NULL) {
        delete this->shockwaveEffect;
        this->shockwaveEffect = NULL;
    }
    if (this->fireSmokeEmitter1 != NULL) {
        delete this->fireSmokeEmitter1;
        this->fireSmokeEmitter1 = NULL;
    }
    if (this->fireSmokeEmitter2 != NULL) {
        delete this->fireSmokeEmitter2;
        this->fireSmokeEmitter2 = NULL;
    }

    if (this->starLockFadeAnim != NULL) {
        delete this->starLockFadeAnim;
        this->starLockFadeAnim = NULL;
    }

    if (this->lockSnapEmitter != NULL) {
        delete this->lockSnapEmitter;
        this->lockSnapEmitter = NULL;
    }
    if (this->lockSnapOnoEmitter != NULL) {
        delete this->lockSnapOnoEmitter;
        this->lockSnapOnoEmitter = NULL;
    }
    if (this->leftLockHalfEmitter != NULL) {
        delete this->leftLockHalfEmitter;
        this->leftLockHalfEmitter = NULL;
    }
    if (this->rightLockHalfEmitter != NULL) {
        delete this->rightLockHalfEmitter;
        this->rightLockHalfEmitter = NULL;
    }
}

void SelectLevelMenuState::AbstractLevelMenuItem::RebuildItem(bool enabled, const Point2D& topLeftCorner) {
    this->isEnabled = enabled;
    this->topLeftCorner = topLeftCorner;
}

float SelectLevelMenuState::AbstractLevelMenuItem::GetUnlockStarSize() const {
    return STAR_LOCKED_PADLOCK_SCALE * this->GetHeight() * 0.7f;
}

float SelectLevelMenuState::AbstractLevelMenuItem::GetUnlockStarCenterYOffset() const {
    return -STAR_LOCKED_PADLOCK_SCALE * this->GetHeight() * 0.15f;
}

void SelectLevelMenuState::AbstractLevelMenuItem::DrawBG(bool isSelected) {
    if (!isSelected) {
        float leftX   = this->GetTopLeftCorner()[0] - BORDER_GAP;
        float rightX  = this->GetTopLeftCorner()[0] + this->GetWidth() + BORDER_GAP;
        float topY    = this->GetTopLeftCorner()[1] + BORDER_GAP/2;
        float bottomY = this->GetTopLeftCorner()[1] - this->GetHeight() - BORDER_GAP;

        glColor4f(1,1,1,1);
        glBegin(GL_QUADS);
        glVertex2f(leftX,  bottomY);
        glVertex2f(rightX, bottomY);
        glVertex2f(rightX, topY);
        glVertex2f(leftX,  topY);
        glEnd();

        glLineWidth(2.0f);
        glColor4f(0.4f, 0.6f, 0.8f, 1.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(leftX,  bottomY);
        glVertex2f(rightX, bottomY);
        glVertex2f(rightX, topY);
        glVertex2f(leftX,  topY);
        glEnd();
    }
}

void SelectLevelMenuState::AbstractLevelMenuItem::DrawPadlock(double dT, const Camera& camera) {
    this->lockedAnim.Tick(dT);
    float lockMoveX = this->lockedAnim.GetInterpolantValue();
    
    glPushAttrib(GL_CURRENT_BIT);

    glColor4f(1,1,1,1);
    glPushMatrix();
    float currCenterX = lockMoveX + this->topLeftCorner[0] + this->GetWidth()/2.0f;
    float currCenterY = this->topLeftCorner[1] - this->GetHeight()/2.0f;
    glTranslatef(currCenterX, currCenterY, 0.0f);
    
    float padlockScale = 1.0f;
    if (this->level->GetNumStarsRequiredToUnlock() > 0) {
        padlockScale = static_cast<int>(STAR_LOCKED_PADLOCK_SCALE * this->GetHeight());
    }
    else {
        padlockScale = static_cast<int>(PADLOCK_SCALE * this->GetHeight());
    }
    
    float padlockAlpha = 1.0f;
    float lockRotateAmt = 0.0f;
    Vector2D lockTranslateAmt(0,0); 

    if (this->isStarUnlockAnimPlaying) {
        assert(this->lockShakeRotateAnim != NULL);
        assert(this->lockShakeTranslateAnim != NULL);
        assert(this->lockShrinkAnim != NULL);

        this->lockShakeRotateAnim->Tick(dT);
        this->lockShakeTranslateAnim->Tick(dT);

        padlockAlpha = this->starLockFadeAnim->GetInterpolantValue();
        padlockScale *= this->lockShrinkAnim->GetInterpolantValue();
        lockRotateAmt = this->lockShakeRotateAnim->GetInterpolantValue();
        lockTranslateAmt = padlockScale * this->lockShakeTranslateAnim->GetInterpolantValue();
    }
    else if (this->isBasicUnlockAnimPlaying) {
        padlockAlpha = 0.0f;
    }

    glPushMatrix();
    glTranslatef(lockTranslateAmt[0], lockTranslateAmt[1], 0.0f);
    glRotatef(lockRotateAmt, 0, 0, 1);
    glScalef(padlockScale, padlockScale, 1.0f);
    
    glColor4f(1,1,1,padlockAlpha);
    this->state->padlockTexture->BindTexture();
    GeometryMaker::GetInstance()->DrawQuad();
    glPopMatrix();
    
    if (!this->level->GetAreUnlockStarsPaidFor()) {

        float starAlpha = 1.0f;
        float starSize = this->GetUnlockStarSize();
        float starYOffset = this->GetUnlockStarCenterYOffset();
        currCenterY += starYOffset;

        assert(this->unlockStarColourAnim != NULL);
        this->unlockStarColourAnim->Tick(dT);
        const Colour& starColour = this->unlockStarColourAnim->GetInterpolantValue();

        assert(this->unlockStarGlowPulseAnim != NULL);
        this->unlockStarGlowPulseAnim->Tick(dT);
        float pulseScale = this->unlockStarGlowPulseAnim->GetInterpolantValue();

        assert(this->unlockNumStarsLabel != NULL);
        this->unlockNumStarsLabel->SetTopLeftCorner(currCenterX - this->unlockNumStarsLabel->GetLastRasterWidth()/2.0f, 
            currCenterY + this->unlockNumStarsLabel->GetHeight()/3.0f);

        if (this->isStarUnlockAnimPlaying) {
            assert(this->starShrinkAnim != NULL);
            assert(this->starAlphaAnim != NULL);

            this->starShrinkAnim->Tick(dT);
            starSize *= this->starShrinkAnim->GetInterpolantValue();
            this->starAlphaAnim->Tick(dT);
            starAlpha = this->starAlphaAnim->GetInterpolantValue();
        }

        glPushMatrix();
        glTranslatef(0, starYOffset, 0);
        glScalef(starSize, starSize, 1.0f);

        glPushMatrix();
        glScalef(pulseScale, pulseScale, 1.0f);

        glColor4f(GameViewConstants::GetInstance()->BRIGHT_POINT_STAR_COLOUR.R(),
            GameViewConstants::GetInstance()->BRIGHT_POINT_STAR_COLOUR.G(),
            GameViewConstants::GetInstance()->BRIGHT_POINT_STAR_COLOUR.B(), starAlpha * 0.5f);
        this->state->starGlowTexture->BindTexture();
        GeometryMaker::GetInstance()->DrawQuad();
        glPopMatrix();

        glColor4f(starColour.R(), starColour.G(), starColour.B(), starAlpha);
        this->state->starTexture->BindTexture();
        GeometryMaker::GetInstance()->DrawQuad();
        this->state->starTexture->UnbindTexture();

        glPopMatrix();
    
        this->unlockNumStarsLabel->SetAlpha(starAlpha);
        this->unlockNumStarsLabel->Draw();
    }
    else {
        this->state->padlockTexture->UnbindTexture();
    }

    if (this->isStarUnlockAnimPlaying) {

        assert(this->starLockFadeAnim != NULL);
        assert(this->explosionEmitter != NULL);
        assert(this->explosionOnoEmitter != NULL);

        if (this->lockShrinkAnim->Tick(dT)) {

            this->shockwaveEffect->Tick(dT);
            this->shockwaveEffect->Draw(camera);
            this->fireSmokeEmitter1->Tick(dT);
            this->fireSmokeEmitter1->Draw(camera);
            this->fireSmokeEmitter2->Tick(dT);
            this->fireSmokeEmitter2->Draw(camera);
            this->explosionEmitter->Tick(dT);
            this->explosionEmitter->Draw(camera);
            this->explosionOnoEmitter->Tick(dT);
            this->explosionOnoEmitter->Draw(camera);

            this->starLockFadeAnim->Tick(dT);
            if (!this->isEnabled) {
                this->SetEnabled(true);
            }

            if (this->explosionEmitter->IsDead() && this->isEnabled) {
                // We can now resume player control and general normalcy -- also, now that the player has paid for the
                // the level to be unlocked we unlock it
                this->state->freezePlayerInput = false;
                this->isStarUnlockAnimPlaying = false;
                this->state->levelWasUnlockedViaStarCost = true;
                this->level->SetAreUnlockStarsPaidFor(true);
                GameProgressIO::SaveGameProgress(this->state->display->GetModel());
            }
        }
    }
    else if (this->isBasicUnlockAnimPlaying) {
        assert(this->lockSnapEmitter != NULL);
        assert(this->lockSnapOnoEmitter != NULL);
        assert(this->leftLockHalfEmitter != NULL);
        assert(this->rightLockHalfEmitter != NULL);

        this->leftLockHalfEmitter->Tick(dT);
        this->leftLockHalfEmitter->DrawWithDepth(camera);
        this->rightLockHalfEmitter->Tick(dT);
        this->rightLockHalfEmitter->DrawWithDepth(camera);
        this->lockSnapEmitter->Tick(dT);
        this->lockSnapEmitter->Draw(camera);
        this->lockSnapOnoEmitter->Tick(dT);
        this->lockSnapOnoEmitter->Draw(camera);

        if (!this->isEnabled) {
            this->SetEnabled(true);
        }
        else if (this->leftLockHalfEmitter->IsDead() && this->rightLockHalfEmitter->IsDead()) {
            this->state->freezePlayerInput = false;
            this->isBasicUnlockAnimPlaying = false;
        }
    }

    glPopMatrix();
    glPopAttrib();
}

void SelectLevelMenuState::AbstractLevelMenuItem::ExecuteLockedAnimation() {
    
    std::vector<double> timeValues;
    timeValues.push_back(0.0);
    timeValues.push_back(0.01);
    timeValues.push_back(0.025);
    timeValues.push_back(0.07);
    timeValues.push_back(0.10);
    timeValues.push_back(0.13);
    timeValues.push_back(0.18);
    timeValues.push_back(0.2);

    std::vector<float> movementValues;
    float maxMove = 0.25f * PADLOCK_SCALE * this->GetHeight();
    movementValues.push_back(0.0f);
    movementValues.push_back(maxMove * 0.9f);
    movementValues.push_back(-maxMove * 0.1f);
    movementValues.push_back(-maxMove);
    movementValues.push_back(maxMove * 0.5f);
    movementValues.push_back(maxMove);
    movementValues.push_back(-0.2f * maxMove);
    movementValues.push_back(0.0f);
    this->lockedAnim.SetLerp(timeValues, movementValues);
    this->lockedAnim.SetRepeat(false);
}

void SelectLevelMenuState::AbstractLevelMenuItem::ExecuteUnlockStarsPaidForAnimation() {
    assert(!this->isBasicUnlockAnimPlaying);
    if (this->isStarUnlockAnimPlaying) {
        return;
    }

    // Don't let the user change selection or exit the menu or whatever until the animation is done
    this->state->freezePlayerInput = true;

    const float height = this->GetHeight();

    if (this->starShrinkAnim == NULL) {
        this->starShrinkAnim = new AnimationMultiLerp<float>();
        std::vector<double> timeVals(3);
        timeVals[0] = 0; timeVals[1] = 0.08; timeVals[2] = TOTAL_STAR_UNLOCK_TIME/5.0;
        std::vector<float> scaleVals(timeVals.size());
        scaleVals[0] = 1.0f; scaleVals[1] = 1.25f; scaleVals[2] = 0.01f;

        this->starShrinkAnim->SetLerp(timeVals, scaleVals);
        this->starShrinkAnim->SetInterpolantValue(1.0f);
        this->starShrinkAnim->SetRepeat(false);
    }
    else {
        this->starShrinkAnim->ResetToStart();
    }
    if (this->starAlphaAnim == NULL) {
        this->starAlphaAnim = new AnimationLerp<float>();
        this->starAlphaAnim->SetLerp(TOTAL_STAR_UNLOCK_TIME/10.0, TOTAL_STAR_UNLOCK_TIME/5.0, 1.0f, 0.0f);
        this->starAlphaAnim->SetInterpolantValue(1.0f);
        this->starAlphaAnim->SetRepeat(false);
    }
    else {
        this->starAlphaAnim->ResetToStart();
    }
    
    if (this->lockShakeRotateAnim == NULL) {
        this->lockShakeRotateAnim = new AnimationMultiLerp<float>();

        static const int NUM_ROTS = 10;
        static const double MIN_INTERVAL_TIME = 0.3;
        
        std::vector<double> timeVals(2*NUM_ROTS - 1);
        timeVals[0] = 0.0f;
        std::vector<float> rotVals(timeVals.size());
        rotVals[0] = 0.0f;

        int sign = Randomizer::GetInstance()->RandomNegativeOrPositive();
        for (int i = 0; i < NUM_ROTS-1; i++) {

            double randomIntervalTime = MIN_INTERVAL_TIME + 0.1 * Randomizer::GetInstance()->RandomNumZeroToOne(); 
            timeVals[2*i+1] = timeVals[2*i]   + randomIntervalTime;
            timeVals[2*i+2] = timeVals[2*i+1] + randomIntervalTime;
            
            float currRot = sign * Randomizer::GetInstance()->RandomNumZeroToOne() * 20.0f;
            rotVals[2*i+1]   = currRot;
            rotVals[2*i+2] = currRot;
            sign *= -1;
        }

        this->lockShakeRotateAnim->SetLerp(timeVals, rotVals);
        this->lockShakeRotateAnim->SetRepeat(true);
    }
    else {
        this->lockShakeRotateAnim->ResetToStart();
    }
    
    if (this->lockShakeTranslateAnim == NULL) {
        this->lockShakeTranslateAnim = new AnimationMultiLerp<Vector2D>();

        static const int NUM_SHAKES = 20;
        static const float MAX_MOVE_PERCENT = 0.115f;

        std::vector<double> timeVals(NUM_SHAKES+1);
        std::vector<Vector2D> shakeVals(timeVals.size());
        timeVals[0] = 0.0;
        shakeVals[0] = Vector2D(0,0);
        for (int i = 1; i <= NUM_SHAKES; i++) {
            timeVals[i] = timeVals[i-1] + 0.01 + 0.015*Randomizer::GetInstance()->RandomNumZeroToOne();
            shakeVals[i] = Vector2D(
                Randomizer::GetInstance()->RandomNegativeOrPositive() * MAX_MOVE_PERCENT * (0.005f + Randomizer::GetInstance()->RandomNumZeroToOne()), 
                Randomizer::GetInstance()->RandomNegativeOrPositive() * MAX_MOVE_PERCENT * (0.005f + Randomizer::GetInstance()->RandomNumZeroToOne()));
        }
        
        this->lockShakeTranslateAnim->SetLerp(timeVals, shakeVals);
        this->lockShakeTranslateAnim->SetRepeat(true);
    }
    else {
        this->lockShakeTranslateAnim->ResetToStart();
    }

    static const double LOCK_SHRINK_TIME = TOTAL_STAR_UNLOCK_TIME / 1.5;
    if (this->lockShrinkAnim == NULL) {
        this->lockShrinkAnim = new AnimationLerp<float>();
        this->lockShrinkAnim->SetLerp(0.0, LOCK_SHRINK_TIME, 1.0, PADLOCK_SCALE/STAR_LOCKED_PADLOCK_SCALE);
        this->lockShrinkAnim->SetRepeat(false);
    }
    else {
        this->lockShrinkAnim->ResetToStart();
    }

    if (this->starLockFadeAnim == NULL) {
        this->starLockFadeAnim = new AnimationLerp<float>();
        this->starLockFadeAnim->SetLerp(0.0, 0.1, 1.0f, 0.0f);
        this->starLockFadeAnim->SetInterpolantValue(1.0f);
        this->starLockFadeAnim->SetRepeat(false);
    }
    else {
        this->starLockFadeAnim->ResetToStart();
    }

    static const double EXPLOSION_TIME = TOTAL_STAR_UNLOCK_TIME / 3.0;

    if (this->explosionEmitter == NULL) {
        this->explosionEmitter = new ESPPointEmitter();
        this->explosionEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
        this->explosionEmitter->SetInitialSpd(ESPInterval(0.0f, 0.0f));
        this->explosionEmitter->SetParticleLife(ESPInterval(EXPLOSION_TIME));
        this->explosionEmitter->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
        this->explosionEmitter->SetParticleAlignment(ESP::NoAlignment);
        this->explosionEmitter->SetEmitPosition(Point3D(0,0,0));
        this->explosionEmitter->SetEmitDirection(Vector3D(0,1,0));
        this->explosionEmitter->SetToggleEmitOnPlane(true);
        this->explosionEmitter->SetParticleRotation(ESPInterval(-10.0f, 10.0f));
        this->explosionEmitter->SetParticleSize(ESPInterval(1.25f * PADLOCK_SCALE * height), ESPInterval(1.25f * PADLOCK_SCALE * height));
        this->explosionEmitter->AddEffector(&this->state->particleFader);
        this->explosionEmitter->AddEffector(&this->state->particleMediumGrowth);
        this->explosionEmitter->SetParticles(1, this->state->explosionTex);
    }
    else {
        this->explosionEmitter->Reset();
    }
    
    if (this->explosionOnoEmitter == NULL) {
        this->explosionOnoEmitter = new ESPPointEmitter();
        this->explosionOnoEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
        this->explosionOnoEmitter->SetInitialSpd(ESPInterval(0.0f));
        this->explosionOnoEmitter->SetParticleLife(EXPLOSION_TIME);
        this->explosionOnoEmitter->SetParticleSize(ESPInterval(1.5f*this->state->display->GetTextScalingFactor()));
        this->explosionOnoEmitter->SetParticleRotation(ESPInterval(-20.0f, 20.0f));
        this->explosionOnoEmitter->SetParticleAlignment(ESP::NoAlignment);
        this->explosionOnoEmitter->SetEmitPosition(Point3D(0,0,0));
        this->explosionOnoEmitter->SetEmitDirection(Vector3D(0,1,0));
        this->explosionOnoEmitter->SetToggleEmitOnPlane(true);
        this->explosionOnoEmitter->AddEffector(&this->state->particleFader);
        this->explosionOnoEmitter->AddEffector(&this->state->particleSmallGrowth);
        TextLabel2D bangTextLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium), "");
        bangTextLabel.SetColour(Colour(1, 1, 1));
        bangTextLabel.SetDropShadow(Colour(0, 0, 0), 0.1f);
        this->explosionOnoEmitter->SetParticles(1, bangTextLabel, Onomatoplex::EXPLOSION, Onomatoplex::SUPER_AWESOME, true);
    }
    else {
        this->explosionOnoEmitter->Reset();
    }

    if (this->shockwaveEffect == NULL) {
        this->shockwaveEffect = new ESPPointEmitter();
        this->shockwaveEffect->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
        this->shockwaveEffect->SetInitialSpd(ESPInterval(0.0f, 0.0f));
        this->shockwaveEffect->SetParticleLife(ESPInterval(1.2f*EXPLOSION_TIME));
        this->shockwaveEffect->SetParticleSize(ESPInterval(1.25f * PADLOCK_SCALE * height));
        this->shockwaveEffect->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
        this->shockwaveEffect->SetParticleAlignment(ESP::NoAlignment);
        this->shockwaveEffect->SetEmitPosition(Point3D(0,0,0));
        this->shockwaveEffect->SetEmitDirection(Vector3D(0,1,0));
        this->shockwaveEffect->SetToggleEmitOnPlane(true);
        this->shockwaveEffect->SetParticleColour(ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
        this->shockwaveEffect->AddEffector(&this->state->particleFader);
        this->shockwaveEffect->AddEffector(&this->state->particleSuperGrowth);
        this->shockwaveEffect->SetParticles(1, &this->state->normalTexRefractEffect);
    }
    else {
        this->shockwaveEffect->Reset();
    }


    ESPInterval smokeSize(0.1f * PADLOCK_SCALE * height, 0.75f * PADLOCK_SCALE * height);

    if (this->fireSmokeEmitter1 == NULL) {
        this->fireSmokeEmitter1 = new ESPPointEmitter();
        this->fireSmokeEmitter1->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
        this->fireSmokeEmitter1->SetNumParticleLives(1);
        this->fireSmokeEmitter1->SetInitialSpd(ESPInterval(100.0f, 900.0f));
        this->fireSmokeEmitter1->SetParticleLife(ESPInterval(1.25f, 2.2f));
        this->fireSmokeEmitter1->SetParticleSize(smokeSize);
        this->fireSmokeEmitter1->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
        this->fireSmokeEmitter1->SetParticleAlignment(ESP::NoAlignment);
        this->fireSmokeEmitter1->SetEmitPosition(Point3D(0,0,0));
        this->fireSmokeEmitter1->SetEmitDirection(Vector3D(0,1,0));
        this->fireSmokeEmitter1->SetToggleEmitOnPlane(true);
        this->fireSmokeEmitter1->SetEmitAngleInDegrees(180);
        this->fireSmokeEmitter1->AddEffector(&this->state->particleFireFastColourFader);
        this->fireSmokeEmitter1->AddEffector(&this->state->particleMediumGrowth);
        this->fireSmokeEmitter1->AddEffector(&this->state->smokeRotatorCW);
        this->fireSmokeEmitter1->SetRandomTextureParticles(15, this->state->smokeTextures);
    }
    else {
        this->fireSmokeEmitter1->Reset();
    }
    if (this->fireSmokeEmitter2 == NULL) {
        this->fireSmokeEmitter2 = new ESPPointEmitter();
        this->fireSmokeEmitter2->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
        this->fireSmokeEmitter2->SetNumParticleLives(1);
        this->fireSmokeEmitter2->SetInitialSpd(ESPInterval(100.0f, 900.0f));
        this->fireSmokeEmitter2->SetParticleLife(ESPInterval(1.25f, 2.2f));
        this->fireSmokeEmitter2->SetParticleSize(smokeSize);
        this->fireSmokeEmitter2->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
        this->fireSmokeEmitter2->SetParticleAlignment(ESP::NoAlignment);
        this->fireSmokeEmitter2->SetEmitPosition(Point3D(0,0,0));
        this->fireSmokeEmitter2->SetEmitDirection(Vector3D(0,1,0));
        this->fireSmokeEmitter2->SetToggleEmitOnPlane(true);
        this->fireSmokeEmitter2->SetEmitAngleInDegrees(180);
        this->fireSmokeEmitter2->AddEffector(&this->state->particleFireFastColourFader);
        this->fireSmokeEmitter2->AddEffector(&this->state->particleMediumGrowth);
        this->fireSmokeEmitter2->AddEffector(&this->state->smokeRotatorCCW);
        this->fireSmokeEmitter2->SetRandomTextureParticles(15, this->state->smokeTextures);
    }
    else {
        this->fireSmokeEmitter2->Reset();
    }

    // Play the sound for the unlock event
    this->state->display->GetSound()->PlaySound(GameSound::LevelStarCostPaidUnlockEvent, false, false);

    this->isStarUnlockAnimPlaying = true;
}

void SelectLevelMenuState::AbstractLevelMenuItem::ExecuteBasicUnlockAnimation() {
    assert(!this->isStarUnlockAnimPlaying);
    if (this->isBasicUnlockAnimPlaying) {
        return;
    }

    float height = this->GetHeight();

    // Don't let the user change selection or exit the menu or whatever until the animation is done
    this->state->freezePlayerInput = true;

    static const float SNAP_LIFE = 1.25f;

    if (this->lockSnapEmitter == NULL) {
        this->lockSnapEmitter = new ESPPointEmitter();
        this->lockSnapEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
        this->lockSnapEmitter->SetInitialSpd(ESPInterval(0.0f, 0.0f));
        this->lockSnapEmitter->SetParticleLife(ESPInterval(SNAP_LIFE));
        this->lockSnapEmitter->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
        this->lockSnapEmitter->SetParticleAlignment(ESP::NoAlignment);
        this->lockSnapEmitter->SetEmitPosition(Point3D(0,0,1));
        this->lockSnapEmitter->SetEmitDirection(Vector3D(0,1,0));
        this->lockSnapEmitter->SetToggleEmitOnPlane(true);
        this->lockSnapEmitter->SetParticleRotation(ESPInterval(-180.0f, 180.0f));
        this->lockSnapEmitter->SetParticleSize(ESPInterval(PADLOCK_SCALE * height), ESPInterval(0.75f * PADLOCK_SCALE * height));
        this->lockSnapEmitter->AddEffector(&this->state->particleFader);
        this->lockSnapEmitter->AddEffector(&this->state->particleMediumGrowth);
        this->lockSnapEmitter->SetParticles(1, this->state->snapTexture);
    }
    else {
        this->lockSnapEmitter->Reset();
    }
    if (this->lockSnapOnoEmitter == NULL) {
        this->lockSnapOnoEmitter = new ESPPointEmitter();
        this->lockSnapOnoEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
        this->lockSnapOnoEmitter->SetInitialSpd(ESPInterval(0.0f));
        this->lockSnapOnoEmitter->SetParticleLife(SNAP_LIFE);
        this->lockSnapOnoEmitter->SetParticleSize(ESPInterval(1.25f*this->state->display->GetTextScalingFactor()));
        this->lockSnapOnoEmitter->SetParticleRotation(ESPInterval(-20.0f, 20.0f));
        this->lockSnapOnoEmitter->SetParticleAlignment(ESP::NoAlignment);
        this->lockSnapOnoEmitter->SetEmitPosition(Point3D(0,0,1));
        this->lockSnapOnoEmitter->SetEmitDirection(Vector3D(0,1,0));
        this->lockSnapOnoEmitter->SetParticleColour(Colour(0,0,0));
        this->lockSnapOnoEmitter->SetToggleEmitOnPlane(true);
        this->lockSnapOnoEmitter->AddEffector(&this->state->particleFader);
        this->lockSnapOnoEmitter->AddEffector(&this->state->particleSmallGrowth);
        TextLabel2D snapLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium), "");
        snapLabel.SetColour(Colour(0, 0, 0));
        snapLabel.SetDropShadow(Colour(1, 1, 1), 0.1f);
        this->lockSnapOnoEmitter->SetParticles(1, snapLabel, Onomatoplex::EXPLOSION, Onomatoplex::GOOD, true);
    }
    else {
        this->lockSnapOnoEmitter->Reset();
    }

    static const float LOCK_BREAK_LIFE = 1.5f;
    static const ESPInterval LOCK_BREAK_SPD(90.0f, 120.0f);

    if (this->leftLockHalfEmitter == NULL) {
        this->leftLockHalfEmitter = new ESPPointEmitter();
        this->leftLockHalfEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
        this->leftLockHalfEmitter->SetInitialSpd(LOCK_BREAK_SPD);
        this->leftLockHalfEmitter->SetParticleLife(ESPInterval(LOCK_BREAK_LIFE));
        this->leftLockHalfEmitter->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
        this->leftLockHalfEmitter->SetParticleAlignment(ESP::NoAlignment);
        this->leftLockHalfEmitter->SetEmitPosition(Point3D(0,0,1));
        this->leftLockHalfEmitter->SetEmitDirection(Vector3D(-1,0,0));
        this->leftLockHalfEmitter->SetToggleEmitOnPlane(true);
        this->leftLockHalfEmitter->SetParticleRotation(ESPInterval(0.0f, 30.0f));
        this->leftLockHalfEmitter->SetParticleSize(ESPInterval(PADLOCK_SCALE * height), ESPInterval(PADLOCK_SCALE * height));
        this->leftLockHalfEmitter->AddEffector(&this->state->particleFlickerFader);
        this->leftLockHalfEmitter->AddEffector(&this->state->lockBreakGravityEffector);
        this->leftLockHalfEmitter->SetParticles(1, this->state->padlockBrokenLeftTexture);
    }
    else {
        this->leftLockHalfEmitter->Reset();
    }
    if (this->rightLockHalfEmitter == NULL) {
        this->rightLockHalfEmitter = new ESPPointEmitter();
        this->rightLockHalfEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
        this->rightLockHalfEmitter->SetInitialSpd(LOCK_BREAK_SPD);
        this->rightLockHalfEmitter->SetParticleLife(ESPInterval(LOCK_BREAK_LIFE));
        this->rightLockHalfEmitter->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
        this->rightLockHalfEmitter->SetParticleAlignment(ESP::NoAlignment);
        this->rightLockHalfEmitter->SetEmitPosition(Point3D(0,0,1));
        this->rightLockHalfEmitter->SetEmitDirection(Vector3D(1,0,0));
        this->rightLockHalfEmitter->SetToggleEmitOnPlane(true);
        this->rightLockHalfEmitter->SetParticleRotation(ESPInterval(-30.0f, 0.0f));
        this->rightLockHalfEmitter->SetParticleSize(ESPInterval(PADLOCK_SCALE * height), ESPInterval(PADLOCK_SCALE * height));
        this->rightLockHalfEmitter->AddEffector(&this->state->particleFlickerFader);
        this->rightLockHalfEmitter->AddEffector(&this->state->lockBreakGravityEffector);
        this->rightLockHalfEmitter->SetParticles(1, this->state->padlockBrokenRightTexture);
    }
    else {
        this->rightLockHalfEmitter->Reset();
    }

    // Play the sound for the unlock event
    this->state->display->GetSound()->PlaySound(GameSound::LevelBasicUnlockEvent, false, false);

    this->isBasicUnlockAnimPlaying = true;
}

const float SelectLevelMenuState::LevelMenuItem::NUM_TO_HIGH_SCORE_Y_GAP = 5;
const float SelectLevelMenuState::LevelMenuItem::HIGH_SCORE_TO_STAR_Y_GAP = 4;
const float SelectLevelMenuState::LevelMenuItem::STAR_GAP = 5;

SelectLevelMenuState::LevelMenuItem::LevelMenuItem(SelectLevelMenuState* state,
                                                   int levelNum, GameLevel* level, 
                                                   float width, const Point2D& topLeftCorner, 
                                                   bool isEnabled) : 
AbstractLevelMenuItem(state, level, width, topLeftCorner, isEnabled), numLabel(NULL), 
activeStarDisplayList(0), inactiveStarDisplayList(0), numStars(-1), starDrawPos(-99999,-99999) {

    float scaleFactor = state->display->GetTextScalingFactor();

    assert(levelNum > 0);

    std::stringstream levelNumStr;
    levelNumStr << levelNum;
    this->numLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, 
        GameFontAssetsManager::Huge), levelNumStr.str());
    this->numLabel->SetScale(state->display->GetTextScalingFactor());

    std::string highScoreStr = "High Score: " + stringhelper::AddNumberCommas(this->level->GetHighScore()) ;
    this->highScoreLabel = new TextLabel2D(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, 
        GameFontAssetsManager::Small), highScoreStr);
    this->highScoreLabel->SetScale(scaleFactor * 0.8f);

    this->RebuildItem(isEnabled, topLeftCorner);
}

SelectLevelMenuState::LevelMenuItem::~LevelMenuItem() {
    delete this->numLabel;
    this->numLabel = NULL;
    delete this->highScoreLabel;
    this->highScoreLabel = NULL;

    glDeleteLists(this->activeStarDisplayList, 1);
    this->activeStarDisplayList = 0;
    glDeleteLists(this->inactiveStarDisplayList, 1);
    this->inactiveStarDisplayList = 0;
}

void SelectLevelMenuState::LevelMenuItem::RebuildItem(bool enabled, const Point2D& topLeftCorner) {
    AbstractLevelMenuItem::RebuildItem(enabled, topLeftCorner);

    this->numLabel->SetTopLeftCorner(this->topLeftCorner);

    float scaleFactor = this->state->display->GetTextScalingFactor();
    float nameLabelWidth = this->width - NUM_TO_NAME_GAP - this->numLabel->GetLastRasterWidth();

    std::string levelName;
    if (isEnabled) {
        levelName = this->level->GetName();
    }
    else {
        levelName = std::string("???");
    }

    if (this->nameLabel != NULL) {
        delete this->nameLabel;
    }

    this->nameLabel = new TextLabel2DFixedWidth(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, 
        GameFontAssetsManager::Small), nameLabelWidth, levelName);
    this->nameLabel->SetLineSpacing(4.0f);
    this->nameLabel->SetScale(scaleFactor);

    float nameXPos = this->topLeftCorner[0] + NUM_TO_NAME_GAP + this->numLabel->GetLastRasterWidth();
    float nameYPos = this->topLeftCorner[1] - std::max<float>(0, ((this->numLabel->GetHeight() - this->nameLabel->GetHeight()) / 2.0f));
    this->nameLabel->SetTopLeftCorner(nameXPos, nameYPos);

    if (isEnabled) {
        this->nameLabel->SetColour(Colour(0,0,0));
        this->numLabel->SetColour(Colour(0.2f, 0.6f, 1.0f));
        this->numLabel->SetDropShadow(Colour(0.0f, 0.0f, 0.0f), 0.04f);
    }
    else {
        this->nameLabel->SetColour(DISABLED_COLOUR);
        this->numLabel->SetColour(DISABLED_COLOUR);

        if (this->level->GetNumStarsRequiredToUnlock() > 0) {
            std::stringstream starStrStream;
            starStrStream << this->level->GetNumStarsRequiredToUnlock();

            if (this->unlockNumStarsLabel != NULL) {
                delete this->unlockNumStarsLabel;
            }
            this->unlockNumStarsLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(
                GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium), starStrStream.str());
            this->unlockNumStarsLabel->SetColour(Colour(0,0,0));
            this->unlockNumStarsLabel->SetScale(1.5f * scaleFactor);
            this->unlockNumStarsLabel->SetDropShadow(Colour(1,1,1), 0.05f);

            std::vector<double> timeVals(3);
            timeVals[0] = 0.0; timeVals[1] = 1.0; timeVals[2] = 2.0f;
            std::vector<Colour> colourVals(3);
            colourVals[0] = GameViewConstants::GetInstance()->ACTIVE_POINT_STAR_COLOUR;
            colourVals[1] = GameViewConstants::GetInstance()->BRIGHT_POINT_STAR_COLOUR;
            colourVals[2] = colourVals[0];

            if (this->unlockStarColourAnim != NULL) {
                delete this->unlockStarColourAnim;
            }
            this->unlockStarColourAnim = new AnimationMultiLerp<Colour>();
            this->unlockStarColourAnim->SetLerp(timeVals, colourVals);
            this->unlockStarColourAnim->SetRepeat(true);

            std::vector<float> pulseVals(3);
            pulseVals[0] = 1.2f; pulseVals[1] = 1.4f; pulseVals[2] = pulseVals[0];

            if (this->unlockStarGlowPulseAnim != NULL) {
                delete this->unlockStarGlowPulseAnim;
            }
            this->unlockStarGlowPulseAnim = new AnimationMultiLerp<float>();
            this->unlockStarGlowPulseAnim->SetLerp(timeVals, pulseVals);
            this->unlockStarGlowPulseAnim->SetRepeat(true);
        }
    }

    this->highScoreLabel->SetTopLeftCorner(this->nameLabel->GetTopLeftCorner()[0],
        this->numLabel->GetTopLeftCorner()[1] - (NUM_TO_HIGH_SCORE_Y_GAP + this->numLabel->GetHeight()));

    if (enabled) {
        this->highScoreLabel->SetColour(Colour(0,0,0));
    }
    else {
        this->highScoreLabel->SetColour(DISABLED_COLOUR);
    }

    this->BuildStarDisplayList();
}

void SelectLevelMenuState::LevelMenuItem::BuildStarDisplayList() {
   
    if (this->activeStarDisplayList != 0) {
        glDeleteLists(this->activeStarDisplayList, 1);
        this->activeStarDisplayList = 0;
    }
    if (this->inactiveStarDisplayList != 0) {
        glDeleteLists(this->inactiveStarDisplayList, 1);
        this->inactiveStarDisplayList = 0;
    }

    float nameLabelWidth = this->width - NUM_TO_NAME_GAP - this->numLabel->GetLastRasterWidth();
    this->starSize = (nameLabelWidth - STAR_GAP * (GameLevel::MAX_STARS_PER_LEVEL-1)) / static_cast<float>(GameLevel::MAX_STARS_PER_LEVEL);
    this->numStars = level->GetHighScoreNumStars();
    float halfStarSize = starSize / 2.0f;

    if (isEnabled) {
        this->activeStarDisplayList = glGenLists(1);
        glNewList(this->activeStarDisplayList, GL_COMPILE);

        glPushAttrib(GL_CURRENT_BIT | GL_TEXTURE_BIT);
        glPushMatrix();

        this->starDrawPos[0] = this->highScoreLabel->GetTopLeftCorner()[0] - halfStarSize - STAR_GAP;
        this->starDrawPos[1] = this->highScoreLabel->GetTopLeftCorner()[1] - this->highScoreLabel->GetHeight() - HIGH_SCORE_TO_STAR_Y_GAP - halfStarSize;

        glTranslatef(this->starDrawPos[0], this->starDrawPos[1], 0.0f);
        glTranslatef(0, 0, 0);
        this->state->starTexture->BindTexture();

        for (int i = 0; i < this->numStars; i++) {
            glTranslatef(STAR_GAP + this->starSize, 0, 0);
            glBegin(GL_QUADS);
            glTexCoord2i(0, 0); glVertex2f(-halfStarSize, -halfStarSize);
            glTexCoord2i(1, 0); glVertex2f( halfStarSize, -halfStarSize);
            glTexCoord2i(1, 1); glVertex2f( halfStarSize,  halfStarSize);
            glTexCoord2i(0, 1); glVertex2f(-halfStarSize,  halfStarSize);
            glEnd();
        }

        this->state->starTexture->UnbindTexture();

        glPopMatrix();
        glPopAttrib();
        glEndList();

        this->inactiveStarDisplayList = glGenLists(1);
        glNewList(this->inactiveStarDisplayList, GL_COMPILE);

        glPushAttrib(GL_CURRENT_BIT | GL_TEXTURE_BIT);
        glPushMatrix();

        glTranslatef(this->starDrawPos[0], this->starDrawPos[1], 0.0f);
        glTranslatef(this->numStars * (STAR_GAP + starSize), 0, 0);
        this->state->starTexture->BindTexture();

        for (int i = this->numStars; i < GameLevel::MAX_STARS_PER_LEVEL; i++) {
            glTranslatef(STAR_GAP + starSize, 0, 0);
            glBegin(GL_QUADS);
            glTexCoord2i(0, 0); glVertex2f(-halfStarSize, -halfStarSize);
            glTexCoord2i(1, 0); glVertex2f( halfStarSize, -halfStarSize);
            glTexCoord2i(1, 1); glVertex2f( halfStarSize,  halfStarSize);
            glTexCoord2i(0, 1); glVertex2f(-halfStarSize,  halfStarSize);
            glEnd();
        }

        this->state->starTexture->UnbindTexture();

        glPopMatrix();
        glPopAttrib();
        glEndList();
    }
    else {

        this->inactiveStarDisplayList = glGenLists(1);
        glNewList(this->inactiveStarDisplayList, GL_COMPILE);

        glPushAttrib(GL_CURRENT_BIT | GL_TEXTURE_BIT);
        glPushMatrix();

        glTranslatef(this->highScoreLabel->GetTopLeftCorner()[0], 
            this->highScoreLabel->GetTopLeftCorner()[1] - this->highScoreLabel->GetHeight() - HIGH_SCORE_TO_STAR_Y_GAP, 0.0f);
        glTranslatef(-STAR_GAP - halfStarSize, -halfStarSize, 0);
        this->state->starTexture->BindTexture();

        GLfloat envColour[4] = { 1.0, 1.0, 1.0, 1.0 };
        glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, envColour);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
        glColor4f(DISABLED_COLOUR.R(), DISABLED_COLOUR.G(), DISABLED_COLOUR.B(), 1.0f);
        for (int i = 0; i < GameLevel::MAX_STARS_PER_LEVEL; i++) {
            glTranslatef(STAR_GAP + starSize, 0, 0);
            glBegin(GL_QUADS);
            glTexCoord2i(0, 0); glVertex2f(-halfStarSize, -halfStarSize);
            glTexCoord2i(1, 0); glVertex2f( halfStarSize, -halfStarSize);
            glTexCoord2i(1, 1); glVertex2f( halfStarSize,  halfStarSize);
            glTexCoord2i(0, 1); glVertex2f(-halfStarSize,  halfStarSize);
            glEnd();
        }

        this->state->starTexture->UnbindTexture();

        glPopMatrix();
        glPopAttrib();

        glEndList();
    }
}

float SelectLevelMenuState::LevelMenuItem::GetHeight() const {
    return this->numLabel->GetHeight() + NUM_TO_HIGH_SCORE_Y_GAP +
           this->highScoreLabel->GetHeight() + HIGH_SCORE_TO_STAR_Y_GAP + this->starSize;
}

void SelectLevelMenuState::LevelMenuItem::Draw(const Camera& camera, double dT, bool isSelected) {
    UNUSED_PARAMETER(dT);
    UNUSED_PARAMETER(camera);

    this->DrawBG(isSelected);

    // Draw the level number and name
    this->numLabel->Draw();
    this->nameLabel->Draw();

    // Draw the high score for the level
    this->highScoreLabel->Draw();
    
    // Draw the number of stars earned for the level
    glPushAttrib(GL_CURRENT_BIT);

    if (this->activeStarDisplayList != 0) {
        // If the number of active stars is equal to the max number of stars you can get in a level then
        // we add some extraneous special effects as well...
        if (this->numStars == GameLevel::MAX_STARS_PER_LEVEL) {
            this->state->fiveStarBGGlowEmitter->SetParticleSize(ESPInterval(1.25f*this->starSize));
            glPushMatrix();
            glTranslatef(this->starDrawPos[0], this->starDrawPos[1], 0);
            for (int i = 0; i < this->numStars; i++) {
                glTranslatef(this->starSize + STAR_GAP, 0, 0);
                this->state->fiveStarBGGlowEmitter->Draw(camera);
            }
            glPopMatrix();
        }

        const Colour& starColour = GameViewConstants::GetInstance()->ACTIVE_POINT_STAR_COLOUR;
        glColor4f(starColour.R(), starColour.G(), starColour.B(), 1.0f);
        glCallList(this->activeStarDisplayList);

        if (this->numStars == GameLevel::MAX_STARS_PER_LEVEL) {
            this->state->fiveStarFGSparkleEmitter->SetParticleSize(ESPInterval(this->starSize), true);
            glPushMatrix();
            glTranslatef(this->starDrawPos[0] + this->starSize/8.0f, this->starDrawPos[1] + this->starSize/8.0f, 0);
            for (int i = 0; i < this->numStars; i++) {
                glTranslatef(this->starSize + STAR_GAP, 0, 0);
                this->state->fiveStarFGSparkleEmitter->Draw(camera);
            }
            glPopMatrix();
        }

    }
    if (this->inactiveStarDisplayList != 0) {
        const Colour& starColour = GameViewConstants::GetInstance()->INACTIVE_POINT_STAR_COLOUR;
        glColor4f(starColour.R(), starColour.G(), starColour.B(), 1.0f);
        glCallList(this->inactiveStarDisplayList);
    }

    glPopAttrib();
}

void SelectLevelMenuState::LevelMenuItem::DrawAfter(const Camera& camera, double dT) {
    // When locked, draw the padlock
    if (!this->isEnabled || this->isStarUnlockAnimPlaying || this->isBasicUnlockAnimPlaying) {
        this->DrawPadlock(dT, camera);
    }
}

SelectLevelMenuState::LevelMenuPage::~LevelMenuPage() {
    for (size_t i = 0; i < this->levelItems.size(); i++) {
        AbstractLevelMenuItem* levelItem = this->levelItems.at(i);
        delete levelItem;
        levelItem = NULL;
    }
    this->levelItems.clear();
}

const float SelectLevelMenuState::BossLevelMenuItem::BOSS_ICON_GAP        = 5.0f;
const float SelectLevelMenuState::BossLevelMenuItem::NUM_TO_BOSS_NAME_GAP = 10.0f;
const float SelectLevelMenuState::BossLevelMenuItem::BOSS_NAME_ICON_GAP   = 4.0f;

SelectLevelMenuState::BossLevelMenuItem::BossLevelMenuItem(SelectLevelMenuState* state,
                                                           GameLevel* level,
                                                           float width, float height, const Point2D& topLeftCorner,
                                                           bool isEnabled, const Texture* bossTexture) :
AbstractLevelMenuItem(state, level, width, topLeftCorner, isEnabled), 
height(height), bossTexture(bossTexture), bossDeadLabel(NULL), bossIconDisplayList(0) {

    assert(bossTexture != NULL);

    this->bossLabel = new TextLabel2D(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Huge), "BOSS");
    this->bossLabel->SetScale(0.9f * state->display->GetTextScalingFactor());

    this->RebuildItem(isEnabled, topLeftCorner);
}

SelectLevelMenuState::BossLevelMenuItem::~BossLevelMenuItem() {
    delete this->bossLabel;
    this->bossLabel = NULL;

    if (this->bossDeadLabel != NULL) {
        delete this->bossDeadLabel;
        this->bossDeadLabel = NULL;
    }

    glDeleteLists(this->bossIconDisplayList, 1);
    this->bossIconDisplayList = 0;
}

void SelectLevelMenuState::BossLevelMenuItem::RebuildItem(bool enabled, const Point2D& topLeftCorner) {
    AbstractLevelMenuItem::RebuildItem(enabled, topLeftCorner);

    float scaleFactor = this->state->display->GetTextScalingFactor();
    float nameLabelWidth = this->width - NUM_TO_NAME_GAP;

    std::string levelName;
    if (isEnabled) {
        levelName = this->level->GetName();
    }
    else {
        levelName = std::string("???");
    }

    if (this->nameLabel != NULL) {
        delete this->nameLabel;
    }

    this->nameLabel = new TextLabel2DFixedWidth(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, 
        GameFontAssetsManager::Small), nameLabelWidth, levelName);
    this->nameLabel->SetLineSpacing(4.0f);

    if (isEnabled) {
        this->nameLabel->SetColour(Colour(0,0,0));
    }
    else {
        this->nameLabel->SetColour(DISABLED_COLOUR);

        if (this->level->GetNumStarsRequiredToUnlock() > 0) {
            std::stringstream starStrStream;
            starStrStream << this->level->GetNumStarsRequiredToUnlock();

            if (this->unlockNumStarsLabel != NULL) {
                delete this->unlockNumStarsLabel;
            }
            this->unlockNumStarsLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(
                GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium), starStrStream.str());
            this->unlockNumStarsLabel->SetColour(Colour(0,0,0));
            this->unlockNumStarsLabel->SetScale(1.5f * scaleFactor);
            this->unlockNumStarsLabel->SetDropShadow(Colour(1,1,1), 0.05f);

            std::vector<double> timeVals(3);
            timeVals[0] = 0.0; timeVals[1] = 1.0; timeVals[2] = 2.0f;
            std::vector<Colour> colourVals(3);
            colourVals[0] = GameViewConstants::GetInstance()->ACTIVE_POINT_STAR_COLOUR;
            colourVals[1] = GameViewConstants::GetInstance()->BRIGHT_POINT_STAR_COLOUR;
            colourVals[2] = colourVals[0];

            if (this->unlockStarColourAnim != NULL) {
                delete this->unlockStarColourAnim;
            }
            this->unlockStarColourAnim = new AnimationMultiLerp<Colour>();
            this->unlockStarColourAnim->SetLerp(timeVals, colourVals);
            this->unlockStarColourAnim->SetRepeat(true);

            std::vector<float> pulseVals(3);
            pulseVals[0] = 1.2f; pulseVals[1] = 1.4f; pulseVals[2] = pulseVals[0];

            if (this->unlockStarGlowPulseAnim != NULL) {
                delete this->unlockStarGlowPulseAnim;
            }
            this->unlockStarGlowPulseAnim = new AnimationMultiLerp<float>();
            this->unlockStarGlowPulseAnim->SetLerp(timeVals, pulseVals);
            this->unlockStarGlowPulseAnim->SetRepeat(true);
        }
    }

    
    this->bossIconSize = this->bossLabel->GetHeight();
    float bossXPos = this->topLeftCorner[0] + this->bossIconSize + NUM_TO_NAME_GAP;
 
    this->bossLabel->SetTopLeftCorner(bossXPos, this->topLeftCorner[1]);
    this->nameLabel->SetTopLeftCorner(bossXPos,
        this->topLeftCorner[1] - (NUM_TO_BOSS_NAME_GAP + std::max<float>(this->bossLabel->GetHeight(), this->bossIconSize)));
    this->nameLabel->SetFont(GameFontAssetsManager::GetInstance()->GetFont(
        GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Big));
    this->nameLabel->SetScale(std::min<float>(1.25f, (this->width - NUM_TO_NAME_GAP - this->bossIconSize) / this->nameLabel->GetWidth()));
    this->nameLabel->SetFixedWidth((this->width - NUM_TO_NAME_GAP - this->bossIconSize));

    if (enabled) {
        this->bossLabel->SetColour(Colour(1, 0, 0));
        this->bossLabel->SetDropShadow(Colour(0.0f, 0.0f, 0.0f), 0.04f);

        if (this->bossDeadLabel != NULL) {
            delete this->bossDeadLabel;
            this->bossDeadLabel = NULL;
        }

        // If the boss has already been defeated then we draw a big 'X' over its icon
        if (this->level->GetIsLevelPassedWithScore()) {
            this->bossDeadLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(
                GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Big), "X");
            this->bossDeadLabel->SetScale(1.5f * this->state->display->GetTextScalingFactor());
            this->bossDeadLabel->SetColour(Colour(1, 0, 0));
            this->bossDeadLabel->SetTopLeftCorner(
                this->topLeftCorner[0] + (this->bossIconSize - this->bossDeadLabel->GetLastRasterWidth()) / 2.0f,
                this->topLeftCorner[1] - (this->bossIconSize - this->bossDeadLabel->GetHeight()) / 2.0f);
        }
    }
    else {
        this->bossLabel->SetColour(DISABLED_COLOUR);
    }

    this->BuildBossIconDisplayList();
}

void SelectLevelMenuState::BossLevelMenuItem::BuildBossIconDisplayList() {
    
    if (this->bossIconDisplayList != 0) {
        glDeleteLists(this->bossIconDisplayList, 1);
    }

    this->bossIconSize = this->bossLabel->GetHeight();
    float halfBossIconSize = bossIconSize / 2.0f;

    // Boss Icon Stuff
    this->bossIconDisplayList = glGenLists(1);
    glNewList(this->bossIconDisplayList, GL_COMPILE);

    glPushAttrib(GL_CURRENT_BIT | GL_TEXTURE_BIT);
    glPushMatrix();

    float iconXPos = this->topLeftCorner[0] + halfBossIconSize;
    float iconYPos = this->topLeftCorner[1] - halfBossIconSize;

    glTranslatef(iconXPos, iconYPos, 0.0f);

    this->bossTexture->BindTexture();

    if (this->isEnabled) {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else {
        GLfloat envColour[4] = { 1.0, 1.0, 1.0, 1.0 };
        glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, envColour);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
        glColor4f(DISABLED_COLOUR.R(), DISABLED_COLOUR.G(), DISABLED_COLOUR.B(), 1.0f);
    }
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2f(-halfBossIconSize, -halfBossIconSize);
    glTexCoord2i(1, 0); glVertex2f( halfBossIconSize, -halfBossIconSize);
    glTexCoord2i(1, 1); glVertex2f( halfBossIconSize,  halfBossIconSize);
    glTexCoord2i(0, 1); glVertex2f(-halfBossIconSize,  halfBossIconSize);
    glEnd();

    this->bossTexture->UnbindTexture();

    glPopMatrix();
    glPopAttrib();

    glEndList();
}

void SelectLevelMenuState::BossLevelMenuItem::Draw(const Camera& camera, double dT, bool isSelected) {
    UNUSED_PARAMETER(dT);
    UNUSED_PARAMETER(camera);

    this->DrawBG(isSelected);

    // Draw the level number and name
    this->nameLabel->Draw();
    this->bossLabel->Draw();
    
    // Draw the boss icon
    glCallList(this->bossIconDisplayList);

    if (this->isEnabled && !this->isStarUnlockAnimPlaying && !this->isBasicUnlockAnimPlaying && this->bossDeadLabel != NULL) {
        this->bossDeadLabel->Draw();
    }
}

void SelectLevelMenuState::BossLevelMenuItem::DrawAfter(const Camera& camera, double dT) {
    if (!this->isEnabled || this->isStarUnlockAnimPlaying || this->isBasicUnlockAnimPlaying) {
        this->DrawPadlock(dT, camera);
    }
}

float SelectLevelMenuState::BossLevelMenuItem::GetHeight() const {
    return static_cast<int>(std::max<float>(this->height, NUM_TO_BOSS_NAME_GAP + 
        std::max<float>(this->bossLabel->GetHeight(), this->bossIconSize) + this->nameLabel->GetHeight()));
}
