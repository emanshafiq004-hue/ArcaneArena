// WelcomeScreen.cpp

    #include "WelcomeScreen.h"
    #include "AssetLoader.h"
    #include <cmath>
    #include <cstdint>
    #include <stdexcept>

    using namespace std;
    using namespace sf;

    WelcomeScreen::WelcomeScreen(GameWindow& window,AudioManager& audio) : gw(window),titleText(font), subtitleText(font), promptText(font),Audio(audio)
    {
    gw.setInterface(Color(13, 2, 33), "ARCANE ARENA");
    gw.setIconFromFile("assets/icon/arcane_arena.png");
    if (!AssetLoader::openFontWithFallback(font, {"assets/font/WelcomeScreenCinzel.ttf","assets/font/Philosopher-Bold.ttf"}))
    throw runtime_error("WelcomeScreen: Cannot load font");
    AssetLoader::emplaceCoverSprite(bgTex, bgSprite,{"assets/texture/Arcane_Arena.png","assets/texture/bg_arcane_arena_welcome.png","assets/texture/default.png"}, static_cast<float>(gw.getWidth()), static_cast<float>(gw.getHeight()));
    const float cx = static_cast<float>(gw.getWidth()) * 0.5f;
    const float h = static_cast<float>(gw.getHeight());
    //TITLE TEXT
    titleText.setString("ARCANE ARENA");
    titleText.setCharacterSize(68);
    titleText.setFillColor(Color(205, 185, 150));
    titleText.setOutlineColor(Color(35, 0, 55));
    titleText.setOutlineThickness(3.0f);
    titleText.setStyle(Text::Bold);
	FloatRect tb = titleText.getLocalBounds();//gets the bounding box of the text (size and position)
	titleText.setOrigin({ tb.size.x / 2.f, tb.size.y / 2.f });//size is the width and height of the text..
	titleText.setPosition({ cx, h * 0.45f });//dow upto 45% of the height of the window...
    //SUB TITLE TEXT
    subtitleText.setFont(font);
    subtitleText.setString("A MYTHICAL GAMING UNIVERSE");
    subtitleText.setCharacterSize(24);
    subtitleText.setFillColor(Color(155, 110, 185));
    subtitleText.setOutlineColor(Color(40, 0, 70));
    subtitleText.setOutlineThickness(1.0f);
    subtitleText.setStyle(Text::Bold);
	FloatRect sb = subtitleText.getLocalBounds();//local bound gives position of text relative to its own origin..
    subtitleText.setOrigin({ sb.size.x / 2.f, sb.size.y / 2.f });
    subtitleText.setPosition({ cx, h * 0.60f });
    //PROMPT TEXT
    promptText.setFont(font);
    promptText.setString("PRESS ENTER TO BEGIN..");
    promptText.setCharacterSize(22);
    promptText.setFillColor(Color(215, 190, 160));
	promptText.setOutlineColor(Color(40, 0, 70));
	promptText.setOutlineThickness(1.0f);
    promptText.setStyle(Text::Bold);
    FloatRect pb = promptText.getLocalBounds();
    promptText.setOrigin({ pb.size.x / 2.f, pb.size.y / 2.f });
    promptText.setPosition({ cx, h * 0.85f });
    }

    void WelcomeScreen::run()
    {
    Audio.PlayMusic("assets/audio/arcane_arena.ogg", 90.f);
    while (gw.isOpen())
    {
    while (auto event = gw.pollEvent()) //fetch event from queue until it is empty...
    {
    if (event->is<Event::Closed>())//if event is closed(input from cursor) then close window
    gw.getWindow().close();
    if (auto* key = event->getIf<Event::KeyPressed>())//if event is any key pressed get pointer..
    if (key->code == Keyboard::Key::Enter)//if pointer is equal to ASCII of enter key then return from this function and move to next screen...
    return;
    if (auto* mouse = event->getIf<Event::MouseButtonPressed>())
    {
    if (mouse->button == Mouse::Button::Left)
    {
    Vector2i mousePos = Mouse::getPosition(gw.getWindow());
    if (promptText.getGlobalBounds().contains(static_cast<Vector2f>(mousePos)))//global bound gives position of text on screen..
    { //vector 2i (interger) to vector 2f (float) conversion for mouse position and text bounds comparison...
    Audio.preloadOptional("Click", "assets/audio/Main_Confirm.ogg");
    Audio.playOptional("Click", 80.f);
    return;
    }
    }
    }
    }
    float t = animClock.getElapsedTime().asSeconds();
    uint8_t alpha = static_cast<uint8_t>((sin(t * 2.5f) * 0.5f + 0.5f) * 255);//sin from <cmath>...unit8_t from <cstdint> gives fixed integer types...
    //sin used for animation and unit8_t for rgb values...
    promptText.setFillColor(Color(215, 190, 160, alpha));
    //Rendering...
    gw.clear();
    if (bgSprite)
    gw.getWindow().draw(*bgSprite);
    gw.getWindow().draw(titleText);
    gw.getWindow().draw(subtitleText);
    gw.getWindow().draw(promptText);
    gw.display();
    }
    }