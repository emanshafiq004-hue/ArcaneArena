// Spirit.h
// Abstract BASE CLASS.... (Abstraction + Inheritance + Polymorphism)
// Pure virtual attackPlayer() forces every subclass to define unique behavior.

#pragma once
#include <string>
using namespace std;

class Spirit 
{
protected:
string name;
int hp,maxHp;
int attackPower;
int defense;
int xpReward;
bool isBoss;
public:
Spirit(const string& name="Unknown Spirit",int hp=50,int atk=10,int def=2,int xpReward=30,bool boss=false);
virtual ~Spirit() {}
virtual int  attackPlayer() = 0; // Pure virtual function ... ABSTRACTION
virtual void onHalfHealth() {}   // Optional override for bosses....
virtual void takeDamage(int dmg);
virtual bool isAlive()const;
virtual bool hasReachedHalfHealth()const;
string getName()const;
int getHp()const;
int getMaxHp()const;
int getXPReward()const;
bool getIsBoss()const;
};