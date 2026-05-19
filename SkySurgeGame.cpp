#include "Renderer.h"
#include <stdexcept>
#include <SFML/Audio.hpp>

int main() {
    //___________________Window------------------------
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(800u, 600u)),
        "Sky Surge", sf::Style::Default);
    window.setFramerateLimit(60);

    //_________________Icon_____________________
    sf::Image icon;
    if (icon.loadFromFile("Assets/icon.jpeg"))
        window.setIcon(icon);

    //________________Font_______________________
    sf::Font font;
    bool fontOk = false;
    try {
        if (!font.openFromFile("Font/Cinzel-Bold.ttf"))
            throw std::runtime_error("Font not found");
        fontOk = true;
    }
    catch (const std::exception&) { fontOk = false; }

    //_________________Drone Texture____________________________
    sf::Texture droneTex;
    try {
        if (!droneTex.loadFromFile("Assets/Bird.png"))
            throw std::runtime_error("Bird.png not found");
    }
    catch (...) {
        sf::Image img(sf::Vector2u(40u, 24u), sf::Color(0, 220, 255, 255));
        (void)droneTex.loadFromImage(img);
    }

    //_________________Core Systems_________________________
    ParticleSystem  ps(400);
    Drone           drone(droneTex, ps);
    CityBackground  city;
    GameWorld       world;
    ScoreManager    sm;
    FileManager     fm;
    UIRenderer      ui(window, font, fontOk);

    GameStats::reset();
    sm.setBest(fm.getBestScoreFromFile());

    //______________________Audio_________________________
    sf::SoundBuffer coinBuffer, gameoverBuffer;


    bool coinOk = coinBuffer.loadFromFile("Assets/GetCoin.wav");
    bool gameoverOk = gameoverBuffer.loadFromFile("Assets/GameOver.wav");
    sf::Sound       coinSound(coinBuffer), gameoverSound(gameoverBuffer);
    sf::Music       bgMusic;
    bool bgOk = bgMusic.openFromFile("Assets/Background.mp3");



    if (bgOk) {
        bgMusic.setLooping(true);
        bgMusic.setVolume(40.f);   // background lower so gameplay sounds clear
        bgMusic.play();
    }

    coinSound.setVolume(90.f);
    gameoverSound.setVolume(90.f);

    //______________________Starfield___________________________
    std::vector<Star> stars(120);
    for (auto& s : stars) {
        s.pos = Vec2(randRange(0.f, BASE_W), randRange(0.f, BASE_H));
        s.speed = randRange(20.f, 80.f);
        s.brightness = randRange(100.f, 255.f);
        s.size = randRange(0.5f, 2.f);
    }

    //_________________Buttons________________________
    Button btnPlay(300.f, 240.f, 200.f, 46.f, "PLAY");
    Button btnExit(300.f, 414.f, 200.f, 46.f, "EXIT");
    Button btnRetry(210.f, 380.f, 170.f, 46.f, "RETRY");
    Button btnMenu2(420.f, 380.f, 170.f, 46.f, "MENU");
    Button btnConfirm(310.f, 340.f, 180.f, 46.f, "START GAME");

    // Pause button area (top-right corner)
    sf::FloatRect pauseRect = makeRect(BASE_W - 50.f, 6.f, 38.f, 32.f);

    //___________________Game State__________________________
    State       state = State::Menu;
    std::string playerName = "";
    float       countdownTimer = 3.f;
    float       totalTime = 0.f;
    bool        paused = false;

    auto resetGame = [&]() {
        world.clear();
        drone.reset();
        sm.reset(fm.getBestScoreFromFile());
        city.setSpeed(180.f);
        countdownTimer = 3.f;
        paused = false;
        GameStats::reset();

        // Restart background music on retry
        if (bgOk) {
            bgMusic.stop();
            bgMusic.play();
        }
        };

    sf::Clock clock;

    //  MAIN LOOP

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;
        totalTime += dt;

        sf::View gameView = getLetterboxView(
            (float)window.getSize().x,
            (float)window.getSize().y);
        window.setView(gameView);
        sf::Vector2f mp = window.mapPixelToCoords(
            sf::Mouse::getPosition(window), gameView);

        //________________________EVENTS_______________________
        while (auto ev = window.pollEvent()) {

            if (ev->is<sf::Event::Closed>()) {
                window.close();
                break;
            }

            if (auto* res = ev->getIf<sf::Event::Resized>()) {
                window.setView(getLetterboxView(
                    (float)res->size.x, (float)res->size.y));
            }

            if (auto* kr = ev->getIf<sf::Event::KeyPressed>()) {
                // Thrust
                if (state == State::Playing && !paused &&
                    kr->code == sf::Keyboard::Key::Space)
                    drone.thrust();

                // Pause with Escape key
                if (state == State::Playing &&
                    kr->code == sf::Keyboard::Key::Escape)
                    paused = !paused;

                if (bgOk)
                {
                    if (paused)
                        bgMusic.pause();
                    else
                        bgMusic.play();
                }

                // Name entry keys
                if (state == State::NameEntry) {
                    if (kr->code == sf::Keyboard::Key::Backspace &&
                        !playerName.empty())
                        playerName.pop_back();
                    if (kr->code == sf::Keyboard::Key::Enter &&
                        !playerName.empty()) {
                        resetGame();
                        state = State::Countdown;
                    }
                }
            }

            if (auto* tc = ev->getIf<sf::Event::TextEntered>()) {
                if (state == State::NameEntry &&
                    playerName.size() < 12)
                {
                    char c = static_cast<char>(tc->unicode);
                    if (std::isalpha((unsigned char)c))  // only alphabets allowed
                        playerName += c;
                }
            }

            if (auto* mb = ev->getIf<sf::Event::MouseButtonPressed>()) {
                sf::Vector2f p = window.mapPixelToCoords(mb->position, gameView);

                // Thrust on click
                if (state == State::Playing && !paused)
                    drone.thrust();

                // Pause button click (top-right)
                if (state == State::Playing && pauseRect.contains(p)) {
                    paused = !paused;
                    if (bgOk) {
                        if (paused) bgMusic.pause();
                        else        bgMusic.play();
                    }
                }

                // Name entry confirm
                if (state == State::NameEntry &&
                    btnConfirm.rect.contains(p) &&
                    !playerName.empty()) {
                    resetGame();
                    state = State::Countdown;
                }

                // Main menu
                if (state == State::Menu) {
                    if (btnPlay.rect.contains(p))
                        state = State::NameEntry;   // ask name first
                    if (btnExit.rect.contains(p)) window.close();
                }

                // Dead screen
                if (state == State::Dead) {
                    if (btnRetry.rect.contains(p)) {
                        resetGame();
                        state = State::Countdown;
                    }
                    if (btnMenu2.rect.contains(p))
                        state = State::Menu;
                }

            }

        } // end pollEvent

        //____________________UPDATE____________________________
        for (auto& s : stars) {
            s.pos.x -= s.speed * dt;
            if (s.pos.x < 0.f) {
                s.pos.x = BASE_W;
                s.pos.y = randRange(0.f, BASE_H);
            }
        }

        if (state == State::Countdown) {
            city.update(dt);
            countdownTimer -= dt;
            if (countdownTimer <= 0.f)
                state = State::Playing;
        }

        // Only update game logic when not paused
        if (state == State::Playing && !paused) {
            float spd = sm.scrollSpeed();
            city.setSpeed(spd);
            city.update(dt);
            sm.update(dt);

            world.update(dt, spd, sm);
            drone.update(dt);
            ps.update(dt);

            world.checkCollisions(drone, sm, ps);              
            if (world.coinCollected && coinOk)                
                coinSound.play();

            if (drone.isDead()) {
                if (sm.getScore() > sm.getBest())
                    sm.setBest(sm.getScore());
                fm.addScore(playerName, sm.getScore());

                // Stop background music and play game over sound
                if (bgOk)       bgMusic.stop();
                if (gameoverOk) gameoverSound.play();

                state = State::Dead;
            }
        }

        //_________________________DRAW______________________________
        window.clear(COL_BG_TOP);
       

        ui.drawStars(stars, totalTime);
        city.draw(window);

        if (state == State::Playing ||
            state == State::Countdown ||
            state == State::Dead)
        {
            world.draw(window);
            ps.draw(window);
            drone.draw(window);
            ui.drawHUD(sm, totalTime);

            // Pause button always visible during gameplay
            if (state == State::Playing)
                ui.drawPauseButton(paused, mp);

            // Paused overlay
            if (paused) {
                sf::RectangleShape overlay(sf::Vector2f(BASE_W, BASE_H));
                overlay.setFillColor(sf::Color(0, 0, 0, 120));
                window.draw(overlay);
                ui.drawText("PAUSED", BASE_W / 2.f, BASE_H / 2.f - 20.f,
                    42, COL_HL, true);
                ui.drawText("Click || or press Esc to resume",
                    BASE_W / 2.f, BASE_H / 2.f + 30.f,
                    16, sf::Color(160, 220, 255, 200), true);
            }
        }

        switch (state) {
        case State::Menu:
            ui.drawMenu(sm, totalTime, totalTime,
                btnPlay, btnExit, mp);
            break;
        case State::NameEntry:
            ui.drawNameEntry(sm, playerName, totalTime, btnConfirm, mp);
            break;
        case State::Countdown:
            ui.drawCountdown(countdownTimer);
            break;
        case State::Dead:
            ui.drawDead(sm, totalTime, btnRetry, btnMenu2, mp);
            break;
        default: break;
        }

        window.display();

    } // end main loop

    return 0;
}