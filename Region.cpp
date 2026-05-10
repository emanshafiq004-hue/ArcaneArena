// Region.cpp
#include "Region.h"
#include "Spirits.h"
#include <cstdlib>
#include <stdexcept>

Region::Region(const string& n, const Color& color, int idx)
    : name(n), themeColor(color), regionIndex(idx), currentEncounterIndex(0), completed(false) {
}

void Region::addEncounter(const Encounter& enc) {
    encounters.push_back(enc);
}

const Encounter& Region::peekEncounter() const {
    if (currentEncounterIndex >= static_cast<int>(encounters.size()))
        throw runtime_error("Region: no encounter to peek");
    return encounters[currentEncounterIndex];
}

void Region::advanceEncounter() {
    if (currentEncounterIndex < static_cast<int>(encounters.size()))
        currentEncounterIndex++;
}

void Region::undoLastEncounter() {
    if (currentEncounterIndex > 0)
        currentEncounterIndex--;
}

int Region::getEncounterIndex() const {
    return currentEncounterIndex;
}

void Region::setEncounterIndex(int idx) {
    if (idx < 0)
        idx = 0;
    if (idx > static_cast<int>(encounters.size()))
        idx = static_cast<int>(encounters.size());
    currentEncounterIndex = idx;
}

int Region::getEncounterCount() const {
    return static_cast<int>(encounters.size());
}

bool Region::isComplete() const {
    return completed;
}

void Region::markComplete() {
    completed = true;
}

string Region::getName() const {
    return name;
}

Color Region::getThemeColor() const {
    return themeColor;
}

int Region::getIndex() const {
    return regionIndex;
}

int Region::getRemainingEncounters() const {
    return static_cast<int>(encounters.size()) - currentEncounterIndex;
}

Spirit* Region::createBoss() const {
    switch (regionIndex) {
    case 0:
        return new ForestCorruptor();
    case 1:
        return new TempleWarden();
    case 2:
        return new EmberTyrant();
    case 3:
        return new ShadowSovereign();
    default:
        throw runtime_error("Unknown region index for boss!");
    }
}

Spirit* Region::createRandomEnemy() const {
    int roll = rand() % 3;
    if (roll == 0)
        return new PhantomWraith();
    switch (regionIndex) {
    case 0:
        return new Willowisp();
    case 1:
        return new StoneGolem();
    case 2:
        return new Emberwing();
    case 3:
        return new PhantomWraith();
    default:
        return new Willowisp();
    }
}

Spirit* Region::createEnemyForSlot(int slotIndex) const {
    (void)slotIndex;
    return createRandomEnemy();
}
