#include <iostream>
#include <utility>
#include <vector>
#include <fstream>
#include <sstream>

inline void readEntries(std::string_view filepath, std::vector<std::string> &entries) {
    auto file = std::ifstream(filepath.data());
    std::string number;
    while (file >> number) {
        entries.emplace_back(std::move(number));
    }
}

inline unsigned long convertLiteralToNumber(const std::string &binary_literal) {
    return std::strtoul(binary_literal.c_str(), nullptr, 2);
}

void countZerosAndOnes(std::vector<std::string> &entries, std::vector<unsigned int> &zero_counts, std::vector<unsigned int> &one_counts) {
    for (auto &entry: entries) {
        for (size_t index = 0; auto &number_literal: entry) {
            switch (number_literal) {
                case '0': {
                    ++zero_counts[index];
                    break;
                }
                case '1': {
                    ++one_counts[index];
                    break;
                }
                default:
                    throw std::runtime_error("Unknown number found.");
                    break;
            }
            ++index;
        }
    }
}

int main() {
    std::string filepath = "input.txt";
    std::vector<std::string> entries;

    readEntries(filepath, entries);

    auto zero_counts = std::vector<unsigned int>(entries[0].size());
    auto one_counts = std::vector<unsigned int>(entries[0].size());

    countZerosAndOnes(entries, zero_counts, one_counts);
    
    //    Part 1

    std::stringstream gamma_rate_literal, epsilon_rate_literal;
    unsigned long gamma_rate, epsilon_rate, power_consumption;

    for (size_t index = 0; index < entries[0].size(); index++) {
        if (zero_counts[index] < one_counts[index]) {
            gamma_rate_literal << '1';
            epsilon_rate_literal << '0';
        } else {
            gamma_rate_literal << '0';
            epsilon_rate_literal << '1';
        }
    }

    gamma_rate = convertLiteralToNumber(gamma_rate_literal.str());
    epsilon_rate = convertLiteralToNumber(epsilon_rate_literal.str());

    std::cout << "Gamma rate literal: " << gamma_rate_literal.str() << std::endl;
    std::cout << "Epsilon rate literal: " << epsilon_rate_literal.str() << std::endl;
    std::cout << "Gamma rate: " << gamma_rate << std::endl;
    std::cout << "Gamma rate literal: " << epsilon_rate << std::endl;
    std::cout << "Power consumption: " << gamma_rate * epsilon_rate << std::endl;

    //    Part 2

    std::vector<std::string> entries_oxygen = entries;
    std::vector<std::string> entries_carbon_dioxide = entries;
    
    for (size_t index = 0; index < entries[0].size(); index++) {
        {
            auto zeros = std::vector<unsigned int>(entries[0].size());
            auto ones = std::vector<unsigned int>(entries[0].size());
            countZerosAndOnes(entries_oxygen, zeros, ones);
            entries_oxygen.erase(std::remove_if(entries_oxygen.begin(), entries_oxygen.end(),
                                                [&zeros, &ones, &index](std::string &entry) {
                                                    if (zeros[index] == ones[index]) {
                                                        return entry[index] != '1';
                                                    } else if (zeros[index] < ones[index]) {
                                                        return entry[index] != '1';
                                                    } else {
                                                        return entry[index] != '0';
                                                    }
                                                }), entries_oxygen.end());
        }

        {
            auto zeros = std::vector<unsigned int>(entries[0].size());
            auto ones = std::vector<unsigned int>(entries[0].size());
            countZerosAndOnes(entries_carbon_dioxide, zeros, ones);
            entries_carbon_dioxide.erase(std::remove_if(entries_carbon_dioxide.begin(), entries_carbon_dioxide.end(),
                                                        [&zeros, &ones, &index](std::string &entry) {
                                                            if (zeros[index] == ones[index]) {
                                                                return entry[index] != '0';
                                                            } else if (zeros[index] < ones[index]) {
                                                                return entry[index] != '0';
                                                            } else {
                                                                return entry[index] != '1';
                                                            }
                                                        }), entries_carbon_dioxide.end());
        }
    }

    unsigned int oxygen_rating = convertLiteralToNumber(entries_oxygen[0]);
    unsigned int carbon_dioxide_rating = convertLiteralToNumber(entries_carbon_dioxide[0]);

    std::cout << "O2 rating: " << oxygen_rating << std::endl;
    std::cout << "CO2 rating: " << carbon_dioxide_rating << std::endl;
    std::cout <<  "Life support rating: " << oxygen_rating * carbon_dioxide_rating << std::endl;

    return 0;
}
