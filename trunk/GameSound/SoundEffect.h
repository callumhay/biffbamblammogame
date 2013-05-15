/**
 * SoundEffect.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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
    //static const char* COMPRESSOR_STR;
    
    typedef std::map<std::string, float> EffectParameterMap;
    typedef EffectParameterMap::const_iterator EffectParameterMapConstIter;
    enum TypeFlag { None = 0x00000000, Reverb3D = 0x00000001, Distortion = 0x00000002, Gargle = 0x00000004 };
    
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