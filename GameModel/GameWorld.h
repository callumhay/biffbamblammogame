#ifndef __GAMEWORLD_H__
#define __GAMEWORLD_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameEventManager.h"

class GameModel;
class GameLevel;
class GameTransformMgr;

// A set of levels makes up a game world, a game world
// has its own distinct style. The GameWorld class acts as
// a proxy to itself thus it has a state where the world is not
// actually loaded - it can be loaded and unloaded from memory on demand.
class GameWorld {

public:
	enum WorldStyle { None = -1, Deco = 0, Cyberpunk = 1 };

	static bool IsValidWorldStyle(const std::string &s);
	static WorldStyle GetWorldStyleFromString(const std::string &s);

private:
	bool isLoaded;												// Has this world been loaded into memory or not?
	std::string worldFilepath;						// Path to the world defintion file
	std::vector<GameLevel*> loadedLevels;	// Levels loaded into memory
	unsigned int currentLevelNum;					// Current level expressed as an index into loaded levels vector

	std::string name;											// Human-readable name of the world
	WorldStyle style;											// Style of the world loaded (None if no world is loaded)

	GameTransformMgr& transformMgr;

public:
	GameWorld(std::string worldFilepath, GameTransformMgr& transformMgr);
	~GameWorld();

	bool Load();
	bool Unload();
	
	const std::vector<GameLevel*>& GetAllLevelsInWorld() const {
		return this->loadedLevels;
	}

	GameLevel* GetCurrentLevel() {
		assert(this->isLoaded);
		return this->loadedLevels[this->currentLevelNum];
	}
	const GameLevel* GetCurrentLevel() const {
		assert(this->isLoaded);
		return this->loadedLevels[this->currentLevelNum];
	}

	unsigned int GetCurrentLevelNum() const {
		assert(this->isLoaded);
		return this->currentLevelNum;
	}

	WorldStyle GetStyle() const {
		assert(this->isLoaded);
		return this->style;
	}
	const std::string& GetName() const {
		assert(this->isLoaded);
		return this->name;
	}

	void IncrementLevel() {
		assert(this->isLoaded);
		this->SetCurrentLevel(this->currentLevelNum + 1);
		// EVENT: New Level Started
		GameEventManager::Instance()->ActionLevelStarted(*this, *this->GetCurrentLevel());
	}

	void SetCurrentLevel(unsigned int levelNum);

	// Returns whether the current level is the last level in this world.
	bool IsLastLevel() const {
		assert(this->isLoaded);
		return this->currentLevelNum == (this->loadedLevels.size()-1);
	}


};
#endif