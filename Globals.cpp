#include "Globals.h"

// ______________Global RNG (used by randRange<T>)____________
std::mt19937 gRng(static_cast<unsigned>(std::time(nullptr)));

//_______________Static member definitions____________________
int GameStats::totalBarriersPassed = 0;
int GameStats::totalCrystalsCollected = 0;
int GameStats::totalDeaths = 0;

void GameStats::reset() {
    totalBarriersPassed = totalCrystalsCollected = totalDeaths = 0;
}

std::string GameStats::summary() {
    return "Barriers:" + std::to_string(totalBarriersPassed) +
        "  Crystals:" + std::to_string(totalCrystalsCollected) +
        "  Deaths:" + std::to_string(totalDeaths);
}

//__________________Letterbox view helper_________________
sf::View getLetterboxView(float winW, float winH) {
    float sc = std::min(winW / BASE_W, winH / BASE_H);
    float vx = (winW - BASE_W * sc) / 2.f / winW;
    float vy = (winH - BASE_H * sc) / 2.f / winH;
    sf::View v;
    v.setSize(sf::Vector2f(BASE_W, BASE_H));
    v.setCenter(sf::Vector2f(BASE_W / 2.f, BASE_H / 2.f));
    v.setViewport(makeRect(vx, vy, BASE_W * sc / winW, BASE_H * sc / winH));
    return v;
}
// Window is 1200x700
// winW/BASE_W = 1200/800 = 1.5
// winH/BASE_H = 700/600  = 1.166
// sc = min(1.5, 1.166) = 1.166  ← smaller value chosen