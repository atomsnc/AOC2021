#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <optional>
#include <chrono>

static uint32_t GRID_SIZE_X = 0;
static uint32_t GRID_SIZE_Y = 0;

class Point {
public:
    uint32_t x, y;

    Point() = default;
    Point(uint32_t x, uint32_t y): x(x), y(y) {}

    bool operator==(const Point& point) const
    {
        return (this->x == point.x && this->y == point.y);
    }
};

namespace std {
    template <>
    struct hash<Point> {
        std::size_t operator()(const Point &point) const {
            return static_cast<uint64_t>(point.x) << 32 | point.y;
        }
    };
}

class Chitcon {
public:

    Point point;
    int risk;

    Chitcon() = default;
    Chitcon(uint32_t x, uint32_t y, int risk) : point(x, y), risk(risk) {
        if (risk < 0 || risk > 9) {
            throw std::runtime_error("Octopus energy level cannot be smaller than 0 or greater than 9.");
        }
    }

    [[nodiscard]] std::vector<Point> getNeighborsAsCoordinates(uint32_t max_x, uint32_t max_y) const {
        std::vector<Point> coordinates;
        coordinates.reserve(4);
        /* Get left */
        if (point.x > 0) {
            coordinates.emplace_back(Point(point.x - 1, point.y));
        }
        /* Get up */
        if (point.y > 0) {
            coordinates.emplace_back(Point(point.x, point.y - 1));
        }
        /* Get right */
        if (point.x < max_x - 1) {
            coordinates.emplace_back(Point(point.x + 1, point.y));
        }
        /* Get down */
        if (point.y < max_y - 1) {
            coordinates.emplace_back(Point(point.x, point.y + 1));
        }
        if (coordinates.size() < 2) {
            throw std::runtime_error("There should be at least 2 neighbors for a point");
        }
        return coordinates;
    }
};

void printChitconGrid(std::unordered_map<Point, Chitcon> &chitcon_map, uint32_t max_x, uint32_t max_y) {
    std::stringstream stream;
    for (int y = 0; y < max_y; y++) {
        for (int x = 0; x < max_x; x++) {
            std::cout << chitcon_map[Point(x, y)].risk;
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void readEntries(std::string_view filepath, std::vector<std::vector<uint32_t>> &map) {
    std::ifstream file(filepath.data());
    std::string line;

    while (std::getline(file, line)) {
        std::vector<uint32_t> entries;
        for (auto &c: line) {
            entries.emplace_back(c - '0');
        }
        map.emplace_back(std::move(entries));
    }
}

std::optional<int>
getShortestPathCost(std::unordered_map<Point, Chitcon> &chitcon_map, Point &start, Point &end, uint32_t max_x, uint32_t max_y) {
    auto compare = [](const std::pair<Point, int> &lhs, const std::pair<Point, int> &rhs) {
        return lhs.second > rhs.second;
    };

    std::priority_queue<std::pair<Point, int>, std::vector<std::pair<Point, int>>, decltype(compare)> frontier_list;
    std::unordered_set<Point> visited;

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

int main() {
    std::string filepath = "input.txt";
    std::vector<std::vector<uint32_t>> map;
    readEntries(filepath, map);

    GRID_SIZE_Y = map.size();
    GRID_SIZE_X = map[0].size();

    std::unordered_map<Point, Chitcon> chitcon_map;
    std::unordered_map<Point, Chitcon> chitcon_map_tiled;

    for (uint32_t y = 0; y < GRID_SIZE_Y; y++) {
        for (uint32_t x = 0; x < GRID_SIZE_X; x++) {
            chitcon_map.emplace(Point(x, y), Chitcon(x, y, map[y][x]));
        }
    }

    // Repeat 5 times
    for (uint32_t y = 0; y < GRID_SIZE_Y * 5; y++) {
        for (uint32_t x = 0; x < GRID_SIZE_X * 5; x++) {

            int risk = map[y % GRID_SIZE_Y][x % GRID_SIZE_X] + (x / GRID_SIZE_X) + (y / GRID_SIZE_Y);
            risk = risk - (9 * (risk / 10));

            chitcon_map_tiled.emplace(Point(x, y), Chitcon(x, y, risk));
        }
    }

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto start = high_resolution_clock::now();

    Point start_point(0, 0);
    Point end_point(GRID_SIZE_X - 1, GRID_SIZE_Y - 1);

    // Part 1
    auto risk = getShortestPathCost(chitcon_map, start_point, end_point, GRID_SIZE_Y, GRID_SIZE_Y);
    if (!risk.has_value()) throw std::runtime_error("No path found");

    std::cout << "Lowest total risk: " << *risk << std::endl;

    // Part 2
    end_point = Point((GRID_SIZE_X * 5) - 1, (GRID_SIZE_Y * 5) - 1);

    risk = getShortestPathCost(chitcon_map_tiled, start_point, end_point, GRID_SIZE_Y * 5, GRID_SIZE_Y * 5);
    if (!risk.has_value()) throw std::runtime_error("No path found");

    std::cout << "Lowest total risk: " << *risk << std::endl;

    auto end = high_resolution_clock::now();
    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = end - start;
    std::cout << "Time taken: " << ms_double.count() << "ms";

}