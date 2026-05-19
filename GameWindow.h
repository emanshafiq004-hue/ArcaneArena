//-------------------------------- ( GameWindow.h ) -------------------------------------------
// Central window wrapper. ONE object created in main(), shared via reference.
// BASE utility class....all screens AGGREGATE this class (hold reference, don't own).

#pragma once
#include <SFML/Graphics.hpp>
#include"AudioManager.h"
#include <optional>
using namespace std;
using namespace sf;

class GameWindow {
private:
//Encapsulation...
    RenderWindow window;
    Color bgColor;
    string title;
    unsigned int width;
    unsigned int height;
    AudioManager audio;
public:
    //constructor:
    GameWindow(const string& t="ARCANE ARENA",unsigned int w = 800,unsigned int h = 600);
    //Destructor:
    ~GameWindow();
    //Setter:
    void setInterface(const Color& bg, const string& newTitle);
    //Setting Icon for Window:
    void setIconFromFile(const string& iconPath);
    //Rendering Functions:
    void clear();
    void display();
    //optional data type to store event or nullopt;
    optional<Event> pollEvent();
  
    //check window is open?
    bool isOpen() const;
    //Getters:
    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }
	AudioManager& getAudio() { return audio; }
	string getTitle() const { return title; }
    RenderWindow& getWindow();
};