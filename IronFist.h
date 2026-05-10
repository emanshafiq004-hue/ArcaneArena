// IronFist.h
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
public:
    IronFist(GameWindow& w);
    void run();
};
