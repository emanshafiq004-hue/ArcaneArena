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

float RealmOfEchoes::centerX() const {
    return static_cast<float>(gw.getWidth()) * 0.5f;
}

string RealmOfEchoes::upperText(const string& s) const {
    string out;
    out.reserve(s.size());
    for (unsigned char c : s)
        out.push_back(static_cast<char>(toupper(c)));
    return out;
}

void RealmOfEchoes::setupAudio() {
    audio.preloadOptional("ui", "assets/audio/ui_confirm.ogg");
    audio.preloadOptional("ui_move", "assets/audio/ui_move.ogg");
    audio.preloadOptional("hit", "assets/audio/battle_hit.ogg");
    audio.preloadOptional("block", "assets/audio/battle_block.ogg");
    audio.preloadOptional("boss", "assets/audio/boss_intro.ogg");
    audio.preloadOptional("boss_defeat", "assets/audio/boss_defeat.ogg");
    audio.preloadOptional("region0", "assets/audio/region_forest.ogg");
    audio.preloadOptional("region1", "assets/audio/region_temple.ogg");
    audio.preloadOptional("region2", "assets/audio/region_mountain.ogg");
    audio.preloadOptional("region3", "assets/audio/region_veil.ogg");
}

void RealmOfEchoes::clearStaticBackground() {
    staticBgSpr.reset();
}

bool RealmOfEchoes::loadStaticBackground(const vector<string>& paths) {
    scrollActive = false;
    scrollSprite0.reset();
    scrollSprite1.reset();
    staticBgSpr.reset();
    const float vw = static_cast<float>(gw.getWidth());
    const float vh = static_cast<float>(gw.getHeight());
    return AssetLoader::emplaceCoverSprite(staticBgTex, staticBgSpr, paths, vw, vh);
}

void RealmOfEchoes::tryBindScrollForRegion(int regionIndex) {
    clearStaticBackground();
    scrollActive = false;
    scrollSprite0.reset();
    scrollSprite1.reset();
    if (regionIndex < 0 || regionIndex > 3)
        return;
    const char* path = nullptr;
    switch (regionIndex) {
    case 0:
        path = "assets/texture/bg_forest.png";
        break;
    case 1:
        path = "assets/texture/bg_temple.png";
        break;
    case 2:
        path = "assets/texture/bg_mountain.png";
        break;
    case 3:
        path = "assets/texture/bg_veil.png";
        break;
    default:
        return;
    }
    if (!scrollTex.loadFromFile(path))
        return;
    scrollTex.setRepeated(true);
    scrollTex.setSmooth(true);
    scrollSprite0.emplace(scrollTex);
    scrollSprite1.emplace(scrollTex);
    const float w = static_cast<float>(gw.getWidth());
    const float h = static_cast<float>(gw.getHeight());
    const float texW = static_cast<float>(scrollTex.getSize().x);
    const float scale = h / static_cast<float>(scrollTex.getSize().y);
    scrollSprite0->setScale({ scale, scale });
    scrollSprite1->setScale({ scale, scale });
    scrollSprite0->setTextureRect(
        IntRect({ 0, 0 }, { static_cast<int>(texW), static_cast<int>(scrollTex.getSize().y) }));
    scrollSprite1->setTextureRect(
        IntRect({ 0, 0 }, { static_cast<int>(texW), static_cast<int>(scrollTex.getSize().y) }));
    scrollSprite0->setPosition({ 0.f, 0.f });
    scrollSprite1->setPosition({ texW * scale, 0.f });
    scrollOffset = 0.f;
    scrollActive = true;
    if (regionIndex >= 0 && regionIndex <= 3) {
        const char* keys[] = { "region0", "region1", "region2", "region3" };
        audio.playOptional(keys[regionIndex], 38.f);
    }
}

void RealmOfEchoes::tryBindScrollForWorldMap() {
    clearStaticBackground();
    scrollActive = false;
    scrollSprite0.reset();
    scrollSprite1.reset();
    const char* path = "assets/texture/bg_roe_map.png";
    if (!scrollTex.loadFromFile(path))
        return;
    scrollTex.setRepeated(true);
    scrollTex.setSmooth(true);
    scrollSprite0.emplace(scrollTex);
    scrollSprite1.emplace(scrollTex);
    const float w = static_cast<float>(gw.getWidth());
    const float h = static_cast<float>(gw.getHeight());
    const float texW = static_cast<float>(scrollTex.getSize().x);
    const float scale = h / static_cast<float>(scrollTex.getSize().y);
    scrollSprite0->setScale({ scale, scale });
    scrollSprite1->setScale({ scale, scale });
    scrollSprite0->setTextureRect(
        IntRect({ 0, 0 }, { static_cast<int>(texW), static_cast<int>(scrollTex.getSize().y) }));
    scrollSprite1->setTextureRect(
        IntRect({ 0, 0 }, { static_cast<int>(texW), static_cast<int>(scrollTex.getSize().y) }));
    scrollSprite0->setPosition({ 0.f, 0.f });
    scrollSprite1->setPosition({ texW * scale, 0.f });
    scrollOffset = 0.f;
    scrollActive = true;
}

void RealmOfEchoes::updateScroll(float dt) {
    if (!scrollActive || !scrollSprite0 || !scrollSprite1)
        return;
    const float spd = 40.f;
    scrollOffset -= spd * dt;
    const float pieceW = scrollSprite0->getGlobalBounds().size.x;
    if (pieceW <= 1.f)
        return;
    while (scrollOffset <= -pieceW)
        scrollOffset += pieceW;
    scrollSprite0->setPosition({ scrollOffset, 0.f });
    scrollSprite1->setPosition({ scrollOffset + pieceW, 0.f });
}

void RealmOfEchoes::drawScrollBehind() {
    if (!scrollActive || !scrollSprite0 || !scrollSprite1)
        return;
    gw.getWindow().draw(*scrollSprite0);
    gw.getWindow().draw(*scrollSprite1);
}

void RealmOfEchoes::drawBackdrop() {
    if (staticBgSpr)
        gw.getWindow().draw(*staticBgSpr);
    else
        drawScrollBehind();
}

void RealmOfEchoes::syncSave(int regionIndex, const Region& region) {
    questLog.setEncounterProgress(regionIndex, region.getEncounterIndex());
    try {
        FileManager::savePlayer(player->serialize(), questLog.serialize());
    } catch (const exception& e) {
        cerr << "[Save Error] " << e.what() << "\n";
    }
}

int RealmOfEchoes::bossCoinReward(int regionIndex) const {
    return 50 + regionIndex * 25;
}

RealmOfEchoes::RealmOfEchoes(GameWindow& window)
    : gw(window), player(nullptr), logger("saves/game_log.txt") {
    gw.setIconFromFile("assets/icon/realm_of_echoes.png");
    if (!AssetLoader::openFontWithFallback(font,
            { "assets/font/Philosopher-Bold.ttf", "assets/fonts/Philosopher-Bold.ttf" }))
        throw runtime_error("RealmOfEchoes: Cannot load font!");
    setupAudio();
    setupRegions();
    gw.setInterface(Color(13, 2, 33), "Realm of Echoes: Spirits & Shadows");
}

RealmOfEchoes::~RealmOfEchoes() {
    delete player;
    player = nullptr;
}

void RealmOfEchoes::setupRegions() {
    regions.clear();

    Region forest("THE WHISPERING FOREST", Color(27, 94, 32), 0);
    forest.addEncounter({ EncounterType::Battle, "A WILLOWISP FLICKERS IN THE MIST." });
    forest.addEncounter({ EncounterType::Negotiation, "A CALM SPIRIT OFFERS A HINT: FOLLOW THE BLUE MOTH." });
    forest.addEncounter({ EncounterType::Battle, "ROOTS TWIST LIKE FINGERS." });
    forest.addEncounter({ EncounterType::Shrine, "A MOSSY SHRINE HUMS WITH SOFT LIGHT." });
    forest.addEncounter({ EncounterType::PhantomSurvival, "A PHANTOM WRAITH CIRCLES FOR A HUNGRY STARE." });
    forest.addEncounter({ EncounterType::Battle, "THE FOREST EXHALES COLD AIR." });
    forest.addEncounter({ EncounterType::Negotiation, "A GUIDE SPIRIT MARKS A SAFE PATH ON YOUR MAP." });
    forest.addEncounter({ EncounterType::Battle, "SHADOWS MIMIC YOUR FOOTSTEPS." });
    forest.addEncounter({ EncounterType::Shrine, "RUNES ON STONE PROMISE REST." });
    forest.addEncounter({ EncounterType::BossBattle, "THE CORRUPTED HEART OF THE WOOD AWAKENS." });
    regions.push_back(forest);

    Region temples("THE SUNKEN TEMPLES", Color(0, 77, 64), 1);
    temples.addEncounter({ EncounterType::Battle, "A STONE GOLEM BLOCKS THE STAIR." });
    temples.addEncounter({ EncounterType::Battle, "WATER SPIRITS WHISPER RIDDLES." });
    temples.addEncounter({ EncounterType::Negotiation, "A PRIEST SPIRIT TEACHES A VEIL-BREATHING CHANT." });
    temples.addEncounter({ EncounterType::Shrine, "CRYSTAL WATER REFLECTS STARS BELOW." });
    temples.addEncounter({ EncounterType::PhantomSurvival, "THE WRAITH DRAINS WARMTH FROM THE TORCHES." });
    temples.addEncounter({ EncounterType::Trickster, "A FOX SPIRIT SWAPS YOUR FOOTPRINTS FOR LAUGHS." });
    temples.addEncounter({ EncounterType::Battle, "CRACKS IN THE FLOOR BETRAY OLD TRAPS." });
    temples.addEncounter({ EncounterType::Battle, "GUARDIAN MURMURS ECHO OFF THE WALLS." });
    temples.addEncounter({ EncounterType::PhantomSurvival, "THE WRAITH RETURNS BETWEEN THE PILLARS." });
    temples.addEncounter({ EncounterType::BossBattle, "THE TEMPLE GUARDIAN RISES." });
    regions.push_back(temples);

    Region mountains("THE CRIMSON MOUNTAINS", Color(183, 28, 28), 2);
    mountains.addEncounter({ EncounterType::Battle, "AN EMBERWING SCREECHES OVERHEAD." });
    mountains.addEncounter({ EncounterType::Trickster, "A HOT-WIND SPRITE DEMANDS A SILLY DANCE. YOU HUMOR IT." });
    mountains.addEncounter({ EncounterType::Battle, "LAVA VEINS PULSE UNDER THE PATH." });
    mountains.addEncounter({ EncounterType::Negotiation, "A CLIMBER SPIRIT SHARES A ROUTE AROUND THE CLIFF." });
    mountains.addEncounter({ EncounterType::Shrine, "A HOT SPRING MISTS THE AIR." });
    mountains.addEncounter({ EncounterType::PhantomSurvival, "COLD MIST CONDENSES INTO A WRAITH." });
    mountains.addEncounter({ EncounterType::Battle, "ASH STINGS YOUR EYES." });
    mountains.addEncounter({ EncounterType::Battle, "THE PEAK HOWLS LIKE A BEAST." });
    mountains.addEncounter({ EncounterType::Negotiation, "A HERMIT SPIRIT TRADES TALES FOR WARNINGS." });
    mountains.addEncounter({ EncounterType::BossBattle, "THE EMBER TYRANT DESCENDS." });
    regions.push_back(mountains);

    Region veil("THE VEIL GATE", Color(18, 0, 36), 3);
    veil.addEncounter({ EncounterType::Battle, "SHADOWS TEAR AT THE EDGES OF YOUR SOUL." });
    veil.addEncounter({ EncounterType::Battle, "THE VEIL ITSELF SHIMMERS WITH TEETH." });
    veil.addEncounter({ EncounterType::PhantomSurvival, "A WRAITH TESTS YOUR WILL IN THE VOID." });
    veil.addEncounter({ EncounterType::Negotiation, "A QUIET ANCESTOR NAMES THE SOVEREIGN'S WEAKNESS: PATIENCE." });
    veil.addEncounter({ EncounterType::Trickster, "A JESTER SPIRIT LIES FOR FUN, THEN VANISHES." });
    veil.addEncounter({ EncounterType::Battle, "ECHOES OF FAILED WALKERS CLAW AT YOU." });
    veil.addEncounter({ EncounterType::Negotiation, "A STAR-TETHERED GUIDE POINTS TRUE NORTH THROUGH THE VEIL." });
    const bool trapLeft = (rand() % 2) == 0;
    string lie = trapLeft
        ? string("THE MASTER TRICKSTER POINTS LEFT, SWEARING SALVATION WAITS THERE.")
        : string("THE MASTER TRICKSTER INSISTS THE RIGHT GATE IS THE TRUE PATH.");
    veil.addEncounter({ EncounterType::Trickster, lie });
    veil.addEncounter({ EncounterType::GateChoice, "TWO GATES OPEN. ONE IS A TRAP THAT REWINDS YOUR STEPS.", trapLeft });
    veil.addEncounter({ EncounterType::BossBattle, "THE SHADOW SOVEREIGN RISES." });
    regions.push_back(veil);
}

void RealmOfEchoes::drawCenteredText(const string& str, float y, unsigned int size, const Color& color) {
    Text text(font);
    text.setString(upperText(str));
    text.setCharacterSize(size);
    text.setFillColor(color);
    FloatRect b = text.getLocalBounds();
    text.setOrigin({ b.size.x / 2.f, b.size.y / 2.f });
    text.setPosition({ centerX(), y });
    gw.getWindow().draw(text);
}

string RealmOfEchoes::showNameEntryScreen() {
    gw.setInterface(Color(13, 2, 33), "Realm of Echoes - Enter Your Name");
    loadStaticBackground({ "assets/texture/bg_roe_story_parchment.png", "assets/textures/bg_roe_story_parchment.png" });
    string playerName;
    Text nameDisplay(font);
    nameDisplay.setCharacterSize(28);
    nameDisplay.setFillColor(Color(0, 245, 255));
    const float midY = static_cast<float>(gw.getHeight()) * 0.55f;
    while (gw.isOpen()) {
        while (auto event = gw.pollEvent()) {
            if (event->is<Event::Closed>())
                gw.getWindow().close();
            if (auto* te = event->getIf<Event::TextEntered>()) {
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
                    return playerName;
        }
        nameDisplay.setString(upperText(playerName + "_"));
        FloatRect b = nameDisplay.getLocalBounds();
        nameDisplay.setOrigin({ b.size.x / 2.f, b.size.y / 2.f });
        nameDisplay.setPosition({ centerX(), midY });
        gw.clear();
        drawBackdrop();
        drawCenteredText("ENTER YOUR NAME, VEILWALKER", static_cast<float>(gw.getHeight()) * 0.28f, 26,
            Color(240, 240, 255));
        drawCenteredText("(PRESS ENTER TO CONFIRM)", static_cast<float>(gw.getHeight()) * 0.36f, 16,
            Color(191, 0, 255));
        gw.getWindow().draw(nameDisplay);
        gw.display();
    }
    return "WANDERER";
}

void RealmOfEchoes::showStoryScreen() {
    loadStaticBackground({ "assets/texture/bg_roe_story_parchment.png", "assets/textures/bg_roe_story_parchment.png" });
    string story = questLog.getStoryIntro();
    if (story.empty())
        story = "You are the Veilwalker.\nBorn between worlds.\nOnly you can restore the balance.";
    bool done = false;
    Clock clk;
    while (gw.isOpen() && !done) {
        float dt = clk.restart().asSeconds();
        while (auto event = gw.pollEvent()) {
            if (event->is<Event::Closed>())
                gw.getWindow().close();
            if (auto* key = event->getIf<Event::KeyPressed>())
                if (key->code == Keyboard::Key::Enter) {
                    audio.playOptional("ui");
                    done = true;
                }
        }
        updateScroll(dt);
        gw.clear();
        drawBackdrop();
        istringstream ss(story);
        string line;
        float y = static_cast<float>(gw.getHeight()) * 0.12f;
        while (getline(ss, line)) {
            Text lineText(font);
            lineText.setString(upperText(line));
            lineText.setCharacterSize(16);
            lineText.setFillColor(Color(240, 240, 255));
            lineText.setPosition({ 36.f, y });
            gw.getWindow().draw(lineText);
            y += 26.f;
        }
        drawCenteredText("PRESS ENTER TO CONTINUE", static_cast<float>(gw.getHeight()) * 0.88f, 15,
            Color(191, 0, 255));
        gw.display();
    }
}

void RealmOfEchoes::showWorldMapScreen(int& selectedRegion) {
    gw.setInterface(Color(15, 15, 35), "Realm of Echoes - World Map");
    gw.setIconFromFile("assets/icon/realm_of_echoes_map.png");
    tryBindScrollForWorldMap();
    struct Option {
        string label;
        int regionIndex;
    };
    vector<Option> options;
    for (int i = 0; i < 4; i++) {
        if (questLog.isRegionUnlocked(i)) {
            string label = regions[i].getName();
            if (questLog.isRegionComplete(i))
                label += " [DONE]";
            options.push_back({ label, i });
        }
    }
    options.push_back({ "SAVE & EXIT TO ARCANE ARENA", -1 });
    vector<Text> optionTexts;
    optionTexts.reserve(options.size());
    for (size_t i = 0; i < options.size(); i++) {
        optionTexts.emplace_back(font);
        optionTexts[i].setString(upperText(options[i].label));
        optionTexts[i].setCharacterSize(22);
        FloatRect b = optionTexts[i].getLocalBounds();
        optionTexts[i].setOrigin({ b.size.x / 2.f, b.size.y / 2.f });
        optionTexts[i].setPosition({ centerX(), 120.f + static_cast<float>(i) * 48.f });
    }
    int selected = 0;
    Clock animClk;
    while (gw.isOpen() && !quitToHub) {
        float dt = animClk.restart().asSeconds();
        while (auto event = gw.pollEvent()) {
            if (event->is<Event::Closed>()) {
                gw.getWindow().close();
                return;
            }
            if (auto* key = event->getIf<Event::KeyPressed>()) {
                if (key->code == Keyboard::Key::Up) {
                    audio.playOptional("ui");
                    selected = (selected - 1 + static_cast<int>(options.size())) % static_cast<int>(options.size());
                }
                if (key->code == Keyboard::Key::Down) {
                    audio.playOptional("ui");
                    selected = (selected + 1) % static_cast<int>(options.size());
                }
                if (key->code == Keyboard::Key::Enter) {
                    audio.playOptional("ui");
                    if (options[selected].regionIndex == -1) {
                        try {
                            FileManager::savePlayer(player->serialize(), questLog.serialize());
                            FileManager::saveScore(player->getName(), player->getLevel(), player->getXP());
                            logger.showMessage(&gw, font, "GAME SAVED! RETURNING TO ARCANE ARENA...", 1.5f);
                            logger.logToFile("Player saved: " + player->getName());
                        } catch (const exception& e) {
                            cerr << "[Save Error] " << e.what() << "\n";
                        }
                        selectedRegion = -1;
                        return;
                    }
                    selectedRegion = options[selected].regionIndex;
                    return;
                }
            }
        }
        updateScroll(dt * 0.35f);
        gw.clear();
        drawBackdrop();
        drawCenteredText("THE VEILLANDS", 40.f, 30, Color(0, 245, 255));
        drawCenteredText("UP / DOWN  ENTER  (MAP)", 72.f, 14, Color(191, 0, 255));
        for (size_t i = 0; i < optionTexts.size(); i++) {
            bool isSelected = (static_cast<int>(i) == selected);
            optionTexts[i].setFillColor(isSelected ? Color(0, 245, 255) : Color(200, 200, 220));
            optionTexts[i].setCharacterSize(isSelected ? 24 : 22);
            FloatRect b = optionTexts[i].getLocalBounds();
            optionTexts[i].setOrigin({ b.size.x / 2.f, b.size.y / 2.f });
            optionTexts[i].setPosition({ centerX(), 120.f + static_cast<float>(i) * 48.f });
            gw.getWindow().draw(optionTexts[i]);
        }
        if (player) {
            Text stats(font);
            stats.setString(upperText(player->getName() + "  LV." + to_string(player->getLevel()) + "  HP " +
                    to_string(player->getHp()) + "/" + to_string(player->getMaxHp()) + "  COINS " +
                    to_string(player->getCoins())));
            stats.setCharacterSize(14);
            stats.setFillColor(Color(57, 255, 20));
            stats.setPosition({ 16.f, static_cast<float>(gw.getHeight()) - 28.f });
            gw.getWindow().draw(stats);
        }
        gw.display();
    }
}

void RealmOfEchoes::runNegotiationScreen(const string& msg) {
    bool done = false;
    Clock clk;
    while (gw.isOpen() && !done) {
        float dt = clk.restart().asSeconds();
        while (auto event = gw.pollEvent()) {
            if (event->is<Event::Closed>())
                gw.getWindow().close();
            if (auto* key = event->getIf<Event::KeyPressed>())
                if (key->code == Keyboard::Key::Enter) {
                    audio.playOptional("ui");
                    done = true;
                }
        }
        updateScroll(dt);
        gw.clear();
        drawBackdrop();
        drawCenteredText("GUIDANCE", static_cast<float>(gw.getHeight()) * 0.2f, 26, Color(191, 0, 255));
        Text body(font);
        body.setString(upperText(msg));
        body.setCharacterSize(15);
        body.setFillColor(Color(240, 240, 255));
        body.setPosition({ 40.f, static_cast<float>(gw.getHeight()) * 0.32f });
        gw.getWindow().draw(body);
        drawCenteredText("PRESS ENTER TO CONTINUE", static_cast<float>(gw.getHeight()) * 0.88f, 15,
            Color(0, 245, 255));
        gw.display();
    }
}

bool RealmOfEchoes::runGateChoice(int regionIndex, Region& region, const Encounter& enc) {
    (void)regionIndex;
    int choice = 0;
    Clock frameClk;
    const bool trapLeft = enc.leftGateIsTrap;
    Texture gateTex;
    optional<Sprite> gateLeftSpr;
    optional<Sprite> gateRightSpr;
    if (AssetLoader::loadTextureWithFallback(gateTex,
            { string(R"(assets/gates/veil_gate(final).png)"), "assets/gates/veil_gate.png",
                "assets/gates/veil_gate_final.png" })) {
        gateTex.setSmooth(true);
        gateLeftSpr.emplace(gateTex);
        gateRightSpr.emplace(gateTex);
        const float targetH = static_cast<float>(gw.getHeight()) * 0.26f;
        const float sc = targetH / static_cast<float>(gateTex.getSize().y);
        gateLeftSpr->setScale({ sc, sc });
        gateRightSpr->setScale({ sc, sc });
        const Vector2u gts = gateTex.getSize();
        gateLeftSpr->setOrigin({ gts.x * 0.5f, gts.y * 0.92f });
        gateRightSpr->setOrigin({ gts.x * 0.5f, gts.y * 0.92f });
    }
    while (gw.isOpen()) {
        float dt = frameClk.restart().asSeconds();
        updateScroll(dt);
        while (auto event = gw.pollEvent()) {
            if (event->is<Event::Closed>())
                gw.getWindow().close();
            if (auto* key = event->getIf<Event::KeyPressed>()) {
                if (key->code == Keyboard::Key::Left || key->code == Keyboard::Key::A)
                    choice = 0;
                if (key->code == Keyboard::Key::Right || key->code == Keyboard::Key::D)
                    choice = 1;
                if (key->code == Keyboard::Key::Up || key->code == Keyboard::Key::Down)
                    choice = 1 - choice;
                if (key->code == Keyboard::Key::Enter) {
                    audio.playOptional("ui");
                    const bool pickedLeft = (choice == 0);
                    const bool wrong = (pickedLeft && trapLeft) || (!pickedLeft && !trapLeft);
                    if (wrong) {
                        drawCenteredText("THE TRAP GATE CLAIMS YOU!", static_cast<float>(gw.getHeight()) * 0.4f,
                            24, Color(255, 23, 68));
                        gw.display();
                        sleep(seconds(2.f));
                        region.setEncounterIndex(0);
                        questLog.resetEncounterProgress(regionIndex);
                        syncSave(regionIndex, region);
                        return false;
                    }
                    drawCenteredText("THE TRUE PATH OPENS.", static_cast<float>(gw.getHeight()) * 0.45f, 22,
                        Color(57, 255, 20));
                    gw.display();
                    sleep(seconds(1.2f));
                    return true;
                }
            }
        }
        gw.clear();
        drawBackdrop();
        drawCenteredText("CHOOSE YOUR GATE", static_cast<float>(gw.getHeight()) * 0.12f, 24, Color(0, 245, 255));
        drawCenteredText(upperText(enc.description), static_cast<float>(gw.getHeight()) * 0.2f, 14,
            Color(240, 240, 255));
        const float yGate = static_cast<float>(gw.getHeight()) * 0.62f;
        if (gateLeftSpr && gateRightSpr) {
            const float xL = static_cast<float>(gw.getWidth()) * 0.28f;
            const float xR = static_cast<float>(gw.getWidth()) * 0.72f;
            gateLeftSpr->setPosition({ xL, yGate });
            gateRightSpr->setPosition({ xR, yGate });
            const float bump = (choice == 0) ? 1.06f : 1.f;
            const float bumpR = (choice == 1) ? 1.06f : 1.f;
            const float baseSc = static_cast<float>(gw.getHeight()) * 0.26f / static_cast<float>(gateTex.getSize().y);
            gateLeftSpr->setScale({ baseSc * bump, baseSc * bump });
            gateRightSpr->setScale({ baseSc * bumpR, baseSc * bumpR });
            gw.getWindow().draw(*gateLeftSpr);
            gw.getWindow().draw(*gateRightSpr);
        }
        Text leftLbl(font);
        leftLbl.setString(upperText(string(choice == 0 ? "> LEFT" : "  LEFT")));
        leftLbl.setCharacterSize(18);
        leftLbl.setFillColor(choice == 0 ? Color(0, 245, 255) : Color(191, 0, 255));
        FloatRect lb = leftLbl.getLocalBounds();
        leftLbl.setOrigin({ lb.size.x / 2.f, lb.size.y / 2.f });
        leftLbl.setPosition({ static_cast<float>(gw.getWidth()) * 0.28f, yGate + static_cast<float>(gw.getHeight()) * 0.14f });
        gw.getWindow().draw(leftLbl);
        Text rightLbl(font);
        rightLbl.setString(upperText(string(choice == 1 ? "> RIGHT" : "  RIGHT")));
        rightLbl.setCharacterSize(18);
        rightLbl.setFillColor(choice == 1 ? Color(0, 245, 255) : Color(191, 0, 255));
        lb = rightLbl.getLocalBounds();
        rightLbl.setOrigin({ lb.size.x / 2.f, lb.size.y / 2.f });
        rightLbl.setPosition({ static_cast<float>(gw.getWidth()) * 0.72f, yGate + static_cast<float>(gw.getHeight()) * 0.14f });
        gw.getWindow().draw(rightLbl);
        drawCenteredText("ARROWS / A-D  ENTER", static_cast<float>(gw.getHeight()) * 0.92f, 13,
            Color(255, 109, 0));
        gw.display();
    }
    return false;
}

void RealmOfEchoes::runPhantomSurvival(int regionIndex, Region& region) {
    const int hpStart = player->getHp();
    Clock timer;
    Clock hitClock;
    Clock frameClk;
    float hitInterval = 0.7f;
    bool done = false;
    while (gw.isOpen() && !done) {
        float dt = timer.getElapsedTime().asSeconds();
        float frameDt = frameClk.restart().asSeconds();
        updateScroll(frameDt);
        while (auto event = gw.pollEvent()) {
            if (event->is<Event::Closed>())
                gw.getWindow().close();
            if (auto* key = event->getIf<Event::KeyPressed>())
                if (key->code == Keyboard::Key::Space || key->code == Keyboard::Key::Enter)
                    audio.playOptional("ui");
        }
        if (dt >= kSurvivalSeconds)
            done = true;
        if (hitClock.getElapsedTime().asSeconds() >= hitInterval) {
            hitClock.restart();
            player->takeRawDamage(1);
            audio.playOptional("hit");
        }
        gw.clear();
        drawBackdrop();
        drawCenteredText("SURVIVE THE WRAITH", static_cast<float>(gw.getHeight()) * 0.15f, 22,
            Color(255, 109, 0));
        ostringstream oss;
        oss.setf(ios::fixed);
        oss.precision(1);
        oss << "TIME LEFT: " << (kSurvivalSeconds - dt);
        drawCenteredText(oss.str(), static_cast<float>(gw.getHeight()) * 0.28f, 18, Color(0, 245, 255));
        drawCenteredText("HOLD YOUR GROUND. EACH STING TAKES 1 HP.", static_cast<float>(gw.getHeight()) * 0.38f, 13,
            Color(240, 240, 255));
        ostringstream hp;
        hp << "HP: " << player->getHp() << "/" << player->getMaxHp();
        drawCenteredText(hp.str(), static_cast<float>(gw.getHeight()) * 0.5f, 18, Color(57, 255, 20));
        gw.display();
        if (!player->isAlive()) {
            done = true;
        }
    }
    if (player->getHp() < hpStart)
        player->removeCoins(5);
    showShrineScreen(regionIndex, true);
}

void RealmOfEchoes::showShrineScreen(int regionIndex, bool fullRestore) {
    gw.setInterface(Color(13, 2, 33), "Realm of Echoes - Ancient Shrine");
    const bool shrineArt = loadStaticBackground(
            { "assets/texture/shrine.jpg", "assets/textures/shrine.jpg", "assets/texture/shrine.png" });
    if (!shrineArt)
        tryBindScrollForRegion(regionIndex);
    if (fullRestore) {
        player->heal(player->getMaxHp());
        player->restoreEnergy(player->getMaxEnergy());
    } else {
        player->heal(40);
        player->restoreEnergy(player->getMaxEnergy());
    }
    bool done = false;
    Clock clk;
    while (gw.isOpen() && !done) {
        float dt = clk.restart().asSeconds();
        while (auto event = gw.pollEvent()) {
            if (event->is<Event::Closed>())
                gw.getWindow().close();
            if (auto* key = event->getIf<Event::KeyPressed>())
                if (key->code == Keyboard::Key::Enter) {
                    audio.playOptional("ui");
                    done = true;
                }
        }
        updateScroll(dt);
        gw.clear();
        drawBackdrop();
        drawCenteredText("ANCIENT SHRINE", static_cast<float>(gw.getHeight()) * 0.28f, 28, Color(191, 0, 255));
        drawCenteredText("YOU REST. VITALITY RETURNS.", static_cast<float>(gw.getHeight()) * 0.42f, 16,
            Color(240, 240, 255));
        drawCenteredText("PRESS ENTER TO CONTINUE", static_cast<float>(gw.getHeight()) * 0.88f, 15,
            Color(0, 245, 255));
        gw.display();
    }
    if (shrineArt) {
        clearStaticBackground();
        tryBindScrollForRegion(regionIndex);
    }
}

PostBattleChoice RealmOfEchoes::runPostBattleChoice(BattleResult result) {
    int choice = 0;
    Clock cooldown;
    const float h = static_cast<float>(gw.getHeight());
    while (gw.isOpen()) {
        while (auto event = gw.pollEvent()) {
            if (event->is<Event::Closed>()) {
                gw.getWindow().close();
                return PostBattleChoice::ExitToHub;
            }
            if (auto* key = event->getIf<Event::KeyPressed>()) {
                if (cooldown.getElapsedTime().asMilliseconds() > 140) {
                    cooldown.restart();
                    if (key->code == Keyboard::Key::Up)
                        choice = (choice + 2) % 3;
                    if (key->code == Keyboard::Key::Down)
                        choice = (choice + 1) % 3;
                    if (key->code == Keyboard::Key::Enter) {
                        audio.playOptional("ui");
                        if (choice == 0)
                            return PostBattleChoice::ContinuePath;
                        if (choice == 1)
                            return PostBattleChoice::PlayAgain;
                        quitToHub = true;
                        try {
                            FileManager::savePlayer(player->serialize(), questLog.serialize());
                            FileManager::saveScore(player->getName(), player->getLevel(), player->getXP());
                        } catch (const exception& e) {
                            cerr << "[Save Error] " << e.what() << "\n";
                        }
                        return PostBattleChoice::ExitToHub;
                    }
                }
            }
        }
        gw.clear();
        drawBackdrop();
        drawCenteredText(result == BattleResult::PlayerWon ? "BATTLE WON" : "BATTLE LOST", h * 0.18f, 26,
            result == BattleResult::PlayerWon ? Color(57, 255, 20) : Color(255, 23, 68));
        drawCenteredText("CHOOSE NEXT ACTION", h * 0.28f, 14, Color(240, 240, 255));
        drawCenteredText((choice == 0 ? "> " : "  ") + string("CONTINUE PATH"), h * 0.42f, 16,
            Color(0, 245, 255));
        drawCenteredText((choice == 1 ? "> " : "  ") + string("PLAY AGAIN (SAME STATE)"), h * 0.52f, 16,
            Color(191, 0, 255));
        drawCenteredText((choice == 2 ? "> " : "  ") + string("EXIT TO ARCANE ARENA"), h * 0.62f, 16,
            Color(255, 109, 0));
        gw.display();
    }
    return PostBattleChoice::ExitToHub;
}

void RealmOfEchoes::runRegion(int regionIndex) {
    Region& region = regions[regionIndex];
    region.setEncounterIndex(questLog.getEncounterProgress(regionIndex));
    tryBindScrollForRegion(regionIndex);
    gw.setInterface(region.getThemeColor(), "Realm of Echoes - " + region.getName());
    Clock frameClk;
    while (gw.isOpen() && !quitToHub) {
        if (region.getEncounterIndex() >= region.getEncounterCount())
            break;
        float dt = frameClk.restart().asSeconds();
        updateScroll(dt);
        const Encounter& enc = region.peekEncounter();

        if (enc.type == EncounterType::Shrine) {
            showShrineScreen(regionIndex, false);
            region.advanceEncounter();
            syncSave(regionIndex, region);
            continue;
        }
        if (enc.type == EncounterType::Negotiation) {
            runNegotiationScreen(enc.description);
            region.advanceEncounter();
            syncSave(regionIndex, region);
            continue;
        }
        if (enc.type == EncounterType::Trickster) {
            Text trickBody(font);
            trickBody.setString(upperText(enc.description));
            trickBody.setCharacterSize(14);
            trickBody.setFillColor(Color(240, 240, 255));
            bool done = false;
            while (gw.isOpen() && !done) {
                float t = frameClk.restart().asSeconds();
                while (auto event = gw.pollEvent()) {
                    if (event->is<Event::Closed>())
                        gw.getWindow().close();
                    if (auto* key = event->getIf<Event::KeyPressed>())
                        if (key->code == Keyboard::Key::Enter) {
                            audio.playOptional("ui");
                            done = true;
                        }
                }
                updateScroll(t);
                gw.clear();
                drawBackdrop();
                drawCenteredText("TRICKSTER", static_cast<float>(gw.getHeight()) * 0.18f, 26,
                    Color(255, 109, 0));
                trickBody.setPosition({ 36.f, static_cast<float>(gw.getHeight()) * 0.3f });
                gw.getWindow().draw(trickBody);
                drawCenteredText("PRESS ENTER", static_cast<float>(gw.getHeight()) * 0.88f, 14,
                    Color(0, 245, 255));
                gw.display();
            }
            region.advanceEncounter();
            syncSave(regionIndex, region);
            continue;
        }
        if (enc.type == EncounterType::PhantomSurvival) {
            runPhantomSurvival(regionIndex, region);
            region.advanceEncounter();
            syncSave(regionIndex, region);
            continue;
        }
        if (enc.type == EncounterType::GateChoice) {
            if (runGateChoice(regionIndex, region, enc))
                region.advanceEncounter();
            syncSave(regionIndex, region);
            continue;
        }
        if (enc.type == EncounterType::Battle) {
            Spirit* enemy = region.createRandomEnemy();
            gw.setInterface(region.getThemeColor(), "Battle: " + enemy->getName());
            Battle battle(gw, *player, *enemy);
            BattleResult result = battle.run();
            delete enemy;
            if (result == BattleResult::PlayerExited) {
                try {
                    FileManager::savePlayer(player->serialize(), questLog.serialize());
                    FileManager::saveScore(player->getName(), player->getLevel(), player->getXP());
                    logger.showMessage(&gw, font, "GAME SAVED! RETURNING TO MAP...", 1.4f);
                } catch (const exception& e) {
                    cerr << "[Save Error] " << e.what() << "\n";
                }
                return;
            }
            if (result == BattleResult::PlayerDied) {
                gw.clear();
                drawBackdrop();
                drawCenteredText("YOU HAVE FALLEN...", static_cast<float>(gw.getHeight()) * 0.42f, 28,
                    Color(255, 23, 68));
                gw.display();
                sleep(seconds(2.f));
            }
            try {
                player->getInventory().addItem(Item("Healing Potion", ItemType::HealingPotion, 30));
            } catch (...) {
            }
            PostBattleChoice pb = runPostBattleChoice(result);
            if (pb == PostBattleChoice::ExitToHub || quitToHub)
                return;
            if (pb == PostBattleChoice::PlayAgain)
                continue;
            region.advanceEncounter();
            syncSave(regionIndex, region);
            continue;
        }
        if (enc.type == EncounterType::BossBattle) {
            audio.playOptional("boss", 55.f);
            Spirit* boss = region.createBoss();
            gw.setInterface(Color(13, 2, 33), "BOSS: " + boss->getName());
            Battle bossBattle(gw, *player, *boss);
            BattleResult result = bossBattle.run();
            delete boss;
            if (result == BattleResult::PlayerExited) {
                try {
                    FileManager::savePlayer(player->serialize(), questLog.serialize());
                    FileManager::saveScore(player->getName(), player->getLevel(), player->getXP());
                    logger.showMessage(&gw, font, "GAME SAVED! RETURNING TO MAP...", 1.4f);
                } catch (const exception& e) {
                    cerr << "[Save Error] " << e.what() << "\n";
                }
                return;
            }
            if (result == BattleResult::PlayerDied) {
                player->heal(player->getMaxHp());
                gw.clear();
                drawBackdrop();
                drawCenteredText("THE BOSS DEFEATED YOU!", static_cast<float>(gw.getHeight()) * 0.4f, 24,
                    Color(255, 109, 0));
                drawCenteredText("YOU AWAKEN AT FULL STRENGTH.", static_cast<float>(gw.getHeight()) * 0.5f, 16,
                    Color(57, 255, 20));
                gw.display();
                sleep(seconds(2.5f));
                syncSave(regionIndex, region);
                return;
            }
            region.advanceEncounter();
            player->addCoins(bossCoinReward(regionIndex));
            region.markComplete();
            questLog.completeRegion(regionIndex);
            questLog.setCurrentRegion(min(3, regionIndex + 1));
            showRegionCompleteScreen(regionIndex);
            try {
                FileManager::savePlayer(player->serialize(), questLog.serialize());
                FileManager::saveScore(player->getName(), player->getLevel(), player->getXP());
                logger.logToFile("Region " + to_string(regionIndex) + " completed by " + player->getName());
            } catch (const exception& e) {
                cerr << "[Save Error] " << e.what() << "\n";
            }
            if (questLog.isGameComplete())
                showEndingScreen();
            return;
        }
    }
}

void RealmOfEchoes::showRegionCompleteScreen(int regionIndex) {
    const string names[] = { "WHISPERING FOREST", "SUNKEN TEMPLES", "CRIMSON MOUNTAINS", "VEIL GATE" };
    bool done = false;
    Clock clk;
    while (gw.isOpen() && !done) {
        float dt = clk.restart().asSeconds();
        while (auto event = gw.pollEvent()) {
            if (event->is<Event::Closed>())
                gw.getWindow().close();
            if (auto* key = event->getIf<Event::KeyPressed>())
                if (key->code == Keyboard::Key::Enter) {
                    audio.playOptional("ui");
                    done = true;
                }
        }
        updateScroll(dt);
        gw.clear();
        drawBackdrop();
        drawCenteredText("REGION CLEARED!", static_cast<float>(gw.getHeight()) * 0.28f, 26,
            Color(57, 255, 20));
        drawCenteredText(names[regionIndex] + string(" SECURED."), static_cast<float>(gw.getHeight()) * 0.42f, 16,
            Color(240, 240, 255));
        drawCenteredText("PRESS ENTER FOR THE VEILLANDS.", static_cast<float>(gw.getHeight()) * 0.88f, 14,
            Color(191, 0, 255));
        gw.display();
    }
}

void RealmOfEchoes::showEndingScreen() {
    bool done = false;
    Clock clk;
    while (gw.isOpen() && !done) {
        float dt = clk.restart().asSeconds();
        while (auto event = gw.pollEvent()) {
            if (event->is<Event::Closed>())
                gw.getWindow().close();
            if (auto* key = event->getIf<Event::KeyPressed>())
                if (key->code == Keyboard::Key::Enter) {
                    audio.playOptional("ui");
                    done = true;
                }
        }
        updateScroll(dt);
        gw.clear();
        drawBackdrop();
        drawCenteredText("THE SHADOW SOVEREIGN FALLS!", static_cast<float>(gw.getHeight()) * 0.2f, 22,
            Color(0, 245, 255));
        drawCenteredText("BALANCE RETURNS TO THE VEILLANDS.", static_cast<float>(gw.getHeight()) * 0.32f, 15,
            Color(240, 240, 255));
        drawCenteredText(upperText("VEILWALKER: " + player->getName()),
            static_cast<float>(gw.getHeight()) * 0.48f, 16, Color(57, 255, 20));
        drawCenteredText("PRESS ENTER FOR ARCANE ARENA.", static_cast<float>(gw.getHeight()) * 0.88f, 14,
            Color(191, 0, 255));
        gw.display();
    }
}

void RealmOfEchoes::run() {
    quitToHub = false;
    string playerData, questData;
    FileManager::LoadStatus loadStatus = FileManager::loadPlayer(playerData, questData);
    if (loadStatus != FileManager::LoadStatus::NotFound) {
        loadStaticBackground({ "assets/texture/bg_roe_story_parchment.png", "assets/textures/bg_roe_story_parchment.png" });
        bool decided = false, loadSave = false;
        while (gw.isOpen() && !decided) {
            while (auto event = gw.pollEvent()) {
                if (event->is<Event::Closed>())
                    gw.getWindow().close();
                if (auto* key = event->getIf<Event::KeyPressed>()) {
                    if (key->code == Keyboard::Key::L) {
                        loadSave = (loadStatus == FileManager::LoadStatus::Ok);
                        decided = true;
                    }
                    if (key->code == Keyboard::Key::N) {
                        decided = true;
                    }
                }
            }
            gw.clear();
            drawBackdrop();
            if (loadStatus == FileManager::LoadStatus::Ok) {
                drawCenteredText("SAVE FILE FOUND!", static_cast<float>(gw.getHeight()) * 0.38f, 24,
                    Color(0, 245, 255));
                drawCenteredText("L = LOAD    N = NEW GAME", static_cast<float>(gw.getHeight()) * 0.48f, 16,
                    Color(240, 240, 255));
            } else {
                drawCenteredText("SAVE FILE IS CORRUPT", static_cast<float>(gw.getHeight()) * 0.36f, 24,
                    Color(255, 109, 0));
                drawCenteredText("PRESS N FOR NEW GAME", static_cast<float>(gw.getHeight()) * 0.48f, 16,
                    Color(240, 240, 255));
            }
            gw.display();
        }
        if (loadSave) {
            player = new Veilwalker("Loading...");
            try {
                player->deserialize(playerData);
                questLog.deserialize(questData);
            } catch (const exception& e) {
                delete player;
                player = nullptr;
                logger.showMessage(&gw, font, string("SAVE CORRUPT: ") + e.what(), 2.0f);
                loadSave = false;
            }
            if (loadSave && player) {
                try {
                    questLog.loadStory("assets/data/story.txt");
                } catch (...) {
                }
                logger.showMessage(&gw, font, "WELCOME BACK, " + upperText(player->getName()) + "!", 1.8f);
                while (gw.isOpen() && !quitToHub) {
                    int regionSelect = 0;
                    showWorldMapScreen(regionSelect);
                    if (regionSelect == -1)
                        return;
                    if (!questLog.isRegionUnlocked(regionSelect))
                        continue;
                    runRegion(regionSelect);
                    if (quitToHub)
                        return;
                    if (questLog.isGameComplete())
                        break;
                }
                return;
            }
        }
    }

    string pName = showNameEntryScreen();
    player = new Veilwalker(pName);
    player->getInventory().addItem(Item("Healing Potion", ItemType::HealingPotion, 30, 2));
    player->getInventory().addItem(Item("Spirit Crystal", ItemType::SpiritCrystal, 20, 1));
    player->addCoins(10);
    try {
        questLog.loadStory("assets/data/story.txt");
    } catch (...) {
    }
    showStoryScreen();
    tryBindScrollForRegion(0);
    while (gw.isOpen() && !quitToHub) {
        int selectedRegion = 0;
        tryBindScrollForRegion(-1);
        showWorldMapScreen(selectedRegion);
        if (selectedRegion == -1)
            break;
        if (!questLog.isRegionUnlocked(selectedRegion))
            continue;
        tryBindScrollForRegion(selectedRegion);
        runRegion(selectedRegion);
        if (quitToHub)
            return;
        if (questLog.isGameComplete())
            break;
    }
}
