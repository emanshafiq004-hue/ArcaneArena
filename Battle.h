//Battle.h
#pragma once
#include "Veilwalker.h"
#include "Spirit.h"
#include "GameWindow.h"
#include <optional>
#include <string>
#include <vector>

using namespace std;
using namespace sf;

enum class BattleAction { Attack, UseAbility, UseItem, Exit };

enum class BattleResult { PlayerWon, PlayerExited, PlayerDied };

class Battle {
private:
    GameWindow& gw;
    Veilwalker& player;
    Spirit& enemy;
    Font font;
    vector<string> battleLog;
    int selectedAction;
    bool bossHalfHealthTriggered;

    Texture battleFrameTex;
    Texture interfaceTex;
    Texture playerPortraitTex;
    Texture enemyPortraitTex;
    Texture coinTex;
    optional<Sprite> battleFrameSpr;
    optional<Sprite> interfaceSpr;
    optional<Sprite> playerPortraitSpr;
    optional<Sprite> enemyPortraitSpr;
    optional<Sprite> coinSpr;

    void addLog(const string& message);
    void drawBattleUI();
    void drawGradientBar(float x, float y, float w, float h, int current, int max, Color left, Color right);
    void drawActionMenu();
    void drawBattleLog();
    int playerTurn(BattleAction action);
    void enemyTurn();
    void tryLoadBattleDecor();

public:
    Battle(GameWindow& window, Veilwalker& player, Spirit& enemy);
    BattleResult run();
};
