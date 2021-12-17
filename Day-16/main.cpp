#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <bitset>
#include <optional>
#include <numeric>
#include <chrono>

class Transmission {
public:
    std::shared_ptr<std::stringstream> stream;
    uint64_t end_position;

    Transmission(std::string &transmission): end_position(transmission.size()) {
        stream = std::make_shared<std::stringstream>(transmission);
    }
    Transmission(std::shared_ptr<std::stringstream> &stream, uint64_t end_position): end_position(end_position) {
        this->stream = stream;
    }

};

class Packet {
public:
    uint8_t id, version;
    std::vector<std::shared_ptr<Packet>> sub_packets;

    explicit Packet(uint8_t version, uint8_t id) : id(id), version(version) {}

    uint64_t getVersionsSum() {
        uint64_t version_sum = version;
        for (const auto& packet : sub_packets) {
            version_sum += packet->getVersionsSum();
        }
        return version_sum;
    }

    virtual uint64_t compute() = 0;
    virtual void getSubPackets() = 0;

    static std::optional<std::shared_ptr<Packet>> parsePacket(Transmission &transmission);
};

class LiteralPacket : public Packet {
public:
    uint64_t value;

    LiteralPacket(Transmission &transmission, uint8_t version, uint8_t id): Packet(version, id) {
        std::stringstream ss;
        char buf[6] = "11111";
        while (buf[0] != '0') {
            *(transmission.stream) >> buf;
            ss << &buf[1];
        }
        value = std::stoul(ss.str(), nullptr, 2);
    }

    uint64_t compute() override {
        return value;
    }

    void getSubPackets() override {}
};

class OperatorPacket : public Packet {
public:
    uint8_t length_type;

    OperatorPacket(Transmission &transmission, uint8_t version, uint8_t id): Packet(version, id) {
        {
            char buf[2];
            *(transmission.stream) >> buf;
            switch (buf[0]) {
                case '0':
                    length_type = 0;
                    break;
                case '1':
                    length_type = 1;
                    break;
                default:
                    throw std::runtime_error("Unknown length type found");
            }
        }
        /* Fixed bits length */
        if (length_type == 0) {
            char buf[16];
            *(transmission.stream) >> buf;
            uint64_t total_bits = std::stoul(buf, nullptr, 2);
            uint64_t current_position = transmission.stream->tellg();
            Transmission sub_transmission(transmission.stream, total_bits + current_position);
            for(;;) {
                auto packet = Packet::parsePacket(sub_transmission);
                if (!packet.has_value()) break;
                sub_packets.emplace_back(*packet);
            }
        } /* Fixed sub-packets */
        else {
            char buf[12];
            *(transmission.stream) >> buf;
            uint64_t total_packets = std::stoul(buf, nullptr, 2);
            for (int count = 0; count < total_packets; count++) {
                auto packet = Packet::parsePacket(transmission);
                    sub_packets.emplace_back(*packet);
            }
        }
    }

    uint64_t compute() override {
        switch (id) {
            case 0: {
                return std::accumulate(sub_packets.begin(), sub_packets.end(), (uint64_t) 0, [](uint64_t sum, auto &packet) {
                    return sum + packet->compute();
                });
            }
            case 1: {
                return std::accumulate(sub_packets.begin(), sub_packets.end(), (uint64_t) 1, [](uint64_t product, auto &packet) {
                    return product * packet->compute();
                });
            }
            case 2: {
               auto it = std::min_element(sub_packets.begin(), sub_packets.end(), [](auto &packet_1, auto &packet_2) -> bool {
                   return packet_1->compute() < packet_2->compute();
               });
               return (*it)->compute();
            }
            case 3: {
                auto it = std::max_element(sub_packets.begin(), sub_packets.end(), [](auto &packet_1, auto &packet_2) -> bool {
                    return packet_1->compute() < packet_2->compute();
                });
                return (*it)->compute();
            }
            case 5: {
                if (sub_packets.size() > 2) throw std::runtime_error("More than 2 packets in operator with ID 5");
                return sub_packets[0]->compute() > sub_packets[1]->compute();
            }
            case 6: {
                if (sub_packets.size() > 2) throw std::runtime_error("More than 2 packets in operator with ID 6");
                return sub_packets[0]->compute() < sub_packets[1]->compute();
            }
            case 7: {
                if (sub_packets.size() > 2) throw std::runtime_error("More than 2 packets in operator with ID 7");
                return sub_packets[0]->compute() == sub_packets[1]->compute();
            }
            default:
                throw std::runtime_error("Error while computing.");
        }
    }

    void getSubPackets() override {
        for (auto &packet: sub_packets) packet->getSubPackets();
        std::cout << '[';
        for (uint64_t index = 0; index < sub_packets.size() - 1; index++) {
            std::cout << sub_packets[index]->compute() << ", ";
        }
        std::cout << sub_packets[sub_packets.size() - 1]->compute() << "]" << std::endl;
    }
};

std::optional<std::shared_ptr<Packet>> Packet::parsePacket(Transmission &transmission) {
    if (transmission.end_position - transmission.stream->tellg() < 8) return {};

    uint8_t id, version;
    std::stringstream ss;

    {
        char buf[4];
        *(transmission.stream) >> buf;
        version = std::stoul(buf, nullptr, 2);
        *(transmission.stream) >> buf;
        id = std::stoul(buf, nullptr, 2);
    }
    if (id == 4) {
        return std::make_shared<LiteralPacket>(transmission, version, id);
    } else {
        return std::make_shared<OperatorPacket>(transmission, version, id);
    }

    /* Sanity: Never reachable */
    throw std::runtime_error("Error while parsing.");
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
    for (char &hex: hex_tranmission) {
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

int main() {
    std::string filepath = "input.txt";
    std::string hex_transmission = readTransmission(filepath);

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto start = high_resolution_clock::now();

    std::string binary_tranmission = convertTransmission(hex_transmission);

    Transmission transmission(binary_tranmission);

    auto packet = Packet::parsePacket(transmission);
    if (!packet.has_value()) throw std::runtime_error("No packet found?");

    uint64_t versions_sum = (*packet)->getVersionsSum();

    std::cout << "Version total sum: " << versions_sum << std::endl;

    uint64_t evaluation = (*packet)->compute();

    std::cout << "Evaluation: " << evaluation << std::endl;

    auto end = high_resolution_clock::now();
    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = end - start;
    std::cout << "Time taken: " << ms_double.count() << "ms";
}