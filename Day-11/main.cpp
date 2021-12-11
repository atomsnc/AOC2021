#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <chrono>

constexpr int GRID_SIZE = 10;
constexpr int STEPS = 100;

class Octopus {
public:

    int x, y, energy_level;
    /* one copy shared across all points */
    static std::stringstream stringstream;

    Octopus() = default;

    Octopus(int x, int y, int energy_level) : x(x), y(y), energy_level(energy_level) {
        if (energy_level < 0 || energy_level > 9) {
            throw std::runtime_error("Octopus energy level cannot be smaller than 0 or greater than 9.");
        }
    }

    [[nodiscard]] std::vector<std::string> getNeighborsAsCoordinates() const {
        std::vector<std::string> coordinates;
        coordinates.reserve(6);
        std::stringstream &stream = Octopus::stringstream;
        /* Get left */
        if (x > 0) {
            stream.str(std::string());
            stream << x - 1 << ',' << y;
            coordinates.emplace_back(stream.str());
        }
        /* Get up-left */
        if (y > 0 && x > 0) {
            stream.str(std::string());
            stream << x - 1 << ',' << y - 1;
            coordinates.emplace_back(stream.str());
        }
        /* Get up */
        if (y > 0) {
            stream.str(std::string());
            stream << x << ',' << y - 1;
            coordinates.emplace_back(stream.str());
        }
        /* Get up-right */
        if (y > 0 && x < GRID_SIZE - 1) {
            stream.str(std::string());
            stream << x + 1 << ',' << y - 1;
            coordinates.emplace_back(stream.str());
        }
        /* Get right */
        if (x < GRID_SIZE - 1) {
            stream.str(std::string());
            stream << x + 1 << ',' << y;
            coordinates.emplace_back(stream.str());
        }
        /* Get down-right */
        if (y < GRID_SIZE - 1 && x < GRID_SIZE - 1) {
            stream.str(std::string());
            stream << x + 1 << ',' << y + 1;
            coordinates.emplace_back(stream.str());
        }
        /* Get down */
        if (y < GRID_SIZE - 1) {
            stream.str(std::string());
            stream << x << ',' << y + 1;
            coordinates.emplace_back(stream.str());
        }
        /* Get down-left */
        if (y < GRID_SIZE - 1 && x > 0) {
            stream.str(std::string());
            stream << x - 1 << ',' << y + 1;
            coordinates.emplace_back(stream.str());
        }
        if (coordinates.size() < 3) {
            throw std::runtime_error("There should be at least 2 neighbors for a point");
        }
        return coordinates;
    }
};

void printOctopusGrid(std::vector<Octopus> &octopuses) {
    for (int index = 0; auto &octopus: octopuses) {
        if (index % GRID_SIZE == 0) {
            std::cout << std::endl;
        }
        std::cout << octopus.energy_level;
        index++;
    }
    std::cout << std::endl;
}

std::stringstream Octopus::stringstream = std::stringstream();

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

void simulateOctopusField(std::vector<Octopus> &octopuses, std::unordered_map<std::string, Octopus*> &octopuses_map,
                          std::unordered_set<Octopus *> &used_up_octopuses) {
    for (auto &octopus: octopuses) {
        std::deque<Octopus *> powered_up_octopuses;

        ++octopus.energy_level;
        if (octopus.energy_level == 10) {
            used_up_octopuses.insert(&octopus);

            auto neighbors = octopus.getNeighborsAsCoordinates();
            for (auto &neighbor: neighbors) {
                auto neighbor_octopus = octopuses_map[neighbor];
                powered_up_octopuses.push_back(neighbor_octopus);
            }
            while (!powered_up_octopuses.empty()) {
                auto neighbor_octopus = powered_up_octopuses.front();
                powered_up_octopuses.pop_front();
                ++(neighbor_octopus->energy_level);
                if (neighbor_octopus->energy_level == 10) {
                    used_up_octopuses.insert(neighbor_octopus);
                    auto neighbor_neighbors = neighbor_octopus->getNeighborsAsCoordinates();
                    for (auto &nn: neighbor_neighbors) {
                        auto nn_octopus = octopuses_map[nn];
                        if (nn_octopus->energy_level < 10) {
                            powered_up_octopuses.push_back(nn_octopus);
                        }
                    }
                }
            }
        }
    }
}

int main() {
    std::string filepath = "input.txt";
    std::vector<std::vector<int>> map;
    readEntries(filepath, map);

    std::vector<Octopus> octopuses;
    /* We need to reserve this space so the pointers don't get invalidated. */
    octopuses.reserve(GRID_SIZE * GRID_SIZE);
    std::unordered_map<std::string, Octopus *> octopuses_map;
    std::stringstream stream;

    for (int y = 0; auto &row: map) {
        for (int x = 0; auto &point: row) {
            stream.str(std::string());
            stream << x << ',' << y;

            octopuses.emplace_back(Octopus(x, y, point));
            octopuses_map.insert({stream.str(), &octopuses.back()});

            x++;
        }
        y++;
    }

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto start = high_resolution_clock::now();

    // Part 1
    int total_flashes = 0;
    for (int i = 0; i < STEPS; i++) {
        std::unordered_set<Octopus *> used_up_octopuses;

        simulateOctopusField(octopuses, octopuses_map, used_up_octopuses);

        for (auto used_octopus: used_up_octopuses) {
            used_octopus->energy_level = 0;
            total_flashes++;
        }
    }
    std::cout << "Total flashes: " << total_flashes << std::endl;


    // Part 2
    bool full_flash = false;
    int total_steps = 0;
    while (!full_flash) {
        full_flash = true;
        std::unordered_set<Octopus *> used_up_octopuses;

        simulateOctopusField(octopuses, octopuses_map, used_up_octopuses);

        for (auto used_octopus: used_up_octopuses) {
            used_octopus->energy_level = 0;
        }

        for (auto &octopus: octopuses) {
            if (octopus.energy_level) {
                full_flash = false;
                break;
            }
        }
        total_steps++;
    }
    std::cout << "Full flash at step " << total_steps + STEPS << std::endl;

    auto end = high_resolution_clock::now();
    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = end - start;
    std::cout << "Time taken: " << ms_double.count() << "ms";

}

