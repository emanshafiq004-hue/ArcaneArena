#pragma once
#include "player.h"
#include "enemy.h"

// ============================================================
//  CombatState
// ============================================================

class CombatState
{
private:
    sf::Clock enemyHitClock;
    float     enemyHitCooldown;

public:
    CombatState()
    {
        enemyHitCooldown = 1.0f;
    }

    bool isOnCooldown() const
    {
        return enemyHitClock.getElapsedTime().asSeconds() <= enemyHitCooldown;
    }

    void resetClock()
    {
        enemyHitClock.restart();
    }
};



void resolveBodyCollision(Player& player, Enemy& enemy);

// Standard J-attack hit: uses player.getHitRegistered() / setHitRegistered().
bool checkPlayerHitsEnemy(Player& player, Enemy& enemy, int attackDamage = 5);

// Enemy hitting player (generic — used for Creature, Knight, and Samurai normal hits).
bool checkEnemyHitsPlayer(Enemy& enemy, Player& player, CombatState& state, int attackDamage = 5);

bool checkPlayerComboPhase1HitsEnemy(Player& player, Enemy& enemy, int attackDamage);

// Phase 2 — upper / attack2 animation (the finishing blow).
bool checkPlayerComboPhase2HitsEnemy(Player& player, Enemy& enemy, int attackDamage);
