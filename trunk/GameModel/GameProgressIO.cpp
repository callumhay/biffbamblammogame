/**
 * GameProgressIO.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "GameProgressIO.h"
#include "GameModel.h"

#include "../ResourceManager.h"

const char* GameProgressIO::PROGRESS_FILENAME = "bbb_progress.dat";

bool GameProgressIO::LoadGameProgress(GameModel* model) {
#define ON_ERROR_CLEAN_UP_AND_EXIT(condition) if (!(condition)) { inFile.close(); return false; }

    if (model == NULL) {
        assert(false);
        return false;
    }

    std::ifstream inFile((ResourceManager::GetLoadDir() + std::string(PROGRESS_FILENAME)).c_str(),
        std::ifstream::in | std::ifstream::binary);
    bool success = inFile.is_open();

	if (success) {
        // Read in the progress file and set all the values across the model
        std::string nameStr;
        int numWorlds;
        long highScore;
        ON_ERROR_CLEAN_UP_AND_EXIT(inFile >> numWorlds);
        std::getline(inFile, nameStr);  // finish reading the line
        
        for (int worldIdx = 0; worldIdx < numWorlds; worldIdx++) {

            // Read the world name
            ON_ERROR_CLEAN_UP_AND_EXIT(std::getline(inFile, nameStr));
            
            // Look up which world we're dealing with based on the read name
            GameWorld* currWorld = model->GetWorldByName(nameStr);
            if (currWorld == NULL) {
                continue;
            }

            int numLevels;
            ON_ERROR_CLEAN_UP_AND_EXIT(inFile >> numLevels);
            std::getline(inFile, nameStr);  // finish reading the line
            
            for (int levelIdx = 0; levelIdx < numLevels; levelIdx++) {
                // Read the level name
                ON_ERROR_CLEAN_UP_AND_EXIT(std::getline(inFile, nameStr));
                // Look up the level we're dealing with based on the read name
                GameLevel* currLevel = currWorld->GetLevelByName(nameStr);
                if (currLevel == NULL) {
                    continue;
                }

                // Read the high-score for the level and set it
                ON_ERROR_CLEAN_UP_AND_EXIT(inFile >> highScore);
                std::getline(inFile, nameStr);  // finish reading the line
                currLevel->SetHighScore(highScore, true);
            }

            currWorld->UpdateLastLevelPassedIndex();
        }
        
        inFile.close();
    }
    else {
        // Failed to read from the progress file, create a new one
        return GameProgressIO::SaveGameProgress(model);
    }

    return true;

#undef ON_ERROR_CLEAN_UP_AND_EXIT
}

bool GameProgressIO::SaveGameProgress(const GameModel* model) {
    if (model == NULL) {
        assert(false);
        return false;
    }

    std::ofstream outFile((ResourceManager::GetLoadDir() + std::string(PROGRESS_FILENAME)).c_str(), std::ios::out | std::ios::binary);
    if (!outFile.is_open()) {
        return false;
    }

    // Number of worlds...
    const std::vector<GameWorld*>& gameWorlds = model->GetGameWorlds();
    outFile << gameWorlds.size() << std::endl;

    for (size_t worldIdx = 0; worldIdx < gameWorlds.size(); worldIdx++) {
        const GameWorld* currWorld = gameWorlds.at(worldIdx);
        assert(currWorld != NULL);

        // World index followed by scope
        outFile << currWorld->GetName() << std::endl;

        const std::vector<GameLevel*>& worldLevels = currWorld->GetAllLevelsInWorld();
        size_t numLevels = currWorld->GetNumLevels();
        
        // Number of levels of progress
        outFile << numLevels << std::endl;
        for (size_t levelIdx = 0; levelIdx < numLevels; levelIdx++) {
            const GameLevel* currLevel = worldLevels.at(levelIdx);
            assert(currLevel != NULL);
            
            // Store each unlocked level's high-score
            outFile << currLevel->GetName() << std::endl;
            outFile << currLevel->GetHighScore() << std::endl;
        }
    }
    outFile.close();
 
    return true;
}

bool GameProgressIO::WipeoutGameProgress(GameModel* model) {
    std::ofstream outFile((ResourceManager::GetLoadDir() + std::string(PROGRESS_FILENAME)).c_str(), std::ios::out | std::ios::binary);
    if (!outFile.is_open()) {
        return false;
    }

    outFile << 0;
    outFile.close();

    model->ClearAllGameProgress();

    return true;
}

#ifdef _DEBUG

bool GameProgressIO::SaveFullProgressOfGame(const GameModel* model) {
    if (model == NULL) {
        assert(false);
        return false;
    }

    std::ofstream outFile((ResourceManager::GetLoadDir() + std::string(PROGRESS_FILENAME)).c_str(), std::ios::out | std::ios::binary);
    if (!outFile.is_open()) {
        return false;
    }

    // Number of worlds...
    const std::vector<GameWorld*>& gameWorlds = model->GetGameWorlds();
    outFile << gameWorlds.size() << std::endl;

    for (size_t worldIdx = 0; worldIdx < gameWorlds.size(); worldIdx++) {
        const GameWorld* currWorld = gameWorlds.at(worldIdx);
        assert(currWorld != NULL);

        // World index followed by scope
        outFile << currWorld->GetName() << std::endl;

        const std::vector<GameLevel*>& worldLevels = currWorld->GetAllLevelsInWorld();
        size_t numLevels = currWorld->GetNumLevels();
        
        // Number of levels of progress
        outFile << numLevels << std::endl;
        for (size_t levelIdx = 0; levelIdx < numLevels; levelIdx++) {
            const GameLevel* currLevel = worldLevels.at(levelIdx);
            assert(currLevel != NULL);
            
            // Store each unlocked level's high-score
            outFile << currLevel->GetName() << std::endl;
            outFile << std::max<long>(1, currLevel->GetHighScore()) << std::endl;
        }
    }
    outFile.close();
 
    return true;
}

bool GameProgressIO::SaveFullProgressOfWorld(const GameModel* model, const GameWorld::WorldStyle& worldStyle) {
    if (model == NULL) {
        assert(false);
        return false;
    }

    std::ofstream outFile((ResourceManager::GetLoadDir() + std::string(PROGRESS_FILENAME)).c_str(), std::ios::out | std::ios::binary);
    if (!outFile.is_open()) {
        return false;
    }

    // Number of worlds...
    const std::vector<GameWorld*>& gameWorlds = model->GetGameWorlds();
    outFile << gameWorlds.size() << std::endl;

    for (size_t worldIdx = 0; worldIdx < gameWorlds.size(); worldIdx++) {
        const GameWorld* currWorld = gameWorlds.at(worldIdx);
        assert(currWorld != NULL);

        // World index followed by scope
        outFile << currWorld->GetName() << std::endl;

        const std::vector<GameLevel*>& worldLevels = currWorld->GetAllLevelsInWorld();
        size_t numLevels = currWorld->GetNumLevels();
        
        // Number of levels of progress
        outFile << numLevels << std::endl;
        for (size_t levelIdx = 0; levelIdx < numLevels; levelIdx++) {
            const GameLevel* currLevel = worldLevels.at(levelIdx);
            assert(currLevel != NULL);
            
            // Store each unlocked level's high-score
            outFile << currLevel->GetName() << std::endl;
            if (currWorld->GetStyle() == worldStyle) {
                outFile << std::max<long>(1, currLevel->GetHighScore()) << std::endl;
            }
            else {
                outFile << currLevel->GetHighScore() << std::endl;
            }
        }
    }
    outFile.close();
 
    return true;
}

#endif // _DEBUG