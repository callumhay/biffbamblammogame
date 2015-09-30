/**
 * GameWorld.cpp
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
const char* GameWorld::POMO_WORLD_NAME              = "Postmodern";
const char* GameWorld::SURREALISM_DADA_WORLD_NAME   = "Surrealism and Dada";

const char* GameWorld::CLASSICAL_WORLD_NAME_SHORT       = "classical";
const char* GameWorld::GOTHIC_ROMANTIC_WORLD_NAME_SHORT = "gothic_romantic";
const char* GameWorld::NOUVEAU_WORLD_NAME_SHORT         = "nouveau";
const char* GameWorld::DECO_WORLD_NAME_SHORT            = "deco";
const char* GameWorld::FUTURISM_WORLD_NAME_SHORT        = "futurism";
const char* GameWorld::POMO_WORLD_NAME_SHORT            = "pomo";
const char* GameWorld::SURREALISM_DADA_WORLD_NAME_SHORT = "surrealism_dada";

/* 
 * Constructor for GameWorld class, requires a list of level text/filenames
 * which will be loaded when they are required.
 */
GameWorld::GameWorld(std::string worldFilepath, GameTransformMgr& transformMgr) : 
worldFilepath(worldFilepath), isLoaded(false), style(None), currentLevelNum(0),
lastLevelPassedIndex(GameWorld::NO_LEVEL_PASSED), transformMgr(transformMgr), 
hasBeenUnlocked(false) {
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
bool GameWorld::Load(GameModel* gameModel) {
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
    std::vector<int> levelUnlockStarAmts;
	std::string currLineStr;

    while (std::getline(*inFile, currLineStr)) {
        currLineStr = stringhelper::trim(currLineStr);
        if (!currLineStr.empty()) {

            if (currLineStr[0] != '#') {
                // Check to see if the level has a milestone star amount associated with it
                std::vector<std::string> tokens;
                stringhelper::Tokenize(currLineStr, tokens, ":");
                
                int numStarsRequiredForUnlock = 0;
                std::string levelFilepathStr = currLineStr;
                if (tokens.size() > 1) {
                    // There's a milestone star amount...
                    std::stringstream numStarsStrStream(stringhelper::trim(tokens.back()));
                    if (!(numStarsStrStream >> numStarsRequiredForUnlock)) {
                        debug_output("ERROR: Failed to read the number of milestone stars for level: " + levelFilepathStr + " in world " + this->worldFilepath);
                        delete inFile;
                        inFile = NULL;
                        return false;
                    }
                    else {
                        levelFilepathStr = stringhelper::trim(tokens.front());
                        if (levelFilepathStr.empty()) {
                            debug_output("ERROR: Failed to read file: " + levelFilepathStr + " from line " + currLineStr + " in world " + this->worldFilepath);
                            delete inFile;
                            inFile = NULL;
                            return false;
                        }
                    }
                }

		        levelFileList.push_back(levelFilepathStr);
                levelUnlockStarAmts.push_back(numStarsRequiredForUnlock);
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
    assert(levelUnlockStarAmts.size() == levelFileList.size());
	for (size_t i = 0; i < levelFileList.size(); i++) {
        GameLevel* lvl = GameLevel::CreateGameLevelFromFile(gameModel, this->GetStyle(), i, levelUnlockStarAmts[i], levelFileList[i]);
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
GameLevel* GameWorld::GetLevelByName(const std::string& name) const {
    for (std::vector<GameLevel*>::const_iterator iter = this->loadedLevels.begin();
         iter != this->loadedLevels.end(); ++iter) {
        GameLevel* level = *iter;
        if (level->GetName().compare(name) == 0) {
            return level;
        }
    }
    return NULL;
}

GameLevel* GameWorld::GetLevelByIndex(int idx) const {
    if (idx < 0 || idx >= static_cast<int>(this->loadedLevels.size())) {
        assert(false);
        return NULL;
    }

    return this->loadedLevels[idx];
}

/*
 * Figures out if the given string is a world style.
 * Precondition: true.
 * Returns: true if s is a world style string, false otherwise.
 */
bool GameWorld::IsValidWorldStyle(const std::string &s) {
    if (s == CLASSICAL_WORLD_NAME || s == CLASSICAL_WORLD_NAME_SHORT || 
        s == GOTHIC_ROMANTIC_WORLD_NAME || s == GOTHIC_ROMANTIC_WORLD_NAME_SHORT ||
        s == NOUVEAU_WORLD_NAME || s == NOUVEAU_WORLD_NAME_SHORT ||
        s == DECO_WORLD_NAME || s == DECO_WORLD_NAME_SHORT ||
        s == FUTURISM_WORLD_NAME || s == FUTURISM_WORLD_NAME_SHORT ||
        s == POMO_WORLD_NAME || s == POMO_WORLD_NAME_SHORT ||
        s == SURREALISM_DADA_WORLD_NAME || s == SURREALISM_DADA_WORLD_NAME_SHORT) {

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
    if (s == CLASSICAL_WORLD_NAME || s == CLASSICAL_WORLD_NAME_SHORT) {
        ret = Classical;
    }
    else if (s == GOTHIC_ROMANTIC_WORLD_NAME || s == GOTHIC_ROMANTIC_WORLD_NAME_SHORT) {
        ret = GothicRomantic;
    }
    else if (s == NOUVEAU_WORLD_NAME || s == NOUVEAU_WORLD_NAME_SHORT) {
        ret = Nouveau;
    }
    else if (s == DECO_WORLD_NAME || s == DECO_WORLD_NAME_SHORT) {
		ret = Deco;
	}
	else if (s == FUTURISM_WORLD_NAME || s == FUTURISM_WORLD_NAME_SHORT) {
		ret = Futurism;
	}
    else if (s == POMO_WORLD_NAME || s == POMO_WORLD_NAME_SHORT) {
        assert(false);
        // TODO
    }
    else if (s == SURREALISM_DADA_WORLD_NAME || s == SURREALISM_DADA_WORLD_NAME_SHORT) {
        ret = SurrealismDada;
    }
	return ret;
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

void GameWorld::ClearProgress(bool arcadeMode) {
    this->SetHasBeenUnlocked(false);
    
    for (int i = 0; i < static_cast<int>(this->loadedLevels.size()); i++) {
        GameLevel* level = this->loadedLevels[i];
        level->ClearProgress(arcadeMode);
    }
    this->lastLevelPassedIndex = NO_LEVEL_PASSED;
}