#pragma once
#include <string>
#include <vector>

using namespace std;

class QuestLog {
private:
vector<bool> regionCompleted;
vector<int> encounterProgress;
int currentRegion;
string storyIntro;
public:
QuestLog();
void loadStory(const string& filepath);
const string& getStoryIntro() const;
void completeRegion(int regionIndex);
bool isRegionComplete(int regionIndex) const;
bool isRegionUnlocked(int regionIndex) const;
int getCurrentRegion() const;
void setCurrentRegion(int r);
bool isGameComplete() const;
int getEncounterProgress(int regionIndex) const;
void setEncounterProgress(int regionIndex, int encounterIndex);
void resetEncounterProgress(int regionIndex);
string serialize() const;
void deserialize(const string& data);
};