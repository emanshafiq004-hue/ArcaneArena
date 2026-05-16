#pragma once
#include <SFML/Graphics.hpp>
class Ball
{
private:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float radius;
    bool active;
public:
    Ball(float r = 8.f);
    void setPosition(float x, float y);
    sf::Vector2f  getPosition() const;
    sf::FloatRect getBounds() const;
    float getRadius() const;
    bool isActive() const;
    void launch();
    void reset(float x, float y);
    void multiplyVelocity(float factor);
    void update(float dt, float winW, float winH);
    bool isOutOfBounds(float winH) const;
    void bounceY();
    void bounceX();
    void bounceOffPaddle(float paddleCenterX);
    void draw(sf::RenderWindow& window);
};