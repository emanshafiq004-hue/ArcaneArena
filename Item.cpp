#include "Item.h"
#include <stdexcept>
#include <sstream>

using namespace std;

Item::Item(const string& n, ItemType t, int eff, int qty): name(n), type(t), effectValue(eff), quantity(qty) {
if (qty < 0)
throw invalid_argument("Item: quantity cannot be negative!");
}

string Item::getName() const {
return name;
}

ItemType Item::getType() const {
return type;
}

int Item::getEffectValue() const {
return effectValue;
}

int Item::getQuantity() const {
return quantity;
}

void Item::use() {
if (quantity <= 0)
throw runtime_error("Item '" + name + "' is out of stock!");
quantity--;
}

bool Item::isAvailable() const {
return quantity > 0;
}

string Item::serialize() const {
ostringstream oss;
oss << name << "," << static_cast<int>(type) << "," << effectValue << "," << quantity;
return oss.str();
}