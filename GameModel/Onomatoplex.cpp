/**
 * Onomatoplex.cpp
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

#include "Onomatoplex.h"
#include "../BlammoEngine/Algebra.h"

#include <stdlib.h>
#include <assert.h>

using namespace Onomatoplex;
	
// Singleton instance
Generator* Generator::instance = NULL;

const char* Generator::DEFAULT_END_PUNCTUATION = "!";
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
	this->LoadGooWords();
	this->LoadShatterWords();
	this->LoadCrazyWords();
    this->LoadLaunchWords();
    this->LoadAttachWords();
    this->LoadAngryWords();

    this->LoadVictoryDescriptors();
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
    singleWordExplosion.push_back("Blamm");
    singleWordExplosion.push_back("Blast");
	singleWordExplosion.push_back("Pow");
	singleWordExplosion.push_back("Ppfft");
	singleWordExplosion.push_back("Bam");
	singleWordExplosion.push_back("Floom");
	singleWordExplosion.push_back("Ploom");
	singleWordExplosion.push_back("Zam");
    singleWordExplosion.push_back("Zonk");
	singleWordExplosion.push_back("Ziff");
	singleWordExplosion.push_back("Poof");
	singleWordExplosion.push_back("Ploof");
    singleWordExplosion.push_back("Bloof");
    singleWordExplosion.push_back("Piff");
    singleWordExplosion.push_back("Poff");
    singleWordExplosion.push_back("Splam");
    singleWordExplosion.push_back("Zop");
    singleWordExplosion.push_back("Zoop");
    singleWordExplosion.push_back("Zorp");
    singleWordExplosion.push_back("Zoorp");
    singleWordExplosion.push_back("Whoap");
    singleWordExplosion.push_back("Whoop");
    singleWordExplosion.push_back("Zoop");
	this->simpleSingleWords[EXPLOSION] = singleWordExplosion;

	std::vector<std::string> goodSingleWordExplosion;
	goodSingleWordExplosion.push_back("Blaam");
    goodSingleWordExplosion.push_back("Blaamy");
    goodSingleWordExplosion.push_back("Blasty");
    goodSingleWordExplosion.push_back("Blasto");
	goodSingleWordExplosion.push_back("Kabloom");
	goodSingleWordExplosion.push_back("Kaboom");
	goodSingleWordExplosion.push_back("Floomo");
    goodSingleWordExplosion.push_back("Flammo");
	goodSingleWordExplosion.push_back("Kablamm");
	goodSingleWordExplosion.push_back("Zaam");
	goodSingleWordExplosion.push_back("Kazaam");
	goodSingleWordExplosion.push_back("Blammo");
	goodSingleWordExplosion.push_back("Explodo");
    goodSingleWordExplosion.push_back("Explosion");
	goodSingleWordExplosion.push_back("Ziff");
	goodSingleWordExplosion.push_back("Biff");
	goodSingleWordExplosion.push_back("Boomy");
    goodSingleWordExplosion.push_back("Poofy");
	goodSingleWordExplosion.push_back("Splazoom");
	goodSingleWordExplosion.push_back("Powy");
    goodSingleWordExplosion.push_back("Zoppow");
	goodSingleWordExplosion.push_back("Splammo");
	goodSingleWordExplosion.push_back("Sploom");
	goodSingleWordExplosion.push_back("Pphhoom");
	goodSingleWordExplosion.push_back("Kurfloom");
	goodSingleWordExplosion.push_back("Kurflaam");
    goodSingleWordExplosion.push_back("Zorpowow");
    goodSingleWordExplosion.push_back("Implosion");
    goodSingleWordExplosion.push_back("Implodo");
	this->goodSingleWords[EXPLOSION] = goodSingleWordExplosion;

	std::vector<std::string> awesomeSingleWordExplosion;
	awesomeSingleWordExplosion.push_back("Kaafloom");
	awesomeSingleWordExplosion.push_back("Kaafloomy");
	awesomeSingleWordExplosion.push_back("Kaafloomo");
    awesomeSingleWordExplosion.push_back("Kaablasto");
	awesomeSingleWordExplosion.push_back("BiffBlammo");
	awesomeSingleWordExplosion.push_back("BiffBamBlammo");
	awesomeSingleWordExplosion.push_back("Kabblaamo");
	awesomeSingleWordExplosion.push_back("Blooommoo");
	awesomeSingleWordExplosion.push_back("Wablaammy");
    awesomeSingleWordExplosion.push_back("Wamblasto");
	awesomeSingleWordExplosion.push_back("Kaaboomy");
	awesomeSingleWordExplosion.push_back("Kabblaamy");
	awesomeSingleWordExplosion.push_back("BiffBammy");
	awesomeSingleWordExplosion.push_back("Flamblam");
    awesomeSingleWordExplosion.push_back("Flamblasto");
	awesomeSingleWordExplosion.push_back("Explodey");
    awesomeSingleWordExplosion.push_back("Explosiony");
    awesomeSingleWordExplosion.push_back("Explooosion");
	awesomeSingleWordExplosion.push_back("Pphoommy");
	awesomeSingleWordExplosion.push_back("Kablaamosion");
	awesomeSingleWordExplosion.push_back("Kazaammo");
	awesomeSingleWordExplosion.push_back("Kazaammy");
    awesomeSingleWordExplosion.push_back("Zorpowowy");
    awesomeSingleWordExplosion.push_back("Implodey");
	this->awesomeSingleWords[EXPLOSION] = awesomeSingleWordExplosion;

	std::vector<std::string> uberSingleWordExplosion;
	std::vector<std::string> uberEndFixExplosion;
	uberEndFixExplosion.push_back("oplex");
	uberEndFixExplosion.push_back("oplast");
	uberEndFixExplosion.push_back("osity");
	uberEndFixExplosion.push_back("ation");
	uberEndFixExplosion.push_back("otron");
    uberEndFixExplosion.push_back("otronic");
	uberEndFixExplosion.push_back("atron");
    uberEndFixExplosion.push_back("atronic");
    uberEndFixExplosion.push_back("aphon");
    uberEndFixExplosion.push_back("ophon");
    uberEndFixExplosion.push_back("aphonic");
    uberEndFixExplosion.push_back("ophonic");
	uberEndFixExplosion.push_back("inator");
	uberEndFixExplosion.push_back("plosion");
	uberEndFixExplosion.push_back("afrazz");
    uberEndFixExplosion.push_back("nation");

	for (std::vector<std::string>::iterator iterEnd = uberEndFixExplosion.begin(); iterEnd != uberEndFixExplosion.end(); ++iterEnd) {
		for (std::vector<std::string>::iterator iter = goodSingleWordExplosion.begin(); iter != goodSingleWordExplosion.end(); ++iter) {
			uberSingleWordExplosion.push_back(JoinEndfixes(*iter, *iterEnd));
		}
		for (std::vector<std::string>::iterator iter = awesomeSingleWordExplosion.begin(); iter != awesomeSingleWordExplosion.end(); ++iter) {
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
	singleWordBounce.push_back("Clonk");
	singleWordBounce.push_back("Conk");
	singleWordBounce.push_back("Thwack");
	singleWordBounce.push_back("Bop");
	singleWordBounce.push_back("Pong");
	singleWordBounce.push_back("Boing");
	singleWordBounce.push_back("Pang");
	singleWordBounce.push_back("Ping");
    singleWordBounce.push_back("Pip");
	singleWordBounce.push_back("Bip");
	singleWordBounce.push_back("Plong");
	singleWordBounce.push_back("Boop");
	singleWordBounce.push_back("Thunk");
	singleWordBounce.push_back("Thonk");
	singleWordBounce.push_back("Wack");
	singleWordBounce.push_back("Wonk");
	singleWordBounce.push_back("Bonk");
	singleWordBounce.push_back("Smack");
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
	goodSingleWordBounce.push_back("Smash");
    goodSingleWordBounce.insert(goodSingleWordBounce.end(), singleWordBounce.begin(), singleWordBounce.end());
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

	for (std::vector<std::string>::iterator iterEnd = uberEndFixBounce.begin(); iterEnd != uberEndFixBounce.end(); ++iterEnd) {
		for (std::vector<std::string>::iterator iter = goodSingleWordBounce.begin(); iter != goodSingleWordBounce.end(); ++iter) {
			uberSingleWordBounce.push_back(JoinEndfixes(*iter, *iterEnd));
		}
		for (std::vector<std::string>::iterator iter = awesomeSingleWordBounce.begin(); iter != awesomeSingleWordBounce.end(); ++iter) {
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
	goodSingleWordElectric.push_back("Ksszzk");
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
    uberEndFixElectric.push_back("etric");

	for (std::vector<std::string>::iterator iterEnd = uberEndFixElectric.begin(); iterEnd != uberEndFixElectric.end(); ++iterEnd) {
		for (std::vector<std::string>::iterator iter = goodSingleWordElectric.begin(); iter != goodSingleWordElectric.end(); ++iter) {
			uberSingleWordElectric.push_back(JoinEndfixes(*iter, *iterEnd));
		}
		for (std::vector<std::string>::iterator iter = awesomeSingleWordElectric.begin(); iter != awesomeSingleWordElectric.end(); ++iter) {
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
    singleWordBadSad.push_back("Narf");
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
	shotSingleWords.push_back("Blam");
	shotSingleWords.push_back("Bang");
	shotSingleWords.push_back("Pow");
	shotSingleWords.push_back("Blast");
	shotSingleWords.push_back("Shoot");
	shotSingleWords.push_back("Pew");
	shotSingleWords.push_back("Ploom");
	shotSingleWords.push_back("Floom");
	shotSingleWords.push_back("Spew");
	shotSingleWords.push_back("Fire");
	this->simpleSingleWords[SHOT]		= shotSingleWords;

	std::vector<std::string> shotBetterWords;
	shotBetterWords.push_back("Pew Pew");
	shotBetterWords.push_back("Bang Bang");
	shotBetterWords.push_back("Blam Blam");
	shotBetterWords.push_back("Powy");
	shotBetterWords.push_back("Blasto");
	shotBetterWords.push_back("Blasty");
	shotBetterWords.push_back("Blammo");
	shotBetterWords.push_back("Ploomo");
	shotBetterWords.push_back("Floomo");
	shotBetterWords.push_back("Kablaam");
	shotBetterWords.push_back("Kaboom");
	this->goodSingleWords[SHOT] = shotBetterWords;
	this->goodSingleWords[SHOT].insert(this->goodSingleWords[SHOT].end(), shotSingleWords.begin(), shotSingleWords.end());

	std::vector<std::string> shotAwesomeWords;
	shotAwesomeWords.push_back("Pew Pew Pew");
	shotAwesomeWords.push_back("Blastotron");
	shotAwesomeWords.push_back("Kabloomanator");
	this->awesomeSingleWords[SHOT] = shotAwesomeWords;
	this->awesomeSingleWords[SHOT].insert(this->awesomeSingleWords[SHOT].end(), shotSingleWords.begin(), shotSingleWords.end());
	this->awesomeSingleWords[SHOT].insert(this->awesomeSingleWords[SHOT].end(), shotBetterWords.begin(), shotBetterWords.end());
	this->uberSingleWords[SHOT] = this->awesomeSingleWords[SHOT];
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


/**
 * Private helper function for loading goo type words.
 */
void Generator::LoadGooWords() {
	std::vector<std::string> gooSimpleSingleWords;
	gooSimpleSingleWords.reserve(13);
	gooSimpleSingleWords.push_back("Splat");
	gooSimpleSingleWords.push_back("Goo");
	gooSimpleSingleWords.push_back("Gack");
	gooSimpleSingleWords.push_back("Plop");
	gooSimpleSingleWords.push_back("Gloop");
	gooSimpleSingleWords.push_back("Ploop");
	gooSimpleSingleWords.push_back("Gloop");
	gooSimpleSingleWords.push_back("Gob");
	gooSimpleSingleWords.push_back("Glob");
	gooSimpleSingleWords.push_back("Glop");
	gooSimpleSingleWords.push_back("Goop");
	gooSimpleSingleWords.push_back("Blob");
	gooSimpleSingleWords.push_back("Blop");
    gooSimpleSingleWords.push_back("Ptooey");
	this->simpleSingleWords[GOO] = gooSimpleSingleWords;

	std::vector<std::string> gooGoodSingleWords;
	gooGoodSingleWords.reserve(9 + gooSimpleSingleWords.size());
	gooGoodSingleWords.push_back("Splotch");
	gooGoodSingleWords.push_back("Splatter");
	gooGoodSingleWords.push_back("Gooey");
	gooGoodSingleWords.push_back("Spewy");
	gooGoodSingleWords.push_back("Goober");
	gooGoodSingleWords.push_back("Gloopy");
	gooGoodSingleWords.push_back("Bloop");
	gooGoodSingleWords.push_back("Splortch");
	gooGoodSingleWords.push_back("Globber");
	gooGoodSingleWords.insert(gooGoodSingleWords.end(), gooSimpleSingleWords.begin(), gooSimpleSingleWords.end());
	this->goodSingleWords[GOO] = gooGoodSingleWords;

	std::vector<std::string> gooAwesomeSingleWords;
	gooAwesomeSingleWords.reserve(3 + gooSimpleSingleWords.size()*gooSimpleSingleWords.size() - gooSimpleSingleWords.size());
	gooAwesomeSingleWords.push_back("Gobbledygook");
	gooAwesomeSingleWords.push_back("Goobergack");
	gooAwesomeSingleWords.push_back("Gooey Kablewy");
	for (unsigned int i = 0; i < gooSimpleSingleWords.size(); i++) {
		for (unsigned int j = 0; j < gooSimpleSingleWords.size(); j++) {
			if (i == j) { continue; }
			gooAwesomeSingleWords.push_back(gooSimpleSingleWords[i] + " " + gooSimpleSingleWords[j]);
		}
	}
	this->awesomeSingleWords[GOO] = gooAwesomeSingleWords;

	std::vector<std::string> gooUberSingleWords;
	gooUberSingleWords.reserve(4 + gooGoodSingleWords.size() * gooSimpleSingleWords.size());
	gooUberSingleWords.push_back("SplattyMcGoo");
	gooUberSingleWords.push_back("Splatterama");
	gooUberSingleWords.push_back("Gobbledygook");
	gooUberSingleWords.push_back("EwwGooSplat");
	for (unsigned int i = 0; i < gooGoodSingleWords.size(); i++) {
		for (unsigned int j = 0; j < gooSimpleSingleWords.size(); j++) {
			gooUberSingleWords.push_back(gooGoodSingleWords[i] + " " +  gooSimpleSingleWords[j]);
		}
	}
	this->uberSingleWords[GOO] = gooUberSingleWords;

}


void Generator::LoadShatterWords() {
	std::vector<std::string> shatterSimpleWords;
	shatterSimpleWords.push_back("Crack");
	shatterSimpleWords.push_back("Smash");
	shatterSimpleWords.push_back("Shatter");
	shatterSimpleWords.push_back("Crunch");
    shatterSimpleWords.push_back("Crush");
	shatterSimpleWords.push_back("Splinter");
	shatterSimpleWords.push_back("Break");
	shatterSimpleWords.push_back("Krrzzk");
	this->simpleSingleWords[SHATTER] = shatterSimpleWords;

	std::vector<std::string> shatterGoodWords;
	shatterGoodWords.push_back("Smashy");
	shatterGoodWords.push_back("Crackkk");
	shatterGoodWords.push_back("Cracky");
	shatterGoodWords.push_back("Krssszzk");
    shatterGoodWords.push_back("Crrggnk");
    shatterGoodWords.push_back("Crringgk");
    shatterGoodWords.push_back("Crrizzk");
	shatterGoodWords.push_back("Smaassh");
	shatterGoodWords.push_back("Crrassh");
	shatterGoodWords.push_back("Crunchy");
	this->goodSingleWords[SHATTER] = shatterGoodWords;
	this->goodSingleWords[SHATTER].insert(this->goodSingleWords[SHATTER].end(), shatterSimpleWords.begin(), shatterSimpleWords.end());

	this->awesomeSingleWords[SHATTER] = this->goodSingleWords[SHATTER];
	this->uberSingleWords[SHATTER] = this->goodSingleWords[SHATTER];
}

void Generator::LoadCrazyWords() {
	std::vector<std::string> crazySimpleWords;
	//crazySimpleWords.push_back("Waka");
	//crazySimpleWords.push_back("Wee");
    crazySimpleWords.reserve(3);
	crazySimpleWords.push_back("?");
	crazySimpleWords.push_back("!");
	//crazySimpleWords.push_back("Mummble");
	//crazySimpleWords.push_back("Gaah");
	//crazySimpleWords.push_back("Moo");
	//crazySimpleWords.push_back("Meow");
	//crazySimpleWords.push_back("Woof");
	//crazySimpleWords.push_back("Doo");
	//crazySimpleWords.push_back("Daa");
	//crazySimpleWords.push_back("Duurrr");
	//crazySimpleWords.push_back("Blee");
	//crazySimpleWords.push_back("Bloo");
	crazySimpleWords.push_back("");

	this->simpleSingleWords[CRAZY]  = crazySimpleWords;
	this->goodSingleWords[CRAZY]	= crazySimpleWords;
	this->awesomeSingleWords[CRAZY] = crazySimpleWords;
	this->uberSingleWords[CRAZY]	= crazySimpleWords;
}

void Generator::LoadLaunchWords() {
    std::vector<std::string> launchSimpleWords;
    launchSimpleWords.reserve(9);
    
    launchSimpleWords.push_back("Launch");
    launchSimpleWords.push_back("Woosh");
    launchSimpleWords.push_back("Ploong");
    launchSimpleWords.push_back("Floong");
    launchSimpleWords.push_back("Pwoosh");
    launchSimpleWords.push_back("Fling");
    launchSimpleWords.push_back("Spoop");
    launchSimpleWords.push_back("Poop");
    launchSimpleWords.push_back("Ptooey");

	this->simpleSingleWords[LAUNCH]  = launchSimpleWords;
	this->goodSingleWords[LAUNCH]	 = launchSimpleWords;
	this->awesomeSingleWords[LAUNCH] = launchSimpleWords;
	this->uberSingleWords[LAUNCH]	 = launchSimpleWords;
}

void Generator::LoadAttachWords() {
    std::vector<std::string> attachSimpleWords;
    attachSimpleWords.reserve(15);

    attachSimpleWords.push_back("Kachunk");
    attachSimpleWords.push_back("Clunk");
    attachSimpleWords.push_back("Chunk");
    attachSimpleWords.push_back("Plunk");
    attachSimpleWords.push_back("Fasten");
    attachSimpleWords.push_back("Attach");
    attachSimpleWords.push_back("Kaclunk");
    attachSimpleWords.push_back("Blomp");
    attachSimpleWords.push_back("Glomp");
    attachSimpleWords.push_back("Shlomp");
    attachSimpleWords.push_back("Kurblomp");
    attachSimpleWords.push_back("Kurchunk");
    attachSimpleWords.push_back("Kurclunk");
    attachSimpleWords.push_back("Kurplunk");
    attachSimpleWords.push_back("Kurshlomp");

	this->simpleSingleWords[ATTACH]  = attachSimpleWords;
	this->goodSingleWords[ATTACH]	 = attachSimpleWords;
	this->awesomeSingleWords[ATTACH] = attachSimpleWords;
	this->uberSingleWords[ATTACH]	 = attachSimpleWords;
}

void Generator::LoadAngryWords() {
    std::vector<std::string> angrySimpleWords;
    angrySimpleWords.push_back("Grrrrr");
    angrySimpleWords.push_back("Rawwrr");
    angrySimpleWords.push_back("Arrggh");
    angrySimpleWords.push_back("Blarrg");
    angrySimpleWords.push_back("Errggh");
    angrySimpleWords.push_back("Errggh");
    angrySimpleWords.push_back("Flarrg");
    angrySimpleWords.push_back("Urrgah");
    angrySimpleWords.push_back("Grrggh");
    angrySimpleWords.push_back("Grrwar");
    angrySimpleWords.push_back("Grawwr");
    angrySimpleWords.push_back("Grawwr");

	this->simpleSingleWords[ANGRY]   = angrySimpleWords;
	this->goodSingleWords[ANGRY]	 = angrySimpleWords;
	this->awesomeSingleWords[ANGRY]  = angrySimpleWords;
	this->uberSingleWords[ANGRY]	 = angrySimpleWords;
}

void Generator::LoadVictoryDescriptors() {
    this->victoryDescriptors.clear();

    this->victoryDescriptors.push_back("Sweet");
    this->victoryDescriptors.push_back("Succulent");
    this->victoryDescriptors.push_back("Delicious");
    this->victoryDescriptors.push_back("Immaculate");
    this->victoryDescriptors.push_back("Illustrious");
    this->victoryDescriptors.push_back("Invigorating");
    this->victoryDescriptors.push_back("Satiating");
    this->victoryDescriptors.push_back("Sunshiny");
    this->victoryDescriptors.push_back("Sensational");
    this->victoryDescriptors.push_back("Splendiferous");
    this->victoryDescriptors.push_back("Awesome");
    this->victoryDescriptors.push_back("Implacable");
    this->victoryDescriptors.push_back("Juicy");
    this->victoryDescriptors.push_back("Gooey");
    this->victoryDescriptors.push_back("Satisfying");
    this->victoryDescriptors.push_back("Glorious");
    this->victoryDescriptors.push_back("Satisfying");

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

std::string Generator::GenerateVictoryDescriptor() const {
    return this->victoryDescriptors[Randomizer::GetInstance()->RandomUnsignedInt() % this->victoryDescriptors.size()];
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
	if (Generator::IsVowel(endFix1.at(endFix1.size()-1))) {

		// find the first non-vowel in endfix1 and append the rest
		size_t indexOfNonVowel1 = endFix1.find_last_not_of("aAeEiIoOuU");
		if (indexOfNonVowel1 == std::string::npos) {
			return endFix2;
		}

		result = endFix1.substr(0, indexOfNonVowel1 + 1) + endFix2;
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
