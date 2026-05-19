//// GameLogger.h
//// ASSOCIATION: GameLogger USES GameWindow temporarily to display messages.
//// It does NOT own the window, does NOT store it as a member.
//// This is pure association... a "uses-a" relationship, the weakest OOP link.
//// Unlike aggregation (stores a reference) or composition (owns the object).
//
//#pragma once
//#include "GameWindow.h"
//#include <SFML/Graphics.hpp>
//#include <string>
//#include <fstream>
//#include <optional>
//
//class GameLogger 
//{
//private:
//string logFilePath;
//
//public:
//// Constructor only needs the log file path, NOT the window
//GameLogger(const string& filePath);
//~GameLogger();
//void showMessage(GameWindow* gw,Font& font,const string& message,float seconds,Texture& texture,vector<string> path,optional<Sprite> sprite,const int width ,const int height);// showMessage() receives a GameWindow* temporarily for one call... GameLogger does NOT store this pointer ...it has no lasting relationship.This is ASSOCIATION...uses the object for a brief interaction.
//// Also writes to log file...demonstrates FILE HANDLING.
//void logToFile(const string& message);
//};


//--------------------------------------------------------------
// GameLogger.h
// ASSOCIATION: GameLogger USES GameWindow temporarily to display messages.
// It does NOT own or store the window (pure association — "uses-a" relationship).
// FIX: sprite parameter changed from optional<Sprite> (by value) to
//      optional<Sprite>& (by reference) so the caller's sprite is updated.
// FIX: namespace prefixes added to all types so the header compiles standalone.

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