// CharacterRenderer.h
// Manages sprite‑sheet animation for Veilwalker and Spirits.
// Each character has states (Idle, Walk, Attack, …) and we cycle
// through the appropriate set of frames.

#pragma once
#include <SFML/Graphics.hpp>
#include<iostream>
#include <unordered_map>
#include <vector>

enum class CharacterState
{
    Idle,
    Walk,
    Attack,
    Block,
    Negotiate,      // for guidance / trickster spirits
    Ability,        // Veilwalker special move
    Item,           // using a potion
    Dying,
    Steal           // Phantom Wraith only
};

class CharacterRenderer
{
public:
    CharacterRenderer();

    // Load the sprite sheet and define the frame rectangles for each state.
    // sheetPath – path to the texture file (PNG)
    // frameSize – width and height of a single frame
    // animations – map from state to a vector of frame indices (row‑major, 0‑based)
    // frameDuration – seconds per frame for all animations (can be overridden per state later)
    bool loadFromSheet(const std::string& sheetPath,
        sf::Vector2i frameSize,
        const std::unordered_map<CharacterState, std::vector<int>>& animations,
        float frameDuration = 0.12f);

    // Set the current visual state. Animation resets.
    void setState(CharacterState state);

    // Advance animation by dt seconds.
    void update(float dt);

    // Draw the sprite at the current world position.
    void draw(sf::RenderWindow& window);

    // Position helpers (the anchor is the bottom‑center of the feet).
    void setPosition(const sf::Vector2f& feetPos);
    sf::Vector2f getPosition() const;

    // Scale / flip
    void setScale(float scale);
    void setFacingRight(bool facingRight);

    // Bounding boxes for collisions (if needed later).
    sf::FloatRect getGlobalBounds() const;
    sf::FloatRect getAttackBox() const;       // approximate attack hitbox

private:
    sf::Texture m_texture;
    sf::Sprite  m_sprite;
    sf::Vector2i m_frameSize;
    std::unordered_map<CharacterState, std::vector<sf::IntRect>> m_frames;
    CharacterState m_currentState = CharacterState::Idle;
    int m_currentFrameIndex = 0;
    float m_frameDuration = 0.12f;
    float m_timeAccum = 0.f;
    bool m_facingRight = true;
};