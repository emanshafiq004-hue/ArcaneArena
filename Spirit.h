//// Spirit.h
//// Abstract BASE CLASS.... (Abstraction + Inheritance + Polymorphism)
//// Pure virtual attackPlayer() forces every subclass to define unique behavior.
//
//#pragma once
//#include <string>
//using namespace std;
//
//class Spirit 
//{
//protected:
//string name;
//int hp,maxHp;
//int attackPower;
//int defense;
//int xpReward;
//bool isBoss;
//public:
//Spirit(const string& name="Unknown Spirit",int hp=50,int atk=10,int def=2,int xpReward=30,bool boss=false);
//virtual ~Spirit() {}
//virtual int  attackPlayer() = 0; // Pure virtual function ... ABSTRACTION
//virtual void onHalfHealth() {}   // Optional override for bosses....
//virtual void takeDamage(int dmg);
//virtual bool isAlive()const;
//virtual bool hasReachedHalfHealth()const;
//string getName()const;
//int getHp()const;
//int getMaxHp()const;
//int getXPReward()const;
//bool getIsBoss()const;
//};
//-------------------------------------------------------------------------
// Spirit.h – Abstract base for all enemies.
// Holds the CharacterRenderer for sprite animations.

#pragma once
#include "CharacterRenderer.h"
#include <string>
#include <unordered_map>
#include <vector>

class Spirit
{
protected:
    std::string name;
    int hp, maxHp;
    int attackPower;
    int defense;
    int xpReward;
    bool isBoss;

    CharacterRenderer m_renderer;

    // Helper for subclasses to load their sprite sheet.
    bool loadSheet(const std::string& path,
        sf::Vector2i frameSize,
        const std::unordered_map<CharacterState, std::vector<int>>& anims);

public:
    Spirit(const std::string& n = "Unknown Spirit", int hp = 50, int atk = 10,
        int def = 2, int xpReward = 30, bool boss = false);
    virtual ~Spirit() {}

    virtual int  attackPlayer() = 0;
    virtual void onHalfHealth() {}
    virtual void takeDamage(int dmg);
    virtual bool isAlive() const;
    virtual bool hasReachedHalfHealth() const;

    std::string getName() const;
    int getHp() const;
    int getMaxHp() const;
    int getXPReward() const;
    bool getIsBoss() const;

    CharacterRenderer& getRenderer() { return m_renderer; }
};