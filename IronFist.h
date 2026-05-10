// IronFist.h

#pragma once
#include "GameWindow.h"
#include <stdexcept>

class IronFist {
private:
GameWindow& gw;
Font font;
public:
IronFist(GameWindow& w) : gw(w) {
if (!font.openFromFile("assets/font/Philosopher-Bold.ttf"))
throw runtime_error("IronFist: Cannot load font!");
gw.setInterface(Color(30, 10, 10), "Iron Fist Chronicles - Coming Soon");
}
void run() {
Text t(font);
t.setString("Iron Fist Chronicles\n\nComing Soon!\n\nPress ESC to return.");
t.setCharacterSize(32);
t.setFillColor(Color(255, 100, 100));
t.setPosition({ 200.f, 250.f });
while (gw.isOpen()) {
while (auto event = gw.pollEvent()) {
if (event->is<Event::Closed>())
gw.getWindow().close();
if (auto* key = event->getIf<Event::KeyPressed>())
if (key->code == Keyboard::Key::Escape)
return;
}
gw.clear();
gw.getWindow().draw(t);
gw.display();
}
}
};