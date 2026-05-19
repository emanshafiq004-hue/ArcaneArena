//------------------------------------- ( Region.cpp ) -----------------------------------------
#include "Region.h"
#include "Spirits.h"
#include <cstdlib>
#include <stdexcept>

//Constructor:-
Region::Region(const string& n, const Color& color, int idx): name(n), themeColor(color), regionIndex(idx), currentEncounterIndex(0), completed(false) 
{}

//Add Encounter:-
void Region::addEncounter(const Encounter& enc) 
{
encounters.push_back(enc);
}

//Check on Current Encounter Index..
const Encounter& Region::peekEncounter() const 
{
if (currentEncounterIndex >= static_cast<int>(encounters.size()))
throw runtime_error("Region: no encounter to peek");
return encounters[currentEncounterIndex];
}

//Advance Encounter..
void Region::advanceEncounter() 
{
if (currentEncounterIndex < static_cast<int>(encounters.size()))
currentEncounterIndex++;
}

//Undo Last Encounter..
void Region::undoLastEncounter()
{
if (currentEncounterIndex > 0)
currentEncounterIndex--;
}

//Getters:-
int Region::getEncounterIndex() const 
{
return currentEncounterIndex;
}

//Total Encounter in a region...
int Region::getEncounterCount() const 
{
return static_cast<int>(encounters.size());
}

//Region Complete?
bool Region::isComplete() const
{
    return completed;
}

//Get Name..
string Region::getName() const
{
return name;
}


Color Region::getThemeColor() const
{
    return themeColor;
}


int Region::getIndex() const 
{
return regionIndex;
}

int Region::getRemainingEncounters() const 
{
return static_cast<int>(encounters.size()) - currentEncounterIndex;
}

//Setters:-
void Region::setEncounterIndex(int idx) 
{
if (idx < 0)
idx = 0;
if (idx > static_cast<int>(encounters.size()))
idx = static_cast<int>(encounters.size());
currentEncounterIndex = idx;
}

void Region::markComplete() 
{
completed = true;
}

//Create Boss:-
Spirit* Region::createBoss() const 
{
switch (regionIndex) 
{
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

//Create Enemy:-
Spirit* Region::createEnemy() const
{
int i = currentEncounterIndex;
if (regionIndex == 0) // Forest
{
if (encounters[i].type == EncounterType::Battle)
return new Willowisp();
else if (encounters[i].type == EncounterType::PhantomSurvival)
return new PhantomWraith();
else if (encounters[i].type == EncounterType::BossBattle)
return createBoss();
else
return new Willowisp();
}
else if (regionIndex == 1) // Temple
{
if (encounters[i].type == EncounterType::Battle)
return new StoneGolem();
else if (encounters[i].type == EncounterType::PhantomSurvival)
return new PhantomWraith();
else if (encounters[i].type == EncounterType::BossBattle)
return createBoss();
else
return new StoneGolem();
}
else if(regionIndex == 2) // Mountains
{
if (encounters[i].type == EncounterType::Battle)
return new Emberwing();
else if (encounters[i].type == EncounterType::PhantomSurvival)
return new PhantomWraith();
else if (encounters[i].type == EncounterType::BossBattle)
return createBoss();
else
return new Emberwing();
}
else if(regionIndex == 3) // Veil Gate
{
if (encounters[i].type == EncounterType::Battle)
return new Shadow();
else if (encounters[i].type == EncounterType::PhantomSurvival)
return new PhantomWraith();
else if (encounters[i].type == EncounterType::BossBattle)
return createBoss();
else
return new Shadow();
}
else
throw runtime_error("Unknown region index for random enemy!");
}

