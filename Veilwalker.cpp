// Veilwalker.cpp

#include "Veilwalker.h"
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <algorithm>

Veilwalker::Veilwalker(const string& playerName):name(playerName),level(1),hp(100),maxHp(100),attackPower(15),defense(5),spiritEnergy(50),maxEnergy(50),xp(0),xpToNextLevel(100),coins(0)
{
abilities.emplace_back("Spirit Strike",10,25,"Channel your veil energy into a focused strike.");
cout<<"[Veilwalker] "<<name<< "enters the Veillands!\n";
    // ---- Load sprite sheet for Veilwalker ----
std::unordered_map<CharacterState, std::vector<int>> vwAnims;
// Example layout: row 0: idle (frames 0-4), row 1: walk (5-10), row 2: attack (11-15), etc.
// The user must verify the actual indices.
vwAnims[CharacterState::Idle] = { 0,1,2,3,4 };
vwAnims[CharacterState::Walk] = { 6,7,8,9,10,11 };
vwAnims[CharacterState::Attack] = { 12,13,14,15,16 };
vwAnims[CharacterState::Block] = { 18,19,20,21 };
vwAnims[CharacterState::Ability] = { 24,25,26,27 };
vwAnims[CharacterState::Item] = { 30,31,32,33 };
vwAnims[CharacterState::Negotiate] = { 36,37 };       // if needed
vwAnims[CharacterState::Dying] = { 42,43,44,45 };
m_renderer.loadFromSheet("assets/characters/veilwalker.png",
    sf::Vector2i(128, 128),    // frame size (adjust to actual)
    vwAnims);
// fallback – if load fails, the old static portrait is still loaded elsewhere,
// but we'll keep the renderer uninitialized; Battle will handle that.
}

void Veilwalker::levelUp() 
{
level++;
maxHp+=20;
hp=maxHp;   // Full heal on level up....
attackPower+=5;
defense+=2;
maxEnergy+=10;
spiritEnergy=maxEnergy;
xp-=xpToNextLevel;
xpToNextLevel=static_cast<int>(xpToNextLevel*1.4f);
cout <<"[Veilwalker] LEVEL UP! Now level "<< level << "\n";
checkAbilityUnlock();
}

void Veilwalker::checkAbilityUnlock() 
{
if (level==3) 
abilities.emplace_back("Veil Burst",20,45,"Unleash a burst of veil energy.");
else if (level==5) 
abilities.emplace_back("Shadow Bind",30,70,"Trap the spirit in shadow chains.");
else if (level==8) 
abilities.emplace_back("Echoing Slash",40,100,"A slash that echoes through the spirit realm.");
}

int Veilwalker::attackEnemy() 
{ 
return attackPower; 
}

int Veilwalker::useAbility(int index) 
{
if(index<0||index>=static_cast<int>(abilities.size()))
throw out_of_range("Invalid ability index!");
const Ability& ab=abilities[index];
if(spiritEnergy<ab.getEnergyCost()) 
{
cout<<"[Veilwalker] Not enough spirit energy!\n";
return -1;
}
spiritEnergy-=ab.getEnergyCost();
return ab.getDamage();
}

void Veilwalker::takeDamage(int dmg) 
{
int actual=max(1,dmg-defense);
hp-=actual;
if(hp<0)hp=0;
}

void Veilwalker::takeRawDamage(int dmg)
{
    if (dmg < 0)
        dmg = 0;
    hp -= dmg;
    if (hp < 0)
        hp = 0;
}

void Veilwalker::heal(int amount) 
{ 
hp=min(maxHp,hp+amount); 
}
void Veilwalker::restoreEnergy(int amount) 
{ 
spiritEnergy=min(maxEnergy,spiritEnergy + amount); 
}

Inventory& Veilwalker::getInventory() 
{ 
return inventory; 
}

int Veilwalker::useItemFromInventory(int idx) 
{
try 
{
int effect=inventory.useItem(idx);
heal(effect);
return effect;
}
catch (const exception& e) 
{
cout <<"[Inventory Error] " <<e.what()<<"\n";
return 0;
}
}

void Veilwalker::gainXP(int amount) 
{
xp += amount;
while(xp>=xpToNextLevel)
levelUp();
}

int Veilwalker::getCoins() const
{
    return coins;
}

void Veilwalker::addCoins(int amount)
{
    if (amount > 0)
        coins += amount;
}

void Veilwalker::removeCoins(int amount)
{
    if (amount <= 0)
        return;
    coins -= amount;
    if (coins < 0)
        coins = 0;
}

string Veilwalker::getName()const 
{ 
return name; 
}
int Veilwalker::getLevel()const 
{
return level; 
}
int Veilwalker::getHp() const 
{
return hp; 
}
int Veilwalker::getMaxHp()const 
{ 
return maxHp; 
}
int Veilwalker::getAttackPower()const 
{ 
return attackPower; 
}
int Veilwalker::getDefense()const 
{ 
return defense; 
}
int Veilwalker::getSpiritEnergy()const 
{ 
return spiritEnergy; 
}
int Veilwalker::getMaxEnergy()const 
{ 
return maxEnergy; 
}
int Veilwalker::getXP()const 
{ 
return xp; 
}
int Veilwalker::getXPToNext()const 
{ 
return xpToNextLevel; 
}
bool Veilwalker::isAlive()const 
{ 
return hp>0; 
}
const vector<Ability>& Veilwalker::getAbilities()const 
{ 
return abilities; 
}

string Veilwalker::serialize()const 
{
ostringstream oss;
oss << name << "|" << level << "|" << hp << "|"<< maxHp << "|" << attackPower << "|" << defense << "|"<< spiritEnergy << "|" << maxEnergy << "|" << xp << "|"<< xpToNextLevel << "|" << inventory.serialize() << "|" << coins;
return oss.str();
}

static void sanitizeName(string& n)
{
    string out;
    out.reserve(n.size());
    for (char c : n) {
        if (c == '|' || c == '\r' || c == '\n')
            continue;
        out.push_back(c);
    }
    n = out.empty() ? string("Wanderer") : out;
}

void Veilwalker::deserialize(const string& data) 
{
istringstream ss(data);
string token;
vector<string> parts;
while (getline(ss, token, '|'))
parts.push_back(token);
if (parts.size()<11)
throw runtime_error("Corrupt saved data for Veilwalker!");
name = parts[0];
sanitizeName(name);
level=stoi(parts[1]);
hp=stoi(parts[2]);
maxHp=stoi(parts[3]);
attackPower=stoi(parts[4]);
defense=stoi(parts[5]);
spiritEnergy=stoi(parts[6]);
maxEnergy=stoi(parts[7]);
xp=stoi(parts[8]);
xpToNextLevel=stoi(parts[9]);
inventory.deserialize(parts[10]);
coins = 0;
if (parts.size() >= 12)
    coins = stoi(parts[11]);
if (coins < 0)
    coins = 0;
abilities.clear();
abilities.emplace_back("Spirit Strike",10,25,"Channel your veil energy.");
for (int i=2;i<=level;i++) 
{
if (i==3) 
abilities.emplace_back("Veil Burst",20,45, "Unleash a burst of veil energy.");
if (i==5) 
abilities.emplace_back("Shadow Bind",30,70,"Trap the spirit in shadow chains.");
if (i==8) 
abilities.emplace_back("Echoing Slash",40,100,"A slash that echoes.");
}
}