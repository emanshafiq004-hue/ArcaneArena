//------------------------------------------------ ( Veilwalker.cpp ) ----------------------------------------------------------------------
#include "Veilwalker.h"
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <tuple>

Veilwalker::Veilwalker(const string& playerName):name(playerName),level(1),hp(100),maxHp(100),attackPower(15),defense(5),spiritEnergy(100),maxEnergy(100),xp(0),xpToNextLevel(50),coins(0)
{
abilities.emplace_back("SPIRIT STRIKE",10,20,"Channel your veil energy into a focused strike.");
cout<<"[Veilwalker] "<<name<< " enters the Veillands!\n";
std::unordered_map<CharacterState, std::tuple<std::string, sf::Vector2i, std::vector<int>>> vwAnims;
vwAnims[CharacterState::Idle] = { "assets/characters/VIdle.png", { 128, 128 }, { 0, 1, 2, 3, 4, 5 } };
vwAnims[CharacterState::Attack] = { "assets/characters/VAttack.png", { 128, 128 }, { 0, 1, 2, 3, 4, 5 } };
vwAnims[CharacterState::Block] = { "assets/characters/VBlock.png", { 128, 128 }, { 0, 1, 2, 3, 4, 5 } };
vwAnims[CharacterState::Walk] = { "assets/characters/VMove.png", { 128, 128 }, { 0, 1, 2, 3, 4, 5 } };
vwAnims[CharacterState::Ability] = { "assets/characters/VAbility.png", { 128, 128 }, { 0, 1, 2, 3, 4} };
vwAnims[CharacterState::Item] = { "assets/characters/VHeal.png", { 128, 128 }, { 0, 1, 2, 3, 4, 5 } };
vwAnims[CharacterState::Dying] = { "assets/characters/VDie.png", { 128, 128 }, { 0, 1, 2, 3, 4, 5 } };
m_renderer.loadMultipleStates(vwAnims);
}

//LEVEL UP:-
void Veilwalker::levelUp() 
{
level++;
maxHp+=20;
hp=maxHp;// Full heal on level up....
attackPower+=5;
defense+=5;
maxEnergy+=10;
spiritEnergy=maxEnergy;
xp-=xpToNextLevel;
xpToNextLevel=static_cast<int>(xpToNextLevel*2.f);
cout <<"[Veilwalker] LEVEL UP! Now level "<< level <<endl;
checkAbilityUnlock();
}

//ABILITY UNLOCK CHECK:-
void Veilwalker::checkAbilityUnlock() 
{
if (level==3) 
abilities.emplace_back("VEIL BURST",20,40,"Unleash a burst of veil energy.");
else if (level==5) 
abilities.emplace_back("SHADOW BIND",30,70,"Trap the spirit in shadow chains.");
else if (level==8) 
abilities.emplace_back("ECHOING SLASH",40,100,"A slash that echoes through the spirit realm.");
}

//GETTER:-
int Veilwalker::attackEnemy() 
{ 
return attackPower; 
}

//USE ABILITY:-
int Veilwalker::useAbility(int index) 
{
if (index < 0 || index >= static_cast<int>(abilities.size()))
{
cerr << "[Inventory] Invalid Ability index: " << index << ". Available Abilities: " << abilities.size() << endl;
return -1;
}
const Ability& ab=abilities[index];
if(spiritEnergy<ab.getEnergyCost()) 
{
cout<<"[Veilwalker] Not enough spirit energy!\n";
return -1;
}
spiritEnergy-=ab.getEnergyCost();
return ab.getDamage();
}

//TAKE DAMAGE :-
void Veilwalker::takeDamage(int dmg) 
{
int actual=max(1,dmg-defense);
hp-=actual;
if(hp<0)
hp=0;
}

//Take Raw Damage ...
void Veilwalker::takeRawDamage(int dmg)
{
if (dmg < 0)
dmg = 0;
hp -= dmg;
if (hp < 0)
hp = 0;
}

//Heal..
void Veilwalker::heal(int amount) 
{ 
hp=min(maxHp,hp+amount); 
}

//Restore Energy...
void Veilwalker::restoreEnergy(int amount) 
{ 
spiritEnergy=min(maxEnergy,spiritEnergy + amount); 
}

//Get Inventory..
Inventory& Veilwalker::getInventory() 
{ 
return inventory; 
}

//USE INVENTORY:-
int Veilwalker::useItemFromInventory(int idx) 
{
int effect = inventory.useItem(idx);
if (effect != -1) 
{
heal(effect);
return effect;
}
return 0;
}

//Gain XP..
void Veilwalker::gainXP(int amount) 
{
xp += amount;
while(xp>=xpToNextLevel)
levelUp();
}

//Get Coins..
int Veilwalker::getCoins() const
{
return coins;
}

//Add Coins.
void Veilwalker::addCoins(int amount)
{
if (amount > 0)
coins += amount;
}

//Remove Coins ...
void Veilwalker::removeCoins(int amount)
{
if (amount <= 0)
return;
coins -= amount;
if (coins < 0)
coins = 0;
}

//GETTERS:-
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

//SERIALIZE:-
string Veilwalker::serialize()const 
{
ostringstream oss;
oss << name << "|" << level << "|" << hp << "|"<< maxHp << "|" << attackPower << "|" << defense << "|"<< spiritEnergy << "|" << maxEnergy << "|" << xp << "|"<< xpToNextLevel << "|" << inventory.serialize() << "|" << coins;
return oss.str();
}

//SANITIZE NAME:-
static void sanitizeName(string& n)
{
string out;
out.reserve(n.size());
for (char c : n) 
{
if (c == '|' || c == '\r' || c == '\n')
continue;
out.push_back(c);
}
n = out.empty() ? string("Wanderer") : out;
}

//DESERIALIZE:-
void Veilwalker::deserialize(const string& data) 
{
istringstream ss(data);
string token;
vector<string> parts;
while (getline(ss, token, '|'))
parts.push_back(token);
if (parts.size()<11)//only check for imp data..
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
abilities.emplace_back("SPIRIT STRIKE",10,20,"Channel your veil energy.");
for (int i=2;i<=level;i++) 
{
if (i==3) 
abilities.emplace_back("VEIL BURST",20,40, "Unleash a burst of veil energy.");
if (i==5) 
abilities.emplace_back("SHADOW BIND",30,70,"Trap the spirit in shadow chains.");
if (i==8) 
abilities.emplace_back("ECHOING SLASH",40,100,"A slash that echoes.");
}
}