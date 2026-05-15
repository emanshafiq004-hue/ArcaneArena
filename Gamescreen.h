#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include "Brick.h"
#include "Ball.h"
#include "Paddle.h"
#include "ScoreStorage.h"
#include "GameWindow.h"
using namespace std;
class GameScreen
{
public:
    // Constructor — now also takes player name
    GameScreen(GameWindow& gw,
        const string& playerName,int level,int startScore,int startLives);
    ~GameScreen();  // deletes all Brick* pointers
    // Runs the game loop for ONE level.
    // Returns true  = level complete
    // Returns false = game over or quit
    bool run();
    // Getters — main.cpp reads these after run()
    int getScore() const { return score; }
    int getLives() const { return lives; }
private:
    // Core References 
    GameWindow& gameWindow;
    sf::Font font;
    // Background
    sf::Texture bgTexture;
    sf::Sprite  bgSprite;
    bool bgLoaded;
    // Game Objects (Composition)
    Ball ball;
    Paddle paddle;
    vector<Brick*> bricks;   // Polymorphism via Brick*
    // Player & Progress Data
    string playerName;      
    int score;
    int lives;
    int currentLevel;
    //Score Management (Composition)
    // ShatterScoreManager handles ALL Shatter Shift file I/O
    ShatterScoreManager scoreManager;  
    // Sound Effects
    sf::SoundBuffer hitBuffer;
    sf::SoundBuffer gameOverBuffer;
    sf::SoundBuffer levelDoneBuffer;
    sf::Sound hitSound;
    sf::Sound gameOverSound;
    sf::Sound levelDoneSound;
    // Background Music
    sf::Music bgMusic;
    // Private Helper Functions
    void loadSounds();
    void loadLevel();
    void generateFallbackLevel();
    void loadBackground();
    void applyBallSpeedMultiplier();
    void resetBall();
    void handleCollisions();
    void drawHUD();                 // shows name, score, level, lives, highscore
    bool allBricksDestroyed();
    // Screen Functions
    bool showNextLevelPrompt();     // Y / N after each level
    void showFinalScreen();         // gold screen after level 5
    void showGameOverScreen();      // game over with leaderboard
};