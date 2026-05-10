#include "AudioManager.h"

void AudioManager::preloadOptional(const string& key, const string& path) 
{
sf::SoundBuffer buf;//Temporary sound buffer
if (buf.loadFromFile(path))
buffers[key] = std::move(buf);//store buf in hashmap (paired values)..move used to give direct ownership...
}

void AudioManager::playOptional(const string& key, float volume)
{
auto it = buffers.find(key);//find the key->soundbuffer
if (it == buffers.end())//safe exit if buffer not found..
return;
size_t idx = nextChannel++ % 6;//circulation in the array ...never exceeds index 5...
channels[idx].reset();//clear the previous buffer at that index..
channels[idx].emplace(it->second);//add new buffer to channel ...it->second points to sound buffer at that index ...
channels[idx]->setVolume(volume);
channels[idx]->play();
}
void AudioManager::PlayMusic(const string& path, float volume)
{
if (music.getStatus() == sf::SoundSource::Status::Playing)
music.stop();
if (music.openFromFile(path))
{
music.setLooping(true);
music.setVolume(volume);
music.play();
}
}
void AudioManager::Stop()
{
music.stop();
}