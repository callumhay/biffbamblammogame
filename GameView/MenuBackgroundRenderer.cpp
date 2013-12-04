/**
 * MenuBackgroundRenderer.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "MenuBackgroundRenderer.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/GeometryMaker.h"
#include "../ResourceManager.h"

MenuBackgroundRenderer::MenuBackgroundRenderer() : bgEffect(NULL) {

    Texture* fgTex = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_STARFIELD_FG, Texture::Trilinear);
    Texture* bgTex = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_STARFIELD_BG, Texture::Trilinear);

    this->bgEffect = new CgFxSkybox(fgTex, bgTex);
    this->bgEffect->SetMoveFrequency(0.007f);
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