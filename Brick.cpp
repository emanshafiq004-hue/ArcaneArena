#include "Brick.h"
#include <cmath>

//  BASE BRICK – dark fill + bright thin outline
Brick::Brick(float x, float y, float w, float h, int hp, int pts, sf::Color color)
    : health(hp), points(pts), destroyed(false)
{
    shape.setPosition({ x, y });
    shape.setSize({ w, h });
    shape.setFillColor(color);
    // Shining edge: bright, thin outline (2px)
    shape.setOutlineColor(sf::Color(220, 240, 255, 180)); // ice white
    shape.setOutlineThickness(2.f);
}

bool Brick::hit()
{
    health--;
    if (health <= 0)
    {
        destroyed = true;
        return true;
    }

    // Damage effect: make fill even darker
    sf::Color c = shape.getFillColor();
    shape.setFillColor({
        (uint8_t)(c.r > 40 ? c.r - 40 : 10),
        (uint8_t)(c.g > 40 ? c.g - 40 : 10),
        (uint8_t)(c.b > 40 ? c.b - 40 : 10)
        });
    // Keep the outline bright to maintain shine
    return false;
}

void Brick::update(float dt) {}

sf::FloatRect Brick::getBounds() const
{
    return shape.getGlobalBounds();
}

//  ARMORED BRICK – deep navy blue + cyan shine
ArmoredBrick::ArmoredBrick(float x, float y, float w, float h)
    : Brick(x, y, w, h, 2, 20, sf::Color(25, 60, 140))   // dark blue
{
    shape.setOutlineColor(sf::Color(0, 200, 255, 220));   // bright cyan rim
}

//  EXPLOSIVE BRICK – dark magenta + pink shine
ExplosiveBrick::ExplosiveBrick(float x, float y, float w, float h)
    : Brick(x, y, w, h, 1, 50, sf::Color(120, 25, 100))   // deep purple/magenta
{
    shape.setOutlineColor(sf::Color(255, 80, 180, 220));   // neon pink rim
}

bool ExplosiveBrick::hit()
{
    destroyed = true;
    health = 0;
    return true;
}

//  MOVING BRICK – dark forest green + lime shine
MovingBrick::MovingBrick(float x, float y, float w, float h,float leftBound, float rightBound)
    : Brick(x, y, w, h, 1, 30, sf::Color(30, 90, 45))     // dark green
{
    speed = 100.f;
    minX = leftBound;
    maxX = rightBound - w;
    shape.setOutlineColor(sf::Color(100, 255, 120, 220));  // bright lime rim
}

void MovingBrick::update(float dt)
{
    sf::Vector2f pos = shape.getPosition();
    pos.x += speed * dt;
    if (pos.x <= minX)
    {
        pos.x = minX;
        speed = std::abs(speed);
    }
    if (pos.x >= maxX)
    {
        pos.x = maxX;
        speed = -std::abs(speed);
    }
    shape.setPosition(pos);
}