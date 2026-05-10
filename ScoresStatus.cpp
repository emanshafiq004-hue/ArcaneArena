#include "ScoresStatus.h"
#include "FileManager.h"
#include "AssetLoader.h"
#include <stdexcept>

using namespace std;
using namespace sf;

ScoresStatus::ScoresStatus(GameWindow& window) : gw(window) {
gw.setInterface(Color(13, 2, 33), "ARCANE ARENA - SCORES AND STATUS");
gw.setIconFromFile("assets/icon/arcane_arena_scores.png");
if (!AssetLoader::openFontWithFallback(font, { "assets/font/Philosopher-Bold.ttf", "assets/fonts/Philosopher-Bold.ttf" }))
throw runtime_error("ScoresStatus: Cannot load font!");
AssetLoader::emplaceCoverSprite(bgTex, bgSprite,
        { "assets/texture/bg_arcane_arena_menu.png", "assets/textures/bg_arcane_arena_menu.png" },
        static_cast<float>(gw.getWidth()), static_cast<float>(gw.getHeight()));
scoresData = FileManager::loadScores();
buildUI();
}

ScoresStatus::~ScoresStatus() {
for (Text* ptr : textObjects) {
delete ptr;
}
textObjects.clear();
}

Text* ScoresStatus::createText(const string& str, unsigned int size, const Color& color,float x, float y) {
Text* t = new Text(font);
t->setString(str);
t->setCharacterSize(size);
t->setFillColor(color);
t->setPosition({ x, y });
textObjects.push_back(t);
return t;
}

void ScoresStatus::buildUI() {
const float cx = static_cast<float>(gw.getWidth()) * 0.5f;
Text* title = createText("SCORES AND STATUS", 48, Color(0, 245, 255), 0.f, 60.f);
FloatRect tb = title->getLocalBounds();
title->setOrigin({ tb.size.x / 2.f, tb.size.y / 2.f });
title->setPosition({ cx, 70.f });
createText("------------------------------------------------", 20, Color(26, 10, 59), 48.f, 118.f);
createText("REALM OF ECHOES - TOP PLAYERS:", 28, Color(191, 0, 255), 60.f, 150.f);
float yPos = 200.f;
const float LINE_SPACING = 38.f;
string line, remaining = scoresData;
size_t pos;
while (!remaining.empty()) {
pos = remaining.find('\n');
if (pos == string::npos) {
line = remaining;
remaining.clear();
}
else {
line = remaining.substr(0, pos);
remaining = remaining.substr(pos + 1);
}
if (line.empty())
continue;
Color rowColor = (textObjects.size() % 2 == 0)? Color(240, 240, 255): Color(180, 180, 220);
createText(line, 22, rowColor, 120.f, yPos);
yPos += LINE_SPACING;
if (yPos > static_cast<float>(gw.getHeight()) - 120.f) {
createText("... and more. Check saves/scores.dat for full list.", 18, Color(191, 0, 255), 60.f, yPos);
break;
}
}
const float bot = static_cast<float>(gw.getHeight());
createText("Iron Fist Chronicles - Coming Soon", 22, Color(100, 100, 100), 60.f, bot - 95.f);
createText("[ ESC ] Return to Main Menu", 20, Color(255, 109, 0), 60.f, bot - 48.f);
}

void ScoresStatus::run() {
while (gw.isOpen()) {
while (auto event = gw.pollEvent()) {
if (event->is<Event::Closed>()) {
gw.getWindow().close();
return;
}
if (auto* key = event->getIf<Event::KeyPressed>()) {
if (key->code == Keyboard::Key::Escape) {
return;
}
}
}
gw.clear();
if (bgSprite)
gw.getWindow().draw(*bgSprite);
for (Text* ptr : textObjects)
gw.getWindow().draw(*ptr);
gw.display();
}
}