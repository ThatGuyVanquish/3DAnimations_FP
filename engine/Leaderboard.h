#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

class Leaderboard {
public:
    struct Entry {
        std::string name;
        int points;

        bool operator<(const Entry& other) const {
            return points > other.points;
        }
    };
    Leaderboard() {};
    Leaderboard(const std::string& fileName) : m_fileName(fileName) {
        load();
    }

    void Init(char* fileName)
    {
        m_fileName = fileName;
        load();
    }

    void add(const std::string& name, int points) {
        m_entries.push_back({ name, points });
        sort();
        while (m_entries.size() > 10) {
            m_entries.pop_back();
        }
        save();
    }

    std::vector<Entry> getEntries() const {
        return m_entries;
    }

private:
    void load() {
        std::ifstream file(m_fileName);
        if (!file.is_open()) {
            return;
        }

        std::string name;
        int points;
        while (file >> name >> points) {
            m_entries.push_back({ name, points });
        }

        sort();
        while (m_entries.size() > 10) {
            m_entries.pop_back();
        }
    }

    void save() {
        std::ofstream file(m_fileName);
        if (!file.is_open()) {
            return;
        }

        for (const auto& entry : m_entries) {
            file << entry.name << ' ' << entry.points << '\n';
        }
    }

    void sort() {
        std::sort(m_entries.begin(), m_entries.end());
    }

    std::string m_fileName;
    std::vector<Entry> m_entries;
};
