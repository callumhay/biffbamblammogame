#include "Onomatoplex.h"
#include "../BlammoEngine/Algebra.h"

#include <stdlib.h>
#include <assert.h>

namespace Onomatoplex {
	
	// Singleton instance
	Generator* Generator::instance = NULL;

	const std::string Generator::DEFAULT_END_PUNCTUATION = "!";
	std::vector<std::string> Generator::endPunctuation;

	/*
	 * Sets up all the dictionaries of pre/mid/post strings for making up words.
	 * This should only ever be called ONCE.
	 */
	Generator::Generator() {

		this->simpleSingleWords.clear();
		this->goodSingleWords.clear();
		this->awesomeSingleWords.clear();
		this->uberSingleWords.clear();

		Generator::endPunctuation.push_back(Generator::DEFAULT_END_PUNCTUATION);
		Generator::endPunctuation.push_back("?");

		this->LoadExplosionWords();
		this->LoadBounceWords();
		this->LoadElectricWords();
		this->LoadSmokeWords();
		this->LoadBadSadWords();
		this->LoadShotWords();
		this->LoadGoodHappyWords();
	}

	// Destructor, deletes singleton, cleans up dictionaries
	Generator::~Generator() {
		this->simpleSingleWords.clear();
		this->goodSingleWords.clear();
		this->awesomeSingleWords.clear();
		this->uberSingleWords.clear();
	}


	/**
	 * Private helper function for specifically loading explosion type words.
	 */
	void Generator::LoadExplosionWords() {
		std::vector<std::string> singleWordExplosion;
		singleWordExplosion.push_back("Biff");
		singleWordExplosion.push_back("Boom");
		singleWordExplosion.push_back("Blam");
		singleWordExplosion.push_back("Pow");
		singleWordExplosion.push_back("Ppfft");
		singleWordExplosion.push_back("Bam");
		singleWordExplosion.push_back("Floom");
		singleWordExplosion.push_back("Zam");
		this->simpleSingleWords[EXPLOSION] = singleWordExplosion;

		std::vector<std::string> goodSingleWordExplosion;
		goodSingleWordExplosion.push_back("Blaam");
		goodSingleWordExplosion.push_back("Kabloom");
		goodSingleWordExplosion.push_back("Kaboom");
		goodSingleWordExplosion.push_back("Floomo");
		goodSingleWordExplosion.push_back("Kablamm");
		goodSingleWordExplosion.push_back("Zaam");
		goodSingleWordExplosion.push_back("Kazaam");
		goodSingleWordExplosion.push_back("Blammo");
		goodSingleWordExplosion.push_back("Explodo");
		goodSingleWordExplosion.push_back("Ziff");
		goodSingleWordExplosion.push_back("Biff");
		goodSingleWordExplosion.push_back("Boomy");
		goodSingleWordExplosion.push_back("Splazoom");
		goodSingleWordExplosion.push_back("Powy");
		goodSingleWordExplosion.push_back("Pphhoom");
		this->goodSingleWords[EXPLOSION] = goodSingleWordExplosion;

		std::vector<std::string> awesomeSingleWordExplosion;
		awesomeSingleWordExplosion.push_back("BiffBlammo");
		awesomeSingleWordExplosion.push_back("Kabblaamo");
		awesomeSingleWordExplosion.push_back("Blooommoo");
		awesomeSingleWordExplosion.push_back("Wablaammy");
		awesomeSingleWordExplosion.push_back("Kaaboomy");
		awesomeSingleWordExplosion.push_back("Kabblaamy");
		awesomeSingleWordExplosion.push_back("BiffBammy");
		awesomeSingleWordExplosion.push_back("Flamblam");
		awesomeSingleWordExplosion.push_back("Explosiony");
		awesomeSingleWordExplosion.push_back("Explodo");
		awesomeSingleWordExplosion.push_back("Pphoommy");
		awesomeSingleWordExplosion.push_back("Kablaamosion");
		awesomeSingleWordExplosion.push_back("Kazaammo");
		this->awesomeSingleWords[EXPLOSION] = awesomeSingleWordExplosion;

		std::vector<std::string> uberSingleWordExplosion;
		std::vector<std::string> uberEndFixExplosion;
		uberEndFixExplosion.push_back("oplex");
		uberEndFixExplosion.push_back("oise");
		uberEndFixExplosion.push_back("osity");
		uberEndFixExplosion.push_back("etric");
		uberEndFixExplosion.push_back("ation");
		uberEndFixExplosion.push_back("otron");
		uberEndFixExplosion.push_back("atron");
		uberEndFixExplosion.push_back("inator");
		uberEndFixExplosion.push_back("plosion");
		uberEndFixExplosion.push_back("frazz");

		for (std::vector<std::string>::iterator iterEnd = uberEndFixExplosion.begin(); iterEnd != uberEndFixExplosion.end(); iterEnd++) {
			for (std::vector<std::string>::iterator iter = singleWordExplosion.begin(); iter != singleWordExplosion.end(); iter++) {
				uberSingleWordExplosion.push_back(JoinEndfixes(*iter, *iterEnd));
			}
			for (std::vector<std::string>::iterator iter = goodSingleWordExplosion.begin(); iter != goodSingleWordExplosion.end(); iter++) {
				uberSingleWordExplosion.push_back(JoinEndfixes(*iter, *iterEnd));
			}
			for (std::vector<std::string>::iterator iter = awesomeSingleWordExplosion.begin(); iter != awesomeSingleWordExplosion.end(); iter++) {
				uberSingleWordExplosion.push_back(JoinEndfixes(*iter, *iterEnd));
			}
		}
		this->uberSingleWords[EXPLOSION] = uberSingleWordExplosion;
	}

	
	/**
	 * Private helper function for specifically loading bounce type words.
	 */
	void Generator::LoadBounceWords() {
		std::vector<std::string> singleWordBounce;
		singleWordBounce.push_back("Bounce");
		singleWordBounce.push_back("Boink");
		singleWordBounce.push_back("Bipp");
		singleWordBounce.push_back("Clang");
		singleWordBounce.push_back("Bop");
		singleWordBounce.push_back("Pong");
		singleWordBounce.push_back("Boing");
		singleWordBounce.push_back("Pang");
		singleWordBounce.push_back("Ping");
		singleWordBounce.push_back("Bip");
		singleWordBounce.push_back("Plong");
		singleWordBounce.push_back("Boop");
		singleWordBounce.push_back("Thunk");
		singleWordBounce.push_back("Thonk");
		singleWordBounce.push_back("Wack");
		singleWordBounce.push_back("Wonk");
		singleWordBounce.push_back("Bonk");
		this->simpleSingleWords[BOUNCE] = singleWordBounce;

		std::vector<std::string> goodSingleWordBounce;
		goodSingleWordBounce.push_back("Bouncy");
		goodSingleWordBounce.push_back("Boingy");
		goodSingleWordBounce.push_back("Boinky");
		goodSingleWordBounce.push_back("BopBoing");
		goodSingleWordBounce.push_back("BipBop");
		goodSingleWordBounce.push_back("Plonky");
		goodSingleWordBounce.push_back("Poingo");
		goodSingleWordBounce.push_back("Thonky");
		goodSingleWordBounce.push_back("Clonk");
		goodSingleWordBounce.push_back("Pingy");
		goodSingleWordBounce.push_back("Ponk");
		goodSingleWordBounce.push_back("Clangy");
		goodSingleWordBounce.push_back("Clonk");
		goodSingleWordBounce.push_back("Smack");
		goodSingleWordBounce.push_back("Whaop");
		goodSingleWordBounce.push_back("Bloop");
		goodSingleWordBounce.push_back("Poang");
		goodSingleWordBounce.push_back("Bangy");
		this->goodSingleWords[BOUNCE] = goodSingleWordBounce;

		std::vector<std::string> awesomeSingleWordBounce;
		awesomeSingleWordBounce.push_back("BouncyBounce");
		awesomeSingleWordBounce.push_back("Booiiinng");
		awesomeSingleWordBounce.push_back("Pooonnk");
		awesomeSingleWordBounce.push_back("Clonky");
		awesomeSingleWordBounce.push_back("ClangBang");
		awesomeSingleWordBounce.push_back("BangyBong");
		awesomeSingleWordBounce.push_back("Walloop");
		awesomeSingleWordBounce.push_back("Whoomp");
		awesomeSingleWordBounce.push_back("Klaangg");
		awesomeSingleWordBounce.push_back("Floong");
		awesomeSingleWordBounce.push_back("Baassh");
		awesomeSingleWordBounce.push_back("BaaAAam");
		awesomeSingleWordBounce.push_back("Thwaaak");
		awesomeSingleWordBounce.push_back("Smaaakky");
		awesomeSingleWordBounce.push_back("Bloncko");
		awesomeSingleWordBounce.push_back("Whhaaap");
		this->awesomeSingleWords[BOUNCE] = awesomeSingleWordBounce;

		std::vector<std::string> uberSingleWordBounce;
		std::vector<std::string> uberEndFixBounce;
		uberEndFixBounce.push_back(" Zonk");
		uberEndFixBounce.push_back(" Bip");
		uberEndFixBounce.push_back(" Yoing");
		uberEndFixBounce.push_back(" Clonk");
		uberEndFixBounce.push_back(" Clank");
		uberEndFixBounce.push_back(" Bop");
		uberEndFixBounce.push_back(" Boink");
		uberEndFixBounce.push_back(" Bang");
		uberEndFixBounce.push_back(" Boing");
		uberEndFixBounce.push_back(" Blip");
		uberEndFixBounce.push_back(" Pong");
		uberEndFixBounce.push_back(" Ponk");

		for (std::vector<std::string>::iterator iterEnd = uberEndFixBounce.begin(); iterEnd != uberEndFixBounce.end(); iterEnd++) {
			for (std::vector<std::string>::iterator iter = singleWordBounce.begin(); iter != singleWordBounce.end(); iter++) {
				uberSingleWordBounce.push_back(JoinEndfixes(*iter, *iterEnd));
			}
			for (std::vector<std::string>::iterator iter = goodSingleWordBounce.begin(); iter != goodSingleWordBounce.end(); iter++) {
				uberSingleWordBounce.push_back(JoinEndfixes(*iter, *iterEnd));
			}
			for (std::vector<std::string>::iterator iter = awesomeSingleWordBounce.begin(); iter != awesomeSingleWordBounce.end(); iter++) {
				uberSingleWordBounce.push_back(JoinEndfixes(*iter, *iterEnd));
			}
		}
		this->uberSingleWords[BOUNCE] = uberSingleWordBounce;
	}
	
	/**
	 * Private helper function for specifically loading electric type words.
	 */
	void Generator::LoadElectricWords() {
		std::vector<std::string> singleWordElectric;
		singleWordElectric.push_back("Bzzt");
		singleWordElectric.push_back("Zap");
		singleWordElectric.push_back("Spark");
		singleWordElectric.push_back("Crackle");
		singleWordElectric.push_back("Fizz");
		singleWordElectric.push_back("Sizzle");
		this->simpleSingleWords[ELECTRIC] = singleWordElectric;

		std::vector<std::string> goodSingleWordElectric;
		goodSingleWordElectric.push_back("BzzaAap");
		goodSingleWordElectric.push_back("ZZzZap");
		goodSingleWordElectric.push_back("Crackley");
		goodSingleWordElectric.push_back("Sparky");
		goodSingleWordElectric.push_back("Sizzlizzle");
		goodSingleWordElectric.push_back("Fizzle");
		goodSingleWordElectric.push_back("Zaap");
		goodSingleWordElectric.push_back("PzZztt");
		goodSingleWordElectric.push_back("PssZztt");
		goodSingleWordElectric.push_back("Zappy");
		goodSingleWordElectric.push_back("BzZsstt");
		goodSingleWordElectric.push_back("Fzzstt");
		this->goodSingleWords[ELECTRIC] = goodSingleWordElectric;

		std::vector<std::string> awesomeSingleWordElectric;
		awesomeSingleWordElectric.push_back("Siizzzzley");
		awesomeSingleWordElectric.push_back("Electrofry");
		awesomeSingleWordElectric.push_back("ElectroBzzzz");
		awesomeSingleWordElectric.push_back("Biizzzzaaap");
		awesomeSingleWordElectric.push_back("Zaaapyyy");
		awesomeSingleWordElectric.push_back("PzZzZAatt");
		awesomeSingleWordElectric.push_back("FiizzZlle");
		awesomeSingleWordElectric.push_back("Electrocuty");
		awesomeSingleWordElectric.push_back("Sparkzzz");
		awesomeSingleWordElectric.push_back("Craaacckle");
		this->awesomeSingleWords[ELECTRIC] = awesomeSingleWordElectric;

		std::vector<std::string> uberSingleWordElectric;
		std::vector<std::string> uberEndFixElectric;
		uberEndFixElectric.push_back("kizzap");
		uberEndFixElectric.push_back("zizat");
		uberEndFixElectric.push_back("fizzle");
		uberEndFixElectric.push_back("blat");
		uberEndFixElectric.push_back("uple");
		uberEndFixElectric.push_back("aple");
		uberEndFixElectric.push_back("iple");
		uberEndFixElectric.push_back("frazz");

		for (std::vector<std::string>::iterator iterEnd = uberEndFixElectric.begin(); iterEnd != uberEndFixElectric.end(); iterEnd++) {
			for (std::vector<std::string>::iterator iter = singleWordElectric.begin(); iter != singleWordElectric.end(); iter++) {
				uberSingleWordElectric.push_back(JoinEndfixes(*iter, *iterEnd));
			}
			for (std::vector<std::string>::iterator iter = goodSingleWordElectric.begin(); iter != goodSingleWordElectric.end(); iter++) {
				uberSingleWordElectric.push_back(JoinEndfixes(*iter, *iterEnd));
			}
			for (std::vector<std::string>::iterator iter = awesomeSingleWordElectric.begin(); iter != awesomeSingleWordElectric.end(); iter++) {
				uberSingleWordElectric.push_back(JoinEndfixes(*iter, *iterEnd));
			}
		}
		this->uberSingleWords[ELECTRIC] = uberSingleWordElectric;
	}

	
	/**
	 * Private helper function for specifically loading smoke type words.
	 */
	void Generator::LoadSmokeWords() {
		std::vector<std::string> singleWordSmoke;
		singleWordSmoke.push_back("Puff");
		singleWordSmoke.push_back("Poof");
		singleWordSmoke.push_back("Psph");
		singleWordSmoke.push_back("Smokey");
		singleWordSmoke.push_back("Pffp");
		singleWordSmoke.push_back("Fssp");
		singleWordSmoke.push_back("Ploom");
		this->simpleSingleWords[SMOKE] = singleWordSmoke;

		std::vector<std::string> goodSingleWordSmoke;
		goodSingleWordSmoke.push_back("Poofy");
		goodSingleWordSmoke.push_back("Floommo");
		goodSingleWordSmoke.push_back("Smokey");
		goodSingleWordSmoke.push_back("Puffoo");
		goodSingleWordSmoke.push_back("Floofo");
		goodSingleWordSmoke.push_back("Psssppff");
		goodSingleWordSmoke.push_back("Psshh");
		goodSingleWordSmoke.push_back("Ploof");
		goodSingleWordSmoke.push_back("PuffPuff");
		goodSingleWordSmoke.push_back("Ploommo");
		this->goodSingleWords[SMOKE] = goodSingleWordSmoke;

		std::vector<std::string> awesomeSingleWordSmoke;
		awesomeSingleWordSmoke.push_back("Puffpuff");
		awesomeSingleWordSmoke.push_back("Flooomoo");
		awesomeSingleWordSmoke.push_back("PoofPuffy");
		awesomeSingleWordSmoke.push_back("Smokesalot");
		awesomeSingleWordSmoke.push_back("Ppphhfffft");
		awesomeSingleWordSmoke.push_back("FloomPloom");
		awesomeSingleWordSmoke.push_back("Smokahol");
		awesomeSingleWordSmoke.push_back("PoOOoOofFf");
		awesomeSingleWordSmoke.push_back("BilloOowwy");
		awesomeSingleWordSmoke.push_back("PoofPoof");
		awesomeSingleWordSmoke.push_back("FloofyPoof");
		this->awesomeSingleWords[SMOKE] = awesomeSingleWordSmoke;

		std::vector<std::string> uberSingleWordSmoke;
		uberSingleWordSmoke.push_back("Smokearama");
		uberSingleWordSmoke.push_back("SmokeSmokePoof");
		uberSingleWordSmoke.push_back("SootPoofPloom");
		uberSingleWordSmoke.push_back("PoOOoOofFfy");
		uberSingleWordSmoke.push_back("PoofPoofyFloom");
		uberSingleWordSmoke.push_back("Poofoplex");
		uberSingleWordSmoke.push_back("BillowBillowSmokey");
		uberSingleWordSmoke.push_back("FloomooPlumeoo");
		this->uberSingleWords[SMOKE] = uberSingleWordSmoke;
	}
	
	/**
	 * Private helper function for specifically loading badsad type words.
	 */
	void Generator::LoadBadSadWords() {
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
		singleWordBadSad.push_back("Boom");
		singleWordBadSad.push_back("Gaak");
		singleWordBadSad.push_back("Gah");
		singleWordBadSad.push_back("Agh");
		singleWordBadSad.push_back("Egad");
		this->simpleSingleWords[BADSAD] = singleWordBadSad;

		std::vector<std::string> goodSingleWordBadSad;
		goodSingleWordBadSad.push_back("Eeek");
		goodSingleWordBadSad.push_back("Agh");
		goodSingleWordBadSad.push_back("Gaak");
		goodSingleWordBadSad.push_back("Oof Oww");
		goodSingleWordBadSad.push_back("Ouchy");
		goodSingleWordBadSad.push_back("Pain");
		goodSingleWordBadSad.push_back("Acky");
		goodSingleWordBadSad.push_back("CryCry");
		goodSingleWordBadSad.push_back("Owwch");
		goodSingleWordBadSad.push_back("Owwchy");
		goodSingleWordBadSad.push_back("Sob Cry");
		goodSingleWordBadSad.push_back("Eeepp");
		goodSingleWordBadSad.push_back("Egads");
		goodSingleWordBadSad.push_back("Gaah");
		goodSingleWordBadSad.push_back("OwOw");
		this->goodSingleWords[BADSAD] = goodSingleWordBadSad;

		std::vector<std::string> awesomeSingleWordBadSad;
		awesomeSingleWordBadSad.push_back("OuchOuch");
		awesomeSingleWordBadSad.push_back("Owwwwww");
		awesomeSingleWordBadSad.push_back("Owwwwwwyy");
		awesomeSingleWordBadSad.push_back("Ooowf Ack");
		awesomeSingleWordBadSad.push_back("It Hurts");
		awesomeSingleWordBadSad.push_back("LotsOfPain");
		awesomeSingleWordBadSad.push_back("MakeItStop");
		awesomeSingleWordBadSad.push_back("OwwHurting");
		awesomeSingleWordBadSad.push_back("EeEeeppp");
		awesomeSingleWordBadSad.push_back("OwOwOw");
		awesomeSingleWordBadSad.push_back("Oooucchy");
		awesomeSingleWordBadSad.push_back("Paaaiin");
		awesomeSingleWordBadSad.push_back("GaAAaahh");
		awesomeSingleWordBadSad.push_back("Aaggghh");
		this->awesomeSingleWords[BADSAD] = awesomeSingleWordBadSad;

		std::vector<std::string> uberSingleWordBadSad;
		uberSingleWordBadSad.push_back("MakeTheBadManStop");
		uberSingleWordBadSad.push_back("HolyPainBatman");
		uberSingleWordBadSad.push_back("Eeeggads");
		uberSingleWordBadSad.push_back("Ouchyness");
		uberSingleWordBadSad.push_back("OuchOuchOuch");
		uberSingleWordBadSad.push_back("OwOwOwOw");
		uberSingleWordBadSad.push_back("Aggghhhhh");
		uberSingleWordBadSad.push_back("ItHurtsOhSoBad");
		uberSingleWordBadSad.push_back("ItHurtsSoMuch");
		uberSingleWordBadSad.push_back("BadNewsBears");
		uberSingleWordBadSad.push_back("Eeeekkkk");
		uberSingleWordBadSad.push_back("MakeThePainStop");
		this->uberSingleWords[BADSAD] = uberSingleWordBadSad;
	}

	/**
	 * Private helper function for specifically loading shooting type words.
	 */
	void Generator::LoadShotWords() {
		std::vector<std::string> shotSingleWords;
		shotSingleWords.push_back("Pew pew pew");
		shotSingleWords.push_back("Blam Blam");
		shotSingleWords.push_back("Bang Bang");
		shotSingleWords.push_back("Pow pow");
		shotSingleWords.push_back("Blasty");
		shotSingleWords.push_back("Blasto");
		shotSingleWords.push_back("Blammo");
		shotSingleWords.push_back("Zap");
		shotSingleWords.push_back("Shooty");
		shotSingleWords.push_back("Pew");
		shotSingleWords.push_back("Ploop");
		shotSingleWords.push_back("Spew");
		this->simpleSingleWords[SHOT]		= shotSingleWords;
		this->goodSingleWords[SHOT]			= shotSingleWords;
		this->awesomeSingleWords[SHOT]	= shotSingleWords;
		this->uberSingleWords[SHOT]			= shotSingleWords;
	}

	
	/**
	 * Private helper function for specifically loading good happy type words.
	 */
	void Generator::LoadGoodHappyWords() {
		std::vector<std::string> goodHappySingleWords;
		goodHappySingleWords.push_back("Yay");
		goodHappySingleWords.push_back("Woot");
		goodHappySingleWords.push_back("Rad");
		goodHappySingleWords.push_back("Cool");
		goodHappySingleWords.push_back("Nice");
		goodHappySingleWords.push_back("Neat");
		goodHappySingleWords.push_back("Happy");
		goodHappySingleWords.push_back("Woo");
		goodHappySingleWords.push_back("Super");
		goodHappySingleWords.push_back("Joy");
		goodHappySingleWords.push_back("Great");
		goodHappySingleWords.push_back("Hazzah");
		goodHappySingleWords.push_back("Hurray");
		goodHappySingleWords.push_back("Good");
		goodHappySingleWords.push_back("Sweet");
		this->simpleSingleWords[GOODHAPPY] = goodHappySingleWords;
		
		std::vector<std::string> goodHappyGoodWords;
		goodHappyGoodWords.push_back("Woohoo");
		goodHappyGoodWords.push_back("Whoopie");
		goodHappyGoodWords.push_back("Tubular");
		goodHappyGoodWords.push_back("Radical");
		goodHappyGoodWords.push_back("Neato");
		goodHappyGoodWords.push_back("Yeehaa");
		goodHappyGoodWords.push_back("Sweetness");
		goodHappyGoodWords.push_back("Niceness");
		goodHappyGoodWords.push_back("Groovy");
		goodHappyGoodWords.push_back("Super");
		goodHappyGoodWords.push_back("Wow");
		goodHappyGoodWords.push_back("Great");
		goodHappyGoodWords.push_back("Goody");
		goodHappyGoodWords.push_back("Wonderful");
		goodHappyGoodWords.push_back("Joy Joy");
		goodHappyGoodWords.push_back("Splendid");
		this->goodSingleWords[GOODHAPPY] = goodHappyGoodWords;

		std::vector<std::string> awesomeHappyGoodWords;
		awesomeHappyGoodWords.push_back("Awesome");
		awesomeHappyGoodWords.push_back("Yummy");
		awesomeHappyGoodWords.push_back("Delicious");
		awesomeHappyGoodWords.push_back("Spectacular");
		awesomeHappyGoodWords.push_back("Brilliant");
		awesomeHappyGoodWords.push_back("Wicked");
		awesomeHappyGoodWords.push_back("Superb");
		awesomeHappyGoodWords.push_back("Fabulous");
		awesomeHappyGoodWords.push_back("Groovyness");
		awesomeHappyGoodWords.push_back("Wonderbar");
		awesomeHappyGoodWords.push_back("Glorious");
		awesomeHappyGoodWords.push_back("Terrific");
		awesomeHappyGoodWords.push_back("Sensational");
		awesomeHappyGoodWords.push_back("Gorgeous");
		awesomeHappyGoodWords.push_back("Well Done");
		awesomeHappyGoodWords.push_back("Nice One");
		awesomeHappyGoodWords.push_back("Happy Happy");
		this->awesomeSingleWords[GOODHAPPY] = awesomeHappyGoodWords;

		std::vector<std::string> uberHappyGoodWords;
		uberHappyGoodWords.push_back("Magnifico");
		uberHappyGoodWords.push_back("Magnificent");
		uberHappyGoodWords.push_back("Incredible");
		uberHappyGoodWords.push_back("Sweet Awesome");
		uberHappyGoodWords.push_back("Very Nice");
		uberHappyGoodWords.push_back("Wicked Awesome");
		uberHappyGoodWords.push_back("Insane Cool");
		uberHappyGoodWords.push_back("Marvellous");
		uberHappyGoodWords.push_back("Primo");
		uberHappyGoodWords.push_back("Dazzling");
		uberHappyGoodWords.push_back("Divine");
		uberHappyGoodWords.push_back("Happy Happy Joy Joy");
		uberHappyGoodWords.push_back("Joygasm");
		uberHappyGoodWords.push_back("Totally Tubular");
		uberHappyGoodWords.push_back("Awesomeness");
		uberHappyGoodWords.push_back("Fantabulous");
		uberHappyGoodWords.push_back("Impressive");
		this->uberSingleWords[GOODHAPPY] = uberHappyGoodWords;
	}

	/*
	 * Generates crazy onomatopoeia based on the sound type given and the extremeness of the context.
	 * Precondition: true.
	 * Returns: Onomatopoeia text.
	 */
	std::string Generator::Generate(SoundType type, Extremeness amt) {
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
			unsigned int randomChoice =  Randomizer::GetInstance()->RandomUnsignedInt() % 2;
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
		unsigned int randomAmt =  Randomizer::GetInstance()->RandomUnsignedInt() % (amount + 1);

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
					size_t randomIndex =  Randomizer::GetInstance()->RandomUnsignedInt() % Generator::endPunctuation.size();
					punctuation = punctuation + Generator::endPunctuation[randomIndex];
				}
			}
		}

		switch(type) {
			case ELECTRIC:
			case BOUNCE:
				if (punctuation.size() > 1 && (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 1)) {
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
		unsigned int randomIndex =  Randomizer::GetInstance()->RandomUnsignedInt() % this->simpleSingleWords[type].size();
		return this->simpleSingleWords[type][randomIndex];
	}

	/*
	 * Generate a 'normal' crazy word.
	 * Precondition: true.
	 * Returns: Crazy word text.
	 */
	std::string Generator::GenerateNormalSoundText(SoundType type) {
		// These are just weak words but with the possiblity of end punctuation.
		std::string result = this->GenerateWeakSoundText(type);
		return result + this->GenerateAbsurdPunctuation(type, NORMAL);
	}

	/*
	 * Generate a 'pretty good' crazy word.
	 * Precondition: true.
	 * Returns: Crazy word text.
	 */
	std::string Generator::GeneratePrettyGoodSoundText(SoundType type) {
		std::vector<std::string> &fullSimpleWords = this->goodSingleWords[type];
		unsigned int randomIndex =  Randomizer::GetInstance()->RandomUnsignedInt() % fullSimpleWords.size();
		std::string result = fullSimpleWords[randomIndex];
		return result + this->GenerateAbsurdPunctuation(type, PRETTY_GOOD);
	}

	/*
	 * Generate a 'good' crazy word.
	 * Precondition: true.
	 * Returns: Crazy word text.
	 */
	std::string Generator::GenerateGoodSoundText(SoundType type) {

		std::vector<std::string> &fullSimpleWords = this->goodSingleWords[type];
		unsigned int randomIndex =  Randomizer::GetInstance()->RandomUnsignedInt() % fullSimpleWords.size();
		std::string result = fullSimpleWords[randomIndex];
		return result + this->GenerateAbsurdPunctuation(type, GOOD);
	}

	/*
	 * Generate a 'awesome' crazy word.
	 * Precondition: true.
	 * Returns: Crazy word text.
	 */
	std::string Generator::GenerateAwesomeSoundText(SoundType type) {
		std::vector<std::string> &fullSimpleWords = this->awesomeSingleWords[type];
		unsigned int randomIndex =  Randomizer::GetInstance()->RandomUnsignedInt() % fullSimpleWords.size();
		std::string result = fullSimpleWords[randomIndex];
		return result + this->GenerateAbsurdPunctuation(type, AWESOME);
	}

	/*
	 * Generate a 'super awesome' crazy word.
	 * Precondition: true.
	 * Returns: Crazy word text.
	 */
	std::string Generator::GenerateSuperAwesomeSoundText(SoundType type) {
		std::vector<std::string> &fullSimpleWords = this->awesomeSingleWords[type];
		unsigned int randomIndex =  Randomizer::GetInstance()->RandomUnsignedInt() % fullSimpleWords.size();
		std::string result = fullSimpleWords[randomIndex];
		return result + this->GenerateAbsurdPunctuation(type, SUPER_AWESOME);
	}

	/*
	 * Generate a 'uber' crazy word.
	 * Precondition: true.
	 * Returns: Crazy word text.
	 */
	std::string Generator::GenerateUberSoundText(SoundType type) {
		std::vector<std::string> &fullSimpleWords = this->uberSingleWords[type];
		unsigned int randomIndex =  Randomizer::GetInstance()->RandomUnsignedInt() % fullSimpleWords.size();
		std::string result = fullSimpleWords[randomIndex];
		return result + this->GenerateAbsurdPunctuation(type, UBER);
	}

}