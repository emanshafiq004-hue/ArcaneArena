// Spirits.h
// All enemy subclasses....demonstrates INHERITANCE and POLYMORPHISM.
// Each overrides attackPlayer() differently (runtime polymorphism).

#pragma once
#include "Spirit.h"
#include <cstdlib> 
using namespace sf;

class Willowisp:public Spirit 
{
public:
Willowisp():Spirit("Willowisp",40,12,1,35){}
int attackPlayer() override 
{ 
return attackPower; 
}
bool drainsEnergy()const 
{ 
return true; 
}
};

class StoneGolem:public Spirit 
{
public:
StoneGolem():Spirit("Stone Golem",120,25,15,60){}
int attackPlayer() override 
{ 
return attackPower; 
}
};

class Emberwing:public Spirit 
{
public:
Emberwing():Spirit("Emberwing",80,18,5,55) {}
int attackPlayer() override 
{ 
return attackPower * 2; //Attack twice
}
};

class PhantomWraith:public Spirit 
{
public:
PhantomWraith():Spirit("Phantom Wraith",60,15,3,45) {}
int attackPlayer() override 
{ 
return attackPower; 
}
bool stealsItems() const
{ 
return false; 
}
};

class ForestCorruptor : public Spirit {
public:
    ForestCorruptor() : Spirit("Corrupted Dryad", 95, 20, 5, 90, true) {}
    int attackPlayer() override { return attackPower; }
};

class TempleWarden : public Spirit {
public:
    TempleWarden() : Spirit("Temple Guardian", 140, 28, 12, 110, true) {}
    int attackPlayer() override { return attackPower; }
};

class EmberTyrant : public Spirit {
public:
    EmberTyrant() : Spirit("Ember Tyrant", 120, 24, 8, 100, true) {}
    int attackPlayer() override { return attackPower + rand() % 10; }
};

class ShadowSovereign:public Spirit 
{
private:
int phase;
int attackBoost;
public:
ShadowSovereign():Spirit("Shadow Sovereign",300,30,10,500,true),phase(1),attackBoost(0) {}
int getPhase() const 
{
return phase; 
}
// Phase change at half health....POLYMORPHISM via virtual override
void onHalfHealth() override 
{
if (phase==1) 
{
phase=2;
attackBoost=15;
attackPower+=attackBoost;
}
else if(phase==2&&hp<=maxHp/3) 
{
phase = 3;
attackBoost=25;
attackPower+=attackBoost;
}
}
int attackPlayer() override 
{
if (phase==3) 
return attackPower+(rand()%20);
return attackPower;
}
};