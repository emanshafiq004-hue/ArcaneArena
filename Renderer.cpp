#include "Renderer.h"
#include <cmath>
#include <algorithm>

//  CityBackground

void CityBackground::addBuilding(float sx) {
    BuildingData b;
    b.w = randRange(35.f, 90.f);
    b.h = randRange(80.f, 350.f);
    b.x = sx; b.y = BASE_H - b.h;
    for (float cy = b.y + 10.f; cy < BASE_H - 5.f; cy += 12.f)
        for (float cx = b.x + 5.f; cx < b.x + b.w - 4.f; cx += 10.f)
            b.windows.push_back(sf::Vector2f(cx, cy));
    buildings_.push_back(std::move(b));
}

void CityBackground::buildGrid() {
    grid_.clear();
    for (float x = 0.f; x < BASE_W; x += 40.f) {
        grid_.push_back(sf::Vertex(sf::Vector2f(x, 0.f), COL_GRID));
        grid_.push_back(sf::Vertex(sf::Vector2f(x, BASE_H), COL_GRID));
    }
    for (float y = 0.f; y < BASE_H; y += 40.f) {
        grid_.push_back(sf::Vertex(sf::Vector2f(0.f, y), COL_GRID));
        grid_.push_back(sf::Vertex(sf::Vector2f(BASE_W, y), COL_GRID));
    }
}

CityBackground::CityBackground() {
    float x = 0.f;
    while (x < BASE_W + 200.f) {
        addBuilding(x);
        x += buildings_.back().w + randRange(2.f, 10.f);
    }
    buildGrid();
}

void CityBackground::setSpeed(float s) { scrollSpd_ = s * 0.25f; }

void CityBackground::update(float dt) {
    for (auto& b : buildings_) {
        b.x -= scrollSpd_ * dt;
        for (auto& w : b.windows) w.x -= scrollSpd_ * dt;
    }
    while (!buildings_.empty() && buildings_.front().x + buildings_.front().w < -10.f) {
        float nx = buildings_.back().x + buildings_.back().w + randRange(2.f, 10.f);
        buildings_.erase(buildings_.begin());
        addBuilding(nx);
    }
}

void CityBackground::draw(sf::RenderTarget& rt) {
    // Gradient background
    sf::VertexArray bg(sf::PrimitiveType::Triangles, 6);
    bg[0] = sf::Vertex(sf::Vector2f(0.f, 0.f), COL_BG_TOP);
    bg[1] = sf::Vertex(sf::Vector2f(BASE_W, 0.f), COL_BG_TOP);
    bg[2] = sf::Vertex(sf::Vector2f(BASE_W, BASE_H), COL_BG_BOT);
    bg[3] = sf::Vertex(sf::Vector2f(0.f, 0.f), COL_BG_TOP);
    bg[4] = sf::Vertex(sf::Vector2f(BASE_W, BASE_H), COL_BG_BOT);
    bg[5] = sf::Vertex(sf::Vector2f(0.f, BASE_H), COL_BG_BOT);
    rt.draw(bg);

    // Grid overlay
    rt.draw(grid_.data(), grid_.size(), sf::PrimitiveType::Lines);

    // Buildings + windows
    sf::RectangleShape rs, ws(sf::Vector2f(6.f, 4.f));
    for (auto& bd : buildings_) {
        rs.setSize(sf::Vector2f(bd.w, bd.h));
        rs.setPosition(sf::Vector2f(bd.x, bd.y));
        rs.setFillColor(COL_BLDG);
        rs.setOutlineColor(sf::Color(0, 80, 180, 80));
        rs.setOutlineThickness(1.f);
        rt.draw(rs);
        for (auto& wpos : bd.windows) {
            ws.setFillColor(randRange(0, 20) == 0 ? COL_WINLIT : sf::Color(20, 50, 120, 120));
            ws.setPosition(wpos);
            rt.draw(ws);
        }
    }
}


//  Button

Button::Button() : rect(makeRect(0, 0, 0, 0)) {}
Button::Button(float x, float y, float w, float h, std::string l)
    : rect(makeRect(x, y, w, h)), label(std::move(l)) {
}

//  UIRenderer

UIRenderer::UIRenderer(sf::RenderWindow& win, sf::Font& font, bool fontOk)
    : window_(win), font_(font), fontOk_(fontOk) {
}

void UIRenderer::drawText(const std::string& str, float x, float y,
    unsigned sz, sf::Color col, bool center)
{
    if (!fontOk_) return;
    sf::Text t(font_, str, sz);
    t.setFillColor(col);
    if (center) {
        auto b = t.getLocalBounds();
        t.setOrigin(sf::Vector2f(b.size.x / 2.f, b.size.y / 2.f));
        t.setPosition(sf::Vector2f(x, y));
    }
    else {
        t.setPosition(sf::Vector2f(x, y));
    }
    window_.draw(t);
}

void UIRenderer::drawPanel(float x, float y, float w, float h,
    sf::Color fill, sf::Color outline)
{
    sf::RectangleShape rs(sf::Vector2f(w, h));
    rs.setPosition(sf::Vector2f(x, y));
    rs.setFillColor(fill);
    rs.setOutlineColor(outline);
    rs.setOutlineThickness(2.f);
    window_.draw(rs);
}

void UIRenderer::drawButton(const Button& btn, bool hover) {
    sf::Color fill = hover ? sf::Color(0, 160, 220, 220) : sf::Color(10, 30, 80, 200);
    sf::Color border = hover ? sf::Color(0, 230, 255, 255) : sf::Color(0, 130, 200, 200);
    drawPanel(btn.rect.position.x, btn.rect.position.y,
        btn.rect.size.x, btn.rect.size.y, fill, border);
    drawText(btn.label,
        btn.rect.position.x + btn.rect.size.x / 2.f,
        btn.rect.position.y + btn.rect.size.y / 2.f - 8.f,
        20, COL_TEXT, true);
}

void UIRenderer::drawStars(const std::vector<Star>& stars, float totalTime) {
    sf::CircleShape starS;
    for (auto& s : stars) {
        float br = s.brightness * (0.7f + 0.3f * std::sin(totalTime * 1.5f + s.pos.x));
        starS.setRadius(s.size);
        starS.setOrigin(sf::Vector2f(s.size, s.size));
        starS.setFillColor(sf::Color(200, 220, 255,
            static_cast<uint8_t>(std::min(br, 255.f))));
        starS.setPosition(sf::Vector2f(s.pos));
        window_.draw(starS);
    }
}

void UIRenderer::drawHUD(const ScoreManager& sm, float totalTime) {
    drawPanel(0.f, 0.f, BASE_W, 44.f,
        sf::Color(0, 0, 20, 180), sf::Color(0, 160, 255, 100));

    drawText("SCORE: " + std::to_string(sm.getScore()),
        12.f, 10.f, 22, COL_HL);
    drawText("BEST: " + std::to_string(sm.getBest()),
        200.f, 10.f, 18, sf::Color(160, 220, 255, 200));

    // Coin counter with gem icon colour
    drawText("COINS: " + std::to_string(sm.getCoins()),
        370.f, 10.f, 18, COL_CRYSTAL);

    if (sm.hasMulti()) {
        float fl = (std::sin(totalTime * 8.f) > 0.f) ? 1.f : 0.7f;
        drawText("x2! " + std::to_string((int)sm.getMultTimer() + 1) + "s",
            540.f, 10.f, 18,
            sf::Color(static_cast<uint8_t>(255.f * fl), 100,
                static_cast<uint8_t>(255.f * fl), 255));
    }
    if (sm.getStormSlow() > 0.f)
        drawText("STORM!", 540.f, 26.f, 13, sf::Color(200, 150, 255, 220));

    // Shield indicator
    if (sm.hasShield())
        drawText("SHIELD ACTIVE", 370.f, 26.f, 13, COL_SHIELD);

    const char* tn[] = { "SLOW","MEDIUM","FAST","ULTRA" };
    drawText(tn[sm.tier()], BASE_W - 76.f, 28.f, 12, sf::Color(100, 180, 255, 160));
}

void UIRenderer::drawMenu(const ScoreManager& sm, float totalTime, float,
    Button& btnPlay, Button& btnHigh,
    Button& btnSet, Button& btnExit,
    sf::Vector2f mp)
{
    float ly = 80.f + 8.f * std::sin(totalTime * 1.2f);
    sf::RectangleShape glow(sf::Vector2f(420.f, 80.f));
    glow.setOrigin(sf::Vector2f(210.f, 40.f));
    glow.setPosition(sf::Vector2f(BASE_W / 2.f, ly));
    glow.setFillColor(sf::Color(0, 180, 255, 22));
    window_.draw(glow);

    drawText("SKY  SURGE", BASE_W / 2.f, ly - 22.f, 52, COL_HL, true);
    drawText("The ultimate sky survival adventure.", BASE_W / 2.f, ly + 38.f, 16,
        sf::Color(160, 200, 255, 200), true);

    drawButton(btnPlay, btnPlay.rect.contains(mp));
    drawButton(btnHigh, btnHigh.rect.contains(mp));
    drawButton(btnSet, btnSet.rect.contains(mp));
    drawButton(btnExit, btnExit.rect.contains(mp));

    drawText("BEST: " + std::to_string(sm.getBest()),
        BASE_W / 2.f, 476.f, 18, COL_HL, true);
    drawText(GameStats::summary(),
        BASE_W / 2.f, 500.f, 12, sf::Color(120, 160, 200, 160), true);
}

void UIRenderer::drawCountdown(float countdownTimer) {
    int cd = (int)std::ceil(countdownTimer);
    std::string cds = (cd > 0) ? std::to_string(cd) : "GO!";
    unsigned fs = static_cast<unsigned>(
        std::clamp(60.f * (2.5f - (countdownTimer - std::floor(countdownTimer))), 40.f, 120.f));
    drawText(cds, BASE_W / 2.f, BASE_H / 2.f - 30.f, fs, COL_HL, true);
}

void UIRenderer::drawDead(const ScoreManager& sm, float,
    Button& btnRetry, Button& btnMenu2,
    sf::Vector2f mp)
{
    drawPanel(150.f, 160.f, 500.f, 270.f, sf::Color(4, 8, 40, 220), COL_DEAD);
    drawText("GAME OVER", BASE_W / 2.f, 185.f, 36, COL_DEAD, true);
    drawText("SCORE: " + std::to_string(sm.getScore()),
        BASE_W / 2.f, 242.f, 24, COL_TEXT, true);
    drawText("COINS: " + std::to_string(sm.getCoins()),
        BASE_W / 2.f, 276.f, 18, COL_CRYSTAL, true);
    int diff = sm.getScore() - sm.getBest();
    std::string ds = (diff >= 0) ? "NEW BEST!" :
        ("Best: " + std::to_string(sm.getBest()) +
            "  (" + std::to_string(diff) + ")");
    drawText(ds, BASE_W / 2.f, 308.f, 18,
        diff >= 0 ? COL_HL : sf::Color(200, 200, 200, 200), true);
    drawText(GameStats::summary(),
        BASE_W / 2.f, 336.f, 13, sf::Color(120, 160, 200, 180), true);
    drawButton(btnRetry, btnRetry.rect.contains(mp));
    drawButton(btnMenu2, btnMenu2.rect.contains(mp));
}

void UIRenderer::drawNameEntry(const ScoreManager& sm, const std::string& playerName,
    float totalTime, Button& btnConfirm,
    sf::Vector2f mp)
{
    drawPanel(160.f, 150.f, 480.f, 280.f, sf::Color(4, 10, 50, 230), COL_HL);
    drawText("ENTER YOUR NAME", BASE_W / 2.f, 175.f, 28, COL_HL, true);
    drawText("Your name will appear on the leaderboard!",
        BASE_W / 2.f, 218.f, 14, sf::Color(160, 200, 255, 180), true);

    // Input box
    drawPanel(220.f, 248.f, 360.f, 44.f, sf::Color(0, 20, 60, 220), COL_HL);
    std::string cur = playerName + ((int(totalTime * 2) % 2 == 0) ? "_" : " ");
    drawText(cur, BASE_W / 2.f, 262.f, 22, COL_TEXT, true);

    drawButton(btnConfirm, btnConfirm.rect.contains(mp));
    drawText("(max 12 chars — Enter or click Start)",
        BASE_W / 2.f, 448.f, 12, sf::Color(120, 160, 200, 160), true);
}

void UIRenderer::drawHighScores(FileManager& fm, Button& btnBackHS, sf::Vector2f mp) {
    drawPanel(120.f, 80.f, 560.f, 400.f, sf::Color(4, 8, 40, 230), COL_HL);
    drawText("HIGH SCORES", BASE_W / 2.f, 108.f, 30, COL_HL, true);

    auto scores = fm.loadScores();
    const char* ranks[] = { "1.","2.","3.","4.","5." };
    sf::Color rc[] = { sf::Color(255,220,0,255), sf::Color(200,210,220,255),
                    sf::Color(200,130,60,255), COL_TEXT, COL_TEXT };
    for (int i = 0; i < (int)scores.size(); i++) {
        std::string row = std::string(ranks[i]) + "  " + scores[i].name +
            "   " + std::to_string(scores[i].score) +
            "   " + scores[i].date;
        drawText(row, BASE_W / 2.f, 162.f + i * 52.f, 20, rc[i], true);
    }
    if (scores.empty())
        drawText("No scores yet!", BASE_W / 2.f, 290.f, 18, sf::Color(160, 200, 255, 180), true);
    drawButton(btnBackHS, btnBackHS.rect.contains(mp));
}

void UIRenderer::drawSettings(FileManager::Settings& settings,
    Button& btnSnd, Button& btnBackSet,
    sf::Vector2f mp)
{
    drawPanel(180.f, 120.f, 440.f, 280.f, sf::Color(4, 8, 40, 230), COL_HL);
    drawText("SETTINGS", BASE_W / 2.f, 150.f, 30, COL_HL, true);
    btnSnd.label = settings.soundOn ? "SOUND: ON" : "SOUND: OFF";
    drawButton(btnSnd, btnSnd.rect.contains(mp));
    drawText(settings.soundOn ? "Music and effects are ON" : "Music and effects are OFF",
        BASE_W / 2.f, 342.f, 13, sf::Color(120, 160, 200, 200), true);
    drawButton(btnBackSet, btnBackSet.rect.contains(mp));
}
void UIRenderer::drawPauseButton(bool paused, sf::Vector2f mp) {
    // Pause button in top-right corner
    float bx = BASE_W - 50.f, by = 6.f;
    sf::RectangleShape bg(sf::Vector2f(38.f, 32.f));
    bg.setPosition(sf::Vector2f(bx, by));
    bool hover = sf::FloatRect(sf::Vector2f(bx, by),
        sf::Vector2f(38.f, 32.f)).contains(mp);
    bg.setFillColor(hover ? sf::Color(0, 160, 220, 180)
        : sf::Color(0, 0, 0, 0));
    window_.draw(bg);

    if (!paused) {
        // Draw || symbol using two rectangles
        sf::RectangleShape bar(sf::Vector2f(7.f, 20.f));
        bar.setFillColor(sf::Color(220, 240, 255, 220));
        bar.setPosition(sf::Vector2f(bx + 6.f, by + 6.f));
        window_.draw(bar);
        bar.setPosition(sf::Vector2f(bx + 19.f, by + 6.f));
        window_.draw(bar);
    }
    else {
        // Draw > triangle when paused
        sf::ConvexShape tri;
        tri.setPointCount(3);
        tri.setPoint(0, sf::Vector2f(0.f, 0.f));
        tri.setPoint(1, sf::Vector2f(0.f, 20.f));
        tri.setPoint(2, sf::Vector2f(16.f, 10.f));
        tri.setFillColor(sf::Color(220, 240, 255, 220));
        tri.setPosition(sf::Vector2f(bx + 8.f, by + 6.f));
        window_.draw(tri);
    }
}