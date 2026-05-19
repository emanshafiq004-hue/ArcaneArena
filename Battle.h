////// Battle.h
////#pragma once
////#include "Veilwalker.h"
////#include "Spirit.h"
////#include "GameWindow.h"//Aggregation
////#include <optional>
////#include <string>
////#include <vector>
////
////using namespace std;
////using namespace sf;
////
////enum class BattleAction { Attack, UseAbility, UseItem, Exit };
////
////enum class BattleResult { PlayerWon, PlayerExited, PlayerDied };
////
////class Battle 
////{
////private:
////    GameWindow& gw;
////    Veilwalker& player;
////    Spirit& enemy;
////    Font font;
////    vector<string> battleLog;
////    int selectedAction;
////    bool bossHalfHealthTriggered;
////
////    Texture battleFrameTex;
////    Texture playerPortraitTex;
////    Texture enemyPortraitTex;
////    Texture coinTex;
////    optional<Sprite> battleFrameSpr;
////    optional<Sprite> playerPortraitSpr;
////    optional<Sprite> enemyPortraitSpr;
////    optional<Sprite> coinSpr;
////
////    void addLog(const string& message);
////    void drawBattleUI();
////    void drawGradientBar(float x, float y, float w, float h,int current, int max, Color left, Color right);
////    void drawActionMenu();
////    void drawBattleLog();
////    int  playerTurn(BattleAction action);
////    void enemyTurn();
////    void tryLoadBattleDecor();
////
////public:
////    Battle(GameWindow& window, Veilwalker& player, Spirit& enemy);//aggregates GameWindow,Veilwalker and Spirit....
////    BattleResult run();
////};
//
//
//
//
//
//
//
//
//
//
////---------------------------------------
//// Battle.h – Real‑time battle between Veilwalker and a Spirit.
//// Uses arrow keys for movement, mouse for action menu, and automatic enemy AI.
//
//#pragma once
//#include "Veilwalker.h"
//#include "Spirit.h"
//#include "GameWindow.h"
//#include "AudioManager.h"
//#include <SFML/Graphics.hpp>
//#include <optional>
//#include <string>
//#include <vector>
//
//enum class BattleResult { PlayerWon, PlayerExited, PlayerDied };
//
//class Battle
//{
//private:
//    GameWindow& gw;
//    Veilwalker& player;
//    Spirit& enemy;
//    AudioManager& audio;
//    sf::Font font;
//
//    // region background (optional)
//    sf::Sprite* bgSprite;
//
//    // ── Player physics & animation ─────────────────────────────────
//    sf::Vector2f playerPos;            // feet position
//    float playerSpeed = 200.f;
//    bool playerJumping = false;
//    float playerVelY = 0.f;
//    const float gravity = 800.f;
//    const float groundY = 500.f;       // floor level
//
//    // ── Enemy AI ────────────────────────────────────────────────────
//    sf::Vector2f enemyPos;
//    float enemySpeed = 80.f;
//    bool enemyAttacking = false;
//    sf::Clock enemyAttackClock;
//    float enemyAttackCooldown = 1.5f;
//    float enemyAttackDuration = 0.5f;
//    bool enemyFacingLeft = false;
//
//    // ── Action menu (mouse + Enter) ────────────────────────────────
//    int selectedAction = 0;             // 0:Attack 1:Ability 2:Item 3:Exit
//    sf::RectangleShape menuButtons[4];
//    sf::Text buttonLabels[4];
//
//    // sub‑menu for ability selection
//    bool showAbilityMenu = false;
//    int selectedAbilityIndex = 0;
//    std::vector<sf::Text> abilityTexts;
//
//    // attack cooldown for player
//    sf::Clock playerAttackClock;
//    float playerAttackCooldown = 0.5f;
//
//    // combat state
//    bool playerCanAct = true;
//
//    // ── Battle log ─────────────────────────────────────────────────
//    std::vector<std::string> battleLog;
//    void addLog(const std::string& msg);
//
//    // ── Internal helpers ───────────────────────────────────────────
//    void handleInput(float dt);
//    void handlePlayerAttack();
//    void handlePlayerAbility();
//    void handlePlayerItem();
//    void handleExit();
//    void updatePhysics(float dt);
//    void updateEnemyAI(float dt);
//    void updateAnimations(float dt);
//    void applyDamageToPlayer(int dmg);
//    void applyDamageToEnemy(int dmg);
//    void drawHUD();
//    void drawActionMenu();
//    void drawAbilityMenu();
//    void drawBattleLog();
//    void drawPlayerSprite();
//    void drawEnemySprite();
//
//public:
//    Battle(GameWindow& window, Veilwalker& player, Spirit& enemy,
//        AudioManager& audioRef, sf::Sprite* bgSpr = nullptr);
//    BattleResult run();
//};












//----------------------------------------------
// Battle.h
// Real-time battle between Veilwalker and a Spirit.
// Arrow keys move the player. Left/Right select action. Enter confirms.
// Mouse hover highlights buttons. Enemy AI moves and attacks automatically.

#pragma once
#include "Veilwalker.h"
#include "Spirit.h"
#include "GameWindow.h"
#include "AudioManager.h"
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include <vector>

// ── Result returned to the caller after battle ends ──────────────────────────
enum class BattleResult { PlayerWon, PlayerExited, PlayerDied };

// ─────────────────────────────────────────────────────────────────────────────
class Battle
{
private:

    // ── Core references (Aggregation) ────────────────────────────────────────
    GameWindow& gw;
    Veilwalker& player;
    Spirit& enemy;
    AudioManager& audio;
    sf::Font      font;

    // Optional region background sprite (owned externally).
    sf::Sprite* bgSprite;

    // ── Player physics ───────────────────────────────────────────────────────
    sf::Vector2f playerPos;
    float        playerSpeed = 200.f;
    bool         playerJumping = false;
    float        playerVelY = 0.f;
    const float  gravity = 800.f;
    const float  groundY = 480.f;   // floor level (pixels from top)

    // ── Enemy AI ─────────────────────────────────────────────────────────────
    sf::Vector2f enemyPos;
    float        enemySpeed = 80.f;
    bool         enemyAttacking = false;
    sf::Clock    enemyAttackClock;
    float        enemyAttackCooldown = 1.5f;
    float        enemyAttackDuration = 0.5f;
    bool         enemyFacingLeft = true;
    bool         bossPhaseTriggered = false;

    // ── Action menu ──────────────────────────────────────────────────────────
    // Four buttons laid out left to right at the bottom of the screen.
    int               selectedAction = 0;   // 0:Attack 1:Ability 2:Item 3:Exit
    sf::RectangleShape menuButtons[4];
    sf::Text          buttonLabels[4];

    // Cursor triangle drawn next to the selected button label.
    sf::ConvexShape cursor;

    // Sub-menu for ability selection.
    bool              showAbilityMenu = false;
    int               selectedAbilityIndex = 0;
    std::vector<sf::Text> abilityTexts;

    // ── Attack cooldown ──────────────────────────────────────────────────────
    sf::Clock playerAttackClock;
    float     playerAttackCooldown = 0.5f;

    // Set to false when player chooses Exit; returns PlayerExited next frame.
    bool playerCanAct = true;

    // ── Battle log (last 5 messages) ─────────────────────────────────────────
    std::vector<std::string> battleLog;

    // ── Internal helpers ─────────────────────────────────────────────────────
    void addLog(const std::string& msg);
    void buildCursor();

    void handleInput(float dt);
    void handlePlayerAttack();
    void handlePlayerAbility();
    void handlePlayerItem();
    void handleExit();

    void updatePhysics(float dt);
    void updateEnemyAI(float dt);
    void updateAnimations(float dt);

    void applyDamageToPlayer(int dmg);
    void applyDamageToEnemy(int dmg);

    // HUD: bars aligned top-left (player) and top-right (enemy).
    void drawHUD();
    void drawGradientBar(float x, float y, float w, float h,
        int current, int max,
        sf::Color left, sf::Color right);

    // Action menu at the bottom, left to right.
    void drawActionMenu();
    void drawAbilityMenu();
    void drawBattleLog();
    void drawPlayerSprite();
    void drawEnemySprite();

public:
    // bgSpr is optional; pass nullptr if no background sprite is available.
    Battle(GameWindow& window, Veilwalker& player, Spirit& enemy,
        AudioManager& audioRef, sf::Sprite* bgSpr = nullptr);

    BattleResult run();
};