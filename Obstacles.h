#pragma once
//  Contains:
//    Obstacle  (Abstract class)
//    StormCloud  (Inheritance + Polymorphism)
//    Missile     (Inheritance + Polymorphism)
//
//  Hierarchy:
//    GameObject
//        Obstacle (abstract)
//                StormCloud
//                Missile

#include "Globals.h"

// Forward declarations
class ScoreManager;
class Drone;

//_________ABSTRACT CLASS: Obstacle_______________
//      INHERITANCE: extends GameObject
class Obstacle : public GameObject {
protected:
    float speed_;
    float animT_ = 0.f;

public:
    //Parameterized constructor + initializer list
    Obstacle(Vec2 pos, float spd);
    virtual ~Obstacle() = default;   // OOP #7

    //Pure virtual: each obstacle defines its own effect
    virtual void applyEffect(ScoreManager& sm, Drone& d) = 0;

    //shared scroll update; draw stays pure virtual
    void update(float dt) override;
};

//StormCloud : Obstacle
//  Overrides applyEffect (slows drone) and draw
class StormCloud : public Obstacle {
public:
    StormCloud(Vec2 pos, float spd);

    void applyEffect(ScoreManager& sm, Drone& d) override;
    void draw(sf::RenderTarget& rt) override;
    sf::FloatRect getBounds() const override;
};

//Missile : Obstacle
//  Overrides applyEffect (kills or breaks shield) and draw
class Missile : public Obstacle {
public:
    Missile(Vec2 pos, float spd);

    void applyEffect(ScoreManager& sm, Drone& d) override;
    void draw(sf::RenderTarget& rt) override;
    sf::FloatRect getBounds() const override;
};
