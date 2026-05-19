#include "CharacterRenderer.h"

CharacterRenderer::CharacterRenderer():m_sprite(m_texture) {}

bool CharacterRenderer::loadFromSheet(const std::string& sheetPath,
    sf::Vector2i frameSize,
    const std::unordered_map<CharacterState, std::vector<int>>& animations,
    float frameDuration)
{
    if (!m_texture.loadFromFile(sheetPath))
        return false;
    m_sprite = sf::Sprite(m_texture);
    m_frameSize = frameSize;
    m_frameDuration = frameDuration;

    // Convert the index lists to sf::IntRect vectors.
    int columns = m_texture.getSize().x / frameSize.x;
    for (const auto& [state, indices] : animations)
    {
        std::vector<sf::IntRect> rects;
        for (int idx : indices)
        {
            int col = idx % columns;
            int row = idx / columns;
            rects.push_back(sf::IntRect({ col * frameSize.x, row * frameSize.y },
                { frameSize.x, frameSize.y }));
        }
        m_frames[state] = rects;
    }

    // Set initial texture rect.
    if (m_frames.count(CharacterState::Idle) && !m_frames[CharacterState::Idle].empty())
        m_sprite.setTextureRect(m_frames[CharacterState::Idle][0]);
    return true;
}

void CharacterRenderer::setState(CharacterState state)
{
    if (m_currentState == state)
        return;
    m_currentState = state;
    m_currentFrameIndex = 0;
    m_timeAccum = 0.f;
    if (m_frames.count(state) && !m_frames[state].empty())
        m_sprite.setTextureRect(m_frames[state][0]);
}

void CharacterRenderer::update(float dt)
{
    auto it = m_frames.find(m_currentState);
    if (it == m_frames.end() || it->second.empty())
        return;

    const auto& frames = it->second;
    m_timeAccum += dt;
    while (m_timeAccum >= m_frameDuration && !frames.empty())
    {
        m_timeAccum -= m_frameDuration;
        m_currentFrameIndex = (m_currentFrameIndex + 1) % frames.size();
    }
    m_sprite.setTextureRect(frames[m_currentFrameIndex]);
}

void CharacterRenderer::draw(sf::RenderWindow& window)
{
    window.draw(m_sprite);
}

void CharacterRenderer::setPosition(const sf::Vector2f& feetPos)
{
    // m_sprite origin is at the bottom centre of the frame.
    sf::FloatRect local = m_sprite.getLocalBounds();
    m_sprite.setOrigin({ local.size.x / 2.f, local.size.y });
    m_sprite.setPosition(feetPos);

    // Flip horizontally according to facing direction.
    if (m_facingRight)
        m_sprite.setScale({ 1.f, 1.f });
    else
        m_sprite.setScale({ -1.f, 1.f });
}

sf::Vector2f CharacterRenderer::getPosition() const
{
    return m_sprite.getPosition();
}

void CharacterRenderer::setScale(float scale)
{
    sf::Vector2f s = m_sprite.getScale();
    bool isFlipped = (s.x < 0.f);
    m_sprite.setScale({ isFlipped ? -scale : scale, scale });
}

void CharacterRenderer::setFacingRight(bool facingRight)
{
    if (m_facingRight == facingRight)
        return;
    m_facingRight = facingRight;
    setPosition(m_sprite.getPosition()); // reapply flipping through setPosition
}

sf::FloatRect CharacterRenderer::getGlobalBounds() const
{
    return m_sprite.getGlobalBounds();
}

sf::FloatRect CharacterRenderer::getAttackBox() const
{
    // Approximate attack box in front of the character.
    auto bounds = getGlobalBounds();
    float atkWidth = bounds.size.x * 0.6f;
    float atkHeight = bounds.size.y * 0.4f;
    float atkX = m_facingRight ? bounds.position.x + bounds.size.x : bounds.position.x - atkWidth;
    float atkY = bounds.position.y + bounds.size.y * 0.3f;
    return sf::FloatRect({ atkX, atkY }, { atkWidth, atkHeight });
}