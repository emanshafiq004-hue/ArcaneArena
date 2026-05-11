#pragma once
//  Contains:
//    • Particle struct
//    • ParticleSystem  (Function Overloading, Composition)
//    • Drone           (Composition)
//    • Barrier
//    • Collectible hierarchy  (multi-level inheritance)
//        └── Crystal, ShieldPowerUp, MultiplierPowerUp

#include "Globals.h"


//_____________Forward declarations____________
class ScoreManager;

//____________Particle — plain data struct used by ObjectPool<Particle>_________
struct Particle {
    Vec2      pos, vel;
    sf::Color col;
    float     life = 0.f;
    float     maxLife = 1.f;
    float     size = 1.f;
    bool      active = false;   // required by ObjectPool<T>
};


//___________CLASS: ParticleSystem________________
//___________COMPOSITION: owns ObjectPool<Particle>_______________
//__________FUNCTION OVERLOADING: three emit() signatures_________

class ParticleSystem : public IDrawable, public IUpdatable {
    ObjectPool<Particle> pool_;   //_________Composition

public:
    explicit ParticleSystem(int n = 400);   //Parameterized constructor

    //_______Overload 1: full control_________
    void emit(Vec2 pos, sf::Color col, int n, float speed,
        float lifeMin, float lifeMax,
        float sizeMin, float sizeMax,
        float dirX = 0.f, float dirY = 0.f, float spread = 360.f);

    //________Overload 2: simplified (life + size only)_______
    void emit(Vec2 pos, sf::Color col, int n, float speed,
        float life, float size);

    //__________Overload 3: named explosion preset_____________
    void emitExplosion(Vec2 pos);

    void update(float dt) override;
    void draw(sf::RenderTarget& rt) override;
};

//________ABSTRACT CLASS: Collectible________
//______INHERITANCE (multi-level): GameObject -> Collectible
//
//  GameObject
//      Collectible (abstract)
//              Crystal
//              ShieldPowerUp
//              MultiplierPowerUp

class Collectible : public GameObject {
protected:
    float animT_ = 0.f;
    float scrollSpd_ = 180.f;

public:
    Collectible(Vec2 pos, float spd);   
    virtual ~Collectible() = default;   

    //___________Pure virtual methods____________
    virtual void      onCollect(ScoreManager& sm, class Drone& d) = 0;
    virtual sf::Color getColor() const = 0;

    //________Shared update, unique draw via drawShape()________
    void update(float dt) override;
    void draw(sf::RenderTarget& rt) override;   // calls drawShape()
    sf::FloatRect getBounds() const override;

protected:
    virtual void drawShape(sf::RenderTarget& rt, float pulse) = 0;
};

//____________Crystal____________________
class Crystal : public Collectible {
public:
    Crystal(Vec2 pos, float spd);
    sf::Color getColor() const override;
    void onCollect(ScoreManager& sm, class Drone& d) override;

protected:
    void drawShape(sf::RenderTarget& rt, float pulse) override;
};

//__________________Shield Power-Up_____________________
class ShieldPowerUp : public Collectible {
public:
    ShieldPowerUp(Vec2 pos, float spd);
    sf::Color getColor() const override;
    void onCollect(ScoreManager& sm, class Drone& d) override;

protected:
    void drawShape(sf::RenderTarget& rt, float pulse) override;
};

//_______________Multiplier Power-Up_______________________
class MultiplierPowerUp : public Collectible {
public:
    MultiplierPowerUp(Vec2 pos, float spd);
    sf::Color getColor() const override;
    void onCollect(ScoreManager& sm, class Drone& d) override;

protected:
    void drawShape(sf::RenderTarget& rt, float pulse) override;
};

//__________CLASS: Drone
//__________COMPOSITION: holds ParticleSystem by reference
class Drone : public GameObject {
    sf::Sprite      sprite_;
    Vec2            vel_;
    bool            shield_ = false;
    float           shieldTimer_ = 0.f;
    float           invincTimer_ = 0.f;   // NEW — invincibility frames
    bool            dead_ = false;
    float           animTimer_ = 0.f;
    float           tiltAngle_ = 0.f;
    ParticleSystem& ps_;

public:
    Drone(sf::Texture& tex, ParticleSystem& ps);
    ~Drone() override = default;

    void reset();
    void thrust();
    void update(float dt) override;
    void draw(sf::RenderTarget& rt) override;
    sf::FloatRect getBounds() const override;

    bool  isDead()       const { return dead_; }
    bool  hasShield()    const { return shield_; }
    bool  isInvincible() const { return invincTimer_ > 0.f; }  // NEW
    void  grantShield(float dur) { shield_ = true; shieldTimer_ = dur; }
    void  breakShield() {
        shield_ = false;
        shieldTimer_ = 0.f;
        invincTimer_ = 1.2f;   // 1.2 seconds of invincibility after shield breaks
    }
    void  kill();
};

//__________CLASS: Barrier_______________

class Barrier : public GameObject {
    float gapY_, gapSize_;
    bool  moving_ = false;
    float moveDir_ = 1.f;
    float moveSpd_ = 60.f;
    float glowTimer_ = 0.f;

public:
    bool scored = false;

    Barrier(float x, float gapY, float gapSz, bool mv = false);

    void update(float dt) override;
    void scroll(float spd, float dt);
    void draw(sf::RenderTarget& rt) override;

    bool          offScreen() const;
    float         getX()      const { return pos_.x; }
    sf::FloatRect topBounds() const;
    sf::FloatRect botBounds() const;
};
