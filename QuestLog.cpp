#include "QuestLog.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace std;

QuestLog::QuestLog() : currentRegion(0) {
    regionCompleted.assign(4, false);
    encounterProgress.assign(4, 0);
}

void QuestLog::loadStory(const string& filepath) {
    ifstream file(filepath);
    if (!file.is_open())
        throw runtime_error("Cannot open story file: " + filepath);
    ostringstream oss;
    string line;
    while (getline(file, line))
        oss << line << "\n";
    storyIntro = oss.str();
}

const string& QuestLog::getStoryIntro() const {
    return storyIntro;
}

void QuestLog::completeRegion(int r) {
    if (r >= 0 && r < 4)
        regionCompleted[r] = true;
}

bool QuestLog::isRegionComplete(int r) const {
    if (r < 0 || r >= 4)
        return false;
    return regionCompleted[r];
}

bool QuestLog::isRegionUnlocked(int r) const {
    if (r == 0)
        return true;
    return regionCompleted[r - 1];
}

int QuestLog::getCurrentRegion() const {
    return currentRegion;
}

void QuestLog::setCurrentRegion(int r) {
    currentRegion = r;
}

bool QuestLog::isGameComplete() const {
    for (bool b : regionCompleted)
        if (!b)
            return false;
    return true;
}

int QuestLog::getEncounterProgress(int regionIndex) const {
    if (regionIndex < 0 || regionIndex >= 4)
        return 0;
    return encounterProgress[regionIndex];
}

void QuestLog::setEncounterProgress(int regionIndex, int encounterIndex) {
    if (regionIndex >= 0 && regionIndex < 4 && encounterIndex >= 0)
        encounterProgress[regionIndex] = encounterIndex;
}

void QuestLog::resetEncounterProgress(int regionIndex) {
    if (regionIndex >= 0 && regionIndex < 4)
        encounterProgress[regionIndex] = 0;
}

string QuestLog::serialize() const {
    ostringstream oss;
    oss << currentRegion;
    for (bool b : regionCompleted)
        oss << "|" << (b ? 1 : 0);
    for (int i = 0; i < 4; i++)
        oss << "|" << encounterProgress[i];
    return oss.str();
}

void QuestLog::deserialize(const string& data) {
    istringstream ss(data);
    string token;
    vector<string> parts;
    while (getline(ss, token, '|'))
        parts.push_back(token);
    if (parts.empty())
        return;
    currentRegion = stoi(parts[0]);
    for (int i = 0; i < 4; i++) {
        if (static_cast<int>(parts.size()) > 1 + i)
            regionCompleted[i] = (stoi(parts[1 + i]) == 1);
    }
    encounterProgress.assign(4, 0);
    const int base = 5;
    for (int i = 0; i < 4; i++) {
        if (static_cast<int>(parts.size()) > base + i)
            encounterProgress[i] = stoi(parts[base + i]);
    }
}