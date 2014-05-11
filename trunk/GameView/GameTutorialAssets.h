/**
 * GameTutorialAssets.h
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

    const Texture2D* GetLifeTutorialHUDTexture() const;
    const Texture2D* GetLifeTutorialItemDropTexture() const;

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

    Texture2D* lifeTutorialHUDTex;
    Texture2D* lifeTutorialItemDropTex;

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

inline const Texture2D* GameTutorialAssets::GetLifeTutorialHUDTexture() const {
    return this->lifeTutorialHUDTex;
}

inline const Texture2D* GameTutorialAssets::GetLifeTutorialItemDropTexture() const {
    return this->lifeTutorialItemDropTex;
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