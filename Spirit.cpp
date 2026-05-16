// Spirit.cpp

#include "Spirit.h"
#include <algorithm>  

Spirit::Spirit(const std::string& n,int h,int atk,int def,int xp,bool boss):name(n),hp(h),maxHp(h),attackPower(atk),defense(def),xpReward(xp),isBoss(boss) 
{}

void Spirit::takeDamage(int dmg) 
{
int actual=std::max(1,dmg-defense);
hp-=actual;
if(hp<0)hp=0;
}

bool Spirit::loadSheet(const std::string& path,
 sf::Vector2i frameSize,
    const std::unordered_map<CharacterState, std::vector<int>>& anims)
{
return m_renderer.loadFromSheet(path, frameSize, anims);
}
bool Spirit::isAlive()const
{ 
return hp>0;
}
bool Spirit::hasReachedHalfHealth()const 
{ 
return hp<=(maxHp/2); 
}
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