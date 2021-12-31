#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <deque>

using Rotation = std::array<std::pair<int64_t, int64_t>, 3>;
const constexpr int64_t X = 0;
const constexpr int64_t Y = 1;
const constexpr int64_t Z = 2;
const constexpr int64_t POS = 1;
const constexpr int64_t NEG = -1;

const constexpr std::array<Rotation, 24> ROTATIONS = {{
                                                              {{{X, POS}, {Y, POS}, {Z, POS}}},
                                                              {{{X, POS}, {Z, POS}, {Y, NEG}}},
                                                              {{{X, POS}, {Y, NEG}, {Z, NEG}}},
                                                              {{{X, POS}, {Z, NEG}, {Y, POS}}},
                                                              {{{X, NEG}, {Y, POS}, {Z, NEG}}},
                                                              {{{X, NEG}, {Z, NEG}, {Y, NEG}}},
                                                              {{{X, NEG}, {Y, NEG}, {Z, POS}}},
                                                              {{{X, NEG}, {Z, POS}, {Y, POS}}},
                                                              {{{Y, POS}, {X, POS}, {Z, NEG}}},
                                                              {{{Y, POS}, {Z, NEG}, {X, NEG}}},
                                                              {{{Y, POS}, {X, NEG}, {Z, POS}}},
                                                              {{{Y, POS}, {Z, POS}, {X, POS}}},
                                                              {{{Y, NEG}, {X, POS}, {Z, POS}}},
                                                              {{{Y, NEG}, {Z, POS}, {X, NEG}}},
                                                              {{{Y, NEG}, {X, NEG}, {Z, NEG}}},
                                                              {{{Y, NEG}, {Z, NEG}, {X, POS}}},
                                                              {{{Z, POS}, {X, POS}, {Y, POS}}},
                                                              {{{Z, POS}, {Y, POS}, {X, NEG}}},
                                                              {{{Z, POS}, {X, NEG}, {Y, NEG}}},
                                                              {{{Z, POS}, {Y, NEG}, {X, POS}}},
                                                              {{{Z, NEG}, {X, POS}, {Y, NEG}}},
                                                              {{{Z, NEG}, {Y, NEG}, {X, NEG}}},
                                                              {{{Z, NEG}, {X, NEG}, {Y, POS}}},
                                                              {{{Z, NEG}, {Y, POS}, {X, POS}}},
                                                      }};

class Point {
public:
    int64_t x, y, z;

    Point() : x(0), y(0), z(0) {}

    Point(int64_t x, int64_t y, int64_t z) : x(x), y(y), z(z) {}

    int64_t manhattan() { return std::abs(x) + std::abs(y) + std::abs(z); }

    [[nodiscard]] Point rotate(const Rotation &rotation) const {
        const std::array<int64_t, 3> values{x, y, z};
        return {
                (values[rotation[0].first]) * rotation[0].second,
                (values[rotation[1].first]) * rotation[1].second,
                (values[rotation[2].first]) * rotation[2].second
        };
    }

    bool operator==(const Point &other) const {
        return (x == other.x && y == other.y && z == other.z);
    }

    friend Point operator-(const Point &lhs, const Point &rhs) {
        return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
    }

    friend Point operator+(const Point &lhs, const Point &rhs) {
        return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
    }

    friend std::ostream &operator<<(std::ostream &stream, const Point &point) {
        stream << point.x << "," << point.y << "," << point.z;
        return stream;
    };
};

template<>
struct std::hash<Point> {
    std::size_t operator()(Point const &element) const {
        std::size_t h1 = std::hash<int64_t>{}(element.x);
        std::size_t h2 = std::hash<int64_t>{}(element.y);
        std::size_t h3 = std::hash<int64_t>{}(element.z);
        /* Simple one bit shift hash */
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

class ScannerOffset {
public:
    Rotation rotation;
    Point position;
};

class Scanner {
public:
    uint64_t id;
    Point position;
    std::vector<Point> beacons;

    Scanner(uint64_t id, std::vector<Point> beacons) : id(id), beacons(std::move(beacons)) {}

    std::optional<ScannerOffset> overlaps(const Scanner &other) {
        for (auto &rotation: ROTATIONS) {
            std::unordered_map<Point, uint32_t> matches;
            for (auto &absolute_beacon: beacons) {
                for (auto &relative_beacon: other.beacons) {
                    matches[absolute_beacon - relative_beacon.rotate(rotation)]++;
                }
            }
            for (auto &[point, count]: matches) {
                if (count >= 12) {
                    return ScannerOffset{rotation, point};
                }
            }
        }
        return {};
    }

    void correctOrientation(const ScannerOffset &offset) {
        for (auto &beacon: beacons) {
            beacon = beacon.rotate(offset.rotation) + offset.position;
        }
        position = offset.position;
    }
};


void readEntries(std::string_view filepath, std::vector<Scanner> &scanners) {
    std::ifstream file(filepath.data());
    std::string line;

    std::vector<Point> scanned_points;
    uint64_t id = 0;

    while (std::getline(file, line)) {
        if (line.find("scanner") != std::string::npos) {
            scanned_points.clear();
        } else if (line.empty()) {
            scanners.emplace_back(id++, scanned_points);
        } else [[likely]] {
            int64_t x, y, z;
            std::sscanf(line.c_str(), "%ld,%ld,%ld", &x, &y, &z);
            scanned_points.emplace_back(x, y, z);
        }
    }
    scanners.emplace_back(id++, scanned_points);
}

void find_pairs_and_fix(std::vector<Scanner> &scanners) {
    std::unordered_set<uint64_t> fixed_ids;
    std::deque<uint64_t> ids_queue;

    fixed_ids.insert(0);
    ids_queue.emplace_back(0);
    scanners[0].position = Point(0, 0, 0);

    while (!ids_queue.empty()) {
        uint64_t done = ids_queue.front();
        ids_queue.pop_front();

        for (size_t not_done = 0; not_done < scanners.size(); not_done++) {
            if (fixed_ids.contains(not_done)) continue;

            auto rotation = scanners[done].overlaps(scanners[not_done]);
            if (!rotation.has_value()) continue;

            scanners[not_done].correctOrientation(rotation.value());

            ids_queue.emplace_back(not_done);
            fixed_ids.insert(not_done);
        }
    }
}

int main() {
    std::string filepath = "input.txt";

    std::vector<Scanner> scanners;
    readEntries(filepath, scanners);

    find_pairs_and_fix(scanners);

    std::unordered_set<Point> unique_beacons;
    for (auto &scanner: scanners) {
        for (auto &beacon: scanner.beacons) {
            unique_beacons.insert(beacon);
        }
    }

    std::cout << "Actual beacons: " << unique_beacons.size() << std::endl;

    int64_t largest_manhattan = 0;
    for (auto &lhs : scanners)
        for (auto &rhs : scanners)
            largest_manhattan = std::max(largest_manhattan, (lhs.position - rhs.position).manhattan());
    std::cout << "Largest Manhattan distance: " << largest_manhattan << std::endl;
}