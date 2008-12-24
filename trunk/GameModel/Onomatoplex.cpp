#include "Onomatoplex.h"
#include <stdlib.h>
#include <time.h>
#include <assert.h>

namespace Onomatoplex {
	
	// Singleton instance
	Generator* Generator::instance = NULL;

	// Static dictionaries for word creation lookup
	std::map<SoundType, std::vector<std::string>> Generator::simpleSingleWords;
	std::map<SoundType, std::vector<std::string>> Generator::firstFix;
	std::map<SoundType, std::vector<std::string>> Generator::secondFix;
	std::map<SoundType, std::vector<std::string>> Generator::thirdFix;
	std::map<SoundType, std::vector<std::string>> Generator::endFix;
	std::map<SoundType, std::vector<std::string>> Generator::superEndFix;
	std::map<SoundType, std::vector<std::string>> Generator::uberEndFix;

	const std::string Generator::DEFAULT_END_PUNCTUATION = "!";
	std::vector<std::string> Generator::endPunctuation;

	/* 
	 * Obtain the singleton instance of the generator.
	 * Precondition: true.
	 * Returns: The singleton of the Generator class.
	 */
	Generator* Generator::Instance() {
		if (Generator::instance == NULL) {
			Generator::instance = new Generator();
		}
		return Generator::instance;
	}

	/*
	 * Sets up all the dictionaries of pre/mid/post strings for making up words.
	 * This should only ever be called ONCE.
	 */
	Generator::Generator() {
		Generator::simpleSingleWords.clear();
		Generator::firstFix.clear();
		Generator::secondFix.clear();
		Generator::thirdFix.clear();
		Generator::endFix.clear();
		Generator::superEndFix.clear();
		Generator::uberEndFix.clear();
		Generator::endPunctuation.clear();

		Generator::endPunctuation.push_back(Generator::DEFAULT_END_PUNCTUATION);
		Generator::endPunctuation.push_back("?");

		// Single words
		std::vector<std::string> singleWordExplosion;
		singleWordExplosion.push_back("Biff");
		singleWordExplosion.push_back("Boom");
		singleWordExplosion.push_back("Blam");
		singleWordExplosion.push_back("Pow");
		singleWordExplosion.push_back("Ppfft");
		Generator::simpleSingleWords[EXPLOSION] = singleWordExplosion;

		std::vector<std::string> singleWordBounce;
		singleWordBounce.push_back("Bounce");
		singleWordBounce.push_back("Boing");
		singleWordBounce.push_back("Boink");
		singleWordBounce.push_back("Bipp");
		singleWordBounce.push_back("Clang");
		singleWordBounce.push_back("Bip");
		singleWordBounce.push_back("Bop");
		Generator::simpleSingleWords[BOUNCE] = singleWordBounce;

		std::vector<std::string> singleWordElectric;
		singleWordElectric.push_back("Bzzt");
		singleWordElectric.push_back("Zap");
		singleWordElectric.push_back("Spark");
		singleWordElectric.push_back("Crackle");
		singleWordElectric.push_back("Fizz");
		singleWordElectric.push_back("Sizzle");
		Generator::simpleSingleWords[ELECTRIC] = singleWordElectric;

		std::vector<std::string> singleWordSmoke;
		singleWordSmoke.push_back("Puff");
		singleWordSmoke.push_back("Poof");
		singleWordSmoke.push_back("Psph");
		singleWordSmoke.push_back("Smokey");
		singleWordSmoke.push_back("Pffp");
		singleWordSmoke.push_back("Fssp");
		Generator::simpleSingleWords[SMOKE] = singleWordSmoke;

		std::vector<std::string> singleWordBadSad;
		singleWordBadSad.push_back("Aww");
		singleWordBadSad.push_back("Ow");
		singleWordBadSad.push_back("Eek");
		singleWordBadSad.push_back("Ouch");
		singleWordBadSad.push_back("Oof");
		singleWordBadSad.push_back("Pain");
		singleWordBadSad.push_back("Hurt");
		singleWordBadSad.push_back("Cry");
		singleWordBadSad.push_back("Tear");
		singleWordBadSad.push_back("Sob");
		singleWordBadSad.push_back("Boo");
		singleWordBadSad.push_back("Gaak");
		singleWordBadSad.push_back("Gah");
		singleWordBadSad.push_back("Agh");
		singleWordBadSad.push_back("Egad");
		Generator::simpleSingleWords[BADSAD] = singleWordBadSad;

		// First fixes
		std::vector<std::string> firstFixExplosion;
		firstFixExplosion.push_back("B");
		firstFixExplosion.push_back("Bl");
		firstFixExplosion.push_back("Fl");
		firstFixExplosion.push_back("Kph");
		firstFixExplosion.push_back("Kabl");
		firstFixExplosion.push_back("Kab");
		firstFixExplosion.push_back("Z");
		firstFixExplosion.push_back("Pph");
		firstFixExplosion.push_back("Pow");
		firstFixExplosion.push_back("Splz");
		Generator::firstFix[EXPLOSION] = firstFixExplosion;

		std::vector<std::string> firstFixBounce;
		firstFixBounce.push_back("B");
		firstFixBounce.push_back("P");
		firstFixBounce.push_back("Z");
		Generator::firstFix[BOUNCE] = firstFixBounce;

		std::vector<std::string> firstFixElectric;
		firstFixElectric.push_back("Pss");
		firstFixElectric.push_back("Pzz");
		firstFixElectric.push_back("Bzz");
		firstFixElectric.push_back("Fzz");
		firstFixElectric.push_back("Fiz");
		Generator::firstFix[ELECTRIC] = firstFixElectric;

		std::vector<std::string> firstFixSmoke;
		firstFixSmoke.push_back("Pss");
		firstFixSmoke.push_back("Puf");
		firstFixSmoke.push_back("Poo");
		firstFixSmoke.push_back("Fss");
		Generator::firstFix[SMOKE] = firstFixSmoke;

		std::vector<std::string> firstFixBadSad;
		firstFixBadSad.push_back("Eeek");
		firstFixBadSad.push_back("Agh");
		firstFixBadSad.push_back("Gaak");
		firstFixBadSad.push_back("Oof");
		firstFixBadSad.push_back("Ouch");
		firstFixBadSad.push_back("Pain");
		firstFixBadSad.push_back("Ack");
		firstFixBadSad.push_back("Cry");
		firstFixBadSad.push_back("Owwch");
		Generator::firstFix[BADSAD] = firstFixBadSad;

		// Second fixes
		std::vector<std::string> secondFixExplosion;
		secondFixExplosion.push_back("a");	
		secondFixExplosion.push_back("aa");
		secondFixExplosion.push_back("oo");
		Generator::secondFix[EXPLOSION] = secondFixExplosion;

		std::vector<std::string> secondFixBounce;
		secondFixBounce.push_back("");
		Generator::secondFix[BOUNCE] = secondFixBounce;
		
		std::vector<std::string> secondFixElectric;
		secondFixElectric.push_back("izz");
		secondFixElectric.push_back("shh");
		secondFixElectric.push_back("azz");
		secondFixElectric.push_back("sst");
		secondFixElectric.push_back("");
		Generator::secondFix[ELECTRIC] = secondFixElectric;

		std::vector<std::string> secondFixSmoke;
		secondFixSmoke.push_back("f");
		secondFixSmoke.push_back("fp");
		secondFixSmoke.push_back("ph");
		secondFixSmoke.push_back("t");
		secondFixSmoke.push_back("");
		Generator::secondFix[SMOKE] = secondFixSmoke;

		std::vector<std::string> secondFixBadSad;
		secondFixBadSad.push_back("gah");
		secondFixBadSad.push_back("oof");
		secondFixBadSad.push_back(" Cry");
		secondFixBadSad.push_back(" Sob");
		secondFixBadSad.push_back(" Pain");
		secondFixBadSad.push_back(" ItHurts");
		secondFixBadSad.push_back("gah");
		secondFixBadSad.push_back("eek");
		Generator::secondFix[BADSAD] = secondFixBadSad;

		// Third fixes
		std::vector<std::string> thirdFixExplosion;
		thirdFixExplosion.push_back("bl");
		thirdFixExplosion.push_back("b");
		thirdFixExplosion.push_back("z");
		Generator::thirdFix[EXPLOSION] = thirdFixExplosion;

		std::vector<std::string> thirdFixBounce;
		thirdFixBounce.push_back("");
		Generator::thirdFix[BOUNCE] = thirdFixBounce;

		std::vector<std::string> thirdFixElectric;
		thirdFixElectric.push_back("bzz");
		thirdFixElectric.push_back("kzz");
		thirdFixElectric.push_back("");
		Generator::thirdFix[ELECTRIC] = thirdFixElectric;

		std::vector<std::string> thirdFixSmoke;
		thirdFixSmoke.push_back("");
		Generator::thirdFix[SMOKE] = thirdFixSmoke;

		std::vector<std::string> thirdFixBadSad;
		thirdFixBadSad.push_back("");
		Generator::thirdFix[BADSAD] = thirdFixBadSad;

		// End-fixes - these add a neat ending to an already ridiculous word, they may also
		// be combined almost endlessly...

		// Basic end-fixes
		std::vector<std::string> endFixExplosion;
		endFixExplosion.push_back("oom");
		endFixExplosion.push_back("aam");
		endFixExplosion.push_back("am");
		endFixExplosion.push_back("oof");
		endFixExplosion.push_back("ash");
		endFixExplosion.push_back("osh");
		Generator::endFix[EXPLOSION] =  endFixExplosion;
		
		std::vector<std::string> endFixBounce;
		endFixBounce.push_back("lop");
		endFixBounce.push_back("oing");
		endFixBounce.push_back("ip");
		endFixBounce.push_back("ong");
		endFixBounce.push_back("onk");
		endFixBounce.push_back("ang");
		Generator::endFix[BOUNCE] = endFixBounce;

		std::vector<std::string> endFixElectric;
		endFixElectric.push_back("t");
		endFixElectric.push_back("z");
		endFixElectric.push_back("st");
		endFixElectric.push_back("tt");
		endFixElectric.push_back("zt");
		Generator::endFix[ELECTRIC] = endFixElectric;

		std::vector<std::string> endFixSmoke;
		endFixSmoke.push_back("ft");
		endFixSmoke.push_back("sh");
		endFixSmoke.push_back("f");
		endFixSmoke.push_back("pf");
		Generator::endFix[SMOKE] = endFixSmoke;

		std::vector<std::string> endFixBadSad;
		endFixBadSad.push_back("");
		Generator::endFix[BADSAD] = endFixBadSad;

		// Super end-fixes
		std::vector<std::string> superEndFixExplosion;
		superEndFixExplosion.push_back("y");
		superEndFixExplosion.push_back("o");
		superEndFixExplosion.push_back("bo");
		superEndFixExplosion.push_back("po");
		superEndFixExplosion.push_back("");
		Generator::superEndFix[EXPLOSION] = superEndFixExplosion;
		
		std::vector<std::string> superEndFixBounce;
		superEndFixBounce.push_back("y");
		superEndFixBounce.push_back("");
		superEndFixBounce.push_back("");
		Generator::superEndFix[BOUNCE] = superEndFixBounce;

		std::vector<std::string> superEndFixElectric;
		superEndFixElectric.push_back("");
		Generator::superEndFix[ELECTRIC] = superEndFixElectric;

		std::vector<std::string> superEndFixSmoke;
		superEndFixSmoke.push_back("");
		superEndFixSmoke.push_back("puff");
		superEndFixSmoke.push_back("poof");
		superEndFixSmoke.push_back("f");
		Generator::superEndFix[SMOKE] = superEndFixSmoke;

		std::vector<std::string> superEndFixBadSad;
		superEndFixBadSad.push_back("y");
		superEndFixBadSad.push_back("eep");
		superEndFixBadSad.push_back("Oof");
		superEndFixBadSad.push_back(" ItHurts");
		superEndFixBadSad.push_back(" LotsOfPain");
		superEndFixBadSad.push_back(" MakeItStop");
		superEndFixBadSad.push_back(" Hurting");
		superEndFixBadSad.push_back("OwOw");
		Generator::superEndFix[BADSAD] = superEndFixBadSad;

		// Uber end-fixes
		std::vector<std::string> uberEndFixExplosion;
		uberEndFixExplosion.push_back("oplex");
		uberEndFixExplosion.push_back("oise");
		uberEndFixExplosion.push_back("osity");
		uberEndFixExplosion.push_back("etric");
		uberEndFixExplosion.push_back("ation");
		uberEndFixExplosion.push_back("otron");
		uberEndFixExplosion.push_back("atron");
		uberEndFixExplosion.push_back("plosion");
		uberEndFixExplosion.push_back("frazz");
		Generator::uberEndFix[EXPLOSION] = uberEndFixExplosion;

		std::vector<std::string> uberEndFixBounce;
		uberEndFixBounce.push_back(" Zonk");
		uberEndFixBounce.push_back(" Bip");
		uberEndFixBounce.push_back(" Yoing");
		uberEndFixBounce.push_back(" Clonk");
		uberEndFixBounce.push_back(" Clank");
		uberEndFixBounce.push_back(" Bop");
		Generator::uberEndFix[BOUNCE] = uberEndFixBounce;

		std::vector<std::string> uberEndFixElectric;
		uberEndFixElectric.push_back("kizzap");
		uberEndFixElectric.push_back("zizat");
		uberEndFixElectric.push_back("fizzle");
		uberEndFixElectric.push_back("blat");
		uberEndFixElectric.push_back("uple");
		uberEndFixElectric.push_back("aple");
		uberEndFixElectric.push_back("iple");
		uberEndFixElectric.push_back("frazz");
		Generator::uberEndFix[ELECTRIC] = uberEndFixElectric;

		std::vector<std::string> uberEndFixSmoke;
		uberEndFixSmoke.push_back(" Psssf");
		uberEndFixSmoke.push_back(" Puff");
		uberEndFixSmoke.push_back("oplex");
		uberEndFixSmoke.push_back(" Poof");
		Generator::uberEndFix[SMOKE] = uberEndFixSmoke;

		std::vector<std::string> uberEndFixBadSad;
		uberEndFixBadSad.push_back(" MakeTheBadManStop");
		uberEndFixBadSad.push_back(" HolyPainBatman");
		uberEndFixBadSad.push_back("Eeeggads");
		uberEndFixBadSad.push_back("Ouchyness");
		uberEndFixBadSad.push_back(" ItHurtsSoMuch");
		uberEndFixBadSad.push_back("OwOwOw");
		uberEndFixBadSad.push_back("Agghh");
		uberEndFixBadSad.push_back("eeEp");
		Generator::uberEndFix[BADSAD] = uberEndFixBadSad;
	}

	// Destructor, deletes singleton, cleans up dictionaries
	Generator::~Generator() {
		Generator::simpleSingleWords.clear();
		Generator::firstFix.clear();
		Generator::secondFix.clear();
		Generator::thirdFix.clear();
		Generator::endFix.clear();
		Generator::superEndFix.clear();
		Generator::uberEndFix.clear();
		Generator::endPunctuation.clear();
		delete Generator::instance;
	}

	/*
	 * Generates crazy onomatopoeia based on the sound type given and the extremeness of the context.
	 * Precondition: true.
	 * Returns: Onomatopoeia text.
	 */
	std::string Generator::Generate(SoundType type, Extremeness amt) {
		return this->Generate(type, amt, static_cast<unsigned int>(time(NULL)));
	}

	/*
	 * Generates crazy onomatopoeia based on the sound type given and the extremeness of the context.
	 * Precondition: true.
	 * Returns: Onomatopoeia text.
	 */
	std::string Generator::Generate(SoundType type, Extremeness amt, unsigned int seed) {
		srand(seed);
		std::string result = "";

		switch(amt) {
			case WEAK:
				result = this->GenerateWeakSoundText(type);
				break;
			case NORMAL:
				result = this->GenerateNormalSoundText(type);
				break;
			case PRETTY_GOOD:
				result = this->GeneratePrettyGoodSoundText(type);
				break;
			case GOOD:
				result = this->GenerateGoodSoundText(type);
				break;
			case AWESOME:
				result = this->GenerateAwesomeSoundText(type);
				break;
			case SUPER_AWESOME:
				result = this->GenerateSuperAwesomeSoundText(type);
				break;
			case UBER:
				result = this->GenerateUberSoundText(type);
				break;
			default:
				assert(false);
				break;
		}

		return result;
	}

	// Private helper functions -----------------------------------------------

	/*
	 * Joins two given endfixes together so that it looks (and sounds proper as far as
	 * crazy english goes).
	 * Precondition: true.
	 * Return: Something similar to endFix1 + endFix2.
	 */
	std::string Generator::JoinEndfixes(const std::string &endFix1, const std::string &endFix2) {
		std::string result = endFix1 + endFix2;
		
		if (endFix1.size() == 0 && endFix2.size() != 0) {
			return endFix2;
		}
		else if (endFix1.size() != 0 && endFix2.size() == 0) {
			return endFix1;
		}
		else if (endFix1.size() == 0 && endFix2.size() == 0) {
			return "";
		}

		// Check to see if the first endfix ends in a vowel and the second begins with one, in which
		// case we must choose.
		if (Generator::IsVowel(endFix1.at(endFix1.size()-1)) && Generator::IsVowel(endFix2.at(0))) {
			unsigned int randomChoice = rand() % 2;
			if (randomChoice) {
				// find the first non-vowel in endfix2 and append the rest
				size_t indexOfNonVowel = endFix2.find_first_not_of("aAeEiIoOuU");
				if (indexOfNonVowel == std::string::npos) {
					return endFix1;
				}

				result = endFix1 + endFix2.substr(indexOfNonVowel);
			}
			else {
				// find the first non-vowel in endfix2 and append the rest
				size_t indexOfNonVowel = endFix1.find_first_not_of("aAeEiIoOuU");
				if (indexOfNonVowel == std::string::npos) {
					return endFix2;
				}

				result = endFix1.substr(indexOfNonVowel) + endFix2;
			}
		}

		return result;
	}

	bool Generator::IsVowel(const char c) {
		return c == 'a' || c == 'A' || c == 'e' || c == 'E' || c == 'i' || 
					 c == 'I' || c == 'o' || c == 'O' || c == 'u' || c == 'U';
	}

	/*
	 * Generates a string of punctuation for the ends of crazy words, based
	 * on a given extremeness.
	 * Precondition: true.
	 * Return: string of absurd punctuation.
	 */
	std::string Generator::GenerateAbsurdPunctuation(SoundType type, Extremeness ex) {
		unsigned int amount = static_cast<unsigned int>(abs(ex));
		unsigned int randomAmt = rand() % (amount + 1);

		if (randomAmt > 3) {
			randomAmt = 3;
		}

		std::string punctuation = "";
		// In the case where we are generating a truly nutzoid word we want to make sure it at least 
		// has a default end-punctuation
		if (amount >= 3 && randomAmt == 0) {
			randomAmt = 1;	
		}

		// Ensure that if we only have one piece of end punctuation that it's the default one
		if (randomAmt >= 1) {
			punctuation = Generator::DEFAULT_END_PUNCTUATION;

			if (randomAmt > 1) {
				for (unsigned int i = 0; i < randomAmt - 1; i++) {
					size_t randomIndex = rand() % Generator::endPunctuation.size();
					punctuation = punctuation + Generator::endPunctuation[randomIndex];
				}
			}
		}

		switch(type) {
			case ELECTRIC:
			case BOUNCE:
				if (punctuation.size() > 1 && (rand() % 2 == 1)) {
					punctuation = Generator::DEFAULT_END_PUNCTUATION;
				}
				else {
					punctuation = "";
				}
			default:
				break;
		}

		return punctuation;
	}

	/*
	 * Generate a 'weak' crazy word - very basic and usually fairly small in length.
	 * Precondition: true.
	 * Returns: Weak/simple crazy word text.
	 */
	std::string Generator::GenerateWeakSoundText(SoundType type) {
		std::string result = "";
		size_t randomIndex = 0;
		
		randomIndex = rand() % 4;
		if (randomIndex < 3) {
			// Just take the first fix and add an end fix, both random of course...
			std::vector<std::string> firstFixes = Generator::firstFix[type];
			randomIndex = rand() % firstFixes.size();
			result = firstFixes[randomIndex];

			std::vector<std::string> endFixes = Generator::endFix[type];
			randomIndex = rand() % endFixes.size();
			result = result + endFixes[randomIndex];
		}
		else {
			std::vector<std::string> fullSimpleWords = Generator::simpleSingleWords[type];
			randomIndex = rand() % fullSimpleWords.size();
			result = fullSimpleWords[randomIndex];
		}

		return result;
	}

	/*
	 * Generate a 'normal' crazy word.
	 * Precondition: true.
	 * Returns: Crazy word text.
	 */
	std::string Generator::GenerateNormalSoundText(SoundType type) {
		std::string result = "";
		
		// These are just weak words but with the possiblity of end punctuation.
		result = this->GenerateWeakSoundText(type);
		return result + this->GenerateAbsurdPunctuation(type, NORMAL);
	}

	/*
	 * Generate a 'pretty good' crazy word.
	 * Precondition: true.
	 * Returns: Crazy word text.
	 */
	std::string Generator::GeneratePrettyGoodSoundText(SoundType type) {
		std::string result = "";
		size_t randomIndex = 0;

		// Take a first, end a super-end fix and put them together.
		std::vector<std::string> firstFixes = this->firstFix[type];
		randomIndex = rand() % firstFixes.size();
		result = firstFixes[randomIndex];

		std::string endFix1, endFix2;
		std::vector<std::string> endFixes = Generator::endFix[type];
		randomIndex = rand() % endFixes.size();
		endFix1 = endFixes[randomIndex];

		std::vector<std::string> superEndFixes = Generator::superEndFix[type];
		randomIndex = rand() % superEndFixes.size();
		endFix2 = superEndFixes[randomIndex];

		result = result + Generator::JoinEndfixes(endFix1, endFix2);

		return result + this->GenerateAbsurdPunctuation(type, PRETTY_GOOD);
	}

	/*
	 * Generate a 'good' crazy word.
	 * Precondition: true.
	 * Returns: Crazy word text.
	 */
	std::string Generator::GenerateGoodSoundText(SoundType type) {
		std::string result = "";
		size_t randomIndex = 0;

		// Use first, second and third fixes with an end fix
		std::vector<std::string> firstFixes = this->firstFix[type];
		randomIndex = rand() % firstFixes.size();
		result = firstFixes[randomIndex];

		std::vector<std::string> secondFixes = this->secondFix[type];
		randomIndex = rand() % secondFixes.size();
		result = result + secondFixes[randomIndex];

		std::vector<std::string> thirdFixes = this->thirdFix[type];
		randomIndex = rand() % thirdFixes.size();
		result = result + thirdFixes[randomIndex];

		std::vector<std::string> endFixes = Generator::endFix[type];
		randomIndex = rand() % endFixes.size();
		result = result + endFixes[randomIndex];
		
		return result + this->GenerateAbsurdPunctuation(type, GOOD);
	}

	/*
	 * Generate a 'awesome' crazy word.
	 * Precondition: true.
	 * Returns: Crazy word text.
	 */
	std::string Generator::GenerateAwesomeSoundText(SoundType type) {
		std::string result = "";
		size_t randomIndex = 0;

		// Use first, second and third fixes with a end and super end fix
		std::vector<std::string> firstFixes = this->firstFix[type];
		randomIndex = rand() % firstFixes.size();
		result = firstFixes[randomIndex];

		std::vector<std::string> secondFixes = this->secondFix[type];
		randomIndex = rand() % secondFixes.size();
		result = result + secondFixes[randomIndex];

		std::vector<std::string> thirdFixes = this->thirdFix[type];
		randomIndex = rand() % thirdFixes.size();
		result = result + thirdFixes[randomIndex];

		std::string endFix1, endFix2;
		std::vector<std::string> endFixes = Generator::endFix[type];
		randomIndex = rand() % endFixes.size();
		endFix1 = endFixes[randomIndex];

		std::vector<std::string> superEndFixes = Generator::superEndFix[type];
		randomIndex = rand() % superEndFixes.size();
		endFix2 = superEndFixes[randomIndex];

		result = result + Generator::JoinEndfixes(endFix1, endFix2);

		return result + this->GenerateAbsurdPunctuation(type, AWESOME);
	}

	/*
	 * Generate a 'super awesome' crazy word.
	 * Precondition: true.
	 * Returns: Crazy word text.
	 */
	std::string Generator::GenerateSuperAwesomeSoundText(SoundType type) {
		std::string result = "";
		size_t randomIndex = 0;

		// Use first, second and third fixes with a end and uber end fix
		std::vector<std::string> firstFixes = this->firstFix[type];
		randomIndex = rand() % firstFixes.size();
		result = firstFixes[randomIndex];

		std::vector<std::string> secondFixes = this->secondFix[type];
		randomIndex = rand() % secondFixes.size();
		result = result + secondFixes[randomIndex];

		std::vector<std::string> thirdFixes = this->thirdFix[type];
		randomIndex = rand() % thirdFixes.size();
		result = result + thirdFixes[randomIndex];

		std::string endFix1, endFix2;
		std::vector<std::string> endFixes = Generator::endFix[type];
		randomIndex = rand() % endFixes.size();
		endFix1 = endFixes[randomIndex];

		std::vector<std::string> uberEndFixes = Generator::uberEndFix[type];
		randomIndex = rand() % uberEndFixes.size();
		endFix2 = uberEndFixes[randomIndex];

		result = result + Generator::JoinEndfixes(endFix1, endFix2);

		return result + this->GenerateAbsurdPunctuation(type, SUPER_AWESOME);
	}

	/*
	 * Generate a 'uber' crazy word.
	 * Precondition: true.
	 * Returns: Crazy word text.
	 */
	std::string Generator::GenerateUberSoundText(SoundType type) {
		std::string result = "";
		size_t randomIndex = 0;

		std::vector<std::string> firstFixes = this->firstFix[type];
		randomIndex = rand() % firstFixes.size();
		result = firstFixes[randomIndex];

		std::vector<std::string> secondFixes = this->secondFix[type];
		randomIndex = rand() % secondFixes.size();
		result = result + secondFixes[randomIndex];

		std::vector<std::string> thirdFixes = this->thirdFix[type];
		randomIndex = rand() % thirdFixes.size();
		result = result + thirdFixes[randomIndex];

		std::string endFix1, endFix2, endFix3;
		std::vector<std::string> endFixes = Generator::endFix[type];
		randomIndex = rand() % endFixes.size();
		endFix1 = endFixes[randomIndex];

		std::vector<std::string> superEndFixes = Generator::superEndFix[type];
		randomIndex = rand() % superEndFixes.size();
		endFix2 = superEndFixes[randomIndex];

		std::vector<std::string> uberEndFixes = Generator::uberEndFix[type];
		randomIndex = rand() % uberEndFixes.size();
		endFix3 = uberEndFixes[randomIndex];

		result = result + Generator::JoinEndfixes(endFix1, Generator::JoinEndfixes(endFix2, endFix3));

		return result + this->GenerateAbsurdPunctuation(type, UBER);
	}

}