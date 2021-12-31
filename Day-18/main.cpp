#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <stack>
#include <memory>
#include <sstream>
#include <cmath>

class Container {
public:
    enum State {
        LEFT,
        RIGHT,
        NONE
    };
    enum Type {
        NUMBER,
        PAIR
    };

    Container(unsigned int depth) : value(), depth(depth), leaf_state(LEFT), type(PAIR), state(NONE) {}

    Container(unsigned int depth, Container::State state)
            : value(), depth(depth), leaf_state(LEFT), type(PAIR), state(state) {}

    Container(unsigned int depth, unsigned int value, Container::State state) : value(value), depth(depth),
                                                                                leaf_state(NONE), state(state),
                                                                                type(NUMBER) {}

    bool isNumber() {
        return type == NUMBER;
    }

    bool isPair() {
        return type == PAIR;
    }

    void setLeaf(const std::shared_ptr<Container> &container) {
        switch (leaf_state) {
            case LEFT:
                setLeft(container);
                break;
            case RIGHT:
                setRight(container);
                break;
            default:
                throw std::runtime_error("Unknown container state.");
        }
    }

    void setLeafState(Container::State new_state) {
        leaf_state = new_state;
    }

    void setLeft(const std::shared_ptr<Container> &container) {
        left = container;
    }

    void setRight(const std::shared_ptr<Container> &container) {
        right = container;
    }

    void setParent(const std::shared_ptr<Container> &container) {
        parent = container;
    }

    void setValue(unsigned int new_value) {
        value = new_value;
    }

    void setState(Container::State new_state) {
        state = new_state;
    }

    [[nodiscard]] unsigned int get() const {
        return value;
    }

    [[nodiscard]] unsigned int getDepth() const {
        return depth;
    }

    void increaseDepth() {
        depth++;
        if (left) left->increaseDepth();
        if (right) right->increaseDepth();
    }

    std::shared_ptr<Container> getRoot() {
        if (!parent) return std::shared_ptr<Container>(this);
        else return parent->getRoot();
    }

    std::shared_ptr<Container> getLeft() {
        return left;
    }

    std::shared_ptr<Container> getRight() {
        return right;
    }

    std::shared_ptr<Container> getParent() {
        return parent;
    }

    Container::State getLeafState() {
        return leaf_state;
    }

    Container::State getState() {
        return state;
    }

    unsigned int getMagnitude() {
        if (this->isNumber()) return value;
        else {
            unsigned int magnitude = (3 * this->getLeft()->getMagnitude()) + (2 * this->getRight()->getMagnitude());
            return magnitude;
        }
    }

private:
    std::shared_ptr<Container> left, right, parent;
    unsigned int value, depth;
    Type type;
    State leaf_state, state;
};

void readEntries(std::string_view filepath, std::vector<std::shared_ptr<Container>> &numbers) {
    std::ifstream file(filepath.data());
    std::string line;

    std::stack<std::shared_ptr<Container>> stack;

    unsigned int depth = 0;

    while (std::getline(file, line)) {
        std::string number = std::string();
        for (auto &c: line) {
            switch (c) {
                case '[': {
                    std::shared_ptr<Container> container;
                    if (stack.empty())
                        container = std::make_shared<Container>(depth++);
                    else
                        container = std::make_shared<Container>(depth++, stack.top()->getLeafState());
                    stack.push(container);
                    break;
                }
                case ',': {
                    if (!number.empty()) {
                        auto container = std::make_shared<Container>(stack.top()->getDepth(), std::stoul(number),
                                                                     stack.top()->getLeafState());
                        container->setParent(stack.top());
                        stack.top()->setLeaf(container);
                        number = std::string();
                    }
                    stack.top()->setLeafState(Container::State::RIGHT);
                    break;
                }
                case ']': {
                    if (!number.empty()) {
                        auto container = std::make_shared<Container>(stack.top()->getDepth(), std::stoul(number),
                                                                     stack.top()->getLeafState());
                        container->setParent(stack.top());
                        stack.top()->setLeaf(container);
                        number = std::string();
                    }
                    auto container = stack.top();
                    stack.pop();
                    depth--;
                    if (stack.empty()) {
                        numbers.push_back(container);
                    } else {
                        container->setParent(stack.top());
                        stack.top()->setLeaf(container);
                    }
                    break;
                }
                default: {
                    number += c;
                    break;
                }
            }
        }
    }
}

void printNumber(const std::shared_ptr<Container> &container) {
    if (container->isNumber()) {
        std::cout << container->get();
    } else if (container->isPair()) {
        std::cout << '[';
        printNumber(container->getLeft());
        std::cout << ',';
        printNumber(container->getRight());
        std::cout << ']';
    }
}

void createAdjacencyList(const std::shared_ptr<Container> &container,
                         std::vector<std::shared_ptr<Container>> &value_containers) {
    if (container->isNumber()) {
        value_containers.emplace_back(container);
    } else if (container->isPair()) {
        createAdjacencyList(container->getLeft(), value_containers);
        createAdjacencyList(container->getRight(), value_containers);
    }
}

std::shared_ptr<Container>
addNumbers(std::shared_ptr<Container> &number_left, std::shared_ptr<Container> &number_right) {
    std::shared_ptr<Container> current_container;
    current_container = std::make_shared<Container>(0);
    current_container->setLeaf(number_left);
    number_left->setParent(current_container);
    number_left->setState(Container::State::LEFT);
    current_container->setLeafState(Container::State::RIGHT);
    current_container->setLeaf(number_right);
    number_right->setParent(current_container);
    number_right->setState(Container::State::RIGHT);

    current_container->getLeft()->increaseDepth();
    current_container->getRight()->increaseDepth();
    return current_container;
};

bool explode(const std::shared_ptr<Container> &sum) {
    std::vector<std::shared_ptr<Container>> value_containers;
    createAdjacencyList(sum, value_containers);

    for (unsigned int i = 0; i < value_containers.size(); i++) {
        if (value_containers[i]->getDepth() == 4) {
            // Get parent;
            auto parent = value_containers[i]->getParent();
            auto parent_parent = parent->getParent();
            // Check if both children of parent are actual values and not pairs
            if (!parent->getLeft()->isNumber() || !parent->getRight()->isNumber()) {
                throw std::runtime_error("Both children are not values");
            }

            auto container = std::make_shared<Container>(parent_parent->getDepth(), 0, parent->getState());
            container->setParent(parent_parent);
            parent_parent->setLeafState(parent->getState());
            parent_parent->setLeaf(container);
            if (i != 0)
                value_containers[i - 1]->setValue(value_containers[i - 1]->get() + value_containers[i]->get());
            if (i < value_containers.size() - 2)
                value_containers[i + 2]->setValue(value_containers[i + 2]->get() + value_containers[i + 1]->get());
            return true;
        }
    }
    return false;
}

bool split(const std::shared_ptr<Container> &sum) {
    std::vector<std::shared_ptr<Container>> value_containers;
    createAdjacencyList(sum, value_containers);

    for (auto &value_container: value_containers) {
        if (value_container->get() >= 10) {
            // get parent
            auto parent = value_container->getParent();
            parent->setLeafState(value_container->getState());

            unsigned int new_depth = parent->getDepth() + 1;

            auto container = std::make_shared<Container>(new_depth, value_container->getState());
            container->setParent(parent);
            parent->setLeaf(container);

            auto left_value = std::make_shared<Container>(new_depth,
                                                          std::floor(static_cast<float>(value_container->get()) / 2),
                                                          Container::State::LEFT);
            left_value->setParent(container);
            auto right_value = std::make_shared<Container>(new_depth,
                                                           std::ceil(static_cast<float>(value_container->get()) / 2),
                                                           Container::State::RIGHT);
            right_value->setParent(container);

            container->setLeft(left_value);
            container->setRight(right_value);

            return true;
        }
    }
    return false;
}

void reduce(const std::shared_ptr<Container> &sum) {
    for (;;) {
        bool explode_result = explode(sum);
        if (explode_result) {
            continue;
        }
        bool split_result = split(sum);
        if (split_result) {
            continue;
        }
        break;
    }
}

int main() {
    std::string filepath = "input.txt";
    std::vector<std::shared_ptr<Container>> numbers;

    readEntries(filepath, numbers);

    auto sum = addNumbers(numbers[0], numbers[1]);
    reduce(sum);

    for (unsigned int i = 2; i < numbers.size(); i++) {
        sum = addNumbers(sum, numbers[i]);
        reduce(sum);
    }

    std::cout << "Magnitude: " << sum->getMagnitude() << std::endl;

    unsigned int largest_magnitude = 0;
    for (unsigned int i = 0; i < numbers.size() - 1; i++) {
        for (unsigned int j = i + 1; j < numbers.size(); j++) {
            numbers.clear();
            readEntries(filepath, numbers);

            auto t_sum = addNumbers(numbers[i], numbers[j]);
            reduce(t_sum);
            unsigned int magnitude = t_sum->getMagnitude();

            if (magnitude > largest_magnitude) largest_magnitude = magnitude;

            numbers.clear();
            readEntries(filepath, numbers);

            t_sum = addNumbers(numbers[j], numbers[i]);
            reduce(t_sum);
            magnitude = t_sum->getMagnitude();

            if (magnitude > largest_magnitude) largest_magnitude = magnitude;
        }
    }


    std::cout << "Largest Magnitude: " << largest_magnitude << std::endl;
}