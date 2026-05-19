//-------------------------------- ( Spirit.cpp ) ----------------------------------------------
#include "Spirit.h"
#include <algorithm>  

//Constructor:-
Spirit::Spirit(const std::string& n,int h,int atk,int def,int xp,bool boss):name(n),hp(h),maxHp(h),attackPower(atk),defense(def),xpReward(xp),isBoss(boss) 
{}

void Spirit::takeDamage(int dmg) 
{
int actual=std::max(1,dmg-defense);
hp-=actual;
if(hp<0)
hp=0;
}

// Load multiple separate sprite sheets (one per state)...
bool Spirit::loadMultipleStateSheets(const std::unordered_map<CharacterState, std::tuple<std::string, sf::Vector2i, std::vector<int>>>& stateConfigs, float frameDuration)
{
return m_renderer.loadMultipleStates(stateConfigs, frameDuration);
}

// Check if the spirit is still alive (hp > 0)...
bool Spirit::isAlive()const
{ 
return hp>0;
}

// Check if the spirit has reached half health or below (hp <= maxHp/2)...
bool Spirit::hasReachedHalfHealth()const 
{ 
return hp<=(maxHp/2); 
}

// Getters for spirit attributes...
std::string Spirit::getName()const 
{ 
return name; 
}

int Spirit::getHp()const 
{ 
return hp; 
}

int Spirit::getMaxHp()const 
{ 
return maxHp; 
}

int Spirit::getXPReward()const 
{ 
return xpReward; 
}

bool Spirit::getIsBoss()const 
{ 
return isBoss; 
}