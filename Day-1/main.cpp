#include <iostream>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <filesystem>

std::string get_current_dir() {
    char buff[FILENAME_MAX]; //create string buffer to hold path
    getcwd(buff, FILENAME_MAX);
    std::string current_working_dir(buff);
    return current_working_dir;
}

std::string get_current_directory_contents() {
    std::string current_directory = get_current_dir();

    std::cout << "Current directory listing: " << std::endl;
    for (auto &entry: std::filesystem::directory_iterator(current_directory)) {
        std::cout << entry.path() << std::endl;
    }
}

inline void read_entries(std::string_view filepath, std::vector<int> &entries) {
    std::ifstream file;

    file.open(filepath.data(), std::ifstream::in);

    if (!file.is_open()) {
        throw std::runtime_error("File is not open.");
    }

    int current;
    while (file >> current) {
        entries.emplace_back(current);
    }
}

int main() {
    std::string filepath = "input.txt";

    std::cout << "Current working directory is " << get_current_dir() << std::endl;

    std::vector<int> entries;
    read_entries(filepath, entries);

    int total_measurements_larger_than_previous = 0;

    for (auto index = 1; index < entries.size(); index++) {
        int previous = entries[index - 1];
        int current = entries[index];

        if (current > previous) {
//            std::cout << current << " > " << previous << std::endl;
            total_measurements_larger_than_previous++;
        }
    }

    std::cout << "Total measurements greater than their previous measurement: "
              << total_measurements_larger_than_previous << std::endl;

    int total_window_measurements_larger_than_previous = 0;

    for (auto index = 1; index + 2 < entries.size(); index++) {
        int current_window_measurement = entries[index] + entries[index + 1] + entries[index + 2];
        int previous_window_measurement = entries[index - 1] + entries[index] + entries[index + 1];

        if (current_window_measurement > previous_window_measurement) {
//            std::cout << current_window_measurement << " > " << previous_window_measurement << std::endl;
            total_window_measurements_larger_than_previous++;
        }
    }

    std::cout << "Total window measurements greater than their previous measurement: "
              << total_window_measurements_larger_than_previous << std::endl;


    return 0;
}
