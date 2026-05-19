//--------------------------------- ( Spirit.h ) ---------------------------------------------
// Abstract base Class for all enemies...
// Holds the CharacterRenderer for sprite animations(Composition).

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
// Helper to load multiple separate sprite sheets (one per state)...
bool loadMultipleStateSheets(const std::unordered_map<CharacterState, std::tuple<std::string, sf::Vector2i, std::vector<int>>>& stateConfigs, float frameDuration = 0.10f);
public:
Spirit(const std::string& n = "Unknown Spirit", int hp = 100, int atk =10,int def =5, int xpReward =30, bool boss = false);
virtual ~Spirit() {}//polymorphism..+ inheritance..
virtual int  attackPlayer() = 0;//Abstraction..
virtual void onHalfHealth() {}
virtual void takeDamage(int dmg);
virtual bool isAlive() const;
virtual bool hasReachedHalfHealth() const;
//getters..
std::string getName() const;
int getHp() const;
int getMaxHp() const;
int getXPReward() const;
bool getIsBoss() const;
CharacterRenderer& getRenderer() { return m_renderer; }
};