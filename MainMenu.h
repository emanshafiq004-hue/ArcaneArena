// MainMenu.h
// AGGREGATION....holds GameWindow& (reference, doesn't own it)
    #pragma once
    #include "GameWindow.h"
    #include <SFML/Graphics.hpp>
    #include <optional>
    #include <vector>

    class MainMenu 
    {
    private:
    GameWindow& gw;
    Font font;
    Texture bgTex;
    optional<Sprite> bgSprite;
    Text headerText;
    vector<Text> menuItems;
    int selectedIndex;
    void updateHighlight();
	AudioManager& Audio;//AGGREGATION...
    public:
    static const int ITEM_COUNT = 6;
    MainMenu(GameWindow& window, AudioManager& audio);
    int run();
    };