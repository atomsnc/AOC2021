#include <iostream>
#include <utility>
#include <vector>
#include <fstream>
#include <chrono>
#include <unordered_set>

static uint32_t MAX_X = 0;
static uint32_t MAX_Y = 0;

class Point {
public:
    uint32_t x, y;

    Point(uint32_t x, uint32_t y): x(x), y(y) {}

    bool operator==(const Point& point) const
    {
        return (this->x == point.x && this->y == point.y);
    }
};

class Fold {
public:
    char type;
    uint32_t line;

    Fold(char type, uint32_t line): type(type), line(line) {}
};

namespace std {
    template <>
    struct hash<Point> {
        std::size_t operator()(const Point &point) const {
            uint64_t hash = 0;
            hash = static_cast<uint64_t>(point.x) << 32 | point.y;
            return hash;
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

void readEntries(std::string_view filepath, std::unordered_set<Point> &points, std::vector<Fold> &folds) {
    std::ifstream file(filepath.data());
    std::string line;

    while(std::getline(file, line)) {
        if (line.empty()) break;
        std::vector<std::string> point_literals;
        split(line, ',', point_literals);
        const Point point(std::stoul(point_literals[0]), std::stoul(point_literals[1]));
        if (MAX_X < point.x) MAX_X = point.x;
        if (MAX_Y < point.y) MAX_Y = point.y;
        points.emplace(point);
    }

    while (std::getline(file, line)) {
        char type;
        uint32_t value;
        std::sscanf(line.c_str(), "fold along %c=%d", &type, &value);
        folds.emplace_back(type, value);
    }
}

void printGrid(std::unordered_set<Point> &points) {
    for (uint32_t y = 0; y <= MAX_Y; y++) {
        for (uint32_t x = 0; x <= MAX_X; x++) {
            const Point point(x, y);
            if (points.contains(point)) {
                std::cout << '#';
            } else std::cout << '.';
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    std::string filepath = "input.txt";
    std::unordered_set<Point> points;
    std::vector<Fold> folds;
    readEntries(filepath, points, folds);

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto start = high_resolution_clock::now();

//    printGrid(points);

    for (int i = 0; auto &fold : folds) {
        /* Here we perform fold */
        switch (fold.type) {
            case 'y' : {
                /* Find points to change */
                std::vector<Point> points_to_change;
                for (auto &point : points) {
                    if (point.y > fold.line) {
                        points_to_change.emplace_back(point);
                    }
                }
                /* Remove points from points set */
                for (auto &point : points_to_change) {
                    points.erase(point);
                }
                /* Re-add modified points to points set */
                for (auto &point : points_to_change) {
                    point.y = (2 * fold.line) - point.y;
                    points.emplace(point);
                }
                /* Set MAX_Y to ``line - 1`` */
                MAX_Y = fold.line - 1;
                break;
            }
            case 'x' : {
                /* Find points to change */
                std::vector<Point> points_to_change;
                for (auto &point : points) {
                    if (point.x > fold.line) {
                        points_to_change.emplace_back(point);
                    }
                }
                /* Remove points from points set */
                for (auto &point : points_to_change) {
                    points.erase(point);
                }
                /* Re-add modified points to points set */
                for (auto &point : points_to_change) {
                    point.x = (2 * fold.line) - point.x;
                    points.emplace(point);
                }
                /* Set MAX_X to ``line - 1`` */
                MAX_X = fold.line - 1;
                break;
            }
            default : throw std::runtime_error("Unknown fold type found.");
        }

        i++;
        int total_dots = 0;
        for (uint32_t y = 0; y <= MAX_Y; y++) {
            for (uint32_t x = 0; x <= MAX_X; x++) {
                const Point point(x, y);
                if (points.contains(point)) {
                    total_dots++;
                }
            }
        }
//        printGrid(points);
        std::cout << "Total dots after fold " << i << ": " << total_dots << std::endl;
//        std::cout << std::endl;
    }

    auto end = high_resolution_clock::now();
    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = end - start;
    std::cout << "Time taken: " << ms_double.count() << "ms";
}