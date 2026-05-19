//---------------------------------- ( Spirits.h ) ----------------------------------------------
// All enemy subclasses.
// Demonstrates INHERITANCE and POLYMORPHISM.
// Each subclass overrides attackPlayer() with unique behavior.

#pragma once
#include "Spirit.h"
#include <cstdlib>
#include <tuple>

using namespace sf;

//-----------------------------------------------------------------------------------------------
// FOREST REGION ENEMIES:-
//-----------------------------------------------------------------------------------------------

class Willowisp : public Spirit
{
public:
Willowisp() : Spirit("Willowisp",100,10,5,30)
{
std::unordered_map<CharacterState, std::tuple<std::string, sf::Vector2i, std::vector<int>>> configs;
configs[CharacterState::Idle] = {"assets/characters/WIdle.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
configs[CharacterState::Walk] = {"assets/characters/WMove.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
configs[CharacterState::Attack] = {"assets/characters/WAttack.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
configs[CharacterState::Dying] = {"assets/characters/WDie.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
loadMultipleStateSheets(configs);
}
int attackPlayer() override { return attackPower; }
// Willowisp drains spirit energy on attack...
bool drainsEnergy() const { return true; }
};
//-------------------------------------------------------------------------------------------------------------------------------
// TEMPLE REGION ENEMIES:-
//-------------------------------------------------------------------------------------------------------------------------------

class StoneGolem : public Spirit
{
public:
StoneGolem() : Spirit("Stone Golem", 120, 25, 5, 60)
{
std::unordered_map<CharacterState, std::tuple<std::string, sf::Vector2i, std::vector<int>>> configs;
configs[CharacterState::Idle] = {"assets/characters/GIdle.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
configs[CharacterState::Walk] = {"assets/characters/GMove.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
configs[CharacterState::Attack] = {"assets/characters/GAttack.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
configs[CharacterState::Dying] = {"assets/characters/GDie.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
loadMultipleStateSheets(configs);
}
int attackPlayer() override { return attackPower; }
};

//---------------------------------------------------------------------------------
// VOLCANO REGION ENEMIES:-
//---------------------------------------------------------------------------------

class Emberwing : public Spirit
{
public:
Emberwing() : Spirit("Emberwing", 150, 30, 10, 100)
{
std::unordered_map<CharacterState, std::tuple<std::string, sf::Vector2i, std::vector<int>>> configs;
configs[CharacterState::Idle] = {"assets/characters/EIdle.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
configs[CharacterState::Walk] = {"assets/characters/EMove.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
configs[CharacterState::Attack] = {"assets/characters/EAttack.png", {128, 128}, {0, 1, 2, 3, 4,5}};
configs[CharacterState::Dying] = {"assets/characters/EDie.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
loadMultipleStateSheets(configs);
}
int attackPlayer() override { return attackPower; }
};

//----------------------------------------------------------------------------------------------
//VEIL GATE ENEMIES:-
//----------------------------------------------------------------------------------------------

class Shadow : public Spirit
{
public:
Shadow() : Spirit("Shadow", 170, 40, 15, 150)
{
std::unordered_map<CharacterState, std::tuple<std::string, sf::Vector2i, std::vector<int>>> configs;
configs[CharacterState::Idle] = { "assets/characters/SIdle.png", {128, 128}, {0, 1, 2, 3, 4, 5} };
configs[CharacterState::Walk] = { "assets/characters/SMove.png", {128, 128}, {0, 1, 2, 3, 4, 5} };
configs[CharacterState::Attack] = { "assets/characters/SAttack.png", {128, 128}, {0, 1, 2, 3, 4, 5} };
configs[CharacterState::Dying] = { "assets/characters/SDie.png", {128, 128}, {0, 1, 2, 3, 4, 5} };
loadMultipleStateSheets(configs);
}
int attackPlayer() override { return attackPower; }
};

//----------------------------------------------------------------------------------------------
// PHANTOM WRAITH:-
//----------------------------------------------------------------------------------------------

class PhantomWraith : public Spirit
{
public:
PhantomWraith() : Spirit("Phantom Wraith", 50, 10, 2, 40)
{
std::unordered_map<CharacterState, std::tuple<std::string, sf::Vector2i, std::vector<int>>> configs;
configs[CharacterState::Idle] = {"assets/characters/PIdle.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
configs[CharacterState::Walk] = {"assets/characters/PMove.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
configs[CharacterState::Attack] = {"assets/characters/PAttack.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
configs[CharacterState::Steal] = {"assets/characters/PSteal.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
configs[CharacterState::Dying] = {"assets/characters/PDie.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
loadMultipleStateSheets(configs);
}
int attackPlayer() override { return attackPower; }
// PhantomWraith can steal items from the player.
bool stealsItems() const { return true; }
};

// -------------------------------------------------------------------------------------------------
// BOSSES:-
// -------------------------------------------------------------------------------------------------

// Region 0 Boss: Forest Corrupted Dryad..
class ForestCorruptor : public Spirit
{
public:
ForestCorruptor() : Spirit("Corrupted Dryad", 120, 20, 10, 90, true)
{
std::unordered_map<CharacterState, std::tuple<std::string, sf::Vector2i, std::vector<int>>> configs;
configs[CharacterState::Idle] = {"assets/characters/WBIdle.png", {128, 128}, {0, 1, 2, 3, 4,5}};
configs[CharacterState::Walk] = {"assets/characters/WBMove.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
configs[CharacterState::Attack] = {"assets/characters/WBAttack.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
configs[CharacterState::Dying] = {"assets/characters/WBDie.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
loadMultipleStateSheets(configs);
}
int attackPlayer() override { return attackPower; }
};

// Region 1 Boss: Temple Guardian..
class TempleWarden : public Spirit
{
public:
TempleWarden() : Spirit("Temple Guardian", 140, 30, 15, 120, true)
{
std::unordered_map<CharacterState, std::tuple<std::string, sf::Vector2i, std::vector<int>>> configs;
configs[CharacterState::Idle] = {"assets/characters/GBIdle.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
configs[CharacterState::Walk] = {"assets/characters/GBMove.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
configs[CharacterState::Attack] = {"assets/characters/GBAttack.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
configs[CharacterState::Dying] = {"assets/characters/GBDie.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
loadMultipleStateSheets(configs);
}
int attackPlayer() override { return attackPower; }
};

// Region 2 Boss: Ember Tyrant..
class EmberTyrant : public Spirit
{
public:
EmberTyrant() : Spirit("Ember Tyrant", 160, 35, 18, 150, true)
{
std::unordered_map<CharacterState, std::tuple<std::string, sf::Vector2i, std::vector<int>>> configs;
configs[CharacterState::Idle] = {"assets/characters/EBIdle.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
configs[CharacterState::Walk] = {"assets/characters/EBMove.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
configs[CharacterState::Attack] = {"assets/characters/EBAttack.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
configs[CharacterState::Dying] = {"assets/characters/EBDie.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
loadMultipleStateSheets(configs);
}
// EmberTyrant adds random bonus damage...
int attackPlayer() override { return attackPower + rand() % 10; }
};

// Region 3 Boss: Shadow Sovereign (3 phases)...
class ShadowSovereign : public Spirit
{
private:
int phase;
int attackBoost;
public:
ShadowSovereign(): Spirit("Shadow Sovereign", 200, 50, 25, 500, true),phase(1), attackBoost(0)
{
std::unordered_map<CharacterState, std::tuple<std::string, sf::Vector2i, std::vector<int>>> configs;
configs[CharacterState::Idle] = {"assets/characters/SBIdle.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
configs[CharacterState::Walk] = {"assets/characters/SBMove.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
configs[CharacterState::Attack] = {"assets/characters/SBAttack.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
configs[CharacterState::ShapeShift] = {"assets/characters/SBShapeShift.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
configs[CharacterState::Dying] = {"assets/characters/SBDie.png", {128, 128}, {0, 1, 2, 3, 4, 5}};
loadMultipleStateSheets(configs);
}
int getPhase() const { return phase; }
// Phase change at half health — POLYMORPHISM via virtual override.
void onHalfHealth() override
{
if (phase == 1)
{
phase = 2;
attackBoost = 10;
attackPower += attackBoost;
}
else if (phase == 2 && hp <= maxHp / 3)
{
phase = 3;
attackBoost = 15;
attackPower += attackBoost;
}
}
// Phase 3 adds random chaos damage...
int attackPlayer() override
{
if (phase == 3)
return attackPower + rand() % 20;
return attackPower;
}
};
