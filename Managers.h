#pragma once
//  Contains:
//    ScoreEntry   (Operator Overloading)
//    ScoreManager (Friend Class)
//    GameWorld    (Aggregation)

#include "FileManager.h"
#include "Globals.h"
#include "Entities.h"
#include "Obstacles.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <stdexcept>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>


//_____________OPERATOR OVERLOADING: ScoreEntry__________

struct ScoreEntry {
    std::string name;
    int         score = 0;
    std::string date;

    ScoreEntry() = default;   //Default ctor
    ScoreEntry(std::string n, int s, std::string d)
        : name(std::move(n)), score(s), date(std::move(d)) {
    }

    bool operator>(const ScoreEntry& o)  const { return score > o.score; }
    bool operator==(const ScoreEntry& o) const { return score == o.score && name == o.name; }
};

//___________CLASS: ScoreManager_____________

class ScoreManager {

    int   score_ = 0;
    int   bestScore_ = 0;
    int   coins_ = 0;
    bool  multiplier_ = false;
    float multTimer_ = 0.f;
    float stormSlow_ = 0.f;
    bool  shield_ = false;

public:
    ScoreManager() = default;

    void reset(int best);
    void addBarrier();
    void addCrystal();
    void activateMulti();
    void activateStorm();
    void setShield(bool v);
    void update(float dt);

    //_________Encapsulated getters/setters__________
    int   getScore()     const { return score_; }
    int   getBest()      const { return bestScore_; }
    int   getCoins()     const { return coins_; }
    bool  hasMulti()     const { return multiplier_; }
    bool  hasShield()    const { return shield_; }
    float getMultTimer() const { return multTimer_; }
    float getStormSlow() const { return stormSlow_; }
    void  setBest(int b) { bestScore_ = b; }

    int   tier()        const;
    float scrollSpeed() const;
    float gapSize()     const;
};

//____________CLASS: FileManager (for ScoreEntry/SkySurge)_______________
//  Handles scores and settings for SkySurge game (works with ScoreEntry)
//  Uses try/catch for all file I/O operations

class FileManager_SkySurge {
private:
    static constexpr const char* SCORE_FILE = "saves/skysurge_scores.dat";
    static constexpr const char* SETTING_FILE = "saves/skysurge_settings.dat";
    static constexpr int TOP5 = 5;

public:
    std::vector<ScoreEntry> loadScores();
    void saveScores(const std::vector<ScoreEntry>& entries);
    void addScore(const std::string& name, int score);
    int getBestScoreFromFile();

    struct Settings { bool soundOn = true; };
    Settings loadSettings();
    void     saveSettings(const Settings& s);
};

//____________________________________________________________
//  AGGREGATION: GameWorld
//  Holds (aggregates) all live game objects via smart pointers.
//  Objects are spawned externally — GameWorld does not own
//  the types, only the instances during their lifetime.
//______________________________________________________________
class GameWorld {
public:
    std::vector<Barrier>                      barriers;
    std::vector<std::unique_ptr<Obstacle>>    obstacles;
    std::vector<std::unique_ptr<Collectible>> collectibles;

    float barrierSpawnT = 0.f;
    float obstacleSpawnT = 0.f;
    float collectSpawnT = 0.f;
    bool  coinCollected = false;

    void clear();

    // Factory spawn methods (polymorphic creation)
    void spawnObstacle(float x, float spd, int tier);
    void spawnCollectible(float x, float spd);

    void update(float dt, float spd, ScoreManager& sm);

    //Polymorphic draw via virtual dispatch
    void draw(sf::RenderTarget& rt);

    //Polymorphic collision (dynamic_cast for collectibles)
    void checkCollisions(Drone& drone, ScoreManager& sm,
        ParticleSystem& ps);
};
