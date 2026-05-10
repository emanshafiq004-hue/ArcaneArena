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
    Texture scrollTex;
    optional<sf::Sprite> scrollSprite0;
    optional<sf::Sprite> scrollSprite1;
    Texture staticBgTex;
    optional<sf::Sprite> staticBgSpr;
    bool scrollActive = false;
    float scrollOffset = 0.f;
    bool quitToHub = false;

    float centerX() const;
    string upperText(const string& s) const;
    void setupRegions();
    void setupAudio();
    void tryBindScrollForRegion(int regionIndex);
    void tryBindScrollForWorldMap();
    bool loadStaticBackground(const vector<string>& paths);
    void clearStaticBackground();
    void updateScroll(float dt);
    void drawScrollBehind();
    void drawBackdrop();
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
