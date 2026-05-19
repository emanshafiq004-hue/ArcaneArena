// Region.h

#pragma once
#include <SFML/Graphics.hpp>
#include<iostream>
#include <string>
#include <vector>
#include "Spirit.h"
using namespace sf;
using namespace std;

enum class EncounterType {
    Battle,
    Shrine,
    BossBattle,
    PhantomSurvival,
    Negotiation,
    Trickster,
    GateChoice
};

struct Encounter {
    EncounterType type = EncounterType::Battle;
    string description;
    bool leftGateIsTrap = true;
};

class Region {
private:
    string name;
    Color themeColor;
    int regionIndex;
    vector<Encounter> encounters;
    int currentEncounterIndex;
    bool completed;
public:
    Region(const string& name, const Color& color, int index);
    void addEncounter(const Encounter& enc);
    const Encounter& peekEncounter() const;
    void advanceEncounter();
    void undoLastEncounter();
    int getEncounterIndex() const;
    void setEncounterIndex(int idx);
    int getEncounterCount() const;
    bool isComplete() const;
    void markComplete();
    string getName() const;
    Color getThemeColor() const;
    int getIndex() const;
    int getRemainingEncounters() const;
    Spirit* createBoss() const;
    Spirit* createRandomEnemy() const;
    Spirit* createEnemyForSlot(int slotIndex) const;
};
