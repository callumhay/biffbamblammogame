#ifndef __ONOMATAGENERATOR_H__
#define __ONOMATAGENERATOR_H__

#include <string>
#include <map>
#include <vector>

namespace Onomatoplex {

	// TODO: Shooting, GoodHappy
	enum SoundType {EXPLOSION = 0, BOUNCE = 1, ELECTRIC = 2, SMOKE = 3, BADSAD = 4, SHOT = 5};
	enum Extremeness {WEAK = 0, NORMAL = 1, PRETTY_GOOD = 2, GOOD = 3, AWESOME = 4, SUPER_AWESOME = 5, UBER = 6};
	
	static const int NumSoundTypes = 5;
	static const int NumExtremenessTypes = 7;
	
	// Singleton class for generating crazy words.
	class Generator {

	private:
		static Generator* instance;
		Generator();
		~Generator();

		// Dictionaries for word creation
		std::map<SoundType, std::vector<std::string>> simpleSingleWords;
		std::map<SoundType, std::vector<std::string>> goodSingleWords;
		std::map<SoundType, std::vector<std::string>> awesomeSingleWords;
		std::map<SoundType, std::vector<std::string>> uberSingleWords;

		// Punctuation structures and functions
		static const std::string DEFAULT_END_PUNCTUATION;
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
	};
}
#endif