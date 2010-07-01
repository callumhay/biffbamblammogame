#ifndef _MSF_READER_H_
#define _MSF_READER_H_

#include "../BlammoEngine/BasicIncludes.h"

class Sound;

class MSFReader {

public:
	static bool ReadMSF(const std::string& filepath, std::map<int, Sound*>& sounds);

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
	static const char* IGNORE_LINE;

	// Keyword parameters for the MSF definition blocks
	static const char* FILE_KEYWORD;
	static const char* LOOPS_KEYWORD;
	static const char* FADE_IN_KEYWORD;
	static const char* FADE_OUT_KEYWORD;

	// Sound string identifiers
	
	// Main menu sound identifiers
	static const char* MAIN_MENU_BG_MUSIC;
	static const char* MAIN_MENU_BG_BANG_SMALL_EVENT;
	static const char* MAIN_MENU_BG_BANG_MEDIUM_EVENT;
	static const char* MAIN_MENU_BG_BANG_BIG_EVENT;
	static const char* MAIN_MENU_ITEM_HIGHLIGHTED_EVENT;
	static const char* MAIN_MENU_ITEM_ENTERED_EVENT;
	static const char* MAIN_MENU_ITEM_BACK_AND_CANCEL_EVENT;
	static const char* MAIN_MENU_ITEM_VERIFY_AND_SEL_EVENT;
	static const char* MAIN_MENU_ITEM_SCROLLED_EVENT;
	
	// World sound identifiers
	static const char* WORLD_BG_MUSIC;
	static const char* WORLD_PADDLE_HIT_WALL_EVENT;
	static const char* WORLD_BALL_LOST_EVENT;
	static const char* WORLD_LAST_BALL_EXPLODED_EVENT;
	static const char* WORLD_BALL_SPAWN_EVENT;
	static const char* WORLD_BALL_BLOCK_COLLISION_EVENT;
	static const char* WORLD_BALL_PADDLE_COLLISION_EVENT;
	static const char* WORLD_STICKY_BALL_PADDLE_COLLISION_EVENT;
	static const char* WORLD_BALL_BALL_COLLISION_EVENT;
	static const char* WORLD_BOMB_BLOCK_DESTROYED_EVENT;
	static const char* WORLD_INK_BLOCK_DESTROYED_EVENT;
	static const char* WORLD_BASIC_BLOCK_DESTROYED_EVENT;
	static const char* WORLD_COLLATERAL_BLOCK_DESTROYED_EVENT;
	static const char* WORLD_CANNON_BLOCK_LOADED_EVENT;
	static const char* WORLD_PORTAL_TELEPORT_EVENT;
	static const char* WORLD_SAFETY_NET_CREATED_EVENT;
	static const char* WORLD_SAFETY_NET_DESTROYED_EVENT;
	static const char* WORLD_ITEM_SPAWNED_EVENT;
	static const char* WORLD_POWER_UP_ACTIVATED_EVENT;
	static const char* WORLD_POWER_NEUTRAL_ACTIVATED_EVENT;
	static const char* WORLD_POWER_DOWN_ACTIVATED_EVENT;
	static const char* WORLD_POWER_UP_TIMER_END_EVENT;
	static const char* WORLD_POWER_NEUTRAL_TIMER_END_EVENT;
	static const char* WORLD_POWER_DOWN_TIMER_END_EVENT;
	static const char* WORLD_BALL_OR_PADDLE_GROW_EVENT;
	static const char* WORLD_BALL_OR_PADDLE_SHRINK_EVENT;
	static const char* WORLD_LASER_BULLET_SHOT_EVENT;
	static const char* WORLD_LEVEL_COMPLETED_EVENT;
	static const char* WORLD_WORLD_COMPLETED_EVENT;
	static const char* WORLD_INK_SPLATTER_MASK;
	static const char* WORLD_POISON_SICK_MASK;
	static const char* WORLD_LASER_BULLET_MOVING_MASK;
	static const char* WORLD_ROCKET_MOVING_MASK;
	static const char* WORLD_LASER_BEAM_FIRING_MASK;
	static const char* WORLD_COLLATERAL_BLOCK_FLASHING_MASK;
	static const char* WORLD_COLLATERAL_BLOCK_FALLING_MASK;
	static const char* WORLD_CANNON_BLOCK_ROTATING_MASK;


	// Initializer/Default values
	static const int INVALID_SOUND_TYPE = -1;

	static int ConvertKeywordToSoundType(const std::string& soundName);
	static bool FoundEqualsSyntax(bool& noEquals, std::string& errorStr, std::istream* inStream);

};

#endif // _MSF_READER_H_