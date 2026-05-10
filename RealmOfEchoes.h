#pragma once
#include "GameWindow.h"
#include "Veilwalker.h"
#include "Region.h"
#include "QuestLog.h"
#include "Battle.h"
#include "GameLogger.h"
#include "AudioManager.h"
#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <string>

using namespace std;

enum class PostBattleChoice { ContinuePath, PlayAgain, ExitToHub };

class RealmOfEchoes {
private:
    GameWindow& gw;
    Font font;
    Veilwalker* player;
    vector<Region> regions;
    QuestLog questLog;
    GameLogger logger;
    AudioManager audio;

    // ── Static background (one sprite, no scrolling) ──────────────────────
    Texture staticBgTex;
    optional<sf::Sprite> staticBgSpr;

    bool quitToHub = false;

    float centerX() const;
    string upperText(const string& s) const;
    void setupRegions();
    void setupAudio();

    // Loads a static cover-fitted background from the first path that succeeds.
    bool loadStaticBackground(const vector<string>& paths);
    void loadRegionBackground(int regionIndex);   // per-region static BG
    void loadWorldMapBackground();                // world-map static BG
    void clearStaticBackground();

    void drawBackdrop();                          // draw static BG (no scroll)

    void syncSave(int regionIndex, const Region& region);
    string showNameEntryScreen();
    void showStoryScreen();
    void showWorldMapScreen(int& selectedRegion);
    void runRegion(int regionIndex);
    void showShrineScreen(int regionIndex, bool fullRestore);
    void showRegionCompleteScreen(int regionIndex);
    void showEndingScreen();
    void drawCenteredText(const string& str, float y, unsigned int size, const Color& color);
    void runPhantomSurvival(int regionIndex, Region& region);
    void runNegotiationScreen(const string& msg);
    bool runGateChoice(int regionIndex, Region& region, const Encounter& enc);
    PostBattleChoice runPostBattleChoice(BattleResult result);
    int bossCoinReward(int regionIndex) const;

public:
    RealmOfEchoes(GameWindow& window);
    ~RealmOfEchoes();
    void run();
};