/**
 * MenuBackgroundRenderer.h
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

#ifndef __MENUBACKGROUNDRENDERER_H__
#define __MENUBACKGROUNDRENDERER_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Texture.h"

#include "CgFxSkybox.h"

class Camera;

class MenuBackgroundRenderer {
public:
    MenuBackgroundRenderer();
    ~MenuBackgroundRenderer();

    void DrawBG(const Camera& camera, float alpha = 1.0f);
    void DrawShakeBG(const Camera& camera, float shakeX, float shakeY, float alpha = 1.0f);
    void DrawNonAnimatedFadeBG(float alpha);
    void DrawNonAnimatedFadeShakeBG(float shakeX, float shakeY, float alpha);

    //void UseEffectWithDrawCall(const Camera& camera, boost::function<void()> drawFunc);

    Texture* GetMenuBGTexture() const;

private:
    CgFxSkybox* bgEffect;
    GLuint fullscreenQuadDL;

    static void DrawFadeOverlayWithTex(int width, int height, float alpha, const Texture* tex, float shakeS = 0, float shakeT = 0);

    DISALLOW_COPY_AND_ASSIGN(MenuBackgroundRenderer);
};

//inline void MenuBackgroundRenderer::UseEffectWithDrawCall(const Camera& camera, boost::function<void()> drawFunc) {
//    this->bgEffect->SetAlpha(1.0f);
//    this->bgEffect->Draw(camera, drawFunc);
//}

inline void MenuBackgroundRenderer::DrawNonAnimatedFadeBG(float alpha) {
    DrawFadeOverlayWithTex(Camera::GetWindowWidth(), Camera::GetWindowHeight(), alpha, this->GetMenuBGTexture());
}

inline void MenuBackgroundRenderer::DrawNonAnimatedFadeShakeBG(float shakeX, float shakeY, float alpha) {
    DrawFadeOverlayWithTex(Camera::GetWindowWidth(), Camera::GetWindowHeight(), alpha, this->GetMenuBGTexture(), shakeX, shakeY);
}

inline Texture* MenuBackgroundRenderer::GetMenuBGTexture() const {
    return this->bgEffect->GetBGSkyTexture();
}

#endif // __MENUBACKGROUNDRENDERER_H__