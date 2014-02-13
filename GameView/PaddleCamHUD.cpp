/**
 * PaddleCamHUD.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2014
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "PaddleCamHUD.h"
#include "GameAssets.h"
#include "GameViewConstants.h"

const float PaddleCamHUD::PADDLE_CAM_FG_KEY_LIGHT_Y_OFFSET = -8.0f;

PaddleCamHUD::PaddleCamHUD(GameAssets& assets) : boostMalfunctionHUD(NULL) {
    this->boostMalfunctionHUD = new MalfunctionTextHUD(GameViewConstants::BOOST_MALFUNCTION_TEXT, assets.GetSound());
}

PaddleCamHUD::~PaddleCamHUD() {
    delete this->boostMalfunctionHUD;
    this->boostMalfunctionHUD = NULL;
}

void PaddleCamHUD::ActivateBoostMalfunctionHUD() {
    this->boostMalfunctionHUD->Activate();
}

void PaddleCamHUD::Activate() {

}

void PaddleCamHUD::Deactivate() {
    this->boostMalfunctionHUD->Deactivate();
}

void PaddleCamHUD::Reinitialize() {
    this->boostMalfunctionHUD->Reset();
}