#pragma once
#include "player.h"
#include "enemy.h"

// ============================================================
//  CombatState
//
//  Tracks the cooldown timer that prevents the enemy from
//  registering a hit on the player every single frame during
//  its attack animation.
//
//  Encapsulation: the clock and cooldown value are private.
//  External code uses isOnCooldown() / resetClock().
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


// ---- Free functions (declared here, defined in combat.cpp) ----

void resolveBodyCollision(Player& player, Enemy& enemy);

// Standard J-attack hit: uses player.getHitRegistered() / setHitRegistered().
bool checkPlayerHitsEnemy(Player& player, Enemy& enemy, int attackDamage = 5);

// Enemy hitting player (generic — used for Creature, Knight, and Samurai normal hits).
bool checkEnemyHitsPlayer(Enemy& enemy, Player& player, CombatState& state, int attackDamage = 5);

// ---- Adventurer K-combo hit checks ----
//
// Each combo has two independent hits tracked by separate flags
// (comboHit1Registered / comboHit2Registered) so both the first
// (attack1) and the second (upper / attack2) swing can each deal
// damage exactly once per combo activation.

// Phase 1 — attack1 animation (same sprite as J-attack, higher base damage).
bool checkPlayerComboPhase1HitsEnemy(Player& player, Enemy& enemy, int attackDamage);

// Phase 2 — upper / attack2 animation (the finishing blow).
bool checkPlayerComboPhase2HitsEnemy(Player& player, Enemy& enemy, int attackDamage);
