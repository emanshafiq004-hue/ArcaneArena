//#pragma once
//#include <SFML/Audio.hpp>
//#include <optional>
//#include <unordered_map>
//using namespace std;
//
//class AudioManager
//{
//unordered_map<string, sf::SoundBuffer> buffers;//sound buffer stores the sound raw data in memory...we need to keep it alive as long as we want to play the sound...
////unordered_map is just like dictionary stores key value pair...key is string and value is sound buffer...we can look up sound buffer by key...it does not keep order of insertion...
//optional<sf::Sound> channels[6];
//size_t nextChannel = 0;
////size_t is unsigned int data type used to store size of array here..
//public:
//sf::Music music;
//AudioManager() = default;//default constructor..
//void preloadOptional(const string& key, const string& path);//load buffer
//void playOptional(const string& key, float volume = 65.f);//play sound on next channel...
//void PlayMusic(const string& path, float volume = 50.f);//load music from file and set volume...returns true if successful...
//void Stop();
//};

//----------------------------------------------------------------------------------
// AudioManager.h
// Manages background music (sf::Music) and short sound effects (sf::Sound).
// preloadOptional loads a sound buffer by key.
// playOptional plays a loaded sound on a rotating channel (6 channels).
// PlayMusic starts looping background music (stops previous track first).
// Stop halts background music.

#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <unordered_map>
#include <optional>

class AudioManager
{
private:
    // Sound effect buffers keyed by name.
    std::unordered_map<std::string, sf::SoundBuffer> buffers;

    // 6 rotating sound channels for simultaneous effects.
    std::optional<sf::Sound> channels[6];
    size_t nextChannel = 0;
    std::string chennelKey[6];

public:
    // Background music stream (public so callers can check status if needed).
    sf::Music music;

    // Load a sound effect buffer from file. Silent on failure.
    void preloadOptional(const std::string& key, const std::string& path);

    // Play a previously loaded sound effect. Silent if key not found.
    void playOptional(const std::string& key, float volume = 80.f);

    // Start looping background music from file. Stops current track first.
    // FIX: also stops if status is Paused (not just Playing).
    void PlayMusic(const std::string& path, float volume = 80.f);

    // Stop background music.
    void Stop();
    void StopSound(std::string& key);
};
