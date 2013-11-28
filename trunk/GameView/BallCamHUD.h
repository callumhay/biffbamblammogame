/**
 * BallCamHUD.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __BALLCAMHUD_H__
#define __BALLCAMHUD_H__

#include "ButtonTutorialHint.h"
#include "CountdownHUD.h"
#include "BoostMalfunctionHUD.h"

class GameAssets;

class BallCamHUD {
public:
    BallCamHUD(GameAssets& assets, BoostMalfunctionHUD* boostMalfunctionHUD);
    ~BallCamHUD();

    bool GetIsMalfunctionHUDActive() const;

    void Draw(double dT, const Camera& camera);

    void ActivateBoostMalfunctionHUD();
    void ToggleCannonHUD(bool activate, const CannonBlock* cannon);
    void Deactivate();
    void Reinitialize();

private:
    static const float ROTATE_HINT_BOTTOM_FROM_SCREEN_BOTTOM;

    BoostMalfunctionHUD* boostMalfunctionHUD; // Not owned by this object
   
    const CannonBlock* cannon;
    bool cannonHUDActive;
    CountdownHUD cannonCountdown;
    ButtonTutorialHint cannonRotateHint;
    ButtonTutorialHint cannonFireHint;

    // Misc. Textures and overlays
    Texture* barrelOverlayTex;	// Texture for overlay of the cannon barrel
    AnimationLerp<float> overlayFadeAnim;
    AnimationMultiLerp<ColourRGBA> arrowColourAnim;

    void DrawCannonHUD(double dT, const Camera& camera);
    void DrawCannonBarrelOverlay(double dT, float alpha);

    DISALLOW_COPY_AND_ASSIGN(BallCamHUD);
};

inline bool BallCamHUD::GetIsMalfunctionHUDActive() const {
    return this->boostMalfunctionHUD->GetIsActive();
}

inline void BallCamHUD::Draw(double dT, const Camera& camera) {

    // Cannon HUD drawing
    this->DrawCannonHUD(dT, camera);

    // Boost malfunction HUD drawing
    this->boostMalfunctionHUD->Draw(dT, camera);
}

inline void BallCamHUD::ActivateBoostMalfunctionHUD() {
    if (!this->cannonHUDActive) {
        this->boostMalfunctionHUD->Activate();
    }
}

inline void BallCamHUD::Deactivate() {
    this->ToggleCannonHUD(false, NULL);
    this->boostMalfunctionHUD->Deactivate();
}

#endif // __BALLCAMHUD_H__