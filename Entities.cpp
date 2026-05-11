#include "Entities.h"
#include "Managers.h"   // ScoreManager definition needed for onCollect


//  ParticleSystem

ParticleSystem::ParticleSystem(int n) : pool_(n) {}

//Overload 1: full control
void ParticleSystem::emit(Vec2 pos, sf::Color col, int n, float speed,
    float lifeMin, float lifeMax,
    float sizeMin, float sizeMax,
    float dirX, float dirY, float spread)
{
    int spawned = 0;
    for (auto& p : pool_.all()) {
        if (!p.active) {
            float angle = randRange(0.f, spread) * 3.14159f / 180.f;
            float spd = randRange(speed * 0.5f, speed);
            p.vel = Vec2(dirX + std::cos(angle) * spd, dirY + std::sin(angle) * spd);
            p.pos = pos;
            p.col = col;
            p.life = p.maxLife = randRange(lifeMin, lifeMax);
            p.size = randRange(sizeMin, sizeMax);
            p.active = true;
            if (++spawned >= n) break;
        }
    }
}

//Overload 2: simplified
void ParticleSystem::emit(Vec2 pos, sf::Color col, int n,
    float speed, float life, float size)
{
    emit(pos, col, n, speed, life * 0.7f, life, size * 0.6f, size);
}

//Overload 3: explosion preset
void ParticleSystem::emitExplosion(Vec2 pos) {
    emit(pos, sf::Color(255, 120, 0, 255), 60, 220.f, 0.4f, 1.2f, 3.f, 7.f);
    emit(pos, sf::Color(255, 255, 80, 200), 30, 160.f, 0.3f, 0.8f, 2.f, 4.f);
}

void ParticleSystem::update(float dt) {
    for (auto& p : pool_.all()) {
        if (!p.active) continue;
        p.pos += p.vel * dt;
        p.vel.y += 60.f * dt;
        p.life -= dt;
        if (p.life <= 0.f) p.active = false;
    }
}

void ParticleSystem::draw(sf::RenderTarget& rt) {
    sf::CircleShape cs;
    cs.setPointCount(6);
    for (auto& p : pool_.all()) {
        if (!p.active) continue;
        float alpha = p.life / p.maxLife;
        sf::Color c = p.col;
        c.a = static_cast<uint8_t>(alpha * 255.f);
        float r = p.size * alpha;
        cs.setRadius(r);
        cs.setOrigin(sf::Vector2f(r, r));
        cs.setFillColor(c);
        cs.setPosition(sf::Vector2f(p.pos));
        rt.draw(cs);
    }
}

//  Collectible (abstract base)
Collectible::Collectible(Vec2 pos, float spd)
    : GameObject(pos, true), scrollSpd_(spd) {
}

void Collectible::update(float dt) {
    pos_.x -= scrollSpd_ * dt;
    animT_ += dt;
    pos_.y += std::sin(animT_ * 3.f) * 0.5f;
    if (pos_.x < -20.f) active_ = false;
}

void Collectible::draw(sf::RenderTarget& rt) {
    float pulse = 1.f + 0.12f * std::sin(animT_ * 4.f);
    drawShape(rt, pulse);   // template method — polymorphic
}

sf::FloatRect Collectible::getBounds() const {
    return makeRect(pos_.x - 12.f, pos_.y - 12.f, 24.f, 24.f);
}

//________________Crystal________________________
Crystal::Crystal(Vec2 pos, float spd) : Collectible(pos, spd) {}
sf::Color Crystal::getColor() const { return COL_CRYSTAL; }

void Crystal::onCollect(ScoreManager& sm, Drone& d) {
    sm.addCrystal();
}

void Crystal::drawShape(sf::RenderTarget& rt, float pulse) {
    float r = 10.f * pulse;
    sf::ConvexShape gem;
    gem.setPointCount(6);
    for (int i = 0; i < 6; i++) {
        float a = i * 60.f * 3.14159f / 180.f;
        gem.setPoint(i, sf::Vector2f(std::cos(a) * r, std::sin(a) * r));
    }
    gem.setPosition(sf::Vector2f(pos_));
    gem.setFillColor(sf::Color(COL_CRYSTAL.r, COL_CRYSTAL.g, COL_CRYSTAL.b, 220));
    gem.setOutlineColor(COL_CRYSTAL);
    gem.setOutlineThickness(2.f);
    rt.draw(gem);
}

//____________________ShieldPowerUp_____________________
ShieldPowerUp::ShieldPowerUp(Vec2 pos, float spd) : Collectible(pos, spd) {}
sf::Color ShieldPowerUp::getColor() const { return COL_SHIELD; }

void ShieldPowerUp::onCollect(ScoreManager& sm, Drone& d) {
    d.grantShield(8.f);
    sm.setShield(true);
}

void ShieldPowerUp::drawShape(sf::RenderTarget& rt, float pulse) {
    float r = 13.f * pulse;
    sf::CircleShape cs(r);
    cs.setOrigin(sf::Vector2f(r, r));
    cs.setPosition(sf::Vector2f(pos_));
    cs.setFillColor(sf::Color(COL_SHIELD.r, COL_SHIELD.g, COL_SHIELD.b, 60));
    cs.setOutlineColor(COL_SHIELD);
    cs.setOutlineThickness(2.5f);
    rt.draw(cs);
    sf::RectangleShape dot(sf::Vector2f(5.f, 5.f));
    dot.setFillColor(COL_SHIELD);
    dot.setOrigin(sf::Vector2f(2.5f, 2.5f));
    dot.setPosition(sf::Vector2f(pos_));
    rt.draw(dot);
}

//_______________MultiplierPowerUp___________________
MultiplierPowerUp::MultiplierPowerUp(Vec2 pos, float spd) : Collectible(pos, spd) {}
sf::Color MultiplierPowerUp::getColor() const { return COL_MULTI; }

void MultiplierPowerUp::onCollect(ScoreManager& sm, Drone& d) {
    sm.activateMulti();
}

void MultiplierPowerUp::drawShape(sf::RenderTarget& rt, float pulse) {
    float r = 13.f * pulse;
    sf::CircleShape cs(r);
    cs.setOrigin(sf::Vector2f(r, r));
    cs.setPosition(sf::Vector2f(pos_));
    cs.setFillColor(sf::Color(COL_MULTI.r, COL_MULTI.g, COL_MULTI.b, 60));
    cs.setOutlineColor(COL_MULTI);
    cs.setOutlineThickness(2.5f);
    rt.draw(cs);
    sf::RectangleShape dot(sf::Vector2f(5.f, 5.f));
    dot.setFillColor(COL_MULTI);
    dot.setOrigin(sf::Vector2f(2.5f, 2.5f));
    dot.setPosition(sf::Vector2f(pos_));
    rt.draw(dot);
}
//  Drone

Drone::Drone(sf::Texture& tex, ParticleSystem& ps)
    : GameObject(Vec2(DRONE_X, BASE_H / 2.f), true)
    , sprite_(tex)
    , ps_(ps)
{
    auto lb = sprite_.getLocalBounds();
    sprite_.setOrigin(sf::Vector2f(lb.size.x / 2.f, lb.size.y / 2.f));
}

void Drone::reset() {
    pos_ = Vec2(DRONE_X, BASE_H / 2.f);
    vel_ = Vec2();
    shield_ = false;
    shieldTimer_ = 0.f;
    invincTimer_ = 0.f;
    dead_ = false;
    tiltAngle_ = 0.f;
}

void Drone::thrust() { vel_.y = THRUST_FORCE; }

void Drone::kill() { dead_ = true; GameStats::totalDeaths++; }

void Drone::update(float dt) {
    vel_.y += GRAVITY * dt;
    pos_ += vel_ * dt;

    if (pos_.y < 20.f) { pos_.y = 20.f; vel_.y = 0.f; }
    if (pos_.y > BASE_H - 20.f) dead_ = true;

    float targetTilt = std::clamp(vel_.y * 0.04f, -30.f, 30.f);
    tiltAngle_ += (targetTilt - tiltAngle_) * dt * 8.f;
    sprite_.setRotation(sf::degrees(tiltAngle_));
    sprite_.setPosition(sf::Vector2f(pos_));

    if (shield_) {
        shieldTimer_ -= dt;
        if (shieldTimer_ <= 0.f) {
            shield_ = false;
        }
    }

    // Invincibility frames after shield absorbs a hit
    if (invincTimer_ > 0.f) invincTimer_ -= dt;

    animTimer_ += dt;
    if (animTimer_ > 0.04f) {
        animTimer_ = 0.f;
        ps_.emit(pos_ + Vec2(-12.f, 4.f),
            sf::Color(0, 200, 255, 200), 3, 80.f, 0.3f, 2.f);
        if (vel_.y < -100.f)
            ps_.emit(pos_ + Vec2(0.f, 8.f),
                sf::Color(255, 160, 30, 200), 4, 120.f, 0.25f, 3.f);
    }
}
void Drone::draw(sf::RenderTarget& rt) {
    auto lb = sprite_.getLocalBounds();
    float sc = 40.f / std::max(lb.size.x, lb.size.y);
    sprite_.setScale(sf::Vector2f(sc, sc));
    rt.draw(sprite_);

    if (shield_) {
        float pulse = 1.f + 0.08f * std::sin(shieldTimer_ * 8.f);
        float r = 22.f * pulse;
        sf::CircleShape sh(r);
        sh.setOrigin(sf::Vector2f(r, r));
        sh.setPosition(sf::Vector2f(pos_));
        sh.setFillColor(sf::Color(80, 255, 180, 40));
        sh.setOutlineColor(sf::Color(80, 255, 180, 200));
        sh.setOutlineThickness(2.f);
        rt.draw(sh);
    }
}

sf::FloatRect Drone::getBounds() const {
    return makeRect(pos_.x - 16.f, pos_.y - 12.f, 32.f, 24.f);
}

//  Barrier
Barrier::Barrier(float x, float gapY, float gapSz, bool mv)
    : GameObject(Vec2(x, 0.f), true)
    , gapY_(gapY), gapSize_(gapSz), moving_(mv) {
}

void Barrier::update(float dt) {
    glowTimer_ += dt;
    if (moving_) {
        gapY_ += moveDir_ * moveSpd_ * dt;
        if (gapY_ < 30.f) { gapY_ = 30.f;                  moveDir_ = 1.f; }
        if (gapY_ + gapSize_ > BASE_H - 30.f) { gapY_ = BASE_H - gapSize_ - 30.f; moveDir_ = -1.f; }
    }
}

void Barrier::scroll(float spd, float dt) { pos_.x -= spd * dt; }

void Barrier::draw(sf::RenderTarget& rt) {
    float pulse = 4.f + 2.f * std::sin(glowTimer_ * 3.f);
    float botY = gapY_ + gapSize_;

    auto drawSeg = [&](float bx, float by, float bw, float bh) {
        if (bh <= 0.f) return;
        sf::RectangleShape g(sf::Vector2f(bw + pulse * 2.f, bh + pulse * 2.f));
        g.setPosition(sf::Vector2f(bx - pulse, by - pulse));
        g.setFillColor(COL_BGLOW); rt.draw(g);
        sf::RectangleShape rs(sf::Vector2f(bw, bh));
        rs.setPosition(sf::Vector2f(bx, by));
        rs.setFillColor(COL_BARRIER); rt.draw(rs);
        };
    drawSeg(pos_.x, 0.f, BARRIER_W, gapY_);
    drawSeg(pos_.x, botY, BARRIER_W, BASE_H - botY);

    sf::RectangleShape cap(sf::Vector2f(BARRIER_W + 8.f, 8.f));
    cap.setFillColor(sf::Color(0, 180, 240, 255));
    cap.setPosition(sf::Vector2f(pos_.x - 4.f, gapY_ - 8.f)); rt.draw(cap);
    cap.setPosition(sf::Vector2f(pos_.x - 4.f, botY));       rt.draw(cap);
}

bool Barrier::offScreen() const { return pos_.x + BARRIER_W < -10.f; }

sf::FloatRect Barrier::topBounds() const {
    return makeRect(pos_.x, 0.f, BARRIER_W, gapY_);
}
sf::FloatRect Barrier::botBounds() const {
    float botY = gapY_ + gapSize_;
    return makeRect(pos_.x, botY, BARRIER_W, BASE_H - botY);
}