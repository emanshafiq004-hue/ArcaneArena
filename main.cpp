//// main.cpp
//// One GameWindow created here...lives until Exit selected.
//// All modules receive a reference.... (AGGREGATION).
//
//#include "GameWindow.h"
//#include "WelcomeScreen.h"
//#include "MainMenu.h"
//#include "RealmOfEchoes.h"
//#include "IronFist.h"
//#include "ShatterShift.h"
//#include "SkySurge.h"
//#include "ScoresStatus.h"
//#include <iostream>
//#define WIN32_LEAN_AND_MEAN
//#define NOMINMAX
//#include <windows.h>
//#ifdef byte
//#undef byte
//#endif
//
//using namespace std;
//
//int main() 
//{
//try 
//{
//// Ensure relative asset paths resolve (assets/...). When launched from
//// x64/Debug or x64/Release, step up to the project directory.
//char exePath[MAX_PATH]{};
//GetModuleFileNameA(nullptr, exePath, MAX_PATH);
//string path(exePath);
//for (int i = 0; i < 3; i++) {
//    size_t slash = path.find_last_of("\\/");
//    if (slash == string::npos)
//        break;
//    path.resize(slash);
//}
//SetCurrentDirectoryA(path.c_str());
//GameWindow gameWindow("Arcane Arena");
//WelcomeScreen welcome(gameWindow,gameWindow.getAudio());
//welcome.run();
//bool running=true;
//bool Track = false;
//while(running&&gameWindow.isOpen()) 
//{
//MainMenu menu(gameWindow,gameWindow.getAudio());
//if(Track)
//gameWindow.getAudio().PlayMusic("assets/audio/arcane_arena.ogg", 90.f);
//int choice=menu.run();
//Track = true;
//switch (choice) 
//{
//case 1: 
//{
//gameWindow.getAudio().Stop();
//RealmOfEchoes roe(gameWindow); 
//roe.run(); 
//break; 
//}
//case 2: 
//{ 
//gameWindow.getAudio().Stop();
//IronFist ironFist(gameWindow); 
//ironFist.run(); 
//break; 
//}
//case 3: 
//{ 
//gameWindow.getAudio().Stop();
//ShatterShift shatter(gameWindow); 
//shatter.run();  
//break; 
//}
//case 4: 
//{
//gameWindow.getAudio().Stop();
//SkySurge skySurge(gameWindow); 
//skySurge.run(); 
//break; 
//} 
//case 5: 
//{
//ScoresStatus scores(gameWindow); 
//scores.run();  
//break; 
//}
//case 0:
//default:
//running=false; 
//break;
//}
//}
//}
//catch(const exception& e) 
//{
//cerr<< "[FATAL ERROR] " <<e.what()<<endl;
//return 1;
//}
//return 0;
//}







// ---------------------------------------------( main.cpp )------------------------------------------------------------
#include "GameWindow.h"
#include "WelcomeScreen.h"
#include "MainMenu.h"
#include "RealmOfEchoes.h"
#include "IronFist.h"
#include "ShatterShift.h"
#include "SkySurge.h"
#include "GameScoresScreen.h"
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
        char exePath[MAX_PATH]{};
        GetModuleFileNameA(nullptr, exePath, MAX_PATH);
        string path(exePath);
        for (int i = 0; i < 3; i++) {
            size_t slash = path.find_last_of("\\/");
            if (slash == string::npos) break;
            path.resize(slash);
        }
        SetCurrentDirectoryA(path.c_str());

        GameWindow gameWindow("Arcane Arena");
        WelcomeScreen welcome(gameWindow, gameWindow.getAudio());
        welcome.run();

        bool running = true;
        bool trackMusic = false;
        while (running && gameWindow.isOpen())
        {
            MainMenu menu(gameWindow, gameWindow.getAudio());
            if (trackMusic)
                gameWindow.getAudio().PlayMusic("assets/audio/arcane_arena.ogg", 90.f);
            int choice = menu.run();
            trackMusic = true;

            switch (choice)
            {
            case 1:
            {
                gameWindow.getAudio().Stop();
                RealmOfEchoes roe(gameWindow, gameWindow.getAudio()); roe.run();
                break;
            }
            case 2:
            {
                gameWindow.getAudio().Stop();
                IronFist ironFist(gameWindow); ironFist.run();
                break;
            }
            case 3:
            {
                gameWindow.getAudio().Stop();
                ShatterShift shatter(gameWindow); shatter.run();
                break;
            }
            case 4:
            {
                gameWindow.getAudio().Stop();

                SkySurge skySurge(gameWindow); skySurge.run();

                break;
            }
            case 5:
            {
                GameScoresScreen scores(gameWindow, gameWindow.getAudio());
                scores.run();
                trackMusic = false;
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
    catch (const exception& e)
    {
        cerr << "[FATAL ERROR] " << e.what() << endl;
        return 1;
    }
    return 0;
}