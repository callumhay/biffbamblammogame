/**
 * SoundSource.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include <irrKlang.h>

#include "SoundSource.h"
#include "Sound.h"

#include "../ResourceManager.h"

SoundSource::SoundSource(irrklang::ISoundEngine* soundEngine, const GameSound::SoundType& soundType,
                         const std::string& soundName, const std::string& filePath) : 
soundEngine(soundEngine), soundType(soundType), soundName(soundName), soundFilePath(filePath), source(NULL) {
    assert(soundEngine != NULL);
}

SoundSource::~SoundSource() {
    this->Unload();
}

bool SoundSource::Load() {

    if (this->source == NULL) {
		// Attempt to load the source directly from the engine (in cases where the source already has been loaded)
        this->source = this->soundEngine->getSoundSource(this->soundFilePath.c_str(), false);
        
        if (this->source == NULL) {
            // Looks like the sound hasn't been loaded yet, load it from memory using the file path
            long dataLength = 0;
            char* soundMemData = ResourceManager::GetInstance()->FilepathToMemoryBuffer(this->soundFilePath, dataLength);
		    if (soundMemData == NULL) {
                std::cerr << "[Sound: " << this->soundName << "] - Could not find file: " << this->soundFilePath << std::endl;
			    return false;
		    }

            this->source = this->soundEngine->addSoundSourceFromMemory(soundMemData, dataLength, this->soundFilePath.c_str(), true);
            delete[] soundMemData;
            soundMemData = NULL;

            if (this->source == NULL) {
                return false;
            }
        }
    }

    assert(this->source != NULL);
    return true;
}

void SoundSource::Unload() {
    if (this->source != NULL) {
        this->soundEngine->removeSoundSource(this->source);
        this->source = NULL;
    }
}

Sound* SoundSource::Spawn2DSoundWithID(const SoundID& id, bool isLooped) {
    if (this->source == NULL) {
        assert(false);
        return NULL;
    }

    irrklang::ISound* newSound = this->soundEngine->play2D(this->source, isLooped, false, true, true);
    if (newSound == NULL) {
        assert(false);
        return NULL;
    }

    return new Sound(id, this->soundType, newSound);
}