#pragma once
#include <string>


const int MAX_PLAYERS = 50;
const int MAX_NAME_LEN = 15;

struct PlayerRecord
{
    std::string name;
    int  r1Score;
    int  r2Score;
    int  r3Score;
    bool r1Done;
    bool r2Done;
    bool r3Done;

    PlayerRecord();

    // Sum of the per-round scores.
    int  totalScore() const;

    int  nextRound() const;

    // True if all three rounds are done.
    bool isComplete() const;
};


class ScoreBoard
{
private:
    PlayerRecord players[MAX_PLAYERS];
    int          count;

public:
    ScoreBoard();

    // Load from "players.txt".  Missing file => count = 0.
    void load();

    // Truncate-and-write the entire roster to "players.txt".
    void save() const;

    // Wipe in-memory roster AND the file.
    void clearAll();

    // -1 if not found.
    int  findIndex(const std::string& name) const;

    int  addOrReplace(const std::string& name);

    // Read-only access used by UI code.
    int  getCount() const { return count; }
    const PlayerRecord& get(int idx) const { return players[idx]; }

    // Mutable access used to update scores after winning a round.
    PlayerRecord& at(int idx) { return players[idx]; }

    // Number of players who have NOT cleared all three rounds.
    int  countIncomplete() const;

    int  getIncompleteIndex(int nthIncomplete) const;
};

