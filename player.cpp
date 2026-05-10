#include "player.h"
#include <SFML/Graphics.hpp>

const int IDLE_FRAMES = 4;
const int RUN_FRAMES = 8;
const int ATTACK_FRAMES = 8;

// =======================================================================
//  Block-sprite constants — derived from pixel analysis of the sheets
//
//  block_progression.png   (1536 x 1024)
//  -------------------------------------
//  The sheet has 8 distinct character poses in the content band
//  (rows 471-637).  The user-facing animation uses only the first
//  FOUR poses:
//
//      frame idx   pose description          crop_x   crop_w
//      -------------------------------------------------------
//      0           idle / ready               21       160
//      1           starting to draw           174      179
//      2           sword raised               349      183
//      3           BLOCK HOLD STANCE          533      180   ← hold here
//
//  Poses 4–7 show an orange/golden "hit-absorbed" effect and must NOT
//  be used for the block stance; block_success.png is used for that.
//
//  Crop widths are chosen so each crop ends just before the next pose's
//  content begins, preventing any sprite bleed:
//    frame 0: ends at 181, next content at 184  (+3 px gap)
//    frame 1: ends at 353, next content at 355  (+2 px gap)
//    frame 2: ends at 532, next content at 538  (+6 px gap)
//    frame 3: ends at 713, next content at 747  (+34 px gap)
//
//  Feet are anchored at x ≈ 60 within every crop (measured from the
//  sheet), so the character stays horizontally centred across frames.
//
//  block_success.png   (1536 x 1024)
//  ---------------------------------
//  Only sheet columns 2, 3 and 4 contain pixels (x = 512, 768, 1024).
//  Columns 0-1 and 5 are completely empty.  The character + spark
//  effect span rows 140-820 (height = 680) across all three frames.
// =======================================================================

const int BP_CONTENT_Y = 471;
const int BP_CONTENT_H = 167;     // 637 - 471 + 1

// Crop x-start and width for each of the four block-progression frames.
// Variable widths avoid bleeding into adjacent poses.
static const int BP_FRAME_X[4] = { 21, 174, 349, 533 };
static const int BP_FRAME_W[4] = { 160, 179, 183, 180 };

const int BS_CONTENT_Y = 140;
const int BS_CONTENT_H = 680;  // 819 - 140 + 1
const int BS_CROP_W = 256;
const int BS_TOTAL_FRAMES = 3;
static const int BS_FRAME_X[3] = { 512, 768, 1024 };

// Display scale for the block character.
//
// The main-character body inside its 96x80 frame is ~17-22 px wide and
// ~30-34 px tall in the texture, which becomes ~57x99 px on screen at
// the main scale of 3.0.  The block-progression body fills almost the
// full 158 px of crop height, so to make the visible block character
// match the on-screen height of the main character we scale by
// 99/158 ≈ 0.6.
const float BLOCK_SCALE = 0.6f;

// Block sprite top-left y.
//
// The main character's feet sit at on-screen y = 360 + 57*3 = 531
// (texture body bottom row 57, sprite y=360, scale 3).  Inside the
// block crop the body extends down to relative y = 166, so to put the
// block character's feet at the same on-screen y we use:
//      BLOCK_Y + 166 * 0.6 = 531  ->  BLOCK_Y = 431.
const float BLOCK_Y = 431.f;

// X offset that horizontally aligns the block character's feet with
// the main character's body centre.
//
//   main body centre on screen  = sprite.x + 46 * 3      = sprite.x + 138
//   block feet on screen        = sprite.x + offset + 60 * 0.6
//                               = sprite.x + offset + 36
//   set them equal ->  offset = 102.
const float BLOCK_X_OFFSET = 102.f;

// Block-state constants
const int BLOCK_ENTER_FRAMES = 3;   // frames 0, 1, 2 played on entry
const int BLOCK_HOLD_FRAME_IDX = 3;   // frame 3 = block-hold stance

// =======================================================================
//  Player constructor
// =======================================================================
Player::Player()
{
    idleRightTexture.loadFromFile("Assets/Sprites/mainChar/IDLE/idle_right.png");
    idleLeftTexture.loadFromFile("Assets/Sprites/mainChar/IDLE/idle_left.png");
    runRightTexture.loadFromFile("Assets/Sprites/mainChar/RUN/run_right.png");
    runLeftTexture.loadFromFile("Assets/Sprites/mainChar/RUN/run_left.png");
    attackRightTexture.loadFromFile("Assets/Sprites/mainChar/ATTACK 1/attack1_right.png");
    attackLeftTexture.loadFromFile("Assets/Sprites/mainChar/ATTACK 1/attack1_left.png");
    attackUpTexture.loadFromFile("Assets/Sprites/mainChar/ATTACK 1/attack1_up.png");
    blockProgressionTexture.loadFromFile("Assets/Sprites/mainChar/block_progression.png");
    blockSuccessTexture.loadFromFile("Assets/Sprites/mainChar/block_success.png");

    sprite.setTextureRect(sf::IntRect({ 0, 0 }, { 96, 80 }));
    sprite.setScale(sf::Vector2f(3.f, 3.f));
    sprite.setPosition(sf::Vector2f(0.f, 360.f));

    attackDuration = 0.2f;
    animSpeed = 0.08f;
    idleAnimSpeed = 0.18f;
    currentFrame = 0;

    speed = 0.25f;
    velocityY = 0.f;
    gravity = 0.004f;
    onGround = true;

    attacking = false;
    hitRegistered = false;
    jumpKeyHeld = false;
    attackKeyHeld = false;
    movingLeft = false;
    movingRight = false;
    facingRight = true;

    blockedLeft = false;
    blockedRight = false;

    health = 400;
    maxHealth = 400;

    comboKeyHeld = false;
    comboing = false;
    comboPhase = 0;
    comboHit1Registered = false;
    comboHit2Registered = false;

    stunned = false;
    stunDuration = 0.f;

    blockPhase = BlockPhase::NONE;
    blockKeyHeld = false;
    blockFrame = 0;
    blockSuccessFrame = 0;
    blockAnimSpeed = 0.08f;   // entry frame speed
    blockSuccessAnimSpeed = 0.07f;   // success frame speed
}

// =======================================================================
//  reset
// =======================================================================
void Player::reset()
{
    sprite.setPosition(sf::Vector2f(0.f, 360.f));
    health = maxHealth;
    velocityY = 0.f;
    onGround = true;

    attacking = false;
    hitRegistered = false;
    jumpKeyHeld = false;
    attackKeyHeld = false;
    movingLeft = false;
    movingRight = false;
    facingRight = true;
    currentFrame = 0;

    blockedLeft = false;
    blockedRight = false;

    comboing = false;
    comboKeyHeld = false;
    comboPhase = 0;
    comboHit1Registered = false;
    comboHit2Registered = false;

    stunned = false;
    stunDuration = 0.f;

    blockPhase = BlockPhase::NONE;
    blockKeyHeld = false;
    blockFrame = 0;
    blockSuccessFrame = 0;
}

// =======================================================================
//  applyStun — interrupts every active action including the block
// =======================================================================
void Player::applyStun(float dur)
{
    stunned = true;
    stunDuration = dur;
    stunClock.restart();

    attacking = false;
    hitRegistered = false;
    comboing = false;
    comboPhase = 0;
    comboHit1Registered = false;
    comboHit2Registered = false;
    currentFrame = 0;
    animClock.restart();

    blockPhase = BlockPhase::NONE;
    blockFrame = 0;
    blockSuccessFrame = 0;
}

// =======================================================================
//  triggerBlockSuccess — called the instant a blocked hit lands
// =======================================================================
void Player::triggerBlockSuccess()
{
    blockPhase = BlockPhase::SUCCESS;
    blockSuccessFrame = 0;
    blockAnimClock.restart();
}

// =======================================================================
//  handleInput
// =======================================================================
void Player::handleInput()
{
    movingLeft = false;
    movingRight = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    {
        facingRight = true;
        movingRight = true;
        if (!blockedRight)
            sprite.move(sf::Vector2f(speed, 0.f));
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    {
        facingRight = false;
        movingLeft = true;
        if (!blockedLeft)
            sprite.move(sf::Vector2f(-speed, 0.f));
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
    {
        if (onGround && !jumpKeyHeld)
        {
            velocityY = -1.5f;
            onGround = false;
        }
        jumpKeyHeld = true;
    }
    else
    {
        jumpKeyHeld = false;
    }

    // J — single attack (cancels any active block)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::J))
    {
        if (!attackKeyHeld && !attacking && !comboing && !stunned)
        {
            blockPhase = BlockPhase::NONE;
            blockFrame = 0;

            attacking = true;
            hitRegistered = false;
            currentFrame = 0;
            animClock.restart();
            attackClock.restart();
        }
        attackKeyHeld = true;
    }
    else
    {
        attackKeyHeld = false;
    }

    // K — combo (cancels any active block)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::K))
    {
        if (!comboKeyHeld && !attacking && !comboing && !stunned)
        {
            blockPhase = BlockPhase::NONE;
            blockFrame = 0;

            comboing = true;
            comboPhase = 1;
            comboHit1Registered = false;
            comboHit2Registered = false;
            currentFrame = 0;
            animClock.restart();
            comboClock.restart();
        }
        comboKeyHeld = true;
    }
    else
    {
        comboKeyHeld = false;
    }

    // L — block.  Holding allowed; releasing ends ENTER/HOLD immediately.
    // SUCCESS is not interrupted by release; once it finishes the code
    // checks blockKeyHeld to decide whether to return to HOLD or NONE.
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::L))
    {
        if (blockPhase == BlockPhase::NONE && !attacking && !comboing && !stunned)
        {
            blockPhase = BlockPhase::ENTER;
            blockFrame = 0;
            blockAnimClock.restart();
        }
        blockKeyHeld = true;
    }
    else
    {
        blockKeyHeld = false;
        if (blockPhase == BlockPhase::ENTER || blockPhase == BlockPhase::HOLD)
            blockPhase = BlockPhase::NONE;
    }
}

// =======================================================================
//  update
//
//  Block phases here only advance frame counters and state transitions.
//  No textures / rects on the main sprite are touched while blocking —
//  the rendering happens separately in draw() through blockSprite /
//  blockSuccSprite.  Each block branch returns early so the normal
//  idle/run/attack animation block at the bottom is skipped.
// =======================================================================
void Player::update()
{
    // ---- Stun timer ----
    if (stunned && stunClock.getElapsedTime().asSeconds() > stunDuration)
        stunned = false;

    // ---- J-attack timer ----
    if (attacking && attackClock.getElapsedTime().asSeconds() > attackDuration)
    {
        attacking = false;
        hitRegistered = false;
        currentFrame = 0;
        animClock.restart();
    }

    // ---- Combo phase transitions ----
    if (comboing)
    {
        float elapsed = comboClock.getElapsedTime().asSeconds();

        if (comboPhase == 1 && elapsed > attackDuration)
        {
            comboPhase = 2;
            currentFrame = 0;
            animClock.restart();
            comboClock.restart();
        }
        else if (comboPhase == 2 && elapsed > attackDuration)
        {
            comboing = false;
            comboPhase = 0;
            currentFrame = 0;
            animClock.restart();
        }
    }

    // ---- Gravity / floor ----
    velocityY += gravity;
    sprite.move(sf::Vector2f(0.f, velocityY));

    if (sprite.getPosition().y >= 360.f)
    {
        sprite.setPosition(sf::Vector2f(sprite.getPosition().x, 360.f));
        velocityY = 0.f;
        onGround = true;
    }

    if (sprite.getPosition().x < -40.f)
        sprite.setPosition(sf::Vector2f(-40.f, sprite.getPosition().y));
    if (sprite.getPosition().x > 560.f)
        sprite.setPosition(sf::Vector2f(560.f, sprite.getPosition().y));

    // ====================================================================
    //  Block state machine
    // ====================================================================

    if (blockPhase == BlockPhase::ENTER)
    {
        if (blockAnimClock.getElapsedTime().asSeconds() > blockAnimSpeed)
        {
            blockAnimClock.restart();
            blockFrame++;
            if (blockFrame >= BLOCK_ENTER_FRAMES)
            {
                blockFrame = BLOCK_HOLD_FRAME_IDX;
                blockPhase = BlockPhase::HOLD;
            }
        }
        return;   // skip normal animation
    }

    if (blockPhase == BlockPhase::HOLD)
    {
        return;   // draw() shows frame 4 every tick
    }

    if (blockPhase == BlockPhase::SUCCESS)
    {
        if (blockAnimClock.getElapsedTime().asSeconds() > blockSuccessAnimSpeed)
        {
            blockAnimClock.restart();
            blockSuccessFrame++;

            if (blockSuccessFrame >= BS_TOTAL_FRAMES)
            {
                blockSuccessFrame = 0;
                blockFrame = BLOCK_HOLD_FRAME_IDX;
                blockPhase = blockKeyHeld ? BlockPhase::HOLD : BlockPhase::NONE;
                if (blockPhase == BlockPhase::NONE)
                {
                    currentFrame = 0;
                    animClock.restart();
                }
            }
        }
        return;   // skip normal animation
    }

    // ====================================================================
    //  Normal animation: idle / run / attack
    // ====================================================================
    int   totalFrames;
    float activeAnimSpeed;

    if (comboing && comboPhase == 1)
    {
        sprite.setTexture(facingRight ? attackRightTexture : attackLeftTexture);
        totalFrames = ATTACK_FRAMES;
        activeAnimSpeed = animSpeed;
    }
    else if (comboing && comboPhase == 2)
    {
        sprite.setTexture(attackUpTexture);
        totalFrames = ATTACK_FRAMES;
        activeAnimSpeed = animSpeed * 0.75f;
    }
    else if (attacking)
    {
        sprite.setTexture(facingRight ? attackRightTexture : attackLeftTexture);
        totalFrames = ATTACK_FRAMES;
        activeAnimSpeed = animSpeed;
    }
    else if (movingRight)
    {
        sprite.setTexture(runRightTexture);
        totalFrames = RUN_FRAMES;
        activeAnimSpeed = animSpeed;
    }
    else if (movingLeft)
    {
        sprite.setTexture(runLeftTexture);
        totalFrames = RUN_FRAMES;
        activeAnimSpeed = animSpeed;
    }
    else
    {
        sprite.setTexture(facingRight ? idleRightTexture : idleLeftTexture);
        totalFrames = IDLE_FRAMES;
        activeAnimSpeed = idleAnimSpeed;
    }

    if (animClock.getElapsedTime().asSeconds() > activeAnimSpeed)
    {
        currentFrame = (currentFrame + 1) % totalFrames;
        animClock.restart();
    }
    if (currentFrame >= totalFrames) currentFrame = 0;

    sprite.setTextureRect(sf::IntRect({ currentFrame * 96, 0 }, { 96, 80 }));
}

// =======================================================================
//  getAttackBox
// =======================================================================
sf::FloatRect Player::getAttackBox()
{
    if (facingRight)
        return sf::FloatRect(
            sf::Vector2f(sprite.getPosition().x + 150.f, sprite.getPosition().y + 80.f),
            sf::Vector2f(80.f, 60.f));
    else
        return sf::FloatRect(
            sf::Vector2f(sprite.getPosition().x + 60.f, sprite.getPosition().y + 80.f),
            sf::Vector2f(80.f, 60.f));
}

// =======================================================================
//  getBounds
// =======================================================================
sf::FloatRect Player::getBounds()
{
    return sf::FloatRect(
        sf::Vector2f(sprite.getPosition().x + 120.f, sprite.getPosition().y + 80.f),
        sf::Vector2f(70.f, 130.f));
}

// =======================================================================
//  draw
//
//  In a non-block phase the main sprite is drawn (preserving every
//  pre-existing behaviour).  In any block phase the dedicated block
//  sprite is drawn at its own position/scale; the main sprite is
//  not drawn but still drives physics/collision through update().
// =======================================================================
void Player::draw(sf::RenderWindow& window)
{
    if (blockPhase == BlockPhase::NONE)
    {
        window.draw(sprite);
        return;
    }

    // Track horizontal position with the main sprite so the block
    // moves with the player.
    float bx = sprite.getPosition().x;

    // Pick which block_progression frame to show.
    //  ENTER   : frames 0, 1, 2 (entry animation in progress)
    //  HOLD    : frame 3 (defensive block stance)
    //  SUCCESS : frame 3 underneath the spark overlay
    int animIdx = (blockPhase == BlockPhase::HOLD || blockPhase == BlockPhase::SUCCESS)
        ? BLOCK_HOLD_FRAME_IDX : blockFrame;
    if (animIdx < 0) animIdx = 0;
    if (animIdx > 3) animIdx = 3;

    int frameW = BP_FRAME_W[animIdx];   // per-frame width, avoids bleed

    sf::IntRect bpRect(
        { BP_FRAME_X[animIdx], BP_CONTENT_Y },
        { frameW,              BP_CONTENT_H });

    if (facingRight)
    {
        blockSprite.setScale(sf::Vector2f(BLOCK_SCALE, BLOCK_SCALE));
        blockSprite.setPosition(sf::Vector2f(bx + BLOCK_X_OFFSET, BLOCK_Y));
    }
    else
    {
        // Negative x-scale flips horizontally; shift right by on-screen width.
        blockSprite.setScale(sf::Vector2f(-BLOCK_SCALE, BLOCK_SCALE));
        blockSprite.setPosition(sf::Vector2f(
            bx + BLOCK_X_OFFSET + frameW * BLOCK_SCALE, BLOCK_Y));
    }
    blockSprite.setTextureRect(bpRect);
    window.draw(blockSprite);

    // Spark overlay during a successful block
    if (blockPhase == BlockPhase::SUCCESS)
    {
        int succIdx = blockSuccessFrame;
        if (succIdx < 0)                succIdx = 0;
        if (succIdx >= BS_TOTAL_FRAMES) succIdx = BS_TOTAL_FRAMES - 1;

        sf::IntRect bsRect(
            { BS_FRAME_X[succIdx], BS_CONTENT_Y },
            { BS_CROP_W,           BS_CONTENT_H });

        // The 256x680 success crop is large; scale it down so the
        // spark frames the (now smaller) character.
        // The SUCCESS state always shows the hold frame (BP_FRAME_W[3]=180).
        const float HOLD_W = 180.f;   // BP_FRAME_W[BLOCK_HOLD_FRAME_IDX]
        const float SUCC_SCALE = 0.2f;
        const float SUCC_W_ON_SCREEN = BS_CROP_W * SUCC_SCALE;   // ~51 px

        if (facingRight)
        {
            blockSuccSprite.setScale(sf::Vector2f(SUCC_SCALE, SUCC_SCALE));
            blockSuccSprite.setPosition(sf::Vector2f(
                bx + BLOCK_X_OFFSET + (HOLD_W * BLOCK_SCALE - SUCC_W_ON_SCREEN) * 0.5f,
                BLOCK_Y - 25.f));
        }
        else
        {
            blockSuccSprite.setScale(sf::Vector2f(-SUCC_SCALE, SUCC_SCALE));
            blockSuccSprite.setPosition(sf::Vector2f(
                bx + BLOCK_X_OFFSET + (HOLD_W * BLOCK_SCALE + SUCC_W_ON_SCREEN) * 0.5f,
                BLOCK_Y - 25.f));
        }
        blockSuccSprite.setTextureRect(bsRect);
        window.draw(blockSuccSprite);
    }
}

// =======================================================================
//  pushX
// =======================================================================
void Player::pushX(float amount)
{
    sf::Vector2f pos = sprite.getPosition();
    pos.x += amount;
    if (pos.x < -40.f) pos.x = -40.f;
    if (pos.x > 560.f) pos.x = 560.f;
    sprite.setPosition(pos);
}
