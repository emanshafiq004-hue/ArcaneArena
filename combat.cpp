#include "combat.h"

// ---------------------------------------------------------------
// resolveBodyCollision
// ---------------------------------------------------------------
void resolveBodyCollision(Player& player, Enemy& enemy)
{
    if (!player.isOnGround())
    {
        player.setCollisionWall(false, false);
        return;
    }

    sf::FloatRect pb = player.getBounds();
    sf::FloatRect eb = enemy.getBounds();

    auto overlap = pb.findIntersection(eb);
    if (!overlap.has_value())
    {
        player.setCollisionWall(false, false);
        return;
    }

    float overlapW = overlap->size.x;
    float overlapH = overlap->size.y;

    if (overlapW < overlapH)
    {
        float half = overlapW * 0.5f;

        if (pb.position.x < eb.position.x)
        {
            player.pushX(-half);
            enemy.setWorldX(enemy.getWorldX() + half);
            player.setCollisionWall(false, true);
        }
        else
        {
            player.pushX(half);
            enemy.setWorldX(enemy.getWorldX() - half);
            player.setCollisionWall(true, false);
        }
    }
    else
    {
        player.setCollisionWall(false, false);
    }
}

// ---------------------------------------------------------------
// checkPlayerHitsEnemy  — standard J-attack
// ---------------------------------------------------------------
bool checkPlayerHitsEnemy(Player& player, Enemy& enemy, int attackDamage)
{
    if (!player.isAttacking() || player.getHitRegistered())
        return false;

    if (!player.getAttackBox().findIntersection(enemy.getBounds()).has_value())
        return false;

    enemy.takeDamage(attackDamage);
    player.setHitRegistered(true);
    return true;
}

// ---------------------------------------------------------------
// checkEnemyHitsPlayer  — generic enemy hit (Creature / Knight / Samurai)
//
// Block behaviour:
//   If the player is in a blocking stance (ENTER or HOLD phase,
//   i.e. L is pressed) when the attack lands, damage is halved and
//   the block_success reaction animation is triggered on the player.
//   Full damage is applied when the player is not blocking.
// ---------------------------------------------------------------

bool checkEnemyHitsPlayer(Enemy& enemy, Player& player,
    CombatState& state, int attackDamage)
{
    if (!enemy.isAttacking())
        return false;

    if (state.isOnCooldown())
        return false;

    if (!enemy.getAttackBox().findIntersection(player.getBounds()).has_value())
        return false;

    if (player.isBlocking())
    {
        // Block absorbs half the incoming damage.

        int reducedDamage = attackDamage / 2;
        if (reducedDamage < 1) reducedDamage = 1;

        player.takeDamage(reducedDamage);
        player.triggerBlockSuccess();   // play the block_success animation
    }
    else
    {
        player.takeDamage(attackDamage);
    }

    state.resetClock();
    return true;
}

// ---------------------------------------------------------------
// checkPlayerComboPhase1HitsEnemy
//
// Fires during comboPhase == 1 (attack1 animation).
// Uses comboHit1Registered so this hit is counted at most once
// per combo activation.
// ---------------------------------------------------------------

bool checkPlayerComboPhase1HitsEnemy(Player& player, Enemy& enemy, int attackDamage)
{
    if (!player.isComboing())              return false;
    if (player.getComboPhase() != 1)       return false;
    if (player.getComboHit1Registered())   return false;

    if (!player.getAttackBox().findIntersection(enemy.getBounds()).has_value())
        return false;

    enemy.takeDamage(attackDamage);
    player.setComboHit1Registered(true);
    return true;
}

// ---------------------------------------------------------------
// checkPlayerComboPhase2HitsEnemy
//
// Fires during comboPhase == 2 (attack2 / upper animation).
// Uses comboHit2Registered so this hit is counted at most once
// per combo activation.
// ---------------------------------------------------------------

bool checkPlayerComboPhase2HitsEnemy(Player& player, Enemy& enemy, int attackDamage)
{
    if (!player.isComboing())              return false;
    if (player.getComboPhase() != 2)       return false;
    if (player.getComboHit2Registered())   return false;

    if (!player.getAttackBox().findIntersection(enemy.getBounds()).has_value())
        return false;

    enemy.takeDamage(attackDamage);
    player.setComboHit2Registered(true);
    return true;
}
