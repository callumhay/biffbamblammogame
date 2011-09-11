/**
 * GameTutorialAssets.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameTutorialAssets.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Texture2D.h"
#include "../ResourceManager.h"


GameTutorialAssets::GameTutorialAssets() : 
xbox360ButtonTex(NULL), xbox360AnalogStickTex(NULL),
keyboardShortButtonTex(NULL), keyboardLongButtonTex(NULL),
leftMouseButtonTex(NULL), rightMouseButtonTex(NULL) {
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
}

void GameTutorialAssets::Release() {
    bool success = true;
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->xbox360ButtonTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->xbox360AnalogStickTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->xbox360TriggerTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->keyboardShortButtonTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->keyboardLongButtonTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->leftMouseButtonTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->rightMouseButtonTex);
    assert(success);
}