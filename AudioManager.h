#pragma once
#include <SFML/Audio.hpp>
#include <optional>
#include <unordered_map>
using namespace std;

class AudioManager
{
unordered_map<string, sf::SoundBuffer> buffers;//sound buffer stores the sound raw data in memory...we need to keep it alive as long as we want to play the sound...
//unordered_map is just like dictionary stores key value pair...key is string and value is sound buffer...we can look up sound buffer by key...it does not keep order of insertion...
optional<sf::Sound> channels[6];
size_t nextChannel = 0;
//size_t is unsigned int data type used to store size of array here..
public:
sf::Music music;
AudioManager() = default;//default constructor..
void preloadOptional(const string& key, const string& path);//load buffer
void playOptional(const string& key, float volume = 65.f);//play sound on next channel...
void PlayMusic(const string& path, float volume = 50.f);//load music from file and set volume...returns true if successful...
void Stop();
};
