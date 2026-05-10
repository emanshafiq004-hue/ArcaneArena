#pragma once
#include <string>

using namespace std;

class FileManager {
public:
enum class LoadStatus {
    Ok,
    NotFound,
    Corrupt
};

static void savePlayer(const string& playerData, const string& questData,const string& filename = "saves/save.dat");
static LoadStatus loadPlayer(string& outPlayerData, string& outQuestData,const string& filename = "saves/save.dat");
static void saveScore(const string& playerName, int level, int xp,const string& filename = "saves/scores.dat");
static string loadScores(const string& filename = "saves/scores.dat");
static bool saveExists(const string& filename = "saves/save.dat");
};