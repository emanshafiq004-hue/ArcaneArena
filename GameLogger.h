// GameLogger.h
// ASSOCIATION: GameLogger USES GameWindow temporarily to display messages.

#pragma once
#include "GameWindow.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
#include <optional>
#include <vector>

class GameLogger
{
private:
    std::string logFilePath;

public:
    // Constructor only needs the log file path, NOT the window.
    GameLogger(const std::string& filePath);
    ~GameLogger();

    // Display a full-screen message for 'seconds' using the given background.
    // FIX: sprite is now passed by reference so the caller's optional is updated.
    void showMessage(GameWindow* gw,
        sf::Font& font,
        const std::string& message,
        float seconds,
        sf::Texture& texture,
        std::vector<std::string> path,
        std::optional<sf::Sprite>& sprite,
        int width,
        int height);

    // Write a line to the log file (FILE HANDLING).
    void logToFile(const std::string& message);
};