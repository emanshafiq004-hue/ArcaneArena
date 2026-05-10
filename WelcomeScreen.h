// WelcomeScreen.h
// AGGREGATION....holds GameWindow& (reference, doesn't own it)

#pragma once
#include "GameWindow.h"
#include <SFML/Graphics.hpp>
class WelcomeScreen 
{
private:
GameWindow& gw;
Font font;
Texture bgTex;
optional<Sprite> bgSprite;
Text titleText;
Text subtitleText;
Text promptText;
Clock animClock;
AudioManager& Audio;
public:
WelcomeScreen(GameWindow& window,AudioManager& audio);
void run();
};