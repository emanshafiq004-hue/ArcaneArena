// GameLogger.h
// ASSOCIATION: GameLogger USES GameWindow temporarily to display messages.
// It does NOT own the window, does NOT store it as a member.
// This is pure association... a "uses-a" relationship, the weakest OOP link.
// Unlike aggregation (stores a reference) or composition (owns the object).

#pragma once
#include "GameWindow.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>

class GameLogger {
private:
    string logFilePath;

public:
    // Constructor — only needs the log file path, NOT the window
    GameLogger(const string& filePath);
    ~GameLogger();

    // showMessage() receives a GameWindow* temporarily for one call... GameLogger does NOT store this pointer ...it has no lasting relationship.
    // This is ASSOCIATION...uses the object for a brief interaction.
    void showMessage(GameWindow* gw,const Font& font,const string& message,float seconds);

    // Also writes to log file...demonstrates FILE HANDLING.
    void logToFile(const string& message);
};