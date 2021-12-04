#include <iostream>
#include <utility>
#include <vector>
#include <fstream>

#define BOARD_SIZE 5

class Cell {
public:
    int number;
    bool marked;

    explicit Cell(int number) : number(number) {
        this->marked = false;
    }
};

class Table {
public:
    std::vector<std::vector<Cell>> cells;

    Table() = default;
};

inline void split(const std::string &string, char delim, std::vector<std::string> &strings) {
    size_t start;
    size_t end = 0;
    while ((start = string.find_first_not_of(delim, end)) != std::string::npos) {
        end = string.find(delim, start);
        strings.emplace_back(string.substr(start, end - start));
    }
}

void readCallsAndTables(std::string_view filepath, std::vector<int> &calls, std::vector<Table> &tables) {
    std::ifstream file(filepath.data());

    std::string calls_list;
    std::getline(file, calls_list);

    std::vector<std::string> call_literals;

    split(calls_list, ',', call_literals);

    for (auto &call_literal: call_literals) {
        calls.emplace_back(std::stoi(call_literal));
    }

    std::string line;
    std::vector<Cell> cell_row;
    Table *table = nullptr;
    while (std::getline(file, line)) {
        if (line.empty()) {
            if (table != nullptr) {
                tables.emplace_back(std::move(*table));
                delete table;
            }
            table = new Table();
        } else {
            std::vector<std::string> number_literals;
            split(line, ' ', number_literals);
            for (auto &number_literal: number_literals) {
                cell_row.emplace_back(Cell(std::stoi(number_literal)));
            }
            table->cells.emplace_back(cell_row);
            cell_row.clear();
        }
    }
    if (table != nullptr) {
        tables.emplace_back(std::move(*table));
        delete table;
    }

}

// Return index of table that won.
int checkWinnerTable(std::vector<Table> &tables) {
    for (int table_index = 0; auto &table: tables) {
        for (int row_index = 0; auto &cell_row: table.cells) {
            bool filled = true;
            for (auto &cell: cell_row) {
                if (!cell.marked) {
                    filled = false;
                    break;
                }
            }
            if (filled) {
//                std::cout << "Row " << row_index << " of table " << table_index << " is filled." << std::endl;
                return table_index;
            }
            row_index++;
        }
        for (int column_index = 0; column_index < table.cells[0].size();) {
            bool filled = true;
            for (auto &cell_row: table.cells) {
                if (!cell_row[column_index].marked) {
                    filled = false;
                    break;
                }
            }
            if (filled) {
//                std::cout << "Column " << column_index << " of table " << table_index << " is filled." << std::endl;
                return table_index;
            }
            column_index++;
        }
        table_index++;
    }
    return -1;
}

int main() {
    std::string filepath = "input.txt";

    std::vector<int> calls;
    std::vector<Table> tables, tables_2;

    readCallsAndTables(filepath, calls, tables);
    tables_2 = std::vector<Table>(tables);

    //    Part 1

    int winning_table, winning_number = -1;

    for (int call: calls) {
        for (auto &table: tables) {
            for (auto &cell_row: table.cells) {
                for (auto &cell: cell_row) {
                    if (cell.number == call) {
                        cell.marked = true;
                    }
                }
            }
        }
        winning_table = checkWinnerTable(tables);
        if (winning_table > -1) {
            winning_number = call;
            break;
        }
    }

    int unmarked_sum = 0;
    for (auto &cell_row: tables[winning_table].cells) {
        for (auto &cell: cell_row) {
            if (!cell.marked) {
                unmarked_sum += cell.number;
            }
        }
    }
    std::cout << "Final score: " << unmarked_sum * winning_number << std::endl;

    //    Part 2

    std::vector<Table> winning_tables;
    std::vector<int> winning_calls;

    for (int call: calls) {
        for (auto &table: tables_2) {
            for (auto &cell_row: table.cells) {
                for (auto &cell: cell_row) {
                    if (cell.number == call) {
                        cell.marked = true;
                    }
                }
            }
        }
        for (;;) {
            winning_table = checkWinnerTable(tables_2);
            if (winning_table > -1) {
                winning_tables.emplace_back(tables_2[winning_table]);
                winning_calls.emplace_back(call);
                tables_2.erase(tables_2.begin() + winning_table);
            } else {
                break;
            }
        }
    }

    unmarked_sum = 0;
    for (auto &cell_row: winning_tables.back().cells) {
        for (auto &cell: cell_row) {
            if (!cell.marked) {
                unmarked_sum += cell.number;
            }
        }
    }
    std::cout << "Final score last table: " << unmarked_sum * winning_calls.back() << std::endl;

    return 0;

}