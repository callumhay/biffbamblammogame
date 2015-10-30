#include "ArcadeLeaderboard.h"
#include "GameModel.h"
#include "GameLevel.h"

const char* ArcadeLeaderboard::FILEPATH = "leaderboard.txt";
const int ArcadeLeaderboard::NUM_ENTRIES = 5;


static bool LeaderboardEntryGreaterThan(const LeaderboardEntry& left, const LeaderboardEntry& right) {
    return left.score > right.score;
}


bool LeaderboardEntry::ReadFromFile(std::ifstream& inFile, LeaderboardEntry& entry) {
#define READ_IN_FILE_FAIL(inFile, valueToRead) if (!(inFile >> valueToRead)) { return false; }
    READ_IN_FILE_FAIL(inFile, entry.playerName);
    READ_IN_FILE_FAIL(inFile, entry.score);
#undef READ_IN_FILE_FAIL

    return true;
}

bool LeaderboardEntry::WriteToFile(std::ofstream& outFile) const {
#define WRITE_OUT_FILE_FAIL(outFile, valueToWrite) if (!(outFile << valueToWrite)) { try { outFile.close(); } catch(...) {} return false; }
    WRITE_OUT_FILE_FAIL(outFile, this->playerName << " ");
    WRITE_OUT_FILE_FAIL(outFile, this->score << std::endl);
#undef WRITE_OUT_FILE_FAIL

    return true;
}

ArcadeLeaderboard::ArcadeLeaderboard(const GameModel& model) {
    
    // Default entry scores are based on star scores in the game model's first set of levels...
    const GameWorld* firstWorld = model.GetWorldByIndex(0);
    int numLevels = firstWorld->GetNumLevels();
    std::vector<uint32_t> defaultTopScores(NUM_ENTRIES, 0);
    uint32_t topScore = 0;
    for (int i = 0; i < numLevels; i++) {
        const GameLevel* level = firstWorld->GetLevelByIndex(i);
        if (level->GetHasBoss()) {
            topScore += GameWorld::WorldBossArcadeScore(firstWorld->GetStyle());
        }
        else {
            topScore += level->GetScoreForNumStars(4);
        }
    }
    double divider = 1.0;
    for (int i = 0; i < NUM_ENTRIES; i++) {
        defaultTopScores[i] = static_cast<uint32_t>(topScore / divider);
        defaultTopScores[i] = static_cast<uint32_t>(defaultTopScores[i] / 100) * 100;  // Round each score to the nearest 100
        divider += 0.2;
    }

    // Populate the leaderboard with the default player set...
    this->entries.reserve(ArcadeLeaderboard::NUM_ENTRIES);

    this->entries.push_back(LeaderboardEntry("BIF", defaultTopScores[0]));
    this->entries.push_back(LeaderboardEntry("BAM", defaultTopScores[1]));
    this->entries.push_back(LeaderboardEntry("BLM", defaultTopScores[2]));
    this->entries.push_back(LeaderboardEntry("BIP", defaultTopScores[3]));
    this->entries.push_back(LeaderboardEntry("BOP", defaultTopScores[4]));
}

ArcadeLeaderboard::~ArcadeLeaderboard() {
    this->entries.clear();
}

ArcadeLeaderboard* ArcadeLeaderboard::ReadFromFile(const GameModel& model) {
    // Open the file off disk
    std::ifstream inFile;
    inFile.open((ResourceManager::GetLoadDir() + std::string(ArcadeLeaderboard::FILEPATH)).c_str());
    if (!inFile.is_open()) {
        // Couldn't find or open the file
        inFile.close();
        return NULL;
    }

    std::vector<LeaderboardEntry> entries;
    LeaderboardEntry entry;
    while (LeaderboardEntry::ReadFromFile(inFile, entry)) {
        entries.push_back(entry);
    }
    try {
        inFile.close();
    }
    catch(...) {}

    if (entries.size() != NUM_ENTRIES) {
        return NULL;
    }

    // Now read the level highscores...
    std::map<std::pair<int,int>, long> levelHighscores;
    std::pair<int,int> worldLevelIdxPair(0,0);
    long levelHighscore = 0;
    while (ArcadeLeaderboard::ReadLevelHighscoreFromFile(inFile, worldLevelIdxPair, levelHighscore)) {
        levelHighscores.insert(std::make_pair(worldLevelIdxPair, levelHighscore));
    }


    // Sort the entries from highest to lowest score
    std::sort(entries.begin(), entries.end(), LeaderboardEntryGreaterThan);

    ArcadeLeaderboard* leaderboard = new ArcadeLeaderboard(model);
    leaderboard->entries = entries;
    leaderboard->levelHighscores = levelHighscores;

    return leaderboard;
}

bool ArcadeLeaderboard::WriteToFile() const {

    // Open or create the file for writing
    std::ofstream outFile;
    outFile.open((ResourceManager::GetLoadDir() + std::string(ArcadeLeaderboard::FILEPATH)).c_str());
    if (!outFile.is_open()) {
        // Couldn't open for writing...
        outFile.close();
        return false;
    }

    bool success = true;
    // Write the leaderboard entries to file
    for (int i = 0; i < static_cast<int>(this->entries.size()); i++) {
        if (!this->entries[i].WriteToFile(outFile)) {
            success = false;
            break;
        }
    }
    // Write the level highscores to file
    for (std::map<std::pair<int,int>, long>::const_iterator iter = this->levelHighscores.begin(); iter != this->levelHighscores.end(); ++iter) {
        if (!ArcadeLeaderboard::WriteLevelHighscoreToFile(outFile, iter->first, iter->second)) {
            success = false;
            break;
        }
    }

    return success;
}

bool ArcadeLeaderboard::ReadLevelHighscoreFromFile(std::ifstream& inFile, std::pair<int,int>& worldLevelIdxPair, long& levelHighscore) {
#define READ_IN_FILE_FAIL(inFile, valueToRead) if (!(inFile >> valueToRead)) { return false; }
    READ_IN_FILE_FAIL(inFile, worldLevelIdxPair.first);
    READ_IN_FILE_FAIL(inFile, worldLevelIdxPair.second);
    READ_IN_FILE_FAIL(inFile, levelHighscore);
#undef READ_IN_FILE_FAIL

    return true;
}

bool ArcadeLeaderboard::WriteLevelHighscoreToFile(std::ofstream& outFile, const std::pair<int,int>& worldLevelIdxPair, long levelHighscore) {
#define WRITE_OUT_FILE_FAIL(outFile, valueToWrite) if (!(outFile << valueToWrite)) { try { outFile.close(); } catch(...) {} return false; }
    WRITE_OUT_FILE_FAIL(outFile, worldLevelIdxPair.first << " ");
    WRITE_OUT_FILE_FAIL(outFile, worldLevelIdxPair.second << " ");
    WRITE_OUT_FILE_FAIL(outFile, levelHighscore << std::endl);
#undef WRITE_OUT_FILE_FAIL

    return true;
}