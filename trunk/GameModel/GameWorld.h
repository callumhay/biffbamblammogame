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
	enum WorldStyle { None = -1, Deco = 0, Futurism = 1 };

	static bool IsValidWorldStyle(const std::string &s);
	static WorldStyle GetWorldStyleFromString(const std::string &s);

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

	int GetCurrentLevelNum() const {
		assert(this->isLoaded);
		return this->currentLevelNum;
	}
    size_t GetNumLevels() const {
        return this->loadedLevels.size();
    }

    // Zero based index of the world
    int GetWorldIndex() const {
        assert(this->style != None);
        return static_cast<int>(this->style);
    }
    // One based index of the world
    int GetWorldNumber() const {
        return this->GetWorldIndex() + 1;
    }
	WorldStyle GetStyle() const {
		assert(this->isLoaded);
		return this->style;
	}
	const std::string& GetName() const {
		assert(this->isLoaded);
		return this->name;
	}
    const std::string& GetImageFilepath() const {
        assert(this->isLoaded);
        return this->imageFilepath;
    }

	void IncrementLevel(GameModel* model);
	void SetCurrentLevel(GameModel* model, int levelNum);

	// Returns whether the current level is the last level in this world.
	bool IsLastLevel() const {
		assert(this->isLoaded);
		return this->currentLevelNum == (static_cast<int>(this->loadedLevels.size()) - 1);
	}

    // TODO
    //void SetLastLevelPassedIndex(int levelIdx);
    int GetLastLevelIndexPassed() const;

private:
	bool isLoaded;                          // Has this world been loaded into memory or not?
	std::string worldFilepath;              // Path to the world defintion file
	std::vector<GameLevel*> loadedLevels;	// Levels loaded into memory
	int currentLevelNum;                    // Current level expressed as an index into loaded levels vector

    static const int NO_LEVEL_PASSED;
    int lastLevelPassed;                    // Progress indicator - has the number of the last level that was passed by the player,
                                            // Similar to currentLevelNum, this is an index into the loadedLevels vector, if no progress
                                            // has been made in this world yet then it will be equal to NO_LEVEL_PASSED

	std::string name;						// Human-readable name of the world
	WorldStyle style;						// Style of the world loaded (None if no world is loaded)
    std::string imageFilepath;              // The image file path - for the image used to show this world in menus

	GameTransformMgr& transformMgr;

	// Disallow copy and assign
	GameWorld(const GameWorld& w);
	GameWorld& operator=(const GameWorld& w);

};

// Gets the index of the last level that was passed (i.e., progress) of the player in this world
inline int GameWorld::GetLastLevelIndexPassed() const {
    return this->lastLevelPassed;
}

#endif