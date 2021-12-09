#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_map>

/* SEGMENTS_REQUIRED_BY_EACH_NUMBER
 * 0 -> 6
 * 1 -> 2 // Unique
 * 2 -> 5
 * 3 -> 5
 * 4 -> 4 // Unique
 * 5 -> 5
 * 6 -> 6
 * 7 -> 3 // Unique
 * 8 -> 7 // Unique
 * 9 -> 6
 */

constexpr int MAX_CHARACTERS = 26;
constexpr int TOTAL_VALUES = 10;

struct Entry {
    std::vector<std::string> signal_patterns;
    std::vector<std::string> output_value;
};

inline void split(const std::string &string, char delim, std::vector<std::string> &entries) {
    size_t start;
    size_t end = 0;
    while ((start = string.find_first_not_of(delim, end)) != std::string::npos) {
        end = string.find(delim, start);
        std::string entry = string.substr(start, end - start);
        entries.emplace_back(entry);
    }
}

void readEntries(std::string_view filepath, std::vector<Entry> &entries) {
    std::ifstream file(filepath.data());
    std::string line;

    while (std::getline(file, line)) {
        std::vector<std::string> inputs;
        split(line, ' ', inputs);
        auto index_iterator = inputs.begin();
        Entry entry{
            .signal_patterns=std::vector<std::string>(index_iterator, index_iterator + 10),
            .output_value=std::vector<std::string>(index_iterator + 11, index_iterator + 15)
                    };
        entries.emplace_back(entry);
    }

}

int totalCommonCharacters(std::string &string_1, std::string &string_2) {
    int map_1[MAX_CHARACTERS], map_2[MAX_CHARACTERS];

    for (int i = 0; i < MAX_CHARACTERS; i++) {
        map_1[i] = 0;
        map_2[i] = 0;
    }

    int common_characters = 0;

    for (auto &c_1 : string_1) {
        map_1[c_1 - 'a']++;
    }
    for (auto &c_2 : string_2) {
        map_2[c_2 - 'a']++;
    }

    for (int i = 0; i < MAX_CHARACTERS; i++) {
        if (map_1[i] && map_2[i]) {
            common_characters++;
        }
    }

    return common_characters;
}

int main() {
    std::string filepath = "input.txt";

    std::vector<Entry> entries;
    readEntries(filepath, entries);

    // Part - 1
    int total_appearances = 0;
    // Count 1, 4, 7 and 8 by their segments
    for (auto &entry : entries) {
        for (auto &value : entry.output_value) {
            switch (value.size()) {
                case 2: // 1
                case 4: // 4
                case 3: // 7
                case 7: // 8
                {
                    total_appearances++;
                    break;
                }
                default: break;
            }
        }
    }
    std::cout << "Total appearances for 1, 4, 7 and 8: " << total_appearances << std::endl;

    // Part 2

    std::unordered_map<int, std::string> mapping;
    int sum = 0;

    for (auto &entry : entries) {
        for (auto pattern : entry.signal_patterns) {
            std::sort(pattern.begin(), pattern.end());
            switch (pattern.size()) {
                case 2: mapping[1] = pattern; break;
                case 4: mapping[4] = pattern; break;
                case 3: mapping[7] = pattern; break;
                case 7: mapping[8] = pattern; break;
                default: break;
            }
        }

        for (auto pattern : entry.signal_patterns) {
            std::sort(pattern.begin(), pattern.end());
            switch (pattern.size()) {
                case 5: {
                    if (totalCommonCharacters(mapping[1], pattern) == 2) {
                        mapping[3] = pattern;
                    } else if (totalCommonCharacters(mapping[4], pattern) == 3) {
                        mapping[5] = pattern;
                    } else {
                        mapping[2] = pattern;
                    }
                    break;
                }
                case 6: {
                    if (totalCommonCharacters(mapping[4], pattern) == 4) {
                        mapping[9] = pattern;
                    } else if (totalCommonCharacters(mapping[1], pattern) == 2) {
                        mapping[0] = pattern;
                    } else {
                        mapping[6] = pattern;
                    }
                    break;
                }
                default: break;
            }
        }

        std::stringstream ss;

        for (auto value : entry.output_value) {
            std::sort(value.begin(), value.end());
            for (int i = 0; i < TOTAL_VALUES; i++) {
                if (mapping[i] == value) {
                    ss << i;
                }
            }
        }

        std::cout << "Output: " << ss.str() << std::endl;
        sum += std::stoi(ss.str());
    }
    std::cout << "Total Output Sum: " << sum << std::endl;
}