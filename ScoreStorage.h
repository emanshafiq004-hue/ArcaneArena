#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

//  SHATTER SCORE ENTRY STRUCT holds one player's Shatter Shift record
struct ShatterScoreEntry
{
    string name;   // player name
    int level;     // highest level reached
    int score;     // final score
    // Constructor for easy creation
    ShatterScoreEntry(const string& n, int lvl, int sc) : name(n), level(lvl), score(sc) {}
    // Default constructor
    ShatterScoreEntry() : name("---"), level(0), score(0) {}
};

// Centers 'text' inside a column of 'width' characters
string centerText(const string& text, int width);

class ShatterScoreManager
{
private:
    static const int MAX_ENTRIES = 5;
    static const string FILE_PATH;
    vector<ShatterScoreEntry> entries;
    void sortEntries();
    void trimToTop5();
public:
    ShatterScoreManager();
    void load();
    void addScore(const string& name, int level, int score);
    int  getHighestScore() const;
    const vector<ShatterScoreEntry>& getEntries() const;
    void save() const;
};