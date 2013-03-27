/**
 * MSFReader.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __MSFREADER_H__
#define __MSFREADER_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "GameSound.h"

class MSFReader {

public:
    static bool ReadMSF(GameSound& gameSound, const std::string& filepath);

private:
	MSFReader(){}

	// Syntax symbols for the MSF
    static const char* OPEN_SOUND_DEFINTION_BLOCK;
	static const char* CLOSE_SOUND_DEFINITION_BLOCK;
	static const char* OPEN_ENCLOSING_PROB_FILE_PAIR;
	static const char* CLOSE_ENCLOSING_PROB_FILE_PAIR;
	static const char* PROB_DEFINITION_SYNTAX;
    static const char* OPEN_TIME_SEQUENCE_DEFINITION;
    static const char* CLOSE_TIME_SEQUENCE_DEFINITION;

	// Other keywords for the MSF
	static const char* IGNORE_KEYWORD;
	static const char* IGNORE_LINE;

	// Keyword parameters for the MSF definition blocks
	static const char* FILE_KEYWORD;
    static const char* WORLD_KEYWORD;

	// Sound string identifiers
	static void InitSoundTypeMapping();
    static void InitEffectTypeMapping();
    static std::map<std::string, GameSound::SoundType> soundTypeMapping;
    static std::map<std::string, GameSound::EffectType> effectTypeMapping;

	// Initializer/Default values
    static const char* NO_BLOCK_NAME;

    static GameSound::SoundType ConvertKeywordToSoundType(const std::string& soundName);
    static GameSound::EffectType ConvertKeywordToEffectType(const std::string& effectName);
	static bool FoundEqualsSyntax(bool& noEquals, std::string& errorStr, std::istream* inStream);

};

#endif // __MSFREADER_H__