#include "FileManager.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdint>
#include <vector>
#include <cstring>
#include <cerrno>
#include <direct.h>

using namespace std;

static uint32_t fnv1a32(const string& s) {
    uint32_t hash = 2166136261u;
    for (unsigned char c : s) {
        hash ^= c;
        hash *= 16777619u;
    }
    return hash;
}

static bool isPathSep(char c) {
    return c == '/' || c == '\\';
}

static void makeDirs(const string& dir) {
    if (dir.empty())
        return;

    string path;
    path.reserve(dir.size());
    for (size_t i = 0; i < dir.size(); i++) {
        const char c = dir[i];
        path.push_back(c);
        const bool isEnd = (i + 1 == dir.size());
        if (isEnd || isPathSep(c)) {
            if (!path.empty() && !isPathSep(path.back())) {
                const int rc = _mkdir(path.c_str());
                if (rc != 0 && errno != EEXIST) {
                    throw runtime_error("Cannot create directory: " + path);
                }
            }
        }
    }

    if (!path.empty() && !isPathSep(path.back())) {
        const int rc = _mkdir(path.c_str());
        if (rc != 0 && errno != EEXIST) {
            throw runtime_error("Cannot create directory: " + path);
        }
    }
}

static void ensureParentDirExists(const string& filename) {
    size_t cut = filename.find_last_of("/\\");
    if (cut == string::npos)
        return;
    makeDirs(filename.substr(0, cut));
}

static void atomicWriteTextFile(const string& filename, const string& contents) {
    ensureParentDirExists(filename);
    const string tmp = filename + ".tmp";
    {
        ofstream file(tmp, ios::binary | ios::trunc);
        if (!file.is_open())
            throw runtime_error("Cannot open temp save file for writing: " + tmp);
        file.write(contents.data(), static_cast<std::streamsize>(contents.size()));
        file.flush();
        if (!file.good())
            throw runtime_error("Failed writing temp save file: " + tmp);
    }
    if (std::rename(tmp.c_str(), filename.c_str()) != 0) {
        std::remove(filename.c_str());
        if (std::rename(tmp.c_str(), filename.c_str()) != 0) {
            throw runtime_error("Cannot finalize save file: " + filename);
        }
    }
}

void FileManager::savePlayer(const string& playerData, const string& questData,const string& filename) {
    // Save format v2 (text, versioned, integrity-checked, atomic):
    // ARCANEAR_SAVE_V2\n
    // player_len=<n>\n
    // quest_len=<m>\n
    // data_hash=<hex>\n
    // <player bytes><quest bytes>
    const string header = "ARCANEAR_SAVE_V2\n";
    const string payload = playerData + questData;
    const uint32_t hash = fnv1a32(payload);

    ostringstream oss;
    oss << header;
    oss << "player_len=" << playerData.size() << "\n";
    oss << "quest_len=" << questData.size() << "\n";
    oss << "data_hash=" << std::hex << hash << std::dec << "\n";
    oss << playerData;
    oss << questData;

    atomicWriteTextFile(filename, oss.str());
}

FileManager::LoadStatus FileManager::loadPlayer(string& outPlayerData, string& outQuestData,const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open())
        return LoadStatus::NotFound;

    // Peek first line to decide format.
    string firstLine;
    if (!getline(file, firstLine))
        return LoadStatus::Corrupt;

    if (firstLine == "ARCANEAR_SAVE_V2") {
        string playerLenLine, questLenLine, hashLine;
        if (!getline(file, playerLenLine) || !getline(file, questLenLine) || !getline(file, hashLine))
            return LoadStatus::Corrupt;

        const auto parseLen = [](const string& line, const string& key, size_t& out) -> bool {
            if (line.rfind(key, 0) != 0)
                return false;
            try {
                out = static_cast<size_t>(stoull(line.substr(key.size())));
                return true;
            } catch (...) {
                return false;
            }
        };

        size_t playerLen = 0, questLen = 0;
        if (!parseLen(playerLenLine, "player_len=", playerLen))
            return LoadStatus::Corrupt;
        if (!parseLen(questLenLine, "quest_len=", questLen))
            return LoadStatus::Corrupt;
        if (hashLine.rfind("data_hash=", 0) != 0)
            return LoadStatus::Corrupt;

        uint32_t expectedHash = 0;
        try {
            expectedHash = static_cast<uint32_t>(stoul(hashLine.substr(strlen("data_hash=")), nullptr, 16));
        } catch (...) {
            return LoadStatus::Corrupt;
        }

        vector<char> buf;
        buf.resize(playerLen + questLen);
        file.read(buf.data(), static_cast<std::streamsize>(buf.size()));
        if (file.gcount() != static_cast<std::streamsize>(buf.size()))
            return LoadStatus::Corrupt;

        outPlayerData.assign(buf.data(), playerLen);
        outQuestData.assign(buf.data() + playerLen, questLen);

        const string payload = outPlayerData + outQuestData;
        if (fnv1a32(payload) != expectedHash)
            return LoadStatus::Corrupt;

        return LoadStatus::Ok;
    }

    // Legacy v1 format (two lines):
    // <player line>\n
    // <quest line>\n
    // (First line already read into firstLine)
    outPlayerData = firstLine;
    if (!getline(file, outQuestData))
        return LoadStatus::Corrupt;
    return LoadStatus::Ok;
}

void FileManager::saveScore(const string& playerName, int level, int xp,const string& filename) {
    ensureParentDirExists(filename);
    ofstream file(filename, ios::app);
    if (!file.is_open())
        throw runtime_error("Cannot open scores file!");
    file << playerName << "," << level << "," << xp << "\n";
    file.close();
}

string FileManager::loadScores(const string& filename) {
    ifstream file(filename);
    if (!file.is_open())
        return "No scores found yet.";
    ostringstream oss;
    string line;
    int rank = 1;
    while (getline(file, line)) {
        oss << rank++ << ". " << line << "\n";
    }
    return oss.str();
}

bool FileManager::saveExists(const string& filename) {
    ifstream file(filename);
    return file.good();
}