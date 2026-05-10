#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "scoreboard.h"

sf::Text makeCenteredText(const sf::Font& font, const std::string& str,
    unsigned int size, sf::Color color, float windowW, float y);

void drawHealthBar(sf::RenderWindow& window, float x, float y, float width,
    int current, int max, sf::Color color);

void drawPressStartScreen(sf::RenderWindow& window, const sf::Font& font, sf::Clock& blinkClock);
void drawMenuScreen(sf::RenderWindow& window, const sf::Font& font, int menuIndex);

// Scoreboard now reads from the ScoreBoard object instead of the old text file.
void drawScoreboardScreen(sf::RenderWindow& window, const sf::Font& font,
    const ScoreBoard& sb, int optionIndex);

// New screens for the name-input and continue-game flow.
void drawNameInputScreen(sf::RenderWindow& window, const sf::Font& font,
    const std::string& currentName, sf::Clock& blinkClock);
void drawContinueListScreen(sf::RenderWindow& window, const sf::Font& font,
    const ScoreBoard& sb, int selectedIndex);
void drawContinuePlayerScreen(sf::RenderWindow& window, const sf::Font& font,
    const PlayerRecord& player, int optionIndex);

void drawIntroStage1Screen(sf::RenderWindow& window, const sf::Font& font);
void drawIntroStage2Screen(sf::RenderWindow& window, const sf::Font& font);
void drawIntroStage3Screen(sf::RenderWindow& window, const sf::Font& font);
void drawEndMenuScreen(sf::RenderWindow& window, const sf::Font& font, int menuIndex);

void drawHud(sf::RenderWindow& window, const sf::Font& font,
    int playerHp, int playerMaxHp,
    int enemyHp, int enemyMaxHp,
    int stageNum);

void drawEscHint(sf::RenderWindow& window, const sf::Font& font);

void drawYouWinOverlay(sf::RenderWindow& window, const sf::Font& font);
void drawYouWinStage2Overlay(sf::RenderWindow& window, const sf::Font& font);
void drawYouWinStage3Overlay(sf::RenderWindow& window, const sf::Font& font);
void drawYouLoseOverlay(sf::RenderWindow& window, const sf::Font& font);
