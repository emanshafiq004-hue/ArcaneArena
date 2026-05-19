//------------------------------------------------ ( Battle.cpp ) -------------------------------------------------------------------------------
// Real-time battle system...
#include "Battle.h"
#include "AssetLoader.h"

using namespace std;
using namespace sf;

//-------------------------------------------------------------------------------
// CONSTRUCTOR:-
//-------------------------------------------------------------------------------
Battle::Battle(GameWindow& window, Veilwalker& p, Spirit& e,AudioManager& audioRef, Sprite* bgSpr): gw(window), player(p), enemy(e), audio(audioRef), bgSprite(bgSpr),buttonLabels{ Text(font), Text(font), Text(font), Text(font) }
{
AssetLoader::openFontWithFallback(font, {"assets/font/ActionSpectral.ttf","assets/font/Philosopher-Bold.ttf"});
// Initial feet positions on the ground line...
playerPos = { 150.f, groundY };
enemyPos = { static_cast<float>(gw.getWidth()) - 180.f, groundY };
enemyFacingLeft = true;
// Build the four action buttons across the bottom of the screen.
const float vw = static_cast<float>(gw.getWidth());
const float vh = static_cast<float>(gw.getHeight());
const string actions[4] = { "ATTACK", "ABILITY", "ITEM", "EXIT" };
// Total width of all buttons: 4 * 120 + 3 * 20 gaps = 540. Centered.
const float btnW = 120.f;
const float btnH = 30.f;
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
buttonLabels[i].setCharacterSize(15);
buttonLabels[i].setStyle(Text::Bold);
FloatRect lb = buttonLabels[i].getLocalBounds();
buttonLabels[i].setOrigin({ lb.size.x / 2.f, lb.size.y / 2.f });
buttonLabels[i].setPosition({ menuButtons[i].getPosition().x + btnW / 2.f,btnY + btnH / 2.f });
}
addLog("A wild " + enemy.getName() + " appears!");
}
//-------------------------------------------------------------------------------
// BATTLE LOG
//-------------------------------------------------------------------------------
void Battle::addLog(const string& msg)
{
string upper;
for (char c : msg)
upper.push_back(static_cast<char>(toupper(static_cast<unsigned char>(c))));
battleLog.push_back(upper);
if (battleLog.size() > 5)
battleLog.erase(battleLog.begin());
}

//--------------------------------------------------------------------------------
// INPUT:-
//--------------------------------------------------------------------------------
void Battle::handleInput(float dt)
{
    //Player movement...
    bool movingRight = Keyboard::isKeyPressed(Keyboard::Key::Right);
    bool movingLeft = Keyboard::isKeyPressed(Keyboard::Key::Left);
    bool jumping = Keyboard::isKeyPressed(Keyboard::Key::Up);
    if (movingRight || movingLeft)
    {
        float moveSpeed = 200.f;
        playerPos.x += (movingRight ? moveSpeed : -moveSpeed) * dt;
        player.getRenderer().setState(CharacterState::Walk);
        player.getRenderer().setFacingRight(movingRight);
    }
    else if (!playerJumping)
    {
        player.getRenderer().setState(CharacterState::Idle);
        player.getRenderer().setFacingRight(playerPos.x < enemyPos.x);
    }
    if (jumping && !playerJumping)
    {
        playerVelY = -500.f;
        playerJumping = true;
    }
    Vector2i mousePos = Mouse::getPosition(gw.getWindow());
    Vector2f mouseWorld = gw.getWindow().mapPixelToCoords(mousePos);
    bool buttonHovered = false;
    bool abilityHovered = false;
    static Clock actionClock;
    bool confirmAction = Mouse::isButtonPressed(Mouse::Button::Left);
    for (int i = 0; i < 4; i++)
    {
        if (menuButtons[i].getGlobalBounds().contains(mouseWorld))
        {
            buttonHovered = true;
            selectedAction = i;
            if (confirmAction && actionClock.getElapsedTime().asMilliseconds() > 250 && playerCanAct)
            {
                actionClock.restart();
                switch (i)
                {
                case 0: handlePlayerAttack();  break;
                case 1: handlePlayerAbility(); break;
                case 2: handlePlayerItem();    break;
                case 3: handleExit();          break;
                }
            }
            break;
        }
    }
    if (showAbilityMenu)
    {
        for (size_t i = 0; i < abilityTexts.size(); i++)
        {
            if (abilityTexts[i].getGlobalBounds().contains(mouseWorld))
            {
                abilityHovered = true;
                selectedAbilityIndex = static_cast<int>(i);
                if (confirmAction && actionClock.getElapsedTime().asMilliseconds() > 250)
                {
                    actionClock.restart();
                    const auto& ab = player.getAbilities();
                    if (player.getSpiritEnergy() >= ab[i].getEnergyCost())
                    {
                        int dmg = player.useAbility(static_cast<int>(i));
                        applyDamageToEnemy(dmg);
                        addLog(ab[i].getName() + " hits for " + to_string(dmg) + "!");
                        player.getRenderer().setState(CharacterState::Ability);
                        audio.playOptional("ability", 60.f);
                    }
                    else addLog("Not enough Spirit Energy!");
                        showAbilityMenu = false;
                }
                break;
            }
        }
    }
}

//-------------------------------------------------------------------------------
// COMBAT ACTIONS:-
//-------------------------------------------------------------------------------

void Battle::handlePlayerAttack()
{
if (playerAttackClock.getElapsedTime().asSeconds() < playerAttackCooldown)
return;
 player.getRenderer().setPosition(playerPos);
enemy.getRenderer().setPosition(enemyPos);
if (!player.getRenderer().getAttackBox().findIntersection(enemy.getRenderer().getGlobalBounds()).has_value())
{
addLog("Too far to attack!");
return;
}

    playerAttackClock.restart();
    player.getRenderer().setState(CharacterState::Attack);
    int dmg = player.attackEnemy();
    applyDamageToEnemy(dmg);
    addLog("You attack for " + to_string(dmg) + " dmg!");
    audio.playOptional("attack", 60.f);
}

void Battle::handlePlayerAbility()
{
    const auto& ab = player.getAbilities();
    if (ab.empty()) { addLog("No abilities!"); return; }
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
            t.setCharacterSize(12);
            t.setFillColor(Color(220, 200, 255));
            t.setPosition({ vw / 2.f - 130.f, 50.f + static_cast<float>(i) * 32.f });
            abilityTexts.push_back(t);
        }
    }
}

void Battle::handlePlayerItem()
{
    if (player.getInventory().getCount() == 0) { addLog("No items!"); return; }
    int healed = player.useItemFromInventory(0);
    addLog("Used item! Healed " + to_string(healed) + " HP.");
    player.getRenderer().setState(CharacterState::Item);
    audio.playOptional("heal", 60.f);
}

void Battle::handleExit()
{
    addLog("You exit the battle.");
    playerCanAct = false;
}

//----------------------------------------------------------------------------------------
// PHYSICS (gravity + floor collision):-
//----------------------------------------------------------------------------------------
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

    playerPos.x = clamp(playerPos.x, 30.f, static_cast<float>(gw.getWidth()) - 30.f);
    playerPos.y = clamp(playerPos.y, 50.f, static_cast<float>(gw.getHeight()) - 50.f);

    // Only block horizontal passing when grounded
    if (!playerJumping)
    {
        const float minSeparation = 70.f;
        float dx = playerPos.x - enemyPos.x;
        if (abs(dx) < minSeparation)
        {
            if (dx <= 0.f)
                playerPos.x = enemyPos.x - minSeparation;
            else
                playerPos.x = enemyPos.x + minSeparation;
        }
    }
}
//----------------------------------------------------------------------------
// ENEMY AI:-
// ---------------------------------------------------------------------------
void Battle::updateEnemyAI(float dt)
{
    float dx = playerPos.x - enemyPos.x;
    enemyFacingLeft = (dx < 0.f);
    enemy.getRenderer().setFacingRight(!enemyFacingLeft);

    player.getRenderer().setPosition(playerPos);
    enemy.getRenderer().setPosition(enemyPos);
    bool enemyInRange = enemy.getRenderer().getAttackBox().findIntersection(player.getRenderer().getGlobalBounds()).has_value();

    // Walk toward player if outside melee range and not mid-attack.
    if (abs(dx) > 80.f && !enemyAttacking)
    {
        enemyPos.x += (dx > 0.f ? 1.f : -1.f) * enemySpeed * dt;
        enemy.getRenderer().setState(CharacterState::Walk);
    }
    else if (!enemyAttacking)
    {
        enemy.getRenderer().setState(CharacterState::Idle);
    }

    // Start an attack when cooldown is done and player is within the enemy's melee reach.
    if (!enemyAttacking
        && enemyAttackClock.getElapsedTime().asSeconds() > enemyAttackCooldown
        && enemyInRange)
    {
        enemyAttacking = true;
        enemyAttackClock.restart();
        enemy.getRenderer().setState(CharacterState::Attack);
        audio.playOptional("hit", 60.f);
    }

    if (enemyAttacking
        && enemyAttackClock.getElapsedTime().asSeconds() > enemyAttackDuration)
    {
        enemyAttacking = false;
        int dmg = enemy.attackPlayer();
        applyDamageToPlayer(dmg);
        addLog(enemy.getName() + " attacks for " + to_string(dmg) + " dmg!");
    }
}


// DAMAGE HELPERS

void Battle::applyDamageToPlayer(int dmg)
{
    player.takeDamage(dmg);
    player.getRenderer().setState(CharacterState::Block);
}

void Battle::applyDamageToEnemy(int dmg)
{
    enemy.takeDamage(dmg);
    enemy.getRenderer().setState(CharacterState::Attack);
}

// ANIMATION UPDATE

void Battle::updateAnimations(float dt)
{
    player.getRenderer().update(dt);
    enemy.getRenderer().update(dt);
}


// HUD DRAWING

void Battle::drawGradientBar(float x, float y, float w, float h,int current, int max,Color left, Color right)
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

    // Player panel (top-left) 
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

    //  Enemy panel (top-right)
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

// ACTION MENU (bottom, left to right)

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
    }

    // Input hint for hover + confirm controls.
    const float vw = static_cast<float>(gw.getWidth());
    Text hint(font, "HOVER TO SELECT, LEFT CLICK TO CONFIRM", 14);
    hint.setFillColor(Color(180, 220, 255));
    FloatRect hintBounds = hint.getLocalBounds();
    hint.setOrigin({ hintBounds.size.x / 2.f, hintBounds.size.y / 2.f });
    hint.setPosition({ vw / 2.f, menuButtons[0].getPosition().y - 24.f });
    gw.getWindow().draw(hint);
}


// ABILITY SUB-MENU

void Battle::drawAbilityMenu()
{
    // Semi-transparent panel behind the list.
    const float vw = static_cast<float>(gw.getWidth());
    RectangleShape panel({ 280.f, 30.f + static_cast<float>(abilityTexts.size()) * 34.f });
    panel.setPosition({ vw / 2.f - 150.f, 30.f });
    panel.setFillColor(Color(20, 20, 60, 200));
    panel.setOutlineColor(Color(100, 100, 200));
    panel.setOutlineThickness(1.f);
    gw.getWindow().draw(panel);

    for (size_t i = 0; i < abilityTexts.size(); i++)
    {
        bool sel = (static_cast<int>(i) == selectedAbilityIndex);
        abilityTexts[i].setFillColor(sel ? Color(255, 255, 100) : Color(200, 180, 255));
        gw.getWindow().draw(abilityTexts[i]);
    }
}


// BATTLE LOG:-

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

// 
// SPRITE DRAWING
// Player: bottom-left area. Enemy: bottom-right area.

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


// MAIN LOOP:-

BattleResult Battle::run()
{
    Clock dtClock;

    while (gw.isOpen())
    {
        float dt = dtClock.restart().asSeconds();
        if (dt > 0.1f) 
            dt = 0.1f;

        //  Events 
        while (auto ev = gw.pollEvent())
            if (ev->is<Event::Closed>())
                gw.getWindow().close();

        // Player chose to exit — return on next frame after logging...
        if (!playerCanAct) return BattleResult::PlayerExited;

        // Update ..
        handleInput(dt);
        updatePhysics(dt);
        updateEnemyAI(dt);
        updateAnimations(dt);

        //  Win / lose checks..
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
            enemy.getRenderer().setState(CharacterState::ShapeShift);
        }

        // Draw 
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