// AudioManager.cpp
// FIX: PlayMusic now stops music if it is Playing OR Paused (was only Playing).
// This prevents openFromFile being called on a paused track without stopping it first.

#include "AudioManager.h"

using namespace std;

// ── Sound effect loading ──────────────────────────────────────────────────────

void AudioManager::preloadOptional(const string& key, const string& path)
{
    sf::SoundBuffer buf;
    if (buf.loadFromFile(path))
        buffers[key] = std::move(buf);   // move gives ownership directly to the map
}

// ── Sound effect playback ─────────────────────────────────────────────────────

void AudioManager::playOptional(const string& key, float volume)
{
    auto it = buffers.find(key);
    if (it == buffers.end()) return;   // key not found; silent fail

    // Rotate through 6 channels so multiple sounds can overlap.
    size_t idx = nextChannel++ % 6;
    channels[idx].reset();                   // release previous sound at this slot
    channels[idx].emplace(it->second);       // attach buffer (non-owning reference)
    channels[idx]->setVolume(volume);
    channels[idx]->play();
    chennelKey[idx] = key;
}

// ── Background music ──────────────────────────────────────────────────────────

void AudioManager::PlayMusic(const string& path, float volume)
{
    // FIX: Stop music if it is Playing or Paused, not just Playing.
    auto status = music.getStatus();
    if (status == sf::SoundSource::Status::Playing
        || status == sf::SoundSource::Status::Paused)
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

void AudioManager::Stop()
{
    music.stop();
}
void AudioManager::StopSound(string& key)
{
    for (int i = 0; i < 6; i++)
    {
        if (chennelKey[i] == key && channels[i]
            && channels[i]->getStatus() == sf::SoundSource::Status::Playing)
        {
            channels[i]->stop();
            chennelKey[i] = "";   // clear the slot
            return;
        }
    }
}