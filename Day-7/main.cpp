#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <chrono>

inline void split(const std::string &string, char delim, std::vector<int> &ints) {
    size_t start;
    size_t end = 0;
    while ((start = string.find_first_not_of(delim, end)) != std::string::npos) {
        end = string.find(delim, start);
        std::string number_literal = string.substr(start, end - start);
        ints.emplace_back(std::stoi(number_literal));
    }
}

void readEntries(std::string_view filepath, std::vector<int> &crab_positions) {
    std::ifstream file(filepath.data());
    std::string line;

    std::getline(file, line);

    split(line, ',', crab_positions);
}

void minimizeFuel(std::vector<int> &crab_positions) {
    int max_crab_position = *std::max_element(crab_positions.begin(), crab_positions.end());

    int min_fuel_amount = std::numeric_limits<int>::max();
    int min_fuel_amount_position = -1;
    for (int current_position = 0; current_position <= max_crab_position; current_position++) {
        int fuel_amount = 0;
        for (auto crab_position: crab_positions) {
            fuel_amount += std::abs(current_position - crab_position);
        }
        if (fuel_amount < min_fuel_amount) {
            min_fuel_amount_position = current_position;
            min_fuel_amount = fuel_amount;
        }
//        std::cout << "Fuel amount at position " << current_position << ": " << fuel_amount << std::endl;

    }
    std::cout << "Final minimum fuel amount is at position " << min_fuel_amount_position << ": " << min_fuel_amount
              << std::endl;
}

void minimizeFuelSum(std::vector<int> &crab_positions) {
    int max_crab_position = *std::max_element(crab_positions.begin(), crab_positions.end());

    int min_fuel_amount = std::numeric_limits<int>::max();
    int min_fuel_amount_position = -1;
    for (int current_position = 0; current_position <= max_crab_position; current_position++) {
        int fuel_amount = 0;
        for (auto crab_position: crab_positions) {
            int difference = std::abs(current_position - crab_position);
            fuel_amount += (difference * (difference + 1))/2;
        }
        if (fuel_amount < min_fuel_amount) {
            min_fuel_amount_position = current_position;
            min_fuel_amount = fuel_amount;
        }
//        std::cout << "Fuel amount at position " << current_position << ": " << fuel_amount << std::endl;

    }
    std::cout << "Final minimum fuel amount is at position " << min_fuel_amount_position << ": " << min_fuel_amount
              << std::endl;
}

int main() {
    std::string filepath = "input.txt";

    std::vector<int> crab_positions;

    readEntries(filepath, crab_positions);

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto start = high_resolution_clock::now();

    minimizeFuel(crab_positions);
    minimizeFuelSum(crab_positions);

    auto end = high_resolution_clock::now();
    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = end - start;
    std::cout << "Time taken: " << ms_double.count() << "ms";
}