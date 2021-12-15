#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <optional>
#include <chrono>

static int GRID_SIZE_X = 0;
static int GRID_SIZE_Y = 0;

class Chitcon {
public:

    int x, y, risk;
    /* one copy shared across all points */
    static std::stringstream stringstream;

    Chitcon() = default;

    Chitcon(int x, int y, int risk) : x(x), y(y), risk(risk) {
        if (risk < 0 || risk > 9) {
            throw std::runtime_error("Octopus energy level cannot be smaller than 0 or greater than 9.");
        }
    }

    [[nodiscard]] std::vector<std::string> getNeighborsAsCoordinates(int max_x, int max_y) const {
        std::vector<std::string> coordinates;
        coordinates.reserve(6);
        std::stringstream &stream = Chitcon::stringstream;
        /* Get left */
        if (x > 0) {
            stream.str(std::string());
            stream << x - 1 << ',' << y;
            coordinates.emplace_back(stream.str());
        }
        /* Get up */
        if (y > 0) {
            stream.str(std::string());
            stream << x << ',' << y - 1;
            coordinates.emplace_back(stream.str());
        }
        /* Get right */
        if (x < max_x - 1) {
            stream.str(std::string());
            stream << x + 1 << ',' << y;
            coordinates.emplace_back(stream.str());
        }
        /* Get down */
        if (y < max_y - 1) {
            stream.str(std::string());
            stream << x << ',' << y + 1;
            coordinates.emplace_back(stream.str());
        }
        if (coordinates.size() < 2) {
            throw std::runtime_error("There should be at least 2 neighbors for a point");
        }
        return coordinates;
    }
};

void printChitconGrid(std::unordered_map<std::string, Chitcon> &chitcon_map, int max_x, int max_y) {
    std::stringstream stream;
    for (int y = 0; y < max_y; y++) {
        for (int x = 0; x < max_x; x++) {
            stream.str(std::string());
            stream << x << ',' << y;

            std::cout << chitcon_map[stream.str()].risk;
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void readEntries(std::string_view filepath, std::vector<std::vector<int>> &map) {
    std::ifstream file(filepath.data());
    std::string line;

    while (std::getline(file, line)) {
        std::vector<int> entries;
        for (auto &c: line) {
            entries.emplace_back(c - '0');
        }
        map.emplace_back(std::move(entries));
    }
}

std::optional<int>
getShortestPathCost(std::unordered_map<std::string, Chitcon> &chitcon_map, std::string &start, std::string &end, int max_x, int max_y) {
    auto compare = [](const std::pair<std::string, int> &lhs, const std::pair<std::string, int> &rhs) {
        return lhs.second > rhs.second;
    };

    std::priority_queue<std::pair<std::string, int>, std::vector<std::pair<std::string, int>>, decltype(compare)> frontier_list;
    std::unordered_set<std::string> visited;

    frontier_list.emplace(start, 0);

    while (!frontier_list.empty()) {

        auto[node, risk] = frontier_list.top();
        frontier_list.pop();

        if (node == end) {
            return risk;
        }

        if (visited.contains(node)) {
            continue;
        }

        visited.emplace(node);

        for (auto &neighbor: chitcon_map[node].getNeighborsAsCoordinates(max_x, max_y)) {
            frontier_list.emplace(neighbor, risk + chitcon_map[neighbor].risk);
        }

    }
    /* No path found */
    return {};
}

std::stringstream Chitcon::stringstream = std::stringstream();

int main() {
    std::string filepath = "input.txt";
    std::vector<std::vector<int>> map;
    readEntries(filepath, map);

    GRID_SIZE_Y = map.size();
    GRID_SIZE_X = map[0].size();

    std::unordered_map<std::string, Chitcon> chitcon_map;
    std::unordered_map<std::string, Chitcon> chitcon_map_tiled;

    std::stringstream stream;

    for (int y = 0; y < GRID_SIZE_Y; y++) {
        for (int x = 0; x < GRID_SIZE_X; x++) {
            stream.str(std::string());
            stream << x << ',' << y;

            chitcon_map.emplace(stream.str(), Chitcon(x, y, map[y][x]));
        }
    }

    // Repeat 5 times
    for (int y = 0; y < GRID_SIZE_Y * 5; y++) {
        for (int x = 0; x < GRID_SIZE_X * 5; x++) {
            stream.str(std::string());
            stream << x << ',' << y;

            int risk = map[y % GRID_SIZE_Y][x % GRID_SIZE_X] + (x / GRID_SIZE_X) + (y / GRID_SIZE_Y);
            if (risk >= 10) {
                risk = risk - (9 * (risk / 10));
            }

            chitcon_map_tiled.emplace(stream.str(), Chitcon(x, y, risk));
        }
    }

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto start = high_resolution_clock::now();

    std::string start_point = "0,0";
    stream.str(std::string());
    stream << GRID_SIZE_X - 1 << ',' << GRID_SIZE_Y - 1;
    std::string end_point = stream.str();

    // Part 1
    auto risk = getShortestPathCost(chitcon_map, start_point, end_point, GRID_SIZE_Y, GRID_SIZE_Y);
    if (!risk.has_value()) throw std::runtime_error("No path found");

    std::cout << "Lowest total risk: " << *risk << std::endl;

    // Part 2
    stream.str(std::string());
    stream << (GRID_SIZE_X * 5) - 1 << ',' << (GRID_SIZE_Y * 5) - 1;
    end_point = stream.str();

    risk = getShortestPathCost(chitcon_map_tiled, start_point, end_point, GRID_SIZE_Y * 5, GRID_SIZE_Y * 5);
    if (!risk.has_value()) throw std::runtime_error("No path found");

    std::cout << "Lowest total risk: " << *risk << std::endl;

    auto end = high_resolution_clock::now();
    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = end - start;
    std::cout << "Time taken: " << ms_double.count() << "ms";

}