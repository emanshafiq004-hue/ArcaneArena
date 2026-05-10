#pragma once
#include <SFML/Graphics.hpp>

class Player
{
private:
    sf::Texture idleRightTexture;
    sf::Texture idleLeftTexture;
    sf::Texture runRightTexture;
    sf::Texture runLeftTexture;
    sf::Texture attackRightTexture;   // Attack 1 — J key (and combo phase 1)
    sf::Texture attackLeftTexture;
    sf::Texture attackUpTexture;      // Attack 1 up — K combo phase 2
    sf::Texture blockProgressionTexture;  // block_progression.png
    sf::Texture blockSuccessTexture;      // block_success.png

    sf::Sprite  sprite{ idleRightTexture };

    sf::Sprite  blockSprite{ blockProgressionTexture };
    sf::Sprite  blockSuccSprite{ blockSuccessTexture };

    sf::Clock attackClock;
    float     attackDuration;
    sf::Clock animClock;
    float     animSpeed;
    float     idleAnimSpeed;
    int       currentFrame;

    float speed;
    float velocityY;
    float gravity;

    bool attacking;
    bool hitRegistered;
    bool onGround;
    bool jumpKeyHeld;
    bool attackKeyHeld;
    bool movingLeft;
    bool movingRight;
    bool facingRight;

    bool blockedLeft;
    bool blockedRight;

    int health;
    int maxHealth;

    bool      comboKeyHeld;
    bool      comboing;
    int       comboPhase;
    bool      comboHit1Registered;
    bool      comboHit2Registered;
    sf::Clock comboClock;

    bool      stunned;
    sf::Clock stunClock;
    float     stunDuration;

    // ================================================================
    //  Block state machine
    //
    //   NONE    : not blocking
    //   ENTER   : playing entry frames 0..3 from block_progression
    //   HOLD    : L still held; frame 4 (full stance) displayed
    //   SUCCESS : a blocked hit just landed; block_success plays once
    //             then returns to HOLD (if L still held) or NONE
    //
    //  isBlocking() is true during ENTER and HOLD — the window in
    //  which the combat system applies reduced damage.
    // ================================================================

    enum class BlockPhase { NONE, ENTER, HOLD, SUCCESS };

    BlockPhase blockPhase;
    bool       blockKeyHeld;
    int        blockFrame;        // 0..3 during ENTER, 4 during HOLD
    int        blockSuccessFrame; // current frame of success animation
    sf::Clock  blockAnimClock;
    float      blockAnimSpeed;        // seconds per frame for entry
    float      blockSuccessAnimSpeed; // seconds per frame for success

public:
    Player();

    void handleInput();
    void update();
    void reset();
    void draw(sf::RenderWindow& window);

    sf::FloatRect getAttackBox();
    sf::FloatRect getBounds();

    bool isAttacking()    const { return attacking; }
    bool isOnGround()     const { return onGround; }
    int  getHealth()      const { return health; }
    int  getMaxHealth()   const { return maxHealth; }
    bool isDead()         const { return health <= 0; }

    bool getHitRegistered() const { return hitRegistered; }
    void setHitRegistered(bool v) { hitRegistered = v; }

    bool isComboing()             const { return comboing; }
    int  getComboPhase()          const { return comboPhase; }
    bool getComboHit1Registered() const { return comboHit1Registered; }
    bool getComboHit2Registered() const { return comboHit2Registered; }
    void setComboHit1Registered(bool v) { comboHit1Registered = v; }
    void setComboHit2Registered(bool v) { comboHit2Registered = v; }

    bool isStunned() const { return stunned; }
    void applyStun(float dur);

    void takeDamage(int dmg) { health -= dmg; if (health < 0) health = 0; }

    void setCollisionWall(bool left, bool right)
    {
        blockedLeft = left;
        blockedRight = right;
    }

    void pushX(float amount);

    // True while L is held (ENTER or HOLD). The combat code uses this
    // to decide whether incoming damage should be halved.

    bool isBlocking() const
    {
        return blockPhase == BlockPhase::ENTER || blockPhase == BlockPhase::HOLD;
    }

    // Called by combat the moment a blocked hit lands.
    void triggerBlockSuccess();
};
