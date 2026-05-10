#pragma once
#include <string>

// =======================================================================
//  Scoreboard / save data
//
//  Each player has one PlayerRecord stored in players.txt, one record
//  per line, fields separated by '|':
//
//     name|r1Score|r2Score|r3Score|r1Done|r2Done|r3Done
//
//  Round scores default to 0; "done" flags are true only when the
//  player CLEARS that round.  Quitting via ESC or dying does NOT mark
//  the round done, so the player can later continue from that round.
// =======================================================================

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

    // The round number this player should play next:
    //   1 = haven't cleared round 1 yet
    //   2 = round 1 done, haven't cleared round 2
    //   3 = rounds 1-2 done, haven't cleared round 3
    //   4 = ALL rounds cleared (game complete)
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

    // Insert a fresh record (or overwrite an existing one with the same
    // name, resetting all scores).  Returns the record's index in the
    // array, or -1 if the roster is full.
    int  addOrReplace(const std::string& name);

    // Read-only access used by UI code.
    int  getCount() const { return count; }
    const PlayerRecord& get(int idx) const { return players[idx]; }

    // Mutable access used to update scores after winning a round.
    PlayerRecord& at(int idx) { return players[idx]; }

    // Number of players who have NOT cleared all three rounds.
    int  countIncomplete() const;

    // Map "n-th incomplete player" (0-based) -> index into players[].
    // Returns -1 if n is out of range.
    int  getIncompleteIndex(int nthIncomplete) const;
};

