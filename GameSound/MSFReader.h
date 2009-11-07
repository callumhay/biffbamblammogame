#ifndef _MSF_READER_H_
#define _MSF_READER_H_

#include "../BlammoEngine/BasicIncludes.h"

#include "Sound.h"

class MSFReader {

public:
	static bool ReadMainMenuMSF(std::map<Sound::MainMenuSound, Sound*>& mainMenuSounds);

private:
	MSFReader(){}

	// Syntax symbols for the MSF
	static const char* OPEN_SOUND_DEFINTION_BLOCK;
	static const char* CLOSE_SOUND_DEFINITION_BLOCK;
	
	// Keyword parameters for the MSF definition blocks
	static const char* FILE_KEYWORD;
	static const char* DELAY_KEYWORD;
	static const char* LOOPS_KEYWORD;
	static const char* FADE_IN_KEYWORD;
	static const char* FADE_OUT_KEYWORD;

	// Sound string identifiers
	static const char* MAIN_MENU_BG_MASK;
	static const char* MAIN_MENU_ITEM_HIGHLIGHTED_EVENT;
	static const char* MAIN_MENU_ITEM_ENTERED_EVENT;
	static const char* MAIN_MENU_ITEM_SELECTED_EVENT;
	static const char* MAIN_MENU_ITEM_SCROLLED_EVENT;

	// Initializer/Default values
	static const int INVALID_SOUND_TYPE = -1;
	static const int DEFAULT_DELAY = 0;
	static const int DEFAULT_LOOPS = 1;
	static const int DEFAULT_FADEIN = 0;
	static const int DEFAULT_FADEOUT = 0;

	static int ConvertMainMenuKeywordToSoundType(const std::string& soundName);

	static bool FoundEqualsSyntax(bool& noEquals, std::string& errorStr, std::istringstream* inStream);

};

#endif // _MSF_READER_H_