#include "GameScreen.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

//  CONSTRUCTOR
//  takes playerName as well
//  ScoreManager is constructed automatically (it loads scores.txt in its own constructor)
GameScreen::GameScreen(GameWindow& gw,
    const string& name, int level, int startScore, int startLives)
    : gameWindow(gw),
    playerName(name),
    paddle(800.f, 600.f),
    currentLevel(level),
    score(startScore),
    lives(startLives),
    bgSprite(bgTexture),
    bgLoaded(false),
    hitSound(hitBuffer),
    gameOverSound(gameOverBuffer),
    levelDoneSound(levelDoneBuffer)
{
    if (!font.openFromFile("assets/shatterassets/arial.ttf"))
        cout << "ERROR: Could not load font!\n";
    // ScoreManager constructor already loaded scores.txt
    loadBackground();
    loadLevel();
    applyBallSpeedMultiplier();
    resetBall();
    loadSounds();
}

//  DESTRUCTOR — frees all brick memory
GameScreen::~GameScreen()
{
    bgMusic.stop();
    for (Brick* b : bricks)
        delete b;
    bricks.clear();
}

//  LOAD SOUNDS AND MUSIC
void GameScreen::loadSounds()
{
    if (!hitBuffer.loadFromFile("assets/shatterassets/hit.wav"))
        cout << "WARNING: hit.wav not found!\n";
    else
        hitSound.setBuffer(hitBuffer);
    if (!gameOverBuffer.loadFromFile("assets/shatterassets/gameover.mp3"))
        cout << "WARNING: gameover.mp3 not found!\n";
    else
        gameOverSound.setBuffer(gameOverBuffer);
    if (!levelDoneBuffer.loadFromFile("assets/shatterassets/completelevel.wav"))
        cout << "WARNING: completelevel.wav not found!\n";
    else
        levelDoneSound.setBuffer(levelDoneBuffer);
    if (!bgMusic.openFromFile("assets/shatterassets/background-music.mp3"))
        cout << "WARNING: background music not found!\n";
    else
    {
        bgMusic.setLooping(true);
        bgMusic.setVolume(40.f);
        bgMusic.play();
    }
}

//  LOAD BACKGROUND IMAGE (per level)
void GameScreen::loadBackground()
{
    string filename = "assets/shatterassets/bg_level" + to_string(currentLevel) + ".jpg";
    if (!bgTexture.loadFromFile(filename))
    {
        cout << "WARNING: " << filename << " not found.\n";
        bgLoaded = false;
        return;
    }
    sf::Vector2u s = bgTexture.getSize();
    bgSprite.setTexture(bgTexture, true);
    bgSprite.setScale({ 800.f / (float)s.x, 600.f / (float)s.y });
    bgSprite.setPosition({ 0.f, 0.f });
    bgLoaded = true;
}
//  LOAD LEVEL FROM FILE
void GameScreen::loadLevel()
{
    for (Brick* b : bricks) delete b;
    bricks.clear();
    string filename = "assets/shatterassets/level" + to_string(currentLevel) + ".txt";
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "WARNING: " << filename << " not found. Using fallback.\n";
        generateFallbackLevel();
        return;
    }
    float brickW = 68.f, brickH = 22.f, gap = 6.f;
    float startX = 35.f, startY = 60.f;
    string line;
    int row = 0;
    while (getline(file, line) && row < 10)
    {
        istringstream iss(line);
        int brickType, col = 0;
        while (iss >> brickType && col < 10)
        {
            float x = startX + col * (brickW + gap);
            float y = startY + row * (brickH + gap);

            if (brickType == 1)
                bricks.push_back(new Brick(x, y, brickW, brickH, 1, 10, sf::Color(120, 80, 200)));
            else if (brickType == 2)
                bricks.push_back(new ArmoredBrick(x, y, brickW, brickH));
            else if (brickType == 3)
                bricks.push_back(new ExplosiveBrick(x, y, brickW, brickH));
            else if (brickType == 4)
                bricks.push_back(new MovingBrick(x, y, brickW, brickH, 0.f, 800.f));
            col++;
        }
        row++;
    }
    file.close();
    std::cout << "Loaded: " << filename << " | Bricks: " << bricks.size() << "\n";
}
//  FALLBACK LEVEL (if file missing)
void GameScreen::generateFallbackLevel()
{
    float brickW = 68.f, brickH = 22.f, gap = 6.f;
    float startX = 35.f, startY = 60.f;
    for (int row = 0; row < 3; row++)
        for (int col = 0; col < 10; col++)
            bricks.push_back(new Brick(startX + col * (brickW + gap),
                startY + row * (brickH + gap),
                brickW, brickH, 1, 10, sf::Color(120, 80, 200)));
}

//  BALL SPEED — increases each level
void GameScreen::applyBallSpeedMultiplier()
{
    float multiplier = 1.0f + (currentLevel - 1) * 0.4f;
    ball.multiplyVelocity(multiplier);
}

//  RESET BALL — place on paddle
void GameScreen::resetBall()
{
    float px = paddle.getCenterX();
    float py = 600.f - 40.f - ball.getRadius() - 10.f;
    ball.reset(px, py);
}

//  HANDLE COLLISIONS
void GameScreen::handleCollisions()
{
    // Ball vs Paddle
    if (ball.getBounds().findIntersection(paddle.getBounds()))
        ball.bounceOffPaddle(paddle.getCenterX());

    // Ball vs Bricks (Polymorphism: b->hit() calls correct override)
    for (Brick* b : bricks)
    {
        if (b->destroyed) continue;
        if (ball.getBounds().findIntersection(b->getBounds()))
        {
            if (b->hit())           // virtual function call
                score += b->points;
            hitSound.play();
            ball.bounceY();
            break;
        }
    }
    // Remove destroyed bricks
    for (int i = (int)bricks.size() - 1; i >= 0; i--)
    {
        if (bricks[i]->destroyed)
        {
            delete bricks[i];
            bricks.erase(bricks.begin() + i);
        }
    }
}

//  DRAW HUD it Shows player name, score, level, lives and current high score from file
void GameScreen::drawHUD()
{
    // Player name (top left)
    sf::Text nameText(font);
    nameText.setString(playerName);
    nameText.setCharacterSize(18);
    nameText.setFillColor(sf::Color(180, 140, 255));
    nameText.setPosition({ 10.f, 10.f });
    gameWindow.getWindow().draw(nameText);

    // Score (below name)
    sf::Text scoreText(font);
    scoreText.setString("Score: " + to_string(score));
    scoreText.setCharacterSize(18);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition({ 10.f, 32.f });
    gameWindow.getWindow().draw(scoreText);

    // Level (center top)
    sf::Text levelText(font);
    levelText.setString("Level " + to_string(currentLevel) + " / 5");
    levelText.setCharacterSize(20);
    levelText.setFillColor(sf::Color(255, 220, 100));
    sf::FloatRect lb = levelText.getLocalBounds();
    levelText.setOrigin({ lb.size.x / 2.f, 0.f });
    levelText.setPosition({ 400.f, 10.f });
    gameWindow.getWindow().draw(levelText);

    // Lives (top right)
    sf::Text livesText(font);
    livesText.setString("Lives: " + to_string(lives));
    livesText.setCharacterSize(18);
    livesText.setFillColor(sf::Color(255, 150, 150));
    livesText.setPosition({ 650.f, 10.f });
    gameWindow.getWindow().draw(livesText);

    // Best Score from file (top right below lives)
    sf::Text bestText(font);
    bestText.setString("Best: " + to_string(scoreManager.getHighestScore()));
    bestText.setCharacterSize(16);
    bestText.setFillColor(sf::Color(255, 215, 0));
    bestText.setPosition({ 650.f, 32.f });
    gameWindow.getWindow().draw(bestText);
}

//  CHECK WIN
bool GameScreen::allBricksDestroyed()
{
    return bricks.empty();
}

//  LEVEL COMPLETE SCREEN
bool GameScreen::showNextLevelPrompt()
{
    sf::RectangleShape box;
    box.setSize({ 500.f, 220.f });
    box.setOrigin({ 250.f, 110.f });
    box.setPosition({ 400.f, 300.f });
    box.setFillColor(sf::Color(30, 20, 60, 230));
    box.setOutlineColor(sf::Color(180, 140, 255));
    box.setOutlineThickness(2.f);

    sf::Text title(font);
    title.setString("Level " + to_string(currentLevel) + " Complete!");
    title.setCharacterSize(34);
    title.setFillColor(sf::Color(180, 140, 255));
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin({ tb.size.x / 2.f, tb.size.y / 2.f });
    title.setPosition({ 400.f, 225.f });

    sf::Text playerInfo(font);
    playerInfo.setString(playerName + "   |   Score: " + to_string(score));
    playerInfo.setCharacterSize(20);
    playerInfo.setFillColor(sf::Color(255, 220, 100));
    sf::FloatRect pb = playerInfo.getLocalBounds();
    playerInfo.setOrigin({ pb.size.x / 2.f, pb.size.y / 2.f });
    playerInfo.setPosition({ 400.f, 280.f });

    sf::Text prompt(font);
    prompt.setString("Press  Y  to play Next Level\nPress  N  to return to Menu");
    prompt.setCharacterSize(22);
    prompt.setFillColor(sf::Color::White);
    sf::FloatRect pob = prompt.getLocalBounds();
    prompt.setOrigin({ pob.size.x / 2.f, pob.size.y / 2.f });
    prompt.setPosition({ 400.f, 340.f });

    while (gameWindow.isOpen())
    {
        while (const std::optional event = gameWindow.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                gameWindow.getWindow().close();
                return false;
            }
            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Y)
                    return true;
                if (key->code == sf::Keyboard::Key::N)
                    return false;
            }
        }
        gameWindow.clear();
        if (bgLoaded) gameWindow.getWindow().draw(bgSprite);
        gameWindow.getWindow().draw(box);
        gameWindow.getWindow().draw(title);
        gameWindow.getWindow().draw(playerInfo);
        gameWindow.getWindow().draw(prompt);
        gameWindow.display();
    }
    return false;
}

//  GAME OVER SCREEN Saves score via ScoreManager and top 5 player scores
void GameScreen::showGameOverScreen()
{
    scoreManager.addScore(playerName, currentLevel, score);
    // Game Over background image
    sf::Texture goTexture;
    sf::Sprite  goSprite(goTexture);
    bool goImgLoaded = false;
    if (goTexture.loadFromFile("assets/shatterassets/gameover.jpg"))
    {
        sf::Vector2u s = goTexture.getSize();
        goSprite.setTexture(goTexture, true);
        goSprite.setScale({ 800.f / (float)s.x, 600.f / (float)s.y });
        goSprite.setPosition({ 0.f, 0.f });
        goImgLoaded = true;
    }
    else
        cout << "WARNING: assets/gameover.jpg not found.\n";

    // ── Fixed X positions for each column ─────
    // These are absolute window X coordinates
    const float COL_RANK = 130.f;
    const float COL_NAME = 240.f;   // pushed right to give space after Rank
    const float COL_LEVEL = 460.f;
    const float COL_SCORE = 560.f;

    // Background box 
    sf::RectangleShape box;
    box.setSize({ 580.f, 470.f });
    box.setOrigin({ 290.f, 235.f });
    box.setPosition({ 400.f, 300.f });
    box.setFillColor(sf::Color(20, 10, 40, 245));
    box.setOutlineColor(sf::Color(200, 60, 60));
    box.setOutlineThickness(2.5f);

    //GAME OVER title
    sf::Text title(font);
    title.setString("GAME OVER!");
    title.setCharacterSize(46);
    title.setFillColor(sf::Color(220, 60, 60));
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin({ tb.size.x / 2.f, tb.size.y / 2.f });
    title.setPosition({ 400.f, 88.f });

    //  Player result line (centered)
    sf::Text resultText(font);
    resultText.setString(playerName + "   |   Level " + to_string(currentLevel)
        + "   |   Score: " + to_string(score));
    resultText.setCharacterSize(19);
    resultText.setFillColor(sf::Color(255, 220, 100));
    sf::FloatRect rb = resultText.getLocalBounds();
    resultText.setOrigin({ rb.size.x / 2.f, rb.size.y / 2.f });
    resultText.setPosition({ 400.f, 142.f });

    // Leaderboard heading
    sf::Text lbTitle(font);
    lbTitle.setString("-- TOP 5 HIGH SCORES --");
    lbTitle.setCharacterSize(20);
    lbTitle.setFillColor(sf::Color(180, 140, 255));
    sf::FloatRect ltb = lbTitle.getLocalBounds();
    lbTitle.setOrigin({ ltb.size.x / 2.f, ltb.size.y / 2.f });
    lbTitle.setPosition({ 400.f, 180.f });

    // Divider line under heading
    sf::RectangleShape divider1;
    divider1.setSize({ 500.f, 1.f });
    divider1.setOrigin({ 250.f, 0.f });
    divider1.setPosition({ 400.f, 200.f });
    divider1.setFillColor(sf::Color(100, 80, 160));

    // ── Column header texts (each at fixed X) ─
    sf::Text hRank(font);  hRank.setString("Rank");
    sf::Text hName(font);  hName.setString("Name");
    sf::Text hLevel(font); hLevel.setString("Level");
    sf::Text hScore(font); hScore.setString("Score");

    for (auto* h : { &hRank, &hName, &hLevel, &hScore })
    {
        h->setCharacterSize(17);
        h->setFillColor(sf::Color(160, 160, 210));
        h->setStyle(sf::Text::Bold);
    }
    hRank.setPosition({ COL_RANK,  210.f });
    hName.setPosition({ COL_NAME,  210.f });
    hLevel.setPosition({ COL_LEVEL, 210.f });
    hScore.setPosition({ COL_SCORE, 210.f });

    // Divider line under headers
    sf::RectangleShape divider2;
    divider2.setSize({ 500.f, 1.f });
    divider2.setOrigin({ 250.f, 0.f });
    divider2.setPosition({ 400.f, 233.f });
    divider2.setFillColor(sf::Color(100, 80, 160));

    // Build leaderboard rows
    // Each row = 4 separate sf::Text objects so
    // each column sits at its own fixed X position
    struct TableRow
    {
        sf::Text rank, name, level, score;
        TableRow(const sf::Font& f) : rank(f), name(f), level(f), score(f) {}
    };

    const auto& entries = scoreManager.getEntries();
    std::vector<TableRow> rows;
    rows.reserve(5);
    for (int i = 0; i < 5; i++)
    {
        rows.emplace_back(font);
        TableRow& r = rows.back();
        // Decide colour: gold = current player, white = others
        bool isCurrentPlayer = (i < (int)entries.size()
            && entries[i].name == playerName
            && entries[i].score == score);
        sf::Color rowColor = isCurrentPlayer
            ? sf::Color(255, 215, 0) : sf::Color(230, 230, 230);
        float y = 243.f + i * 30.f;

        // Rank
        r.rank.setString(std::to_string(i + 1) + ".");
        r.rank.setCharacterSize(17);
        r.rank.setFillColor(rowColor);
        r.rank.setPosition({ COL_RANK, y });

        if (i < (int)entries.size())
        {
            r.name.setString(entries[i].name);
            r.level.setString(std::to_string(entries[i].level));
            r.score.setString(std::to_string(entries[i].score));
        }
        else
        {
            r.name.setString("---");
            r.level.setString("-");
            r.score.setString("---");
        }
        for (auto* t : { &r.name, &r.level, &r.score })
        {
            t->setCharacterSize(17);
            t->setFillColor(rowColor);
        }
        r.name.setPosition({ COL_NAME,  y });
        r.level.setPosition({ COL_LEVEL, y });
        r.score.setPosition({ COL_SCORE, y });
    }

    // Divider line after table rows 
    sf::RectangleShape divider3;
    divider3.setSize({ 500.f, 1.f });
    divider3.setOrigin({ 250.f, 0.f });
    divider3.setPosition({ 400.f, 398.f });
    divider3.setFillColor(sf::Color(100, 80, 160));

    // Quit hint
    sf::Text quit(font);
    quit.setString("Press  ESC  to return to Menu");
    quit.setCharacterSize(18);
    quit.setFillColor(sf::Color(140, 140, 160));
    sf::FloatRect qb = quit.getLocalBounds();
    quit.setOrigin({ qb.size.x / 2.f, qb.size.y / 2.f });
    quit.setPosition({ 400.f, 490.f });

    //Event loop
    bool returnToMenu = false;
    while (gameWindow.isOpen() && !returnToMenu)
    {
        while (const std::optional event = gameWindow.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                gameWindow.getWindow().close();
            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
                if (key->code == sf::Keyboard::Key::Escape)
                    returnToMenu = true;
        }
        gameWindow.clear();
        // Draw background image first (behind everything)
        if (goImgLoaded)
            gameWindow.getWindow().draw(goSprite);
        gameWindow.getWindow().draw(box);
        gameWindow.getWindow().draw(title);
        gameWindow.getWindow().draw(resultText);
        gameWindow.getWindow().draw(lbTitle);
        gameWindow.getWindow().draw(divider1);
        gameWindow.getWindow().draw(hRank);
        gameWindow.getWindow().draw(hName);
        gameWindow.getWindow().draw(hLevel);
        gameWindow.getWindow().draw(hScore);
        gameWindow.getWindow().draw(divider2);
        for (auto& r : rows)
        {
            gameWindow.getWindow().draw(r.rank);
            gameWindow.getWindow().draw(r.name);
            gameWindow.getWindow().draw(r.level);
            gameWindow.getWindow().draw(r.score);
        }
        gameWindow.getWindow().draw(divider3);
        gameWindow.getWindow().draw(quit);
        gameWindow.display();
    }
}

//  FINAL SCREEN  (after completing Level 5)
//  Saves score, shows leaderboard + gold stars
void GameScreen::showFinalScreen()
{
    scoreManager.addScore(playerName, currentLevel, score);
    // ── Fixed X positions (same as Game Over) 
    const float COL_RANK = 130.f;
    const float COL_NAME = 240.f;   // pushed right to give space after Rank
    const float COL_LEVEL = 460.f;
    const float COL_SCORE = 560.f;

    sf::Text title(font);
    title.setString("SHATTER MASTER!");
    title.setCharacterSize(50);
    title.setFillColor(sf::Color(255, 215, 0));
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin({ tb.size.x / 2.f, tb.size.y / 2.f });
    title.setPosition({ 400.f, 60.f });

    sf::Text sub(font);
    sub.setString("You conquered all 5 levels!");
    sub.setCharacterSize(21);
    sub.setFillColor(sf::Color(180, 140, 255));
    sf::FloatRect sb = sub.getLocalBounds();
    sub.setOrigin({ sb.size.x / 2.f, sb.size.y / 2.f });
    sub.setPosition({ 400.f, 112.f });

    sf::Text resultText(font);
    resultText.setString(playerName + "   |   Score: " + to_string(score));
    resultText.setCharacterSize(20);
    resultText.setFillColor(sf::Color::White);
    sf::FloatRect rt = resultText.getLocalBounds();
    resultText.setOrigin({ rt.size.x / 2.f, rt.size.y / 2.f });
    resultText.setPosition({ 400.f, 150.f });

    // Gold stars
    std::vector<sf::CircleShape> stars;
    for (int i = 0; i < 5; i++)
    {
        sf::CircleShape star(12.f, 5);
        star.setFillColor(sf::Color(255, 215, 0));
        star.setOrigin({ 12.f, 12.f });
        star.setPosition({ 200.f + i * 100.f, 185.f });
        stars.push_back(star);
    }

    // Leaderboard heading
    sf::Text lbTitle(font);
    lbTitle.setString("-- TOP 5 HIGH SCORES --");
    lbTitle.setCharacterSize(19);
    lbTitle.setFillColor(sf::Color(180, 140, 255));
    sf::FloatRect ltb = lbTitle.getLocalBounds();
    lbTitle.setOrigin({ ltb.size.x / 2.f, ltb.size.y / 2.f });
    lbTitle.setPosition({ 400.f, 218.f });

    // Divider under heading
    sf::RectangleShape div1;
    div1.setSize({ 500.f, 1.f });
    div1.setOrigin({ 250.f, 0.f });
    div1.setPosition({ 400.f, 234.f });
    div1.setFillColor(sf::Color(100, 80, 160));

    // ── Column headers (each as separate Text) ─
    sf::Text hRank(font);  hRank.setString("Rank");
    sf::Text hName(font);  hName.setString("Name");
    sf::Text hLevel(font); hLevel.setString("Level");
    sf::Text hScore(font); hScore.setString("Score");

    for (auto* h : { &hRank, &hName, &hLevel, &hScore })
    {
        h->setCharacterSize(16);
        h->setFillColor(sf::Color(160, 160, 210));
        h->setStyle(sf::Text::Bold);
    }
    hRank.setPosition({ COL_RANK,  242.f });
    hName.setPosition({ COL_NAME,  242.f });
    hLevel.setPosition({ COL_LEVEL, 242.f });
    hScore.setPosition({ COL_SCORE, 242.f });

    // Divider under column headers
    sf::RectangleShape div2;
    div2.setSize({ 500.f, 1.f });
    div2.setOrigin({ 250.f, 0.f });
    div2.setPosition({ 400.f, 263.f });
    div2.setFillColor(sf::Color(100, 80, 160));

    // Leaderboard rows (4 separate Texts each) ─
    struct TableRow
    {
        sf::Text rank, name, level, score;
        TableRow(const sf::Font& f) : rank(f), name(f), level(f), score(f) {}
    };

    const auto& entries = scoreManager.getEntries();
    std::vector<TableRow> rows;
    rows.reserve(5);
    for (int i = 0; i < 5; i++)
    {
        rows.emplace_back(font);
        TableRow& r = rows.back();
        bool isCurrentPlayer = (i < (int)entries.size()
            && entries[i].name == playerName
            && entries[i].score == score);
        sf::Color rowColor = isCurrentPlayer
            ? sf::Color(255, 215, 0) : sf::Color(230, 230, 230);
        float y = 271.f + i * 28.f;
        r.rank.setString(to_string(i + 1) + ".");
        if (i < (int)entries.size())
        {
            r.name.setString(entries[i].name);
            r.level.setString(to_string(entries[i].level));
            r.score.setString(to_string(entries[i].score));
        }
        else
        {
            r.name.setString("---");
            r.level.setString("-");
            r.score.setString("---");
        }

        for (auto* t : { &r.rank, &r.name, &r.level, &r.score })
        {
            t->setCharacterSize(16);
            t->setFillColor(rowColor);
        }
        r.rank.setPosition({ COL_RANK,  y });
        r.name.setPosition({ COL_NAME,  y });
        r.level.setPosition({ COL_LEVEL, y });
        r.score.setPosition({ COL_SCORE, y });
    }

    // Divider after rows
    sf::RectangleShape div3;
    div3.setSize({ 500.f, 1.f });
    div3.setOrigin({ 250.f, 0.f });
    div3.setPosition({ 400.f, 413.f });
    div3.setFillColor(sf::Color(100, 80, 160));

    sf::Text quit(font);
    quit.setString("Press  ESC  to return to Menu");
    quit.setCharacterSize(18);
    quit.setFillColor(sf::Color(140, 140, 140));
    sf::FloatRect qb = quit.getLocalBounds();
    quit.setOrigin({ qb.size.x / 2.f, qb.size.y / 2.f });
    quit.setPosition({ 400.f, 560.f });

    bool returnToMenu = false;
    while (gameWindow.isOpen() && !returnToMenu)
    {
        while (const std::optional event = gameWindow.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                gameWindow.getWindow().close();
            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
                if (key->code == sf::Keyboard::Key::Escape)
                    returnToMenu = true;
        }
        gameWindow.clear();
        gameWindow.getWindow().draw(title);
        gameWindow.getWindow().draw(sub);
        gameWindow.getWindow().draw(resultText);
        for (auto& s : stars) gameWindow.getWindow().draw(s);
        gameWindow.getWindow().draw(lbTitle);
        gameWindow.getWindow().draw(div1);
        gameWindow.getWindow().draw(hRank);
        gameWindow.getWindow().draw(hName);
        gameWindow.getWindow().draw(hLevel);
        gameWindow.getWindow().draw(hScore);
        gameWindow.getWindow().draw(div2);
        for (auto& r : rows)
        {
            gameWindow.getWindow().draw(r.rank);
            gameWindow.getWindow().draw(r.name);
            gameWindow.getWindow().draw(r.level);
            gameWindow.getWindow().draw(r.score);
        }
        gameWindow.getWindow().draw(div3);
        gameWindow.getWindow().draw(quit);
        gameWindow.display();
    }
}

//  MAIN GAME LOOP
bool GameScreen::run()
{
    sf::Clock clock;
    while (gameWindow.isOpen())
    {
        float dt = clock.restart().asSeconds();
        while (const optional event = gameWindow.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                gameWindow.getWindow().close();
            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                    return false;
                if (key->code == sf::Keyboard::Key::Space && !ball.isActive())
                    ball.launch();
            }
        }
        paddle.update(dt);
        if (!ball.isActive())
            ball.setPosition(paddle.getCenterX(), 600.f - 40.f - ball.getRadius() - 10.f);
        ball.update(dt, 800.f, 600.f);
        // Polymorphism: each brick type updates differently
        for (Brick* b : bricks)
            b->update(dt);
        handleCollisions();
        // Ball fell below screen
        if (ball.isOutOfBounds(600.f))
        {
            lives--;
            if (lives <= 0)
            {
                bgMusic.stop();
                gameOverSound.play();
                sf::sleep(sf::seconds(1.5f));
                showGameOverScreen();    // saves score inside
                return false;
            }
            else
                resetBall();
        }
        // Level complete?
        if (allBricksDestroyed())
        {
            bgMusic.stop();
            levelDoneSound.play();
            sf::sleep(sf::seconds(1.2f));
            if (currentLevel == 5)
            {
                showFinalScreen();
                return false;
            }
            else
            {
                return showNextLevelPrompt();
            }
        }
        // Draw
        gameWindow.clear();
        if (bgLoaded)
            gameWindow.getWindow().draw(bgSprite);
        for (Brick* b : bricks)
            gameWindow.getWindow().draw(b->shape);
        paddle.draw(gameWindow.getWindow());
        ball.draw(gameWindow.getWindow());
        drawHUD();
        gameWindow.display();
    }
    return false;
}