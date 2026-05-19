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

struct Settings {
    bool soundOn = true;
};

static void savePlayer(const string& playerData, const string& questData,const string& filename = "saves/save.dat");
static LoadStatus loadPlayer(string& outPlayerData, string& outQuestData,const string& filename = "saves/save.dat");
static void saveScore(const string& playerName, int level, int xp,const string& filename = "saves/scores.dat");
static string loadScores(const string& filename = "saves/scores.dat");
static bool saveExists(const string& filename = "saves/save.dat");

// SkySurge game methods
static int getBestScoreFromFile(const string& filename = "saves/skysurge_best.dat");
static Settings loadSettings(const string& filename = "saves/skysurge_settings.dat");
static void saveSettings(const Settings& settings, const string& filename = "saves/skysurge_settings.dat");
static void addScore(const string& playerName, int score, const string& filename = "saves/skysurge_scores.dat");
};