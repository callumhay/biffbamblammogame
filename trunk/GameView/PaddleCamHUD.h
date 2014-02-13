/**
 * PaddleCamHUD.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2014
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __PADDLECAMHUD_H__
#define __PADDLECAMHUD_H__

#include "MalfunctionTextHUD.h"
#include "../GameModel/GameModel.h"

class GameAssets;

class PaddleCamHUD {
public:
    static const float PADDLE_CAM_FG_KEY_LIGHT_Y_OFFSET;

    explicit PaddleCamHUD(GameAssets& assets);
    ~PaddleCamHUD();

    bool GetIsBoostMalfunctionHUDActive() const;

    void Draw(double dT, const Camera& camera, const GameModel& gameModel);

    void ActivateBoostMalfunctionHUD();
    void Activate();
    void Deactivate();
    void Reinitialize();

private:
    MalfunctionTextHUD* boostMalfunctionHUD;

    DISALLOW_COPY_AND_ASSIGN(PaddleCamHUD);
};

inline bool PaddleCamHUD::GetIsBoostMalfunctionHUDActive() const {
    return this->boostMalfunctionHUD->GetIsActive();
}

inline void PaddleCamHUD::Draw(double dT, const Camera& camera, const GameModel& gameModel) {
    UNUSED_PARAMETER(gameModel);
    this->boostMalfunctionHUD->Draw(dT, camera);
}

#endif // __PADDLECAMHUD_H__