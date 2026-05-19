//---------------------------------------- ( AudioManager.cpp ) --------------------------------------------------------
#include "AudioManager.h"
using namespace std;

//Sound effect loading:-
void AudioManager::preloadOptional(const string& key, const string& path)
{
sf::SoundBuffer buf;
if (buf.loadFromFile(path))
buffers[key] = std::move(buf);// move gives ownership directly to the map...
}

//Sound effect playback:-
void AudioManager::playOptional(const string& key, float volume)
{
auto it = buffers.find(key);
if (it == buffers.end()) 
return; // key not found... silent fail

// Rotate through 10 channels so multiple sounds can overlap.
size_t idx = nextChannel++ % 10;

channels[idx].reset();// release previous sound at this slot..

channels[idx].emplace(it->second);
channels[idx]->setVolume(volume);
channels[idx]->play();
channelKey[idx] = key;
}

//Background music:-
void AudioManager::PlayMusic(const string& path, float volume)
{
auto status = music.getStatus();
if (status == sf::SoundSource::Status::Playing|| status == sf::SoundSource::Status::Paused)
{
music.stop();
}
if (music.openFromFile(path))
{
music.setLooping(true);
music.setVolume(volume);
music.play();
}
}

//Stop Music:-
void AudioManager::Stop()
{
music.stop();
}

//Stop Sound Effect:-
void AudioManager::StopSound(string& key)
{
for (int i = 0; i < 10; i++)
{
if (channelKey[i] == key && channels[i]&& channels[i]->getStatus() == sf::SoundSource::Status::Playing)
{
channels[i]->stop();
channelKey[i] = "";
return;
}
}
}