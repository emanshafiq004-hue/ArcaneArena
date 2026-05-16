// GameScoresScreen.cpp
// Scores & Status screen.
// FIX: ESC from the score view returns to the tile grid, not to main menu.
// FIX: Audio uses playOptional("Click") matching main menu style.
// FIX: navCooldown moved to a member clock (no static inside function).
// FIX: Placeholder tiles show a "Coming Soon" panel with ESC-to-go-back hint.
// FIX: Cursor triangle added to the selected tile for visual consistency.

#include "GameScoresScreen.h"
#include "AssetLoader.h"
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;
using namespace sf;

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

    // Background image as specified in requirements.
    AssetLoader::emplaceCoverSprite(bgTex, bgSprite,
        { "assets/texture/utility.png", "assets/texture/default.png" },
        static_cast<float>(gw.getWidth()),
        static_cast<float>(gw.getHeight()));
}

// ─────────────────────────────────────────────────────────────────────────────
// DRAW TILE GRID MENU
// Two rows, two columns, side by side. Cursor highlights selected tile.
// ─────────────────────────────────────────────────────────────────────────────
void GameScoresScreen::drawMenu()
{
    gw.clear();
    if (bgSprite) gw.getWindow().draw(*bgSprite);

    // Title.
    Text title(font, "SCORES  &  STATUS", 38);
    title.setFillColor(Color(205, 185, 145));
    title.setOutlineColor(Color(30, 0, 50));
    title.setOutlineThickness(2.f);
    FloatRect tb = title.getLocalBounds();
    title.setOrigin({ tb.size.x / 2.f, tb.size.y / 2.f });
    title.setPosition({ static_cast<float>(gw.getWidth()) / 2.f, 60.f });
    gw.getWindow().draw(title);

    // Subtitle.
    Text sub(font, "Arrow keys / mouse to select     Enter to view", 16);
    sub.setFillColor(Color(160, 140, 120));
    FloatRect sb = sub.getLocalBounds();
    sub.setOrigin({ sb.size.x / 2.f, sb.size.y / 2.f });
    sub.setPosition({ static_cast<float>(gw.getWidth()) / 2.f, 104.f });
    gw.getWindow().draw(sub);

    // Tile dimensions and layout.
    const float cellW = 280.f;
    const float cellH = 110.f;
    const float gapX = 40.f;
    const float gapY = 30.f;
    const float totalW = COLS * cellW + (COLS - 1) * gapX;
    const float startX = (static_cast<float>(gw.getWidth()) - totalW) / 2.f;
    const float startY = 148.f;

    // Cursor triangle shape.
    ConvexShape cur;
    cur.setPointCount(3);
    cur.setPoint(0, { 0.f, 0.f });
    cur.setPoint(1, { 0.f, 14.f });
    cur.setPoint(2, { 12.f, 7.f });
    cur.setFillColor(Color(255, 220, 60));

    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLS; col++)
        {
            bool sel = (row == selectedRow && col == selectedCol);
            float bx = startX + col * (cellW + gapX);
            float by = startY + row * (cellH + gapY);

            // Tile box.
            RectangleShape box({ cellW, cellH });
            box.setPosition({ bx, by });
            box.setFillColor(sel ? Color(80, 60, 160) : Color(40, 30, 90));
            box.setOutlineColor(sel ? Color(255, 220, 60) : Color(80, 70, 140));
            box.setOutlineThickness(sel ? 2.f : 1.f);
            gw.getWindow().draw(box);

            // Game name label.
            Text t(font, gameNames[row][col], 22);
            t.setFillColor(sel ? Color(255, 255, 160) : Color(210, 200, 240));
            FloatRect lb = t.getLocalBounds();
            t.setOrigin({ lb.size.x / 2.f, lb.size.y / 2.f });
            t.setPosition({ bx + cellW / 2.f, by + cellH / 2.f });
            gw.getWindow().draw(t);

            // Cursor to the left of the selected tile.
            if (sel)
            {
                cur.setPosition({ bx + 8.f, by + cellH / 2.f - 7.f });
                gw.getWindow().draw(cur);
            }
        }
    }

    // ESC hint at the bottom.
    Text hint(font, "ESC - Back to main menu", 14);
    hint.setFillColor(Color(120, 110, 100));
    FloatRect hb = hint.getLocalBounds();
    hint.setOrigin({ hb.size.x / 2.f, hb.size.y / 2.f });
    hint.setPosition({ static_cast<float>(gw.getWidth()) / 2.f,
                       static_cast<float>(gw.getHeight()) - 30.f });
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

    // Title.
    Text title(font, currentGameName + " - High Scores", 30);
    title.setFillColor(Color(205, 185, 145));
    FloatRect tb = title.getLocalBounds();
    title.setOrigin({ tb.size.x / 2.f, tb.size.y / 2.f });
    title.setPosition({ static_cast<float>(gw.getWidth()) / 2.f, 50.f });
    gw.getWindow().draw(title);

    float y = 110.f;

    if (currentGameName == "Realm of Echoes")
    {
        // Load from Realm of Echoes save file.
        string data = FileManager::loadScores();
        if (data.empty())
        {
            Text none(font, "No scores recorded yet.", 20);
            none.setFillColor(Color(180, 160, 140));
            FloatRect nb = none.getLocalBounds();
            none.setOrigin({ nb.size.x / 2.f, nb.size.y / 2.f });
            none.setPosition({ static_cast<float>(gw.getWidth()) / 2.f,
                               static_cast<float>(gw.getHeight()) / 2.f });
            gw.getWindow().draw(none);
        }
        else
        {
            istringstream iss(data);
            string line;
            while (getline(iss, line))
            {
                Text t(font, line, 18);
                t.setFillColor(Color::White);
                t.setPosition({ 100.f, y });
                gw.getWindow().draw(t);
                y += 28.f;
            }
        }
    }
    else if (currentGameName == "Iron Fist")
    {
        // Load from Iron Fist ScoreBoard.
        ScoreBoard sb;
        sb.load();
        int n = sb.getCount();

        if (n == 0)
        {
            Text none(font, "No scores recorded yet.", 20);
            none.setFillColor(Color(180, 160, 140));
            FloatRect nb = none.getLocalBounds();
            none.setOrigin({ nb.size.x / 2.f, nb.size.y / 2.f });
            none.setPosition({ static_cast<float>(gw.getWidth()) / 2.f,
                               static_cast<float>(gw.getHeight()) / 2.f });
            gw.getWindow().draw(none);
        }
        else
        {
            // Sort by totalScore descending.
            vector<int> order(n);
            for (int i = 0; i < n; i++) order[i] = i;
            sort(order.begin(), order.end(), [&](int a, int b)
                {
                    return sb.get(a).totalScore() > sb.get(b).totalScore();
                });

            // Show top 10.
            for (int i = 0; i < n && i < 10; i++)
            {
                const auto& p = sb.get(order[i]);
                string entry = to_string(i + 1) + ".  " + p.name +
                    "   " + to_string(p.totalScore()) + " pts";
                Text t(font, entry, 18);
                t.setFillColor(i == 0 ? Color(255, 215, 0) : Color::White);
                t.setPosition({ 100.f, y });
                gw.getWindow().draw(t);
                y += 30.f;
            }
        }
    }
    else
    {
        // Placeholder for games not yet implemented.
        Text soon(font, currentGameName + "\n\nComing Soon!", 26);
        soon.setFillColor(Color(160, 100, 255));
        FloatRect sb = soon.getLocalBounds();
        soon.setOrigin({ sb.size.x / 2.f, sb.size.y / 2.f });
        soon.setPosition({ static_cast<float>(gw.getWidth()) / 2.f,
                           static_cast<float>(gw.getHeight()) / 2.f });
        gw.getWindow().draw(soon);
    }

    // Back hint.
    Text hint(font, "ESC - Back to game list", 14);
    hint.setFillColor(Color(120, 110, 100));
    FloatRect hb = hint.getLocalBounds();
    hint.setOrigin({ hb.size.x / 2.f, hb.size.y / 2.f });
    hint.setPosition({ static_cast<float>(gw.getWidth()) / 2.f,
                       static_cast<float>(gw.getHeight()) - 30.f });
    gw.getWindow().draw(hint);

    gw.display();
}

// ─────────────────────────────────────────────────────────────────────────────
// INPUT HANDLING
// navCooldown is a member clock so there is no static variable inside function.
// ─────────────────────────────────────────────────────────────────────────────
void GameScoresScreen::handleInput()
{
    if (navCooldown.getElapsedTime().asMilliseconds() < 160) return;

    if (!showingScores)
    {
        // Navigate the 2x2 grid.
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
            // ESC from tile grid exits this screen entirely (returns to main menu).
            navCooldown.restart();
            // Signal exit by closing the loop — handled in run().
        }

        // Mouse hover and click on tiles.
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
        // Inside scores view: ESC goes back to tile grid.
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
    // Play same audio as main menu / welcome screen.
    // (Audio is already playing from main menu; we don't restart it here.)
    audio.playOptional("Click", 0.f);  // warm up channel; volume 0 so inaudible.

    while (gw.isOpen())
    {
        // Poll events.
        while (auto ev = gw.pollEvent())
            if (ev->is<Event::Closed>()) gw.getWindow().close();

        // Check for ESC in tile grid (exits this screen).
        if (!showingScores && Keyboard::isKeyPressed(Keyboard::Key::Escape)
            && navCooldown.getElapsedTime().asMilliseconds() > 160)
        {
            navCooldown.restart();
            return;   // Back to main menu.
        }

        handleInput();

        if (!showingScores)
            drawMenu();
        else
            drawScores();
    }
}