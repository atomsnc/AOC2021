#include <iostream>
#include <vector>
#include <fstream>
#include <numeric>

inline void split(const std::string &string, char delim, std::vector<unsigned long> &ints) {
    size_t start;
    size_t end = 0;
    while ((start = string.find_first_not_of(delim, end)) != std::string::npos) {
        end = string.find(delim, start);
        std::string number_literal = string.substr(start, end - start);
        ints.emplace_back(std::stoi(number_literal));
    }
}

void readEntries(std::string_view filepath, std::vector<unsigned long> &lanternfishes) {
    std::ifstream file(filepath.data());
    std::string line;

    std::getline(file, line);

    split(line, ',', lanternfishes);
}

void simulateLanternfishSchoolNaive(std::vector<unsigned long> &lanternfishes, unsigned long days) {
    unsigned long birth_new_lanternfish = 0;
    for (unsigned long day = 0; day < days; day++) {
        std::for_each(lanternfishes.begin(), lanternfishes.end(), [&birth_new_lanternfish](unsigned long &lanternfish) {
            if (lanternfish == 0) {
                lanternfish = 6;
                birth_new_lanternfish++;
            } else [[likely]] {
                lanternfish--;
            }
        });
        for (; birth_new_lanternfish > 0; birth_new_lanternfish--) {
            lanternfishes.emplace_back(8);
        }
    }
    std::cout << "Day " << days << " Total lanternfish: " << lanternfishes.size() << std::endl;
}

void simulateLanternfishSchool(std::vector<unsigned long> lanternfishes_count_by_day_bucket, unsigned long days) {
    unsigned long birth_new_lanternfish;
    for (unsigned long day = 0; day < days; day++) {
        birth_new_lanternfish = lanternfishes_count_by_day_bucket[0];
        lanternfishes_count_by_day_bucket[0] = lanternfishes_count_by_day_bucket[1];
        lanternfishes_count_by_day_bucket[1] = lanternfishes_count_by_day_bucket[2];
        lanternfishes_count_by_day_bucket[2] = lanternfishes_count_by_day_bucket[3];
        lanternfishes_count_by_day_bucket[3] = lanternfishes_count_by_day_bucket[4];
        lanternfishes_count_by_day_bucket[4] = lanternfishes_count_by_day_bucket[5];
        lanternfishes_count_by_day_bucket[5] = lanternfishes_count_by_day_bucket[6];
        lanternfishes_count_by_day_bucket[6] = lanternfishes_count_by_day_bucket[7] + birth_new_lanternfish;
        lanternfishes_count_by_day_bucket[7] = lanternfishes_count_by_day_bucket[8];
        lanternfishes_count_by_day_bucket[8] = birth_new_lanternfish;
    }

    std::cout << "Day " << days << " Total lanternfish: "
              << std::accumulate(lanternfishes_count_by_day_bucket.begin(), lanternfishes_count_by_day_bucket.end(),
                                 static_cast<unsigned long>(0)) << std::endl;
}

int main() {
    std::string filepath = "input.txt";

    std::vector<unsigned long> lanternfishes;
    readEntries(filepath, lanternfishes);

    std::vector<unsigned long> lanternfishes_count_by_day_bucket(9);

    for (auto lanternfish: lanternfishes) {
        lanternfishes_count_by_day_bucket[lanternfish]++;
    }
    std::cout << sizeof(unsigned long) << std::endl;

    //    Part 1
    simulateLanternfishSchool(lanternfishes_count_by_day_bucket, 80);

    //    Part 2
    simulateLanternfishSchool(lanternfishes_count_by_day_bucket, 256);

    return 0;
}