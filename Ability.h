// Ability.h
#pragma once
#include <string>

using namespace std;

class Ability {
private:
    string name;
    int energyCost;
    int damage;
    string description;
public:
    Ability(const string& name, int energyCost, int damage, const string& desc);
    string getName() const;
    int getEnergyCost() const;
    int getDamage() const;
    string getDescription() const;
};