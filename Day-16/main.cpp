#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <bitset>

class Packet {
public:
    uint8_t id, version;
    Packet(uint8_t version, uint8_t id): id(id), version(version) {}
};

std::string inline convertHexToBinary(char character) {
    switch (std::toupper(character)) {
        case '0':
            return "0000";
        case '1':
            return "0001";
        case '2':
            return "0010";
        case '3':
            return "0011";
        case '4':
            return "0100";
        case '5':
            return "0101";
        case '6':
            return "0110";
        case '7':
            return "0111";
        case '8':
            return "1000";
        case '9':
            return "1001";
        case 'A':
            return "1010";
        case 'B':
            return "1011";
        case 'C':
            return "1100";
        case 'D':
            return "1101";
        case 'E':
            return "1110";
        case 'F':
            return "1111";
        default:
            throw std::runtime_error("Bad hex character detected.");
    }
}

std::string convertTransmission(std::string &hex_tranmission) {
    std::stringstream stream;
    for (char &hex : hex_tranmission) {
        stream << convertHexToBinary(hex);
    }
    return stream.str();
}


std::string readTransmission(std::string_view filepath) {
    std::ifstream file(filepath.data());
    std::string line;
    std::getline(file, line);
    return line;
}

bool parseTransmission(std::stringstream &stream, std::vector<std::unique_ptr<Packet>> &packets, uint64_t end_position) {
    std::cout << "Current position: " << stream.tellg() << " End position: " << end_position << std::endl;
    if (end_position - stream.tellg() < 8) return false;

    uint8_t id, version;
    std::stringstream ss;

    {
        char buf[4];

        stream >> buf;
        std::cout << "Version: " << buf << std::endl;
        version = std::stoul(buf, nullptr, 2);
        stream >> buf;
        std::cout << "ID: " << buf << std::endl;
        id = std::stoul(buf, nullptr, 2);
    }

    switch (id) {
        /* Literal */
        case 4: {
            char buf[6] = "11111";
            while (buf[0] != '0') {
                stream >> buf;
                ss << &buf[1];
            }
            uint64_t literal = std::stoul(ss.str(), nullptr, 2);
            packets.emplace_back(std::make_unique<Packet>(version, id));
            break;
        }
        /* Operators */
        default: {
            std::cout << "Operator Packet" << std::endl;
            uint8_t length_type = 2;
            {
                char buf[2];
                stream >> buf;
                switch (buf[0]) {
                    case '0': length_type = 0; break;
                    case '1': length_type = 1; break;
                    default: throw std::runtime_error("Unknown length type found");
                }
            }
            packets.emplace_back(std::make_unique<Packet>(version, id));

            /* Fixed bits length */
            if (length_type == 0) {
                char buf[16];
                stream >> buf;
                uint64_t total_bits = std::stoul(buf, nullptr, 2);
                std::cout << "Fixed bits: " << std::endl;
                uint64_t current_position = stream.tellg();
                while (parseTransmission(stream, packets, current_position + total_bits));
            } /* Fixed sub-packets */
            else {
                char buf[12];
                stream >> buf;
                uint64_t total_packets = std::stoul(buf, nullptr, 2);
                std::cout << "Total sub-packets: " << total_packets << std::endl;
                for (int count = 0; count < total_packets; count++) {
                    parseTransmission(stream, packets, end_position);
                }
            }
            break;
        }
    }
    return true;
}

int main() {
    std::string filepath = "input.txt";
    std::string hex_transmission = readTransmission(filepath);
    std::string binary_tranmission = convertTransmission(hex_transmission);

    std::cout << binary_tranmission << std::endl;
    std::stringstream stream(binary_tranmission);
    uint64_t end_position = binary_tranmission.size();
    std::vector<std::unique_ptr<Packet>> packets;
    while (parseTransmission(stream, packets, end_position));

    uint64_t versions_sum = 0;
    for (auto &packet : packets) {
        versions_sum += packet->version;
    }

    std::cout << "Version total sum: " << versions_sum << std::endl;
}