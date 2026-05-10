#include "Ability.h"

using namespace std;

Ability::Ability(const string& n, int ec, int dmg, const string& desc): name(n), energyCost(ec), damage(dmg), description(desc) {}

string Ability::getName() const {
return name;
}

int Ability::getEnergyCost() const {
return energyCost;
}

int Ability::getDamage() const {
return damage;
}

string Ability::getDescription() const {
return description;
}