// GameLogger.cpp

#include "GameLogger.h"
#include "AssetLoader.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace sf;

// ── Constructor ───────────────────────────────────────────────────────────────
GameLogger::GameLogger(const string& filePath) : logFilePath(filePath)
{
    ofstream testOpen(filePath, ios::app);
    if (!testOpen.is_open())
        throw runtime_error("GameLogger: Cannot open log file: " + filePath);
    testOpen.close();
    cout << "[GameLogger] Logger initialized. Log: " << filePath << endl;
}

// ── Destructor ────────────────────────────────────────────────────────────────
GameLogger::~GameLogger()
{
    cout << "[GameLogger] Logger destroyed.\n";
}

// ── Show a timed full-screen message ─────────────────────────────────────────
// ASSOCIATION: GameLogger USES GameWindow for one call only; does not store it.
void GameLogger::showMessage(GameWindow* gw,
    Font& font,
    const string& message,
    float seconds,
    Texture& texture,
    vector<string> path,
    optional<sf::Sprite>& sprite,   // reference, not copy
    int width,
    int height)
{
    if (gw == nullptr)
        throw invalid_argument("GameLogger::showMessage - null window pointer!");

    // Load background sprite into the caller's optional (via reference).
    AssetLoader::emplaceCoverSprite(texture, sprite, path,
        static_cast<float>(width),
        static_cast<float>(height));

    // Load display font (uses the caller's font reference — no override here
    // to avoid changing the caller's font after the call returns).
    Font displayFont;
    if (!AssetLoader::openFontWithFallback(displayFont, {
            "assets/font/ActionSpectral.ttf",
            "assets/font/Philosopher-Bold.ttf" }))
    {
        // Fallback: use whatever the caller's font already is.
        displayFont = font;
    }

    // Build the text label.
    Text text(displayFont);
    text.setString(message);
    text.setCharacterSize(32);
    text.setFillColor(Color(212, 168, 92));
    text.setOutlineColor(Color(52, 34, 26));
    text.setOutlineThickness(3.f);
    FloatRect b = text.getLocalBounds();
    text.setOrigin({ b.size.x / 2.f, b.size.y / 2.f });
    text.setPosition({ static_cast<float>(width) * 0.5f,
                       static_cast<float>(height) * 0.45f });

    // Display loop for the given duration.
    Clock timer;
    while (gw->isOpen() && timer.getElapsedTime().asSeconds() < seconds)
    {
        while (auto event = gw->pollEvent())
        {
            if (event->is<Event::Closed>())
                gw->getWindow().close();
        }
        gw->clear();
        if (sprite)
            gw->getWindow().draw(*sprite);
        gw->getWindow().draw(text);
        gw->display();
    }

    logToFile(message);
}

// ── Write to log file (FILE HANDLING) ────────────────────────────────────────
void GameLogger::logToFile(const string& message)
{
    ofstream file(logFilePath, ios::app);
    if (!file.is_open())
    {
        cerr << "[GameLogger] Warning: Could not write to log file.\n";
        return;
    }
    file << message << endl;
    file.close();
}