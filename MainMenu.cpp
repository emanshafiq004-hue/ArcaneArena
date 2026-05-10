// MainMenu.cpp

#include "MainMenu.h"
#include "AssetLoader.h"
#include <stdexcept>

static const string labels[MainMenu::ITEM_COUNT] = {
"    REALM OF ECHOES",
"  IRON FIST",
"  SHATTER SHIFT",
"  SKY SURGE",
"   SCORES & STATUS",
"  EXIT"
};

MainMenu::MainMenu(GameWindow& window,AudioManager& audio) : gw(window), headerText(font), selectedIndex(0),Audio(audio) 
{
gw.setInterface(Color(26, 10, 59), "ARCANE ARENA - MAIN MENU");
gw.setIconFromFile("assets/icon/arcane_arena_menu.png");
if (!AssetLoader::openFontWithFallback(font, { "assets/font/MainMenuPlayfairDisplay.ttf", "assets/font/Philosopher-Bold.ttf" }))
throw runtime_error("MainMenu: Cannot load font!");
AssetLoader::emplaceCoverSprite(bgTex, bgSprite,{ "assets/texture/menu.png", "assets/texture/bg_arcane_arena_menu.png","assets/texture/default.png" },static_cast<float>(gw.getWidth()), static_cast<float>(gw.getHeight()));
const float cx = static_cast<float>(gw.getWidth()) * 0.5f;
headerText.setFont(font);
headerText.setString("ARCANE ARENA");
headerText.setCharacterSize(30);
headerText.setFillColor(Color(205, 185, 145));
headerText.setOutlineColor(Color(35, 0, 55));
headerText.setOutlineThickness(2.0f);
headerText.setStyle(Text::Bold);
FloatRect hb = headerText.getLocalBounds();
headerText.setOrigin({ hb.size.x / 2.f, hb.size.y / 2.f });
headerText.setPosition({ cx, static_cast<float>(gw.getHeight()) * 0.18f });
menuItems.reserve(ITEM_COUNT);
for (int i = 0; i < ITEM_COUNT; i++) 
{
menuItems.emplace_back(font);
menuItems[i].setString(labels[i]);
menuItems[i].setCharacterSize(20);
menuItems[i].setFillColor(Color(185, 170, 140));
menuItems[i].setOutlineColor(Color(35, 0, 55));
menuItems[i].setOutlineThickness(1.0f);
FloatRect b = menuItems[i].getLocalBounds();
menuItems[i].setOrigin({ b.size.x / 2.f, b.size.y / 2.f });
if (menuItems[i].getString() == "    REALM OF ECHOES")
menuItems[i].setPosition({ cx, static_cast<float>(gw.getHeight()) * 0.35f});
else if(menuItems[i].getString()=="  IRON FIST")
menuItems[i].setPosition({ cx, static_cast<float>(gw.getHeight()) * 0.44f });
else if (menuItems[i].getString() == "  SHATTER SHIFT")
menuItems[i].setPosition({ cx, static_cast<float>(gw.getHeight()) * 0.53f });
else if (menuItems[i].getString() == "  SKY SURGE")
menuItems[i].setPosition({ cx, static_cast<float>(gw.getHeight()) * 0.62f });
else if (menuItems[i].getString() == "   SCORES & STATUS")
menuItems[i].setPosition({ cx, static_cast<float>(gw.getHeight()) * 0.72f });
else
menuItems[i].setPosition({ cx, static_cast<float>(gw.getHeight()) * 0.81f });
}
updateHighlight();
}

void MainMenu::updateHighlight() 
{
for (int i = 0; i < ITEM_COUNT; i++) 
{
menuItems[i].setFillColor(i == selectedIndex? Color(155, 110, 185): Color(185, 170, 140));
}
}

int MainMenu::run() 
{
while (gw.isOpen()) 
{
while (auto event = gw.pollEvent()) 
{
if (event->is<Event::Closed>()) 
{
gw.getWindow().close();
return 0;
}
if (auto* key = event->getIf<Event::KeyPressed>()) 
{
if (key->code == Keyboard::Key::Up) 
{
selectedIndex = (selectedIndex - 1 + ITEM_COUNT) % ITEM_COUNT;
updateHighlight();
}
if (key->code == Keyboard::Key::Down)
{
selectedIndex = (selectedIndex + 1) % ITEM_COUNT;
updateHighlight();
}
if (key->code == Keyboard::Key::Enter) 
{
Audio.playOptional("Click", 80.f);
if (selectedIndex == 5)
return 0;
return selectedIndex + 1;
}
}
Vector2i mousePos = Mouse::getPosition(gw.getWindow());
for (int i = 0; i < ITEM_COUNT; i++)
{
if (menuItems[i].getGlobalBounds().contains(static_cast<Vector2f>(mousePos)))
{
selectedIndex = i;
updateHighlight();
if (auto* mouse = event->getIf<Event::MouseButtonPressed>())
{
if (mouse->button == Mouse::Button::Left)
{
Audio.playOptional("Click", 80.f);
if (selectedIndex == 5)
return 0;
return selectedIndex + 1;
}
}
}
}
}
gw.clear();
if (bgSprite)
gw.getWindow().draw(*bgSprite);
gw.getWindow().draw(headerText);
for (auto& item : menuItems)
gw.getWindow().draw(item);
gw.display();
}
return 0;
}