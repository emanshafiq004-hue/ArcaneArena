#pragma once
#include "GameWindow.h"
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include <vector>
class ScoresStatus 
{
private:
GameWindow& gw;
Font font;
sf::Texture bgTex;
std::optional<sf::Sprite> bgSprite;
vector<Text*> textObjects;
string scoresData;
Text* createText(const string& str,unsigned int size,const Color& color,float x,float y);
void buildUI();
public:
ScoresStatus(GameWindow& window);
~ScoresStatus();
void run();
};