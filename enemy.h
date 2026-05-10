#pragma once
#include <SFML/Graphics.hpp>

enum AiState
{
    AI_IDLE,
    AI_CHASE,
    AI_ATTACK
};

class Enemy
{
protected:
    sf::Texture idleTexture;
    sf::Texture runTexture;
    sf::Texture attackTexture;
    sf::Texture blockTexture;
    sf::Texture hurtTexture;
    sf::Sprite  sprite{ idleTexture };

    float worldX;

    sf::Clock animClock;
    float     animSpeed;
    float     blockAnimSpeed;
    int       currentFrame;
    int       idleFrames;
    int       runFrames;
    int       attackFrames;
    int       blockFrames;
    int       hurtFrames;
    int       frameW;
    int       frameH;

    int   health;
    int   maxHealth;
    float speed;

    bool attacking;
    bool blocking;
    bool facingLeft;

    bool dyingAnimPlaying;
    bool dyingAnimDone;

    sf::Clock attackClock;
    float     attackCooldown;
    float     attackDuration;

    float velocityY;
    float gravity;
    bool  onGround;

public:
    Enemy(int hp, float spd, float atkCooldown, float atkDuration,
        int fw, int fh, int idleFr, int runFr, int atkFr);

    virtual ~Enemy() {}

    virtual void reset();
    virtual void update(sf::Vector2f playerPos) = 0;
    virtual void block(bool shouldBlock);

    void takeDamage(int dmg);

    virtual sf::FloatRect getBounds()    const;
    virtual sf::FloatRect getAttackBox() const;
    virtual void draw(sf::RenderWindow& window);

    bool  isBlocking()      const { return blocking; }
    int   getHealth()       const { return health; }
    int   getMaxHealth()    const { return maxHealth; }
    bool  isDead()          const { return health <= 0; }
    bool  isDyingAnimDone() const { return dyingAnimDone; }
    bool  isAttacking()     const { return attacking; }
    float getWorldX()       const { return worldX; }

    void setWorldX(float x);

protected:
    void stepAnimation(int frames);
    void updateFacingScale();
    void applyGravityAndFloor(float floorY);
    bool stepHurtAnimation();
};


class Creature : public Enemy
{
private:
    float ATTACK_RANGE;
    float DETECT_RANGE;
    int   aiState;

public:
    Creature();
    void          update(sf::Vector2f playerPos) override;
    sf::FloatRect getBounds()    const override;
    sf::FloatRect getAttackBox() const override;
};


class Knight : public Enemy
{
private:
    float ATTACK_RANGE;
    float DETECT_RANGE;
    int   aiState;

    static const int FRAME_W = 96;

    bool      wasPlayerAttacking;
    bool      blockDecision;
    sf::Clock blockHoldClock;
    float     blockHoldDuration;

public:
    Knight();
    void          update(sf::Vector2f playerPos) override;
    sf::FloatRect getBounds()    const override;
    sf::FloatRect getAttackBox() const override;
    void          reset()              override;
    void tryBlock(bool playerIsAttacking);
};


// ============================================================
//  Samurai  (Stage 3)
// ============================================================

class Samurai : public Enemy
{
private:
    float ATTACK_RANGE;
    float DETECT_RANGE;
    int   aiState;

    static const int FRAME_W = 96;

    // Jump dodge
    bool      jumping;
    float     jumpVelocityX;      
    bool      playerWasAttacking;
    sf::Clock jumpCooldownClock;
    float     jumpCooldownDuration;

    sf::Texture attackVarTexture;
    sf::Sprite  attackVarSprite{ attackVarTexture };

    bool usingVariant;    
    bool drawingVariant;  

    static const int AVAR_FRAME_W_TEX = 192;  
    static const int AVAR_FIRST_ATTACK_FRAME = 3; 
    static const int AVAR_FRAMES = 4;     
    static const int AVAR_CONTENT_Y = 378;
    static const int AVAR_CONTENT_H = 201;

public:
    Samurai();
    void          update(sf::Vector2f playerPos) override;
    void          draw(sf::RenderWindow& window)   override;
    sf::FloatRect getBounds()    const override;
    sf::FloatRect getAttackBox() const override;
    void          reset()              override;

    void notifyPlayerAttacking(bool isAttacking);

    bool isUsingVariant() const { return usingVariant; }
};