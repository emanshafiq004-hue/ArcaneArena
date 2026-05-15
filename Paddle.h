#pragma once
#include <SFML/Graphics.hpp>

class Paddle
{
private:
    sf::RectangleShape shape;
    sf::RectangleShape innerGlow;
    sf::RectangleShape outerGlow;
    float speed;
    float winW;
public:
    Paddle(float winWidth, float winHeight);
    void update(float dt);
    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
    float getCenterX() const;
    void draw(sf::RenderWindow& window);
};