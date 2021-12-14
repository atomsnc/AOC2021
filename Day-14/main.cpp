#include <iostream>
#include <utility>
#include <list>
#include <fstream>
#include <chrono>
#include <unordered_map>
#include <limits>

void readEntries(std::string_view filepath,
                 std::list<char> &polymer,
                 std::unordered_map<std::string, char> &pair_insertions) {
    std::ifstream file(filepath.data());
    std::string line;

    std::getline(file, line);

    for (auto &c: line) {
        polymer.emplace_back(c);
    }

    /* Empty line */
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::string pair(2, 'x');
        char insertion;
        std::sscanf(line.c_str(), "%c%c -> %c", &pair[0], &pair[1], &insertion);
        pair_insertions.emplace(pair, insertion);
    }
}

void printPolymer(std::list<char> &polymer) {
    for (auto &c: polymer) {
        std::cout << c;
    }
    std::cout << std::endl;
}

void printPairCollection(std::unordered_map<std::string, unsigned long> &pair_collection) {
    for (auto &[pair, count]: pair_collection) {
        std::cout << pair << " -> " << count << std::endl;
    }
}

int main() {
    std::string filepath = "input.txt";
    std::list<char> polymer;
    std::unordered_map<std::string, char> pair_insertions;

    readEntries(filepath, polymer, pair_insertions);

    std::unordered_map<std::string, unsigned long> pair_collection;

    for (auto first = polymer.begin(), second = ++polymer.begin(); second != polymer.end(); first++, second++) {
        std::string pair = std::string() + *first + *second;
        pair_collection[pair]++;
    }

    /* This is needed because the last character is counted n - 1 times
     * as we just take the first char from all the pair orderings */
    char last_char = polymer.back();

    for (int step = 0; step < 40; step++) {
        std::unordered_map<std::string, unsigned long> new_pairs;
        for (auto &[pair, count]: pair_collection) {
            auto insertion_it = pair_insertions.find(pair);
            if (insertion_it != pair_insertions.end()) {
                std::string first_new_pair = std::string() + pair[0] + insertion_it->second;
                std::string second_new_pair = std::string() + insertion_it->second + pair[1];

                new_pairs[first_new_pair] += count;
                new_pairs[second_new_pair] += count;
            }
        }
        pair_collection.clear();
        for (auto &[pair, count]: new_pairs) {
            pair_collection[pair] = count;
        }
    }

    unsigned long smallest_character_count = std::numeric_limits<unsigned long>::max();
    unsigned long largest_character_count = 0;
    char smallest_char, largest_char;

    std::unordered_map<char, unsigned long> character_counts;
    std::for_each(pair_collection.begin(), pair_collection.end(),
                  [&character_counts](const std::unordered_map<std::string, unsigned long>::value_type &element) {
                      char c = element.first[0];
                      unsigned long count = element.second;
                      character_counts[c] += count;
                  });

    for (auto &[c, count]: character_counts) {
        if (count < smallest_character_count) {
            smallest_char = c;
            smallest_character_count = count;
        }
        if (count > largest_character_count) {
            largest_char = c;
            largest_character_count = count;
        }
    }

    /* Here we add that last char back into the count to make the n - 1 occurrence of the last character an n*/
    if (smallest_char == last_char) smallest_character_count++;
    if (largest_char == last_char) largest_character_count++;

    std::cout << "Largest character with count is " << largest_char << ": " << largest_character_count << std::endl;
    std::cout << "Smallest character with count is " << smallest_char << ": " << smallest_character_count << std::endl;
    std::cout << "Difference: " << largest_character_count - smallest_character_count;

}

