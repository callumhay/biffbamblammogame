/**
 * GameSound.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include <irrKlang.h>

#include "GameSound.h"
#include "MSFReader.h"
#include "Sound.h"
#include "SoundEffect.h"
#include "SoundSource.h"

#include "../BlammoEngine/Camera.h"
#include "../GameView/GameViewConstants.h"
#include "../ConfigOptions.h"

GameSound::GameSound() : soundEngine(NULL), currLoadedWorldStyle(GameWorld::None) {
    this->soundEngine = irrklang::createIrrKlangDevice();
    assert(this->soundEngine != NULL);
}

GameSound::~GameSound() {

    this->ClearSounds();
    this->ClearSoundSources();

    // ALWAYS KILL THE ENGINE LAST!
    if (this->soundEngine != NULL) {
        this->soundEngine->drop();
        this->soundEngine = NULL;
    }

    assert(this->globalSounds.empty());
    assert(this->worldSounds.empty());
    assert(this->globalEffects.empty());
}

bool GameSound::Init() {

    if (this->soundEngine == NULL) {
        return false;
    }

    // Load the pallet of all game sounds (this will not load the actual sounds, just their proxies).
    // N.B., Sounds are loaded after this function call by calling the Load*Sounds functions of GameSound.
    bool success = MSFReader::ReadMSF(*this, GameViewConstants::GetInstance()->GAME_SOUND_SCRIPT);
    if (!success) {
        // Clean up...
        this->ClearSoundSources();
        return false;
    }

    // Set the master volume from the initial configuration options
    ConfigOptions cfgOptions = ResourceManager::ReadConfigurationOptions(true);
    this->SetMasterVolume(static_cast<float>(cfgOptions.GetVolume()) / 100.0f);

    return success;
}

void GameSound::Tick(double dT) {

    // Go through all the currently playing sounds, tick them, and clean up any that have finished playing
    for (SoundMapIter iter = this->nonAttachedPlayingSounds.begin(); iter != this->nonAttachedPlayingSounds.end();) {
        
        Sound* currSound = iter->second;
        currSound->Tick(dT);
        
        if (currSound->IsFinished()) {
            delete currSound;
            currSound = NULL;
            iter = this->nonAttachedPlayingSounds.erase(iter);
        }
        else {
            ++iter;
        }
    }

}

void GameSound::LoadGlobalSounds() {
    for (SoundSourceMapIter iter = this->globalSounds.begin(); iter != this->globalSounds.end();) {
        SoundSource* currSoundSrc = iter->second;
        assert(currSoundSrc != NULL);
        if (!currSoundSrc->Load()) {
            delete currSoundSrc;
            currSoundSrc = NULL;
            iter = this->globalSounds.erase(iter);
        }
        else {
            ++iter;
        }
    }
}

void GameSound::LoadWorldSounds(const GameWorld::WorldStyle& world) {

    // Go through all world sounds, make sure we load the given world's sounds and unload all other world's sounds
    for (WorldSoundSourceMapIter iter1 = this->worldSounds.begin(); iter1 != this->worldSounds.end(); ++iter1) {
        SoundSourceMap& relevantWorldSoundSourceMap = iter1->second;
        if (iter1->first == world) {
            for (SoundSourceMapIter iter2 = relevantWorldSoundSourceMap.begin(); iter2 != relevantWorldSoundSourceMap.end();) {
                SoundSource* currSoundSrc = iter2->second;
                assert(currSoundSrc != NULL);
                if (!currSoundSrc->Load()) {
                    delete currSoundSrc;
                    currSoundSrc = NULL;
                    iter2 = relevantWorldSoundSourceMap.erase(iter2);
                }
                else {
                    ++iter2;
                }
            }
        }
        else {
            for (SoundSourceMapIter iter2 = relevantWorldSoundSourceMap.begin(); iter2 != relevantWorldSoundSourceMap.end(); ++iter2) {
                SoundSource* currSoundSrc = iter2->second;
                assert(currSoundSrc != NULL);
                currSoundSrc->Unload();
            }
        }
    }

    this->currLoadedWorldStyle = world;
}

/**
 * Reload every sound source from the game's MSF file.
 */
void GameSound::ReloadFromMSF() {
    if (this->soundEngine == NULL) {
        return;
    }

    // TODO...
    // Figure out what sounds are looping, we'll need to restart these after the load...
    //std::list<GameSound::SoundType> loopingSounds;
    

}

void GameSound::StopAllSounds() {
    if (this->soundEngine == NULL) {
        return;
    }
    this->soundEngine->stopAllSounds();
    this->ClearSounds();
}

void GameSound::PauseAllSounds() {
    if (this->soundEngine == NULL) {
        return;
    }
    this->soundEngine->setAllSoundsPaused(true);
}

void GameSound::UnpauseAllSounds() {
    if (this->soundEngine == NULL) {
        return;
    }
    this->soundEngine->setAllSoundsPaused(false);
}

SoundID GameSound::PlaySound(const GameSound::SoundType& soundType, bool isLooped, double fadeInTimeInSecs) {
    assert(soundType != GameSound::NoSound);
    assert(fadeInTimeInSecs == 0.0); // TODO: add this functionality...

    if (this->soundEngine == NULL) {
        return INVALID_SOUND_ID;
    }

    // Try to find the source associated with the given sound type
    SoundSource* source = this->GetSoundSourceFromType(soundType);
    if (source == NULL) {
        return INVALID_SOUND_ID;
    }

    Sound* newSound = source->Spawn2DSound(isLooped);
    if (newSound == NULL) {
        assert(false);
        return INVALID_SOUND_ID;
    }
    
    this->nonAttachedPlayingSounds.insert(std::make_pair(newSound->GetSoundID(), newSound));
    return newSound->GetSoundID();
}

void GameSound::StopSound(SoundID soundID, double fadeOutTimeInSecs) {
    if (this->soundEngine == NULL || soundID == INVALID_SOUND_ID) {
        return;
    }

    Sound* sound = this->GetPlayingSound(soundID);
    if (sound == NULL) {
        return;
    }

    if (fadeOutTimeInSecs > 0.0) {
        sound->SetFadeout(fadeOutTimeInSecs);
    }
    else {
        sound->Stop();
    }
}

void GameSound::SetMasterVolume(float volume) {
    if (this->soundEngine == NULL) {
        return;
    }
    if (volume < 0.0f || volume > 1.0f) {
        assert(false);
        return;
    }

    this->soundEngine->setSoundVolume(volume);
}

void GameSound::SetListenerPosition(const Camera& camera) {
    if (this->soundEngine == NULL) {
        return;
    }

    const Point3D& pos      = camera.GetCurrentCameraPosition();
    const Vector3D& lookDir = camera.GetNormalizedViewVector();
    const Vector3D& upVec   = camera.GetNormalizedUpVector();

    this->soundEngine->setListenerPosition(
        irrklang::vec3df(pos[0], pos[1], pos[2]),
        irrklang::vec3df(lookDir[0], lookDir[1], lookDir[2]), irrklang::vec3df(0,0,0),
        irrklang::vec3df(upVec[0], upVec[1], upVec[2]));
}

bool GameSound::IsSoundPlaying(SoundID soundID) const {
    return this->nonAttachedPlayingSounds.find(soundID) != this->nonAttachedPlayingSounds.end();
}

SoundSource* GameSound::BuildSoundSource(const std::string& soundName, const std::string& filePath) {
    if (this->soundEngine == NULL) {
        assert(false);
        return NULL;
    }

    SoundSource* result = new SoundSource(this->soundEngine, soundName, filePath);
    return result;
}

void GameSound::ClearSounds() {
    for (SoundMapIter iter = this->nonAttachedPlayingSounds.begin(); iter != this->nonAttachedPlayingSounds.end(); ++iter) {
        Sound* currSound = iter->second;
        delete currSound;
    }
    this->nonAttachedPlayingSounds.clear();
}

void GameSound::ClearSoundSources() {
    if (this->soundEngine == NULL) {
        return;
    }

    // Clean up all the sound sources and any active sounds...
    
    // Clear all global sounds
    for (SoundSourceMapIter iter = this->globalSounds.begin(); iter != this->globalSounds.end(); ++iter) {
        SoundSource* currSoundSrc = iter->second;
        delete currSoundSrc;
        currSoundSrc = NULL;
    }
    this->globalSounds.clear();

    // Clear all world sounds
    for (WorldSoundSourceMapIter iter1 = this->worldSounds.begin(); iter1 != this->worldSounds.end(); ++iter1) {
        SoundSourceMap& soundMap = iter1->second;
        for (SoundSourceMapIter iter2 = soundMap.begin(); iter2 != soundMap.end(); ++iter2) {
            SoundSource* currSoundSrc = iter2->second;
            delete currSoundSrc;
            currSoundSrc = NULL;
        }
        soundMap.clear();
    }
    this->worldSounds.clear();

    // Clear all sound effects
    for (EffectMapIter iter = this->globalEffects.begin(); iter != this->globalEffects.end(); ++iter) {
        SoundEffect* effect = iter->second;
        delete effect;
        effect = NULL;
    }
    this->globalEffects.clear();
}

SoundSource* GameSound::GetSoundSourceFromType(const GameSound::SoundType& type) {

    // Try the global sounds...
    SoundSourceMapIter findIter = this->globalSounds.find(type);
    if (findIter != this->globalSounds.end()) {
        return findIter->second;
    }

    // If no world is loaded then we won't find it in that map either
    if (this->currLoadedWorldStyle == GameWorld::None) {
        return NULL;
    }

    // Not in the global sounds, try to find it among the world sounds...
    SoundSourceMap& currWorldSoundSourceMap = this->worldSounds[this->currLoadedWorldStyle];
    findIter = currWorldSoundSourceMap.find(type);
    if (findIter != currWorldSoundSourceMap.end()) {
        return findIter->second;
    }

    return NULL;
}

Sound* GameSound::GetPlayingSound(SoundID soundID) {
    SoundMapIter findIter = this->nonAttachedPlayingSounds.find(soundID);
    if (findIter == this->nonAttachedPlayingSounds.end()) {
        return NULL;
    }

    return findIter->second;
}