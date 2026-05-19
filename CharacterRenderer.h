//------------------------------------ ( CharacterRenderer.h ) ----------------------------------
// Manages sprite sheet animation for Veilwalker and Spirits.
// Each character has states (Idle, Walk, Attack, …)

#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <optional>
#include <unordered_map>
#include <vector>
#include <tuple>

enum class CharacterState
{
Idle,
Walk,
Attack,
Block,
Negotiate,
Ability,      // Veilwalker special move
Item,         // using a healing potion
Dying,
ShapeShift,   // Shadow Sovereign only
Steal         // Phantom Wraith only
};

// Hash function for CharacterState enum to use in unordered_map..
struct CharacterStateHash
{
std::size_t operator()(CharacterState state) const
{
return std::hash<int>()(static_cast<int>(state));
}
};

class CharacterRenderer
{
public:
CharacterRenderer();
bool loadStateFromSheet(CharacterState state,const std::string& sheetPath,sf::Vector2i frameSize,const std::vector<int>& frameIndices,float frameDuration = 0.10f);
bool loadMultipleStates(const std::unordered_map<CharacterState,std::tuple<std::string, sf::Vector2i, std::vector<int>>>& stateConfigs,float frameDuration = 0.10f);
void setState(CharacterState state);
void update(float dt);
void draw(sf::RenderWindow& window);
void setPosition(const sf::Vector2f& feetPos);
sf::Vector2f getPosition() const;
void setScale(float scale);
void setFacingRight(bool facingRight);
bool isFacingRight() const;
sf::FloatRect getGlobalBounds() const;
sf::FloatRect getAttackBox() const;
private:
std::unordered_map<CharacterState, sf::Texture, CharacterStateHash> m_textures;
std::unordered_map<CharacterState, std::optional<sf::Sprite>, CharacterStateHash> m_sprites; 
std::unordered_map<CharacterState, std::vector<sf::IntRect>, CharacterStateHash> m_frames;
std::unordered_map<CharacterState, sf::Vector2i, CharacterStateHash> m_frameSizes;
CharacterState m_currentState = CharacterState::Idle;
int m_currentFrameIndex = 0;
float m_frameDuration = 0.10f;
float m_timeAccum = 0.f;
float m_baseScale = 1.f;
bool m_facingRight = true;
// Internal helpers...
bool hasSpriteFor(CharacterState state) const;
sf::Sprite& spriteFor(CharacterState state);
const sf::Sprite& spriteFor(CharacterState state) const;
};