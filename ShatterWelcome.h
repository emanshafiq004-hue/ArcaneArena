#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "ScoreStorage.h"
#include "GameWindow.h"
class ShatterWelcomeScreen
{
private:
    GameWindow& gameWindow;
    sf::Font font;

    // Background
    sf::Texture bgTexture;
    sf::Sprite  bgSprite;

    // Main screen elements 
    sf::Text titleText;
    sf::RectangleShape startButton;
    sf::Text startText;
    sf::RectangleShape scoreboardButton;   
    sf::Text scoreboardText;     
    sf::RectangleShape exitButton;
    sf::Text exitText;

    // Name-input dialog elements
    sf::RectangleShape overlayDim;
    sf::RectangleShape overlayBox;
    sf::Text dialogTitle;
    sf::RectangleShape nameBox;
    sf::Text nameDisplay;
    sf::Text hintText;
    sf::Text warningText;

    // Scoreboard overlay elements
    ShatterScoreManager scoreManager;     

    //State
    std::string playerName;
    bool showNameDialog;
    bool showScoreboard;      
    bool showWarning;
    sf::Clock warningClock;

    // Helpers
    void setupMainScreen();
    void setupNameDialog();
    bool isClicked(const sf::RectangleShape& btn, sf::Vector2i mousePos);
    void drawScoreboardOverlay(); 
public:
    ShatterWelcomeScreen(GameWindow& gw);
    bool run();
    std::string getPlayerName() const;
};