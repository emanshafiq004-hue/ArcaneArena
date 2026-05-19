#pragma once
//  Contains:
//    BuildingData struct
//    CityBackground  (scrolling neon city)
//    Button          (UI button helper)
//    state enum      (game state machine)
//    UIRenderer      (wraps all HUD + screen drawing)
// _________________________________________________________

#include "Globals.h"
#include "Managers.h"
#include <SFML/Audio.hpp>

//  BuildingData — plain data for one city building

struct BuildingData {
    float x = 0.f, y = 0.f, w = 0.f, h = 0.f;
    std::vector<sf::Vector2f> windows;
};

//  CLASS: CityBackground
//  Scrolling neon skyline with randomised buildings + grid
class CityBackground : public IDrawable, public IUpdatable {
    std::vector<BuildingData> buildings_;
    std::vector<sf::Vertex>   grid_;
    float scrollSpd_ = 60.f;

    void addBuilding(float startX);
    void buildGrid();

public:
    CityBackground();

    void setSpeed(float s);
    void update(float dt) override;
    void draw(sf::RenderTarget& rt) override;
};

//  Button — simple encapsulated UI button

struct Button {
    sf::FloatRect rect;
    std::string   label;

    Button();
    Button(float x, float y, float w, float h, std::string l);
};


//  State — game state machine

enum class State {
    Menu,
    NameEntry,    // appears after clicking Play
    Countdown,
    Playing,
    Paused,       // new
    Dead,
    HighScore,
    Settings
};


// CLASS: UIRenderer
//  Encapsulates all HUD and screen rendering logic
//  Takes sf::RenderWindow by reference (aggregation)

class UIRenderer {
    sf::RenderWindow& window_;
    sf::Font& font_;
    bool              fontOk_;

public:
    UIRenderer(sf::RenderWindow& win, sf::Font& font, bool fontOk);

    

    void drawPanel(float x, float y, float w, float h,
        sf::Color fill, sf::Color outline);

    void drawButton(const Button& btn, bool hover);

    void drawStars(const std::vector<struct Star>& stars, float totalTime);

    void drawHUD(const ScoreManager& sm, float totalTime);

    void drawMenu(const ScoreManager& sm, float totalTime, float totalT,
        Button& btnPlay, Button& btnExit,
        sf::Vector2f mousePos);

    void drawCountdown(float countdownTimer);

    void drawDead(const ScoreManager& sm, float totalTime,
        Button& btnRetry, Button& btnMenu2,
        sf::Vector2f mousePos);

    void drawNameEntry(const ScoreManager& sm, const std::string& playerName,
        float totalTime, Button& btnConfirm,
        sf::Vector2f mousePos);

    void drawHighScores(FileManager_SkySurge& fm, Button& btnBackHS,
        sf::Vector2f mousePos);

    void drawSettings(FileManager_SkySurge::Settings& settings,
        Button& btnSnd, Button& btnBackSet,
        sf::Vector2f mousePos);

    void drawPauseButton(bool paused, sf::Vector2f mousePos);

    // Made public so main.cpp can call it for the pause overlay text
    void drawText(const std::string& str, float x, float y,
        unsigned sz, sf::Color col, bool center = false);

    
};

//_______Star struct (used by UIRenderer::drawStars)_______________
struct Star {
    Vec2  pos;
    float speed = 30.f;
    float brightness = 180.f;
    float size = 1.f;
};