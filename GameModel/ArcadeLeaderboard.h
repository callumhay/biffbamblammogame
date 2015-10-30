#ifndef __ARCADELEADERBOARD_H__
#define __ARCADELEADERBOARD_H__

#include "../ResourceManager.h"
#include "GameWorld.h"

class GameModel;

class LeaderboardEntry {
public:
    LeaderboardEntry(): playerName(""), score(0) {}
    LeaderboardEntry(const std::string& name, long score): playerName(name), score(score) {}
    LeaderboardEntry(const LeaderboardEntry& copy): playerName(copy.playerName), score(copy.score) {}
    ~LeaderboardEntry() {}

    //uint32_t GetTotalScore() const {
    //    uint32_t total = 0;
    //    for (LevelToScoreMapConstIter iter = this->levelScores.begin(); iter != this->levelScores.end(); ++iter) {
    //        total += iter->second;
    //    }
    //    return total;
    //}

    std::string playerName;
    long score;

    static bool ReadFromFile(std::ifstream& inFile, LeaderboardEntry& entry);
    bool WriteToFile(std::ofstream& outFile) const;

    //std::map<std::pair<GameWorld::WorldStyle, int>, uint32_t> levelScores;
};

class ArcadeLeaderboard {
    // Befriend the reading and writing functions in the resource manager - we want the
    // resource manager to be able to use this class easily
    friend ArcadeLeaderboard ResourceManager::ReadLeaderboard(bool, const GameModel&);
    friend bool ResourceManager::WriteLeaderboard(const ArcadeLeaderboard&);

public:
    ArcadeLeaderboard(const GameModel& model);
    ~ArcadeLeaderboard();

    int GetNumEntries() const { return static_cast<int>(this->entries.size()); }
    const LeaderboardEntry& GetEntry(int idx) const { return this->entries[idx]; }
    LeaderboardEntry& GetEntry(int idx) { return this->entries[idx]; }


    // A score only qualifies if it's higher than the lowest score
    bool ScoreQualifies(long score) { return score > this->entries.back().score; }

private:
    // Filepath to the game's arcade leaderboard file
    static const char* FILEPATH;

    static const int NUM_ENTRIES;

    std::vector<LeaderboardEntry> entries; // Sorted from highest to lowest score
    std::map<std::pair<int,int>, long> levelHighscores; // Mappings of <worldIdx,levelIdx> to the highest score ever set for that level by a player

    static ArcadeLeaderboard* ReadFromFile(const GameModel& model);
    bool WriteToFile() const;


    static bool ReadLevelHighscoreFromFile(std::ifstream& inFile, std::pair<int,int>& worldLevelIdxPair, long& levelHighscore);
    static bool WriteLevelHighscoreToFile(std::ofstream& outFile, const std::pair<int,int>& worldLevelIdxPair, long levelHighscore);
};

#endif // __ARCADELEADERBOARD_H__