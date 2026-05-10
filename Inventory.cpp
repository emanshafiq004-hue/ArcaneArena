#include "Inventory.h"
#include <stdexcept>
#include <sstream>
using namespace std;

Inventory::Inventory() {}

void Inventory::addItem(const Item& item) 
{
if (static_cast<int>(items.size()) >= MAX_ITEMS)
throw overflow_error("Inventory is full!");
items.push_back(item);
}

int Inventory::useItem(int index) {
if (index < 0 || index >= static_cast<int>(items.size()))
throw out_of_range("Invalid item index!");
if (!items[index].isAvailable())
throw runtime_error("Item has no uses left!");
int effect = items[index].getEffectValue();
items[index].use();
if (!items[index].isAvailable())
items.erase(items.begin() + index);
return effect;
}

const vector<Item>& Inventory::getItems() const {
return items;
}

int Inventory::getCount() const {
return static_cast<int>(items.size());
}

string Inventory::serialize() const {
ostringstream oss;
for (const auto& item : items)
oss << item.serialize() << ";";
return oss.str();
}

void Inventory::deserialize(const string& data) {
items.clear();
istringstream ss(data);
string token;
while (getline(ss, token, ';')) {
if (token.empty())
continue;
istringstream ts(token);
string name, typeStr, effStr, qtyStr;
getline(ts, name, ',');
getline(ts, typeStr, ',');
getline(ts, effStr, ',');
getline(ts, qtyStr, ',');
if (!name.empty())
items.emplace_back(name,static_cast<ItemType>(stoi(typeStr)),stoi(effStr),stoi(qtyStr));
}
}