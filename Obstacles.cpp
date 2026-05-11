#include "Obstacles.h"
#include "Managers.h"   // ScoreManager
#include "Entities.h"   // Drone

//  Obstacle (base)

Obstacle::Obstacle(Vec2 pos, float spd)
    : GameObject(pos, true), speed_(spd) {
}

void Obstacle::update(float dt) {
    pos_.x -= speed_ * dt;
    animT_ += dt;
    if (pos_.x < -120.f) active_ = false;
}

//  StormCloud

StormCloud::StormCloud(Vec2 pos, float spd) : Obstacle(pos, spd) {}

void StormCloud::applyEffect(ScoreManager& sm, Drone& d) {
    // Shield already checked in GameWorld::checkCollisions
    // If we reach here, drone has NO shield — kill it
    d.kill();
    active_ = false;
}



void StormCloud::draw(sf::RenderTarget& rt) {
    auto drawCirc = [&](float ox, float oy, float r, sf::Color c) {
        sf::CircleShape cs(r);
        cs.setOrigin(sf::Vector2f(r, r));
        cs.setPosition(sf::Vector2f(pos_.x + ox, pos_.y + oy));
        cs.setFillColor(c); rt.draw(cs);
        };
    drawCirc(0.f, 0.f, 28.f, sf::Color(100, 60, 200, 180));
    drawCirc(22.f, -8.f, 22.f, sf::Color(90, 50, 190, 160));
    drawCirc(-22.f, -5.f, 20.f, sf::Color(90, 50, 190, 160));
    drawCirc(0.f, -18.f, 18.f, sf::Color(140, 90, 220, 180));

    // Lightning flash effect
    if ((int)(animT_ * 6.f) % 7 < 2) {
        sf::Vertex bolt[4] = {
            sf::Vertex(sf::Vector2f(pos_.x,      pos_.y + 10.f), sf::Color(255,255, 0,200)),
            sf::Vertex(sf::Vector2f(pos_.x + 8.f, pos_.y + 26.f), sf::Color(255,220, 0,200)),
            sf::Vertex(sf::Vector2f(pos_.x + 4.f, pos_.y + 26.f), sf::Color(255,220, 0,200)),
            sf::Vertex(sf::Vector2f(pos_.x + 14.f, pos_.y + 44.f), sf::Color(255,200, 0,160))
        };
        rt.draw(bolt, 4, sf::PrimitiveType::LineStrip);
    }
}

sf::FloatRect StormCloud::getBounds() const {
    return makeRect(pos_.x - 28.f, pos_.y - 28.f, 68.f, 56.f);
}


//  Missile

Missile::Missile(Vec2 pos, float spd) : Obstacle(pos, spd) {}

void Missile::applyEffect(ScoreManager& sm, Drone& d) {
    // Shield already checked in GameWorld::checkCollisions
    // If we reach here, drone has NO shield — kill it
    d.kill();
    active_ = false;
}

void Missile::draw(sf::RenderTarget& rt) {
    float trail = 30.f + 10.f * std::sin(animT_ * 10.f);

    // Exhaust trail
    sf::VertexArray tr(sf::PrimitiveType::Triangles, 3);
    tr[0] = sf::Vertex(sf::Vector2f(pos_.x + 28.f, pos_.y), sf::Color(255, 120, 0, 0));
    tr[1] = sf::Vertex(sf::Vector2f(pos_.x + 28.f + trail, pos_.y + 4.f), sf::Color(255, 60, 0, 180));
    tr[2] = sf::Vertex(sf::Vector2f(pos_.x + 28.f, pos_.y + 8.f), sf::Color(255, 120, 0, 0));
    rt.draw(tr);

    // Body
    sf::RectangleShape body(sf::Vector2f(36.f, 10.f));
    body.setFillColor(COL_MISSILE);
    body.setPosition(sf::Vector2f(pos_.x, pos_.y));
    rt.draw(body);

    // Nose cone
    sf::ConvexShape nose; nose.setPointCount(3);
    nose.setPoint(0, sf::Vector2f(0.f, 0.f));
    nose.setPoint(1, sf::Vector2f(0.f, 10.f));
    nose.setPoint(2, sf::Vector2f(-14.f, 5.f));
    nose.setFillColor(sf::Color(255, 180, 60, 255));
    nose.setPosition(sf::Vector2f(pos_.x, pos_.y));
    rt.draw(nose);

    // Fin
    sf::ConvexShape fin; fin.setPointCount(3);
    fin.setPoint(0, sf::Vector2f(0.f, 0.f));
    fin.setPoint(1, sf::Vector2f(14.f, 0.f));
    fin.setPoint(2, sf::Vector2f(0.f, -8.f));
    fin.setFillColor(sf::Color(200, 60, 10, 255));
    fin.setPosition(sf::Vector2f(pos_.x + 28.f, pos_.y));
    rt.draw(fin);
}

sf::FloatRect Missile::getBounds() const {
    return makeRect(pos_.x - 14.f, pos_.y, 50.f, 10.f);
}