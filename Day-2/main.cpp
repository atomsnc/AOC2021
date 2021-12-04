#include <iostream>
#include <utility>
#include <vector>
#include <fstream>

class Command {
public:
    std::string type;
    int64_t unit;

    Command(std::string type, int64_t unit)
            : type(std::move(type)), unit(unit) {}
};

class Submarine {
public:
    int64_t horizontal_position;
    int64_t depth;
    int64_t aim;

    Submarine()
            : horizontal_position(0), depth(0), aim(0) {}
};

inline void readCommands(std::string_view filepath, std::vector<Command> &command_list) {
    auto file = std::ifstream(filepath.data());
    std::string type;
    int64_t unit;
    while (file >> type >> unit) {
//        std::cout << type << " - " << unit << std::endl;
        command_list.emplace_back(Command(type, unit));
    }
}

int main() {
    std::string filepath = "input.txt";

    std::vector<Command> command_list;
    readCommands(filepath, command_list);

    auto submarine = Submarine();

//    Part 1

    for (Command &command: command_list) {
        switch (command.type[0]) {
            case 'f': {
                submarine.horizontal_position += command.unit;
                break;
            }
            case 'u': {
                submarine.depth -= command.unit;
                break;
            }
            case 'd': {
                submarine.depth += command.unit;
                break;
            }
            default:
                throw std::runtime_error("Unknown command found.");
        }
    }

    std::cout << "Submarine is at position " << submarine.horizontal_position << " and depth " << submarine.depth
              << std::endl;

//    Part 2

    Submarine submarine2 = Submarine();

    for (Command &command: command_list) {
        switch (command.type[0]) {
            case 'f': {
                submarine2.horizontal_position += command.unit;
                submarine2.depth += submarine2.aim * command.unit;
                break;
            }
            case 'u': {
                submarine2.aim -= command.unit;
                break;
            }
            case 'd': {
                submarine2.aim += command.unit;
                break;
            }
            default:
                throw std::runtime_error("Unknown command found.");
        }
//        std::cout << "Submarine_2 -> position: " << submarine2.horizontal_position << "\tdepth: "
//                  << submarine2.depth << "\taim: " << submarine2.aim << std::endl;
    }
    std::cout << "Submarine_2 is at position " << submarine2.horizontal_position << " and depth " << submarine2.depth
              << std::endl;

    return 0;
}
