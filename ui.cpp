#include "ui.h"
#include <fstream>
#include <string>
using namespace std;

// ============================================================
//  Colour palettes
// ============================================================
const sf::Color C_BG = sf::Color(8, 24, 14);
const sf::Color C_PRIMARY = sf::Color(55, 148, 80);
const sf::Color C_ACCENT = sf::Color(130, 210, 90);
const sf::Color C_DIM = sf::Color(25, 70, 38);

const sf::Color A_BG = sf::Color(10, 18, 48);
const sf::Color A_PRIMARY = sf::Color(220, 175, 55);
const sf::Color A_ACCENT = sf::Color(255, 228, 120);
const sf::Color A_BLUE = sf::Color(80, 130, 210);

const sf::Color K_BG = sf::Color(12, 14, 20);
const sf::Color K_PRIMARY = sf::Color(140, 155, 170);
const sf::Color K_ACCENT = sf::Color(205, 215, 228);
const sf::Color K_DIM = sf::Color(45, 52, 65);

const sf::Color S_BG = sf::Color(18, 8, 8);
const sf::Color S_PRIMARY = sf::Color(190, 60, 40);
const sf::Color S_ACCENT = sf::Color(255, 130, 80);
const sf::Color S_DIM = sf::Color(65, 22, 18);

const sf::Color BG_DARK = sf::Color(8, 8, 18);
const sf::Color FRAME_OUTER = sf::Color(80, 80, 110);
const sf::Color FRAME_INNER = sf::Color(40, 40, 60);
const sf::Color FRAME_DIV = sf::Color(60, 60, 90);
const sf::Color DOT_COLOR = sf::Color(120, 120, 180);
const sf::Color TEXT_BRIGHT = sf::Color(220, 220, 255);
const sf::Color TEXT_DIM = sf::Color(70, 70, 100);
const sf::Color TEXT_HINT = sf::Color(60, 60, 90);

// ============================================================
//  Internal helpers
// ============================================================
static void drawCornerDots(sf::RenderWindow& window, sf::Color color)
{
    float dotX[4] = { 34.f, 758.f, 34.f, 758.f };
    float dotY[4] = { 34.f, 34.f, 558.f, 558.f };
    for (int i = 0; i < 4; i++)
    {
        sf::RectangleShape dot(sf::Vector2f(10.f, 10.f));
        dot.setPosition(sf::Vector2f(dotX[i], dotY[i]));
        dot.setFillColor(color);
        window.draw(dot);
    }
}

static void drawScreenFrame(sf::RenderWindow& window,
    sf::Color outerColor, sf::Color innerColor, sf::Color dotColor,
    sf::Color outerFill = sf::Color::Transparent)
{
    sf::RectangleShape outer(sf::Vector2f(740.f, 540.f));
    outer.setPosition(sf::Vector2f(30.f, 30.f));
    outer.setFillColor(outerFill);
    outer.setOutlineColor(outerColor);
    outer.setOutlineThickness(3.f);
    window.draw(outer);

    sf::RectangleShape inner(sf::Vector2f(700.f, 500.f));
    inner.setPosition(sf::Vector2f(50.f, 50.f));
    inner.setFillColor(sf::Color::Transparent);
    inner.setOutlineColor(innerColor);
    inner.setOutlineThickness(1.f);
    window.draw(inner);

    drawCornerDots(window, dotColor);
}

static void drawDivider(sf::RenderWindow& window, float x, float y, float w, sf::Color color)
{
    sf::RectangleShape div(sf::Vector2f(w, 2.f));
    div.setPosition(sf::Vector2f(x, y));
    div.setFillColor(color);
    window.draw(div);
}

// ============================================================
//  PUBLIC: small helpers
// ============================================================
sf::Text makeCenteredText(const sf::Font& font, const string& str,
    unsigned int size, sf::Color color, float windowW, float y)
{
    sf::Text t(font, str, size);
    t.setFillColor(color);
    sf::FloatRect lb = t.getLocalBounds();
    t.setOrigin(sf::Vector2f(lb.position.x + lb.size.x * 0.5f,
        lb.position.y + lb.size.y * 0.5f));
    t.setPosition(sf::Vector2f(windowW * 0.5f, y));
    return t;
}

void drawHealthBar(sf::RenderWindow& window, float x, float y, float width,
    int current, int max, sf::Color color)
{
    const float H = 12.f;

    sf::RectangleShape bg(sf::Vector2f(width, H));
    bg.setPosition(sf::Vector2f(x, y));
    bg.setFillColor(sf::Color(60, 60, 60));
    window.draw(bg);

    float ratio = (float)current / (float)max;
    if (ratio < 0.f) ratio = 0.f;
    sf::RectangleShape bar(sf::Vector2f(width * ratio, H));
    bar.setPosition(sf::Vector2f(x, y));
    bar.setFillColor(color);
    window.draw(bar);

    sf::RectangleShape border(sf::Vector2f(width, H));
    border.setPosition(sf::Vector2f(x, y));
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color(200, 200, 200));
    border.setOutlineThickness(1.f);
    window.draw(border);
}

// ============================================================
//  PUBLIC: full-screen pages
// ============================================================
void drawPressStartScreen(sf::RenderWindow& window, const sf::Font& font, sf::Clock& blinkClock)
{
    window.clear(BG_DARK);
    drawScreenFrame(window, FRAME_OUTER, FRAME_INNER, DOT_COLOR);

    sf::Text t1 = makeCenteredText(font, "IRON FIST", 80, TEXT_BRIGHT, 800.f, 190.f);
    sf::Text t2 = makeCenteredText(font, "CHRONICLES", 46, sf::Color(140, 140, 200), 800.f, 290.f);
    window.draw(t1);
    window.draw(t2);

    drawDivider(window, 220.f, 340.f, 360.f, FRAME_DIV);

    bool showBlink = ((int)(blinkClock.getElapsedTime().asSeconds() / 0.5f) % 2 == 0);
    if (showBlink)
    {
        sf::Text prompt = makeCenteredText(font, "Press SPACE to Start", 26,
            sf::Color(180, 180, 230), 800.f, 400.f);
        window.draw(prompt);
    }
}

// ----------------------------------------------------------------
//  Main menu — three options now: New / Continue / Scoreboard
// ----------------------------------------------------------------
void drawMenuScreen(sf::RenderWindow& window, const sf::Font& font, int menuIndex)
{
    window.clear(BG_DARK);
    drawScreenFrame(window, FRAME_OUTER, FRAME_INNER, DOT_COLOR);

    sf::Text title = makeCenteredText(font, "MAIN MENU", 48,
        sf::Color(200, 200, 250), 800.f, 145.f);
    window.draw(title);

    drawDivider(window, 220.f, 200.f, 360.f, FRAME_DIV);

    sf::Color col0 = (menuIndex == 0) ? TEXT_BRIGHT : TEXT_DIM;
    sf::Color col1 = (menuIndex == 1) ? TEXT_BRIGHT : TEXT_DIM;
    sf::Color col2 = (menuIndex == 2) ? TEXT_BRIGHT : TEXT_DIM;

    sf::Text o0 = makeCenteredText(font, "START NEW GAME", 30, col0, 800.f, 265.f);
    sf::Text o1 = makeCenteredText(font, "CONTINUE PREVIOUS GAME", 30, col1, 800.f, 325.f);
    sf::Text o2 = makeCenteredText(font, "VIEW SCOREBOARD", 30, col2, 800.f, 385.f);
    window.draw(o0);
    window.draw(o1);
    window.draw(o2);

    sf::Text arrow(font, ">", 30);
    arrow.setFillColor(sf::Color(180, 180, 255));
    float arrowY = 250.f + menuIndex * 60.f;

    sf::FloatRect selBounds;
    if (menuIndex == 0) selBounds = o0.getGlobalBounds();
    else if (menuIndex == 1) selBounds = o1.getGlobalBounds();
    else selBounds = o2.getGlobalBounds();
    arrow.setPosition(sf::Vector2f(selBounds.position.x - 30.f, arrowY));
    window.draw(arrow);

    sf::Text hint = makeCenteredText(font, "W / S  to move     J  to select", 18,
        TEXT_HINT, 800.f, 470.f);
    window.draw(hint);
}

// ----------------------------------------------------------------
//  Name input screen
// ----------------------------------------------------------------
void drawNameInputScreen(sf::RenderWindow& window, const sf::Font& font,
    const string& currentName, sf::Clock& blinkClock)
{
    window.clear(BG_DARK);
    drawScreenFrame(window, FRAME_OUTER, FRAME_INNER, DOT_COLOR);

    sf::Text title = makeCenteredText(font, "NEW GAME", 48,
        sf::Color(200, 200, 250), 800.f, 165.f);
    window.draw(title);

    drawDivider(window, 220.f, 222.f, 360.f, FRAME_DIV);

    sf::Text prompt = makeCenteredText(font, "Enter your name:", 22,
        sf::Color(160, 160, 200), 800.f, 280.f);
    window.draw(prompt);

    // Input box
    sf::RectangleShape box(sf::Vector2f(380.f, 52.f));
    box.setPosition(sf::Vector2f(210.f, 318.f));
    box.setFillColor(sf::Color(20, 20, 35));
    box.setOutlineColor(sf::Color(110, 110, 160));
    box.setOutlineThickness(2.f);
    window.draw(box);

    // Caret blinks
    bool caretOn = ((int)(blinkClock.getElapsedTime().asSeconds() / 0.5f) % 2 == 0);
    string display = currentName + (caretOn ? "_" : " ");

    sf::Text input(font, display, 28);
    input.setFillColor(TEXT_BRIGHT);
    input.setPosition(sf::Vector2f(225.f, 327.f));
    window.draw(input);

    sf::Text hint = makeCenteredText(font,
        "Type your name        ENTER to confirm        BACKSPACE to delete",
        15, TEXT_HINT, 800.f, 405.f);
    window.draw(hint);

    sf::Text esc = makeCenteredText(font,
        "ESC to go back",
        15, TEXT_HINT, 800.f, 475.f);
    window.draw(esc);
}

// ----------------------------------------------------------------
//  Continue Game — list of incomplete players
// ----------------------------------------------------------------
void drawContinueListScreen(sf::RenderWindow& window, const sf::Font& font,
    const ScoreBoard& sb, int selectedIndex)
{
    window.clear(BG_DARK);
    drawScreenFrame(window, FRAME_OUTER, FRAME_INNER, DOT_COLOR);

    sf::Text title = makeCenteredText(font, "CONTINUE GAME", 40,
        sf::Color(200, 200, 250), 800.f, 110.f);
    window.draw(title);

    drawDivider(window, 200.f, 158.f, 400.f, FRAME_DIV);

    int incompleteN = sb.countIncomplete();

    if (incompleteN == 0)
    {
        sf::Text none = makeCenteredText(font,
            "No saved games to continue.",
            22, TEXT_DIM, 800.f, 290.f);
        window.draw(none);

        sf::Text none2 = makeCenteredText(font,
            "Start a new game from the main menu.",
            17, TEXT_DIM, 800.f, 325.f);
        window.draw(none2);
    }
    else
    {
        sf::Text sub = makeCenteredText(font,
            "Pick a player to continue:",
            18, sf::Color(150, 150, 195), 800.f, 188.f);
        window.draw(sub);

        // Show up to 10 players (one row each).  If there are more,
        // we still cycle through them via W/S; only the visible window
        // is drawn.
        const int VISIBLE = 10;
        int firstRow = 0;
        if (selectedIndex >= VISIBLE) firstRow = selectedIndex - (VISIBLE - 1);

        const float ROW_H = 28.f;
        const float START_Y = 230.f;

        for (int row = 0; row < VISIBLE && (firstRow + row) < incompleteN; row++)
        {
            int n = firstRow + row;
            int pi = sb.getIncompleteIndex(n);
            if (pi < 0) break;
            const PlayerRecord& p = sb.get(pi);

            float y = START_Y + row * ROW_H;
            bool  isSel = (n == selectedIndex);
            sf::Color col = isSel ? TEXT_BRIGHT : sf::Color(135, 135, 175);

            // Highlight bar behind the selected row
            if (isSel)
            {
                sf::RectangleShape bar(sf::Vector2f(540.f, ROW_H - 4.f));
                bar.setPosition(sf::Vector2f(130.f, y - 2.f));
                bar.setFillColor(sf::Color(35, 35, 60));
                window.draw(bar);
            }

            sf::Text nameT(font, p.name, 22);
            nameT.setFillColor(col);
            nameT.setPosition(sf::Vector2f(160.f, y));
            window.draw(nameT);

            // Right-side info: "up to round X"
            string info = "Round " + std::to_string(p.nextRound());
            sf::Text infoT(font, info, 18);
            infoT.setFillColor(isSel ? sf::Color(180, 180, 230) : TEXT_DIM);
            sf::FloatRect lb = infoT.getLocalBounds();
            infoT.setPosition(sf::Vector2f(640.f - lb.size.x, y + 3.f));
            window.draw(infoT);

            if (isSel)
            {
                sf::Text arrow(font, ">", 22);
                arrow.setFillColor(sf::Color(180, 180, 255));
                arrow.setPosition(sf::Vector2f(135.f, y));
                window.draw(arrow);
            }
        }
    }

    sf::Text hint = makeCenteredText(font,
        "W / S  to move     J  to select     ESC  to back",
        15, TEXT_HINT, 800.f, 540.f);
    window.draw(hint);
}

// ----------------------------------------------------------------
//  Continue Player — confirm screen for the chosen player
// ----------------------------------------------------------------
void drawContinuePlayerScreen(sf::RenderWindow& window, const sf::Font& font,
    const PlayerRecord& player, int optionIndex)
{
    window.clear(BG_DARK);
    drawScreenFrame(window, FRAME_OUTER, FRAME_INNER, DOT_COLOR);

    sf::Text title = makeCenteredText(font, player.name, 44,
        TEXT_BRIGHT, 800.f, 105.f);
    window.draw(title);

    drawDivider(window, 250.f, 158.f, 300.f, FRAME_DIV);

    // Existing per-round scores
    sf::Text r1Lbl(font, "Round 1  -  The Creature", 19);
    r1Lbl.setFillColor(player.r1Done ? C_ACCENT : TEXT_DIM);
    r1Lbl.setPosition(sf::Vector2f(220.f, 195.f));
    window.draw(r1Lbl);

    sf::Text r1Val(font, player.r1Done ? std::to_string(player.r1Score) + " pts" : "—",
        19);
    r1Val.setFillColor(player.r1Done ? sf::Color(180, 220, 200) : TEXT_DIM);
    r1Val.setPosition(sf::Vector2f(530.f, 195.f));
    window.draw(r1Val);

    sf::Text r2Lbl(font, "Round 2  -  The Knight", 19);
    r2Lbl.setFillColor(player.r2Done ? K_ACCENT : TEXT_DIM);
    r2Lbl.setPosition(sf::Vector2f(220.f, 230.f));
    window.draw(r2Lbl);

    sf::Text r2Val(font, player.r2Done ? std::to_string(player.r2Score) + " pts" : "—",
        19);
    r2Val.setFillColor(player.r2Done ? sf::Color(200, 215, 230) : TEXT_DIM);
    r2Val.setPosition(sf::Vector2f(530.f, 230.f));
    window.draw(r2Val);

    sf::Text r3Lbl(font, "Round 3  -  The Samurai", 19);
    r3Lbl.setFillColor(player.r3Done ? S_ACCENT : TEXT_DIM);
    r3Lbl.setPosition(sf::Vector2f(220.f, 265.f));
    window.draw(r3Lbl);

    sf::Text r3Val(font, player.r3Done ? std::to_string(player.r3Score) + " pts" : "—",
        19);
    r3Val.setFillColor(player.r3Done ? sf::Color(230, 175, 145) : TEXT_DIM);
    r3Val.setPosition(sf::Vector2f(530.f, 265.f));
    window.draw(r3Val);

    drawDivider(window, 220.f, 305.f, 360.f, FRAME_DIV);

    sf::Text totalLbl(font, "Total so far:", 19);
    totalLbl.setFillColor(sf::Color(170, 170, 215));
    totalLbl.setPosition(sf::Vector2f(220.f, 320.f));
    window.draw(totalLbl);

    sf::Text totalVal(font, std::to_string(player.totalScore()) + " pts", 19);
    totalVal.setFillColor(TEXT_BRIGHT);
    totalVal.setPosition(sf::Vector2f(530.f, 320.f));
    window.draw(totalVal);

    drawDivider(window, 220.f, 365.f, 360.f, FRAME_DIV);

    // Option buttons
    int next = player.nextRound();   // 1, 2 or 3 (never 4 here)
    string startLbl = "RESUME ROUND " + std::to_string(next);

    sf::Color colStart = (optionIndex == 0) ? TEXT_BRIGHT : TEXT_DIM;
    sf::Color colBack = (optionIndex == 1) ? TEXT_BRIGHT : TEXT_DIM;

    sf::Text oStart = makeCenteredText(font, startLbl, 28, colStart, 800.f, 410.f);
    sf::Text oBack = makeCenteredText(font, "BACK", 24, colBack, 800.f, 460.f);
    window.draw(oStart);
    window.draw(oBack);

    sf::Text arrow(font, ">", 28);
    arrow.setFillColor(sf::Color(180, 180, 255));
    sf::FloatRect selBounds = (optionIndex == 0) ? oStart.getGlobalBounds()
        : oBack.getGlobalBounds();
    float arrowY = (optionIndex == 0) ? 393.f : 446.f;
    arrow.setPosition(sf::Vector2f(selBounds.position.x - 32.f, arrowY));
    window.draw(arrow);

    sf::Text hint = makeCenteredText(font,
        "W / S  to move     J  to select     ESC  to back",
        15, TEXT_HINT, 800.f, 545.f);
    window.draw(hint);
}

// ----------------------------------------------------------------
//  Scoreboard — list of every player with totals + status
// ----------------------------------------------------------------
void drawScoreboardScreen(sf::RenderWindow& window, const sf::Font& font,
    const ScoreBoard& sb, int optionIndex)
{
    window.clear(BG_DARK);
    drawScreenFrame(window, FRAME_OUTER, FRAME_INNER, DOT_COLOR);

    sf::Text title = makeCenteredText(font, "SCOREBOARD", 40,
        sf::Color(200, 200, 250), 800.f, 105.f);
    window.draw(title);

    drawDivider(window, 180.f, 152.f, 440.f, FRAME_DIV);

    // Column headers
    sf::Text h1(font, "Name", 16);
    h1.setFillColor(sf::Color(110, 110, 150));
    h1.setPosition(sf::Vector2f(125.f, 162.f));
    window.draw(h1);

    sf::Text h2(font, "Total", 16);
    h2.setFillColor(sf::Color(110, 110, 150));
    h2.setPosition(sf::Vector2f(420.f, 162.f));
    window.draw(h2);

    sf::Text h3(font, "Status", 16);
    h3.setFillColor(sf::Color(110, 110, 150));
    h3.setPosition(sf::Vector2f(540.f, 162.f));
    window.draw(h3);

    int n = sb.getCount();

    if (n == 0)
    {
        sf::Text none = makeCenteredText(font,
            "No scores yet.  Start a new game to record one.",
            18, TEXT_DIM, 800.f, 305.f);
        window.draw(none);
    }
    else
    {
        // Build a sorted view (descending by total score) without
        // mutating the underlying ScoreBoard.  This keeps player
        // indices stable for the rest of the game (e.g. continue
        // logic relies on sb.players[currentPlayerIdx] not moving).
        // Selection sort is more than fast enough for MAX_PLAYERS = 50.
        int order[MAX_PLAYERS];
        for (int i = 0; i < n; i++) order[i] = i;
        for (int i = 0; i < n - 1; i++)
        {
            int best = i;
            for (int j = i + 1; j < n; j++)
            {
                if (sb.get(order[j]).totalScore() > sb.get(order[best]).totalScore())
                    best = j;
            }
            if (best != i)
            {
                int tmp = order[i];
                order[i] = order[best];
                order[best] = tmp;
            }
        }

        const float ROW_H = 26.f;
        const float START_Y = 195.f;

        // Show up to 13 rows
        int show = (n > 13) ? 13 : n;
        for (int row = 0; row < show; row++)
        {
            const PlayerRecord& p = sb.get(order[row]);
            float y = START_Y + row * ROW_H;

            if (row % 2 == 0)
            {
                sf::RectangleShape rowBg(sf::Vector2f(540.f, ROW_H - 4.f));
                rowBg.setPosition(sf::Vector2f(125.f, y - 2.f));
                rowBg.setFillColor(sf::Color(20, 20, 35));
                window.draw(rowBg);
            }

            sf::Text nm(font, p.name, 18);
            nm.setFillColor(sf::Color(170, 185, 220));
            nm.setPosition(sf::Vector2f(135.f, y));
            window.draw(nm);

            sf::Text tot(font, std::to_string(p.totalScore()), 18);
            tot.setFillColor(TEXT_BRIGHT);
            tot.setPosition(sf::Vector2f(425.f, y));
            window.draw(tot);

            string status;
            sf::Color stCol;
            if (p.isComplete())
            {
                status = "Complete";
                stCol = A_PRIMARY;
            }
            else
            {
                status = "Round " + std::to_string(p.nextRound());
                stCol = sf::Color(140, 140, 175);
            }
            sf::Text st(font, status, 18);
            st.setFillColor(stCol);
            st.setPosition(sf::Vector2f(545.f, y));
            window.draw(st);
        }
    }

    // Bottom options
    drawDivider(window, 180.f, 488.f, 440.f, FRAME_DIV);

    sf::Color colBack = (optionIndex == 0) ? TEXT_BRIGHT : TEXT_DIM;
    sf::Color colReset = (optionIndex == 1) ? sf::Color(255, 100, 80)
        : sf::Color(120, 50, 40);

    sf::Text optBack = makeCenteredText(font, "BACK", 24, colBack, 800.f, 510.f);
    sf::Text optReset = makeCenteredText(font, "RESET SCORES", 24, colReset, 800.f, 540.f);
    window.draw(optBack);
    window.draw(optReset);

    sf::Text arrow(font, ">", 24);
    arrow.setFillColor(sf::Color(180, 180, 255));
    sf::FloatRect selB = (optionIndex == 0) ? optBack.getGlobalBounds()
        : optReset.getGlobalBounds();
    float arrowY = (optionIndex == 0) ? 496.f : 526.f;
    arrow.setPosition(sf::Vector2f(selB.position.x - 28.f, arrowY));
    window.draw(arrow);
}

// ----------------------------------------------------------------
//  Round intros and end screen (unchanged)
// ----------------------------------------------------------------
void drawIntroStage1Screen(sf::RenderWindow& window, const sf::Font& font)
{
    window.clear(C_BG);
    drawScreenFrame(window, C_PRIMARY, C_DIM, C_ACCENT);
    drawDivider(window, 190.f, 310.f, 420.f, C_DIM);
    sf::Text t1 = makeCenteredText(font, "ROUND ONE", 72, C_ACCENT, 800.f, 220.f);
    sf::Text t2 = makeCenteredText(font, "THE CREATURE", 42, C_PRIMARY, 800.f, 360.f);
    window.draw(t1); window.draw(t2);
}

void drawIntroStage2Screen(sf::RenderWindow& window, const sf::Font& font)
{
    window.clear(K_BG);
    drawScreenFrame(window, K_PRIMARY, K_DIM, K_ACCENT);
    drawDivider(window, 190.f, 310.f, 420.f, K_DIM);
    sf::Text t1 = makeCenteredText(font, "ROUND TWO", 72, K_ACCENT, 800.f, 220.f);
    sf::Text t2 = makeCenteredText(font, "THE KNIGHT", 42, K_PRIMARY, 800.f, 360.f);
    window.draw(t1); window.draw(t2);
}

void drawIntroStage3Screen(sf::RenderWindow& window, const sf::Font& font)
{
    window.clear(S_BG);
    drawScreenFrame(window, S_PRIMARY, S_DIM, S_ACCENT);
    drawDivider(window, 190.f, 310.f, 420.f, S_DIM);
    sf::Text t1 = makeCenteredText(font, "ROUND THREE", 72, S_ACCENT, 800.f, 220.f);
    sf::Text t2 = makeCenteredText(font, "THE SAMURAI", 42, S_PRIMARY, 800.f, 360.f);
    window.draw(t1); window.draw(t2);
}

void drawEndMenuScreen(sf::RenderWindow& window, const sf::Font& font, int menuIndex)
{
    window.clear(BG_DARK);
    drawScreenFrame(window, FRAME_OUTER, FRAME_INNER, DOT_COLOR);

    sf::Text title = makeCenteredText(font, "TOURNAMENT COMPLETE", 40,
        sf::Color(200, 200, 250), 800.f, 160.f);
    window.draw(title);

    drawDivider(window, 220.f, 222.f, 360.f, FRAME_DIV);

    sf::Color col0 = (menuIndex == 0) ? TEXT_BRIGHT : TEXT_DIM;
    sf::Color col1 = (menuIndex == 1) ? TEXT_BRIGHT : TEXT_DIM;

    sf::Text o0 = makeCenteredText(font, "START OVER", 34, col0, 800.f, 295.f);
    sf::Text o1 = makeCenteredText(font, "BACK TO MAIN MENU", 34, col1, 800.f, 365.f);
    window.draw(o0); window.draw(o1);

    sf::Text arrow(font, ">", 34);
    arrow.setFillColor(sf::Color(180, 180, 255));
    arrow.setPosition(sf::Vector2f(165.f, (menuIndex == 0) ? 278.f : 348.f));
    window.draw(arrow);

    sf::Text hint = makeCenteredText(font, "W / S  to move     J  to select", 18,
        TEXT_HINT, 800.f, 470.f);
    window.draw(hint);
}

// ============================================================
//  Fight HUD
// ============================================================
void drawHud(sf::RenderWindow& window, const sf::Font& font,
    int playerHp, int playerMaxHp,
    int enemyHp, int enemyMaxHp,
    int stageNum)
{
    sf::Text playerLbl(font, "Adventurer", 14);
    playerLbl.setFillColor(sf::Color(180, 210, 255));
    playerLbl.setPosition(sf::Vector2f(20.f, 18.f));
    window.draw(playerLbl);

    drawHealthBar(window, 20.f, 38.f, 250.f,
        playerHp, playerMaxHp, sf::Color(80, 150, 230));

    string    enemyName;
    sf::Color labelColor, barColor;

    if (stageNum == 3) { enemyName = "Samurai";  labelColor = S_ACCENT; barColor = S_PRIMARY; }
    else if (stageNum == 2) { enemyName = "Knight";   labelColor = K_ACCENT; barColor = K_PRIMARY; }
    else { enemyName = "Creature"; labelColor = C_ACCENT; barColor = C_PRIMARY; }

    sf::Text enemyLbl(font, enemyName, 14);
    enemyLbl.setFillColor(labelColor);
    sf::FloatRect lb = enemyLbl.getLocalBounds();
    enemyLbl.setPosition(sf::Vector2f(780.f - lb.size.x, 18.f));
    window.draw(enemyLbl);

    drawHealthBar(window, 530.f, 38.f, 250.f,
        enemyHp, enemyMaxHp, barColor);
}

void drawEscHint(sf::RenderWindow& window, const sf::Font& font)
{
    sf::Text escHint(font, "ESC: Menu", 13);
    escHint.setFillColor(sf::Color(55, 55, 80));
    escHint.setPosition(sf::Vector2f(350.f, 20.f));
    window.draw(escHint);
}

// ============================================================
//  End-screen overlays (unchanged)
// ============================================================
void drawYouWinOverlay(sf::RenderWindow& window, const sf::Font& font)
{
    sf::Color outerFill(A_BG.r, A_BG.g, A_BG.b, 220);
    drawScreenFrame(window, A_PRIMARY, A_BLUE, A_PRIMARY, outerFill);
    drawDivider(window, 190.f, 330.f, 420.f, A_BLUE);
    sf::Text t1 = makeCenteredText(font, "YOU WIN", 72, A_ACCENT, 800.f, 240.f);
    sf::Text t2 = makeCenteredText(font, "Adventurer Victorious", 42, A_PRIMARY, 800.f, 370.f);
    window.draw(t1); window.draw(t2);
}

void drawYouWinStage2Overlay(sf::RenderWindow& window, const sf::Font& font)
{
    sf::Color outerFill(K_BG.r, K_BG.g, K_BG.b, 220);
    drawScreenFrame(window, K_ACCENT, K_DIM, K_ACCENT, outerFill);
    drawDivider(window, 190.f, 330.f, 420.f, K_DIM);
    sf::Text t1 = makeCenteredText(font, "ROUND 2 CLEAR!", 64, K_ACCENT, 800.f, 240.f);
    sf::Text t2 = makeCenteredText(font, "Onward, Warrior!", 42, K_PRIMARY, 800.f, 370.f);
    window.draw(t1); window.draw(t2);
}

void drawYouWinStage3Overlay(sf::RenderWindow& window, const sf::Font& font)
{
    sf::Color outerFill(S_BG.r, S_BG.g, S_BG.b, 220);
    drawScreenFrame(window, S_ACCENT, S_DIM, S_ACCENT, outerFill);
    drawDivider(window, 190.f, 330.f, 420.f, S_DIM);
    sf::Text t1 = makeCenteredText(font, "CHAMPION!", 72, S_ACCENT, 800.f, 240.f);
    sf::Text t2 = makeCenteredText(font, "Tournament Complete", 42, S_PRIMARY, 800.f, 370.f);
    window.draw(t1); window.draw(t2);
}

void drawYouLoseOverlay(sf::RenderWindow& window, const sf::Font& font)
{
    sf::Color outerFill(C_BG.r, C_BG.g, C_BG.b, 220);
    drawScreenFrame(window, C_PRIMARY, C_DIM, C_PRIMARY, outerFill);
    drawDivider(window, 190.f, 330.f, 420.f, C_DIM);
    sf::Text t1 = makeCenteredText(font, "YOU LOSE", 72, C_ACCENT, 800.f, 240.f);
    sf::Text t2 = makeCenteredText(font, "The Enemy Wins", 42, C_PRIMARY, 800.f, 370.f);
    window.draw(t1); window.draw(t2);
}
