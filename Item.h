//-------------------------------------------- ( Item.h ) --------------------------------------------------------
#pragma once
#include <string>
using namespace std;

enum class ItemType {HealingPotion,SpiritCrystal,AncientRelic};

class Item 
{
private:
string name;
ItemType type;
int effectValue;
int quantity;
public:
Item(const string& name, ItemType type, int effectValue, int quantity = 1);
string getName() const;
ItemType getType() const;
int getEffectValue() const;
int getQuantity() const;
void use();
bool isAvailable() const;
string serialize() const;
};