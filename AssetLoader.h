#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
//utility class for loading assets...
class AssetLoader 
{
public:
//static because we call by class...no internal state
//stateless utility class....
//each load assets with fall back...
static bool openFontWithFallback(sf::Font& font, const std::vector<std::string>& paths);
static bool loadTextureWithFallback(sf::Texture& texture, const std::vector<std::string>& paths);
static bool loadImageWithFallback(sf::Image& image, const std::vector<std::string>& paths);
static bool emplaceCoverSprite(sf::Texture& texture, std::optional<sf::Sprite>& outSprite,
const std::vector<std::string>& paths, float viewWidth, float viewHeight);
};

