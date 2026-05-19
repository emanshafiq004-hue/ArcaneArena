//-------------------------------------------- ( Inventory.h ) -------------------------------------------------------
// COMPOSITION inside Veilwalker... Veilwalker "has an" Inventory.
// Inventory cannot meaningfully exist without a Veilwalker owning it....
#pragma once
#include <vector>
#include <string>
#include "Item.h"

using namespace std;

class Inventory 
{
private:
vector<Item> items;
static const int MAX_ITEMS = 20;
public:
Inventory();
bool addItem(const Item& item);
int useItem(int index);
const vector<Item>& getItems() const;
int getCount() const;
string serialize() const;
void deserialize(const string& data);
};