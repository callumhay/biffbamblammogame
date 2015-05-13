/**
 * GameWorld.h
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
    static const int NO_LEVEL_PASSED;

    static const char* CLASSICAL_WORLD_NAME;
    static const char* GOTHIC_ROMANTIC_WORLD_NAME;
    static const char* NOUVEAU_WORLD_NAME;
    static const char* DECO_WORLD_NAME;
    static const char* FUTURISM_WORLD_NAME;
    static const char* POMO_WORLD_NAME;
    static const char* SURREALISM_DADA_WORLD_NAME;

    static const char* CLASSICAL_WORLD_NAME_SHORT;
    static const char* GOTHIC_ROMANTIC_WORLD_NAME_SHORT;
    static const char* NOUVEAU_WORLD_NAME_SHORT;
    static const char* DECO_WORLD_NAME_SHORT;
    static const char* FUTURISM_WORLD_NAME_SHORT;
    static const char* POMO_WORLD_NAME_SHORT;
    static const char* SURREALISM_DADA_WORLD_NAME_SHORT;

	enum WorldStyle { None = -1, Classical = 0, GothicRomantic = 1, Nouveau = 2, Deco = 3, Futurism = 4,
        SurrealismDada = 69};

	static bool IsValidWorldStyle(const std::string &s);
	static WorldStyle GetWorldStyleFromString(const std::string &s);

	GameWorld(std::string worldFilepath, GameTransformMgr& transformMgr);
	~GameWorld();

	bool Load(GameModel* gameModel);
	bool Unload();
	
	const std::vector<GameLevel*>& GetAllLevelsInWorld() const {
		return this->loadedLevels;
	}

    int GetNumStarsCollectedInWorld() const;
    int GetTotalAchievableStarsInWorld() const;

	GameLevel* GetCurrentLevel() {
		assert(this->isLoaded);
		return this->loadedLevels[this->currentLevelNum];
	}
	const GameLevel* GetCurrentLevel() const {
		assert(this->isLoaded);
		return this->loadedLevels[this->currentLevelNum];
	}
    GameLevel* GetLevelByName(const std::string& name) const;
    GameLevel* GetLevelByIndex(int idx) const;

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
    const std::string& GetWorldFilepath() const {
        return this->worldFilepath;
    }

	void SetCurrentLevel(GameModel* model, int levelNum);

	// Returns whether the current level is the last level in this world.
	bool IsLastLevel() const {
		assert(this->isLoaded);
		return this->currentLevelNum == (static_cast<int>(this->loadedLevels.size()) - 1);
	}

    void UpdateLastLevelPassedIndex();
    int GetLastLevelIndexPassed() const;
    int GetLastLevelIndex() const;

    bool GetHasBeenUnlocked() const;
    void SetHasBeenUnlocked(bool unlocked);

private:
	bool isLoaded;                          // Has this world been loaded into memory or not?
	std::string worldFilepath;              // Path to the world defintion file
	std::vector<GameLevel*> loadedLevels;	// Levels loaded into memory
	int currentLevelNum;                    // Current level expressed as an index into loaded levels vector

    int lastLevelPassedIndex;  // Progress indicator - has the number of the last level that was passed by the player,
                               // Similar to currentLevelNum, this is an index into the loadedLevels vector, if no progress
                               // has been made in this world yet then it will be equal to NO_LEVEL_PASSED
    
    bool hasBeenUnlocked;  // Progress indicator for the world as a whole -- if the boss in the world prior to this one
                           // has been defeated then the world has been unlocked

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
    return this->lastLevelPassedIndex;
}
// Gets the index of the last level in this world
inline int GameWorld::GetLastLevelIndex() const {
    return static_cast<int>(this->loadedLevels.size()) - 1;
}

inline bool GameWorld::GetHasBeenUnlocked() const {
    return this->hasBeenUnlocked;
}
inline void GameWorld::SetHasBeenUnlocked(bool unlocked) {
    this->hasBeenUnlocked = unlocked;
}

#endif