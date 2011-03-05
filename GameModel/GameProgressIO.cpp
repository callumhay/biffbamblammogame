/**
 * GameProgressIO.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameProgressIO.h"
#include "GameModel.h"

const char* GameProgressIO::PROGRESS_FILENAME = "bbb_progress.dat";

bool GameProgressIO::LoadGameProgress(GameModel* model) {
    if (model == NULL) {
        assert(false);
        return false;
    }

    const std::vector<GameWorld*>& gameWorlds = model->GetGameWorlds();

    std::ifstream inFile(PROGRESS_FILENAME, std::ifstream::in | std::ifstream::binary);
    bool success = inFile.is_open();

	if (success) {
        // TODO...
        
        
        


    }
    else {

        // Failed to read from the progress file, create a new empty one (no saved progress) and
        // set the state in the game model to be a new game
        std::ofstream outFile(PROGRESS_FILENAME, std::ios::out | std::ios::binary);

        // Number of worlds...
        outFile << gameWorlds.size() << std::endl;

        for (size_t worldIdx = 0; worldIdx < gameWorlds.size(); worldIdx++) {
            const GameWorld* currWorld = gameWorlds.at(worldIdx);
            assert(currWorld != NULL);

            // World index followed by scope
            outFile << std::endl << worldIdx << " {" << std::endl;

            const std::vector<GameLevel*>& worldLevels = currWorld->GetAllLevelsInWorld();
            int lastLevelIdx = currWorld->GetLastLevelIndexPassed();
            
            // Number of levels of progress
            outFile << lastLevelIdx << std::endl;
            for (int levelIdx = 0; levelIdx < lastLevelIdx; levelIdx++) {
                const GameLevel* currLevel = worldLevels.at(levelIdx);
                assert(currLevel != NULL);
                
                // Store each unlocked level's score
                //outFile << levelIdx << " { " << currLevel->GetHighScore() << " }" << std::endl;
            }

            // End world scope
            outFile << "}" << std::endl;
        }
        outFile.close();
    }

    return true;
}

bool GameProgressIO::SaveGameProgress(const GameModel* model) {
    if (model == NULL) {
        assert(false);
        return false;
    }

    
    return true;
}
