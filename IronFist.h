#pragma once
#include "GameWindow.h"
#include "player.h"
#include "enemy.h"
#include "combat.h"
#include "ui.h"
#include "scoreboard.h"
#include <string>

class IronFist {
private:
    GameWindow& gw;
    AudioManager& audio;
public:
    IronFist(GameWindow& w, AudioManager& a);
    void run();
};
