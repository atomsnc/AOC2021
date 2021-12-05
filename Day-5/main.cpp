#include <iostream>
#include <utility>
#include <vector>
#include <fstream>
#include <chrono>
#include <unordered_map>
#include <sstream>

class Cell {
public:
    int overlap;
    Cell(): overlap(1) {}

};

class Entry {
public:
    int x1;
    int x2;
    int y1;
    int y2;

    Entry(int x1, int y1, int x2, int y2)
    : x1(x1)
    , y1(y1)
    , x2(x2)
    , y2(y2) {}

    inline std::tuple<int, int, int , int> get() {
        return std::make_tuple(x1, y1, x2, y2);
    }
};

using Map = std::unordered_map<std::string, Cell>;

inline std::vector<std::string> generateCells(int x1, int y1, int x2, int y2, bool diagonal = false) {
    int x = x1, y = y1;
    std::stringstream ss;
    std::vector<std::string> cells;

    if (x1 == x2 || y1 == y2 || diagonal) {
        for (;;) {
//        std::cout << "Creating: " << x << "," << y << std::endl;
            ss << x << ',' << y;
            cells.emplace_back(ss.str());
            ss.str(std::string());

            if (x == x2 && y == y2) break;

            /* Move x and y towards x2 and y2 */
            if (x < x2) {
                x++;
            } else if (x > x2) {
                x--;
            }
            if (y < y2) {
                y++;
            } else if (y > y2) {
                y--;
            }
        }
    }
    return std::move(cells);
}


void readEntries(std::string_view filepath, std::vector<Entry> &entries) {
    std::ifstream file(filepath.data());
    std::string string;
    int x1, y1, x2, y2;
    std::stringstream ss;

    while (std::getline(file, string)) {
        std::sscanf(string.c_str(), "%d,%d -> %d,%d", &x1, &y1, &x2, &y2);
        Entry entry(x1, y1, x2, y2);
        entries.emplace_back(entry);
    }
}

int main() {
    std::string filepath = "input.txt";
    Map map;

    std::vector<Entry> entries;
    readEntries(filepath, entries);

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto start = high_resolution_clock::now();

    int x1, y1, x2, y2;
    for (auto &entry : entries) {
        std::tie(x1, y1, x2, y2) = entry.get();
//        std::cout << x1 << "," << y1 << " : " << x2 << "," << y2 << std::endl;
        auto cells = generateCells(x1, y1, x2, y2);
        for (auto &cell : cells) {
//            std::cout << cell << std::endl;
            auto location = map.find(cell);
            if (location != map.end()) {
                location->second.overlap++;
            } else {
                map.insert({cell, Cell()});
            }
        }
    }
    int total_overlap_locations = 0;

    for (auto &location : map) {
//        std::cout << location.first << "\t" << location.second.overlap << std::endl;
        if (location.second.overlap >= 2) {
            total_overlap_locations++;
        }
    }

    std::cout << "Total overlap locations: " << total_overlap_locations << std::endl;

    map.clear();

    for (auto &entry : entries) {
        std::tie(x1, y1, x2, y2) = entry.get();
//        std::cout << x1 << "," << y1 << " : " << x2 << "," << y2 << std::endl;
        auto cells = generateCells(x1, y1, x2, y2, true);
        for (auto &cell : cells) {
//            std::cout << cell << std::endl;
            auto location = map.find(cell);
            if (location != map.end()) {
                location->second.overlap++;
            } else {
                map.insert({cell, Cell()});
            }
        }
    }
    total_overlap_locations = 0;

    for (auto &location : map) {
//        std::cout << location.first << "\t" << location.second.overlap << std::endl;
        if (location.second.overlap >= 2) {
            total_overlap_locations++;
        }
    }

    std::cout << "Total overlap locations with diagonal: " << total_overlap_locations << std::endl;

    auto end = high_resolution_clock::now();
    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = end - start;
    std::cout << "Time taken: " << ms_double.count() << "ms";

    return 0;
}

