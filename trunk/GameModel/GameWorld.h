#ifndef __GAMEWORLD_H__
#define __GAMEWORLD_H__

#include <vector>
#include <string>

class GameLevel;

// A set of levels makes up a game world, a game world
// has its own distinct style. The GameWorld class acts as
// a proxy to itself thus it has a state where the world is not
// actually loaded - it can be loaded and unloaded from memory on demand.
class GameWorld {

public:
	enum WorldStyle { None = -1, Deco = 0 };

	static bool IsValidWorldStyle(const std::string &s);
	static WorldStyle GetWorldStyleFromString(const std::string &s);

private:
	bool isLoaded;												// Has this world been loaded into memory or not?
	std::string worldFilepath;						// Path to the world defintion file
	std::vector<GameLevel*> loadedLevels;	// Levels loaded into memory
	unsigned int currentLevelNum;					// Current level expressed as an index into loaded levels vector

	WorldStyle style;											// Style of the world loaded (None if no world is loaded)

public:
	GameWorld(std::string worldFilepath);
	~GameWorld();

	bool Load();
	bool Unload();
	
	GameLevel* GetCurrentLevel() {
		return this->loadedLevels[this->currentLevelNum];
	}

	WorldStyle GetStyle() const {
		return this->style;
	}


};
#endif