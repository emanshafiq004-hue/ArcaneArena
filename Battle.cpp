//#include "Battle.h"
//#include "Spirits.h"
//#include "AssetLoader.h"
//#include <sstream>
//#include <algorithm>
//#include <cstdlib>
//#include <cctype>
//#include <stdexcept>
//#include <unordered_map>
//
//using namespace std;
//using namespace sf;
//
////---------------------------------------------------------------------------
//// INTERNAL HELPERS (Static Utilty Functions)
////---------------------------------------------------------------------------
//
//static string slugifyName(const string& name) 
//{
//string s;
//s.reserve(name.size());
//for (unsigned char c : name) 
//{
//if (c == ' ') 
//s += '_';
//else          
//s += static_cast<char>(tolower(c));
//}
//return s;
//}
//
//static vector<string> enemyPortraitPaths(const string& enemyName) 
//{
//static const unordered_map<string, string> aliases = {
//{ "Corrupted Dryad",   "willowisp"        },
//{ "Temple Guardian",   "stone_golem"      },
//{ "Ember Tyrant",      "emberwing"        },
//{ "Shadow Sovereign",  "shadow_sovereign" },
//{ "Stone Golem",       "stone_golem"      },
//{ "Phantom Wraith",    "phantom_wraith"   },
//{ "Willowisp",         "willowisp"        },
//{ "emberwing",         "emberwing"        },
//{ "Guidance Spirit",   "guidance_spirit"  },
//{ "Trickster",         "trickster"        }
//};
//vector<string> paths;
//auto it = aliases.find(enemyName);
//if (it != aliases.end()) 
//{
//paths.push_back("assets/characters/" + it->second + ".png");
//paths.push_back("assets/characters/" + it->second + ".jpg");
//}
//const string sl = slugifyName(enemyName);
//paths.push_back("assets/characters/" + sl + ".png");
//paths.push_back("assets/characters/" + sl + ".jpg");
//return paths;
//}
//
//// -----------------------------------------------------------------------------
//// CONSTRUCTOR:
//// -----------------------------------------------------------------------------
//
//Battle::Battle(GameWindow& window, Veilwalker& p, Spirit& e): gw(window), player(p), enemy(e),selectedAction(0), bossHalfHealthTriggered(false) 
//{
//if (!AssetLoader::openFontWithFallback(font,{ "assets/font/ActionSpectral.ttf","assets/font/Philosopher-Bold.ttf" }))
//throw runtime_error("Battle: Cannot load font!");
//tryLoadBattleDecor();
//addLog("A wild " + enemy.getName() + " appears!");
//}
//
//// ------------------------------------------------------------------------------
//// ASSETS LOADING:
//// ------------------------------------------------------------------------------
//
//void Battle::tryLoadBattleDecor() 
//{
//AssetLoader::emplaceCoverSprite(battleFrameTex, battleFrameSpr, { "assets/texture/ui_battle_frame.png","assets/texture/default.png" }, gw.getWidth(), gw.getHeight());  
//// Player portrait... bottom-left area...
//if (AssetLoader::loadTextureWithFallback(playerPortraitTex,{ "assets/characters/veilwalker.png","assets/characters/player.png","assets/characters/veilwalker.jpg" })) 
//{
//playerPortraitTex.setSmooth(true);
//playerPortraitSpr.emplace(playerPortraitTex);
//const float ph = 160.f;
//const float sc = ph / static_cast<float>(playerPortraitTex.getSize().y);
//playerPortraitSpr->setScale({ sc, sc });
//playerPortraitSpr->setPosition({ 32.f,gw.getHeight() - 210.f});
//}
//// Enemy portrait....bottom-right area...
//if (AssetLoader::loadTextureWithFallback(enemyPortraitTex,enemyPortraitPaths(enemy.getName()))) 
//{
//enemyPortraitTex.setSmooth(true);
//enemyPortraitSpr.emplace(enemyPortraitTex);
//const float eh = 160.f;
//const float sc = eh / static_cast<float>(enemyPortraitTex.getSize().y);
//enemyPortraitSpr->setScale({ sc, sc });
//enemyPortraitSpr->setPosition({gw.getWidth() - 220.f, 48.f});
//}
//
//// Coin icon:
//if (AssetLoader::loadTextureWithFallback(coinTex,{ "assets/coins/Gold_coin.png","assets/coins/Silver_coin.png"})) 
//{
//coinTex.setSmooth(true);
//coinSpr.emplace(coinTex);
//const float sc = 28.f / static_cast<float>(coinTex.getSize().y);
//coinSpr->setScale({ sc, sc });
//}
//}
//
////-----------------------------------------------------------------------------
//// BATTLE LOG:
////-----------------------------------------------------------------------------
//
//void Battle::addLog(const string& msg) 
//{
//string upperMsg;
//upperMsg.reserve(msg.size());
//for (unsigned char c : msg)
//upperMsg.push_back(static_cast<char>(toupper(c)));
//battleLog.push_back(upperMsg);
//if (battleLog.size() > 5)
//battleLog.erase(battleLog.begin());
//}
//
////------------------------------------------------------------------------------
//// GRADIENT BAR HELPER:
////------------------------------------------------------------------------------
//
//void Battle::drawGradientBar(float x, float y, float w, float h,int current, int max,Color left, Color right) 
//{
//// Dark background trough
//RectangleShape bg({ w, h });
//bg.setPosition({ x, y });
//bg.setFillColor(Color(34, 34, 42));
//gw.getWindow().draw(bg);
//const float ratio = (max > 0)? clamp(static_cast<float>(current) / static_cast<float>(max), 0.f, 1.f) : 0.f;
//const float fw = w * ratio;
//if (fw < 0.5f)
//return;
//// Gradient fill using a TriangleStrip
//VertexArray strip(PrimitiveType::TriangleStrip, 4);
//strip[0].position = { x, y }; 
//strip[0].color = left;
//strip[1].position = { x + fw, y }; 
//strip[1].color = right;
//strip[2].position = { x, y + h }; 
//strip[2].color = left;
//strip[3].position = { x + fw,  y + h };
//strip[3].color = right;
//gw.getWindow().draw(strip);
//}
//
////---------------------------------------------------------------------------
//// MAIN RENDER FUNCTION
////---------------------------------------------------------------------------
//
//void Battle::drawBattleUI() 
//{
//const float vw = static_cast<float>(gw.getWidth());
//const float vh = static_cast<float>(gw.getHeight());
//gw.clear();
//RectangleShape vignette({ vw, vh });
//vignette.setFillColor(Color(20, 15, 40));
//gw.getWindow().draw(vignette);
//if (battleFrameSpr)
//gw.getWindow().draw(*battleFrameSpr);
//if (enemyPortraitSpr)
//gw.getWindow().draw(*enemyPortraitSpr);
//if (playerPortraitSpr)
//gw.getWindow().draw(*playerPortraitSpr);
//{
//Text enemyName(font);
//enemyName.setString(enemy.getName());
//enemyName.setCharacterSize(28);
//enemyName.setFillColor(Color(255, 140, 0));
//enemyName.setPosition({ vw - 360.f, 16.f });
//gw.getWindow().draw(enemyName);
//const float eBarX = vw - 360.f;
//drawGradientBar(eBarX, 50.f, 340.f, 22.f,enemy.getHp(), enemy.getMaxHp(),Color(180, 20, 60), Color(255, 120, 90));
//Text eHp(font);
//eHp.setString(to_string(enemy.getHp()) + " / " + to_string(enemy.getMaxHp()));
//eHp.setCharacterSize(14);
//eHp.setFillColor(Color(255, 255, 255));
//eHp.setPosition({ eBarX + 4.f, 76.f });
//gw.getWindow().draw(eHp);
//}
//{
//Text pName(font);
//pName.setString(player.getName() + "  LV." + to_string(player.getLevel()));
//pName.setCharacterSize(20);
//pName.setFillColor(Color(255, 255, 255));
//pName.setPosition({ 28.f, vh - 268.f });
//gw.getWindow().draw(pName);
//// HP bar — green gradient...
//drawGradientBar(28.f, vh - 240.f, 300.f, 20.f,player.getHp(), player.getMaxHp(),Color(30, 160, 60), Color(120, 255, 140));
//Text pHp(font);
//pHp.setString("HP " + to_string(player.getHp()) + "/" + to_string(player.getMaxHp()));
//pHp.setCharacterSize(13);
//pHp.setFillColor(Color(255, 255, 255));
//pHp.setPosition({ 32.f, vh - 214.f });
//gw.getWindow().draw(pHp);
//// SP (Spirit Energy) bar.. purple gradient..
//drawGradientBar(28.f, vh - 190.f, 280.f, 14.f,player.getSpiritEnergy(), player.getMaxEnergy(),Color(100, 40, 180), Color(220, 120, 255));
//Text pEn(font);
//pEn.setString("SP " + to_string(player.getSpiritEnergy())+ "/" + to_string(player.getMaxEnergy()));
//pEn.setCharacterSize(12);
//pEn.setFillColor(Color(255, 255, 255));
//pEn.setPosition({ 32.f, vh - 172.f });
//gw.getWindow().draw(pEn);
//}
//
//    // Shows progress toward the next level using a cyan-to-gold gradient.
//    {
//        const int  currentXP = player.getXP();
//        const int  xpForNext = player.getXPToNext(); // implement if missing
//        drawGradientBar(28.f, vh - 154.f, 260.f, 10.f,
//            currentXP, xpForNext,
//            Color(0, 180, 220), Color(255, 220, 60));
//
//        Text xpLabel(font);
//        xpLabel.setString("XP " + to_string(currentXP) + "/" + to_string(xpForNext));
//        xpLabel.setCharacterSize(11);
//        xpLabel.setFillColor(Color(200, 230, 255));
//        xpLabel.setPosition({ 32.f, vh - 140.f });
//        gw.getWindow().draw(xpLabel);
//    }
//
//    {
//        const float coinY = vh - 124.f;
//        if (coinSpr) {
//            coinSpr->setPosition({ 28.f, coinY });
//            gw.getWindow().draw(*coinSpr);
//        }
//        Text coinsLabel(font);
//        coinsLabel.setString(to_string(player.getCoins()));
//        coinsLabel.setCharacterSize(14);
//        coinsLabel.setFillColor(Color(255, 220, 100));
//        coinsLabel.setPosition({ coinSpr ? 62.f : 28.f, coinY });
//        gw.getWindow().draw(coinsLabel);
//    }
//    drawBattleLog();
//    drawActionMenu();
//    gw.display();
//}
//
//// ─────────────────────────────────────────────────────────────────────────────
//// Action menu  (drawn in Layer 10, on top of portraits and bars)
//// ─────────────────────────────────────────────────────────────────────────────
//
//void Battle::drawActionMenu() {
//    const float vh = static_cast<float>(gw.getHeight());
//    const string actions[] = { "ATTACK", "ABILITY", "ITEM", "EXIT" };
//    for (int i = 0; i < 4; i++) {
//        Text t(font);
//        t.setString((i == selectedAction ? "> " : "  ") + actions[i]);
//        t.setCharacterSize(18);
//        t.setFillColor(i == selectedAction ? Color(0, 255, 200) : Color(220, 220, 255));
//        t.setPosition({ 28.f, vh - 120.f + static_cast<float>(i) * 28.f });
//        gw.getWindow().draw(t);
//    }
//}
//
//// ─────────────────────────────────────────────────────────────────────────────
//// Battle log  (drawn in Layer 9)
//// ─────────────────────────────────────────────────────────────────────────────
//
//void Battle::drawBattleLog() {
//    const float vw = static_cast<float>(gw.getWidth());
//    const float vh = static_cast<float>(gw.getHeight());
//    for (int i = 0; i < static_cast<int>(battleLog.size()); i++) {
//        Text line(font);
//        line.setString(battleLog[i]);
//        line.setCharacterSize(13);
//        line.setFillColor(Color(200, 200, 240));
//        line.setPosition({ 320.f, vh * 0.35f + static_cast<float>(i) * 20.f });
//        gw.getWindow().draw(line);
//    }
//}
//
//// ─────────────────────────────────────────────────────────────────────────────
//// Combat logic  (unchanged)
//// ─────────────────────────────────────────────────────────────────────────────
//
//void Battle::enemyTurn() {
//    PhantomWraith* wraith = dynamic_cast<PhantomWraith*>(&enemy);
//    if (wraith && wraith->stealsItems() && player.getInventory().getCount() > 0) {
//        player.getInventory().useItem(0);
//        addLog("Phantom Wraith STOLE an item!");
//        return;
//    }
//    int dmg = enemy.attackPlayer();
//    player.takeDamage(dmg);
//    addLog(enemy.getName() + " attacks for " + to_string(dmg) + " dmg!");
//}
//
//int Battle::playerTurn(BattleAction action) {
//    switch (action) {
//    case BattleAction::Attack: {
//        int dmg = player.attackEnemy();
//        enemy.takeDamage(dmg);
//        addLog("You attack for " + to_string(dmg) + " dmg!");
//        return dmg;
//    }
//    case BattleAction::UseAbility: {
//        if (player.getAbilities().empty()) {
//            addLog("No abilities!");
//            return 0;
//        }
//        int dmg = player.useAbility(0);
//        if (dmg < 0) {
//            addLog("Not enough Spirit Energy!");
//            return 0;
//        }
//        enemy.takeDamage(dmg);
//        addLog(player.getAbilities()[0].getName() + " hits for " + to_string(dmg) + "!");
//        return dmg;
//    }
//    case BattleAction::UseItem: {
//        if (player.getInventory().getCount() == 0) {
//            addLog("No items!");
//            return 0;
//        }
//        int healed = player.useItemFromInventory(0);
//        addLog("Used item! Healed " + to_string(healed) + " HP.");
//        return 0;
//    }
//    case BattleAction::Exit:
//        addLog("You exit the battle.");
//        return -1;
//    default:
//        return 0;
//    }
//}
//
//// ─────────────────────────────────────────────────────────────────────────────
//// Main loop  (unchanged logic)
//// ─────────────────────────────────────────────────────────────────────────────
//
//BattleResult Battle::run() {
//    Clock inputCooldown;
//    while (gw.isOpen()) {
//        while (auto event = gw.pollEvent()) {
//            if (event->is<Event::Closed>()) {
//                gw.getWindow().close();
//                return BattleResult::PlayerDied;
//            }
//            if (auto* key = event->getIf<Event::KeyPressed>()) {
//                if (inputCooldown.getElapsedTime().asMilliseconds() > 200) {
//                    inputCooldown.restart();
//
//                    if (key->code == Keyboard::Key::Up)
//                        selectedAction = (selectedAction - 1 + 4) % 4;
//                    if (key->code == Keyboard::Key::Down)
//                        selectedAction = (selectedAction + 1) % 4;
//
//                    if (key->code == Keyboard::Key::Enter) {
//                        int result = playerTurn(
//                            static_cast<BattleAction>(selectedAction));
//
//                        if (result == -1)
//                            return BattleResult::PlayerExited;
//
//                        if (!enemy.isAlive()) {
//                            addLog(enemy.getName() + " defeated! +"
//                                + to_string(enemy.getXPReward()) + " XP");
//                            player.gainXP(enemy.getXPReward());
//                            drawBattleUI();
//                            sleep(seconds(2.f));
//                            return BattleResult::PlayerWon;
//                        }
//
//                        if (enemy.getIsBoss()
//                            && enemy.hasReachedHalfHealth()
//                            && !bossHalfHealthTriggered) {
//                            bossHalfHealthTriggered = true;
//                            enemy.onHalfHealth();
//                            addLog("!! " + enemy.getName() + " TRANSFORMS !!");
//                        }
//
//                        enemyTurn();
//
//                        if (!player.isAlive()) {
//                            addLog("You have fallen...");
//                            drawBattleUI();
//                            sleep(seconds(2.f));
//                            return BattleResult::PlayerDied;
//                        }
//                    }
//                }
//            }
//        }
//        drawBattleUI();
//    }
//    return BattleResult::PlayerDied;
//}



//------------------------------------------------------------------------------------------
// Battle.cpp
// Real-time battle system.
// FIX 1: Removed duplicate player.heal() call — useItemFromInventory already heals.
// FIX 2: Removed dead inner-if in enemy AI damage — damage applied unconditionally.
// FIX 3: Cursor triangle added next to selected action button.
// FIX 4: HUD bars properly aligned — player top-left, enemy top-right.
// FIX 5: Action menu is bottom-center, left to right, no '>' character.
// FIX 6: Enemy sprite positioned bottom-right, player sprite bottom-left.

#include "Battle.h"
#include "AssetLoader.h"
#include <sstream>
#include <algorithm>

using namespace std;
using namespace sf;

// ─────────────────────────────────────────────────────────────────────────────
// CONSTRUCTOR
// ─────────────────────────────────────────────────────────────────────────────
Battle::Battle(GameWindow& window, Veilwalker& p, Spirit& e,
    AudioManager& audioRef, Sprite* bgSpr)
    : gw(window), player(p), enemy(e), audio(audioRef), bgSprite(bgSpr),
    buttonLabels{ Text(font), Text(font), Text(font), Text(font) }
{
    // Load font with fallback.
    AssetLoader::openFontWithFallback(font, {
        "assets/font/ActionSpectral.ttf",
        "assets/font/Philosopher-Bold.ttf"
        });

    // Initial feet positions on the ground line.
    playerPos = { 150.f, groundY };
    enemyPos = { static_cast<float>(gw.getWidth()) - 180.f, groundY };
    enemyFacingLeft = true;

    // Build the four action buttons across the bottom of the screen.
    const float vw = static_cast<float>(gw.getWidth());
    const float vh = static_cast<float>(gw.getHeight());
    const string actions[4] = { "ATTACK", "ABILITY", "ITEM", "EXIT" };

    // Total width of all buttons: 4 * 120 + 3 * 20 gaps = 540. Centered.
    const float btnW = 120.f;
    const float btnH = 38.f;
    const float gap = 20.f;
    const float totalW = 4.f * btnW + 3.f * gap;
    const float startX = (vw - totalW) / 2.f;
    const float btnY = vh - 60.f;

    for (int i = 0; i < 4; i++)
    {
        menuButtons[i].setSize({ btnW, btnH });
        menuButtons[i].setPosition({ startX + i * (btnW + gap), btnY });
        menuButtons[i].setFillColor(Color(40, 40, 90));
        menuButtons[i].setOutlineColor(Color(100, 100, 200));
        menuButtons[i].setOutlineThickness(1.f);

        buttonLabels[i].setFont(font);
        buttonLabels[i].setString(actions[i]);
        buttonLabels[i].setCharacterSize(16);
        FloatRect lb = buttonLabels[i].getLocalBounds();
        buttonLabels[i].setOrigin({ lb.size.x / 2.f, lb.size.y / 2.f });
        buttonLabels[i].setPosition({ menuButtons[i].getPosition().x + btnW / 2.f,
                                      btnY + btnH / 2.f });
    }

    // Build the cursor triangle.
    buildCursor();

    addLog("A wild " + enemy.getName() + " appears!");
}

// ─────────────────────────────────────────────────────────────────────────────
// CURSOR TRIANGLE (points right, like a menu arrow)
// ─────────────────────────────────────────────────────────────────────────────
void Battle::buildCursor()
{
    cursor.setPointCount(3);
    cursor.setPoint(0, { 0.f,  0.f });
    cursor.setPoint(1, { 0.f, 12.f });
    cursor.setPoint(2, { 10.f, 6.f });
    cursor.setFillColor(Color(255, 220, 60));
}

// ─────────────────────────────────────────────────────────────────────────────
// BATTLE LOG
// ─────────────────────────────────────────────────────────────────────────────
void Battle::addLog(const string& msg)
{
    string upper;
    for (char c : msg)
        upper.push_back(static_cast<char>(toupper(static_cast<unsigned char>(c))));
    battleLog.push_back(upper);
    if (battleLog.size() > 5)
        battleLog.erase(battleLog.begin());
}

// ─────────────────────────────────────────────────────────────────────────────
// INPUT: arrow keys move player, Left/Right select action, Enter confirms.
// Mouse hover highlights the button under the cursor.
// ─────────────────────────────────────────────────────────────────────────────
void Battle::handleInput(float dt)
{
    // ── Player movement ───────────────────────────────────────────────────────
    bool movingRight = Keyboard::isKeyPressed(Keyboard::Key::Right);
    bool movingLeft = Keyboard::isKeyPressed(Keyboard::Key::Left);

    if (!showAbilityMenu)
    {
        // Left/Right keys also navigate action menu when not moving.
        // We detect a tap (not hold) via a simple cooldown.
        static Clock menuNavClock;
        if (menuNavClock.getElapsedTime().asMilliseconds() > 180)
        {
            if (movingRight) { selectedAction = (selectedAction + 1) % 4; menuNavClock.restart(); }
            if (movingLeft) { selectedAction = (selectedAction + 3) % 4; menuNavClock.restart(); }
        }
    }

    // Up = jump.
    if (Keyboard::isKeyPressed(Keyboard::Key::Up) && !playerJumping)
    {
        playerVelY = -500.f;
        playerJumping = true;
    }

    // Update facing direction based on enemy position.
    bool facingRight = (enemyPos.x > playerPos.x);
    player.getRenderer().setFacingRight(facingRight);

    // ── Mouse: hover highlights button ───────────────────────────────────────
    Vector2i mousePos = Mouse::getPosition(gw.getWindow());
    Vector2f mouseWorld = gw.getWindow().mapPixelToCoords(mousePos);

    if (!showAbilityMenu)
    {
        for (int i = 0; i < 4; i++)
        {
            if (menuButtons[i].getGlobalBounds().contains(mouseWorld))
            {
                selectedAction = i;
                // Left click confirms.
                if (Mouse::isButtonPressed(Mouse::Button::Left) && playerCanAct)
                {
                    static Clock clickClock;
                    if (clickClock.getElapsedTime().asMilliseconds() > 300)
                    {
                        clickClock.restart();
                        audio.playOptional("Click", 70.f);
                        switch (i)
                        {
                        case 0: handlePlayerAttack();  break;
                        case 1: handlePlayerAbility(); break;
                        case 2: handlePlayerItem();    break;
                        case 3: handleExit();          break;
                        }
                    }
                }
                break;
            }
        }
    }
    else
    {
        // Ability sub-menu mouse support.
        for (size_t i = 0; i < abilityTexts.size(); i++)
        {
            if (abilityTexts[i].getGlobalBounds().contains(mouseWorld))
            {
                selectedAbilityIndex = static_cast<int>(i);
                if (Mouse::isButtonPressed(Mouse::Button::Left))
                {
                    const auto& ab = player.getAbilities();
                    if (player.getSpiritEnergy() >= ab[i].getEnergyCost())
                    {
                        int dmg = player.useAbility(static_cast<int>(i));
                        applyDamageToEnemy(dmg);
                        addLog(ab[i].getName() + " hits for " + to_string(dmg) + "!");
                        player.getRenderer().setState(CharacterState::Ability);
                        audio.playOptional("ability", 70.f);
                    }
                    else addLog("Not enough Spirit Energy!");
                    showAbilityMenu = false;
                }
                break;
            }
        }
    }

    // ── Enter key confirms selected action ───────────────────────────────────
    static Clock enterClock;
    if (Keyboard::isKeyPressed(Keyboard::Key::Enter) && playerCanAct)
    {
        if (enterClock.getElapsedTime().asMilliseconds() > 300)
        {
            enterClock.restart();
            audio.playOptional("Click", 70.f);

            if (!showAbilityMenu)
            {
                switch (selectedAction)
                {
                case 0: handlePlayerAttack();  break;
                case 1: handlePlayerAbility(); break;
                case 2: handlePlayerItem();    break;
                case 3: handleExit();          break;
                }
            }
            else
            {
                const auto& ab = player.getAbilities();
                if (selectedAbilityIndex < static_cast<int>(ab.size()))
                {
                    if (player.getSpiritEnergy() >= ab[selectedAbilityIndex].getEnergyCost())
                    {
                        int dmg = player.useAbility(selectedAbilityIndex);
                        applyDamageToEnemy(dmg);
                        addLog(ab[selectedAbilityIndex].getName() +
                            " hits for " + to_string(dmg) + "!");
                        player.getRenderer().setState(CharacterState::Ability);
                        audio.playOptional("ability", 70.f);
                    }
                    else addLog("Not enough Spirit Energy!");
                }
                showAbilityMenu = false;
            }
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// COMBAT ACTIONS
// ─────────────────────────────────────────────────────────────────────────────

void Battle::handlePlayerAttack()
{
    if (playerAttackClock.getElapsedTime().asSeconds() < playerAttackCooldown)
        return;
    playerAttackClock.restart();
    player.getRenderer().setState(CharacterState::Attack);
    int dmg = player.attackEnemy();
    applyDamageToEnemy(dmg);
    addLog("You attack for " + to_string(dmg) + " dmg!");
    audio.playOptional("attack", 70.f);
}

void Battle::handlePlayerAbility()
{
    const auto& ab = player.getAbilities();
    if (ab.empty()) { addLog("No abilities!"); return; }

    // Toggle ability sub-menu.
    if (!showAbilityMenu)
    {
        showAbilityMenu = true;
        selectedAbilityIndex = 0;
        abilityTexts.clear();
        const float vw = static_cast<float>(gw.getWidth());
        for (size_t i = 0; i < ab.size(); i++)
        {
            Text t(font);
            t.setString(ab[i].getName() + "  (SP:" + to_string(ab[i].getEnergyCost()) + ")");
            t.setCharacterSize(16);
            t.setFillColor(Color(220, 200, 255));
            t.setPosition({ vw / 2.f - 150.f, 300.f + static_cast<float>(i) * 32.f });
            abilityTexts.push_back(t);
        }
    }
}

void Battle::handlePlayerItem()
{
    if (player.getInventory().getCount() == 0) { addLog("No items!"); return; }

    // FIX: useItemFromInventory already calls heal() internally.
    // Do NOT call player.heal() again here — that would double-heal.
    int healed = player.useItemFromInventory(0);
    addLog("Used item! Healed " + to_string(healed) + " HP.");
    player.getRenderer().setState(CharacterState::Item);
    audio.playOptional("heal", 70.f);
}

void Battle::handleExit()
{
    addLog("You exit the battle.");
    playerCanAct = false;
}

// ─────────────────────────────────────────────────────────────────────────────
// PHYSICS (gravity + floor collision)
// ─────────────────────────────────────────────────────────────────────────────
void Battle::updatePhysics(float dt)
{
    if (playerJumping)
    {
        playerVelY += gravity * dt;
        playerPos.y += playerVelY * dt;
        if (playerPos.y >= groundY)
        {
            playerPos.y = groundY;
            playerVelY = 0.f;
            playerJumping = false;
            player.getRenderer().setState(CharacterState::Idle);
        }
    }

    // Clamp player inside screen width.
    playerPos.x = clamp(playerPos.x, 30.f, static_cast<float>(gw.getWidth()) - 30.f);
}

// ─────────────────────────────────────────────────────────────────────────────
// ENEMY AI
// ─────────────────────────────────────────────────────────────────────────────
void Battle::updateEnemyAI(float dt)
{
    float dx = playerPos.x - enemyPos.x;
    enemyFacingLeft = (dx < 0.f);
    enemy.getRenderer().setFacingRight(!enemyFacingLeft);

    // Walk toward player if not within melee range and not mid-attack.
    if (abs(dx) > 70.f && !enemyAttacking)
    {
        enemyPos.x += (dx > 0.f ? 1.f : -1.f) * enemySpeed * dt;
        enemy.getRenderer().setState(CharacterState::Walk);
    }
    else if (!enemyAttacking)
    {
        enemy.getRenderer().setState(CharacterState::Idle);
    }

    // Start an attack when cooldown is done and player is close.
    if (!enemyAttacking
        && enemyAttackClock.getElapsedTime().asSeconds() > enemyAttackCooldown
        && abs(dx) < 120.f)
    {
        enemyAttacking = true;
        enemyAttackClock.restart();
        enemy.getRenderer().setState(CharacterState::Attack);
        audio.playOptional("hit", 60.f);
    }

    // End of attack animation: apply damage unconditionally.
    // FIX: removed dead inner-if (was always true at this point anyway).
    if (enemyAttacking
        && enemyAttackClock.getElapsedTime().asSeconds() > enemyAttackDuration)
    {
        enemyAttacking = false;
        int dmg = enemy.attackPlayer();
        applyDamageToPlayer(dmg);
        addLog(enemy.getName() + " attacks for " + to_string(dmg) + " dmg!");
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// DAMAGE HELPERS
// ─────────────────────────────────────────────────────────────────────────────
void Battle::applyDamageToPlayer(int dmg)
{
    player.takeDamage(dmg);
    player.getRenderer().setState(CharacterState::Block);
}

void Battle::applyDamageToEnemy(int dmg)
{
    enemy.takeDamage(dmg);
    enemy.getRenderer().setState(CharacterState::Block);
}

// ─────────────────────────────────────────────────────────────────────────────
// ANIMATION UPDATE
// ─────────────────────────────────────────────────────────────────────────────
void Battle::updateAnimations(float dt)
{
    player.getRenderer().update(dt);
    enemy.getRenderer().update(dt);
}

// ─────────────────────────────────────────────────────────────────────────────
// HUD DRAWING
// Player bars: top-left  — sky blue / purple palette.
// Enemy bars : top-right — black / gray / red palette.
// ─────────────────────────────────────────────────────────────────────────────
void Battle::drawGradientBar(float x, float y, float w, float h,
    int current, int max,
    Color left, Color right)
{
    // Dark trough.
    RectangleShape bg({ w, h });
    bg.setPosition({ x, y });
    bg.setFillColor(Color(20, 20, 30));
    gw.getWindow().draw(bg);

    if (max <= 0) return;
    float ratio = clamp(static_cast<float>(current) / static_cast<float>(max), 0.f, 1.f);
    float fw = w * ratio;
    if (fw < 1.f) return;

    // Two-color gradient fill using a TriangleStrip.
    VertexArray strip(PrimitiveType::TriangleStrip, 4);
    strip[0].position = { x,      y }; strip[0].color = left;
    strip[1].position = { x + fw, y }; strip[1].color = right;
    strip[2].position = { x,      y + h }; strip[2].color = left;
    strip[3].position = { x + fw, y + h }; strip[3].color = right;
    gw.getWindow().draw(strip);
}

void Battle::drawHUD()
{
    const float vw = static_cast<float>(gw.getWidth());

    // ── Player panel (top-left) ───────────────────────────────────────────────
    const float pX = 16.f;

    // Name and level.
    Text pName(font, player.getName() + "  Lv." + to_string(player.getLevel()), 17);
    pName.setFillColor(Color(180, 220, 255));
    pName.setPosition({ pX, 14.f });
    gw.getWindow().draw(pName);

    // HP bar: sky blue -> purple.
    drawGradientBar(pX, 40.f, 220.f, 16.f,
        player.getHp(), player.getMaxHp(),
        Color(30, 150, 220), Color(140, 60, 200));
    Text hpTxt(font, "HP " + to_string(player.getHp()) + "/" + to_string(player.getMaxHp()), 12);
    hpTxt.setFillColor(Color(220, 240, 255));
    hpTxt.setPosition({ pX + 2.f, 58.f });
    gw.getWindow().draw(hpTxt);

    // SP bar: purple -> violet.
    drawGradientBar(pX, 76.f, 200.f, 12.f,
        player.getSpiritEnergy(), player.getMaxEnergy(),
        Color(100, 30, 180), Color(200, 80, 255));
    Text spTxt(font, "SP " + to_string(player.getSpiritEnergy()) + "/" + to_string(player.getMaxEnergy()), 11);
    spTxt.setFillColor(Color(200, 180, 255));
    spTxt.setPosition({ pX + 2.f, 90.f });
    gw.getWindow().draw(spTxt);

    // XP bar: cyan -> gold.
    drawGradientBar(pX, 108.f, 180.f, 10.f,
        player.getXP(), player.getXPToNext(),
        Color(0, 180, 220), Color(255, 200, 40));
    Text xpTxt(font, "XP " + to_string(player.getXP()) + "/" + to_string(player.getXPToNext()), 11);
    xpTxt.setFillColor(Color(200, 240, 200));
    xpTxt.setPosition({ pX + 2.f, 120.f });
    gw.getWindow().draw(xpTxt);

    // Coins.
    Text coinTxt(font, "Coins: " + to_string(player.getCoins()), 12);
    coinTxt.setFillColor(Color(255, 220, 80));
    coinTxt.setPosition({ pX, 136.f });
    gw.getWindow().draw(coinTxt);

    // ── Enemy panel (top-right) ───────────────────────────────────────────────
    const float eBarW = 220.f;
    const float eX = vw - eBarW - 16.f;

    Text eName(font, enemy.getName(), 17);
    eName.setFillColor(Color(255, 100, 80));
    // Right-align the name.
    FloatRect enb = eName.getLocalBounds();
    eName.setOrigin({ enb.size.x, 0.f });
    eName.setPosition({ vw - 16.f, 14.f });
    gw.getWindow().draw(eName);

    // HP bar: dark gray -> red.
    drawGradientBar(eX, 40.f, eBarW, 16.f,
        enemy.getHp(), enemy.getMaxHp(),
        Color(60, 60, 60), Color(200, 30, 30));
    Text eHpTxt(font, "HP " + to_string(enemy.getHp()) + "/" + to_string(enemy.getMaxHp()), 12);
    eHpTxt.setFillColor(Color(240, 200, 200));
    eHpTxt.setPosition({ eX + 2.f, 58.f });
    gw.getWindow().draw(eHpTxt);
}

// ─────────────────────────────────────────────────────────────────────────────
// ACTION MENU (bottom, left to right)
// Highlighted button gets a bright border. Cursor arrow beside the label.
// ─────────────────────────────────────────────────────────────────────────────
void Battle::drawActionMenu()
{
    for (int i = 0; i < 4; i++)
    {
        // Highlighted = brighter fill and yellow border.
        bool sel = (i == selectedAction);
        menuButtons[i].setFillColor(sel ? Color(70, 70, 160) : Color(30, 30, 80));
        menuButtons[i].setOutlineColor(sel ? Color(255, 220, 60) : Color(80, 80, 160));
        menuButtons[i].setOutlineThickness(sel ? 2.f : 1.f);
        gw.getWindow().draw(menuButtons[i]);

        buttonLabels[i].setFillColor(sel ? Color(255, 255, 100) : Color(200, 200, 240));
        gw.getWindow().draw(buttonLabels[i]);

        // Draw cursor triangle to the left of selected button label.
        if (sel)
        {
            Vector2f labelPos = buttonLabels[i].getPosition();
            FloatRect lb = buttonLabels[i].getLocalBounds();
            float cursorX = menuButtons[i].getPosition().x + 6.f;
            float cursorY = labelPos.y - lb.size.y / 2.f;
            cursor.setPosition({ cursorX, cursorY });
            gw.getWindow().draw(cursor);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// ABILITY SUB-MENU
// ─────────────────────────────────────────────────────────────────────────────
void Battle::drawAbilityMenu()
{
    // Semi-transparent panel behind the list.
    const float vw = static_cast<float>(gw.getWidth());
    RectangleShape panel({ 340.f, 30.f + static_cast<float>(abilityTexts.size()) * 34.f });
    panel.setPosition({ vw / 2.f - 170.f, 288.f });
    panel.setFillColor(Color(20, 20, 60, 200));
    panel.setOutlineColor(Color(100, 100, 200));
    panel.setOutlineThickness(1.f);
    gw.getWindow().draw(panel);

    for (size_t i = 0; i < abilityTexts.size(); i++)
    {
        bool sel = (static_cast<int>(i) == selectedAbilityIndex);
        abilityTexts[i].setFillColor(sel ? Color(255, 255, 100) : Color(200, 180, 255));
        gw.getWindow().draw(abilityTexts[i]);

        if (sel)
        {
            Vector2f pos = abilityTexts[i].getPosition();
            cursor.setPosition({ pos.x - 18.f, pos.y + 2.f });
            gw.getWindow().draw(cursor);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// BATTLE LOG (center area)
// ─────────────────────────────────────────────────────────────────────────────
void Battle::drawBattleLog()
{
    const float vw = static_cast<float>(gw.getWidth());
    const float startY = 170.f;
    for (size_t i = 0; i < battleLog.size(); i++)
    {
        Text line(font, battleLog[i], 13);
        line.setFillColor(Color(180, 180, 230));
        FloatRect lb = line.getLocalBounds();
        line.setOrigin({ lb.size.x / 2.f, 0.f });
        line.setPosition({ vw / 2.f, startY + static_cast<float>(i) * 22.f });
        gw.getWindow().draw(line);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// SPRITE DRAWING
// Player: bottom-left area. Enemy: bottom-right area.
// ─────────────────────────────────────────────────────────────────────────────
void Battle::drawPlayerSprite()
{
    player.getRenderer().setPosition(playerPos);
    player.getRenderer().draw(gw.getWindow());
}

void Battle::drawEnemySprite()
{
    enemy.getRenderer().setPosition(enemyPos);
    enemy.getRenderer().draw(gw.getWindow());
}

// ─────────────────────────────────────────────────────────────────────────────
// MAIN LOOP
// ─────────────────────────────────────────────────────────────────────────────
BattleResult Battle::run()
{
    Clock dtClock;

    while (gw.isOpen())
    {
        float dt = dtClock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f;  // cap delta time (prevents spiral of death)

        // ── Events ──────────────────────────────────────────────────────────
        while (auto ev = gw.pollEvent())
            if (ev->is<Event::Closed>())
                gw.getWindow().close();

        // Player chose to exit — return on next frame after logging.
        if (!playerCanAct) return BattleResult::PlayerExited;

        // ── Update ──────────────────────────────────────────────────────────
        handleInput(dt);
        updatePhysics(dt);
        updateEnemyAI(dt);
        updateAnimations(dt);

        // ── Win / lose checks ────────────────────────────────────────────────
        if (!player.isAlive())
        {
            player.getRenderer().setState(CharacterState::Dying);
            addLog("You have fallen...");
            return BattleResult::PlayerDied;
        }

        if (!enemy.isAlive())
        {
            enemy.getRenderer().setState(CharacterState::Dying);
            addLog(enemy.getName() + " defeated!  +" + to_string(enemy.getXPReward()) + " XP");
            player.gainXP(enemy.getXPReward());
            // Draw one last frame so player sees the result.
            gw.clear();
            if (bgSprite) gw.getWindow().draw(*bgSprite);
            drawPlayerSprite();
            drawEnemySprite();
            drawHUD();
            drawBattleLog();
            gw.display();
            sf::sleep(sf::seconds(1.2f));
            return BattleResult::PlayerWon;
        }

        // Boss phase trigger at half health.
        if (enemy.getIsBoss() && enemy.hasReachedHalfHealth() && !bossPhaseTriggered)
        {
            bossPhaseTriggered = true;
            enemy.onHalfHealth();
            addLog("!! " + enemy.getName() + " TRANSFORMS !!");
        }

        // ── Draw ─────────────────────────────────────────────────────────────
        gw.clear();

        // Background.
        if (bgSprite)
        {
            gw.getWindow().draw(*bgSprite);
        }
        else
        {
            RectangleShape fallback(
                { static_cast<float>(gw.getWidth()), static_cast<float>(gw.getHeight()) });
            fallback.setFillColor(Color(18, 12, 36));
            gw.getWindow().draw(fallback);
        }

        // Characters.
        drawPlayerSprite();
        drawEnemySprite();

        // UI layers.
        drawHUD();
        drawActionMenu();
        if (showAbilityMenu) drawAbilityMenu();
        drawBattleLog();

        gw.display();
    }

    return BattleResult::PlayerDied;
}