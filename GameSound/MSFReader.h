#ifndef _MSF_READER_H_
#define _MSF_READER_H_

#include "../BlammoEngine/BasicIncludes.h"

#include "Sound.h"

class MSFReader {

private:
	MSFReader(){}

public:
	static bool ReadMainMenuMSF(std::map<Sound::MainMenuSound, Sound*>& mainMenuSounds);


};

#endif // _MSF_READER_H_