// GameScoresScreen.h
// Scores & Status screen for Arcane Arena.
// Shows a 2x2 grid of game tiles. First row: Realm of Echoes, Iron Fist.
// Second row: Shatter Shift (placeholder), Sky Surge (placeholder).
// Selecting a tile shows that game's scores. ESC returns from scores view.
// Arrow keys + Enter + mouse all work for navigation.
// Background uses assets/texture/utility.png.
// Audio uses the same AudioManager as the main menu.

#pragma once
#include "GameWindow.h"
#include "AudioManager.h"
#include "FileManager.h"
#include "scoreboard.h"
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>

class GameScoresScreen
{
private:
    GameWindow& gw;
    AudioManager& audio;
    sf::Font               font;
    sf::Texture            bgTex;
    std::optional<sf::Sprite> bgSprite;

    // Grid layout: 2 rows x 2 columns.
    static const int ROWS = 2;
    static const int COLS = 2;

    int selectedRow = 0;
    int selectedCol = 0;

    // Game names in grid order.
    std::string gameNames[ROWS][COLS] =
    {
        { "Realm of Echoes", "Iron Fist"  },
        { "Shatter Shift",   "Sky Surge"  }
    };

    bool        showingScores = false;
    std::string currentGameName;

    // Cooldown clock prevents key repeat being too fast.
    sf::Clock navCooldown;

    void drawMenu();
    void drawScores();
    void handleInput();

public:
    GameScoresScreen(GameWindow& window, AudioManager& audioManager);
    void run();
};