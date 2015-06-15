/**
 * GameTutorialAssets.cpp
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

#include "GameTutorialAssets.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Texture2D.h"
#include "../ResourceManager.h"


GameTutorialAssets::GameTutorialAssets() : 
xbox360ButtonTex(NULL), xbox360AnalogStickTex(NULL),
keyboardShortButtonTex(NULL), keyboardLongButtonTex(NULL),
leftMouseButtonTex(NULL), rightMouseButtonTex(NULL),  boostTutorialHUDTex(NULL),
boostTutorialDirTex(NULL), multiplierTutorialTex(NULL) {
    this->Init();
}

GameTutorialAssets::~GameTutorialAssets() {
    this->Release();
}

const Texture2D* GameTutorialAssets::GetXBox360Texture(GameViewConstants::XBoxButtonType buttonType) const {
    switch (buttonType) {
        case GameViewConstants::XBoxPushButton:
            return this->GetXBox360ButtonTexture();
        case GameViewConstants::XBoxAnalogStick:
            return this->GetXBox360AnalogStickTexture();
        case GameViewConstants::XBoxTrigger:
            return this->GetXBox360TriggerTexture();
        default:
            assert(false);
            break;
    }
    return NULL;
}

const Texture2D* GameTutorialAssets::GetArcadeTexture(GameViewConstants::ArcadeButtonType buttonType) const {
    switch (buttonType) {
        case GameViewConstants::ArcadeJoystick:
            return this->GetAracdeJoystickTexture();
        case GameViewConstants::ArcadeBoostButton:
        case GameViewConstants::ArcadeFireButton:
            return this->GetXBox360ButtonTexture();
        default:
            assert(false);
            break;
    }
    return NULL;
}

const Texture2D* GameTutorialAssets::GetKeyboardTexture(GameViewConstants::KeyboardButtonType buttonType) const {
    switch (buttonType) {
        case GameViewConstants::KeyboardChar:
            return this->GetKeyboardShortButtonTexture();
        case GameViewConstants::KeyboardSpaceBar:
            return this->GetKeyboardLongButtonTexture();
        default:
            assert(false);
            break;
    }
    return NULL;
}

const Texture2D* GameTutorialAssets::GetMouseTexture(GameViewConstants::MouseButtonType buttonType) const {
    switch (buttonType) {
        case GameViewConstants::LeftMouseButton:
            return this->GetLeftMouseButtonTexture();
        case GameViewConstants::RightMouseButton:
            return this->GetRightMouseButtonTexture();
        default:
            assert(false);
            break;
    }
    return NULL;
}

void GameTutorialAssets::Init() {
    this->xbox360ButtonTex = 
        dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_XBOX_CONTROLLER_BUTTON, Texture::Trilinear));
    assert(this->xbox360ButtonTex != NULL);

    this->xbox360AnalogStickTex =
        dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_XBOX_CONTROLLER_ANALOG_STICK, Texture::Trilinear));
    assert(this->xbox360AnalogStickTex != NULL);

    this->xbox360TriggerTex =
        dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_XBOX_CONTROLLER_TRIGGER, Texture::Trilinear));
    assert(this->xbox360TriggerTex != NULL);

    this->arcadeJoystickTex =
        dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_ARCADE_JOYSTICK, Texture::Trilinear));
    assert(this->arcadeJoystickTex != NULL);

    this->keyboardShortButtonTex =
        dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_SHORT_KEYBOARD_KEY, Texture::Trilinear));
    assert(this->keyboardShortButtonTex != NULL);

    this->keyboardLongButtonTex =
        dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_LONG_KEYBOARD_KEY, Texture::Trilinear));
    assert(this->keyboardLongButtonTex != NULL);

    this->leftMouseButtonTex =
        dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_LEFT_MOUSE_BUTTON, Texture::Trilinear));
    assert(this->leftMouseButtonTex != NULL);

    this->rightMouseButtonTex =
        dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_RIGHT_MOUSE_BUTTON, Texture::Trilinear));
    assert(this->rightMouseButtonTex != NULL);


    this->boostTutorialHUDTex = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_BOOST_HUD_TUTORIAL, Texture::Trilinear));
    assert(this->boostTutorialHUDTex != NULL);
    
    this->boostTutorialDirTex = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_BOOST_DIR_TUTORIAL, Texture::Trilinear));
    assert(this->boostTutorialDirTex != NULL);

    this->multiplierTutorialTex = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_MULTIPLIER_TUTORIAL, Texture::Trilinear));
    assert(this->boostTutorialDirTex != NULL);


    this->lifeTutorialHUDTex = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_LIFE_HUD_TUTORIAL, Texture::Trilinear));
    assert(this->boostTutorialHUDTex != NULL);
    
    this->lifeTutorialItemDropTex = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_LIFE_ITEM_DROP_TUTORIAL, Texture::Trilinear));
    assert(this->boostTutorialDirTex != NULL);

    this->boostTutorialItemTex = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_BOOST_TUTORIAL_ITEM, Texture::Trilinear));
    assert(this->boostTutorialHUDTex != NULL);

    this->multiplierTutorialItemTex = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_MULTIPLIER_TUTORIAL_ITEM, Texture::Trilinear));
    assert(this->boostTutorialDirTex != NULL);
    
    this->lifeTutorialItemTex = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_LIFE_TUTORIAL_ITEM, Texture::Trilinear));
    assert(this->boostTutorialDirTex != NULL);

}

void GameTutorialAssets::Release() {
    bool success = true;
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->xbox360ButtonTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->xbox360AnalogStickTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->xbox360TriggerTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->arcadeJoystickTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->keyboardShortButtonTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->keyboardLongButtonTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->leftMouseButtonTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->rightMouseButtonTex);
    assert(success);

    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->boostTutorialHUDTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->boostTutorialDirTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->multiplierTutorialTex);
    assert(success);

    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->lifeTutorialHUDTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->lifeTutorialItemDropTex);
    assert(success);

    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->boostTutorialItemTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->multiplierTutorialItemTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->lifeTutorialItemTex);
    assert(success);
}