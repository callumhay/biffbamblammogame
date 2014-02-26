/**
 * SoundEffect.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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

#ifndef __SOUNDEFFECT_H__
#define __SOUNDEFFECT_H__

#include "../BlammoEngine/BasicIncludes.h"

// IrrKlang Forward Declarations
namespace irrklang {
    class ISound;
    class ISoundEffectControl;
}

// GameSound Forward Declarations
class Sound;

class SoundEffect {
public:
    static const char* REVERB3D_STR;
    static const char* DISTORTION_STR;
    static const char* GARGLE_STR;
    static const char* FLANGER_STR;
    static const char* REVERB_WAVE_STR;
    static const char* CHORUS_STR;
    //static const char* COMPRESSOR_STR;
    
    typedef std::map<std::string, float> EffectParameterMap;
    typedef EffectParameterMap::const_iterator EffectParameterMapConstIter;
    enum TypeFlag { None = 0x00000000, Reverb3D = 0x00000001, Distortion = 0x00000002, Gargle = 0x00000004, Flanger = 0x00000008,
        ReverbWave = 0x00000010, Chorus = 0x00000020 };
    
    static SoundEffect* Build(size_t typeFlags, const EffectParameterMap& parameterMap);
    static size_t GetEffectFlagsFromStrList(const std::vector<std::string>& effectsStrs);
    
    virtual ~SoundEffect();

    void ToggleEffectOnSounds(const std::list<Sound*>& sounds, bool effectOn);
    void ToggleEffectOnSound(Sound* sound, bool effectOn);
    void ToggleEffect(irrklang::ISound* sound, bool effectOn);

    virtual bool EnableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const  = 0;
    virtual bool DisableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const = 0;

protected:
    SoundEffect(const EffectParameterMap& parameterMap);

    template<typename T> static void ParseSoundParameter(const EffectParameterMap& parameterMap,
        const std::string& valueParamName, const T& minValue, const T& maxValue, const T& defaultValue, T& value);

private:
    static const float DEFAULT_VOLUME;
    static const char* VOLUME_PARAM_NAME;

    float volume;

    DISALLOW_COPY_AND_ASSIGN(SoundEffect);
};

inline void SoundEffect::ToggleEffectOnSounds(const std::list<Sound*>& sounds, bool effectOn) {
    for (std::list<Sound*>::const_iterator iter = sounds.begin(); iter != sounds.end(); ++iter) {
        this->ToggleEffectOnSound(*iter, effectOn);
    }
}

/**
 * Helper function for parsing sound parameters.
 */
template<typename T>
inline void SoundEffect::ParseSoundParameter(const EffectParameterMap& parameterMap,
                                             const std::string& valueParamName,
                                             const T& minValue, const T& maxValue,
                                             const T& defaultValue, T& value) {
#pragma warning(push)
#pragma warning(disable:4800)
    EffectParameterMapConstIter findIter = parameterMap.find(valueParamName);
    if (findIter != parameterMap.end()) {
        value = std::min<T>(maxValue, std::max<T>(minValue, static_cast<T>(findIter->second)));
    }
    else {
        value = defaultValue;
    }
#pragma warning(pop)
}

#endif // __SOUNDEFFECT_H__