//---------------------------------- ( Veilwalker.h ) -------------------------------------------------
// COMPOSITION... Veilwalker owns Inventory and vector<Ability>...
// Both are destroyed when Veilwalker is destroyed...
#pragma once
#include <vector>
#include "Inventory.h"
#include "Ability.h"
#include "CharacterRenderer.h"

using namespace std;

class Veilwalker
{
private:
string name;
int level;
int hp, maxHp;
int attackPower;
int defense;
int spiritEnergy, maxEnergy;
int xp;
int xpToNextLevel;
int coins;
Inventory inventory;
 vector<Ability> abilities;
void levelUp();
void checkAbilityUnlock();
CharacterRenderer m_renderer; 
public:
Veilwalker(const string& playerName);
int attackEnemy();
int useAbility(int index);
void takeDamage(int dmg);
void takeRawDamage(int dmg);
void heal(int amount);
void restoreEnergy(int amount);
Inventory& getInventory();
int useItemFromInventory(int idx);
void gainXP(int amount);
int getCoins() const;
void addCoins(int amount);
void removeCoins(int amount);
string getName() const;
int getLevel() const;
int getHp() const;
int getMaxHp() const;
int getAttackPower() const;
int getDefense() const;
int getSpiritEnergy() const;
int getMaxEnergy() const;
int getXP() const;
int getXPToNext() const;
bool isAlive() const;
const vector<Ability>& getAbilities() const;
string serialize() const;
void deserialize(const string& data);
CharacterRenderer& getRenderer() { return m_renderer; }
};