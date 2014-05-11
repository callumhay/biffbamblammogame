/**
 * MSFReader.h
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

	// Other keywords for the MSF
	static const char* IGNORE_KEYWORD;
	static const char* IGNORE_LINE;

	// Keyword parameters for the MSF definition blocks
	static const char* FILE_KEYWORD;
    static const char* WORLD_KEYWORD;
    static const char* TYPE_KEYWORD;

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