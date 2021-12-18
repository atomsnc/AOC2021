#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <bitset>
#include <optional>
#include <numeric>
#include <chrono>

class TargetArea {
public:
    int x1, x2, y1, y2;

    TargetArea(int x1, int x2, int y1, int y2) : x1(x1), x2(x2), y1(y1), y2(y2) {}
};

class Probe {
public:
    int x, y;
    Probe(): x(0), y(0) {}

    void move(int vel_x, int vel_y) {
        x += vel_x;
        y += vel_y;
    }

    void printCurrentPosition() {
        std::cout << "Probe position: " << x << '\t' << y << std::endl;
    }

    bool inline inTargetArea(TargetArea &target_area) {
        if (x >= target_area.x1 && x <= target_area.x2 && y >= target_area.y1 && y <= target_area.y2) {
            return true;
        }
        return false;
    }

    bool inline pastTargetArea(TargetArea &target_area, int vel_y) {
        if (x > target_area.x2 && y < target_area.y1) {
            return true;
        }
        if (vel_y < 0 && y < target_area.y1) {
            return true;
        }
        return false;
    }
};

TargetArea readTargetArea(std::string_view filepath) {
    std::ifstream file(filepath.data());
    std::string line;
    std::getline(file, line);

    int x1, x2, y1, y2;

    std::sscanf(line.c_str(), "target area: x=%d..%d, y=%d..%d", &x1, &x2, &y1, &y2);

    if (x1 > x2) std::swap(x1, x2);
    if (y1 > y2) std::swap(y1, y2);

    return {x1, x2, y1, y2};
}

int main() {
    std::string filepath = "input.txt";

    TargetArea target_area = readTargetArea(filepath);

    std::cout << "Target Area: " << target_area.x1 << ".." << target_area.x2 << " " << target_area.y1 << ".."
              << target_area.y2 << std::endl;

    // Part 1

    int final_max_height = std::numeric_limits<int>::min();
    int max_height_init_vel_x, max_height_init_vel_y;
    for (int init_vel_x = 1; init_vel_x <= 1000; init_vel_x++) {
        for (int init_vel_y = -2000; init_vel_y <= 2000; init_vel_y++) {
            int vel_x = init_vel_x, vel_y = init_vel_y, max_height = std::numeric_limits<int>::min();
            Probe probe = Probe();
            for (;!probe.pastTargetArea(target_area, vel_y) && !probe.inTargetArea(target_area);) {
                probe.move(vel_x, vel_y);
                if (max_height < probe.y) {
                    max_height = probe.y;
                }
                /* Step through velocity change */
                if (vel_x > 0) vel_x--;
                else if (vel_x < 0) vel_x++;
                vel_y--;
            }
            if (probe.inTargetArea(target_area)) {
                if (final_max_height < max_height) {
                    final_max_height = max_height;
                    max_height_init_vel_x = init_vel_x;
                    max_height_init_vel_y = init_vel_y;
                }
            }
        }
    }
    std::cout << "Max height of " << final_max_height << " with initial velocity " << max_height_init_vel_x << ' ' << max_height_init_vel_y << std::endl;

    // Part 2
    int distinct_init_vels_hitting_target = 0;
    for (int init_vel_x = 1; init_vel_x <= 1000; init_vel_x++) {
        for (int init_vel_y = -2000; init_vel_y <= 2000; init_vel_y++) {
            int vel_x = init_vel_x, vel_y = init_vel_y;
            Probe probe = Probe();
            for (;!probe.pastTargetArea(target_area, vel_y) && !probe.inTargetArea(target_area);) {
                probe.move(vel_x, vel_y);
                /* Step through velocity change */
                if (vel_x > 0) vel_x--;
                else if (vel_x < 0) vel_x++;
                vel_y--;
            }
            if (probe.inTargetArea(target_area)) {
                distinct_init_vels_hitting_target++;
            }
        }
    }
    std::cout << "Distinct initial velocity values hitting target area: " << distinct_init_vels_hitting_target << std::endl;


}