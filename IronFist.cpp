// IronFist.cpp
// Converted from standalone main.cpp to a module of Arcane Arena.
// The only structural changes:
//   1) int main() → void IronFist::run()
//   2) Window creation → reference from GameWindow (gw.getWindow())
//   3) window.close() → return  (goes back to Arcane Arena menu)
//   4) Removed the SetCurrentDirectory block (binding main.cpp handles it)
//   5) Removed return 0

#include "IronFist.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;
using namespace sf;

enum GameState
{
    STATE_PRESS_START,
    STATE_MENU,
    STATE_NAME_INPUT,
    STATE_CONTINUE_LIST,
    STATE_CONTINUE_PLAYER,
    STATE_SCOREBOARD,
    STATE_INTRO,
    STATE_FIGHTING,
    STATE_YOU_WIN,
    STATE_YOU_LOSE,
    STATE_INTRO_STAGE2,
    STATE_FIGHTING_STAGE2,
    STATE_YOU_WIN_STAGE2,
    STATE_INTRO_STAGE3,
    STATE_FIGHTING_STAGE3,
    STATE_YOU_WIN_STAGE3,
    STATE_END_MENU
};

// Per-round score: HP%-based.
int calculateScore(int currentHealth, int maxHealth)
{
    float pct = (float)currentHealth / (float)maxHealth * 100.f;
    if (pct > 50.f)  return 100;
    if (pct >= 20.f) return 65;
    return 30;
}

// ---------------------------------------------------------------
//  Stage helpers
// ---------------------------------------------------------------
static void startAtRound(int round, GameState& state,
    Player& player, Creature& creature, Knight& knight, Samurai& samurai,
    CombatState& combat1, CombatState& combat2, CombatState& combat3,
    sf::Clock& introClock)
{
    player.reset();

    if (round == 1)
    {
        creature.reset();
        combat1 = CombatState();
        state = STATE_INTRO;
    }
    else if (round == 2)
    {
        knight.reset();
        combat2 = CombatState();
        state = STATE_INTRO_STAGE2;
    }
    else
    {
        samurai.reset();
        combat3 = CombatState();
        state = STATE_INTRO_STAGE3;
    }
    introClock.restart();
}

// ---------------------------------------------------------------
//  Filter for live name-input keystrokes.
// ---------------------------------------------------------------
static bool isAcceptableNameChar(char32_t c)
{
    if (c == '|') return false;
    if (c < 32 || c > 126) return false;
    return true;
}

// ===============================================================
//  Constructor
// ===============================================================
IronFist::IronFist(GameWindow& w) : gw(w) {}

// ===============================================================
//  run()  —  this is your old main(), almost line-for-line
// ===============================================================
void IronFist::run()
{
    srand((unsigned int)time(NULL));

    // *** THE KEY TRICK ***
    // Instead of creating a new window, grab a reference to the shared one.
    // Every existing  window.draw / window.clear / window.display  call
    // works exactly as before — zero search-and-replace needed.
    sf::RenderWindow& window = gw.getWindow();
    window.setFramerateLimit(1300);

    sf::Font font;
    font.openFromFile("Assets/Sprites/Font/BoldPixels.ttf");

    Player   player;
    Creature creature;
    Knight   knight;
    Samurai  samurai;

    CombatState combat1;
    CombatState combat2;
    CombatState combat3;

    // ----------------------------------------------------------------
    //  Background layers  (unchanged)
    // ----------------------------------------------------------------
    // Stage 1
    sf::Texture bgMoonTex, bgMountainsTex, bgGraveyardTex;
    bgMoonTex.loadFromFile("Assets/Sprites/Backgrounds/bg-moon.png");
    bgMountainsTex.loadFromFile("Assets/Sprites/Backgrounds/bg-mountains.png");
    bgGraveyardTex.loadFromFile("Assets/Sprites/Backgrounds/bg-graveyard.png");

    const float S1_SCALE = 800.f / 384.f;

    sf::Sprite bgMoon(bgMoonTex);
    bgMoon.setScale(sf::Vector2f(S1_SCALE, S1_SCALE));
    bgMoon.setPosition(sf::Vector2f(0.f, 0.f));

    sf::Sprite bgMountains(bgMountainsTex);
    bgMountains.setScale(sf::Vector2f(S1_SCALE, S1_SCALE));
    bgMountains.setPosition(sf::Vector2f(0.f, 0.f));

    sf::Sprite bgGraveyard(bgGraveyardTex);
    bgGraveyard.setScale(sf::Vector2f(S1_SCALE, S1_SCALE));
    bgGraveyard.setPosition(sf::Vector2f(0.f, 531.f - 124.f * S1_SCALE));

    // Stage 2
    sf::Texture bgForestTex;
    bgForestTex.loadFromFile("Assets/Sprites/Backgrounds/forest.png");

    const float S2_SCALE = 800.f / 928.f;

    sf::Sprite bgForest(bgForestTex);
    bgForest.setScale(sf::Vector2f(S2_SCALE, S2_SCALE));
    bgForest.setPosition(sf::Vector2f(0.f, 531.f - 735.f * S2_SCALE));

    // Stage 3
    sf::Texture bgHillsTex;
    bgHillsTex.loadFromFile("Assets/Sprites/Backgrounds/Hills.png");

    const float S3_SCALE = 600.f / 256.f;

    sf::Sprite bgHills(bgHillsTex);
    bgHills.setScale(sf::Vector2f(S3_SCALE, S3_SCALE));
    bgHills.setPosition(sf::Vector2f(
        -(512.f * S3_SCALE - 800.f) * 0.5f,
        531.f - 235.f * S3_SCALE));

    // ----------------------------------------------------------------
    //  Sound effects  (unchanged)
    // ----------------------------------------------------------------
    sf::SoundBuffer sbMainAttack, sbMainHit;
    sbMainAttack.loadFromFile("Assets/Sprites/sound-effects/mainCharAttack.mp3");
    sbMainHit.loadFromFile("Assets/Sprites/sound-effects/mainCharGetHit.wav");

    sf::Sound sndMainAttack(sbMainAttack);
    sf::Sound sndMainHit(sbMainHit);

    static constexpr int MAIN_HIT_DELAY_MS = 50;
    bool      mainHitPending = false;
    sf::Clock mainHitDelayClock;

    ScoreBoard sb;
    sb.load();

    int    currentPlayerIdx = -1;
    string nameInput;

    GameState state = STATE_PRESS_START;

    sf::Clock introClock;
    sf::Clock blinkClock;

    // Menu navigation
    int  menuIndex = 0;
    bool menuWHeld = false;
    bool menuSHeld = false;
    bool menuJHeld = false;

    // Scoreboard navigation
    int  sbIndex = 0;
    bool sbWHeld = false;
    bool sbSHeld = false;
    bool sbJHeld = false;

    // Continue-list navigation
    int  contIndex = 0;
    bool contWHeld = false;
    bool contSHeld = false;
    bool contJHeld = false;

    // Continue-player navigation
    int  cpIndex = 0;
    bool cpWHeld = false;
    bool cpSHeld = false;
    bool cpJHeld = false;

    // Name-input one-shot key flags
    bool nameEnterHeld = false;
    bool nameEscHeld = false;

    bool escHeld = false;

    // ==============================================================
    //  GAME LOOP — only 2 changes inside:
    //    window.close()  →  return
    //    (that's it)
    // ==============================================================
    while (window.isOpen())
    {
        // ============================================================
        //  EVENT POLLING
        // ============================================================
        while (optional<sf::Event> ev = window.pollEvent())
        {
            // CHANGED: instead of closing the shared window, just return
            // to the Arcane Arena main menu.
            if (ev->is<sf::Event::Closed>())
                return;   // was: window.close();

            if (state == STATE_NAME_INPUT)
            {
                if (auto* te = ev->getIf<sf::Event::TextEntered>())
                {
                    char32_t c = te->unicode;
                    if (c == 8)
                    {
                        if (nameInput.length() > 0)
                            nameInput.erase(nameInput.length() - 1, 1);
                    }
                    else if (c == 13 || c == 10)
                    {
                        // Enter handled below
                    }
                    else if (isAcceptableNameChar(c)
                        && (int)nameInput.length() < MAX_NAME_LEN)
                    {
                        nameInput += (char)c;
                    }
                }
            }
        }

        // Universal ESC release tracker
        bool escDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape);
        if (!escDown) escHeld = false;

        // ============================================================
        //  LOGIC  (completely unchanged from your original)
        // ============================================================

        if (state == STATE_PRESS_START)
        {
            // CHANGED: ESC on the press-start screen returns to Arcane Arena
            if (escDown && !escHeld)
            {
                escHeld = true;
                return;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
                state = STATE_MENU;
        }

        else if (state == STATE_MENU)
        {
            // CHANGED: ESC on Iron Fist's own menu returns to Arcane Arena
            if (escDown && !escHeld)
            {
                escHeld = true;
                return;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
            {
                if (!menuWHeld) { menuIndex = (menuIndex + 2) % 3; menuWHeld = true; }
            }
            else menuWHeld = false;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
            {
                if (!menuSHeld) { menuIndex = (menuIndex + 1) % 3; menuSHeld = true; }
            }
            else menuSHeld = false;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::J))
            {
                if (!menuJHeld)
                {
                    menuJHeld = true;
                    if (menuIndex == 0)
                    {
                        nameInput.clear();
                        nameEnterHeld = true;
                        nameEscHeld = true;
                        blinkClock.restart();
                        state = STATE_NAME_INPUT;
                    }
                    else if (menuIndex == 1)
                    {
                        contIndex = 0;
                        contWHeld = contSHeld = false;
                        contJHeld = true;
                        escHeld = true;
                        state = STATE_CONTINUE_LIST;
                    }
                    else
                    {
                        sbIndex = 0;
                        sbWHeld = sbSHeld = false;
                        sbJHeld = true;
                        state = STATE_SCOREBOARD;
                    }
                }
            }
            else menuJHeld = false;
        }

        else if (state == STATE_NAME_INPUT)
        {
            if (escDown)
            {
                if (!nameEscHeld && !escHeld)
                {
                    nameEscHeld = true;
                    escHeld = true;
                    menuIndex = 0;
                    menuWHeld = menuSHeld = false;
                    menuJHeld = true;
                    state = STATE_MENU;
                }
            }
            else nameEscHeld = false;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter))
            {
                if (!nameEnterHeld)
                {
                    nameEnterHeld = true;
                    while (nameInput.length() > 0
                        && nameInput[nameInput.length() - 1] == ' ')
                        nameInput.erase(nameInput.length() - 1, 1);

                    if (nameInput.length() > 0)
                    {
                        currentPlayerIdx = sb.addOrReplace(nameInput);
                        if (currentPlayerIdx >= 0) sb.save();
                        startAtRound(1, state, player, creature, knight, samurai,
                            combat1, combat2, combat3, introClock);
                    }
                }
            }
            else nameEnterHeld = false;
        }

        else if (state == STATE_CONTINUE_LIST)
        {
            int incompleteN = sb.countIncomplete();

            if (escDown && !escHeld)
            {
                escHeld = true;
                menuIndex = 0;
                menuWHeld = menuSHeld = false;
                menuJHeld = true;
                state = STATE_MENU;
            }

            if (incompleteN > 0)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
                {
                    if (!contWHeld) {
                        contIndex = (contIndex - 1 + incompleteN) % incompleteN;
                        contWHeld = true;
                    }
                }
                else contWHeld = false;

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
                {
                    if (!contSHeld) {
                        contIndex = (contIndex + 1) % incompleteN;
                        contSHeld = true;
                    }
                }
                else contSHeld = false;

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::J))
                {
                    if (!contJHeld)
                    {
                        contJHeld = true;
                        int pi = sb.getIncompleteIndex(contIndex);
                        if (pi >= 0)
                        {
                            currentPlayerIdx = pi;
                            cpIndex = 0;
                            cpWHeld = cpSHeld = false;
                            cpJHeld = true;
                            state = STATE_CONTINUE_PLAYER;
                        }
                    }
                }
                else contJHeld = false;
            }
        }

        else if (state == STATE_CONTINUE_PLAYER)
        {
            if (escDown && !escHeld)
            {
                escHeld = true;
                contJHeld = true;
                state = STATE_CONTINUE_LIST;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
            {
                if (!cpWHeld) { cpIndex = (cpIndex == 0) ? 1 : 0; cpWHeld = true; }
            }
            else cpWHeld = false;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
            {
                if (!cpSHeld) { cpIndex = (cpIndex == 1) ? 0 : 1; cpSHeld = true; }
            }
            else cpSHeld = false;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::J))
            {
                if (!cpJHeld)
                {
                    cpJHeld = true;
                    if (cpIndex == 0 && currentPlayerIdx >= 0)
                    {
                        int next = sb.get(currentPlayerIdx).nextRound();
                        if (next >= 1 && next <= 3)
                        {
                            startAtRound(next, state, player, creature, knight, samurai,
                                combat1, combat2, combat3, introClock);
                        }
                    }
                    else
                    {
                        contJHeld = true;
                        state = STATE_CONTINUE_LIST;
                    }
                }
            }
            else cpJHeld = false;
        }

        else if (state == STATE_SCOREBOARD)
        {
            if (escDown && !escHeld)
            {
                escHeld = true;
                menuIndex = 0;
                menuWHeld = menuSHeld = false;
                menuJHeld = true;
                state = STATE_MENU;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
            {
                if (!sbWHeld) { sbIndex = (sbIndex == 0) ? 1 : 0; sbWHeld = true; }
            }
            else sbWHeld = false;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
            {
                if (!sbSHeld) { sbIndex = (sbIndex == 1) ? 0 : 1; sbSHeld = true; }
            }
            else sbSHeld = false;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::J))
            {
                if (!sbJHeld)
                {
                    sbJHeld = true;
                    if (sbIndex == 0)
                    {
                        menuIndex = 0;
                        menuWHeld = menuSHeld = false;
                        menuJHeld = true;
                        state = STATE_MENU;
                    }
                    else
                    {
                        sb.clearAll();
                        sbIndex = 0;
                        currentPlayerIdx = -1;
                    }
                }
            }
            else sbJHeld = false;
        }

        // ----------------------------------------------------------------
        //  Stage 1 — Creature
        // ----------------------------------------------------------------
        else if (state == STATE_INTRO)
        {
            if (introClock.getElapsedTime().asSeconds() >= 3.5f)
                state = STATE_FIGHTING;
        }

        else if (state == STATE_FIGHTING)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
            {
                menuIndex = 0;
                menuWHeld = menuSHeld = menuJHeld = false;
                escHeld = true;
                state = STATE_MENU;
            }
            else
            {
                if (!creature.isDead())
                {
                    player.setCollisionWall(false, false);

                    bool wasAttacking = player.isAttacking();
                    bool wasComboing = player.isComboing();

                    player.handleInput();
                    player.update();

                    if ((!wasAttacking && player.isAttacking()) ||
                        (!wasComboing && player.isComboing()))
                    {
                        sndMainAttack.stop();
                        sndMainAttack.play();
                    }

                    sf::Vector2f playerPos = player.getBounds().position;
                    creature.update(playerPos);

                    resolveBodyCollision(player, creature);
                    checkPlayerHitsEnemy(player, creature, 8);
                    checkPlayerComboPhase1HitsEnemy(player, creature, 12);
                    checkPlayerComboPhase2HitsEnemy(player, creature, 22);

                    if (checkEnemyHitsPlayer(creature, player, combat1, 8)) { mainHitPending = true; mainHitDelayClock.restart(); }

                    if (player.isDead())
                    {
                        state = STATE_YOU_LOSE;
                        introClock.restart();
                    }
                }
                else
                {
                    player.update();
                    creature.update(player.getBounds().position);
                }

                if (creature.isDyingAnimDone())
                {
                    int sc = calculateScore(player.getHealth(), player.getMaxHealth());
                    if (currentPlayerIdx >= 0)
                    {
                        PlayerRecord& p = sb.at(currentPlayerIdx);
                        p.r1Score = sc;
                        p.r1Done = true;
                        sb.save();
                    }
                    state = STATE_YOU_WIN;
                    introClock.restart();
                }
            }
        }

        else if (state == STATE_YOU_WIN)
        {
            if (introClock.getElapsedTime().asSeconds() >= 3.5f)
            {
                player.reset(); knight.reset(); combat2 = CombatState();
                state = STATE_INTRO_STAGE2; introClock.restart();
            }
        }

        // ----------------------------------------------------------------
        //  Stage 2 — Knight
        // ----------------------------------------------------------------
        else if (state == STATE_INTRO_STAGE2)
        {
            if (introClock.getElapsedTime().asSeconds() >= 3.5f)
                state = STATE_FIGHTING_STAGE2;
        }

        else if (state == STATE_FIGHTING_STAGE2)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
            {
                menuIndex = 0;
                menuWHeld = menuSHeld = menuJHeld = false;
                escHeld = true;
                state = STATE_MENU;
            }
            else
            {
                if (!knight.isDead())
                {
                    player.setCollisionWall(false, false);

                    bool wasAttacking = player.isAttacking();
                    bool wasComboing = player.isComboing();

                    player.handleInput();
                    player.update();

                    if ((!wasAttacking && player.isAttacking()) ||
                        (!wasComboing && player.isComboing()))
                    {
                        sndMainAttack.stop();
                        sndMainAttack.play();
                    }

                    sf::Vector2f playerPos = player.getBounds().position;
                    knight.tryBlock(player.isAttacking() || player.isComboing());
                    knight.update(playerPos);

                    resolveBodyCollision(player, knight);
                    checkPlayerHitsEnemy(player, knight, 8);
                    checkPlayerComboPhase1HitsEnemy(player, knight, 12);
                    checkPlayerComboPhase2HitsEnemy(player, knight, 22);

                    if (checkEnemyHitsPlayer(knight, player, combat2, 8)) { mainHitPending = true; mainHitDelayClock.restart(); }

                    if (player.isDead())
                    {
                        state = STATE_YOU_LOSE;
                        introClock.restart();
                    }
                }
                else
                {
                    player.update();
                    knight.update(player.getBounds().position);
                    if (knight.isDyingAnimDone())
                    {
                        int sc = calculateScore(player.getHealth(), player.getMaxHealth());
                        if (currentPlayerIdx >= 0)
                        {
                            PlayerRecord& p = sb.at(currentPlayerIdx);
                            p.r2Score = sc;
                            p.r2Done = true;
                            sb.save();
                        }
                        menuIndex = 0;
                        menuWHeld = menuSHeld = false;
                        menuJHeld = true;
                        state = STATE_YOU_WIN_STAGE2;
                        introClock.restart();
                    }
                }
            }
        }

        else if (state == STATE_YOU_WIN_STAGE2)
        {
            if (introClock.getElapsedTime().asSeconds() >= 3.5f)
            {
                player.reset(); samurai.reset(); combat3 = CombatState();
                state = STATE_INTRO_STAGE3; introClock.restart();
            }
        }

        // ----------------------------------------------------------------
        //  Stage 3 — Samurai
        // ----------------------------------------------------------------
        else if (state == STATE_INTRO_STAGE3)
        {
            if (introClock.getElapsedTime().asSeconds() >= 3.5f)
                state = STATE_FIGHTING_STAGE3;
        }

        else if (state == STATE_FIGHTING_STAGE3)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
            {
                menuIndex = 0;
                menuWHeld = menuSHeld = menuJHeld = false;
                escHeld = true;
                state = STATE_MENU;
            }
            else
            {
                if (!samurai.isDead())
                {
                    player.setCollisionWall(false, false);

                    bool wasAttacking = player.isAttacking();
                    bool wasComboing = player.isComboing();

                    player.handleInput();
                    player.update();

                    if ((!wasAttacking && player.isAttacking()) ||
                        (!wasComboing && player.isComboing()))
                    {
                        sndMainAttack.stop();
                        sndMainAttack.play();
                    }

                    sf::Vector2f playerPos = player.getBounds().position;
                    samurai.notifyPlayerAttacking(player.isAttacking() || player.isComboing());
                    samurai.update(playerPos);

                    resolveBodyCollision(player, samurai);
                    checkPlayerHitsEnemy(player, samurai, 8);
                    checkPlayerComboPhase1HitsEnemy(player, samurai, 12);
                    checkPlayerComboPhase2HitsEnemy(player, samurai, 22);

                    bool samuraiHitLanded = false;
                    if (samurai.isUsingVariant())
                        samuraiHitLanded = checkEnemyHitsPlayer(samurai, player, combat3, 22);
                    else
                        samuraiHitLanded = checkEnemyHitsPlayer(samurai, player, combat3, 8);
                    if (samuraiHitLanded) { mainHitPending = true; mainHitDelayClock.restart(); }

                    if (player.isDead())
                    {
                        state = STATE_YOU_LOSE;
                        introClock.restart();
                    }
                }
                else
                {
                    player.update();
                    samurai.update(player.getBounds().position);
                    if (samurai.isDyingAnimDone())
                    {
                        int sc = calculateScore(player.getHealth(), player.getMaxHealth());
                        if (currentPlayerIdx >= 0)
                        {
                            PlayerRecord& p = sb.at(currentPlayerIdx);
                            p.r3Score = sc;
                            p.r3Done = true;
                            sb.save();
                        }
                        menuIndex = 0;
                        menuWHeld = menuSHeld = false;
                        menuJHeld = true;
                        state = STATE_YOU_WIN_STAGE3;
                        introClock.restart();
                    }
                }
            }
        }

        else if (state == STATE_YOU_WIN_STAGE3)
        {
            if (introClock.getElapsedTime().asSeconds() >= 3.5f)
            {
                menuIndex = 0;
                menuWHeld = menuSHeld = menuJHeld = false;
                state = STATE_END_MENU;
            }
        }

        // ----------------------------------------------------------------
        //  End menu
        // ----------------------------------------------------------------
        else if (state == STATE_END_MENU)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
            {
                if (!menuWHeld) { menuIndex = (menuIndex == 0) ? 1 : 0; menuWHeld = true; }
            }
            else menuWHeld = false;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
            {
                if (!menuSHeld) { menuIndex = (menuIndex == 1) ? 0 : 1; menuSHeld = true; }
            }
            else menuSHeld = false;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::J))
            {
                if (!menuJHeld)
                {
                    menuJHeld = true;
                    if (menuIndex == 0)
                    {
                        nameInput.clear();
                        nameEnterHeld = true;
                        nameEscHeld = true;
                        blinkClock.restart();
                        state = STATE_NAME_INPUT;
                    }
                    else
                    {
                        state = STATE_MENU;
                    }
                }
            }
            else menuJHeld = false;
        }

        else if (state == STATE_YOU_LOSE)
        {
            if (introClock.getElapsedTime().asSeconds() >= 3.5f)
            {
                menuIndex = 0;
                menuWHeld = menuSHeld = menuJHeld = false;
                state = STATE_MENU;
            }
        }

        // ============================================================
        //  DRAW  (completely unchanged)
        // ============================================================
        if (state == STATE_PRESS_START) { drawPressStartScreen(window, font, blinkClock); window.display(); continue; }
        if (state == STATE_MENU) { drawMenuScreen(window, font, menuIndex);        window.display(); continue; }
        if (state == STATE_NAME_INPUT) { drawNameInputScreen(window, font, nameInput, blinkClock); window.display(); continue; }
        if (state == STATE_CONTINUE_LIST) { drawContinueListScreen(window, font, sb, contIndex); window.display(); continue; }
        if (state == STATE_CONTINUE_PLAYER) {
            const PlayerRecord& p = (currentPlayerIdx >= 0) ? sb.get(currentPlayerIdx)
                : PlayerRecord();
            drawContinuePlayerScreen(window, font, p, cpIndex);
            window.display();
            continue;
        }
        if (state == STATE_SCOREBOARD) { drawScoreboardScreen(window, font, sb, sbIndex); window.display(); continue; }
        if (state == STATE_INTRO) { drawIntroStage1Screen(window, font); window.display(); continue; }
        if (state == STATE_INTRO_STAGE2) { drawIntroStage2Screen(window, font); window.display(); continue; }
        if (state == STATE_INTRO_STAGE3) { drawIntroStage3Screen(window, font); window.display(); continue; }
        if (state == STATE_END_MENU) { drawEndMenuScreen(window, font, menuIndex); window.display(); continue; }

        // ----------------------------------------------------------------
        //  Fighting / win / lose screens
        // ----------------------------------------------------------------
        window.clear(sf::Color::Black);

        bool isStage2 = (state == STATE_FIGHTING_STAGE2 || state == STATE_YOU_WIN_STAGE2);
        bool isStage3 = (state == STATE_FIGHTING_STAGE3 || state == STATE_YOU_WIN_STAGE3);

        if (isStage3)
            window.draw(bgHills);
        else if (isStage2)
            window.draw(bgForest);
        else
        {
            window.draw(bgMoon);
            window.draw(bgMountains);
            window.draw(bgGraveyard);
        }

        player.draw(window);
        if (isStage3)      samurai.draw(window);
        else if (isStage2) knight.draw(window);
        else               creature.draw(window);

        if (isStage3)
            drawHud(window, font, player.getHealth(), player.getMaxHealth(),
                samurai.getHealth(), samurai.getMaxHealth(), 3);
        else if (isStage2)
            drawHud(window, font, player.getHealth(), player.getMaxHealth(),
                knight.getHealth(), knight.getMaxHealth(), 2);
        else
            drawHud(window, font, player.getHealth(), player.getMaxHealth(),
                creature.getHealth(), creature.getMaxHealth(), 1);

        if (state == STATE_FIGHTING || state == STATE_FIGHTING_STAGE2 || state == STATE_FIGHTING_STAGE3)
            drawEscHint(window, font);

        if (state == STATE_YOU_WIN)        drawYouWinOverlay(window, font);
        if (state == STATE_YOU_WIN_STAGE2) drawYouWinStage2Overlay(window, font);
        if (state == STATE_YOU_WIN_STAGE3) drawYouWinStage3Overlay(window, font);
        if (state == STATE_YOU_LOSE)       drawYouLoseOverlay(window, font);

        if (mainHitPending &&
            mainHitDelayClock.getElapsedTime().asMilliseconds() >= MAIN_HIT_DELAY_MS)
        {
            sndMainHit.stop();
            sndMainHit.play();
            mainHitPending = false;
        }

        window.display();
    }

    // No return 0 — this is void, not main().
    // When this function ends, control goes back to Arcane Arena's menu loop.
}
