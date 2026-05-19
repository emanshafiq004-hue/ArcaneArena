//---------------------------------------- ( Inventory.cpp ) --------------------------------------------------------------
#include "Inventory.h"
#include <sstream>
#include <iostream>
using namespace std;

//CONSTRUCTOR:-
Inventory::Inventory() {}

//ADD ITEMS:-
bool Inventory::addItem(const Item& item) 
{
if (static_cast<int>(items.size()) >= MAX_ITEMS) 
{
cerr << "[Inventory] Inventory is full! Cannot add " << item.getName() << endl;
return false;
}
items.push_back(item);
return true;
}

//USE ITEMS:-
int Inventory::useItem(int index) 
{
if (index < 0 || index >= static_cast<int>(items.size())) 
{
cerr << "[Inventory] Invalid item index: " << index << ". Available items: " << items.size() << endl;
return -1;
}
if (!items[index].isAvailable()) 
{
cerr << "[Inventory] Item '" << items[index].getName() << "' has no uses left!" << endl;
return -1;
}
int effect = items[index].getEffectValue();
items[index].use();
if (!items[index].isAvailable())
items.erase(items.begin() + index);
return effect;
}

//GETTERS:-
const vector<Item>& Inventory::getItems() const 
{
return items;
}

int Inventory::getCount() const 
{
return static_cast<int>(items.size());
}

//SERIALIZE:-
string Inventory::serialize() const 
{
ostringstream oss;
for (const auto& item : items)
oss << item.serialize() << ";";
return oss.str();
}

//DESERIALIZE:-
void Inventory::deserialize(const string& data) 
{
items.clear();
istringstream ss(data);
string token;
while (getline(ss, token, ';'))
{
if (token.empty())
continue;
istringstream ts(token);
string name, typeStr, effStr, qtyStr;
getline(ts, name, ',');
getline(ts, typeStr, ',');
getline(ts, effStr, ',');
getline(ts, qtyStr, ',');
if (!name.empty())items.emplace_back(name,static_cast<ItemType>(stoi(typeStr)),stoi(effStr),stoi(qtyStr));
}
}