/**
 * AbstractSoundSource.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include <irrKlang.h>
#include "AbstractSoundSource.h"
#include "Sound.h"

#include "../ResourceManager.h"

AbstractSoundSource::AbstractSoundSource(irrklang::ISoundEngine* soundEngine, 
                                         const GameSound::SoundType& soundType,
                                         const std::string& soundName) :
soundEngine(soundEngine), soundType(soundType), soundName(soundName), isMusic(GameSound::IsMusic(soundType)) {
    assert(soundEngine != NULL);
}

AbstractSoundSource::~AbstractSoundSource() {
}

irrklang::ISoundSource* AbstractSoundSource::LoadSoundSource(const std::string& filepath) {
    irrklang::ISoundSource* source = NULL;

    // Attempt to load the source directly from the engine (in cases where the source already has been loaded)
    source = this->soundEngine->getSoundSource(filepath.c_str(), false);
    if (source == NULL) {
        
        // Looks like the sound hasn't been loaded yet, load it from memory using the file path
        long dataLength = 0;
        char* soundMemData = ResourceManager::GetInstance()->FilepathToMemoryBuffer(filepath, dataLength);
        if (soundMemData == NULL) {
            std::cerr << "[Sound: " << this->soundName << "] - Could not find file: " << filepath << std::endl;
            return NULL;
        }

        source = this->soundEngine->addSoundSourceFromMemory(soundMemData, dataLength, filepath.c_str(), true);
        delete[] soundMemData;
        soundMemData = NULL;
    }

    return source;
}

void AbstractSoundSource::UnloadSoundSource(irrklang::ISoundSource* source) {
    assert(source != NULL);
    if (this->soundEngine == NULL) {
        assert(false);
        return;
    }

    this->soundEngine->removeSoundSource(source);
}

Sound* AbstractSoundSource::Spawn2DSoundWithIDAndSource(const SoundID& id, irrklang::ISoundSource* source, 
                                                        bool isLooped, bool startPaused) {
    assert(source != NULL);

    irrklang::ISound* newSound = this->soundEngine->play2D(source, isLooped, startPaused, true, true);
    if (newSound == NULL) {
        assert(false);
        return NULL;
    }

    return new Sound(id, this->soundType, newSound, this->isMusic);
}

Sound* AbstractSoundSource::Spawn2DSoundWithIDAndSource(const SoundID& id, irrklang::ISoundSource* source, 
                                                        bool isLooped, const Point3D& pos, bool startPaused) {
    assert(source != NULL);
    
    irrklang::ISound* newSound = this->soundEngine->play3D(source, irrklang::vec3df(pos[0], pos[1], pos[2]), isLooped, startPaused, true, true);
    newSound->setMinDistance(GameSound::DEFAULT_MIN_3D_SOUND_DIST); // Make sure we're playing at max volume everywhere in the game
    if (newSound == NULL) {
        assert(false);
        return NULL;
    }

    return new Sound(id, this->soundType, newSound, this->isMusic);
}