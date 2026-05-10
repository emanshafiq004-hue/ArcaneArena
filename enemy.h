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
//
//  JUMP DODGE
//    35% chance on rising edge of player attack.
//    Jumps with the same arc as the adventurer (gravity 0.004,
//    velocityY -1.5) and dashes horizontally AWAY from the
//    player so the dodge covers real screen distance.
//
//  ATTACK VARIANT
//    On each attack trigger the Samurai randomly (50/50) picks
//    either ATTACK 1 (normal sword swing) or attack_var (a
//    powerful spinning slash arc with higher damage).
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
    float     jumpVelocityX;        // horizontal speed set at jump-trigger time
    bool      playerWasAttacking;
    sf::Clock jumpCooldownClock;
    float     jumpCooldownDuration;

    // ----------------------------------------------------------------
    //  attack_var support
    //
    //  attack_var_2.png is 1344x1024 (7 frames on a 192 px grid).
    //  The sprite content sits in rows 378-578 (201 px tall)
    //  in the centre of the canvas.  7 frames total at 192 px each.
    //  Frames 0-2 are idle walk-in poses; the actual attack plays
    //  frames 3-6 (AVAR_FIRST_ATTACK_FRAME = 3, AVAR_FRAMES = 4).
    //
    //  A dedicated sprite is used so the main sprite's physics/bounds
    //  stay untouched; we just render a different visual on top.
    //
    //  Scale 0.8 renders the body at ~62 x 99 px, matching the normal
    //  samurai body size.
    //
    //  AVAR_Y positions the crop so the character's feet (texture row
    //  575, local y 197 within the crop) land at the same screen y as
    //  the IDLE sprite's feet:
    //      idle feet screen y  = SAMURAI_FLOOR_Y + 80 * SAMURAI_SCALE
    //                          = 291 + 80 * 3 = 531
    //      AVAR_Y + 197 * 0.8  = 531
    //      AVAR_Y              = 373
    //
    //  AVAR_X_OFFSET centres the (narrower) attack_var frame inside
    //  the same on-screen column the normal sprite occupies:
    //      normal rendered width = 96  * 3.0 = 288 px
    //      avar   rendered width = 192 * 0.8 = 153.6 px
    //      offset = (288 - 153.6) / 2 = 67.2
    // ----------------------------------------------------------------
    sf::Texture attackVarTexture;
    sf::Sprite  attackVarSprite{ attackVarTexture };

    bool usingVariant;    // true when this attack uses attack_var
    bool drawingVariant;  // true whenever attackVarSprite should be drawn this tick

    static const int AVAR_FRAME_W_TEX = 192;   // true frame width in texture
    static const int AVAR_FIRST_ATTACK_FRAME = 3;   // skip idle walk-in frames 0-2
    static const int AVAR_FRAMES = 4;     // frames 3-6 (attack sequence, 2 glitchy frames removed)
    static const int AVAR_CONTENT_Y = 378;   // first content row in the PNG
    static const int AVAR_CONTENT_H = 201;   // 578 - 378 + 1

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