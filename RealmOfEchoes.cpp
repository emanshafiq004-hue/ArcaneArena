// ------------------------------------( RealmOfEchoes.cpp )------------------------------------------------------------
// Core game logic for Realm of Echoes: Spirits & Shadows.

#include "RealmOfEchoes.h"
#include "FileManager.h"
#include "Spirits.h"
#include "AssetLoader.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <cctype>

using namespace std;
using namespace sf;

static const float kSurvivalSeconds = 30.f;

//-----------------------------------------------------------------------------------------------------------------------
// HELPERS:-
//-----------------------------------------------------------------------------------------------------------------------

//Centered X coordinate for text drawing...
float RealmOfEchoes::centerX()const
{
return static_cast<float>(gw.getWidth()) * 0.5f;
}

// Convert a string to uppercase....
string RealmOfEchoes::upperText(const string& s) const
{
string out;
out.reserve(s.size());
for (unsigned char c : s)
out.push_back(static_cast<char>(toupper(c)));
return out;
}

//----------------------------------------------------------------------------------------------------------------------
// AUDIO SETUP :-
//----------------------------------------------------------------------------------------------------------------------

void RealmOfEchoes::setupAudio()
{
// UI sounds....
audio.preloadOptional("ui", "assets/audio/ui_confirm.ogg");
audio.preloadOptional("ui_move", "assets/audio/ui_move.ogg");
audio.preloadOptional("Click", "assets/audio/Main_Confirm.ogg");
// Battle sounds....
audio.preloadOptional("attack", "assets/audio/attack.mp3");
audio.preloadOptional("hit", "assets/audio/battle_hit.ogg");
audio.preloadOptional("block", "assets/audio/battle_block.ogg");
audio.preloadOptional("ability", "assets/audio/ability.ogg");
audio.preloadOptional("heal", "assets/audio/heal.ogg");
// Boss sounds.....
audio.preloadOptional("boss", "assets/audio/boss_intro.ogg");
audio.preloadOptional("boss_defeat", "assets/audio/boss_defeat.ogg");
// Encounter sounds....
audio.preloadOptional("guidance", "assets/audio/Guidance_spirit.ogg");
audio.preloadOptional("phantom", "assets/audio/Phantom_Wraith.ogg");
audio.preloadOptional("trickster", "assets/audio/Trickster.ogg");
audio.preloadOptional("gate", "assets/audio/GateChoice.ogg");
audio.preloadOptional("battlefield", "assets/audio/BattleField.ogg");
}

//-----------------------------------------------------------------------------------------------------------------------
// STATIC BACKGROUND:-
//-----------------------------------------------------------------------------------------------------------------------
void RealmOfEchoes::clearStaticBackground()
{
staticBgSpr.reset();
}

bool RealmOfEchoes::loadStaticBackground(const vector<string>& paths)
{
staticBgSpr.reset();
const float vw = static_cast<float>(gw.getWidth());
const float vh = static_cast<float>(gw.getHeight());
return AssetLoader::emplaceCoverSprite(staticBgTex, staticBgSpr, paths, vw, vh);
}

void RealmOfEchoes::loadRegionBackground(int regionIndex)
{
if (regionIndex < 0 || regionIndex > 3)
{
clearStaticBackground();
return;
}
static const char* texPaths[4][2] =
{
{ "assets/texture/bg_forest.png",   "assets/texture/default.png" },
{ "assets/texture/bg_temple.png",   "assets/texture/default.png" },
{ "assets/texture/bg_mountain.png", "assets/texture/default.png" },
{ "assets/texture/bg_veil.png",     "assets/texture/default.png" },
};
static const char* musicPaths[4] =
{
"assets/audio/region_forest.ogg",
"assets/audio/region_temple.ogg",
"assets/audio/region_mountain.ogg",
"assets/audio/region_veil.ogg",
};
loadStaticBackground({ texPaths[regionIndex][0], texPaths[regionIndex][1] });
audio.PlayMusic(musicPaths[regionIndex], 50.f);
}

void RealmOfEchoes::loadWorldMapBackground()
{
loadStaticBackground({"assets/texture/bg_roe_map.png","assets/texture/default.png"});
}

void RealmOfEchoes::drawBackdrop()
{
if (staticBgSpr)
gw.getWindow().draw(*staticBgSpr);
}

//----------------------------------------------------------------------------------------------------------------------
// SAVE HELPER:-
//----------------------------------------------------------------------------------------------------------------------

void RealmOfEchoes::syncSave(int regionIndex, const Region& region)
{
questLog.setEncounterProgress(regionIndex, region.getEncounterIndex());
try
{
FileManager::savePlayer(player->serialize(),questLog.serialize());
}
catch (const exception& e)
{
cerr << "[Save Error] " << e.what() <<endl;
}
}

int RealmOfEchoes::bossCoinReward(int regionIndex)const
{
return 50 + regionIndex *50;
}

//-----------------------------------------------------------------------------------------------------------------------
// CONSTRUCTOR / DESTRUCTOR:-
//-----------------------------------------------------------------------------------------------------------------------

RealmOfEchoes::RealmOfEchoes(GameWindow& window,AudioManager& a): gw(window),audio(a),player(nullptr),logger("saves/game_log.txt")
{
gw.setIconFromFile("assets/icon/realm_of_echoes.png");
if (!AssetLoader::openFontWithFallback(font,{ "assets/font/GameUILora.ttf", "assets/font/Philosopher-Bold.ttf" }))
throw runtime_error("RealmOfEchoes: Cannot load font!");
setupAudio();
setupRegions();
gw.setInterface(Color(13, 2, 33), "REALM OF ECHOES: SPIRITS AND SHADOWS");
}

RealmOfEchoes::~RealmOfEchoes()
{
delete player;
player = nullptr;
}

//-----------------------------------------------------------------------------------------------------------------------
// REGION SETUP:-
//-----------------------------------------------------------------------------------------------------------------------

void RealmOfEchoes::setupRegions()
{
regions.clear();
//Region 0: Whispering Forest...
Region forest("THE WHISPERING FOREST", Color(27, 94, 32), 0);
forest.addEncounter({ EncounterType::Battle, "A WILLOWISP FLICKERS IN THE MIST." });
forest.addEncounter({ EncounterType::Negotiation, "A CALM SPIRIT OFFERS A HINT: FOLLOW THE BLUE MOTH." });
forest.addEncounter({ EncounterType::Battle, "ROOTS TWIST LIKE FINGERS." });
forest.addEncounter({ EncounterType::Shrine, "A MOSSY SHRINE HUMS WITH SOFT LIGHT." });
forest.addEncounter({ EncounterType::PhantomSurvival, "A PHANTOM WRAITH CIRCLES WITH A HUNGRY STARE." });
forest.addEncounter({ EncounterType::Battle, "THE FOREST EXHALES COLD AIR." });
// Gate choice ..
forest.addEncounter({ EncounterType::GateChoice, "TWO ANCIENT STONES MARK THE BOSS LAIR. CHOOSE THE TRUE GATE.", (rand() % 2) == 0 });
forest.addEncounter({ EncounterType::BossBattle, "THE CORRUPTED HEART OF THE WOOD AWAKENS." });
regions.push_back(forest);

//Region 1: Sunken Temples...
Region temples("THE SUNKEN TEMPLES", Color(0, 77, 64), 1);
temples.addEncounter({ EncounterType::Battle, "A STONE GOLEM BLOCKS THE STAIR." });
temples.addEncounter({ EncounterType::Battle, "WATER SPIRITS WHISPER RIDDLES." });
temples.addEncounter({ EncounterType::Negotiation, "A PRIEST SPIRIT TEACHES A VEIL-BREATHING CHANT." });
temples.addEncounter({ EncounterType::Shrine, "CRYSTAL WATER REFLECTS STARS BELOW." });
temples.addEncounter({ EncounterType::PhantomSurvival, "THE WRAITH DRAINS WARMTH FROM THE TORCHES." });
temples.addEncounter({ EncounterType::Trickster, "A FOX SPIRIT SWAPS YOUR FOOTPRINTS FOR LAUGHS." });
temples.addEncounter({ EncounterType::Battle, "CRACKS IN THE FLOOR BETRAY OLD TRAPS." });
//Gate choice..
temples.addEncounter({ EncounterType::GateChoice, "A HALL OF PILLARS SPLITS IN TWO. WHICH PATH IS TRUE?", (rand() % 2) == 0 });
temples.addEncounter({ EncounterType::BossBattle,  "THE TEMPLE GUARDIAN RISES." });
regions.push_back(temples);

//Region 2: Crimson Mountains...
Region mountains("THE CRIMSON MOUNTAINS", Color(183, 28, 28), 2);
mountains.addEncounter({ EncounterType::Battle, "AN EMBERWING SCREECHES OVERHEAD." });
mountains.addEncounter({ EncounterType::Trickster, "A HOT-WIND SPRITE DEMANDS A SILLY DANCE. YOU HUMOR IT." });
mountains.addEncounter({ EncounterType::Battle, "LAVA VEINS PULSE UNDER THE PATH." });
mountains.addEncounter({ EncounterType::Negotiation, "A CLIMBER SPIRIT SHARES A ROUTE AROUND THE CLIFF." });
mountains.addEncounter({ EncounterType::Shrine, "A HOT SPRING MISTS THE AIR." });
mountains.addEncounter({ EncounterType::PhantomSurvival, "COLD MIST CONDENSES INTO A WRAITH." });
mountains.addEncounter({ EncounterType::Battle, "ASH STINGS YOUR EYES." });
mountains.addEncounter({ EncounterType::GateChoice, "TWO LEDGES OVER THE VOLCANO. PICK THE ONE THAT WON'T CRUMBLE.", (rand() % 2) == 0 });
mountains.addEncounter({ EncounterType::BossBattle,  "THE EMBER TYRANT DESCENDS." });
regions.push_back(mountains);

//Region 3: The Veil Gate...
Region veil("THE VEIL GATE", Color(18, 0, 36), 3);
veil.addEncounter({ EncounterType::Battle, "SHADOWS TEAR AT THE EDGES OF YOUR SOUL." });
veil.addEncounter({ EncounterType::Battle, "THE VEIL ITSELF SHIMMERS WITH TEETH." });
veil.addEncounter({ EncounterType::PhantomSurvival, "A WRAITH TESTS YOUR WILL IN THE VOID." });
veil.addEncounter({ EncounterType::Negotiation, "A QUIET ANCESTOR NAMES THE SOVEREIGN'S WEAKNESS: PATIENCE." });
veil.addEncounter({ EncounterType::Trickster, "A JESTER SPIRIT LIES FOR FUN, THEN VANISHES." });
veil.addEncounter({ EncounterType::Battle, "ECHOES OF FAILED WALKERS CLAW AT YOU." });
veil.addEncounter({ EncounterType::GateChoice, "TWO GATES OPEN. ONE IS A TRAP THAT REWINDS YOUR STEPS.",(rand() % 2) == 0});
veil.addEncounter({ EncounterType::BossBattle, "THE SHADOW SOVEREIGN RISES." });
regions.push_back(veil);
}

// ─────────────────────────────────────────────────────────────────────────────
// SHARED DRAW HELPER
// ─────────────────────────────────────────────────────────────────────────────

void RealmOfEchoes::drawCenteredText(const string& str, float y,
    unsigned int size, const Color& color)
{
    Text text(font);
    text.setString(upperText(str));
    text.setCharacterSize(size);
    text.setFillColor(color);
    FloatRect b = text.getLocalBounds();
    text.setOrigin({ b.size.x / 2.f, b.size.y / 2.f });
    text.setPosition({ centerX(), y });
    gw.getWindow().draw(text);
}

// ─────────────────────────────────────────────────────────────────────────────
// NAME ENTRY SCREEN
// ─────────────────────────────────────────────────────────────────────────────

string RealmOfEchoes::showNameEntryScreen()
{
    gw.setInterface(Color(13, 2, 33), "REALM OF ECHOES: PROFILE");
    loadStaticBackground({
        "assets/texture/bg_roe_story_parchment.png",
        "assets/texture/default.png"
        });
    audio.PlayMusic("assets/audio/world_map.ogg", 80.f);

    string playerName;
    Text nameDisplay(font);
    nameDisplay.setCharacterSize(28);
    nameDisplay.setFillColor(Color(52, 34, 26));
    nameDisplay.setStyle(Text::Bold);
    const float midY = static_cast<float>(gw.getHeight()) * 0.55f;

    while (gw.isOpen())
    {
        while (auto event = gw.pollEvent())
        {
            if (event->is<Event::Closed>())
                gw.getWindow().close();
            if (auto* te = event->getIf<Event::TextEntered>())
            {
                char c = static_cast<char>(te->unicode);
                if (c == '\b' && !playerName.empty())
                    playerName.pop_back();
                else if (c == '\r' || c == '\n')
                    continue;
                else if (c >= 32 && c < 127 && playerName.size() < 18 && c != '|')
                    playerName.push_back(c);
            }
            if (auto* key = event->getIf<Event::KeyPressed>())
                if (key->code == Keyboard::Key::Enter && !playerName.empty())
                {
                    audio.playOptional("ui");
                    return playerName;
                }
        }

        nameDisplay.setString(upperText(playerName + "_"));
        FloatRect b = nameDisplay.getLocalBounds();
        nameDisplay.setOrigin({ b.size.x / 2.f, b.size.y / 2.f });
        nameDisplay.setPosition({ centerX(), midY });

        gw.clear();
        drawBackdrop();
        drawCenteredText("ENTER YOUR NAME, VEILWALKER",
            static_cast<float>(gw.getHeight()) * 0.28f, 26, Color(52, 34, 26));
        drawCenteredText("(PRESS ENTER TO CONFIRM)",
            static_cast<float>(gw.getHeight()) * 0.93f, 20, Color(212, 168, 92));
        gw.getWindow().draw(nameDisplay);
        gw.display();
    }
    return "WANDERER";
}

// ─────────────────────────────────────────────────────────────────────────────
// STORY SCREEN
// ─────────────────────────────────────────────────────────────────────────────

void RealmOfEchoes::showStoryScreen()
{
 sf::Font font;
 if (!AssetLoader::openFontWithFallback(font, { "assets/font/StoryUncialAntiqua.ttf", "assets/font/Philosopher-Bold.ttf" }))
        throw runtime_error("RealmOfEchoes: Cannot load font!");
    loadStaticBackground({
        "assets/texture/bg_roe_story_parchment.png",
        "assets/texture/default.png"
        });

    string story = questLog.getStoryIntro();
    if (story.empty())
        story = "You are the Veilwalker.\nBorn between worlds.\nOnly you can restore the balance.";

    bool done = false;
    while (gw.isOpen() && !done)
    {
        while (auto event = gw.pollEvent())
        {
            if (event->is<Event::Closed>())
                gw.getWindow().close();
            if (auto* key = event->getIf<Event::KeyPressed>())
                if (key->code == Keyboard::Key::Enter) { audio.playOptional("ui"); done = true; }
        }

        gw.clear();
        drawBackdrop();

        istringstream ss(story);
        string line;
        float y = static_cast<float>(gw.getHeight()) * 0.12f;
        while (getline(ss, line))
        {
            Text lineText(font);
            lineText.setString(upperText(line));
            lineText.setCharacterSize(16);
            lineText.setFillColor(Color(52, 34, 26));
            lineText.setPosition({ 50.f, y });
            lineText.setStyle(Text::Bold);
            gw.getWindow().draw(lineText);
            y += 26.f;
        }
        drawCenteredText("PRESS ENTER TO CONTINUE",
            static_cast<float>(gw.getHeight()) * 0.93f, 20, Color(212, 168, 92));
        gw.display();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// WORLD MAP SCREEN
// ─────────────────────────────────────────────────────────────────────────────

void RealmOfEchoes::showWorldMapScreen(int& selectedRegion)
{
    gw.setInterface(Color(15, 15, 35), "REALM OF ECHOES: WORLD MAP");
    gw.setIconFromFile("assets/icon/realm_of_echoes_map.png");
    loadWorldMapBackground();

    audio.PlayMusic("assets/audio/world_map.ogg", 50.f);

    if (player && player->getHp() <= 0)
    {
        player->heal(player->getMaxHp());
        player->restoreEnergy(player->getMaxEnergy());
        int previousRegion = max(0, questLog.getCurrentRegion() - 1);

        gw.clear();
        drawBackdrop();
        drawCenteredText("REVIVED! RETURNING TO PREVIOUS REGION...",
            static_cast<float>(gw.getHeight()) * 0.45f, 20, Color(0, 245, 255));
        drawCenteredText("PRESS ENTER TO CONTINUE",
            static_cast<float>(gw.getHeight()) * 0.60f, 16, Color(191, 0, 255));
        gw.display();
        sf::sleep(sf::seconds(2.f));

        selectedRegion = previousRegion;
        return;
    }

    struct Option { string label; int regionIndex; };
    vector<Option> options;
    for (int i = 0; i < 4; i++)
    {
        if (questLog.isRegionUnlocked(i))
        {
            string label = regions[i].getName();
            if (questLog.isRegionComplete(i))
                label += " [DONE]";
            options.push_back({ label, i });
        }
    }
    options.push_back({ "SAVE & EXIT TO ARCANE ARENA", -1 });

    vector<Text> optionTexts;
    optionTexts.reserve(options.size());
    for (size_t i = 0; i < options.size(); i++)
    {
        optionTexts.emplace_back(font);
        optionTexts[i].setString(upperText(options[i].label));
        optionTexts[i].setCharacterSize(22);
    }

    // Cursor triangle (pointing right).
    ConvexShape cursor;
    cursor.setPointCount(3);
    cursor.setPoint(0, { 0.f,  0.f });
    cursor.setPoint(1, { 0.f,  14.f });
    cursor.setPoint(2, { 12.f, 7.f });
    cursor.setFillColor(Color(0, 245, 255));

    int selected = 0;
    Clock navClock;

    while (gw.isOpen() && !quitToHub)
    {
        while (auto event = gw.pollEvent())
        {
            if (event->is<Event::Closed>()) { gw.getWindow().close(); return; }
            if (auto* key = event->getIf<Event::KeyPressed>())
            {
                if (key->code == Keyboard::Key::Up)
                {
                    audio.playOptional("ui_move");
                    selected = (selected - 1 + static_cast<int>(options.size()))
                        % static_cast<int>(options.size());
                }
                if (key->code == Keyboard::Key::Down)
                {
                    audio.playOptional("ui_move");
                    selected = (selected + 1) % static_cast<int>(options.size());
                }
                if (key->code == Keyboard::Key::Enter)
                {
                    audio.playOptional("ui");
                    if (options[selected].regionIndex == -1)
                    {
                        try {
                            FileManager::savePlayer(player->serialize(), questLog.serialize());
                            FileManager::saveScore(player->getName(),
                                player->getLevel(), player->getXP());
                        }
                        catch (const exception& e) { cerr << "[Save Error] " << e.what() << "\n"; }
                        selectedRegion = -1;
                        return;
                    }
                    selectedRegion = options[selected].regionIndex;
                    return;
                }
            }
        }

        // Mouse hover and click.
        Vector2i mousePos = Mouse::getPosition(gw.getWindow());
        Vector2f mouseWorld = gw.getWindow().mapPixelToCoords(mousePos);
        for (size_t i = 0; i < optionTexts.size(); i++)
        {
            if (optionTexts[i].getGlobalBounds().contains(mouseWorld))
            {
                selected = static_cast<int>(i);
                if (Mouse::isButtonPressed(Mouse::Button::Left)
                    && navClock.getElapsedTime().asMilliseconds() > 300)
                {
                    navClock.restart();
                    audio.playOptional("ui");
                    if (options[selected].regionIndex == -1)
                    {
                        try {
                            FileManager::savePlayer(player->serialize(), questLog.serialize());
                            FileManager::saveScore(player->getName(),
                                player->getLevel(), player->getXP());
                        }
                        catch (...) {}
                        selectedRegion = -1;
                        return;
                    }
                    selectedRegion = options[selected].regionIndex;
                    return;
                }
                break;
            }
        }

        // Draw.
        gw.clear();
        drawBackdrop();
        drawCenteredText("THE VEILLANDS", 40.f, 30, Color(52, 34, 26));
        drawCenteredText("UP/DOWN  ENTER  TO SELECT", 72.f, 14, Color(212, 168, 92));

        for (size_t i = 0; i < optionTexts.size(); i++)
        {
            bool isSel = (static_cast<int>(i) == selected);
            optionTexts[i].setFillColor(isSel ? Color(255, 220, 100) : Color(212, 168, 92));
            optionTexts[i].setCharacterSize(isSel ? 24 : 22);
            FloatRect b = optionTexts[i].getLocalBounds();
            optionTexts[i].setOrigin({ b.size.x / 2.f, b.size.y / 2.f });
            const float optY = 120.f + static_cast<float>(i) * 48.f;
            optionTexts[i].setPosition({ centerX(), optY });
            gw.getWindow().draw(optionTexts[i]);

            // Draw cursor to the left of selected option.
            if (isSel)
            {
                cursor.setPosition({ centerX() - b.size.x / 2.f - 24.f, optY - 7.f });
                gw.getWindow().draw(cursor);
            }
        }

        // Player stats bar at bottom.
        if (player)
        {
            Text stats(font);
            stats.setString(upperText(
                player->getName() + "  LV." + to_string(player->getLevel()) +
                "  HP " + to_string(player->getHp()) + "/" + to_string(player->getMaxHp()) +
                "  COINS " + to_string(player->getCoins())));
            stats.setCharacterSize(14);
            stats.setFillColor(Color(57, 255, 20));
            stats.setPosition({ 16.f, static_cast<float>(gw.getHeight()) - 28.f });
            gw.getWindow().draw(stats);
        }
        gw.display();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// NEGOTIATION SCREEN
// ─────────────────────────────────────────────────────────────────────────────

void RealmOfEchoes::runNegotiationScreen(const string& msg)
{
    audio.playOptional("guidance");
    // Load guidance spirit sprite
    Texture guidanceTex;
    optional<Sprite> guidanceSpr;
    if (AssetLoader::loadTextureWithFallback(guidanceTex, { "assets/characters/guidance_spirit.png" })) {
        guidanceTex.setSmooth(true);
        guidanceSpr.emplace(guidanceTex);
        const float targetH = static_cast<float>(gw.getHeight()) * 0.35f;
        const float sc = targetH / static_cast<float>(guidanceTex.getSize().y);
        guidanceSpr->setScale({ sc, sc });
        guidanceSpr->setPosition({ static_cast<float>(gw.getWidth()) * 0.75f, static_cast<float>(gw.getHeight()) * 0.35f });
    }
    bool done = false;
    while (gw.isOpen() && !done)
    {
        while (auto event = gw.pollEvent())
        {
            if (event->is<Event::Closed>()) gw.getWindow().close();
            if (auto* key = event->getIf<Event::KeyPressed>())
                if (key->code == Keyboard::Key::Enter) { audio.playOptional("ui"); done = true; }
        }

        gw.clear();
        drawBackdrop();
        drawCenteredText("GUIDANCE",
            static_cast<float>(gw.getHeight()) * 0.2f, 26, Color(191, 0, 255));
        if (guidanceSpr) gw.getWindow().draw(*guidanceSpr);
        Text body(font);
        body.setString(upperText(msg));
        body.setCharacterSize(15);
        body.setFillColor(Color(240, 240, 255));
        body.setPosition({ 40.f, static_cast<float>(gw.getHeight()) * 0.32f });
        gw.getWindow().draw(body);
        drawCenteredText("PRESS ENTER TO CONTINUE",
            static_cast<float>(gw.getHeight()) * 0.88f, 15, Color(0, 245, 255));
        gw.display();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// GATE CHOICE SCREEN
// ─────────────────────────────────────────────────────────────────────────────

bool RealmOfEchoes::runGateChoice(int regionIndex, Region& region, const Encounter& enc)
{
    audio.playOptional("gate");
    int choice = 0;
    const bool trapLeft = enc.leftGateIsTrap;

    // Load region-specific gate sprite (falls back to default veil gate).
    Texture gateTex;
    optional<Sprite> gateLeftSpr, gateRightSpr;

    static const char* gatePaths[4][2] =
    {
        { "assets/gates/forest_gate.png",  "assets/gates/veil_gate.png" },
        { "assets/gates/temple_gate.png",  "assets/gates/veil_gate.png" },
        { "assets/gates/volcano_gate.png", "assets/gates/veil_gate.png" },
        { "assets/gates/veil_gate.png",    "assets/gates/veil_gate.png" },
    };

    if (AssetLoader::loadTextureWithFallback(gateTex,
        { gatePaths[regionIndex][0], gatePaths[regionIndex][1] }))
    {
        gateTex.setSmooth(true);
        gateLeftSpr.emplace(gateTex);
        gateRightSpr.emplace(gateTex);
        const float targetH = static_cast<float>(gw.getHeight()) * 0.28f;
        const float sc = targetH / static_cast<float>(gateTex.getSize().y);
        gateLeftSpr->setScale({ sc, sc });
        gateRightSpr->setScale({ sc, sc });
        const Vector2u gts = gateTex.getSize();
        gateLeftSpr->setOrigin({ gts.x * 0.5f, gts.y * 0.92f });
        gateRightSpr->setOrigin({ gts.x * 0.5f, gts.y * 0.92f });
    }

    // Cursor triangle for selection.
    ConvexShape cursor;
    cursor.setPointCount(3);
    cursor.setPoint(0, { 0.f,  0.f });
    cursor.setPoint(1, { 0.f,  14.f });
    cursor.setPoint(2, { 12.f, 7.f });
    cursor.setFillColor(Color(255, 220, 60));

    while (gw.isOpen())
    {
        while (auto event = gw.pollEvent())
        {
            if (event->is<Event::Closed>()) gw.getWindow().close();
            if (auto* key = event->getIf<Event::KeyPressed>())
            {
                if (key->code == Keyboard::Key::Left || key->code == Keyboard::Key::A) choice = 0;
                if (key->code == Keyboard::Key::Right || key->code == Keyboard::Key::D) choice = 1;
                if (key->code == Keyboard::Key::Up || key->code == Keyboard::Key::Down)
                    choice = 1 - choice;

                if (key->code == Keyboard::Key::Enter)
                {
                    audio.playOptional("ui");
                    const bool pickedLeft = (choice == 0);
                    const bool wrong = (pickedLeft == trapLeft);

                    if (wrong)
                    {
                        gw.clear(); drawBackdrop();
                        drawCenteredText("THE TRAP GATE CLAIMS YOU!",
                            static_cast<float>(gw.getHeight()) * 0.4f, 24, Color(255, 23, 68));
                        gw.display();
                        // FIX: sf::sleep instead of Sleep() (Windows-only).
                        sf::sleep(sf::seconds(2.f));
                        // Reset region to start on wrong gate (done here only, not in runRegion).
                        region.setEncounterIndex(0);
                        questLog.resetEncounterProgress(regionIndex);
                        syncSave(regionIndex, region);
                        return false;
                    }

                    gw.clear(); drawBackdrop();
                    drawCenteredText("THE TRUE PATH OPENS.",
                        static_cast<float>(gw.getHeight()) * 0.45f, 22, Color(57, 255, 20));
                    gw.display();
                    sf::sleep(sf::seconds(2.f));
                    return true;
                }
            }
        }

        // Mouse hover.
        Vector2i mp = Mouse::getPosition(gw.getWindow());
        Vector2f mw = gw.getWindow().mapPixelToCoords(mp);
        const float vw = static_cast<float>(gw.getWidth());
        const float vh = static_cast<float>(gw.getHeight());
        const float yGate = vh * 0.62f;
        const float xL = vw * 0.28f;
        const float xR = vw * 0.72f;

        // Click detection for left/right gate areas.
        if (Mouse::isButtonPressed(Mouse::Button::Left))
        {
            if (mw.x < vw * 0.5f) choice = 0;
            else                   choice = 1;
        }

        gw.clear();
        drawBackdrop();
        drawCenteredText("CHOOSE YOUR GATE",
            vh * 0.12f, 24, Color(0, 245, 255));
        drawCenteredText(enc.description,
            vh * 0.20f, 14, Color(240, 240, 255));

        if (gateLeftSpr && gateRightSpr)
        {
            const float baseSc = (vh * 0.28f) / static_cast<float>(gateTex.getSize().y);
            gateLeftSpr->setPosition({ xL, yGate });
            gateRightSpr->setPosition({ xR, yGate });
            gateLeftSpr->setScale({ baseSc * (choice == 0 ? 1.06f : 1.f),
                                    baseSc * (choice == 0 ? 1.06f : 1.f) });
            gateRightSpr->setScale({ baseSc * (choice == 1 ? 1.06f : 1.f),
                                     baseSc * (choice == 1 ? 1.06f : 1.f) });
            gw.getWindow().draw(*gateLeftSpr);
            gw.getWindow().draw(*gateRightSpr);
        }

        // Gate labels with cursor marker.
        const float lblY = yGate + vh * 0.14f;

        Text leftLbl(font, "LEFT", 18);
        leftLbl.setFillColor(choice == 0 ? Color(255, 255, 100) : Color(191, 0, 255));
        FloatRect lb = leftLbl.getLocalBounds();
        leftLbl.setOrigin({ lb.size.x / 2.f, lb.size.y / 2.f });
        leftLbl.setPosition({ xL, lblY });
        gw.getWindow().draw(leftLbl);
        if (choice == 0)
        {
            cursor.setPosition({ xL - lb.size.x / 2.f - 20.f, lblY - 7.f });
            gw.getWindow().draw(cursor);
        }

        Text rightLbl(font, "RIGHT", 18);
        rightLbl.setFillColor(choice == 1 ? Color(255, 255, 100) : Color(191, 0, 255));
        FloatRect rb = rightLbl.getLocalBounds();
        rightLbl.setOrigin({ rb.size.x / 2.f, rb.size.y / 2.f });
        rightLbl.setPosition({ xR, lblY });
        gw.getWindow().draw(rightLbl);
        if (choice == 1)
        {
            cursor.setPosition({ xR - rb.size.x / 2.f - 20.f, lblY - 7.f });
            gw.getWindow().draw(cursor);
        }

        drawCenteredText("ARROWS / A-D   ENTER TO CHOOSE",
            vh * 0.92f, 13, Color(255, 109, 0));
        gw.display();
    }
    return false;
}

// ─────────────────────────────────────────────────────────────────────────────
// PHANTOM SURVIVAL
// ─────────────────────────────────────────────────────────────────────────────

void RealmOfEchoes::runPhantomSurvival(int regionIndex, Region& region)
{
    (void)region;
    audio.playOptional("phantom");
    const int hpStart = player->getHp();
    Clock timer, hitClock;
    float hitInterval = 0.7f;
    bool done = false;
    int currentFrame = 0;

    // Load phantom wraith animation frames..
    vector<Texture> phantomTextures;
    vector<string> framePaths = { 
        "assets/characters/PIdle.png", 
        "assets/characters/PAttack.png", 
        "assets/characters/PMove.png", 
        "assets/characters/PSteal.png", 
        "assets/characters/PDie.png" 
    };
    for (const auto& path : framePaths) {
        Texture tex;
        if (AssetLoader::loadTextureWithFallback(tex, { path })) {
            tex.setSmooth(true);
            phantomTextures.push_back(tex);
        }
    }

    while (gw.isOpen() && !done)
    {
        float dt = timer.getElapsedTime().asSeconds();
        while (auto event = gw.pollEvent())
        {
            if (event->is<Event::Closed>()) gw.getWindow().close();
        }

        if (dt >= kSurvivalSeconds) done = true;

        if (hitClock.getElapsedTime().asSeconds() >= hitInterval)
        {
            hitClock.restart();
            player->takeRawDamage(1);
            audio.playOptional("hit");
            currentFrame = (currentFrame + 1) % (int)phantomTextures.size();
        }

        gw.clear();
        drawBackdrop();
        drawCenteredText("SURVIVE THE WRAITH",
            static_cast<float>(gw.getHeight()) * 0.15f, 22, Color(255, 109, 0));

        // Draw phantom wraith animation
        if (!phantomTextures.empty()) {
            Sprite phantomSpr(phantomTextures[currentFrame]);
            const float targetH = static_cast<float>(gw.getHeight()) * 0.35f;
            const float sc = targetH / static_cast<float>(phantomTextures[currentFrame].getSize().y);
            phantomSpr.setScale({ sc, sc });
            phantomSpr.setPosition({ centerX(), static_cast<float>(gw.getHeight()) * 0.35f });
            gw.getWindow().draw(phantomSpr);
        }

        ostringstream oss;
        oss.setf(ios::fixed); oss.precision(1);
        oss << "TIME LEFT: " << (kSurvivalSeconds - dt);
        drawCenteredText(oss.str(),
            static_cast<float>(gw.getHeight()) * 0.28f, 18, Color(0, 245, 255));
        drawCenteredText("HOLD YOUR GROUND. EACH STING TAKES 1 HP.",
            static_cast<float>(gw.getHeight()) * 0.38f, 13, Color(240, 240, 255));

        ostringstream hp;
        hp << "HP: " << player->getHp() << "/" << player->getMaxHp();
        drawCenteredText(hp.str(),
            static_cast<float>(gw.getHeight()) * 0.5f, 18, Color(57, 255, 20));
        gw.display();

        if (!player->isAlive()) done = true;
    }

    if (player->getHp() < hpStart) player->removeCoins(5);

    // After survival, give the shrine rest.
    showShrineScreen(regionIndex);
}

// ─────────────────────────────────────────────────────────────────────────────
// SHRINE SCREEN
// Full HP and SP restore (as per requirements).
// FIX: audio.PlayMusic moved BEFORE the render loop.
// ─────────────────────────────────────────────────────────────────────────────

void RealmOfEchoes::showShrineScreen(int regionIndex)
{
    gw.setInterface(Color(13, 2, 33), "REALM OF ECHOES: ANCIENT SHRINE");

    const bool shrineArt = loadStaticBackground({
        "assets/texture/shrine.png",
        "assets/texture/shrine.jpg",
        "assets/texture/default.png"
        });
    if (!shrineArt) loadRegionBackground(regionIndex);

    // Full HP and SP restore at every shrine.
    player->heal(player->getMaxHp());
    player->restoreEnergy(player->getMaxEnergy());

    // FIX: Start shrine music ONCE before the loop.
    audio.PlayMusic("assets/audio/world_map.ogg", 80.f);

    bool done = false;
    while (gw.isOpen() && !done)
    {
        while (auto event = gw.pollEvent())
        {
            if (event->is<Event::Closed>()) gw.getWindow().close();
            if (auto* key = event->getIf<Event::KeyPressed>())
                if (key->code == Keyboard::Key::Enter) { audio.playOptional("ui"); done = true; }
        }

        gw.clear();
        drawBackdrop();
        drawCenteredText("ANCIENT SHRINE",
            static_cast<float>(gw.getHeight()) * 0.28f, 28, Color(191, 0, 255));
        drawCenteredText("YOU REST. HP AND SP FULLY RESTORED.",
            static_cast<float>(gw.getHeight()) * 0.42f, 16, Color(240, 240, 255));

        // Show restored values.
        ostringstream stats;
        stats << "HP: " << player->getHp() << "/" << player->getMaxHp()
            << "   SP: " << player->getSpiritEnergy() << "/" << player->getMaxEnergy();
        drawCenteredText(stats.str(),
            static_cast<float>(gw.getHeight()) * 0.54f, 15, Color(57, 255, 20));

        drawCenteredText("PRESS ENTER TO CONTINUE",
            static_cast<float>(gw.getHeight()) * 0.88f, 15, Color(0, 245, 255));
        gw.display();
    }

    // Restore region background after shrine.
    if (shrineArt) loadRegionBackground(regionIndex);
}

// ─────────────────────────────────────────────────────────────────────────────
// REGION COMPLETE SCREEN
// ─────────────────────────────────────────────────────────────────────────────

void RealmOfEchoes::showRegionCompleteScreen(int regionIndex)
{
    static const char* regionNames[4] = {
        "WHISPERING FOREST", "SUNKEN TEMPLES",
        "CRIMSON MOUNTAINS",  "VEIL GATE"
    };

    bool done = false;
    while (gw.isOpen() && !done)
    {
        while (auto event = gw.pollEvent())
        {
            if (event->is<Event::Closed>()) gw.getWindow().close();
            if (auto* key = event->getIf<Event::KeyPressed>())
                if (key->code == Keyboard::Key::Enter) { audio.playOptional("ui"); done = true; }
        }

        gw.clear(); drawBackdrop();
        drawCenteredText("REGION CLEARED!",
            static_cast<float>(gw.getHeight()) * 0.28f, 26, Color(57, 255, 20));
        drawCenteredText(regionNames[regionIndex],
            static_cast<float>(gw.getHeight()) * 0.42f, 20, Color(0, 245, 255));
        drawCenteredText("SECURED.",
            static_cast<float>(gw.getHeight()) * 0.52f, 16, Color(240, 240, 255));
        drawCenteredText("PRESS ENTER TO RETURN TO THE VEILLANDS.",
            static_cast<float>(gw.getHeight()) * 0.88f, 14, Color(191, 0, 255));
        gw.display();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// WIN / ENDING SCREEN (all 4 regions cleared)
// ─────────────────────────────────────────────────────────────────────────────

void RealmOfEchoes::showEndingScreen()
{
    bool done = false;
    while (gw.isOpen() && !done)
    {
        while (auto event = gw.pollEvent())
        {
            if (event->is<Event::Closed>()) gw.getWindow().close();
            if (auto* key = event->getIf<Event::KeyPressed>())
                if (key->code == Keyboard::Key::Enter) { audio.playOptional("ui"); done = true; }
        }

        gw.clear(); drawBackdrop();
        drawCenteredText("YOU HAVE WON!",
            static_cast<float>(gw.getHeight()) * 0.12f, 30, Color(255, 220, 0));
        drawCenteredText("THE SHADOW SOVEREIGN FALLS!",
            static_cast<float>(gw.getHeight()) * 0.24f, 22, Color(0, 245, 255));
        drawCenteredText("BALANCE RETURNS TO THE VEILLANDS.",
            static_cast<float>(gw.getHeight()) * 0.36f, 15, Color(240, 240, 255));
        drawCenteredText("VEILWALKER: " + upperText(player->getName()),
            static_cast<float>(gw.getHeight()) * 0.48f, 16, Color(57, 255, 20));
        drawCenteredText("THANK YOU FOR PLAYING!",
            static_cast<float>(gw.getHeight()) * 0.60f, 18, Color(191, 0, 255));
        drawCenteredText("PRESS ENTER TO RETURN TO ARCANE ARENA.",
            static_cast<float>(gw.getHeight()) * 0.88f, 14, Color(191, 0, 255));
        gw.display();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// REGION RUNNER (core gameplay loop)
// FIX: Removed redundant region.setEncounterIndex(0) in GateChoice else branch
//      (runGateChoice already resets it on wrong-gate path).
// FIX: Healing potion given after every battle (win and lose).
// FIX: No more "continue path" post-battle menu; player always proceeds forward.
// ─────────────────────────────────────────────────────────────────────────────

void RealmOfEchoes::runRegion(int regionIndex)
{
    Region& region = regions[regionIndex];
    region.setEncounterIndex(questLog.getEncounterProgress(regionIndex));
    loadRegionBackground(regionIndex);
    gw.setInterface(region.getThemeColor(), "Realm of Echoes - " + region.getName());

    while (gw.isOpen() && !quitToHub)
    {
        if (region.getEncounterIndex() >= region.getEncounterCount())
            break;

        const Encounter& enc = region.peekEncounter();

        // ── Shrine ────────────────────────────────────────────────────────────
        if (enc.type == EncounterType::Shrine)
        {
            showShrineScreen(regionIndex);
            region.advanceEncounter();
            syncSave(regionIndex, region);
            continue;
        }

        // ── Negotiation ───────────────────────────────────────────────────────
        if (enc.type == EncounterType::Negotiation)
        {
            runNegotiationScreen(enc.description);
            region.advanceEncounter();
            syncSave(regionIndex, region);
            continue;
        }

        // ── Trickster ─────────────────────────────────────────────────────────
        if (enc.type == EncounterType::Trickster)
        {
            audio.playOptional("trickster");
            // Load trickster sprite
            Texture tricksterTex;
            optional<Sprite> tricksterSpr;
            if (AssetLoader::loadTextureWithFallback(tricksterTex, { "assets/characters/trickster.png" })) {
                tricksterTex.setSmooth(true);
                tricksterSpr.emplace(tricksterTex);
                const float targetH = static_cast<float>(gw.getHeight()) * 0.35f;
                const float sc = targetH / static_cast<float>(tricksterTex.getSize().y);
                tricksterSpr->setScale({ sc, sc });
                tricksterSpr->setPosition({ static_cast<float>(gw.getWidth()) * 0.75f, static_cast<float>(gw.getHeight()) * 0.35f });
            }
            Text body(font);
            body.setString(upperText(enc.description));
            body.setCharacterSize(14);
            body.setFillColor(Color(240, 240, 255));
            bool done = false;
            while (gw.isOpen() && !done)
            {
                while (auto event = gw.pollEvent())
                {
                    if (event->is<Event::Closed>()) gw.getWindow().close();
                    if (auto* key = event->getIf<Event::KeyPressed>())
                        if (key->code == Keyboard::Key::Enter) { audio.playOptional("ui"); done = true; }
                }
                gw.clear(); drawBackdrop();
                drawCenteredText("TRICKSTER",
                    static_cast<float>(gw.getHeight()) * 0.18f, 26, Color(255, 109, 0));
                if (tricksterSpr) gw.getWindow().draw(*tricksterSpr);
                body.setPosition({ 36.f, static_cast<float>(gw.getHeight()) * 0.3f });
                gw.getWindow().draw(body);
                drawCenteredText("PRESS ENTER",
                    static_cast<float>(gw.getHeight()) * 0.88f, 14, Color(0, 245, 255));
                gw.display();
            }
            region.advanceEncounter();
            syncSave(regionIndex, region);
            continue;
        }

        // ── Phantom Survival ─────────────────────────────────────────────────
        if (enc.type == EncounterType::PhantomSurvival)
        {
            runPhantomSurvival(regionIndex, region);
            region.advanceEncounter();
            syncSave(regionIndex, region);
            continue;
        }

        // ── Gate Choice (before boss) ─────────────────────────────────────────
        if (enc.type == EncounterType::GateChoice)
        {
            if (runGateChoice(regionIndex, region, enc))
            {
                // Correct gate: advance to boss.
                region.advanceEncounter();
            }
            // FIX: Wrong gate resets are handled INSIDE runGateChoice.
            //      Do NOT call region.setEncounterIndex(0) here (was a double-reset bug).
            syncSave(regionIndex, region);
            continue;
        }

        // ── Normal Battle ─────────────────────────────────────────────────────
        if (enc.type == EncounterType::Battle)
        {
            audio.playOptional("battlefield");
            Spirit* enemy = region.createEnemy();
            Battle battle(gw, *player, *enemy, audio,
                staticBgSpr ? &(*staticBgSpr) : nullptr);
            BattleResult result = battle.run();
            delete enemy;

            if (result == BattleResult::PlayerExited) return;

            // Give healing potion after EVERY battle (win or lose) per requirements.
            if (!player->getInventory().addItem(Item("Healing Potion", ItemType::HealingPotion, 30))) {
                // Inventory is full, but don't block progression.
                // Player can still advance without the reward.
            }

            if (result == BattleResult::PlayerDied)
            {
                // Player died in normal battle: return to world map with potion.
                return;
            }

            // Victory: advance to next encounter.
            region.advanceEncounter();
            syncSave(regionIndex, region);
            continue;
        }

        // ── Boss Battle (3 attempts with full HP each time) ───────────────────
        if (enc.type == EncounterType::BossBattle)
        {
            bossAttemptsLeft = 3;
            bool bossDefeated = false;

            while (gw.isOpen() && !quitToHub && bossAttemptsLeft > 0 && !bossDefeated)
            {
                // Full restore before each boss attempt.
                player->heal(player->getMaxHp());
                player->restoreEnergy(player->getMaxEnergy());

                audio.playOptional("boss", 55.f);
                Spirit* boss = region.createBoss();
                Battle bossBattle(gw, *player, *boss, audio,
                    staticBgSpr ? &(*staticBgSpr) : nullptr);
                BattleResult res = bossBattle.run();
                delete boss;

                if (res == BattleResult::PlayerExited) return;

                if (res == BattleResult::PlayerWon)
                {
                    bossDefeated = true;
                    audio.playOptional("boss_defeat", 70.f);
                    region.advanceEncounter();
                    player->addCoins(bossCoinReward(regionIndex));
                    region.markComplete();
                    questLog.completeRegion(regionIndex);
                    questLog.setCurrentRegion(std::min(3, regionIndex + 1));
                    showRegionCompleteScreen(regionIndex);
                    try {
                        FileManager::savePlayer(player->serialize(), questLog.serialize());
                        FileManager::saveScore(player->getName(), player->getLevel(), player->getXP());
                    }
                    catch (...) {}

                    // Show ending screen if all 4 regions cleared.
                    if (questLog.isGameComplete()) showEndingScreen();
                }
                else
                {
                    // Player died in boss fight.
                    bossAttemptsLeft--;
                    if (bossAttemptsLeft > 0)
                    {
                        gw.clear(); drawBackdrop();
                        drawCenteredText("YOU FELL... " + to_string(bossAttemptsLeft) + " ATTEMPTS LEFT",
                            static_cast<float>(gw.getHeight()) * 0.45f, 22, Color(255, 109, 0));
                        gw.display();
                        sf::sleep(sf::seconds(2.f));
                    }
                    else
                    {
                        // All 3 attempts used — reset region, return to world map.
                        gw.clear(); drawBackdrop();
                        drawCenteredText("THE BOSS DEFEATED YOU!",
                            static_cast<float>(gw.getHeight()) * 0.4f, 24, Color(255, 109, 0));
                        drawCenteredText("YOU MUST CLEAR THE REGION AGAIN.",
                            static_cast<float>(gw.getHeight()) * 0.5f, 16, Color(57, 255, 20));
                        gw.display();
                        sf::sleep(sf::seconds(2.5f));
                        region.setEncounterIndex(0);
                        questLog.resetEncounterProgress(regionIndex);
                        syncSave(regionIndex, region);
                        return;
                    }
                }
            }
            return;  // Boss encounter finished.
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// MAIN ENTRY POINT
// ─────────────────────────────────────────────────────────────────────────────

void RealmOfEchoes::run()
{
    quitToHub = false;

    // Check for existing save file.
    string playerData, questData;
    FileManager::LoadStatus loadStatus = FileManager::loadPlayer(playerData, questData);

    if (loadStatus != FileManager::LoadStatus::NotFound)
    {
        loadStaticBackground({
            "assets/texture/bg_roe_story_parchment.png",
            "assets/texture/default.png"
            });

        bool decided = false, loadSave = false;
        while (gw.isOpen() && !decided)
        {
            while (auto event = gw.pollEvent())
            {
                if (event->is<Event::Closed>()) gw.getWindow().close();
                if (auto* key = event->getIf<Event::KeyPressed>())
                {
                    if (key->code == Keyboard::Key::L)
                    {
                        loadSave = (loadStatus == FileManager::LoadStatus::Ok);
                        decided = true;
                    }
                    if (key->code == Keyboard::Key::N) decided = true;
                }
            }
            gw.clear(); drawBackdrop();
            if (loadStatus == FileManager::LoadStatus::Ok)
            {
                drawCenteredText("SAVE FILE FOUND!",
                    static_cast<float>(gw.getHeight()) * 0.38f, 24, Color(0, 245, 255));
                drawCenteredText("L = LOAD    N = NEW GAME",
                    static_cast<float>(gw.getHeight()) * 0.48f, 16, Color(240, 240, 255));
            }
            else
            {
                drawCenteredText("SAVE FILE IS CORRUPT",
                    static_cast<float>(gw.getHeight()) * 0.36f, 24, Color(255, 109, 0));
                drawCenteredText("PRESS N FOR NEW GAME",
                    static_cast<float>(gw.getHeight()) * 0.48f, 16, Color(240, 240, 255));
            }
            gw.display();
        }

        if (loadSave)
        {
            player = new Veilwalker("Loading...");
            try
            {
                player->deserialize(playerData);
                questLog.deserialize(questData);
            }
            catch (const exception& e)
            {
                delete player; player = nullptr;
                // FIX: staticBgSpr passed by reference (GameLogger fix).
                logger.showMessage(&gw, font,
                    "SAVE CORRUPT: " + string(e.what()), 2.0f,
                    staticBgTex,
                    { "assets/texture/LogAndProfile.png", "assets/texture/default.png" },
                    staticBgSpr,
                    static_cast<int>(gw.getWidth()),
                    static_cast<int>(gw.getHeight()));
                loadSave = false;
            }

            if (loadSave && player)
            {
                try { questLog.loadStory("assets/data/story.txt"); }
                catch (...) {}
                logger.showMessage(&gw, font,
                    "WELCOME BACK, " + upperText(player->getName()) + "!", 1.8f,
                    staticBgTex,
                    { "assets/texture/LogAndProfile.png", "assets/texture/default.png" },
                    staticBgSpr,
                    static_cast<int>(gw.getWidth()),
                    static_cast<int>(gw.getHeight()));

                while (gw.isOpen() && !quitToHub)
                {
                    int regionSelect = 0;
                    showWorldMapScreen(regionSelect);
                    if (regionSelect == -1) return;
                    if (!questLog.isRegionUnlocked(regionSelect)) continue;
                    runRegion(regionSelect);
                    if (quitToHub) return;
                    if (questLog.isGameComplete()) break;
                }
                return;
            }
        }
    }

    // ── New game flow ─────────────────────────────────────────────────────────
    string pName = showNameEntryScreen();
    player = new Veilwalker(pName);

    // Give starting items.
    if (!player->getInventory().addItem(Item("Healing Potion", ItemType::HealingPotion, 30, 2))) {
        // Starting items failed - inventory full (should not happen at game start).
    }
    if (!player->getInventory().addItem(Item("Spirit Crystal", ItemType::SpiritCrystal, 20, 1))) {
        // Spirit crystal failed to add.
    }
    player->addCoins(10);

    try { questLog.loadStory("assets/data/story.txt"); }
    catch (...) {}
    showStoryScreen();

    while (gw.isOpen() && !quitToHub)
    {
        int selectedRegion = 0;
        showWorldMapScreen(selectedRegion);
        if (selectedRegion == -1) break;
        if (!questLog.isRegionUnlocked(selectedRegion)) continue;
        runRegion(selectedRegion);
        if (quitToHub) return;
        if (questLog.isGameComplete()) break;
    }
}