//---------------------------------- ( CharacterRenderer.cpp ) ---------------------------------------
// Rendering Abstraction for all characters (Veilwalker and Spirits).
#include "CharacterRenderer.h"

//CONSTRUCTOR:-
CharacterRenderer::CharacterRenderer() {}

//---------------------------------------------------------------------
//Internal helpers
//---------------------------------------------------------------------
// Returns true only if this state has a fully constructed sprite...
bool CharacterRenderer::hasSpriteFor(CharacterState state) const
{
auto it = m_sprites.find(state);
return it != m_sprites.end() && it->second.has_value();
}

// Non-const accessor..
sf::Sprite& CharacterRenderer::spriteFor(CharacterState state)
{
return m_sprites.at(state).value();
}

// Const accessor..
const sf::Sprite& CharacterRenderer::spriteFor(CharacterState state) const
{
return m_sprites.at(state).value();
}

// Applies horizontal flip based on facing direction...
static void applyFacingScale(sf::Sprite& sprite, float baseScale, bool facingRight)
{
float xScale = facingRight ? baseScale : -baseScale;
sprite.setScale({ xScale, baseScale });
}

//Load one state..
bool CharacterRenderer::loadStateFromSheet(CharacterState state,const std::string& sheetPath,sf::Vector2i frameSize,const std::vector<int>& frameIndices,float frameDuration)
{
if (frameIndices.empty())
return false;
// Load texture for this state.
sf::Texture& texture = m_textures[state];
if (!texture.loadFromFile(sheetPath))
{
m_textures.erase(state);
return false;
}
m_sprites.insert_or_assign(state, std::make_optional<sf::Sprite>(texture));
m_frameSizes[state] = frameSize;
m_frameDuration = frameDuration;
// Convert frame indices to sf::IntRect list.
std::vector<sf::IntRect> rects;
int columns = static_cast<int>(texture.getSize().x) / frameSize.x;
for (int idx : frameIndices)
{
int col = idx % columns;
int row = idx / columns;
rects.push_back(sf::IntRect({ col * frameSize.x, row * frameSize.y },{ frameSize.x,       frameSize.y }));}
m_frames[state] = rects;
// Set the first frame and correct facing scale.
if (!rects.empty())
spriteFor(state).setTextureRect(rects[0]);
applyFacingScale(spriteFor(state), m_baseScale, m_facingRight);
return true;
}

//Load multiple states at once .......
bool CharacterRenderer::loadMultipleStates(const std::unordered_map<CharacterState,std::tuple<std::string, sf::Vector2i, std::vector<int>>>& stateConfigs,float frameDuration)
{
bool allLoaded = true;
for (const auto& [state, config] : stateConfigs)
{
const auto& [sheetPath, frameSize, frameIndices] = config;
if (!loadStateFromSheet(state, sheetPath, frameSize, frameIndices, frameDuration))
allLoaded = false;
}
return allLoaded;
}

//Set active animation state ..
void CharacterRenderer::setState(CharacterState state)
{
if (m_currentState == state)
return;
if (!hasSpriteFor(state))
return;   // state not loaded — ignore silently
m_currentState = state;
m_currentFrameIndex = 0;
m_timeAccum = 0.f;
auto it = m_frames.find(state);
if (it != m_frames.end() && !it->second.empty())
spriteFor(state).setTextureRect(it->second[0]);
}

//Advance animation..
void CharacterRenderer::update(float dt)
{
if (!hasSpriteFor(m_currentState))
return;
auto it = m_frames.find(m_currentState);
if (it == m_frames.end() || it->second.empty())
return;
const auto& frames = it->second;
m_timeAccum += dt;
while (m_timeAccum >= m_frameDuration)
{
m_timeAccum -= m_frameDuration;
m_currentFrameIndex = (m_currentFrameIndex + 1) % static_cast<int>(frames.size());
}
spriteFor(m_currentState).setTextureRect(frames[m_currentFrameIndex]);
}

//Draw ...
void CharacterRenderer::draw(sf::RenderWindow& window)
{
if (hasSpriteFor(m_currentState))
window.draw(spriteFor(m_currentState));
}

// Set position (origin at bottom-centre of frame)
void CharacterRenderer::setPosition(const sf::Vector2f& feetPos)
{
for (auto& [state, optSprite] : m_sprites)
{
if (!optSprite.has_value())
continue;
sf::Sprite& sprite = optSprite.value();
sf::FloatRect local = sprite.getLocalBounds();
sprite.setOrigin({ local.size.x / 2.f, local.size.y });
sprite.setPosition(feetPos);
applyFacingScale(sprite, m_baseScale, m_facingRight);
}
}

//Get position:-
sf::Vector2f CharacterRenderer::getPosition() const
{
if (hasSpriteFor(m_currentState))
return spriteFor(m_currentState).getPosition();
return sf::Vector2f(0.f, 0.f);
}

//Set scale.....
void CharacterRenderer::setScale(float scale)
{
m_baseScale = scale;
for (auto& [state, optSprite] : m_sprites)
{
if (optSprite.has_value())
applyFacingScale(optSprite.value(), m_baseScale, m_facingRight);
}
}

//Set facing direction.
void CharacterRenderer::setFacingRight(bool facingRight)
{
m_facingRight = facingRight;
for (auto& [state, optSprite] : m_sprites)
{
if (optSprite.has_value())
applyFacingScale(optSprite.value(), m_baseScale, m_facingRight);
}
}

bool CharacterRenderer::isFacingRight() const
{
return m_facingRight;
}

//Bounding boxes...
sf::FloatRect CharacterRenderer::getGlobalBounds() const
{
if (hasSpriteFor(m_currentState))
return spriteFor(m_currentState).getGlobalBounds();
return sf::FloatRect();
}

sf::FloatRect CharacterRenderer::getAttackBox() const
{
auto bounds = getGlobalBounds();
float atkWidth = bounds.size.x * 0.1f;
float atkHeight = bounds.size.y * 0.1f;
float atkX = m_facingRight? bounds.position.x + bounds.size.x: bounds.position.x - atkWidth;
float atkY = bounds.position.y + bounds.size.y * 0.1f;
return sf::FloatRect({ atkX, atkY }, { atkWidth, atkHeight });
}