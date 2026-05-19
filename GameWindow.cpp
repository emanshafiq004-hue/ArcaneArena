//------------------------------------ ( GameWindow.cpp ) --------------------------------------
// Encapsulation...(private window, public interface)
//this pointer used implicitly throughout member functions....

#include "GameWindow.h"
#include "AssetLoader.h"
#include <iostream>
//constructor:
GameWindow::GameWindow(const string& t, unsigned int w, unsigned int h): title(t), bgColor(Color(13, 2, 33)), width(w), height(h) 
{
window.create(VideoMode({ width, height }), title);//setting resolution..
window.setFramerateLimit(60);//setting frames per sec...
cout << "[GameWindow] Created: " << title <<endl;
}
//destructor:
GameWindow::~GameWindow() 
{
if (window.isOpen())
window.close();
cout << "[GameWindow] Destroyed. Goodbye!" << endl;
}
//setter:
void GameWindow::setInterface(const Color& bg, const string& newTitle) 
{
bgColor = bg;
title = newTitle;
window.setTitle(title);
}
//setter for icon:
void GameWindow::setIconFromFile(const string& iconPath) 
{
Image icon;
if (!AssetLoader::loadImageWithFallback(icon, { iconPath }))
return;
window.setIcon(icon);
}
//Rendering utility functions...
void GameWindow::clear() 
{
window.clear(bgColor);
}
void GameWindow::display()
{
window.display();
}
//check is window open ?
bool GameWindow::isOpen() const 
{
return window.isOpen();
}
//Getter ...gets windows reference and trigger aggregation..
RenderWindow& GameWindow::getWindow() 
{
return window;
}

//handles different events of window..
optional<Event> GameWindow::pollEvent() 
{
//pollEvent accesses the queue of events from the os and pull events from queue until it is empty..
return window.pollEvent();
}