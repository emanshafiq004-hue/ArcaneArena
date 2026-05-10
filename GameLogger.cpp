#include "GameLogger.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace sf;

GameLogger::GameLogger(const string& filePath) : logFilePath(filePath) 
{
ofstream testOpen(filePath, ios::app);
if (!testOpen.is_open())
throw runtime_error("GameLogger: Cannot open log file: " + filePath);//concatination
testOpen.close();
cout << "[GameLogger] Logger initialized. Log: " << filePath <<endl;
}

GameLogger::~GameLogger()
{
cout << "[GameLogger] Logger destroyed.\n";
}

void GameLogger::showMessage(GameWindow* gw,const Font& font,const string& message,float seconds) 
{
if (gw == nullptr)
throw invalid_argument("GameLogger::showMessage - null window pointer!");
Text text(font);
text.setString(message);
text.setCharacterSize(28);
text.setFillColor(Color(0, 245, 255));
FloatRect b = text.getLocalBounds();
text.setOrigin({ b.size.x / 2.f, b.size.y / 2.f });
text.setPosition({ 640.f, 360.f });
Clock timer;
while (gw->isOpen() && timer.getElapsedTime().asSeconds() < seconds) {
while (auto event = gw->pollEvent()) {
if (event->is<Event::Closed>())
gw->getWindow().close();
}
gw->clear();
gw->getWindow().draw(text);
gw->display();
}
logToFile(message);
}

void GameLogger::logToFile(const string& message) {
ofstream file(logFilePath, ios::app);
if (!file.is_open()) {
cerr << "[GameLogger] Warning: Could not write to log file.\n";
return;
}
file << message << "\n";
file.close();
}