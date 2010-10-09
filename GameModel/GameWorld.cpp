/**
 * GameWorld.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameWorld.h"
#include "GameLevel.h"
#include "GameTransformMgr.h"

#include "../BlammoEngine/StringHelper.h"

#include "../ResourceManager.h"

/* 
 * Constructor for GameWorld class, requires a list of level text/filenames
 * which will be loaded when they are required.
 */
GameWorld::GameWorld(std::string worldFilepath, GameTransformMgr& transformMgr) : 
worldFilepath(worldFilepath), isLoaded(false), style(None), currentLevelNum(-1),
transformMgr(transformMgr) {
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
bool GameWorld::Load(GameModel* model) {
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

	// Read all the level file names
	std::vector<std::string> levelFileList;
	std::string currLvlFile;
	while (*inFile >> currLvlFile) {
		levelFileList.push_back(currLvlFile);
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
		GameLevel* lvl = GameLevel::CreateGameLevelFromFile(levelFileList[i]);
		if (lvl == NULL) {
			// Clean up and exit on erroroneous level read
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
	this->currentLevelNum = -1;

	return true;
}

/*
 * Figures out if the given string is a world style.
 * Precondition: true.
 * Returns: true if s is a world style string, false otherwise.
 */
bool GameWorld::IsValidWorldStyle(const std::string &s) {
	if (s == "Deco") {
		return true;
	}
	else if (s == "Cyberpunk") {
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
	if (s == "Deco") {
		ret = Deco;
	}
	else if (s == "Cyberpunk") {
		ret = Cyberpunk;
	}
	return ret;
}

/**
 * Set the current level to the one given. This will also establish
 * various other properties required for the level - including making sure the
 * camera is properly transformed to view it and raising an event that the level has
 * been started.
 */
void GameWorld::SetCurrentLevel(GameModel* model, unsigned int levelNum) {
	assert(isLoaded);
	assert(levelNum < this->loadedLevels.size());
	assert(levelNum >= 0);
	this->currentLevelNum = levelNum;

	GameLevel* currentLevel = this->GetCurrentLevel();
	currentLevel->InitAfterLevelLoad(model);

	// Setup the default transforms for the new level
	this->transformMgr.SetupLevelCameraDefaultPosition(*this->GetCurrentLevel());
}