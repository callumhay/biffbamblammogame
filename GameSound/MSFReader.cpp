#include "MSFReader.h"
#include "SoundEvent.h"
#include "SoundMask.h"

#include "../ResourceManager.h"

#include "../GameView/GameViewConstants.h"

bool MSFReader::ReadMainMenuMSF(std::map<Sound::MainMenuSound, Sound*>& mainMenuSounds) {

	// Grab a file in stream from the main manu music script file
	std::istringstream* inStream = ResourceManager::GetInstance()->FilepathToInStream(GameViewConstants::GetInstance()->MAIN_MENU_SOUND_SCRIPT);

	// TODO: Read in the file...

	// Clean up and finish
	delete inStream;
	inStream = NULL;

	return true;
}