/**
 * MenuBackgroundRenderer.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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
    void DrawNonAnimatedFadeBG(float alpha);

    //void UseEffectWithDrawCall(const Camera& camera, boost::function<void()> drawFunc);

    Texture* GetMenuBGTexture() const;

private:
    CgFxSkybox* bgEffect;
    GLuint fullscreenQuadDL;

    static void DrawFadeOverlayWithTex(int width, int height, float alpha, const Texture* tex);

    DISALLOW_COPY_AND_ASSIGN(MenuBackgroundRenderer);
};

//inline void MenuBackgroundRenderer::UseEffectWithDrawCall(const Camera& camera, boost::function<void()> drawFunc) {
//    this->bgEffect->SetAlpha(1.0f);
//    this->bgEffect->Draw(camera, drawFunc);
//}

inline void MenuBackgroundRenderer::DrawNonAnimatedFadeBG(float alpha) {
    DrawFadeOverlayWithTex(Camera::GetWindowWidth(), Camera::GetWindowHeight(), alpha, this->GetMenuBGTexture());
}

inline Texture* MenuBackgroundRenderer::GetMenuBGTexture() const {
    return this->bgEffect->GetSkyTexture();
}

#endif // __MENUBACKGROUNDRENDERER_H__