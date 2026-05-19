//--------------------------------------- ( AsetLoader.cpp ) ----------------------------------------------
#include "AssetLoader.h"
#include <algorithm>
using namespace std;

//Font Load:-
bool AssetLoader::openFontWithFallback(sf::Font& font, const vector<string>& paths) 
{
//auto used to tell compiler to decide the return type at compile time...
for (const auto& path : paths) 
{
if (font.openFromFile(path))
return true;
}
return false;
}

//Texture Load:-
bool AssetLoader::loadTextureWithFallback(sf::Texture& texture, const vector<string>& paths) 
{
for (const auto& path : paths) 
{
if (texture.loadFromFile(path))
return true;
}
return false;
}

//Image Load:-
bool AssetLoader::loadImageWithFallback(sf::Image& image, const vector<string>& paths) 
{
for (const auto& path : paths) 
{
if (image.loadFromFile(path))
return true;
}
return false;
}

//Cover Sprite Load:-
bool AssetLoader::emplaceCoverSprite(sf::Texture& texture, optional<sf::Sprite>& outSprite, const vector<string>& paths,float viewWidth, float viewHeight) 
{
outSprite.reset();
//clear the previous sprite if it exists...
if (!loadTextureWithFallback(texture, paths))
//checks if we can load the texture...if not,
return false;
texture.setSmooth(true);
//removing pixalization ...makes it look better when we scale up the image...
sf::Sprite sprite(texture);
//making sprite....from texture...
const auto ts = texture.getSize();
//geting the size of the texture...to calculate the scale factor...
const float sx = viewWidth / static_cast<float>(ts.x);
const float sy = viewHeight / static_cast<float>(ts.y);
//max of the two scales...to make sure we cover the entire view...even if it means cropping some of the image...
const float sc = max(sx,sy);
//seting the origin to the center of the sprite...so we can position it in the center of the view...
sprite.setOrigin({ static_cast<float>(ts.x) * 0.5f, static_cast<float>(ts.y) * 0.5f });
//scaling the sprite to cover the entire view.
sprite.setScale({ sc, sc });
//positioning the sprite in the center of the view...so it covers the entire view...
sprite.setPosition({ viewWidth * 0.5f, viewHeight * 0.5f });
//emplacing the sprite into the optional.move is used to avoid copying the sprite...we want to move it into the optional...since we don't need it anymore after this function...
outSprite.emplace(std::move(sprite));
return true;
}

