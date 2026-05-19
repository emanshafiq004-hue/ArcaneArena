//------------------------------------------------------- ( Item.cpp ) -----------------------------------------------------
#include "Item.h"
#include <iostream>
#include <sstream>

using namespace std;

Item::Item(const string& n, ItemType t, int eff, int qty): name(n), type(t), effectValue(eff), quantity(qty) 
{
if (qty < 0) 
{
cerr << "[Item] Error: quantity cannot be negative for item '" << n << "'! Setting quantity to 0." << endl;
quantity = 0;
}
}

string Item::getName() const 
{
return name;
}

ItemType Item::getType() const 
{
return type;
}

int Item::getEffectValue() const 
{
return effectValue;
}

int Item::getQuantity() const 
{
return quantity;
}

void Item::use() 
{
if (quantity <= 0) 
{
cerr << "[Item] Warning: Item '" << name << "' is out of stock! Cannot use." << endl;
return;
}
quantity--;
}

bool Item::isAvailable() const 
{
return quantity > 0;
}

string Item::serialize() const 
{
ostringstream oss;
oss << name << "," << static_cast<int>(type) << "," << effectValue << "," << quantity;
return oss.str();
}