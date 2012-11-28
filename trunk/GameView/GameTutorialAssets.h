/**
 * GameTutorialAssets.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __GAMETUTORIALASSETS_H__
#define __GAMETUTORIALASSETS_H__

#include "GameViewConstants.h"
#include "../BlammoEngine/BasicIncludes.h"


class Texture2D;

class GameTutorialAssets {
public:
    GameTutorialAssets();
    ~GameTutorialAssets();

    const Texture2D* GetXBox360Texture(GameViewConstants::XBoxButtonType buttonType) const;
    const Texture2D* GetKeyboardTexture(GameViewConstants::KeyboardButtonType buttonType) const;
    const Texture2D* GetMouseTexture(GameViewConstants::MouseButtonType buttonType) const;

    const Texture2D* GetXBox360ButtonTexture() const;
    const Texture2D* GetXBox360AnalogStickTexture() const;
    const Texture2D* GetXBox360TriggerTexture() const;
    const Texture2D* GetKeyboardShortButtonTexture() const;
    const Texture2D* GetKeyboardLongButtonTexture() const;
    const Texture2D* GetLeftMouseButtonTexture() const;
    const Texture2D* GetRightMouseButtonTexture() const;
    
    const Texture2D* GetBoostTutorialHUDTexture() const;
    const Texture2D* GetBoostTutorialDirTexture() const;
    const Texture2D* GetMultiplierTutorialTexture() const;

    const Texture2D* GetBoostTutorialItemTexture() const;
    const Texture2D* GetMultiplierTutorialItemTexture() const;
    const Texture2D* GetLifeTutorialItemTexture() const;

private:
    Texture2D* xbox360ButtonTex;
    Texture2D* xbox360AnalogStickTex;
    Texture2D* xbox360TriggerTex;
    Texture2D* keyboardShortButtonTex;
    Texture2D* keyboardLongButtonTex;
    Texture2D* leftMouseButtonTex;
    Texture2D* rightMouseButtonTex;

    Texture2D* boostTutorialHUDTex;
    Texture2D* boostTutorialDirTex;
    Texture2D* multiplierTutorialTex;

    Texture2D* boostTutorialItemTex;
    Texture2D* multiplierTutorialItemTex;
    Texture2D* lifeTutorialItemTex;

    void Init();
    void Release();

    DISALLOW_COPY_AND_ASSIGN(GameTutorialAssets);
};

inline const Texture2D* GameTutorialAssets::GetXBox360ButtonTexture() const {
    return this->xbox360ButtonTex;
}

inline const Texture2D* GameTutorialAssets::GetXBox360AnalogStickTexture() const {
    return this->xbox360AnalogStickTex;
}

inline const Texture2D* GameTutorialAssets::GetXBox360TriggerTexture() const {
    return this->xbox360TriggerTex;
}

inline const Texture2D* GameTutorialAssets::GetKeyboardShortButtonTexture() const {
    return this->keyboardShortButtonTex;
}

inline const Texture2D* GameTutorialAssets::GetKeyboardLongButtonTexture() const {
    return this->keyboardLongButtonTex;
}

inline const Texture2D* GameTutorialAssets::GetLeftMouseButtonTexture() const {
    return this->leftMouseButtonTex;
}

inline const Texture2D* GameTutorialAssets::GetRightMouseButtonTexture() const {
    return this->rightMouseButtonTex;
}

inline const Texture2D* GameTutorialAssets::GetBoostTutorialHUDTexture() const {
    return this->boostTutorialHUDTex;
}

inline const Texture2D* GameTutorialAssets::GetBoostTutorialDirTexture() const {
    return this->boostTutorialDirTex;
}

inline const Texture2D* GameTutorialAssets::GetMultiplierTutorialTexture() const {
    return this->multiplierTutorialTex;
}

inline const Texture2D* GameTutorialAssets::GetBoostTutorialItemTexture() const {
    return this->boostTutorialItemTex;
}

inline const Texture2D* GameTutorialAssets::GetMultiplierTutorialItemTexture() const {
    return this->multiplierTutorialItemTex;
}

inline const Texture2D* GameTutorialAssets::GetLifeTutorialItemTexture() const {
    return this->lifeTutorialItemTex;
}

#endif // __GAMETUTORIALASSETS_H__