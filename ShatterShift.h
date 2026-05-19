// ShatterShift.h

#pragma once
#include "GameWindow.h"
#include "AudioManager.h"
#include "ShatterWelcome.h"
#include "GameScreen.h"
#include <string>

class ShatterShift {
private:
    GameWindow& gw;
    AudioManager& audio;
public:
    ShatterShift(GameWindow& window, AudioManager& audioManager) : gw(window), audio(audioManager) {}
    void run()
    {
        gw.setInterface(Color(10, 10, 40), "Shatter Shift");
        gw.setIconFromFile("assets/shatterassets/icon.jpg");

        while (gw.isOpen()) {
            ShatterWelcomeScreen welcome(gw);
            bool startGame = welcome.run();
            if (!startGame || !gw.isOpen())
                return;

            std::string playerName = welcome.getPlayerName();
            int currentLevel = 1;
            int score = 0;
            int lives = 3;
            while (currentLevel <= 5 && gw.isOpen())
            {
                GameScreen game(gw, playerName, currentLevel, score, lives);
                bool levelComplete = game.run();
                if (levelComplete)
                {
                    score = game.getScore();
                    lives = game.getLives();
                    currentLevel++;
                }
                else
                {
                    break;
                }
            }
        }
    }
};