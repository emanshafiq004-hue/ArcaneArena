#include "Paddle.h"

Paddle::Paddle(float winWidth, float winHeight)
    : speed(500.f), winW(winWidth)
{
    //OUTER GLOW (largest, faint) 
    outerGlow.setSize({ 122.f, 30.f });
    outerGlow.setOrigin({ 61.f, 15.f });
    outerGlow.setFillColor(sf::Color(0, 200, 255, 35)); // very transparent cyan
    outerGlow.setOutlineThickness(0.f);

    //INNER GLOW (medium, brighter)
    innerGlow.setSize({ 110.f, 22.f });
    innerGlow.setOrigin({ 55.f, 11.f });
    innerGlow.setFillColor(sf::Color(0, 255, 255, 80)); // semi-transparent neon cyan
    innerGlow.setOutlineThickness(0.f);

    //MAIN PADDLE (core – bright white/cyan)
    shape.setSize({ 100.f, 14.f });
    shape.setOrigin({ 50.f, 7.f });
    shape.setFillColor(sf::Color(220, 250, 255));          // almost white with hint of cyan
    shape.setOutlineColor(sf::Color(0, 255, 255, 200));    // strong cyan outline
    shape.setOutlineThickness(2.5f);

    // Position all at center-bottom
    sf::Vector2f startPos(winWidth / 2.f, winHeight - 40.f);
    shape.setPosition(startPos);
    innerGlow.setPosition(startPos);
    outerGlow.setPosition(startPos);
}

void Paddle::update(float dt)
{
    // Move left/right
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
        shape.move({ -speed * dt, 0.f });
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
        shape.move({ speed * dt, 0.f });

    // Clamp to window edges
    sf::Vector2f pos = shape.getPosition();
    float halfWidth = shape.getSize().x / 2.f;
    if (pos.x - halfWidth < 0.f)    pos.x = halfWidth;
    if (pos.x + halfWidth > winW)   pos.x = winW - halfWidth;

    shape.setPosition(pos);
    innerGlow.setPosition(pos);
    outerGlow.setPosition(pos);
}

sf::FloatRect Paddle::getBounds() const
{
    return shape.getGlobalBounds();
}

sf::Vector2f Paddle::getPosition() const
{
    return shape.getPosition();
}

float Paddle::getCenterX() const
{
    return shape.getPosition().x;
}

void Paddle::draw(sf::RenderWindow& window)
{
    // Draw from back to front
    window.draw(outerGlow);
    window.draw(innerGlow);
    window.draw(shape);
}