/**
 * MenuBackgroundRenderer.cpp
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

#include "MenuBackgroundRenderer.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/GeometryMaker.h"
#include "../ResourceManager.h"

const float MenuBackgroundRenderer::DEFAULT_MOVE_FREQUENCY = 0.007f;

MenuBackgroundRenderer::MenuBackgroundRenderer() : bgEffect(NULL) {

    Texture* fgTex = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_STARFIELD_FG, Texture::Trilinear);
    Texture* bgTex = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_STARFIELD_BG, Texture::Trilinear);

    this->bgEffect = new CgFxSkybox(fgTex, bgTex);
    this->bgEffect->SetMoveFrequency(DEFAULT_MOVE_FREQUENCY);
    this->bgEffect->SetFGScale(1.0f);

    glNewList(this->fullscreenQuadDL, GL_COMPILE); 

    GeometryMaker::GetInstance()->DrawTiledFullScreenQuad(Camera::GetWindowWidth(), Camera::GetWindowHeight(), 
        GameViewConstants::STARRY_BG_TILE_MULTIPLIER * static_cast<float>(Camera::GetWindowWidth()) / static_cast<float>(bgTex->GetWidth()),
        GameViewConstants::STARRY_BG_TILE_MULTIPLIER * static_cast<float>(Camera::GetWindowHeight()) / static_cast<float>(bgTex->GetHeight()),
        ColourRGBA(1,1,1,1));

    glEndList();
}

MenuBackgroundRenderer::~MenuBackgroundRenderer() {
    delete this->bgEffect;
    this->bgEffect = NULL;
    glDeleteLists(this->fullscreenQuadDL, 1);
}

void MenuBackgroundRenderer::DrawBG(const Camera& camera, float alpha) {
    
    glPushAttrib(GL_ENABLE_BIT | GL_TRANSFORM_BIT);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, Camera::GetWindowWidth(), 0, Camera::GetWindowHeight());

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    this->bgEffect->SetAlpha(alpha);
    this->bgEffect->Draw(camera, this->fullscreenQuadDL);

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glPopAttrib();

    debug_opengl_state();
}

void MenuBackgroundRenderer::DrawShakeBG(const Camera& camera, float shakeX, float shakeY, float alpha) {

    this->bgEffect->SetUVTranslation(shakeX, shakeY);
    this->DrawBG(camera, alpha);
    this->bgEffect->SetUVTranslation(0, 0);
}

void MenuBackgroundRenderer::DrawFadeOverlayWithTex(int width, int height, float alpha, const Texture* tex, 
                                                    float shakeS, float shakeT) {
    // Draw the fade quad overlay
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    tex->BindTexture();
    GeometryMaker::GetInstance()->DrawTiledFullScreenQuad(width, height, 
        GameViewConstants::STARRY_BG_TILE_MULTIPLIER * static_cast<float>(width) / static_cast<float>(tex->GetWidth()),
        GameViewConstants::STARRY_BG_TILE_MULTIPLIER * static_cast<float>(height) / static_cast<float>(tex->GetHeight()),
        ColourRGBA(1,1,1, alpha), shakeS, shakeT);
    tex->UnbindTexture();

    glPopAttrib();
}