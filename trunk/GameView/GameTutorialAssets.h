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

    const Texture2D* GetXBox360ButtonTexture() const;
    const Texture2D* GetXBox360AnalogStickTexture() const;
    const Texture2D* GetKeyboardShortButtonTexture() const;
    const Texture2D* GetKeyboardLongButtonTexture() const;
        
private:
    Texture2D* xbox360ButtonTex;
    Texture2D* xbox360AnalogStickTex;
    Texture2D* keyboardShortButtonTex;
    Texture2D* keyboardLongButtonTex;

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

inline const Texture2D* GameTutorialAssets::GetKeyboardShortButtonTexture() const {
    return this->keyboardShortButtonTex;
}

inline const Texture2D* GameTutorialAssets::GetKeyboardLongButtonTexture() const {
    return this->keyboardLongButtonTex;
}

#endif // __GAMETUTORIALASSETS_H__