#include "Ball.h"
#include <cmath>

Ball::Ball(float r) : radius(r), active(false)
{
    shape.setRadius(radius);
    shape.setOrigin({ radius, radius });
    shape.setFillColor(sf::Color(240, 230, 210));
    shape.setOutlineColor(sf::Color(255, 255, 255, 120));
    shape.setOutlineThickness(1.f);
    velocity = { 200.f, -300.f };
}

void Ball::setPosition(float x, float y)
{
    shape.setPosition({ x, y });
}

sf::Vector2f Ball::getPosition() const
{
    return shape.getPosition();
}

sf::FloatRect Ball::getBounds() const
{
    return shape.getGlobalBounds();
}

float Ball::getRadius() const
{
    return radius;
}

//Checks if ball is active
bool Ball::isActive() const
{
    return active;
}

void Ball::launch()
{
    active = true;
}

void Ball::reset(float x, float y)
{
    shape.setPosition({ x, y });
    velocity = { 200.f, -300.f };
    active = false;
}

// Multiply current velocity by factor (e.g., 1.2 = +20% speed)
void Ball::multiplyVelocity(float factor)
{
    velocity.x *= factor;
    velocity.y *= factor;
}

void Ball::update(float dt, float winW, float winH)
{
    if (!active)
        return;
    shape.move(velocity * dt);
    sf::Vector2f pos = shape.getPosition();

    //left wall
    if (pos.x - radius <= 0.f)
    {
        pos.x = radius;
        velocity.x = std::abs(velocity.x);
        shape.setPosition(pos);
    }

    //right wall
    if (pos.x + radius >= winW)
    {
        pos.x = winW - radius;
        velocity.x = -std::abs(velocity.x);
        shape.setPosition(pos);
    }

    //top wall 
    if (pos.y - radius <= 0.f)
    {
        pos.y = radius;
        velocity.y = std::abs(velocity.y);
        shape.setPosition(pos);
    }
}

//Checks if ball has fallen below screen
bool Ball::isOutOfBounds(float winH) const
{
    return shape.getPosition().y - radius > winH;
}


void Ball::bounceY()
{
    velocity.y = -velocity.y;
}

void Ball::bounceX()
{
    velocity.x = -velocity.x;
}

void Ball::bounceOffPaddle(float paddleCenterX)
{
    float offset = (shape.getPosition().x - paddleCenterX) / 50.f;
    velocity.x = offset * 350.f;
    velocity.y = -std::abs(velocity.y);

    float spd = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    float want = 320.f;
    velocity *= (want / spd);
}

void Ball::draw(sf::RenderWindow& window)
{
    window.draw(shape);
}