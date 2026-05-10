#include "Battle.h"
#include "Spirits.h"
#include "AssetLoader.h"
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <stdexcept>
#include <unordered_map>

using namespace std;
using namespace sf;

// ─────────────────────────────────────────────────────────────────────────────
// Internal helpers
// ─────────────────────────────────────────────────────────────────────────────

static string slugifyName(const string& name) {
    string s;
    s.reserve(name.size());
    for (unsigned char c : name) {
        if (c == ' ') s += '_';
        else          s += static_cast<char>(tolower(c));
    }
    return s;
}

static vector<string> enemyPortraitPaths(const string& enemyName) {
    static const unordered_map<string, string> aliases = {
        { "Corrupted Dryad",   "willowisp"        },
        { "Temple Guardian",   "stone_golem"      },
        { "Ember Tyrant",      "emberwing"        },
        { "Shadow Sovereign",  "shadow_sovereign" },
        { "Stone Golem",       "stone_golem"      },
        { "Phantom Wraith",    "phantom_wraith"   },
        { "Willowisp",         "willowisp"        },
        { "Emberwing",         "emberwing"        },
    };
    vector<string> paths;
    auto it = aliases.find(enemyName);
    if (it != aliases.end()) {
        paths.push_back("assets/characters/" + it->second + ".png");
        paths.push_back("assets/characters/" + it->second + ".jpg");
    }
    const string sl = slugifyName(enemyName);
    paths.push_back("assets/characters/" + sl + ".png");
    paths.push_back("assets/characters/" + sl + ".jpg");
    return paths;
}

// ─────────────────────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────────────────────

Battle::Battle(GameWindow& window, Veilwalker& p, Spirit& e)
    : gw(window), player(p), enemy(e),
    selectedAction(0), bossHalfHealthTriggered(false) {
    if (!AssetLoader::openFontWithFallback(font,
        { "assets/font/Philosopher-Bold.ttf",
          "assets/fonts/Philosopher-Bold.ttf" }))
        throw runtime_error("Battle: Cannot load font!");
    tryLoadBattleDecor();
    addLog("A wild " + enemy.getName() + " appears!");
}

// ─────────────────────────────────────────────────────────────────────────────
// Asset loading  (unchanged logic, only comments added)
// ─────────────────────────────────────────────────────────────────────────────

void Battle::tryLoadBattleDecor() {
    const float vw = static_cast<float>(gw.getWidth());
    const float vh = static_cast<float>(gw.getHeight());

    // Ornamental frame — drawn LAST so it sits on top of everything.
    // REQUIREMENT: ui_battle_frame.png must have a transparent centre so
    // sprites and HUD text are visible through it.
    if (AssetLoader::loadTextureWithFallback(battleFrameTex,
        { "assets/texture/ui_battle_frame.png",
          "assets/textures/ui_battle_frame.png" })) {
        battleFrameTex.setSmooth(true);
        Sprite spr(battleFrameTex);
        const auto ts = battleFrameTex.getSize();
        const float sx = vw / static_cast<float>(ts.x);
        const float sy = vh / static_cast<float>(ts.y);
        spr.setOrigin({ static_cast<float>(ts.x) * 0.5f,
                        static_cast<float>(ts.y) * 0.5f });
        spr.setScale({ max(sx, sy), max(sx, sy) });
        spr.setPosition({ vw * 0.5f, vh * 0.5f });
        battleFrameSpr.emplace(std::move(spr));
    }

    // Semi-transparent background overlay — drawn immediately after the
    // vignette rect so all content (sprites, HUD) renders on top of it.
    if (AssetLoader::loadTextureWithFallback(interfaceTex,
        { "assets/interface/interface_.png",
          "assets/interface/interface.png" })) {
        interfaceTex.setSmooth(true);
        Sprite spr(interfaceTex);
        const auto ts = interfaceTex.getSize();
        const float sx = vw / static_cast<float>(ts.x);
        const float sy = vh / static_cast<float>(ts.y);
        spr.setOrigin({ static_cast<float>(ts.x) * 0.5f,
                        static_cast<float>(ts.y) * 0.5f });
        spr.setScale({ max(sx, sy), max(sx, sy) });
        spr.setPosition({ vw * 0.5f, vh * 0.5f });
        spr.setColor(Color(255, 255, 255, 72)); // alpha=72: semi-transparent
        interfaceSpr.emplace(std::move(spr));
    }

    // Player portrait — bottom-left area.
    if (AssetLoader::loadTextureWithFallback(playerPortraitTex,
        { "assets/characters/veilwalker.png",
          "assets/characters/player.png",
          "assets/characters/veilwalker.jpg" })) {
        playerPortraitTex.setSmooth(true);
        playerPortraitSpr.emplace(playerPortraitTex);
        const float ph = 160.f;
        const float sc = ph / static_cast<float>(playerPortraitTex.getSize().y);
        playerPortraitSpr->setScale({ sc, sc });
        playerPortraitSpr->setPosition({ 32.f, vh - 210.f });
    }

    // Enemy portrait — top-right area.
    if (AssetLoader::loadTextureWithFallback(enemyPortraitTex,
        enemyPortraitPaths(enemy.getName()))) {
        enemyPortraitTex.setSmooth(true);
        enemyPortraitSpr.emplace(enemyPortraitTex);
        const float eh = 200.f;
        const float sc = eh / static_cast<float>(enemyPortraitTex.getSize().y);
        enemyPortraitSpr->setScale({ sc, sc });
        enemyPortraitSpr->setPosition({ vw - 220.f, 48.f });
    }

    // Coin icon — small, drawn beside coin count.
    if (AssetLoader::loadTextureWithFallback(coinTex,
        { "assets/coins/Gold_coin.png",
          "assets/coin/Gold_coin.png",
          "assets/coins/gold_coin.png" })) {
        coinTex.setSmooth(true);
        coinSpr.emplace(coinTex);
        const float sc = 28.f / static_cast<float>(coinTex.getSize().y);
        coinSpr->setScale({ sc, sc });
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Battle log
// ─────────────────────────────────────────────────────────────────────────────

void Battle::addLog(const string& msg) {
    string upperMsg;
    upperMsg.reserve(msg.size());
    for (unsigned char c : msg)
        upperMsg.push_back(static_cast<char>(toupper(c)));
    battleLog.push_back(upperMsg);
    if (battleLog.size() > 5)
        battleLog.erase(battleLog.begin());
}

// ─────────────────────────────────────────────────────────────────────────────
// Gradient bar helper
// ─────────────────────────────────────────────────────────────────────────────

void Battle::drawGradientBar(float x, float y, float w, float h,
    int current, int max,
    Color left, Color right) {
    // Dark background trough
    RectangleShape bg({ w, h });
    bg.setPosition({ x, y });
    bg.setFillColor(Color(34, 34, 42));
    gw.getWindow().draw(bg);

    const float ratio = (max > 0)
        ? clamp(static_cast<float>(current) / static_cast<float>(max), 0.f, 1.f)
        : 0.f;
    const float fw = w * ratio;
    if (fw < 0.5f) return;

    // Gradient fill using a TriangleStrip
    VertexArray strip(PrimitiveType::TriangleStrip, 4);
    strip[0].position = { x,       y }; strip[0].color = left;
    strip[1].position = { x + fw,  y }; strip[1].color = right;
    strip[2].position = { x,       y + h }; strip[2].color = left;
    strip[3].position = { x + fw,  y + h }; strip[3].color = right;
    gw.getWindow().draw(strip);
}

// ─────────────────────────────────────────────────────────────────────────────
// MAIN RENDER FUNCTION
//
// ═══ SFML PAINTER'S ALGORITHM — draw order IS visual stack order ═══
//
//  Layer 1 (bottom)  gw.clear() + vignette rect          solid dark BG
//  Layer 2           interfaceSpr (alpha=72)              semi-transparent overlay
//  Layer 3           enemyPortraitSpr                     enemy sprite — visible
//  Layer 4           playerPortraitSpr                    player sprite — visible
//  Layer 5           enemy name / HP bar / HP text        enemy HUD
//  Layer 6           player name / HP bar / SP bar        player HUD
//  Layer 7           XP bar                               player XP progress
//  Layer 8           coin icon + coin count               coins
//  Layer 9           battleLog text                       combat messages
//  Layer 10          actionMenu text                      ATTACK / ABILITY / ITEM / EXIT
//  Layer 11 (top)    battleFrameSpr                       ornamental border
//  gw.display()
//
// ─────────────────────────────────────────────────────────────────────────────

void Battle::drawBattleUI() {
    const float vw = static_cast<float>(gw.getWidth());
    const float vh = static_cast<float>(gw.getHeight());

    // ── Layer 1: clear + solid dark vignette ─────────────────────────────
    gw.clear();
    RectangleShape vignette({ vw, vh });
    vignette.setFillColor(Color(20, 15, 40));
    gw.getWindow().draw(vignette);

    // ── Layer 2: semi-transparent background overlay ──────────────────────
    // Drawn before sprites so sprites are visible on top of it.
    if (interfaceSpr)
        gw.getWindow().draw(*interfaceSpr);

    // ── Layers 3–4: character sprites ────────────────────────────────────
    // Both drawn before any HUD element so HUD text renders on top of
    // sprite pixels rather than being hidden beneath them.
    if (enemyPortraitSpr)
        gw.getWindow().draw(*enemyPortraitSpr);
    if (playerPortraitSpr)
        gw.getWindow().draw(*playerPortraitSpr);

    // ── Layer 5: enemy stats (name + HP bar + HP text) ───────────────────
    {
        Text enemyName(font);
        enemyName.setString(enemy.getName());
        enemyName.setCharacterSize(28);
        enemyName.setFillColor(Color(255, 140, 0));
        enemyName.setPosition({ vw - 360.f, 16.f });
        gw.getWindow().draw(enemyName);

        const float eBarX = vw - 360.f;
        drawGradientBar(eBarX, 50.f, 340.f, 22.f,
            enemy.getHp(), enemy.getMaxHp(),
            Color(180, 20, 60), Color(255, 120, 90));

        Text eHp(font);
        eHp.setString(to_string(enemy.getHp()) + " / " + to_string(enemy.getMaxHp()));
        eHp.setCharacterSize(14);
        eHp.setFillColor(Color(255, 255, 255));
        eHp.setPosition({ eBarX + 4.f, 76.f });
        gw.getWindow().draw(eHp);
    }

    // ── Layer 6: player stats (name + HP bar + SP bar) ───────────────────
    {
        Text pName(font);
        pName.setString(player.getName() + "  LV." + to_string(player.getLevel()));
        pName.setCharacterSize(20);
        pName.setFillColor(Color(255, 255, 255));
        pName.setPosition({ 28.f, vh - 268.f });
        gw.getWindow().draw(pName);

        // HP bar — green gradient
        drawGradientBar(28.f, vh - 240.f, 300.f, 20.f,
            player.getHp(), player.getMaxHp(),
            Color(30, 160, 60), Color(120, 255, 140));

        Text pHp(font);
        pHp.setString("HP " + to_string(player.getHp()) + "/" + to_string(player.getMaxHp()));
        pHp.setCharacterSize(13);
        pHp.setFillColor(Color(255, 255, 255));
        pHp.setPosition({ 32.f, vh - 214.f });
        gw.getWindow().draw(pHp);

        // SP (Spirit Energy) bar — purple gradient
        drawGradientBar(28.f, vh - 190.f, 280.f, 14.f,
            player.getSpiritEnergy(), player.getMaxEnergy(),
            Color(100, 40, 180), Color(220, 120, 255));

        Text pEn(font);
        pEn.setString("SP " + to_string(player.getSpiritEnergy())
            + "/" + to_string(player.getMaxEnergy()));
        pEn.setCharacterSize(12);
        pEn.setFillColor(Color(255, 255, 255));
        pEn.setPosition({ 32.f, vh - 172.f });
        gw.getWindow().draw(pEn);
    }

    // ── Layer 7: XP bar ──────────────────────────────────────────────────
    // Shows progress toward the next level using a cyan-to-gold gradient.
    {
        const int  currentXP = player.getXP();
        const int  xpForNext = player.getXPToNext(); // implement if missing
        drawGradientBar(28.f, vh - 154.f, 260.f, 10.f,
            currentXP, xpForNext,
            Color(0, 180, 220), Color(255, 220, 60));

        Text xpLabel(font);
        xpLabel.setString("XP " + to_string(currentXP) + "/" + to_string(xpForNext));
        xpLabel.setCharacterSize(11);
        xpLabel.setFillColor(Color(200, 230, 255));
        xpLabel.setPosition({ 32.f, vh - 140.f });
        gw.getWindow().draw(xpLabel);
    }

    // ── Layer 8: coin icon + count ────────────────────────────────────────
    {
        const float coinY = vh - 124.f;
        if (coinSpr) {
            coinSpr->setPosition({ 28.f, coinY });
            gw.getWindow().draw(*coinSpr);
        }
        Text coinsLabel(font);
        coinsLabel.setString(to_string(player.getCoins()));
        coinsLabel.setCharacterSize(14);
        coinsLabel.setFillColor(Color(255, 220, 100));
        coinsLabel.setPosition({ coinSpr ? 62.f : 28.f, coinY });
        gw.getWindow().draw(coinsLabel);
    }

    // ── Layer 9: battle log ───────────────────────────────────────────────
    drawBattleLog();

    // ── Layer 10: action menu ─────────────────────────────────────────────
    drawActionMenu();

    // ── Layer 11: ornamental frame (topmost) ──────────────────────────────
    // Drawn last so it sits above all other content.
    // ui_battle_frame.png must have a transparent centre — if it does not,
    // sprites and HUD will be hidden. Verify the PNG has an alpha channel.
    if (battleFrameSpr)
        gw.getWindow().draw(*battleFrameSpr);

    gw.display();
}

// ─────────────────────────────────────────────────────────────────────────────
// Action menu  (drawn in Layer 10, on top of portraits and bars)
// ─────────────────────────────────────────────────────────────────────────────

void Battle::drawActionMenu() {
    const float vh = static_cast<float>(gw.getHeight());
    const string actions[] = { "ATTACK", "ABILITY", "ITEM", "EXIT" };
    for (int i = 0; i < 4; i++) {
        Text t(font);
        t.setString((i == selectedAction ? "> " : "  ") + actions[i]);
        t.setCharacterSize(18);
        t.setFillColor(i == selectedAction ? Color(0, 255, 200) : Color(220, 220, 255));
        t.setPosition({ 28.f, vh - 120.f + static_cast<float>(i) * 28.f });
        gw.getWindow().draw(t);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Battle log  (drawn in Layer 9)
// ─────────────────────────────────────────────────────────────────────────────

void Battle::drawBattleLog() {
    const float vw = static_cast<float>(gw.getWidth());
    const float vh = static_cast<float>(gw.getHeight());
    for (int i = 0; i < static_cast<int>(battleLog.size()); i++) {
        Text line(font);
        line.setString(battleLog[i]);
        line.setCharacterSize(13);
        line.setFillColor(Color(200, 200, 240));
        line.setPosition({ 320.f, vh * 0.35f + static_cast<float>(i) * 20.f });
        gw.getWindow().draw(line);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Combat logic  (unchanged)
// ─────────────────────────────────────────────────────────────────────────────

void Battle::enemyTurn() {
    PhantomWraith* wraith = dynamic_cast<PhantomWraith*>(&enemy);
    if (wraith && wraith->stealsItems() && player.getInventory().getCount() > 0) {
        player.getInventory().useItem(0);
        addLog("Phantom Wraith STOLE an item!");
        return;
    }
    int dmg = enemy.attackPlayer();
    player.takeDamage(dmg);
    addLog(enemy.getName() + " attacks for " + to_string(dmg) + " dmg!");
}

int Battle::playerTurn(BattleAction action) {
    switch (action) {
    case BattleAction::Attack: {
        int dmg = player.attackEnemy();
        enemy.takeDamage(dmg);
        addLog("You attack for " + to_string(dmg) + " dmg!");
        return dmg;
    }
    case BattleAction::UseAbility: {
        if (player.getAbilities().empty()) {
            addLog("No abilities!");
            return 0;
        }
        int dmg = player.useAbility(0);
        if (dmg < 0) {
            addLog("Not enough Spirit Energy!");
            return 0;
        }
        enemy.takeDamage(dmg);
        addLog(player.getAbilities()[0].getName() + " hits for " + to_string(dmg) + "!");
        return dmg;
    }
    case BattleAction::UseItem: {
        if (player.getInventory().getCount() == 0) {
            addLog("No items!");
            return 0;
        }
        int healed = player.useItemFromInventory(0);
        addLog("Used item! Healed " + to_string(healed) + " HP.");
        return 0;
    }
    case BattleAction::Exit:
        addLog("You exit the battle.");
        return -1;
    default:
        return 0;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Main loop  (unchanged logic)
// ─────────────────────────────────────────────────────────────────────────────

BattleResult Battle::run() {
    Clock inputCooldown;
    while (gw.isOpen()) {
        while (auto event = gw.pollEvent()) {
            if (event->is<Event::Closed>()) {
                gw.getWindow().close();
                return BattleResult::PlayerDied;
            }
            if (auto* key = event->getIf<Event::KeyPressed>()) {
                if (inputCooldown.getElapsedTime().asMilliseconds() > 200) {
                    inputCooldown.restart();

                    if (key->code == Keyboard::Key::Up)
                        selectedAction = (selectedAction - 1 + 4) % 4;
                    if (key->code == Keyboard::Key::Down)
                        selectedAction = (selectedAction + 1) % 4;

                    if (key->code == Keyboard::Key::Enter) {
                        int result = playerTurn(
                            static_cast<BattleAction>(selectedAction));

                        if (result == -1)
                            return BattleResult::PlayerExited;

                        if (!enemy.isAlive()) {
                            addLog(enemy.getName() + " defeated! +"
                                + to_string(enemy.getXPReward()) + " XP");
                            player.gainXP(enemy.getXPReward());
                            drawBattleUI();
                            sleep(seconds(2.f));
                            return BattleResult::PlayerWon;
                        }

                        if (enemy.getIsBoss()
                            && enemy.hasReachedHalfHealth()
                            && !bossHalfHealthTriggered) {
                            bossHalfHealthTriggered = true;
                            enemy.onHalfHealth();
                            addLog("!! " + enemy.getName() + " TRANSFORMS !!");
                        }

                        enemyTurn();

                        if (!player.isAlive()) {
                            addLog("You have fallen...");
                            drawBattleUI();
                            sleep(seconds(2.f));
                            return BattleResult::PlayerDied;
                        }
                    }
                }
            }
        }
        drawBattleUI();
    }
    return BattleResult::PlayerDied;
}