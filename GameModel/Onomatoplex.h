/**
 * Onomatoplex.h
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

#ifndef __ONOMATOPLEX_H__
#define __ONOMATOPLEX_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Algebra.h"

namespace Onomatoplex {

enum SoundType {
    EXPLOSION = 0,
    BOUNCE = 1,
    ELECTRIC = 2,
    SMOKE = 3,
    BADSAD = 4,
    SHOT = 5,
    GOODHAPPY = 6,
    GOO = 7,
    SHATTER = 8,
    CRAZY = 9,
    LAUNCH = 10,
    ATTACH = 11,
    ANGRY = 12
};
enum Extremeness {WEAK = 0, NORMAL = 1, PRETTY_GOOD = 2, GOOD = 3, AWESOME = 4, SUPER_AWESOME = 5, UBER = 6};

static const int NumSoundTypes = 8;
static const int NumExtremenessTypes = 7;

// Singleton class for generating crazy words.
class Generator {
public:
	/* 
	 * Obtain the singleton instance of the generator.
	 * Precondition: true.
	 * Returns: The singleton of the Generator class.
	 */
	static Generator* Generator::GetInstance() {
		if (Generator::instance == NULL) {
			Generator::instance = new Generator();
		}
		return Generator::instance;
	}

	static void DeleteInstance() {
		if (Generator::instance != NULL) {
			delete Generator::instance;
			Generator::instance = NULL;
		}
	}

	std::string Generate(SoundType type, Extremeness amt);
    std::string GenerateVictoryDescriptor() const;

	/**
	 * Obtain a random extremeness in the given interval of min and max extremenesses.
	 * Result: random extremeness value in [min, max].
	 */
	static inline Extremeness Generator::GetRandomExtremeness(Extremeness min, Extremeness max) {
		assert(min < max);
		return static_cast<Extremeness>(min + (Randomizer::GetInstance()->RandomUnsignedInt() % (max - min + 1)));
	}

private:
	static Generator* instance;
	Generator();
	~Generator();

	// Dictionaries for word creation
	std::map<SoundType, std::vector<std::string> > simpleSingleWords;
	std::map<SoundType, std::vector<std::string> > goodSingleWords;
	std::map<SoundType, std::vector<std::string> > awesomeSingleWords;
	std::map<SoundType, std::vector<std::string> > uberSingleWords;

    std::vector<std::string> victoryDescriptors;

	void LoadExplosionWords();
	void LoadBounceWords();
	void LoadElectricWords();
	void LoadSmokeWords();
	void LoadBadSadWords();
	void LoadShotWords();
	void LoadGoodHappyWords();
	void LoadGooWords();
	void LoadShatterWords();
	void LoadCrazyWords();
    void LoadLaunchWords();
    void LoadAttachWords();
    void LoadAngryWords();

    void LoadVictoryDescriptors();

	// Punctuation structures and functions
	static const char* DEFAULT_END_PUNCTUATION;
	static std::vector<std::string> endPunctuation;
	std::string GenerateAbsurdPunctuation(SoundType type, Extremeness ex);

	static std::string JoinEndfixes(const std::string &endFix1, const std::string &endFix2);
	static bool IsVowel(const char c);

	std::string GenerateWeakSoundText(SoundType type);
	std::string GenerateNormalSoundText(SoundType type);
	std::string GeneratePrettyGoodSoundText(SoundType type);
	std::string GenerateGoodSoundText(SoundType type);
	std::string GenerateAwesomeSoundText(SoundType type);
	std::string GenerateSuperAwesomeSoundText(SoundType type);
	std::string GenerateUberSoundText(SoundType type);

	DISALLOW_COPY_AND_ASSIGN(Generator);
};

};
#endif // __ONOMATOPLEX_H__