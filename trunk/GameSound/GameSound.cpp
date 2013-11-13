/**
 * GameSound.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include <irrKlang.h>

#include "GameSound.h"
#include "MSFReader.h"
#include "Sound.h"
#include "SoundEffect.h"
#include "SingleSoundSource.h"
#include "RandomSoundSource.h"

#include "../BlammoEngine/Camera.h"
#include "../GameView/GameViewConstants.h"
#include "../ConfigOptions.h"

const float GameSound::DEFAULT_MIN_3D_SOUND_DIST       = 1000.0f;
const float GameSound::DEFAULT_3D_SOUND_ROLLOFF_FACTOR = 0.1f;

GameSound::GameSound() : soundEngine(NULL), currLoadedWorldStyle(GameWorld::None), 
levelTranslation(0,0,0), gameFGTransform(), ignorePlaySounds(false)
{
    this->soundEngine = irrklang::createIrrKlangDevice();
    this->soundEngine->setRolloffFactor(DEFAULT_3D_SOUND_ROLLOFF_FACTOR);
    this->soundEngine->setDefault3DSoundMinDistance(DEFAULT_MIN_3D_SOUND_DIST);
    
    assert(this->soundEngine != NULL);
}

GameSound::~GameSound() {

    this->ClearAll();

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
    // Load from the game's MSF file, it defines where all sounds can be found and their settings
    if (!this->LoadFromMSF()) {
        return false;
    }

    // Set the master volume from the initial configuration options
    ConfigOptions cfgOptions = ResourceManager::ReadConfigurationOptions(true);
    this->SetMasterVolume(static_cast<float>(cfgOptions.GetVolume()) / 100.0f);

    return true;
}

void GameSound::Tick(double dT) {

    // Go through all the currently playing sounds, tick them, and clean up any that have finished playing

    // Non-attached sounds...
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

    // Attached sounds...
    for (AttachedSoundMapIter iter1 = this->attachedPlayingSounds.begin(); iter1 != this->attachedPlayingSounds.end();) {
        
        const IPositionObject* positionObj = iter1->first;
        SoundMap& soundMap = iter1->second.soundMap;
        Point3D pos = this->gameFGTransform * (positionObj->GetPosition3D() + iter1->second.localTranslation);

        for (SoundMapIter iter2 = soundMap.begin(); iter2 != soundMap.end();) {
            
            Sound* currSound = iter2->second;
            currSound->Tick(dT);

            if (currSound->IsFinished()) {
                delete currSound;
                currSound = NULL;
                iter2 = soundMap.erase(iter2);
            }
            else {
                currSound->SetPosition(pos);
                ++iter2;
            }
        }

        if (soundMap.empty()) {
            iter1 = this->attachedPlayingSounds.erase(iter1);
        }
        else {
            ++iter1;
        }
    }

}

void GameSound::LoadGlobalSounds() {
    for (SoundSourceMapIter iter = this->globalSounds.begin(); iter != this->globalSounds.end();) {
        
        AbstractSoundSource* currSoundSrc = iter->second;
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
                AbstractSoundSource* currSoundSrc = iter2->second;
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
                AbstractSoundSource* currSoundSrc = iter2->second;
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

    // Figure out what sounds are looping, we'll need to restart these after the reload...
    std::list<std::pair<GameSound::SoundType, SoundID> > loopingSounds;
    for (SoundMapIter iter = this->nonAttachedPlayingSounds.begin(); iter != this->nonAttachedPlayingSounds.end(); ++iter) {
        Sound* currSound = iter->second;
        if (currSound->IsLooped() && !currSound->IsFinished() && !currSound->IsFadingOut()) {
            loopingSounds.push_back(std::make_pair(currSound->GetSoundType(), currSound->GetSoundID()));
        }
    }

    // Figure out what effects are active, we'll need to restart these after the reload...
    EffectSet effectsThatWereActive = this->activeEffects;

    // Clear all effects, sounds and sound sources...
    this->ClearAll();

    // Reload the game's MSF (Music Script File)
    if (!this->LoadFromMSF()) {
        return;
    }

    // Reload all global sources and world sources
    this->LoadGlobalSounds();
    this->LoadWorldSounds(this->currLoadedWorldStyle);

    // Setup all of the previously active effects...
    for (EffectSetIter iter = effectsThatWereActive.begin(); iter != effectsThatWereActive.end(); ++iter) {
        this->ToggleSoundEffect(*iter, true);
    }

    // Play anything that was looped before
    for (std::list<std::pair<GameSound::SoundType, SoundID> >::iterator iter = loopingSounds.begin();
         iter != loopingSounds.end(); ++iter) {

        const std::pair<GameSound::SoundType, SoundID>& currPair = *iter;
        this->PlaySoundWithID(currPair.second, currPair.first, true);
    }
}

void GameSound::StopAllSounds(double fadeOutTimeInSecs) {
    if (this->soundEngine == NULL) {
        return;
    }

    if (fadeOutTimeInSecs == 0.0) {
        // Simple clean-up of all the sounds
        this->soundEngine->stopAllSounds();
        this->ClearSounds();
    }
    else {
        
        // We have to go though all the sounds and add a fade-out each one...
        for (SoundMapIter iter = this->nonAttachedPlayingSounds.begin(); iter != this->nonAttachedPlayingSounds.end(); ++iter) {
            Sound* currSound = iter->second;
            currSound->SetFadeout(fadeOutTimeInSecs);
        }

        for (AttachedSoundMapIter iter1 = this->attachedPlayingSounds.begin(); iter1 != this->attachedPlayingSounds.end(); ++iter1) {
            SoundMap& soundMap = iter1->second.soundMap;
            for (SoundMapIter iter2 = soundMap.begin(); iter2 != soundMap.end(); ++iter2) {
                Sound* currSound = iter2->second;
                currSound->SetFadeout(fadeOutTimeInSecs);
            }
        }
    }
}

void GameSound::StopAllSoundLoops(double fadeOutTimeInSecs) {
    if (this->soundEngine == NULL) {
        return;
    }
    
    // Clear all looped non-attached sounds from memory
    for (SoundMapIter iter = this->nonAttachedPlayingSounds.begin(); iter != this->nonAttachedPlayingSounds.end();) {
        Sound* currSound = iter->second;
        if (currSound->IsLooped()) {
            
            if (fadeOutTimeInSecs > 0.0) {
                currSound->SetFadeout(fadeOutTimeInSecs);
            }
            else {
                delete currSound;
                currSound = NULL;
                iter = this->nonAttachedPlayingSounds.erase(iter);
                continue;
            }
        }

        ++iter;
    }

    // Clear all looped attached sounds from memory
    for (AttachedSoundMapIter iter1 = this->attachedPlayingSounds.begin(); iter1 != this->attachedPlayingSounds.end();) {
        SoundMap& soundMap = iter1->second.soundMap;
        for (SoundMapIter iter2 = soundMap.begin(); iter2 != soundMap.end();) {
            Sound* currSound = iter2->second;
            if (currSound->IsLooped()) {
                if (fadeOutTimeInSecs > 0.0) {
                    currSound->SetFadeout(fadeOutTimeInSecs);
                }
                else {
                    delete currSound;
                    currSound = NULL;
                    iter2 = soundMap.erase(iter2);
                    continue;
                }
            }
            
            ++iter2;
        }
        if (soundMap.empty()) {
            iter1 = this->attachedPlayingSounds.erase(iter1);
        }
        else {
            ++iter1;
        }
    }
}

void GameSound::StopAllSoundsExcept(const std::set<GameSound::SoundType>& exceptSounds, double fadeOutTimeInSecs) {
    if (this->soundEngine == NULL) {
        return;
    }

    // Clear all looped non-attached sounds from memory
    for (SoundMapIter iter = this->nonAttachedPlayingSounds.begin(); iter != this->nonAttachedPlayingSounds.end();) {
        Sound* currSound = iter->second;
        if (exceptSounds.find(currSound->GetSoundType()) != exceptSounds.end()) {
            ++iter;
            continue;
        }

        if (fadeOutTimeInSecs > 0.0) {
            currSound->SetFadeout(fadeOutTimeInSecs);
        }
        else {
            delete currSound;
            currSound = NULL;
            iter = this->nonAttachedPlayingSounds.erase(iter);
            continue;
        }
 
        ++iter;
    }

    // Clear all attached sounds from memory
    for (AttachedSoundMapIter iter1 = this->attachedPlayingSounds.begin(); iter1 != this->attachedPlayingSounds.end();) {
        SoundMap& soundMap = iter1->second.soundMap;
        for (SoundMapIter iter2 = soundMap.begin(); iter2 != soundMap.end();) {
            Sound* currSound = iter2->second;
            if (exceptSounds.find(currSound->GetSoundType()) != exceptSounds.end()) {
                ++iter2;
                continue;
            }

            if (fadeOutTimeInSecs > 0.0) {
                currSound->SetFadeout(fadeOutTimeInSecs);
            }
            else {
                delete currSound;
                currSound = NULL;
                iter2 = soundMap.erase(iter2);
                continue;
            }

            ++iter2;
        }
        if (soundMap.empty()) {
            iter1 = this->attachedPlayingSounds.erase(iter1);
        }
        else {
            ++iter1;
        }
    }
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

// Plays a non-positional sound in the game.
// Returns: The ID of the sound that was created, INVALID_SOUND_ID if it failed to create a sound.
SoundID GameSound::PlaySound(const GameSound::SoundType& soundType, bool isLooped, bool applyActiveEffects, float volume) {
    if (this->ignorePlaySounds) {
        return INVALID_SOUND_ID;
    }

    Sound* newSound = this->BuildSound(soundType, isLooped, NULL, applyActiveEffects, true);
    if (newSound == NULL) {
        return INVALID_SOUND_ID;
    }
    newSound->SetVolume(volume);
    newSound->SetPause(false);
    
    this->nonAttachedPlayingSounds.insert(std::make_pair(newSound->GetSoundID(), newSound));
    return newSound->GetSoundID();
}

// Plays a positional sound in the game.
// Returns: The ID of the sound that was created, INVALID_SOUND_ID if it failed to create a sound.
SoundID GameSound::PlaySoundAtPosition(const GameSound::SoundType& soundType, bool isLooped, const Point3D& position, 
                                       bool applyActiveEffects, bool applyLevelTranslation, bool applyGameFGTransform,
                                       float minDistance, float volume) {

    if (this->ignorePlaySounds) {
        return INVALID_SOUND_ID;
    }

    Point3D transformedPos = position;
    if (applyLevelTranslation) {
        transformedPos = transformedPos + this->levelTranslation;
    }
    if (applyGameFGTransform) {
        transformedPos = this->gameFGTransform * transformedPos;
    }

    Sound* newSound = this->BuildSound(soundType, isLooped, &transformedPos, applyActiveEffects, true);
    if (newSound == NULL) {
        return INVALID_SOUND_ID;
    }
    newSound->SetMinimumDistance(minDistance);
    newSound->SetVolume(volume);
    newSound->SetPause(false);

    this->nonAttachedPlayingSounds.insert(std::make_pair(newSound->GetSoundID(), newSound));
    return newSound->GetSoundID();
}

// Stops the sound with the given ID, it doesn't matter if the sound is attached to an object or not,
// the sound will be stopped.
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

void GameSound::StopAllSoundsWithType(const GameSound::SoundType& soundType, double fadeOutTimeInSecs) {
    if (this->soundEngine == NULL) {
        return;
    }

    for (SoundMapIter iter = this->nonAttachedPlayingSounds.begin(); iter != this->nonAttachedPlayingSounds.end(); ++iter) {
        Sound* currSound = iter->second;
        if (currSound->GetSoundType() == soundType) {
            if (fadeOutTimeInSecs > 0.0) {
                currSound->SetFadeout(fadeOutTimeInSecs);
            }
            else {
                currSound->Stop();
            }
        }
    }

    for (AttachedSoundMapIter iter1 = this->attachedPlayingSounds.begin(); iter1 != this->attachedPlayingSounds.end(); ++iter1) {
        SoundMap& soundMap = iter1->second.soundMap;
        for (SoundMapIter iter2 = soundMap.begin(); iter2 != soundMap.end(); ++iter2) {
            Sound* currSound = iter2->second;
            if (currSound->GetSoundType() == soundType) {
                if (fadeOutTimeInSecs > 0.0) {
                    currSound->SetFadeout(fadeOutTimeInSecs);
                }
                else {
                    currSound->Stop();
                }
            }
        }
    }
}

// NOTE: This is only meant for in-game foreground objects that have sound attached to them!!!
SoundID GameSound::AttachAndPlaySound(const IPositionObject* posObj, const GameSound::SoundType& soundType, 
                                      bool isLooped, const Vector3D& localTranslation, float volume) {

    if (this->ignorePlaySounds) {
        return INVALID_SOUND_ID;
    }

    // Check to see if there's already the same position object with the same sound type attached to it, if so
    // check to see if it's looped, if it is then we don't attach more than one looping sound of the same type to the object!
    if (isLooped) {
        AttachedSoundMapIter findIter = this->attachedPlayingSounds.find(posObj);
        if (findIter != this->attachedPlayingSounds.end()) {
            const SoundMap& soundMap = findIter->second.soundMap;
            // Search through the sound map for the same sound type...
            for (SoundMapConstIter iter = soundMap.begin(); iter != soundMap.end(); ++iter) {
                const Sound* sound = iter->second;
                if (sound->GetSoundType() == soundType && sound->IsLooped()) {
                    return INVALID_SOUND_ID;
                }
            }
        }
    }

    Point3D position = posObj->GetPosition3D();
    Sound* newSound = this->BuildSound(soundType, isLooped, &position, true, true);
    if (newSound == NULL) {
        return INVALID_SOUND_ID;
    }

    newSound->SetVolume(volume);
    newSound->SetPause(false);

    // Attach the new sound to the object and return its ID
    AttachedSoundInfo& soundInfo = this->attachedPlayingSounds[posObj];
    soundInfo.soundMap.insert(std::make_pair(newSound->GetSoundID(), newSound));
    soundInfo.localTranslation = localTranslation;

    return newSound->GetSoundID();
}

/**
 * Completely gets rid of the given positional object from the game sound -- this will stop
 * all sounds that are currently attached to that object and the object itself will no longer
 * be tracked.
 */
void GameSound::DetachAndStopAllSounds(const IPositionObject* posObj, double fadeOutTimeInSecs) {
    // Try to find the given object among the attached playing sounds
    AttachedSoundMapIter findIter = this->attachedPlayingSounds.find(posObj);
    if (findIter == this->attachedPlayingSounds.end()) {
        return;
    }
    
    // Kill all the sounds for that object and then erase it from the attached sounds map
    AttachedSoundInfo& info = findIter->second;
    if (fadeOutTimeInSecs == 0.0) {
        info.ClearSoundMap();
    }
    else {
        SoundMap& objSoundMap = info.soundMap;

        for (SoundMapIter iter = objSoundMap.begin(); iter != objSoundMap.end(); ++iter) {

            // Move each sound from the attached object to the map of non-attached sounds
            Sound* currSound = iter->second;
            currSound->SetFadeout(fadeOutTimeInSecs);
            this->nonAttachedPlayingSounds.insert(std::make_pair(currSound->GetSoundID(), currSound));
        }

        objSoundMap.clear();
    }

    this->attachedPlayingSounds.erase(findIter);
}

void GameSound::DetachAndStopSound(const IPositionObject* posObj, const GameSound::SoundType& soundType, 
                                   double fadeOutTimeInSecs) {

    // Try to find the given object among the attached playing sounds
    AttachedSoundMapIter findIter = this->attachedPlayingSounds.find(posObj);
    if (findIter == this->attachedPlayingSounds.end()) {
        return;
    }

    // Find the given sound type among those attached to the object...
    AttachedSoundInfo& info = findIter->second;
    SoundMap& objSoundMap = info.soundMap;

    for (SoundMapIter iter = objSoundMap.begin(); iter != objSoundMap.end();) {
        Sound* currSound = iter->second;

        if (currSound->GetSoundType() == soundType) {
            if (fadeOutTimeInSecs == 0.0) {
                delete currSound;
            }
            else {
                // Set the fade out and move the sound over to the other playing sounds...
                currSound->SetFadeout(fadeOutTimeInSecs);
                this->nonAttachedPlayingSounds.insert(std::make_pair(currSound->GetSoundID(), currSound));
            }
            iter = objSoundMap.erase(iter);
        }
        else {
            ++iter;
        }
    }

    // Check to see if there are any sounds left for the object, if not we remove it entirely
    if (objSoundMap.empty()) {
        this->attachedPlayingSounds.erase(findIter);
    }
}

void GameSound::SetPauseForAllAttachedSounds(const IPositionObject* posObj, bool isPaused) {
    // Try to find the given object among the attached playing sounds
    AttachedSoundMapIter findIter = this->attachedPlayingSounds.find(posObj);
    if (findIter == this->attachedPlayingSounds.end()) {
        return;
    }
    
    // Kill all the sounds for that object and then erase it from the attached sounds map
    AttachedSoundInfo& info = findIter->second;
    SoundMap& objSoundMap = info.soundMap;
    for (SoundMapIter iter = objSoundMap.begin(); iter != objSoundMap.end(); ++iter) {
        Sound* currSound = iter->second;
        currSound->SetPause(isPaused);
    }
}

void GameSound::StopAllEffects() {
    this->activeEffects.clear();
    this->pausedEffects.clear();

    std::list<Sound*> allPlayingSounds;
    this->GetAllPlayingSoundsAsList(allPlayingSounds);
    for (std::list<Sound*>::iterator iter = allPlayingSounds.begin(); iter != allPlayingSounds.end(); ++iter) {
        Sound* currSound = *iter;
        currSound->StopAllEffects();
    }
}

void GameSound::PauseAllEffects() {
    EffectSet previouslyActiveEffects = this->activeEffects;
    this->StopAllEffects();
    this->pausedEffects = previouslyActiveEffects;
}

void GameSound::UnpauseAllEffects() {
    for (EffectSetConstIter iter = this->pausedEffects.begin(); iter != this->pausedEffects.end(); ++iter) {
        this->ToggleSoundEffect(*iter, true);
    }
    this->pausedEffects.clear();
}

void GameSound::ToggleSoundEffect(const GameSound::EffectType& effectType, bool effectOn, 
                                  const std::set<SoundID>& ignoreSounds) {

    // If we're turning the effect on and it's already active then just ignore it
    if (this->IsEffectActive(effectType) && effectOn) {
        return;
    }
    
    SoundEffect* effect = this->GetSoundEffectFromType(effectType);
    if (effect == NULL) {
        return;
    }

    // Apply the effect to EVERY sound playing in the game other than the provided ignore sounds...
    std::list<Sound*> allPlayingSounds;
    this->GetAllPlayingSoundsAsList(allPlayingSounds);
    for (std::list<Sound*>::iterator iter = allPlayingSounds.begin(); iter != allPlayingSounds.end();) {
        Sound* currSound = *iter;
        if (ignoreSounds.find(currSound->GetSoundID()) != ignoreSounds.end()) {
            iter = allPlayingSounds.erase(iter);
        }
        else {
            ++iter;
        }
    } 
    effect->ToggleEffectOnSounds(allPlayingSounds, effectOn);

    // Also deal with memoized effect data...
    if (effectOn) {
        this->activeEffects.insert(effectType); 
    }
    else {
        this->activeEffects.erase(effectType);
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

void GameSound::SetSoundVolume(const SoundID& soundID, float volume) {
    if (this->soundEngine == NULL) {
        return;
    }
    Sound* sound = this->GetPlayingSound(soundID);
    if (sound == NULL) {
        return;
    }
    sound->SetVolume(volume);
}

void GameSound::SetSoundTypeVolume(const GameSound::SoundType& soundType, float volume) {
    std::list<Sound*> allPlayingSounds;
    this->GetAllPlayingSoundsAsList(allPlayingSounds);
    for (std::list<Sound*>::iterator iter = allPlayingSounds.begin(); iter != allPlayingSounds.end(); ++iter) {
        Sound* currSound = *iter;
        if (currSound->GetSoundType() == soundType) {
            currSound->SetVolume(volume);
        }
    } 
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

AbstractSoundSource* GameSound::BuildSoundSource(const GameSound::SoundType& soundType,
                                                 const std::string& soundName, const std::vector<std::string>& filePaths) {

    if (this->soundEngine == NULL || filePaths.empty()) {
        assert(false);
        return NULL;
    }

    if (filePaths.size() == 1) {
        return new SingleSoundSource(this->soundEngine, soundType, soundName, filePaths.front());
    }
    else {
        return new RandomSoundSource(this->soundEngine, soundType, soundName, filePaths);
    }
}

SoundEffect* GameSound::BuildSoundEffect(const GameSound::EffectType& effectType, 
                                         const std::string& effectName, const std::vector<std::string>& effectsStrs,
                                         const SoundEffect::EffectParameterMap& parameterMap) {
    UNUSED_PARAMETER(effectName);
    UNUSED_PARAMETER(effectType);
                    
    if (this->soundEngine == NULL) {
        return NULL;
    }

    size_t effectFlags = SoundEffect::GetEffectFlagsFromStrList(effectsStrs);
    return SoundEffect::Build(effectFlags, parameterMap);
}

bool GameSound::LoadFromMSF() {
    if (this->soundEngine == NULL) {
        return false;
    }

    // Load the pallet of all game sounds (this will not load the actual sounds, just their proxies).
    // N.B., Sounds are loaded after this function call by calling the Load*Sounds functions of GameSound.
    bool success = MSFReader::ReadMSF(*this, GameViewConstants::GetInstance()->GAME_SOUND_SCRIPT);
    if (!success) {
        // Clean up...
        this->ClearAll();
    }

    return success;
}

bool GameSound::PlaySoundWithID(const SoundID& id, const GameSound::SoundType& soundType, bool isLooped, bool startPaused) {
    assert(soundType != GameSound::NoSound);
    assert(id != INVALID_SOUND_ID);

    if (this->soundEngine == NULL) {
        return false;
    }

    // Make sure the id isn't already taken...
    if (this->GetPlayingSound(id) != NULL) {
        return false;
    }

    // Try to find the source associated with the given sound type
    AbstractSoundSource* source = this->GetSoundSourceFromType(soundType);
    if (source == NULL) {
        return false;
    }

    Sound* newSound = source->Spawn2DSoundWithID(id, isLooped, startPaused);
    if (newSound == NULL) {
        assert(false);
        return false;
    }
    
    // Apply any active effects to the sound
    for (EffectSetConstIter iter = this->activeEffects.begin(); iter != this->activeEffects.end(); ++iter) {
        SoundEffect* currEffect = this->GetSoundEffectFromType(*iter);
        assert(currEffect != NULL);
        currEffect->ToggleEffectOnSound(newSound, true);
    }

    this->nonAttachedPlayingSounds.insert(std::make_pair(newSound->GetSoundID(), newSound));
    return true;
}

void GameSound::ClearAll() {
    this->ClearEffects();
    this->ClearSounds();
    this->ClearSoundSources();
}

void GameSound::ClearEffects() {
    for (EffectMapIter iter = this->globalEffects.begin(); iter != this->globalEffects.end(); ++iter) {
        SoundEffect* currEffect = iter->second;
        delete currEffect;
    }
    this->globalEffects.clear();
    this->activeEffects.clear();
    this->pausedEffects.clear();
}

void GameSound::ClearSounds() {
    // Clear all non-attached sounds from memory
    for (SoundMapIter iter = this->nonAttachedPlayingSounds.begin(); iter != this->nonAttachedPlayingSounds.end(); ++iter) {
        Sound* currSound = iter->second;
        delete currSound;
    }
    this->nonAttachedPlayingSounds.clear();

    // Clear all attached sounds from memory
    for (AttachedSoundMapIter iter1 = this->attachedPlayingSounds.begin(); iter1 != this->attachedPlayingSounds.end(); ++iter1) {
        AttachedSoundInfo& info = iter1->second;
        info.ClearSoundMap();
    }
    this->attachedPlayingSounds.clear();
}

void GameSound::ClearSoundSources() {
    if (this->soundEngine == NULL) {
        return;
    }

    // Clean up all the sound sources and any active sounds...
    
    // Clear all global sounds
    for (SoundSourceMapIter iter = this->globalSounds.begin(); iter != this->globalSounds.end(); ++iter) {
        AbstractSoundSource* currSoundSrc = iter->second;
        delete currSoundSrc;
        currSoundSrc = NULL;
    }
    this->globalSounds.clear();

    // Clear all world sounds
    for (WorldSoundSourceMapIter iter1 = this->worldSounds.begin(); iter1 != this->worldSounds.end(); ++iter1) {
        SoundSourceMap& soundMap = iter1->second;
        for (SoundSourceMapIter iter2 = soundMap.begin(); iter2 != soundMap.end(); ++iter2) {
            AbstractSoundSource* currSoundSrc = iter2->second;
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

AbstractSoundSource* GameSound::GetSoundSourceFromType(const GameSound::SoundType& type) const {

    // Try the global sounds...
    SoundSourceMapConstIter findIter = this->globalSounds.find(type);
    if (findIter != this->globalSounds.end()) {
        return findIter->second;
    }

    // If no world is loaded then we won't find it in that map either
    if (this->currLoadedWorldStyle == GameWorld::None) {
        return NULL;
    }

    // Not in the global sounds, try to find it among the world sounds...
    WorldSoundSourceMapConstIter worldFindIter = this->worldSounds.find(this->currLoadedWorldStyle);
    if (worldFindIter == this->worldSounds.end()) {
        return NULL;
    }

    const SoundSourceMap& currWorldSoundSourceMap = worldFindIter->second;
    findIter = currWorldSoundSourceMap.find(type);
    if (findIter != currWorldSoundSourceMap.end()) {
        return findIter->second;
    }

    return NULL;
}

SoundEffect* GameSound::GetSoundEffectFromType(const GameSound::EffectType& type) const {
    EffectMapConstIter findIter = this->globalEffects.find(type);
    if (findIter != this->globalEffects.end()) {
        return findIter->second;
    }
    return NULL;
}

Sound* GameSound::GetPlayingSound(SoundID soundID) const {

    // Try to find the sound among the non-attached sounds
    SoundMapConstIter findIter = this->nonAttachedPlayingSounds.find(soundID);
    if (findIter != this->nonAttachedPlayingSounds.end()) {
        return findIter->second;
    }

    // Couldn't find the sound in the non-attached, try searching through attached sounds
    for (AttachedSoundMapConstIter iter = this->attachedPlayingSounds.begin(); iter != this->attachedPlayingSounds.end(); ++iter) {
        const AttachedSoundInfo& info = iter->second;
        const SoundMap& soundMap = info.soundMap;
        findIter = soundMap.find(soundID);
        if (findIter != soundMap.end()) {
            return findIter->second;
        }
    }

    // No sound was found
    return NULL;
}

void GameSound::GetAllPlayingSoundsAsList(std::list<Sound*>& playingSounds) const {
    playingSounds.clear();
    for (SoundMapConstIter iter = this->nonAttachedPlayingSounds.begin(); iter != this->nonAttachedPlayingSounds.end(); ++iter) {
        playingSounds.push_back(iter->second);
    }
    for (AttachedSoundMapConstIter iter1 = this->attachedPlayingSounds.begin(); iter1 != this->attachedPlayingSounds.end(); ++iter1) {
        const AttachedSoundInfo& info = iter1->second;
        const SoundMap& soundMap = info.soundMap;
        for (SoundMapConstIter iter2 = soundMap.begin(); iter2 != soundMap.end(); ++iter2) {
            playingSounds.push_back(iter2->second);
        }
    }
}

// Private helper function for building 2D and 3D sounds
// If the sound could not be built it returns NULL.
Sound* GameSound::BuildSound(const GameSound::SoundType& soundType, bool isLooped, const Point3D* position, 
                             bool applyActiveEffects, bool startPaused) {

    assert(soundType != GameSound::NoSound);

    if (this->soundEngine == NULL) {
        return NULL;
    }

    // Try to find the source associated with the given sound type
    AbstractSoundSource* source = this->GetSoundSourceFromType(soundType);
    if (source == NULL) {
        return NULL;
    }

    // Build the sound (either 2D or 3D based on whether position was provided or not)
    Sound* newSound = NULL;
    if (position != NULL) {
        newSound = source->Spawn3DSound(isLooped, *position, startPaused);
    }
    else {
        newSound = source->Spawn2DSound(isLooped, startPaused);
    }

    // Apply any active effects to the sound
    if (applyActiveEffects) {
        for (EffectSetConstIter iter = this->activeEffects.begin(); iter != this->activeEffects.end(); ++iter) {
            SoundEffect* currEffect = this->GetSoundEffectFromType(*iter);
            assert(currEffect != NULL);
            currEffect->ToggleEffectOnSound(newSound, true);
        }
    }

    return newSound;
}

void GameSound::AttachedSoundInfo::ClearSoundMap() {
    for (SoundMapIter iter = this->soundMap.begin(); iter != this->soundMap.end(); ++iter) {
        Sound* currSound = iter->second;
        delete currSound;
    }
    this->soundMap.clear();
}