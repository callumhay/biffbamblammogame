#ifndef _MSF_READER_H_
#define _MSF_READER_H_

#include "../BlammoEngine/BasicIncludes.h"

class GameSound;

class MSFReader {

public:
	static bool ReadMSF(const std::string& filepath, std::map<int, GameSound*>& sounds);

private:
	MSFReader(){}

	// Syntax symbols for the MSF
	static const char* OPEN_SOUND_DEFINTION_BLOCK;
	static const char* CLOSE_SOUND_DEFINITION_BLOCK;
	static const char* OPEN_ENCLOSING_PROB_FILE_PAIR;
	static const char* CLOSE_ENCLOSING_PROB_FILE_PAIR;
	static const char* PROB_DEFINITION_SYNTAX;

	// Other keywords for the MSF
	static const char* IGNORE_KEYWORD;

	// Keyword parameters for the MSF definition blocks
	static const char* FILE_KEYWORD;
	static const char* DELAY_KEYWORD;
	static const char* LOOPS_KEYWORD;
	static const char* FADE_IN_KEYWORD;
	static const char* FADE_OUT_KEYWORD;

	// Sound string identifiers
	static const char* MAIN_MENU_BG_MASK;
	static const char* MAIN_MENU_BG_BANG_SMALL_EVENT;
	static const char* MAIN_MENU_BG_BANG_MEDIUM_EVENT;
	static const char* MAIN_MENU_BG_BANG_BIG_EVENT;
	static const char* MAIN_MENU_ITEM_HIGHLIGHTED_EVENT;
	static const char* MAIN_MENU_ITEM_ENTERED_EVENT;
	static const char* MAIN_MENU_ITEM_BACK_AND_CANCEL_EVENT;
	static const char* MAIN_MENU_ITEM_VERIFY_AND_SEL_EVENT;
	static const char* MAIN_MENU_ITEM_SCROLLED_EVENT;

	// Initializer/Default values
	static const int INVALID_SOUND_TYPE = -1;

	static int ConvertKeywordToSoundType(const std::string& soundName);
	static bool FoundEqualsSyntax(bool& noEquals, std::string& errorStr, std::istream* inStream);

};

#endif // _MSF_READER_H_