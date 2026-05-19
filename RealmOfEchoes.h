//-------------------------------------------- ( RealmOfEchoes.h ) --------------------------------------------------
#pragma once
#include "GameWindow.h"
#include "Veilwalker.h"
#include "Region.h"
#include "QuestLog.h"
#include "Battle.h"
#include "GameLogger.h"
#include "AudioManager.h"

//Post-battle choices for the player:-
enum class PostBattleChoice { ContinuePath, PlayAgain, ExitToHub };

class RealmOfEchoes
{
private:
//Data Members:-
GameWindow& gw;
sf::Font font;
Veilwalker* player;
std::vector<Region> regions;
QuestLog questLog;
GameLogger logger;
AudioManager& audio;
sf::Texture staticBgTex;
std::optional<sf::Sprite> staticBgSpr;
bool quitToHub = false;
int bossAttemptsLeft = 3;

//Member Functions:-
float centerX() const;
std::string upperText(const std::string& s) const;
void setupRegions();
void setupAudio();
bool loadStaticBackground(const std::vector<std::string>& paths);
void loadRegionBackground(int regionIndex);
void loadWorldMapBackground();
void clearStaticBackground();
void drawBackdrop();
void syncSave(int regionIndex, const Region& region);
std::string showNameEntryScreen();
void showStoryScreen();
void showWorldMapScreen(int& selectedRegion);
void runRegion(int regionIndex);
void showShrineScreen(int regionIndex);
void showRegionCompleteScreen(int regionIndex);
void showEndingScreen();
void drawCenteredText(const std::string& str, float y, unsigned int size, const sf::Color& color);
void runPhantomSurvival(int regionIndex, Region& region);
void runNegotiationScreen(const std::string& msg);
bool runGateChoice(int regionIndex, Region& region, const Encounter& enc);
int bossCoinReward(int regionIndex) const;
public:
RealmOfEchoes(GameWindow& window,AudioManager& a);
~RealmOfEchoes();
void run();
};