#include "enemy.h"
#include <cmath>
#include <cstdlib>

// ============================================================
//  Enemy  (base class)
// ============================================================
Enemy::Enemy(int hp, float spd, float atkCooldown, float atkDuration,
    int fw, int fh, int idleFr, int runFr, int atkFr)
{
    worldX = 550.f;
    animSpeed = 0.12f;
    blockAnimSpeed = 0.12f;
    currentFrame = 0;
    idleFrames = idleFr;
    runFrames = runFr;
    attackFrames = atkFr;
    blockFrames = 4;
    hurtFrames = 0;
    frameW = fw;
    frameH = fh;

    health = hp;
    maxHealth = hp;
    speed = spd;

    attacking = false;
    blocking = false;
    facingLeft = true;

    dyingAnimPlaying = false;
    dyingAnimDone = false;

    attackCooldown = atkCooldown;
    attackDuration = atkDuration;

    velocityY = 0.f;
    gravity = 0.006f;
    onGround = true;
}

void Enemy::block(bool shouldBlock)
{
    blocking = shouldBlock;
    if (blocking)
    {
        if (blockTexture.getSize().x > 0)
            sprite.setTexture(blockTexture);
        else
            sprite.setTexture(idleTexture);

        if (animClock.getElapsedTime().asSeconds() > animSpeed)
        {
            currentFrame = (currentFrame + 1) % blockFrames;
            animClock.restart();
        }
        sprite.setTextureRect(sf::IntRect({ currentFrame * frameW, 0 }, { frameW, frameH }));
    }
}

void Enemy::takeDamage(int dmg)
{
    if (blocking)
        dmg = (int)(dmg * 0.2f);
    health -= dmg;
    if (health < 0) health = 0;

    if (health == 0 && !dyingAnimPlaying)
    {
        dyingAnimPlaying = true;
        dyingAnimDone = false;
        attacking = false;
        blocking = false;
        currentFrame = 0;
        animClock.restart();
    }
}

sf::FloatRect Enemy::getBounds() const
{
    return sf::FloatRect(
        sf::Vector2f(worldX + 40.f, sprite.getPosition().y + 30.f),
        sf::Vector2f(80.f, 130.f));
}

sf::FloatRect Enemy::getAttackBox() const
{
    float bx;
    if (facingLeft)
        bx = worldX - 60.f;
    else
        bx = worldX + frameW * std::abs(sprite.getScale().x);

    return sf::FloatRect(
        sf::Vector2f(bx, sprite.getPosition().y + 60.f),
        sf::Vector2f(80.f, 60.f));
}

void Enemy::stepAnimation(int frames)
{
    if (animClock.getElapsedTime().asSeconds() > animSpeed)
    {
        currentFrame = (currentFrame + 1) % frames;
        animClock.restart();
    }
    if (currentFrame >= frames) currentFrame = 0;
    sprite.setTextureRect(sf::IntRect({ currentFrame * frameW, 0 }, { frameW, frameH }));
}

bool Enemy::stepHurtAnimation()
{
    sprite.setTexture(hurtTexture);
    sprite.setTextureRect(sf::IntRect({ currentFrame * frameW, 0 }, { frameW, frameH }));

    if (animClock.getElapsedTime().asSeconds() > animSpeed)
    {
        animClock.restart();
        if (currentFrame < hurtFrames - 1)
            currentFrame++;
        else
            return true;
    }
    return false;
}

void Enemy::updateFacingScale()
{
    float s = std::abs(sprite.getScale().y);
    float renderedW = (float)frameW * s;
    sprite.setOrigin(sf::Vector2f(0.f, 0.f));

    if (facingLeft)
    {
        sprite.setScale(sf::Vector2f(-s, s));
        sprite.setPosition(sf::Vector2f(worldX + renderedW, sprite.getPosition().y));
    }
    else
    {
        sprite.setScale(sf::Vector2f(s, s));
        sprite.setPosition(sf::Vector2f(worldX, sprite.getPosition().y));
    }
}

void Enemy::setWorldX(float x)
{
    worldX = x;
    updateFacingScale();
}

void Enemy::reset()
{
    health = maxHealth;
    worldX = 550.f;
    attacking = false;
    blocking = false;
    dyingAnimPlaying = false;
    dyingAnimDone = false;
    currentFrame = 0;
    velocityY = 0.f;
    onGround = true;
    facingLeft = true;
    updateFacingScale();
}

void Enemy::applyGravityAndFloor(float floorY)
{
    velocityY += gravity;
    float newY = sprite.getPosition().y + velocityY;
    if (newY >= floorY)
    {
        newY = floorY;
        velocityY = 0.f;
        onGround = true;
    }
    if (facingLeft)
        sprite.setPosition(
            sf::Vector2f(worldX + (float)frameW * std::abs(sprite.getScale().x), newY));
    else
        sprite.setPosition(sf::Vector2f(worldX, newY));
}

void Enemy::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}


// ============================================================
//  Creature  (Stage 1)
// ============================================================
const float CREATURE_SCALE = 1.65f;
const float CREATURE_FLOOR_Y = 382.5f;

Creature::Creature()
    : Enemy(250, 0.15f, 2.0f, 0.55f, 148, 96, 6, 8, 5)   
{
    ATTACK_RANGE = 120.f;
    DETECT_RANGE = 300.f;
    aiState = AI_IDLE;

    idleTexture.loadFromFile("Assets/Sprites/creature/IDLE.png");
    runTexture.loadFromFile("Assets/Sprites/creature/RUN.png");
    attackTexture.loadFromFile("Assets/Sprites/creature/ATTACK 1.png");

    worldX = 550.f;
    facingLeft = true;
    sprite.setTexture(idleTexture);
    sprite.setTextureRect(sf::IntRect({ 0, 0 }, { frameW, frameH }));
    sprite.setScale(sf::Vector2f(CREATURE_SCALE, CREATURE_SCALE));
    sprite.setPosition(sf::Vector2f(worldX, CREATURE_FLOOR_Y));
    updateFacingScale();
}

sf::FloatRect Creature::getBounds() const
{
    float bx = facingLeft ? worldX + 117.f : worldX + 45.f;
    return sf::FloatRect(
        sf::Vector2f(bx, sprite.getPosition().y + 48.f),
        sf::Vector2f(83.f, 101.f));
}

sf::FloatRect Creature::getAttackBox() const
{
    if (facingLeft)
        return sf::FloatRect(
            sf::Vector2f(worldX + 74.f, sprite.getPosition().y + 80.f),
            sf::Vector2f(63.f, 40.f));
    else
        return sf::FloatRect(
            sf::Vector2f(worldX + 107.f, sprite.getPosition().y + 80.f),
            sf::Vector2f(63.f, 40.f));
}

void Creature::update(sf::Vector2f playerPos)
{
    if (health <= 0) { dyingAnimDone = true; return; }

    sf::FloatRect eb = getBounds();
    float bodyCentreX = eb.position.x + eb.size.x * 0.5f;
    float dx = playerPos.x - bodyCentreX;
    float dist = std::abs(dx);

    if (aiState == AI_IDLE)
    {
        sprite.setTexture(idleTexture);
        stepAnimation(idleFrames);
        if (dist < DETECT_RANGE)
        {
            facingLeft = (dx < 0.f); aiState = AI_CHASE;
            currentFrame = 0; animClock.restart();
        }
    }
    else if (aiState == AI_CHASE)
    {
        facingLeft = (dx < 0.f);
        sprite.setTexture(runTexture);
        if (dist > 80.f)
            worldX += (dx > 0.f ? 1.f : -1.f) * speed;
        stepAnimation(runFrames);

        if (dist < ATTACK_RANGE && attackClock.getElapsedTime().asSeconds() > attackCooldown)
        {
            facingLeft = (dx < 0.f); aiState = AI_ATTACK; attacking = true;
            currentFrame = 0; animClock.restart(); attackClock.restart();
        }
        if (dist > DETECT_RANGE) { aiState = AI_IDLE; currentFrame = 0; animClock.restart(); }
    }
    else if (aiState == AI_ATTACK)
    {
        sprite.setTexture(attackTexture);
        stepAnimation(attackFrames);
        attacking = (attackClock.getElapsedTime().asSeconds() < attackDuration * 0.5f);
        if (attackClock.getElapsedTime().asSeconds() > attackDuration)
        {
            attacking = false; aiState = AI_CHASE;
            currentFrame = 0; animClock.restart();
        }
    }

    updateFacingScale();
    applyGravityAndFloor(CREATURE_FLOOR_Y);
    if (worldX < -40.f) { worldX = -40.f; updateFacingScale(); }
    if (worldX > 600.f) { worldX = 600.f; updateFacingScale(); }
}


// ============================================================
//  Knight  (Stage 2)
// ============================================================
const float KNIGHT_SCALE = 2.75f;
const float KNIGHT_FLOOR_Y = 363.f;
const float KNIGHT_MIN_X = -40.f;
const float KNIGHT_MAX_X = 530.f;

Knight::Knight()
    : Enemy(400, 0.15f, 2.0f, 0.55f, FRAME_W, 84, 7, 8, 6) 
{
    ATTACK_RANGE = 120.f;
    DETECT_RANGE = 300.f;
    aiState = AI_IDLE;

    blockFrames = 6;
    blockAnimSpeed = 0.04f;
    blockHoldDuration = 0.5f;
    wasPlayerAttacking = false;
    blockDecision = false;
    hurtFrames = 4;

    idleTexture.loadFromFile("Assets/Sprites/knight/IDLE.png");
    runTexture.loadFromFile("Assets/Sprites/knight/RUN.png");
    attackTexture.loadFromFile("Assets/Sprites/knight/ATTACK 1.png");
    blockTexture.loadFromFile("Assets/Sprites/knight/DEFEND.png");
    hurtTexture.loadFromFile("Assets/Sprites/knight/HURT.png");

    worldX = KNIGHT_MAX_X;
    facingLeft = true;
    sprite.setTexture(idleTexture);
    sprite.setTextureRect(sf::IntRect({ 0, 0 }, { frameW, frameH }));
    sprite.setScale(sf::Vector2f(KNIGHT_SCALE, KNIGHT_SCALE));
    sprite.setPosition(sf::Vector2f(worldX, KNIGHT_FLOOR_Y));
    updateFacingScale();
}

sf::FloatRect Knight::getBounds() const
{
    float bx = facingLeft
        ? worldX + (float)(FRAME_W - 62) * KNIGHT_SCALE
        : worldX + 33.f * KNIGHT_SCALE;
    return sf::FloatRect(
        sf::Vector2f(bx, sprite.getPosition().y + 27.f * KNIGHT_SCALE),
        sf::Vector2f(29.f * KNIGHT_SCALE, 34.f * KNIGHT_SCALE));
}

sf::FloatRect Knight::getAttackBox() const
{
    float sy = sprite.getPosition().y + 27.f * KNIGHT_SCALE;
    float ht = 34.f * KNIGHT_SCALE;
    if (facingLeft)
        return sf::FloatRect(
            sf::Vector2f(worldX + (float)(FRAME_W - 88) * KNIGHT_SCALE, sy),
            sf::Vector2f(26.f * KNIGHT_SCALE, ht));
    else
        return sf::FloatRect(
            sf::Vector2f(worldX + 62.f * KNIGHT_SCALE, sy),
            sf::Vector2f(26.f * KNIGHT_SCALE, ht));
}

void Knight::reset()
{
    Enemy::reset();
    worldX = KNIGHT_MAX_X;
    wasPlayerAttacking = false;
    blockDecision = false;
    blocking = false;
    aiState = AI_IDLE;
    sprite.setPosition(sf::Vector2f(worldX, KNIGHT_FLOOR_Y));
    updateFacingScale();
}

void Knight::tryBlock(bool playerIsAttacking)
{
    if (isDead() || attacking)
    {
        blocking = false;
        wasPlayerAttacking = playerIsAttacking;
        return;
    }

    if (playerIsAttacking && !wasPlayerAttacking)
    {
        blockDecision = (rand() % 100) < 50;
        if (blockDecision)
        {
            blockHoldClock.restart();
            if (!blocking) { currentFrame = 0; animClock.restart(); }
        }
    }
    wasPlayerAttacking = playerIsAttacking;

    if (blockDecision)
    {
        bool wasBlocking = blocking;
        blocking = (blockHoldClock.getElapsedTime().asSeconds() < blockHoldDuration);
        if (blocking && !wasBlocking) { currentFrame = 0; animClock.restart(); }
        if (!blocking) blockDecision = false;
    }
    else
    {
        blocking = false;
    }
}

void Knight::update(sf::Vector2f playerPos)
{
    if (health <= 0)
    {
        if (!dyingAnimDone)
        {
            updateFacingScale();
            applyGravityAndFloor(KNIGHT_FLOOR_Y);
            if (stepHurtAnimation()) dyingAnimDone = true;
            updateFacingScale();
        }
        return;
    }

    sf::FloatRect eb = getBounds();
    float bodyCentreX = eb.position.x + eb.size.x * 0.5f;
    float dx = playerPos.x - bodyCentreX;
    float dist = std::abs(dx);

    if (!attacking) facingLeft = (dx < 0.f);

    if (blocking)
    {
        sprite.setTexture(blockTexture);
        if (animClock.getElapsedTime().asSeconds() > blockAnimSpeed)
        {
            currentFrame = (currentFrame + 1) % blockFrames;
            animClock.restart();
        }
        if (currentFrame >= blockFrames) currentFrame = 0;
        sprite.setTextureRect(sf::IntRect({ currentFrame * frameW, 0 }, { frameW, frameH }));
        updateFacingScale();
        applyGravityAndFloor(KNIGHT_FLOOR_Y);
        if (worldX < KNIGHT_MIN_X) { worldX = KNIGHT_MIN_X; updateFacingScale(); }
        if (worldX > KNIGHT_MAX_X) { worldX = KNIGHT_MAX_X; updateFacingScale(); }
        return;
    }

    if (aiState == AI_IDLE)
    {
        sprite.setTexture(idleTexture);
        stepAnimation(idleFrames);
        if (dist < DETECT_RANGE)
        {
            facingLeft = (dx < 0.f); aiState = AI_CHASE;
            currentFrame = 0; animClock.restart();
        }
    }
    else if (aiState == AI_CHASE)
    {
        facingLeft = (dx < 0.f);
        sprite.setTexture(runTexture);
        if (dist > 80.f) worldX += (dx > 0.f ? 1.f : -1.f) * speed;
        stepAnimation(runFrames);
        if (dist < ATTACK_RANGE && attackClock.getElapsedTime().asSeconds() > attackCooldown)
        {
            facingLeft = (dx < 0.f); aiState = AI_ATTACK; attacking = true;
            currentFrame = 0; animClock.restart(); attackClock.restart();
        }
        if (dist > DETECT_RANGE) { aiState = AI_IDLE; currentFrame = 0; animClock.restart(); }
    }
    else if (aiState == AI_ATTACK)
    {
        sprite.setTexture(attackTexture);
        stepAnimation(attackFrames);
        attacking = (attackClock.getElapsedTime().asSeconds() < attackDuration * 0.5f);
        if (attackClock.getElapsedTime().asSeconds() > attackDuration)
        {
            attacking = false; aiState = AI_CHASE;
            currentFrame = 0; animClock.restart();
        }
    }

    updateFacingScale();
    applyGravityAndFloor(KNIGHT_FLOOR_Y);
    if (worldX < KNIGHT_MIN_X) { worldX = KNIGHT_MIN_X; updateFacingScale(); }
    if (worldX > KNIGHT_MAX_X) { worldX = KNIGHT_MAX_X; updateFacingScale(); }
}


// ============================================================
//  Samurai  (Stage 3)
// ============================================================
const float SAMURAI_SCALE = 3.0f;
const float SAMURAI_FLOOR_Y = 291.f;
const float SAMURAI_MIN_X = -40.f;
const float SAMURAI_MAX_X = 506.f;

const float AVAR_SCALE = 0.8f;
const float AVAR_Y = 373.f;
const float AVAR_FRAME_RENDERED_W = 192.f * AVAR_SCALE;                          // 153.6
const float AVAR_X_OFFSET = (96.f * SAMURAI_SCALE - AVAR_FRAME_RENDERED_W) * 0.5f; // 67.2

Samurai::Samurai()
    : Enemy(600, 0.18f, 1.5f, 0.6f, FRAME_W, 96, 10, 16, 7)
{
    ATTACK_RANGE = 120.f;
    DETECT_RANGE = 300.f;
    aiState = AI_IDLE;
    animSpeed = 0.08f;
    hurtFrames = 4;

    gravity = 0.004f;

    idleTexture.loadFromFile("Assets/Sprites/samurai/IDLE.png");
    runTexture.loadFromFile("Assets/Sprites/samurai/RUN.png");
    attackTexture.loadFromFile("Assets/Sprites/samurai/ATTACK 1.png");
    attackVarTexture.loadFromFile("Assets/Sprites/samurai/attack_var_3.png");
    hurtTexture.loadFromFile("Assets/Sprites/samurai/HURT.png");

    worldX = SAMURAI_MAX_X;
    facingLeft = true;
    sprite.setTexture(idleTexture);
    sprite.setTextureRect(sf::IntRect({ 0, 0 }, { frameW, frameH }));
    sprite.setScale(sf::Vector2f(SAMURAI_SCALE, SAMURAI_SCALE));
    sprite.setPosition(sf::Vector2f(worldX, SAMURAI_FLOOR_Y));
    updateFacingScale();

    usingVariant = false;
    drawingVariant = false;

    jumping = false;
    jumpVelocityX = 0.f;
    playerWasAttacking = false;
    jumpCooldownDuration = 3.0f;
}

sf::FloatRect Samurai::getBounds() const
{
    float bx = facingLeft
        ? worldX + (float)(FRAME_W - 60) * SAMURAI_SCALE   
        : worldX + 26.f * SAMURAI_SCALE;                   
    return sf::FloatRect(
        sf::Vector2f(bx, sprite.getPosition().y + 47.f * SAMURAI_SCALE),
        sf::Vector2f(29.f * SAMURAI_SCALE, 33.f * SAMURAI_SCALE));  
}

sf::FloatRect Samurai::getAttackBox() const
{
    float sy = sprite.getPosition().y + 50.f * SAMURAI_SCALE;
    float ht = 20.f * SAMURAI_SCALE;
    if (facingLeft)
        return sf::FloatRect(
            sf::Vector2f(worldX + 13.f * SAMURAI_SCALE, sy),
            sf::Vector2f(27.f * SAMURAI_SCALE, ht));
    else
        return sf::FloatRect(
            sf::Vector2f(worldX + 55.f * SAMURAI_SCALE, sy),
            sf::Vector2f(27.f * SAMURAI_SCALE, ht));
}

void Samurai::reset()
{
    Enemy::reset();
    gravity = 0.004f;     
    worldX = SAMURAI_MAX_X;
    aiState = AI_IDLE;
    usingVariant = false;
    drawingVariant = false;
    jumping = false;
    jumpVelocityX = 0.f;
    playerWasAttacking = false;
    sprite.setPosition(sf::Vector2f(worldX, SAMURAI_FLOOR_Y));
    updateFacingScale();
}


void Samurai::notifyPlayerAttacking(bool isAttacking)
{
    bool risingEdge = isAttacking && !playerWasAttacking;
    playerWasAttacking = isAttacking;

    if (!risingEdge)        return;
    if (isDead())           return;
    if (!onGround)          return;
    if (attacking)          return;
    if (aiState == AI_IDLE) return;

    if (jumpCooldownClock.getElapsedTime().asSeconds() > jumpCooldownDuration)
    {
        if ((rand() % 100) < 35)
        {
            velocityY = -1.5f;
            onGround = false;
            jumping = true;
            jumpVelocityX = facingLeft ? 0.3f : -0.3f;

            jumpCooldownClock.restart();
        }
    }
}

// ============================================================
//  Samurai::update
// ============================================================
void Samurai::update(sf::Vector2f playerPos)
{
    if (health <= 0)
    {
        jumping = false;
        if (!dyingAnimDone)
        {
            updateFacingScale();
            applyGravityAndFloor(SAMURAI_FLOOR_Y);
            if (stepHurtAnimation()) dyingAnimDone = true;
            updateFacingScale();
        }
        return;
    }

    sf::FloatRect eb = getBounds();
    float bodyCentreX = eb.position.x + eb.size.x * 0.5f;
    float dx = playerPos.x - bodyCentreX;
    float dist = std::abs(dx);

    if (!attacking) facingLeft = (dx < 0.f);

    // ======================================================
    //  JUMP DODGE
    // ======================================================
    if (jumping)
    {
        facingLeft = (dx < 0.f);
        worldX += jumpVelocityX;

        sprite.setTexture(runTexture);
        stepAnimation(runFrames);
        updateFacingScale();
        applyGravityAndFloor(SAMURAI_FLOOR_Y);

        if (onGround)
        {
            jumping = false;
            jumpVelocityX = 0.f;
            aiState = AI_CHASE;
            currentFrame = 0;
            animClock.restart();
        }

        if (worldX < SAMURAI_MIN_X) { worldX = SAMURAI_MIN_X; updateFacingScale(); }
        if (worldX > SAMURAI_MAX_X) { worldX = SAMURAI_MAX_X; updateFacingScale(); }
        return;
    }

    // ======================================================
    //  NORMAL AI STATE MACHINE
    // ======================================================
    if (aiState == AI_IDLE)
    {
        sprite.setTexture(idleTexture);
        stepAnimation(idleFrames);
        if (dist < DETECT_RANGE)
        {
            facingLeft = (dx < 0.f); aiState = AI_CHASE;
            currentFrame = 0; animClock.restart();
        }
    }
    else if (aiState == AI_CHASE)
    {
        facingLeft = (dx < 0.f);
        sprite.setTexture(runTexture);
        if (dist > 80.f) worldX += (dx > 0.f ? 1.f : -1.f) * speed;
        stepAnimation(runFrames);

        if (dist < ATTACK_RANGE && attackClock.getElapsedTime().asSeconds() > attackCooldown)
        {
            facingLeft = (dx < 0.f);
            aiState = AI_ATTACK; attacking = true;
            currentFrame = 0; animClock.restart(); attackClock.restart();

            // 50/50 choice between ATTACK 1 and attack_var
            usingVariant = ((rand() % 100) < 50);
            drawingVariant = usingVariant;
        }
        if (dist > DETECT_RANGE) { aiState = AI_IDLE; currentFrame = 0; animClock.restart(); }
    }
    else if (aiState == AI_ATTACK)
    {
        // --------------------------------------------------
        //  Pick texture and timing based on attack type
        // --------------------------------------------------
        int   activeFrames;
        float frameSpeed;

        if (usingVariant)
        {
            sprite.setTexture(attackVarTexture);
            activeFrames = AVAR_FRAMES;     // 4 (frames 3-6 in the sheet)
            frameSpeed = 0.09f;
            drawingVariant = true;
        }
        else
        {
            sprite.setTexture(attackTexture);
            activeFrames = attackFrames;    // 7
            frameSpeed = animSpeed;       // 0.08
            drawingVariant = false;
        }

        // Advance animation frame
        bool cycleComplete = false;
        if (animClock.getElapsedTime().asSeconds() > frameSpeed)
        {
            if (currentFrame == activeFrames - 1)
                cycleComplete = true;
            currentFrame = (currentFrame + 1) % activeFrames;
            animClock.restart();
        }
        if (currentFrame >= activeFrames) currentFrame = 0;

        // Set texture rect for the main sprite (used for physics; draw() overrides visuals)
        sprite.setTextureRect(sf::IntRect({ currentFrame * FRAME_W, 0 }, { FRAME_W, frameH }));

        // Hitbox active for the first half of the animation
        float elapsed = attackClock.getElapsedTime().asSeconds();
        attacking = (elapsed < attackDuration * 0.5f);

        // End of attack: cycle complete (attack_var) or time expired (ATTACK 1)
        bool timeUp = (!usingVariant && elapsed > attackDuration);
        bool cycleEnd = (usingVariant && cycleComplete);
        if (timeUp || cycleEnd)
        {
            attacking = false;
            usingVariant = false;
            drawingVariant = false;
            aiState = AI_CHASE;
            currentFrame = 0;
            animClock.restart();
        }
    }

    updateFacingScale();
    applyGravityAndFloor(SAMURAI_FLOOR_Y);
    if (worldX < SAMURAI_MIN_X) { worldX = SAMURAI_MIN_X; updateFacingScale(); }
    if (worldX > SAMURAI_MAX_X) { worldX = SAMURAI_MAX_X; updateFacingScale(); }
}

// ============================================================
//  Samurai::draw  (overrides Enemy::draw)
// ============================================================

void Samurai::draw(sf::RenderWindow& window)
{
    if (!drawingVariant)
    {
        window.draw(sprite);
        return;
    }

    // currentFrame counts 0..AVAR_FRAMES-1; offset into sheet by AVAR_FIRST_ATTACK_FRAME
    int sheetFrame = currentFrame + AVAR_FIRST_ATTACK_FRAME;
    int frameX = sheetFrame * AVAR_FRAME_W_TEX;

    attackVarSprite.setTextureRect(sf::IntRect(
        { frameX, AVAR_CONTENT_Y },
        { AVAR_FRAME_W_TEX, AVAR_CONTENT_H }));

    if (facingLeft)
    {
        // Mirror horizontally; anchor is the right edge of the rendered frame
        attackVarSprite.setScale(sf::Vector2f(-AVAR_SCALE, AVAR_SCALE));
        attackVarSprite.setPosition(sf::Vector2f(
            worldX + AVAR_X_OFFSET + AVAR_FRAME_RENDERED_W, AVAR_Y));
    }
    else
    {
        attackVarSprite.setScale(sf::Vector2f(AVAR_SCALE, AVAR_SCALE));
        attackVarSprite.setPosition(sf::Vector2f(
            worldX + AVAR_X_OFFSET, AVAR_Y));
    }
    window.draw(attackVarSprite);
}