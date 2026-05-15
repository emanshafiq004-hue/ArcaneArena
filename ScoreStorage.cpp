#include "ScoreStorage.h"

const std::string ShatterScoreManager::FILE_PATH = "shattershift_scores.txt";
// The divider line â€” written once here, used directly in save()
const std::string DIVIDER = "+------+---------------+-------+----------+";

std::string centerText(const std::string& text, int width)
{
    int totalPadding = width - (int)text.size();

    int leftSpaces = totalPadding / 2;
    int rightSpaces = totalPadding - leftSpaces;

    if (leftSpaces < 0) leftSpaces = 0;
    if (rightSpaces < 0) rightSpaces = 0;

    return std::string(leftSpaces, ' ') + text + std::string(rightSpaces, ' ');
}

//  CONSTRUCTOR
ShatterScoreManager::ShatterScoreManager()
{
    load();
}

//  LOAD and reads scores from the saved table
void ShatterScoreManager::load()
{
    entries.clear();
    std::ifstream file(FILE_PATH);
    if (!file.is_open())
    {
        std::cout << "[ShatterScoreManager] No scores file found. Starting fresh.\n";
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        // Only look at lines that start and end with '|'
        if (line.empty() || line.front() != '|' || line.back() != '|')
            continue;

        // Strip all '|' characters out of the line
        std::string clean = "";
        for (int i = 0; i < (int)line.size(); i++)
        {
            if (line[i] != '|')
                clean += line[i];
        }

        // Now clean looks like: "  1     Ahmed     5     6910  "
        std::istringstream iss(clean);
        int rank, level, score;
        std::string name;

        if (!(iss >> rank >> name >> level >> score))
            continue;

        // Skip empty placeholder rows
        if (name == "---")
            continue;

        entries.emplace_back(name, level, score);
    }

    file.close();
    sortEntries();
    std::cout << "[ShatterScoreManager] Loaded " << entries.size() << " score(s).\n";
}

//  ADD SCORE
void ShatterScoreManager::addScore(const std::string& name, int level, int score)
{
    entries.emplace_back(name, level, score);
    sortEntries();
    trimToTop5();
    save();
    std::cout << "[ShatterScoreManager] Saved: " << name
        << " | Level " << level << " | " << score << " pts\n";
}

//  GET HIGHEST SCORE
int ShatterScoreManager::getHighestScore() const
{
    if (entries.empty()) return 0;
    return entries[0].score;
}

//  GET ALL ENTRIES
const std::vector<ShatterScoreEntry>& ShatterScoreManager::getEntries() const
{
    return entries;
}

//  SORT bubble sort, highest score first
void ShatterScoreManager::sortEntries()
{
    int n = entries.size();
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - 1 - i; j++)
        {
            if (entries[j].score < entries[j + 1].score)
            {
                ShatterScoreEntry temp = entries[j];
                entries[j] = entries[j + 1];
                entries[j + 1] = temp;
            }
        }
    }
}

//  TRIM and keep only top 5
void ShatterScoreManager::trimToTop5()
{
    if ((int)entries.size() > MAX_ENTRIES)
        entries.resize(MAX_ENTRIES);
}

//  SAVE and writes a clean table to scores.txt
void ShatterScoreManager::save() const
{
    std::ofstream file(FILE_PATH);
    if (!file.is_open())
    {
        std::cout << "[ShatterScoreManager] ERROR: Could not open scores.txt!\n";
        return;
    }

    const int W_RANK = 6;
    const int W_NAME = 15;
    const int W_LEVEL = 7;
    const int W_SCORE = 10;

    file << "=====================================================\n";
    file << "|       SHATTER SHIFT  -  HIGH SCORES               |\n";
    file << "=====================================================\n";
    file << "|       Top " << MAX_ENTRIES << " players are saved here.               |\n";
    file << "=====================================================\n\n";

    file << DIVIDER << "\n";
    file << "|" << centerText("Rank", W_RANK)
        << "|" << centerText("Name", W_NAME)
        << "|" << centerText("Level", W_LEVEL)
        << "|" << centerText("Score", W_SCORE) << "|\n";
    file << DIVIDER << "\n";

    for (int i = 0; i < MAX_ENTRIES; i++)
    {
        if (i < (int)entries.size())
        {
            file << "|" << centerText(std::to_string(i + 1), W_RANK)
                << "|" << centerText(entries[i].name, W_NAME)
                << "|" << centerText(std::to_string(entries[i].level), W_LEVEL)
                << "|" << centerText(std::to_string(entries[i].score), W_SCORE)
                << "|\n";
        }
        else
        {
            file << "|" << centerText(std::to_string(i + 1), W_RANK)
                << "|" << centerText("---", W_NAME)
                << "|" << centerText("-", W_LEVEL)
                << "|" << centerText("---", W_SCORE) << "|\n";
        }
    }

    //Table footer 
    file << DIVIDER << "\n";
    file << "| Scores sorted highest to lowest. Top "
        << MAX_ENTRIES << " only.      |\n";
    file << "=====================================================\n";
    file.close();
    std::cout << "[ShatterScoreManager] scores.txt saved.\n";
}