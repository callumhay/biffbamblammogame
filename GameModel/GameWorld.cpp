/**
 * GameWorld.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "GameWorld.h"
#include "GameLevel.h"
#include "GameTransformMgr.h"

#include "../BlammoEngine/StringHelper.h"

#include "../ResourceManager.h"

const int GameWorld::NO_LEVEL_PASSED = -1;

const char* GameWorld::CLASSICAL_WORLD_NAME         = "Classical";
const char* GameWorld::GOTHIC_ROMANTIC_WORLD_NAME   = "Gothic and Romantic";
const char* GameWorld::NOUVEAU_WORLD_NAME           = "Nouveau";
const char* GameWorld::DECO_WORLD_NAME              = "Deco";
const char* GameWorld::FUTURISM_WORLD_NAME          = "Futurism";

/* 
 * Constructor for GameWorld class, requires a list of level text/filenames
 * which will be loaded when they are required.
 */
GameWorld::GameWorld(std::string worldFilepath, GameTransformMgr& transformMgr) : 
worldFilepath(worldFilepath), isLoaded(false), style(None), currentLevelNum(0),
lastLevelPassedIndex(GameWorld::NO_LEVEL_PASSED), transformMgr(transformMgr), hasBeenUnlocked(false) {
}

GameWorld::~GameWorld() {
	this->Unload();
}

/*
 * Load this gameworld into memory. This may be unloaded at
 * anytime using the Unload function.
 * Precondition: true.
 * Return: true on a successful load, false otherwise.
 */
bool GameWorld::Load() {
	if (this->isLoaded) {
		this->Unload();
	}

	std::istringstream* inFile = ResourceManager::GetInstance()->FilepathToInStream(this->worldFilepath);
	if (inFile == NULL) {
		assert(false);
		this->Unload();
		return false;
	}

	// Figure out the world style
	std::string worldStyleStr("");
	while (worldStyleStr.empty()) {
		if (!std::getline(*inFile, worldStyleStr)) {
			debug_output("ERROR: Could not find world style in world file: " << this->worldFilepath);
			delete inFile;
			inFile = NULL;
			return false;		
		}
		worldStyleStr = stringhelper::trim(worldStyleStr);
	}

	this->style = GameWorld::GetWorldStyleFromString(worldStyleStr);
	if (this->style == None) {
		debug_output("ERROR: 'None' is not a valid world style in world file: " << this->worldFilepath);
		delete inFile;
		inFile = NULL;
		return false;	
	}

	// Read the name of the world
	this->name.clear();
	while (this->name.empty()) {
		if (!std::getline(*inFile, this->name)) {
			debug_output("ERROR: Could not find world name in world file: " << this->worldFilepath);
			delete inFile;
			inFile = NULL;
			return false;		
		}
		this->name = stringhelper::trim(this->name);
	}

    // Read the image file path for the world
    this->imageFilepath.clear();
	while (this->imageFilepath.empty()) {
		if (!std::getline(*inFile, this->imageFilepath)) {
			debug_output("ERROR: Could not find world image filepath in world file: " << this->worldFilepath);
			delete inFile;
			inFile = NULL;
			return false;		
		}
		this->imageFilepath = stringhelper::trim(this->imageFilepath);
	}

	// Read all the level file names
	std::vector<std::string> levelFileList;
	std::string currLvlFile;
    while (std::getline(*inFile, currLvlFile)) {
        currLvlFile = stringhelper::trim(currLvlFile);
        if (!currLvlFile.empty()) {
            if (currLvlFile[0] != '#') {
		        levelFileList.push_back(currLvlFile);
            }
        }
	}

	if (levelFileList.size() == 0) {
		debug_output("ERROR: There must be at least one level defined in world file: " << this->worldFilepath);
		delete inFile;
		inFile = NULL;
		return false;			
	}

	delete inFile;
	inFile = NULL;

	// Load each of the levels
	for (size_t i = 0; i < levelFileList.size(); i++) {
        GameLevel* lvl = GameLevel::CreateGameLevelFromFile(this->GetStyle(), i, levelFileList[i]);
		if (lvl == NULL) {
			// Clean up and exit on erroneous level read
			this->Unload();
			debug_output("ERROR: Level file: " << levelFileList[i] << " could not be read properly from world file: " << this->worldFilepath); 
			return false;				
		}
		this->loadedLevels.push_back(lvl);
	}

	this->isLoaded = true;

	return true;
}

/*
 * Unload the current game world from memory. It may be
 * reloaded at anytime using Load().
 * Precondition: true.
 * Returns: true.
 */
bool GameWorld::Unload() {
	for (size_t i = 0; i < this->loadedLevels.size(); i++) {
		delete this->loadedLevels[i];
		this->loadedLevels[i] = NULL;
	}
	this->loadedLevels.clear();
	this->isLoaded = false;
	this->style = None;
	this->currentLevelNum = 0;

	return true;
}

int GameWorld::GetNumStarsCollectedInWorld() const {
    int numStarsCollected = 0;
    for (std::vector<GameLevel*>::const_iterator iter = this->loadedLevels.begin();
         iter != this->loadedLevels.end(); ++iter) {

        GameLevel* level = *iter;
        if (level->GetHasBoss()) {
            continue;
        }
        if (level->GetIsLevelPassedWithScore()) {
            numStarsCollected += level->GetHighScoreNumStars();
        }
    }
    
    return numStarsCollected;
}

int GameWorld::GetTotalAchievableStarsInWorld() const {
    int totalStars = 0;
    for (std::vector<GameLevel*>::const_iterator iter = this->loadedLevels.begin();
         iter != this->loadedLevels.end(); ++iter) {

        GameLevel* level = *iter;
        if (level->GetHasBoss()) {
            continue;
        }
        totalStars += GameLevel::MAX_STARS_PER_LEVEL;
    }

    return totalStars;
}

// Get the level with the given name, NULL if no such level exists
GameLevel* GameWorld::GetLevelByName(const std::string& name) {
    for (std::vector<GameLevel*>::const_iterator iter = this->loadedLevels.begin();
         iter != this->loadedLevels.end(); ++iter) {
        GameLevel* level = *iter;
        if (level->GetName().compare(name) == 0) {
            return level;
        }
    }
    return NULL;
}

/*
 * Figures out if the given string is a world style.
 * Precondition: true.
 * Returns: true if s is a world style string, false otherwise.
 */
bool GameWorld::IsValidWorldStyle(const std::string &s) {
    if (s == CLASSICAL_WORLD_NAME) {
        return true;
    }
    else if (s == GOTHIC_ROMANTIC_WORLD_NAME) {
        return true;
    }
    else if (s == NOUVEAU_WORLD_NAME) {
        return true;
    }
    else if (s == DECO_WORLD_NAME) {
		return true;
	}
	else if (s == FUTURISM_WORLD_NAME) {
		return true;
	}
	else {
		return false;
	}
}

/*
 * Figures out the world style from a given string, used when reading world files.
 * Precondition: true.
 * Returns: The world style if one matches, if no match, returns None.
 */
GameWorld::WorldStyle GameWorld::GetWorldStyleFromString(const std::string &s) {
	GameWorld::WorldStyle ret = None;
    if (s == CLASSICAL_WORLD_NAME) {
        ret = Classical;
    }
    else if (s == GOTHIC_ROMANTIC_WORLD_NAME) {
        ret = GothicRomantic;
    }
    else if (s == NOUVEAU_WORLD_NAME) {
        ret = Nouveau;
    }
    else if (s == DECO_WORLD_NAME) {
		ret = Deco;
	}
	else if (s == FUTURISM_WORLD_NAME) {
		ret = Futurism;
	}
	return ret;
}

/** 
 * Increments the current level in this world - this should be called whenever the
 * player progresses to the next level of the game.
 */
void GameWorld::IncrementLevel(GameModel* model) {
	assert(this->isLoaded);
	this->SetCurrentLevel(model, this->currentLevelNum + 1);

    // Make sure we keep track of the greatest level number that was
    // passed by the player (so we can save their progress later)
    if (this->currentLevelNum > 0) {
        this->lastLevelPassedIndex = std::max<int>(this->lastLevelPassedIndex, this->currentLevelNum-1);
    }
    else if (this->currentLevelNum == 0) {
        this->lastLevelPassedIndex = GameWorld::NO_LEVEL_PASSED;
    }
    else {
        assert(false);
    }

	// EVENT: New Level Started
	GameEventManager::Instance()->ActionLevelStarted(*this, *this->GetCurrentLevel());
}

/**
 * Set the current level to the one given. This will also establish
 * various other properties required for the level - including making sure the
 * camera is properly transformed to view it and raising an event that the level has
 * been started.
 */
void GameWorld::SetCurrentLevel(GameModel* model, int levelNum) {
	assert(isLoaded);
	assert(levelNum < static_cast<int>(this->loadedLevels.size()));
	assert(levelNum >= 0);

	this->currentLevelNum = levelNum;
    
	GameLevel* currentLevel = this->GetCurrentLevel();
	currentLevel->InitAfterLevelLoad(model);

	// Setup the default transforms for the new level
	this->transformMgr.SetupLevelCameraDefaultPosition(*this->GetCurrentLevel());
}

void GameWorld::UpdateLastLevelPassedIndex() {
    for (int i = 0; i < static_cast<int>(this->loadedLevels.size()); i++) {
        const GameLevel* level = this->loadedLevels[i];

        if (!level->GetIsLevelPassedWithScore()) {
            this->lastLevelPassedIndex = i - 1;
            break;
        }
        else if (i == this->GetLastLevelIndex()) {
            this->lastLevelPassedIndex = this->GetLastLevelIndex();
            break;
        }
    }

    if (this->lastLevelPassedIndex < 0) {
        this->lastLevelPassedIndex = NO_LEVEL_PASSED;
    }
}