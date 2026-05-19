//-------------------------------------------- ( AudioManager.h ) ----------------------------------------------------
#pragma once
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <optional>

class AudioManager
{
private:
// Sound effect buffers keyed by name.
std::unordered_map<std::string, sf::SoundBuffer> buffers;
//10 rotating sound channels for simultaneous effects.
std::optional<sf::Sound> channels[10];
size_t nextChannel = 0;
std::string channelKey[10];
public:
sf::Music music;
void preloadOptional(const std::string& key, const std::string& path);
void playOptional(const std::string& key, float volume = 80.f);
void PlayMusic(const std::string& path, float volume = 80.f);
// Stop background music and sound effects.
void Stop();
void StopSound(std::string key);
};
