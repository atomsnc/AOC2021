#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <sstream>

using Path = std::vector<std::string>;

namespace std {
    template<>
    struct hash<Path> {
        std::size_t operator()(const Path &path) const {
            std::stringstream ss;
            for (auto &element: path) {
                ss << element;
            }
            return std::hash<std::string>{}(ss.str());
        }
    };
}

inline void split(const std::string &string, char delim, std::vector<std::string> &entries) {
    size_t start;
    size_t end = 0;
    while ((start = string.find_first_not_of(delim, end)) != std::string::npos) {
        end = string.find(delim, start);
        std::string entry = string.substr(start, end - start);
        entries.emplace_back(entry);
    }
}

void readEntries(std::string_view filepath, std::unordered_map<std::string, std::unordered_set<std::string>> &links) {
    std::ifstream file(filepath.data());
    std::string line;

    while (std::getline(file, line)) {
        std::vector<std::string> link;
        split(line, '-', link);
        {
            std::unordered_set<std::string> &link_neighbors = links[link[0]];
            link_neighbors.emplace(link[1]);
        }
        {
            std::unordered_set<std::string> &link_neighbors = links[link[1]];
            link_neighbors.emplace(link[0]);
        }
    }
}

bool inline allUpper(const std::string &string) {
    return std::all_of(string.begin(), string.end(), [](unsigned char c) { return std::isupper(c); });
}

bool inline allLower(const std::string &string) {
    return std::all_of(string.begin(), string.end(), [](unsigned char c) { return std::islower(c); });
}

bool inline elementNOTExistsInVector(std::vector<std::string> &vector, const std::string &string) {
    ;
    return std::find(vector.begin(), vector.end(), string) == vector.end();
}

int inline countElementInVector(std::vector<std::string> &vector, const std::string &string) {
    return std::count_if(vector.begin(), vector.end(), [string](std::string &element) {
        return element == string;
    });
}

void explore(const std::string &token,
             std::unordered_map<std::string, std::unordered_set<std::string>> &links,
             Path &current_path, std::vector<Path> &total_paths, const std::string &single_double) {

    /* We emulate a path stack using a vector */
    /* Push at start */
    current_path.emplace_back(token);

    if (token == "end") {
        total_paths.emplace_back(current_path);
    } else {
        for (auto &link: links[token]) {
            if (
                    elementNOTExistsInVector(current_path, link)
                    || allUpper(link)
                    || (link == single_double && countElementInVector(current_path, link) < 2)
                    ) {
                explore(link, links, current_path, total_paths, single_double);
            }
        }
    }
    /* Pop at end */
    current_path.pop_back();
}

void explore(const std::string &token,
             std::unordered_map<std::string, std::unordered_set<std::string>> &links,
             Path &current_path, std::unordered_set<Path> &total_paths, const std::string &single_double) {

    /* We emulate a path stack using a vector */
    /* Push at start */
    current_path.emplace_back(token);

    if (token == "end") {
        total_paths.insert(current_path);
    } else {
        for (auto &link: links[token]) {
            if (
                    elementNOTExistsInVector(current_path, link)
                    || allUpper(link)
                    || (link == single_double && countElementInVector(current_path, link) < 2)
                    ) {
                explore(link, links, current_path, total_paths, single_double);
            }
        }
    }
    /* Pop at end */
    current_path.pop_back();
}

int main() {
    std::string filepath = "input.txt";

    std::unordered_map<std::string, std::unordered_set<std::string>> links;
    readEntries(filepath, links);

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto start = high_resolution_clock::now();

    Path current_path;

    // Part 1
    std::vector<Path> total_paths;

    explore("start", links, current_path, total_paths, std::string());

    std::cout << "Total paths: " << total_paths.size() << std::endl;


    // Part 2
    std::unordered_set<Path> unique_paths;

    for (auto&[link, neighbors]: links) {
        if (link != "start" && link != "end" && allLower(link)) {
            explore("start", links, current_path, unique_paths, link);
        }
    }

    std::cout << "Total paths: " << unique_paths.size() << std::endl;

    auto end = high_resolution_clock::now();
    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = end - start;
    std::cout << "Time taken: " << ms_double.count() << "ms";
}