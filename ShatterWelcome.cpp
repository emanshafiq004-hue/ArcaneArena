#include "ShatterWelcome.h"
#include <iostream>
#include <stdexcept>
using namespace std;
//  CONSTRUCTOR
ShatterWelcomeScreen::ShatterWelcomeScreen(GameWindow& gw)
    : gameWindow(gw),
    titleText(font),
    startText(font),
    exitText(font),
    dialogTitle(font),
    nameDisplay(font),
    hintText(font),
    warningText(font),
    bgTexture(),
    bgSprite(bgTexture),
    playerName(""),
    showNameDialog(false),
    showWarning(false)
{
    if (!font.openFromFile("assets/shatterassets/arial.ttf"))
        cout << "ERROR: Could not load font!\n";

    if (!bgTexture.loadFromFile("assets/shatterassets/background.jpg"))
        cout << "ERROR: Could not load background!\n";

    bgSprite = sf::Sprite(bgTexture);
    sf::Vector2u size = bgTexture.getSize();
    if (size.x > 0 && size.y > 0)
        bgSprite.setScale({ 800.f / static_cast<float>(size.x),
                            600.f / static_cast<float>(size.y) });
    setupMainScreen();
    setupNameDialog();
}

//  SETUP MAIN SCREEN, Three buttons: Start Game, Scoreboard, Exit
void ShatterWelcomeScreen::setupMainScreen()
{
    // Title
    titleText.setString("SHATTER SHIFT");
    titleText.setCharacterSize(64);
    titleText.setFillColor(sf::Color(180, 140, 255));
    titleText.setStyle(sf::Text::Bold);
    sf::FloatRect tb = titleText.getLocalBounds();
    titleText.setOrigin({ tb.size.x / 2.f, tb.size.y / 2.f });
    titleText.setPosition({ 400.f, 140.f });

    // Start Button
    startButton.setSize({ 240.f, 58.f });
    startButton.setOrigin({ 120.f, 29.f });
    startButton.setPosition({ 400.f, 290.f });
    startButton.setFillColor(sf::Color(70, 50, 150));
    startButton.setOutlineColor(sf::Color(180, 140, 255));
    startButton.setOutlineThickness(2.5f);
    startText.setString("Start Game");
    startText.setCharacterSize(23);
    startText.setFillColor(sf::Color::White);
    sf::FloatRect sb = startText.getLocalBounds();
    startText.setOrigin({ sb.size.x / 2.f, sb.size.y / 2.f });
    startText.setPosition({ 400.f, 288.f });

    // Exit Button
    exitButton.setSize({ 240.f, 58.f });
    exitButton.setOrigin({ 120.f, 29.f });
    exitButton.setPosition({ 400.f, 400.f });
    exitButton.setFillColor(sf::Color(130, 30, 30));
    exitButton.setOutlineColor(sf::Color(255, 100, 100));
    exitButton.setOutlineThickness(2.5f);
    exitText.setString("Exit");
    exitText.setCharacterSize(26);
    exitText.setFillColor(sf::Color::White);
    sf::FloatRect eb = exitText.getLocalBounds();
    exitText.setOrigin({ eb.size.x / 2.f, eb.size.y / 2.f });
    exitText.setPosition({ 400.f, 400.f });
}

//  SETUP NAME DIALOG
void ShatterWelcomeScreen::setupNameDialog()
{
    overlayDim.setSize({ 800.f, 600.f });
    overlayDim.setPosition({ 0.f, 0.f });
    overlayDim.setFillColor(sf::Color(0, 0, 0, 160));
    overlayBox.setSize({ 460.f, 260.f });
    overlayBox.setOrigin({ 230.f, 130.f });
    overlayBox.setPosition({ 400.f, 300.f });
    overlayBox.setFillColor(sf::Color(22, 14, 48, 245));
    overlayBox.setOutlineColor(sf::Color(180, 140, 255));
    overlayBox.setOutlineThickness(3.f);

    dialogTitle.setString("Enter Your Name");
    dialogTitle.setCharacterSize(30);
    dialogTitle.setFillColor(sf::Color(200, 170, 255));
    dialogTitle.setStyle(sf::Text::Bold);
    sf::FloatRect dt = dialogTitle.getLocalBounds();
    dialogTitle.setOrigin({ dt.size.x / 2.f, dt.size.y / 2.f });
    dialogTitle.setPosition({ 400.f, 210.f });

    nameBox.setSize({ 340.f, 50.f });
    nameBox.setOrigin({ 170.f, 25.f });
    nameBox.setPosition({ 400.f, 290.f });
    nameBox.setFillColor(sf::Color(35, 22, 65));
    nameBox.setOutlineColor(sf::Color(140, 100, 240));
    nameBox.setOutlineThickness(2.f);

    nameDisplay.setString("");
    nameDisplay.setCharacterSize(22);
    nameDisplay.setFillColor(sf::Color::White);
    nameDisplay.setPosition({ 242.f, 278.f });

    hintText.setString("Press  Enter  to start");
    hintText.setCharacterSize(17);
    hintText.setFillColor(sf::Color(150, 150, 200));
    sf::FloatRect ht = hintText.getLocalBounds();
    hintText.setOrigin({ ht.size.x / 2.f, ht.size.y / 2.f });
    hintText.setPosition({ 400.f, 345.f });

    warningText.setString("Please enter your name first!");
    warningText.setCharacterSize(17);
    warningText.setFillColor(sf::Color(255, 80, 80));
    sf::FloatRect wt = warningText.getLocalBounds();
    warningText.setOrigin({ wt.size.x / 2.f, wt.size.y / 2.f });
    warningText.setPosition({ 400.f, 375.f });
}

// click detection
bool ShatterWelcomeScreen::isClicked(const sf::RectangleShape& btn, sf::Vector2i mousePos)
{
    return btn.getGlobalBounds().contains(sf::Vector2f(mousePos));
}

//  GETTER
string ShatterWelcomeScreen::getPlayerName() const
{
    return playerName.empty() ? "Player" : playerName;
}
//  MAIN LOOP
bool ShatterWelcomeScreen::run()
{
    while (gameWindow.isOpen())
    {
        // Event handling
        while (const auto event = gameWindow.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                gameWindow.getWindow().close();
                return false;
            }
            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                {
                    if (showNameDialog)
                    {
                        // Close name dialog, back to main
                        showNameDialog = false;
                        playerName = "";
                        nameDisplay.setString("");
                        nameBox.setOutlineColor(sf::Color(140, 100, 240));
                    }
                    else
                    {
                        gameWindow.getWindow().close();
                        return false;
                    }
                }
                // Enter confirms name
                if (key->code == sf::Keyboard::Key::Enter && showNameDialog)
                {
                    if (!playerName.empty())
                        return true;
                    else
                    {
                        warningText.setString("Please enter your name first!");
                        sf::FloatRect wt = warningText.getLocalBounds();
                        warningText.setOrigin({ wt.size.x / 2.f, wt.size.y / 2.f });
                        warningText.setPosition({ 400.f, 375.f });
                        showWarning = true;
                        warningClock.restart();
                        nameBox.setOutlineColor(sf::Color(255, 70, 70));
                    }
                }
            }
            // Text typed (name dialog only)
            if (const auto* text = event->getIf<sf::Event::TextEntered>())
            {
                if (showNameDialog)
                {
                    try
                    {
                        uint32_t unicode = text->unicode;
                        if (unicode == 8)   // Backspace
                        {
                            if (!playerName.empty())
                            {
                                playerName.pop_back();
                                showWarning = false;
                                nameBox.setOutlineColor(sf::Color(180, 140, 255));
                            }
                        }
                        else
                        {
                            char c = static_cast<char>(unicode);
                            if (!isalpha(c) && c != ' ')
                                throw std::invalid_argument("not a letter or space");
                            if (playerName.size() >= 15)
                                throw std::length_error("too long");
                            if (c == ' ' && playerName.empty())
                                throw std::invalid_argument("no leading space");

                            playerName += c;
                            showWarning = false;
                            nameBox.setOutlineColor(sf::Color(180, 140, 255));
                        }
                    }
                    catch (const std::invalid_argument&)
                    {
                        warningText.setString("Only alphabets allowed!");
                        sf::FloatRect wt = warningText.getLocalBounds();
                        warningText.setOrigin({ wt.size.x / 2.f, wt.size.y / 2.f });
                        warningText.setPosition({ 400.f, 375.f });
                        nameBox.setOutlineColor(sf::Color(255, 70, 70));
                        showWarning = true;
                        warningClock.restart();
                    }
                    catch (const std::length_error&) { /* silently ignore */}
                }
            }

            // Mouse click
            if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouse->button == sf::Mouse::Button::Left)
                {
                    sf::Vector2i pos = sf::Mouse::getPosition(gameWindow.getWindow());

                    if (!showNameDialog)
                    {
                        if (isClicked(startButton, pos))
                        {
                            showNameDialog = true;
                            playerName = "";
                            nameDisplay.setString("");
                            nameBox.setOutlineColor(sf::Color(180, 140, 255));
                            showWarning = false;
                        }
                        else if (isClicked(exitButton, pos))
                        {
                            return false;
                        }
                    }
                }
            }
        }

        // Auto-hide warning after 2s
        if (showWarning && warningClock.getElapsedTime().asSeconds() > 2.f)
        {
            showWarning = false;
            nameBox.setOutlineColor(sf::Color(180, 140, 255));
        }

        // Cursor blink 
        std::string displayed = playerName;
        if (showNameDialog)
            displayed += "|";
        nameDisplay.setString(displayed);

        // Button hover effects
        if (!showNameDialog)
        {
            sf::Vector2i mpos = sf::Mouse::getPosition(gameWindow.getWindow());
            // Start
            if (isClicked(startButton, mpos))
            {
                startButton.setFillColor(sf::Color(100, 75, 200));
                startButton.setOutlineColor(sf::Color(220, 190, 255));
            }
            else
            {
                startButton.setFillColor(sf::Color(70, 50, 150));
                startButton.setOutlineColor(sf::Color(180, 140, 255));
            }
            // Exit
            if (isClicked(exitButton, mpos))
            {
                exitButton.setFillColor(sf::Color(180, 50, 50));
                exitButton.setOutlineColor(sf::Color(255, 140, 140));
            }
            else
            {
                exitButton.setFillColor(sf::Color(130, 30, 30));
                exitButton.setOutlineColor(sf::Color(255, 100, 100));
            }
        }

        // Draw
        gameWindow.clear();
        gameWindow.getWindow().draw(bgSprite);
        // Always draw main screen behind everything
        gameWindow.getWindow().draw(startButton);
        gameWindow.getWindow().draw(exitButton);
        gameWindow.getWindow().draw(titleText);
        gameWindow.getWindow().draw(startText);
        gameWindow.getWindow().draw(exitText);
        // Name dialog on top
        if (showNameDialog)
        {
            gameWindow.getWindow().draw(overlayDim);
            gameWindow.getWindow().draw(overlayBox);
            gameWindow.getWindow().draw(dialogTitle);
            gameWindow.getWindow().draw(nameBox);
            gameWindow.getWindow().draw(nameDisplay);
            gameWindow.getWindow().draw(hintText);
            if (showWarning)
                gameWindow.getWindow().draw(warningText);
        }
        gameWindow.display();
    }
    return false;
}