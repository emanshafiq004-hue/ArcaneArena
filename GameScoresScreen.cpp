//// GameScoresScreen.cpp
//// Scores & Status screen.
//
//#include "GameScoresScreen.h"
//#include "AssetLoader.h"
//#include "Managers.h"
//#include "ScoreStorage.h"
//#include <fstream>
//#include <sstream>
//#include <algorithm>
//
//using namespace std;
//using namespace sf;
//
//struct RealmScore {
//    string name;
//    int level = 0;
//    int xp = 0;
//};
//
//static vector<RealmScore> loadRealmScoresTop5(const string& filename = "saves/scores.dat") {
//    ifstream file(filename);
//    if (!file.is_open())
//        return {};
//
//    vector<RealmScore> scores;
//    string line;
//    while (getline(file, line)) {
//        if (line.empty())
//            continue;
//        size_t first = line.find(',');
//        size_t second = (first == string::npos) ? string::npos : line.find(',', first + 1);
//        if (first == string::npos || second == string::npos)
//            continue;
//
//        RealmScore entry;
//        entry.name = line.substr(0, first);
//        try {
//            entry.level = stoi(line.substr(first + 1, second - first - 1));
//            entry.xp = stoi(line.substr(second + 1));
//        }
//        catch (...) {
//            continue;
//        }
//        if (entry.name.empty())
//            entry.name = "Unknown";
//        scores.push_back(move(entry));
//    }
//
//    sort(scores.begin(), scores.end(), [](const RealmScore& a, const RealmScore& b) {
//        return a.xp > b.xp;
//    });
//    if (scores.size() > 5)
//        scores.resize(5);
//    return scores;
//}
//
//// ─────────────────────────────────────────────────────────────────────────────
//// CONSTRUCTOR
//// ─────────────────────────────────────────────────────────────────────────────
//GameScoresScreen::GameScoresScreen(GameWindow& window, AudioManager& audioManager)
//    : gw(window), audio(audioManager)
//{
//    AssetLoader::openFontWithFallback(font, {
//        "assets/font/MainMenuPlayfairDisplay.ttf",
//        "assets/font/Philosopher-Bold.ttf"
//        });
//    AssetLoader::emplaceCoverSprite(bgTex, bgSprite,
//        { "assets/texture/bg_arcane_arena_welcome.png", "assets/texture/default.png" },
//        static_cast<float>(gw.getWidth()),
//        static_cast<float>(gw.getHeight()));
//}
//
//// ─────────────────────────────────────────────────────────────────────────────
//// DRAW TILE GRID MENU
//// Two rows, two columns, side by side. Cursor highlights selected tile.
//// ─────────────────────────────────────────────────────────────────────────────
//void GameScoresScreen::drawMenu()
//{
//    gw.clear();
//    if (bgSprite) gw.getWindow().draw(*bgSprite);
//
//    // Title in top panel.
//    Text title(font, "SCORES  &  STATUS", 38);
//    title.setFillColor(Color(205, 185, 145));
//    title.setOutlineColor(Color(30, 0, 50));
//    title.setOutlineThickness(2.f);
//    FloatRect tb = title.getLocalBounds();
//    title.setOrigin({ tb.size.x / 2.f, tb.size.y / 2.f });
//    title.setPosition({ static_cast<float>(gw.getWidth()) / 2.f, 40.f });
//    gw.getWindow().draw(title);
//
//    // Subtitle.
//    Text sub(font, "ARROW KEYS / MOUSE TO SELECT - ENTER TO VIEW", 14);
//    sub.setFillColor(Color(160, 140, 120));
//    FloatRect sb = sub.getLocalBounds();
//    sub.setOrigin({ sb.size.x / 2.f, sb.size.y / 2.f });
//    sub.setPosition({ static_cast<float>(gw.getWidth()) / 2.f, 70.f });
//    gw.getWindow().draw(sub);
//
//    // Tile dimensions and layout - games in central panel.
//    const float cellW = 280.f;
//    const float cellH = 110.f;
//    const float gapX = 40.f;
//    const float gapY = 30.f;
//    const float totalW = COLS * cellW + (COLS - 1) * gapX;
//    const float startX = (static_cast<float>(gw.getWidth()) - totalW) / 2.f;
//    const float startY = 140.f;  // Center of central panel
//
//    // Cursor triangle shape.
//    ConvexShape cur;
//    cur.setPointCount(3);
//    cur.setPoint(0, { 0.f, 0.f });
//    cur.setPoint(1, { 0.f, 14.f });
//    cur.setPoint(2, { 12.f, 7.f });
//    cur.setFillColor(Color(255, 220, 60));
//
//    for (int row = 0; row < ROWS; row++)
//    {
//        for (int col = 0; col < COLS; col++)
//        {
//            bool sel = (row == selectedRow && col == selectedCol);
//            float bx = startX + col * (cellW + gapX);
//            float by = startY + row * (cellH + gapY);
//
//            // Game name label.
//            Text t(font, gameNames[row][col], 28);
//            t.setFillColor(sel ? Color(255, 255, 160) : Color(185, 170, 140));
//            FloatRect lb = t.getLocalBounds();
//            t.setOrigin({ lb.size.x / 2.f, lb.size.y / 2.f });
//            t.setPosition({ bx + cellW / 2.f, by + cellH / 2.f });
//            gw.getWindow().draw(t);
//
//            // Cursor to the left of the selected tile.
//            if (sel)
//            {
//                cur.setPosition({ bx + 8.f, by + cellH / 2.f - 7.f });
//                gw.getWindow().draw(cur);
//            }
//        }
//    }
//
//    // ESC hint in lower panel.
//    Text hint(font, "ESC - BACK TO MAIN MENU", 16);
//    hint.setFillColor(Color(120, 110, 100));
//    FloatRect hb = hint.getLocalBounds();
//    hint.setOrigin({ hb.size.x / 2.f, hb.size.y / 2.f });
//    hint.setPosition({ static_cast<float>(gw.getWidth()) / 2.f,
//                       static_cast<float>(gw.getHeight()) - 40.f });
//    gw.getWindow().draw(hint);
//
//    gw.display();
//}
//
//// ─────────────────────────────────────────────────────────────────────────────
//// DRAW SCORES FOR SELECTED GAME
//// ─────────────────────────────────────────────────────────────────────────────
//void GameScoresScreen::drawScores()
//{
//    gw.clear();
//    if (bgSprite) gw.getWindow().draw(*bgSprite);
//
//    // Title in top panel.
//    Text title(font, currentGameName + " - TOP 5 SCORES", 32);
//    title.setFillColor(Color(205, 185, 145));
//    FloatRect tb = title.getLocalBounds();
//    title.setOrigin({ tb.size.x / 2.f, tb.size.y / 2.f });
//    title.setPosition({ static_cast<float>(gw.getWidth()) / 2.f, 40.f });
//    gw.getWindow().draw(title);
//
//    const float tableWidth = 520.f;
//    const float startX = (static_cast<float>(gw.getWidth()) - tableWidth) / 2.f;
//    const float COL_NAME = startX + tableWidth * 0.17f;
//    const float COL_SCORE = startX + tableWidth * 0.52f;
//    const float COL_STATUS = startX + tableWidth * 0.85f;
//    const float START_Y = 140.f;  // Center of central panel
//    const float ROW_SPACING = 30.f;
//
//    auto makeHeader = [&](const string& label, float x) {
//        Text header(font, label, 18);
//        header.setFillColor(Color(180, 170, 210));
//        header.setStyle(Text::Bold);
//        header.setOrigin(header.getLocalBounds().getCenter());
//        header.setPosition({ x, 110.f });
//        gw.getWindow().draw(header);
//    };
//
//    auto drawRow = [&](const string& name, const string& total, const string& status,
//        int index, const Color& color) {
//        float y = START_Y + index * ROW_SPACING;
//        Text nameText(font, name, 18);
//        nameText.setFillColor(color);
//        nameText.setOrigin(nameText.getLocalBounds().getCenter());
//        nameText.setPosition({ COL_NAME, y });
//        gw.getWindow().draw(nameText);
//
//        Text scoreText(font, total, 18);
//        scoreText.setFillColor(color);
//        scoreText.setOrigin(scoreText.getLocalBounds().getCenter());
//        scoreText.setPosition({ COL_SCORE, y });
//        gw.getWindow().draw(scoreText);
//
//        Text statusText(font, status, 18);
//        statusText.setFillColor(color);
//        statusText.setOrigin(statusText.getLocalBounds().getCenter());
//        statusText.setPosition({ COL_STATUS, y });
//        gw.getWindow().draw(statusText);
//    };
//
//    bool hasEntries = false;
//    if (currentGameName == "Realm of Echoes")
//    {
//        auto entries = loadRealmScoresTop5();
//        if (!entries.empty())
//        {
//            hasEntries = true;
//            makeHeader("Name", COL_NAME);
//            makeHeader("XP", COL_SCORE);
//            makeHeader("Status", COL_STATUS);
//            for (int i = 0; i < (int)entries.size(); i++)
//            {
//                const auto& entry = entries[i];
//                drawRow(entry.name,
//                    to_string(entry.xp),
//                    "Level " + to_string(entry.level),
//                    i,
//                    i == 0 ? Color(255, 215, 0) : Color::White);
//            }
//        }
//    }
//    else if (currentGameName == "Iron Fist")
//    {
//        ScoreBoard sb;
//        sb.load();
//        int n = sb.getCount();
//        if (n > 0)
//        {
//            hasEntries = true;
//            makeHeader("Name", COL_NAME);
//            makeHeader("Total", COL_SCORE);
//            makeHeader("Status", COL_STATUS);
//
//            vector<int> order(n);
//            for (int i = 0; i < n; i++) order[i] = i;
//            sort(order.begin(), order.end(), [&](int a, int b) {
//                return sb.get(a).totalScore() > sb.get(b).totalScore();
//            });
//
//            int show = min(n, 10);
//            for (int i = 0; i < show; i++)
//            {
//                const auto& player = sb.get(order[i]);
//                drawRow(player.name,
//                    to_string(player.totalScore()),
//                    player.isComplete() ? "Complete" : "Round " + to_string(player.nextRound()),
//                    i,
//                    i == 0 ? Color(255, 215, 0) : Color::White);
//            }
//        }
//    }
//    else if (currentGameName == "Shatter Shift")
//    {
//        ShatterScoreManager scoreManager;
//        scoreManager.load();
//        const auto& entries = scoreManager.getEntries();
//        if (!entries.empty())
//        {
//            hasEntries = true;
//            makeHeader("Name", COL_NAME);
//            makeHeader("Score", COL_SCORE);
//            makeHeader("Status", COL_STATUS);
//            for (int i = 0; i < (int)entries.size(); i++)
//            {
//                const auto& entry = entries[i];
//                drawRow(entry.name,
//                    to_string(entry.score),
//                    "Level " + to_string(entry.level),
//                    i,
//                    i == 0 ? Color(255, 215, 0) : Color::White);
//            }
//        }
//    }
//    else if (currentGameName == "Sky Surge")
//    {
//        FileManager_SkySurge fm;
//        auto entries = fm.loadScores();
//        if (!entries.empty())
//        {
//            hasEntries = true;
//            makeHeader("Name", COL_NAME);
//            makeHeader("Score", COL_SCORE);
//            makeHeader("Status", COL_STATUS);
//            int show = min((int)entries.size(), 10);
//            for (int i = 0; i < show; i++)
//            {
//                const auto& entry = entries[i];
//                drawRow(entry.name,
//                    to_string(entry.score),
//                    entry.date,
//                    i,
//                    i == 0 ? Color(255, 215, 0) : Color::White);
//            }
//        }
//    }
//
//    if (!hasEntries)
//    {
//        Text none(font, "No scores recorded yet.", 20);
//        none.setFillColor(Color(180, 160, 140));
//        FloatRect nb = none.getLocalBounds();
//        none.setOrigin({ nb.size.x / 2.f, nb.size.y / 2.f });
//        none.setPosition({ static_cast<float>(gw.getWidth()) / 2.f,
//                           static_cast<float>(gw.getHeight()) / 2.f });
//        gw.getWindow().draw(none);
//    }
//
//    // Back hint in lower panel.
//    Text hint(font, "ESC - BACK TO GAME LIST", 14);
//    hint.setFillColor(Color(120, 110, 100));
//    FloatRect hb = hint.getLocalBounds();
//    hint.setOrigin({ hb.size.x / 2.f, hb.size.y / 2.f });
//    hint.setPosition({ static_cast<float>(gw.getWidth()) / 2.f,
//                       static_cast<float>(gw.getHeight()) - 40.f });
//    gw.getWindow().draw(hint);
//
//    gw.display();
//}
//
//// ─────────────────────────────────────────────────────────────────────────────
//// INPUT HANDLING
//// navCooldown is a member clock so there is no static variable inside function.
//// ─────────────────────────────────────────────────────────────────────────────
//void GameScoresScreen::handleInput()
//{
//    if (navCooldown.getElapsedTime().asMilliseconds() < 160) return;
//
//    if (!showingScores)
//    {
//        // Navigate the 2x2 grid.
//        if (Keyboard::isKeyPressed(Keyboard::Key::Left))
//        {
//            selectedCol = (selectedCol + COLS - 1) % COLS;
//            navCooldown.restart();
//            audio.playOptional("Click", 80.f);
//        }
//        else if (Keyboard::isKeyPressed(Keyboard::Key::Right))
//        {
//            selectedCol = (selectedCol + 1) % COLS;
//            navCooldown.restart();
//            audio.playOptional("Click", 80.f);
//        }
//        else if (Keyboard::isKeyPressed(Keyboard::Key::Up))
//        {
//            selectedRow = (selectedRow + ROWS - 1) % ROWS;
//            navCooldown.restart();
//            audio.playOptional("Click", 80.f);
//        }
//        else if (Keyboard::isKeyPressed(Keyboard::Key::Down))
//        {
//            selectedRow = (selectedRow + 1) % ROWS;
//            navCooldown.restart();
//            audio.playOptional("Click", 80.f);
//        }
//        else if (Keyboard::isKeyPressed(Keyboard::Key::Enter))
//        {
//            currentGameName = gameNames[selectedRow][selectedCol];
//            showingScores = true;
//            navCooldown.restart();
//            audio.playOptional("Click", 80.f);
//        }
//        else if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
//        {
//            // ESC from tile grid exits this screen entirely (returns to main menu).
//            navCooldown.restart();
//            // Signal exit by closing the loop — handled in run().
//        }
//
//        // Mouse hover and click on tiles.
//        Vector2i mp = Mouse::getPosition(gw.getWindow());
//        Vector2f mw = gw.getWindow().mapPixelToCoords(mp);
//
//        const float cellW = 280.f;
//        const float cellH = 110.f;
//        const float gapX = 40.f;
//        const float gapY = 30.f;
//        const float totalW = COLS * cellW + (COLS - 1) * gapX;
//        const float startX = (static_cast<float>(gw.getWidth()) - totalW) / 2.f;
//        const float startY = 148.f;
//
//        for (int row = 0; row < ROWS; row++)
//        {
//            for (int col = 0; col < COLS; col++)
//            {
//                FloatRect tileRect(
//                    { startX + col * (cellW + gapX), startY + row * (cellH + gapY) },
//                    { cellW, cellH });
//                if (tileRect.contains(mw))
//                {
//                    selectedRow = row;
//                    selectedCol = col;
//                    if (Mouse::isButtonPressed(Mouse::Button::Left))
//                    {
//                        currentGameName = gameNames[row][col];
//                        showingScores = true;
//                        navCooldown.restart();
//                        audio.playOptional("Click", 80.f);
//                    }
//                }
//            }
//        }
//    }
//    else
//    {
//        // Inside scores view: ESC goes back to tile grid.
//        if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
//        {
//            showingScores = false;
//            navCooldown.restart();
//            audio.playOptional("Click", 80.f);
//        }
//    }
//}
//
//// ─────────────────────────────────────────────────────────────────────────────
//// MAIN LOOP
//// ─────────────────────────────────────────────────────────────────────────────
//void GameScoresScreen::run()
//{
//
//    audio.playOptional("Click", 0.f);  // warm up channel; volume 0 so inaudible.
//
//    while (gw.isOpen())
//    {
//        // Poll events.
//        while (auto ev = gw.pollEvent())
//            if (ev->is<Event::Closed>()) gw.getWindow().close();
//
//        // Check for ESC in tile grid (exits this screen).
//        if (!showingScores && Keyboard::isKeyPressed(Keyboard::Key::Escape)
//            && navCooldown.getElapsedTime().asMilliseconds() > 160)
//        {
//            navCooldown.restart();
//            return;   // Back to main menu.
//        }
//
//        handleInput();
//
//        if (!showingScores)
//            drawMenu();
//        else
//            drawScores();
//    }
//}



// GameScoresScreen.cpp
// Scores & Status screen.

#include "GameScoresScreen.h"
#include "AssetLoader.h"
#include "Managers.h"
#include "ScoreStorage.h"
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;
using namespace sf;

struct RealmScore {
    string name;
    int level = 0;
    int xp = 0;
};

static vector<RealmScore> loadRealmScoresTop5(const string& filename = "saves/scores.dat") {
    ifstream file(filename);
    if (!file.is_open()) return {};

    vector<RealmScore> scores;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        size_t first = line.find(',');
        size_t second = (first == string::npos) ? string::npos : line.find(',', first + 1);
        if (first == string::npos || second == string::npos) continue;

        RealmScore entry;
        entry.name = line.substr(0, first);
        try {
            entry.level = stoi(line.substr(first + 1, second - first - 1));
            entry.xp = stoi(line.substr(second + 1));
        }
        catch (...) { continue; }
        if (entry.name.empty()) entry.name = "Unknown";
        scores.push_back(move(entry));
    }

    sort(scores.begin(), scores.end(), [](const RealmScore& a, const RealmScore& b) {
        return a.xp > b.xp;
        });
    if (scores.size() > 5) scores.resize(5);
    return scores;
}

// ─────────────────────────────────────────────────────────────────────────────
// CONSTRUCTOR
// ─────────────────────────────────────────────────────────────────────────────
GameScoresScreen::GameScoresScreen(GameWindow& window, AudioManager& audioManager)
    : gw(window), audio(audioManager)
{
    AssetLoader::openFontWithFallback(font, {
        "assets/font/MainMenuPlayfairDisplay.ttf",
        "assets/font/Philosopher-Bold.ttf"
        });
    AssetLoader::emplaceCoverSprite(bgTex, bgSprite,
        { "assets/texture/bg_arcane_arena_welcome.png", "assets/texture/default.png" },
        static_cast<float>(gw.getWidth()),
        static_cast<float>(gw.getHeight()));
}

// ─────────────────────────────────────────────────────────────────────────────
// DRAW TILE GRID MENU
// ─────────────────────────────────────────────────────────────────────────────
void GameScoresScreen::drawMenu()
{
    gw.clear();
    if (bgSprite) gw.getWindow().draw(*bgSprite);

    // Title — bright gold, visible on any dark arcane background
    Text title(font, "SCORES  &  STATUS", 38);
    title.setFillColor(Color(255, 215, 80));
    title.setOutlineColor(Color(20, 0, 40));
    title.setOutlineThickness(2.f);
    FloatRect tb = title.getLocalBounds();
    title.setOrigin({ tb.size.x / 2.f, tb.size.y / 2.f });
    title.setPosition({ static_cast<float>(gw.getWidth()) / 2.f, 40.f });
    gw.getWindow().draw(title);

    // Subtitle — soft lavender
    Text sub(font, "ARROW KEYS / MOUSE TO SELECT - ENTER TO VIEW", 14);
    sub.setFillColor(Color(200, 180, 255));
    FloatRect sb = sub.getLocalBounds();
    sub.setOrigin({ sb.size.x / 2.f, sb.size.y / 2.f });
    sub.setPosition({ static_cast<float>(gw.getWidth()) / 2.f, 70.f });
    gw.getWindow().draw(sub);

    const float cellW = 280.f;
    const float cellH = 110.f;
    const float gapX = 40.f;
    const float gapY = 30.f;
    const float totalW = COLS * cellW + (COLS - 1) * gapX;
    const float startX = (static_cast<float>(gw.getWidth()) - totalW) / 2.f;
    const float startY = 140.f;

    ConvexShape cur;
    cur.setPointCount(3);
    cur.setPoint(0, { 0.f,  0.f });
    cur.setPoint(1, { 0.f,  14.f });
    cur.setPoint(2, { 12.f, 7.f });
    cur.setFillColor(Color(255, 215, 60));

    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLS; col++)
        {
            bool sel = (row == selectedRow && col == selectedCol);
            float bx = startX + col * (cellW + gapX);
            float by = startY + row * (cellH + gapY);

            // Draw a subtle dark tile backing
            RectangleShape tile({ cellW, cellH });
            tile.setPosition({ bx, by });
            tile.setFillColor(Color(20, 5, 45, 180));
            tile.setOutlineColor(sel ? Color(200, 80, 255) : Color(80, 50, 120));
            tile.setOutlineThickness(sel ? 2.f : 1.f);
            gw.getWindow().draw(tile);

            // Game name label — gold when selected, soft purple-white otherwise
            Text t(font, gameNames[row][col], 26);
            t.setFillColor(sel ? Color(255, 230, 80) : Color(210, 185, 255));
            FloatRect lb = t.getLocalBounds();
            t.setOrigin({ lb.size.x / 2.f, lb.size.y / 2.f });
            t.setPosition({ bx + cellW / 2.f, by + cellH / 2.f });
            gw.getWindow().draw(t);

            if (sel)
            {
                cur.setPosition({ bx + 8.f, by + cellH / 2.f - 7.f });
                gw.getWindow().draw(cur);
            }
        }
    }

    // ESC hint — muted
    Text hint(font, "ESC - BACK TO MAIN MENU", 16);
    hint.setFillColor(Color(160, 140, 200));
    FloatRect hb = hint.getLocalBounds();
    hint.setOrigin({ hb.size.x / 2.f, hb.size.y / 2.f });
    hint.setPosition({ static_cast<float>(gw.getWidth()) / 2.f,
                       static_cast<float>(gw.getHeight()) - 40.f });
    gw.getWindow().draw(hint);

    gw.display();
}

// ─────────────────────────────────────────────────────────────────────────────
// DRAW SCORES FOR SELECTED GAME
// ─────────────────────────────────────────────────────────────────────────────
void GameScoresScreen::drawScores()
{
    gw.clear();
    if (bgSprite) gw.getWindow().draw(*bgSprite);

    // Title — gold
    Text title(font, currentGameName + " - TOP 5 SCORES", 32);
    title.setFillColor(Color(255, 215, 80));
    title.setOutlineColor(Color(20, 0, 40));
    title.setOutlineThickness(2.f);
    FloatRect tb = title.getLocalBounds();
    title.setOrigin({ tb.size.x / 2.f, tb.size.y / 2.f });
    title.setPosition({ static_cast<float>(gw.getWidth()) / 2.f, 40.f });
    gw.getWindow().draw(title);

    const float tableWidth = 520.f;
    const float startX = (static_cast<float>(gw.getWidth()) - tableWidth) / 2.f;
    const float COL_NAME = startX + tableWidth * 0.17f;
    const float COL_SCORE = startX + tableWidth * 0.52f;
    const float COL_STATUS = startX + tableWidth * 0.85f;
    const float START_Y = 140.f;
    const float ROW_SPACING = 30.f;

    auto makeHeader = [&](const string& label, float x) {
        Text header(font, label, 18);
        // Column headers — bright cyan
        header.setFillColor(Color(0, 210, 255));
        header.setStyle(Text::Bold);
        header.setOrigin(header.getLocalBounds().getCenter());
        header.setPosition({ x, 110.f });
        gw.getWindow().draw(header);
        };

    auto drawRow = [&](const string& name, const string& total, const string& status,
        int index, const Color& color) {
            float y = START_Y + index * ROW_SPACING;

            Text nameText(font, name, 18);
            nameText.setFillColor(color);
            nameText.setOrigin(nameText.getLocalBounds().getCenter());
            nameText.setPosition({ COL_NAME, y });
            gw.getWindow().draw(nameText);

            Text scoreText(font, total, 18);
            scoreText.setFillColor(color);
            scoreText.setOrigin(scoreText.getLocalBounds().getCenter());
            scoreText.setPosition({ COL_SCORE, y });
            gw.getWindow().draw(scoreText);

            Text statusText(font, status, 18);
            statusText.setFillColor(color);
            statusText.setOrigin(statusText.getLocalBounds().getCenter());
            statusText.setPosition({ COL_STATUS, y });
            gw.getWindow().draw(statusText);
        };

    // Row colours: gold for rank 1, lavender for the rest
    auto rowColor = [](int i) -> Color {
        return i == 0 ? Color(255, 215, 0) : Color(200, 185, 255);
        };

    bool hasEntries = false;
    if (currentGameName == "Realm of Echoes")
    {
        auto entries = loadRealmScoresTop5();
        if (!entries.empty())
        {
            hasEntries = true;
            makeHeader("Name", COL_NAME);
            makeHeader("XP", COL_SCORE);
            makeHeader("Status", COL_STATUS);
            for (int i = 0; i < (int)entries.size(); i++)
            {
                const auto& entry = entries[i];
                drawRow(entry.name,
                    to_string(entry.xp),
                    "Level " + to_string(entry.level),
                    i,
                    rowColor(i));
            }
        }
    }
    else if (currentGameName == "Iron Fist")
    {
        ScoreBoard sb;
        sb.load();
        int n = sb.getCount();
        if (n > 0)
        {
            hasEntries = true;
            makeHeader("Name", COL_NAME);
            makeHeader("Total", COL_SCORE);
            makeHeader("Status", COL_STATUS);

            vector<int> order(n);
            for (int i = 0; i < n; i++) order[i] = i;
            sort(order.begin(), order.end(), [&](int a, int b) {
                return sb.get(a).totalScore() > sb.get(b).totalScore();
                });

            int show = min(n, 10);
            for (int i = 0; i < show; i++)
            {
                const auto& pl = sb.get(order[i]);
                drawRow(pl.name,
                    to_string(pl.totalScore()),
                    pl.isComplete() ? "Complete" : "Round " + to_string(pl.nextRound()),
                    i,
                    rowColor(i));
            }
        }
    }
    else if (currentGameName == "Shatter Shift")
    {
        ShatterScoreManager scoreManager;
        scoreManager.load();
        const auto& entries = scoreManager.getEntries();
        if (!entries.empty())
        {
            hasEntries = true;
            makeHeader("Name", COL_NAME);
            makeHeader("Score", COL_SCORE);
            makeHeader("Status", COL_STATUS);
            for (int i = 0; i < (int)entries.size(); i++)
            {
                const auto& entry = entries[i];
                drawRow(entry.name,
                    to_string(entry.score),
                    "Level " + to_string(entry.level),
                    i,
                    rowColor(i));
            }
        }
    }
    else if (currentGameName == "Sky Surge")
    {
        FileManager_SkySurge fm;
        auto entries = fm.loadScores();
        if (!entries.empty())
        {
            hasEntries = true;
            makeHeader("Name", COL_NAME);
            makeHeader("Score", COL_SCORE);
            makeHeader("Status", COL_STATUS);
            int show = min((int)entries.size(), 10);
            for (int i = 0; i < show; i++)
            {
                const auto& entry = entries[i];
                drawRow(entry.name,
                    to_string(entry.score),
                    entry.date,
                    i,
                    rowColor(i));
            }
        }
    }

    if (!hasEntries)
    {
        Text none(font, "No scores recorded yet.", 20);
        none.setFillColor(Color(200, 185, 255));
        FloatRect nb = none.getLocalBounds();
        none.setOrigin({ nb.size.x / 2.f, nb.size.y / 2.f });
        none.setPosition({ static_cast<float>(gw.getWidth()) / 2.f,
                           static_cast<float>(gw.getHeight()) / 2.f });
        gw.getWindow().draw(none);
    }

    // Back hint
    Text hint(font, "ESC - BACK TO GAME LIST", 14);
    hint.setFillColor(Color(160, 140, 200));
    FloatRect hb = hint.getLocalBounds();
    hint.setOrigin({ hb.size.x / 2.f, hb.size.y / 2.f });
    hint.setPosition({ static_cast<float>(gw.getWidth()) / 2.f,
                       static_cast<float>(gw.getHeight()) - 40.f });
    gw.getWindow().draw(hint);

    gw.display();
}

// ─────────────────────────────────────────────────────────────────────────────
// INPUT HANDLING
// ─────────────────────────────────────────────────────────────────────────────
void GameScoresScreen::handleInput()
{
    if (navCooldown.getElapsedTime().asMilliseconds() < 160) return;

    if (!showingScores)
    {
        if (Keyboard::isKeyPressed(Keyboard::Key::Left))
        {
            selectedCol = (selectedCol + COLS - 1) % COLS;
            navCooldown.restart();
            audio.playOptional("Click", 80.f);
        }
        else if (Keyboard::isKeyPressed(Keyboard::Key::Right))
        {
            selectedCol = (selectedCol + 1) % COLS;
            navCooldown.restart();
            audio.playOptional("Click", 80.f);
        }
        else if (Keyboard::isKeyPressed(Keyboard::Key::Up))
        {
            selectedRow = (selectedRow + ROWS - 1) % ROWS;
            navCooldown.restart();
            audio.playOptional("Click", 80.f);
        }
        else if (Keyboard::isKeyPressed(Keyboard::Key::Down))
        {
            selectedRow = (selectedRow + 1) % ROWS;
            navCooldown.restart();
            audio.playOptional("Click", 80.f);
        }
        else if (Keyboard::isKeyPressed(Keyboard::Key::Enter))
        {
            currentGameName = gameNames[selectedRow][selectedCol];
            showingScores = true;
            navCooldown.restart();
            audio.playOptional("Click", 80.f);
        }
        else if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
        {
            navCooldown.restart();
        }

        Vector2i mp = Mouse::getPosition(gw.getWindow());
        Vector2f mw = gw.getWindow().mapPixelToCoords(mp);

        const float cellW = 280.f;
        const float cellH = 110.f;
        const float gapX = 40.f;
        const float gapY = 30.f;
        const float totalW = COLS * cellW + (COLS - 1) * gapX;
        const float startX = (static_cast<float>(gw.getWidth()) - totalW) / 2.f;
        const float startY = 148.f;

        for (int row = 0; row < ROWS; row++)
        {
            for (int col = 0; col < COLS; col++)
            {
                FloatRect tileRect(
                    { startX + col * (cellW + gapX), startY + row * (cellH + gapY) },
                    { cellW, cellH });
                if (tileRect.contains(mw))
                {
                    selectedRow = row;
                    selectedCol = col;
                    if (Mouse::isButtonPressed(Mouse::Button::Left))
                    {
                        currentGameName = gameNames[row][col];
                        showingScores = true;
                        navCooldown.restart();
                        audio.playOptional("Click", 80.f);
                    }
                }
            }
        }
    }
    else
    {
        if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
        {
            showingScores = false;
            navCooldown.restart();
            audio.playOptional("Click", 80.f);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// MAIN LOOP
// ─────────────────────────────────────────────────────────────────────────────
void GameScoresScreen::run()
{
    audio.playOptional("Click", 0.f);  // warm up channel; volume 0 so inaudible.

    while (gw.isOpen())
    {
        while (auto ev = gw.pollEvent())
            if (ev->is<Event::Closed>()) gw.getWindow().close();

        if (!showingScores && Keyboard::isKeyPressed(Keyboard::Key::Escape)
            && navCooldown.getElapsedTime().asMilliseconds() > 160)
        {
            navCooldown.restart();
            return;
        }

        handleInput();

        if (!showingScores) drawMenu();
        else                drawScores();
    }
}