#include "scoreboard.h"
#include <fstream>
#include <cstdlib>   
using namespace std;

// =======================================================================
//  PlayerRecord
// =======================================================================
PlayerRecord::PlayerRecord()
{
    name = "";
    r1Score = 0;
    r2Score = 0;
    r3Score = 0;
    r1Done = false;
    r2Done = false;
    r3Done = false;
}

int PlayerRecord::totalScore() const
{
    return r1Score + r2Score + r3Score;
}

int PlayerRecord::nextRound() const
{
    if (!r1Done) return 1;
    if (!r2Done) return 2;
    if (!r3Done) return 3;
    return 4;   
}

bool PlayerRecord::isComplete() const
{
    return r1Done && r2Done && r3Done;
}


// =======================================================================
//  Internal helpers
// =======================================================================

static bool parseLine(const string& line, PlayerRecord& out)
{
    if (line.length() == 0) return false;

    string fields[7];
    int    fieldCount = 0;
    int    fieldStart = 0;
    int    n = (int)line.length();

    for (int i = 0; i <= n; i++)
    {
        if (i == n || line[i] == '|')
        {
            if (fieldCount >= 7) return false;
            fields[fieldCount++] = line.substr(fieldStart, i - fieldStart);
            fieldStart = i + 1;
        }
    }
    if (fieldCount != 7) return false;

    out.name = fields[0];
    out.r1Score = atoi(fields[1].c_str());
    out.r2Score = atoi(fields[2].c_str());
    out.r3Score = atoi(fields[3].c_str());
    out.r1Done = (atoi(fields[4].c_str()) != 0);
    out.r2Done = (atoi(fields[5].c_str()) != 0);
    out.r3Done = (atoi(fields[6].c_str()) != 0);

    if (out.name.length() == 0) return false;
    return true;
}

// =======================================================================
//  ScoreBoard
// =======================================================================

ScoreBoard::ScoreBoard()
{
    count = 0;
}

void ScoreBoard::load()
{
    count = 0;
    ifstream file("players.txt");
    if (!file.is_open()) return;

    string line;
    while (getline(file, line) && count < MAX_PLAYERS)
    {
        PlayerRecord rec;
        if (parseLine(line, rec))
        {
            players[count] = rec;
            count++;
        }
    }
    file.close();
}

void ScoreBoard::save() const
{
    ofstream file("players.txt", ios::trunc);
    if (!file.is_open()) return;

    for (int i = 0; i < count; i++)
    {
        const PlayerRecord& p = players[i];
        file << p.name << '|'
            << p.r1Score << '|' << p.r2Score << '|' << p.r3Score << '|'
            << (p.r1Done ? 1 : 0) << '|'
            << (p.r2Done ? 1 : 0) << '|'
            << (p.r3Done ? 1 : 0) << '\n';
    }
    file.close();
}

void ScoreBoard::clearAll()
{
    count = 0;
    ofstream wipe("players.txt", ios::trunc);
    wipe.close();
}

int ScoreBoard::findIndex(const string& name) const
{
    for (int i = 0; i < count; i++)
    {
        if (players[i].name == name) return i;
    }
    return -1;
}

int ScoreBoard::addOrReplace(const string& name)
{
    int idx = findIndex(name);
    if (idx >= 0)
    {
        players[idx] = PlayerRecord();
        players[idx].name = name;
        return idx;
    }

    if (count >= MAX_PLAYERS) return -1;

    PlayerRecord rec;
    rec.name = name;
    players[count] = rec;
    return count++;
}

int ScoreBoard::countIncomplete() const
{
    int n = 0;
    for (int i = 0; i < count; i++)
        if (!players[i].isComplete()) n++;
    return n;
}

int ScoreBoard::getIncompleteIndex(int nthIncomplete) const
{
    int seen = 0;
    for (int i = 0; i < count; i++)
    {
        if (!players[i].isComplete())
        {
            if (seen == nthIncomplete) return i;
            seen++;
        }
    }
    return -1;
}
