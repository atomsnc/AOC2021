#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <unordered_map>
#include <chrono>

void readEntries(std::string_view filepath, std::vector<std::string> &entries) {
    std::ifstream file(filepath.data());
    std::string line;

    while (std::getline(file, line)) {
        entries.emplace_back(line);
    }
}

int main() {
    std::string filepath = "input.txt";

    std::vector<std::string> entries;
    readEntries(filepath, entries);

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto start = high_resolution_clock::now();

    // Part 1
    std::vector<char> illegal_characters;
    std::unordered_map<char, char> mapping = {
            {')', '('},
            {']', '['},
            {'}', '{'},
            {'>', '<'}
    };

    entries.erase(
            std::remove_if(entries.begin(), entries.end(),
                           [&illegal_characters, &mapping](const std::string &entry) -> bool {
                               std::stack<char> chunks_open;
                               bool corrupted = false;
                               for (auto &c: entry) {
                                   switch (c) {
                                       case '(':
                                       case '[':
                                       case '{':
                                       case '<': {
                                           chunks_open.push(c);
                                           break;
                                       }
                                       case ')':
                                       case ']':
                                       case '}':
                                       case '>': {
                                           if (chunks_open.top() != mapping[c]) {
                                               corrupted = true;

                                           } else {
                                               chunks_open.pop();
                                           }
                                           break;
                                       }
                                       default: {
                                           throw std::runtime_error("Undefined character found.");
                                       }
                                   }
                                   if (corrupted) {
                                       illegal_characters.emplace_back(c);
                                       return true;
                                   }
                               }
                               return false;
                           }),
            entries.end()
    );

    int syntax_error_score = 0;
    for (auto &illegal_character: illegal_characters) {
        switch (illegal_character) {
            case ')':
                syntax_error_score += 3;
                break;
            case ']':
                syntax_error_score += 57;
                break;
            case '}':
                syntax_error_score += 1197;
                break;
            case '>':
                syntax_error_score += 25137;
                break;
            default: {
                throw std::runtime_error("Undefined character found.");
            }
        }
    }

    std::cout << "Syntax error score: " << syntax_error_score << std::endl;

    // Part 2
    std::vector<unsigned long> scores;

    for (auto &entry : entries) {
        unsigned long score = 0;
        int corrupted = false;
        std::stack<char> chunks_open;
        for (auto &c : entry) {
            switch (c) {
                case '(':
                case '[':
                case '{':
                case '<': {
                    chunks_open.push(c);
                    break;
                }
                case ')':
                case ']':
                case '}':
                case '>': {
                    if (chunks_open.top() != mapping[c]) {
                        corrupted = true;

                    } else {
                        chunks_open.pop();
                    }
                    break;
                }
                default: {
                    throw std::runtime_error("Undefined character found.");
                }
            }
            if (corrupted) {
                throw std::runtime_error("Corrupted cannot be present in part 2.");
            }
        }
        while (!chunks_open.empty()) {
            score *= 5;
            switch (chunks_open.top()) {
                case '(': {
                    /* ) -> 1 point */
                    score += 1;
                    break;
                }
                case '[': {
                    /* ] -> 2 points */
                    score += 2;
                    break;
                }
                case '{': {
                    /* } -> 3 points */
                    score += 3;
                    break;
                }
                case '<': {
                    /* > -> 4 points */
                    score += 4;
                    break;
                }
                default: {
                    throw std::runtime_error("Unknown character found.");
                }
            }
            chunks_open.pop();
        }
        scores.emplace_back(score);
    }

    std::sort(scores.begin(), scores.end());
    std::cout << "Middle score: " << scores[scores.size() / 2] << std::endl;

    auto end = high_resolution_clock::now();
    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = end - start;
    std::cout << "Time taken: " << ms_double.count() << "ms";

}