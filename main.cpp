// main.cpp
// One GameWindow created here...lives until Exit selected.
// All modules receive a reference.... (AGGREGATION).

#include "GameWindow.h"
#include "WelcomeScreen.h"
#include "MainMenu.h"
#include "RealmOfEchoes.h"
#include "IronFist.h"
#include "ShatterShift.h"
#include "SkySurge.h"
#include "ScoresStatus.h"
#include <iostream>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#ifdef byte
#undef byte
#endif

using namespace std;

int main() 
{
try 
{
// Ensure relative asset paths resolve (assets/...). When launched from
// x64/Debug or x64/Release, step up to the project directory.
char exePath[MAX_PATH]{};
GetModuleFileNameA(nullptr, exePath, MAX_PATH);
string path(exePath);
for (int i = 0; i < 3; i++) {
    size_t slash = path.find_last_of("\\/");
    if (slash == string::npos)
        break;
    path.resize(slash);
}
SetCurrentDirectoryA(path.c_str());
GameWindow gameWindow("Arcane Arena");
WelcomeScreen welcome(gameWindow,gameWindow.getAudio());
welcome.run();
bool running=true;
while(running&&gameWindow.isOpen()) 
{
MainMenu menu(gameWindow,gameWindow.getAudio());
int choice=menu.run();
switch (choice) 
{
case 1: 
{
gameWindow.getAudio().Stop();
RealmOfEchoes roe(gameWindow); 
roe.run(); 
break; 
}
case 2: 
{ 
gameWindow.getAudio().Stop();
IronFist ironFist(gameWindow); 
ironFist.run(); 
break; 
}
case 3: 
{ 
gameWindow.getAudio().Stop();
ShatterShift shatter(gameWindow); 
shatter.run();  
break; 
}
case 4: 
{
gameWindow.getAudio().Stop();
SkySurge skySurge(gameWindow, gameWindow.getAudio()); 
skySurge.run(); 
break; 
} 
case 5: 
{ 
ScoresStatus scores(gameWindow); 
scores.run();  
break; 
}
case 0:
default:
{
running = false;
break;
}
}
}
}
catch(const exception& e) 
{
cerr<< "[FATAL ERROR] " <<e.what()<<endl;
return 1;
}
return 0;
}