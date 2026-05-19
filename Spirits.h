//// Spirits.h
//// All enemy subclasses....demonstrates INHERITANCE and POLYMORPHISM.
//// Each overrides attackPlayer() differently (runtime polymorphism).
//
//#pragma once
//#include "Spirit.h"
//#include <cstdlib> 
//using namespace sf;
//
//class Willowisp:public Spirit 
//{
//public:
//Willowisp():Spirit("Willowisp",40,12,1,35){}
//int attackPlayer() override 
//{ 
//return attackPower; 
//}
//bool drainsEnergy()const 
//{ 
//return true; 
//}
//};
//
//class StoneGolem:public Spirit 
//{
//public:
//StoneGolem():Spirit("Stone Golem",120,25,15,60){}
//int attackPlayer() override 
//{ 
//return attackPower; 
//}
//};
//
//class Emberwing:public Spirit 
//{
//public:
//Emberwing():Spirit("Emberwing",80,18,5,55) {}
//int attackPlayer() override 
//{ 
//return attackPower * 2; //Attack twice
//}
//};
//
//class PhantomWraith:public Spirit 
//{
//public:
//PhantomWraith():Spirit("Phantom Wraith",60,15,3,45) {}
//int attackPlayer() override 
//{ 
//return attackPower; 
//}
//bool stealsItems() const
//{ 
//return false; 
//}
//};
//
//class ForestCorruptor : public Spirit {
//public:
//    ForestCorruptor() : Spirit("Corrupted Dryad", 95, 20, 5, 90, true) {}
//    int attackPlayer() override { return attackPower; }
//};
//
//class TempleWarden : public Spirit {
//public:
//    TempleWarden() : Spirit("Temple Guardian", 140, 28, 12, 110, true) {}
//    int attackPlayer() override { return attackPower; }
//};
//
//class EmberTyrant : public Spirit {
//public:
//    EmberTyrant() : Spirit("Ember Tyrant", 120, 24, 8, 100, true) {}
//    int attackPlayer() override { return attackPower + rand() % 10; }
//};
//
//class ShadowSovereign:public Spirit 
//{
//private:
//int phase;
//int attackBoost;
//public:
//ShadowSovereign():Spirit("Shadow Sovereign",300,30,10,500,true),phase(1),attackBoost(0) {}
//int getPhase() const 
//{
//return phase; 
//}
//// Phase change at half health....POLYMORPHISM via virtual override
//void onHalfHealth() override 
//{
//if (phase==1) 
//{
//phase=2;
//attackBoost=15;
//attackPower+=attackBoost;
//}
//else if(phase==2&&hp<=maxHp/3) 
//{
//phase = 3;
//attackBoost=25;
//attackPower+=attackBoost;
//}
//}
//int attackPlayer() override 
//{
//if (phase==3) 
//return attackPower+(rand()%20);
//return attackPower;
//}
//};


//-----------------------------------------------------------------------------
// Spirits.h
// All enemy subclasses.
// Demonstrates INHERITANCE and POLYMORPHISM.
// Each subclass overrides attackPlayer() with unique behavior.
// Sprite-sheet frame indices are row-major (0-based).
// Adjust frame indices to match your actual sprite sheets.

#pragma once
#include "Spirit.h"
#include <cstdlib>

using namespace sf;

// ─────────────────────────────────────────────────────────────────────────────
// FOREST REGION ENEMIES
// ─────────────────────────────────────────────────────────────────────────────

class Willowisp : public Spirit
{
public:
    Willowisp() : Spirit("Willowisp", 40, 12, 1, 35)
    {
        std::unordered_map<CharacterState, std::vector<int>> anim;
        anim[CharacterState::Idle] = { 0, 1, 2, 3 };
        anim[CharacterState::Walk] = { 4, 5, 6, 7 };
        anim[CharacterState::Attack] = { 8, 9, 10 };
        anim[CharacterState::Dying] = { 12, 13, 14 };
        loadSheet("assets/characters/willowisp.png", { 96, 96 }, anim);
    }

    int attackPlayer() override { return attackPower; }

    // Willowisp drains spirit energy on attack.
    bool drainsEnergy() const { return true; }
};

// ─────────────────────────────────────────────────────────────────────────────
// TEMPLE REGION ENEMIES
// ─────────────────────────────────────────────────────────────────────────────

class StoneGolem : public Spirit
{
public:
    StoneGolem() : Spirit("Stone Golem", 120, 25, 15, 60)
    {
        std::unordered_map<CharacterState, std::vector<int>> anim;
        anim[CharacterState::Idle] = { 0, 1, 2, 3 };
        anim[CharacterState::Walk] = { 4, 5, 6, 7 };
        anim[CharacterState::Attack] = { 8, 9, 10, 11 };
        anim[CharacterState::Dying] = { 15, 16, 17 };
        loadSheet("assets/characters/stone_golem.png", { 128, 128 }, anim);
    }

    int attackPlayer() override { return attackPower; }
};

// ─────────────────────────────────────────────────────────────────────────────
// VOLCANO REGION ENEMIES
// ─────────────────────────────────────────────────────────────────────────────

class Emberwing : public Spirit
{
public:
    Emberwing() : Spirit("Emberwing", 80, 18, 5, 55)
    {
        std::unordered_map<CharacterState, std::vector<int>> anim;
        anim[CharacterState::Idle] = { 0, 1, 2 };
        anim[CharacterState::Walk] = { 3, 4, 5, 6 };
        anim[CharacterState::Attack] = { 7, 8, 9 };
        anim[CharacterState::Dying] = { 12, 13, 14 };
        // FIXED: was "embrewing.png" (typo), corrected to "emberwing.png"
        loadSheet("assets/characters/emberwing.png", { 80, 80 }, anim);
    }

    // Emberwing attacks twice per turn.
    int attackPlayer() override { return attackPower * 2; }
};

// ─────────────────────────────────────────────────────────────────────────────
// SHADOW REGION ENEMIES
// ─────────────────────────────────────────────────────────────────────────────

class PhantomWraith : public Spirit
{
public:
    PhantomWraith() : Spirit("Phantom Wraith", 60, 15, 3, 45)
    {
        std::unordered_map<CharacterState, std::vector<int>> anim;
        anim[CharacterState::Idle] = { 0, 1, 2, 3 };
        anim[CharacterState::Walk] = { 4, 5, 6, 7 };
        anim[CharacterState::Attack] = { 8, 9, 10 };
        anim[CharacterState::Steal] = { 12, 13, 14 };
        anim[CharacterState::Dying] = { 16, 17, 18 };
        loadSheet("assets/characters/phantom_wraith.png", { 96, 96 }, anim);
    }

    int attackPlayer() override { return attackPower; }

    // PhantomWraith can steal items from the player.
    bool stealsItems() const { return true; }
};

// ─────────────────────────────────────────────────────────────────────────────
// NEGOTIATION-ONLY SPIRITS (static, no combat animation needed)
// ─────────────────────────────────────────────────────────────────────────────

// GuidanceSpirit: only negotiates, never fights.
class GuidanceSpirit : public Spirit
{
public:
    GuidanceSpirit() : Spirit("Guidance Spirit", 1, 0, 0, 0)
    {
        std::unordered_map<CharacterState, std::vector<int>> anim;
        anim[CharacterState::Idle] = { 0, 1, 2 };
        anim[CharacterState::Negotiate] = { 3, 4, 5 };
        loadSheet("assets/characters/guidance_spirit.png", { 96, 96 }, anim);
    }

    // Guidance spirits do not attack.
    int attackPlayer() override { return 0; }
};

// Trickster: negotiates, never fights.
class Trickster : public Spirit
{
public:
    Trickster() : Spirit("Trickster", 1, 0, 0, 0)
    {
        std::unordered_map<CharacterState, std::vector<int>> anim;
        anim[CharacterState::Idle] = { 0, 1, 2 };
        anim[CharacterState::Negotiate] = { 3, 4, 5 };
        loadSheet("assets/characters/trickster.png", { 96, 96 }, anim);
    }

    int attackPlayer() override { return 0; }
};

// ─────────────────────────────────────────────────────────────────────────────
// BOSSES
// ─────────────────────────────────────────────────────────────────────────────

// Region 0 Boss: Forest Corrupted Dryad
class ForestCorruptor : public Spirit
{
public:
    ForestCorruptor() : Spirit("Corrupted Dryad", 95, 20, 5, 90, true)
    {
        std::unordered_map<CharacterState, std::vector<int>> anim;
        anim[CharacterState::Idle] = { 0, 1, 2, 3 };
        anim[CharacterState::Walk] = { 4, 5, 6, 7 };
        anim[CharacterState::Attack] = { 8, 9, 10 };
        anim[CharacterState::Dying] = { 12, 13, 14 };
        loadSheet("assets/characters/willowisp.png", { 128, 128 }, anim);
    }

    int attackPlayer() override { return attackPower; }
};

// Region 1 Boss: Temple Guardian
class TempleWarden : public Spirit
{
public:
    TempleWarden() : Spirit("Temple Guardian", 140, 28, 12, 110, true)
    {
        std::unordered_map<CharacterState, std::vector<int>> anim;
        anim[CharacterState::Idle] = { 0, 1, 2, 3 };
        anim[CharacterState::Walk] = { 4, 5, 6, 7 };
        anim[CharacterState::Attack] = { 8, 9, 10 };
        anim[CharacterState::Dying] = { 12, 13, 14 };
        loadSheet("assets/characters/stone_golem.png", { 128, 128 }, anim);
    }

    int attackPlayer() override { return attackPower; }
};

// Region 2 Boss: Ember Tyrant
class EmberTyrant : public Spirit
{
public:
    EmberTyrant() : Spirit("Ember Tyrant", 120, 24, 8, 100, true)
    {
        std::unordered_map<CharacterState, std::vector<int>> anim;
        anim[CharacterState::Idle] = { 0, 1, 2, 3 };
        anim[CharacterState::Walk] = { 4, 5, 6, 7 };
        anim[CharacterState::Attack] = { 8, 9, 10 };
        anim[CharacterState::Dying] = { 12, 13, 14 };
        // FIXED: was "embrewing.png" (typo), corrected to "emberwing.png"
        loadSheet("assets/characters/emberwing.png", { 128, 128 }, anim);
    }

    // EmberTyrant adds random bonus damage.
    int attackPlayer() override { return attackPower + rand() % 10; }
};

// Region 3 Boss: Shadow Sovereign (3 phases)
class ShadowSovereign : public Spirit
{
private:
    int phase;
    int attackBoost;

public:
    ShadowSovereign()
        : Spirit("Shadow Sovereign", 300, 30, 10, 500, true),
        phase(1), attackBoost(0)
    {
        std::unordered_map<CharacterState, std::vector<int>> anim;
        anim[CharacterState::Idle] = { 0, 1, 2, 3 };
        anim[CharacterState::Walk] = { 4, 5, 6, 7 };
        anim[CharacterState::Attack] = { 8, 9, 10 };
        anim[CharacterState::Dying] = { 12, 13, 14 };
        loadSheet("assets/characters/shadow_sovereign.png", { 128, 128 }, anim);
    }

    int getPhase() const { return phase; }

    // Phase change at half health — POLYMORPHISM via virtual override.
    void onHalfHealth() override
    {
        if (phase == 1)
        {
            phase = 2;
            attackBoost = 15;
            attackPower += attackBoost;
        }
        else if (phase == 2 && hp <= maxHp / 3)
        {
            phase = 3;
            attackBoost = 25;
            attackPower += attackBoost;
        }
    }

    // Phase 3 adds random chaos damage.
    int attackPlayer() override
    {
        if (phase == 3)
            return attackPower + rand() % 20;
        return attackPower;
    }
};