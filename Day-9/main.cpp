#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <stack>
#include <unordered_set>
#include <chrono>

using Map = std::vector<std::vector<int>>;

void readEntries(std::string_view filepath, Map &heightmap) {
    std::ifstream file(filepath.data());
    std::string line;

    while (std::getline(file, line)) {
        std::vector<int> entries;
        for (auto &c : line) {
            entries.emplace_back(c - '0');
        }
        heightmap.emplace_back(std::move(entries));
    }
}

class Point {
public:
    int x, y, value;
    Map *heightmap;

    Point() = default;

    Point(Map *heightmap, int x, int y, int value) : x(x), y(y), value(value), heightmap(nullptr) {
        if (value < 0 || value > 9) {
            std::cout << value << std::endl;
            throw std::runtime_error("Point height cannot be smaller than 0 or greater than 9");
        }
        this->heightmap = heightmap;
    };

    std::vector<int> getNeightbors() {
        std::vector<int> neighbors;
        /* Get left */
        if (x > 0) {
            neighbors.emplace_back((*heightmap)[y][x - 1]);
        }
        /* Get right */
        if (x < (*heightmap)[y].size() - 1) {
            neighbors.emplace_back((*heightmap)[y][x + 1]);
        }
        /* Get up */
        if (y > 0) {
            neighbors.emplace_back((*heightmap)[y - 1][x]);
        }
        /* Get down */
        if (y < heightmap->size() - 1) {
            neighbors.emplace_back((*heightmap)[y + 1][x]);
        }
        if (neighbors.size() < 2) {
            throw std::runtime_error("There should be at least 2 neighbors for a point");
        }
        return neighbors;
    }

    std::vector<std::string> getBasinNeighborsAsCoordinates() {
        std::vector<std::string> coordinates;
        coordinates.reserve(4);
        std::stringstream coordinate;
        /* Get left */
        if (x > 0) {
            coordinate << x - 1 << ',' << y;
            coordinates.emplace_back(coordinate.str());
            coordinate.str(std::string());
        }
        /* Get right */
        if (x < (*heightmap)[y].size() - 1) {
            coordinate << x + 1 << ',' << y;
            coordinates.emplace_back(coordinate.str());
            coordinate.str(std::string());
        }
        /* Get up */
        if (y > 0) {
            coordinate << x << ',' << y - 1;
            coordinates.emplace_back(coordinate.str());
            coordinate.str(std::string());
        }
        /* Get down */
        if (y < heightmap->size() - 1) {
            coordinate << x << ',' << y + 1;
            coordinates.emplace_back(coordinate.str());
            coordinate.str(std::string());
        }
        if (coordinates.size() < 2) {
            coordinate << x << ',' << y;
            throw std::runtime_error("There should be at least 2 neighbors for a point");
        }
        return coordinates;
    }
};

int calculateFloodFillArea(Point &lowest_point, std::unordered_map<std::string, Point> &points) {
    std::unordered_set<std::string> visited_points;
    std::stack<Point> visiting_points;

    visiting_points.push(lowest_point);

    int area = 0;
    while (!visiting_points.empty()) {
        Point point = visiting_points.top();
        visiting_points.pop();
        area++;
        auto neighbor_coordinates = point.getBasinNeighborsAsCoordinates();
        for (auto &neighbor_coordinate : neighbor_coordinates) {
            Point neighbor = points[neighbor_coordinate];
            if (neighbor.value == 9) {
                continue;
            }
            if (neighbor.value >= point.value && visited_points.find(neighbor_coordinate) == visited_points.end())
            {
                visited_points.insert(neighbor_coordinate);
                visiting_points.push(neighbor);
            }
        }
    }
    return area;

}

int main() {
    std::string filepath = "input.txt";
    Map heightmap;

    readEntries(filepath, heightmap);

    // Part 1

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto start = high_resolution_clock::now();

    std::unordered_map<std::string, Point> points;

    for (int y = 0; y < heightmap.size(); y++) {
        for (int x = 0; x < heightmap[y].size(); x++) {
            std::stringstream coordinate;
            coordinate << x << ',' << y;
            points.insert({coordinate.str(), Point(&heightmap, x, y, heightmap[y][x])});
        }
    }

    int risk_level = 0;
    std::vector<Point> lowest_points;
    for (auto &point : points) {
        bool lowest = true;
        for (auto &neighbor : point.second.getNeightbors()) {
            if (neighbor <= point.second.value) {
                lowest = false;
                break;
            }
        }
        if (lowest) {
            lowest_points.emplace_back(point.second);
            risk_level += point.second.value + 1;
        }
    }

    std::cout << "Total risk level: " << risk_level << std::endl;

    // Part 2

    std::vector<int> areas;
    for (auto &lowest_point : lowest_points) {
        int area = calculateFloodFillArea(lowest_point, points);
        areas.emplace_back(area);
    }

    std::nth_element(areas.begin(), areas.begin() + 3, areas.end(), std::greater());
    std::cout << "Final Area: " << areas[0] * areas[1] * areas[2] << std::endl;

    auto end = high_resolution_clock::now();
    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = end - start;
    std::cout << "Time taken: " << ms_double.count() << "ms";

}

