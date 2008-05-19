#include "Onomatoplex.h"
#include <iostream>
#include <string>

// Unit testing driver, if it passes yay, otherwise boo!
static int main() {
	
	// Unit testing for the Onomatoplex --------------------------------------------------------
	// This must be examined by eye, since it is not a formal sorta thing...
	for (int i = 0; i < Onomatoplex::NumExtremenessTypes; i++) {
		Onomatoplex::Extremeness ext = static_cast<Onomatoplex::Extremeness>(i);
		for (int j = 0; j < Onomatoplex::NumSoundTypes; j++) {
			Onomatoplex::SoundType sound = static_cast<Onomatoplex::SoundType>(j);
			std::cout << "Extremeness level: " << ext << " Sound type: " << sound << std::endl;
			for (int k = 0; k < 100; k++) {
				unsigned int randomSeed = k;
				std::string word = Onomatoplex::Generator::Instance()->Generate(sound, ext, k);
				std::cout << word << std::endl;
			}
		}
		std::cout << std::endl;
	}
	// ------------------------------------------------------------------------------------------

	

	std::string empty;
	std::cin >> empty;
	return 0;
}