/**
 * GameProgressIO.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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

#include "GameProgressIO.h"
#include "GameModel.h"

#include "../ResourceManager.h"

const char* GameProgressIO::PROGRESS_FILENAME = "bbb_progress.dat";

bool GameProgressIO::LoadGameProgress(GameModel* model) {
#define ON_ERROR_CLEAN_UP_AND_EXIT(condition) if (!(condition)) { debug_output("ERROR WHILE LOADING GAME PROGRESS!"); inFile.close(); return false; }

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
        bool isUnlocked;
        int numWorlds;
        long highScore;
        bool starCostPaidFor;

        ON_ERROR_CLEAN_UP_AND_EXIT(inFile >> numWorlds);
        std::getline(inFile, nameStr);  // finish reading the line
        
        for (int worldIdx = 0; worldIdx < numWorlds; worldIdx++) {

            // Read the world name
            ON_ERROR_CLEAN_UP_AND_EXIT(std::getline(inFile, nameStr));
            
            // Look up which world we're dealing with based on the read name
            GameWorld* currWorld = model->GetWorldByName(nameStr);
            if (currWorld == NULL) {
                worldIdx = std::max<int>(0, worldIdx-1);
                continue;
            }
            
            // Read whether the world has been unlocked or not
            ON_ERROR_CLEAN_UP_AND_EXIT(inFile >> isUnlocked);
            currWorld->SetHasBeenUnlocked(isUnlocked);

            int numLevels;
            ON_ERROR_CLEAN_UP_AND_EXIT(inFile >> numLevels);
            std::getline(inFile, nameStr);  // finish reading the line
            
            for (int levelIdx = 0; levelIdx < numLevels; levelIdx++) {
                // Read the level name
                ON_ERROR_CLEAN_UP_AND_EXIT(std::getline(inFile, nameStr));
                // Look up the level we're dealing with based on the read name
                GameLevel* currLevel = currWorld->GetLevelByName(nameStr);
                if (currLevel == NULL) {
                    
                    levelIdx = std::max<int>(0, levelIdx-1);
                    continue;
                }

                // Read the high-score for the level and set it
                ON_ERROR_CLEAN_UP_AND_EXIT(inFile >> highScore);
                std::getline(inFile, nameStr);  // finish reading the line
                currLevel->SetHighScore(highScore, true);

                // A flag is stored to indicate whether the unlock star cost of the level has
                // been paid for (for levels that don't have a cost this will always be true)
                ON_ERROR_CLEAN_UP_AND_EXIT(inFile >> starCostPaidFor);
                currLevel->SetAreUnlockStarsPaidFor(starCostPaidFor);
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

        // World and whether it's been unlocked
        outFile << currWorld->GetName() << std::endl;
        outFile << currWorld->GetHasBeenUnlocked() << std::endl;

        // World's levels...
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
            outFile << currLevel->GetAreUnlockStarsPaidFor() << std::endl;
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
        outFile << true << std::endl; // (The world has been unlocked)

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
            outFile << true << std::endl; // Star cost to unlock is paid for
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
        outFile << true << std::endl; // (The world has been unlocked)

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
                // Beat all levels but the boss
                if (currLevel->GetHasBoss()) {
                    outFile << currLevel->GetHighScore() << std::endl;
                }
                else {
                    outFile << std::max<long>(1, currLevel->GetHighScore()) << std::endl;
                }
                outFile << true << std::endl; // star cost is paid for
            }
            else {
                outFile << currLevel->GetHighScore() << std::endl;
                outFile << currLevel->GetAreUnlockStarsPaidFor() << std::endl;
            }

        }
    }
    outFile.close();
 
    return true;
}

#endif // _DEBUG