// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#include "TableEntry.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include <getopt.h>
#include <algorithm>



// TABLE ENTRY COMPARISON FUNCTORS

class Less {
private:
    TableEntry t;
    size_t index;

public:
    Less(TableEntry t_in, size_t index_in)
        : t(t_in), index(index_in) {}

    // Returns true of data in is less than data from class
    bool operator()(std::vector<TableEntry> row) {
        return t > row[index];
    }
};

class Greater {
private:
    TableEntry t;
    size_t index;
public:
    Greater(TableEntry t_in, size_t index_in)
        : t(t_in), index(index_in) {}

    // Returns true of data in is greater than data from class
    bool operator()(std::vector<TableEntry> row) {
        return t < row[index];
    }
};

class Equal {
private:
    TableEntry t;
    size_t index;
public:
    Equal(TableEntry t_in, size_t index_in)
        : t(t_in), index(index_in) {}

    // Returns true of data in is equal to data from class
    bool operator()(std::vector<TableEntry> row) {
        return t == row[index];
    }
};

// TABLE STRUCT
class Table {
private:
    std::string name;
    std::string indexed_name;
    size_t indexed_col;
    uint32_t num_cols;
    std::unordered_map<TableEntry, std::vector<size_t>> hash_index;
    std::map<TableEntry, std::vector<size_t>> bst_index;
    std::vector<std::vector<TableEntry>> data;
    //std::vector<std::string> col_types;
    std::unordered_map<size_t, std::string> col_types;
    std::unordered_map<std::string, size_t> col_names;
    //std::vector<std::string> col_names;

     // Creates a TableEntry based on type given
    TableEntry create_entry(std::string& datum, std::string& type) {
        char t = type[0];
        if (t == 'b') {
            return TableEntry(datum[0] == 't' ? true : false);
        }
        else if (t == 'i') {
            return TableEntry(stoi(datum));
        }
        else if (t == 'd') {
            return TableEntry(stod(datum));
        }
        else {
            return TableEntry(datum);
        }
    }


public:
    Table(std::string name_in, uint32_t num_cols_in)
        : name(name_in), indexed_col(0), num_cols(num_cols_in) {}


    // FILLS MAPS FOR COL NAMES AND TYPES
    void update_types_names(size_t num_cols) {
        col_types.reserve(num_cols);
        col_names.reserve(num_cols);
        std::cout << "New table " << name << " with column(s) ";
        std::string type;
        std::string col_name;
        for (uint32_t i = 0; i < num_cols; ++i) {
            std::cin >> type;
            col_types.insert({ i, type });
        }
        for (uint32_t i = 0; i < num_cols; ++i) {
            std::cin >> col_name;
            col_names.insert({ col_name, i });
            std::cout << col_name << " ";
        }
    }

    std::string get_name() {
        return name;
    }

    // INSERTS DATA INTO TABLE
    // PRINTS "Added 8 rows to 281class from position 0 to 7"
    void insert() {
        uint32_t rows;
        std::string junk;
        std::cin >> rows >> junk;

        // Reserve size to be current size + new rows
        data.reserve(data.size() + rows);

        // Read data, make table entry, enter into row, enter row into 2d vector
        std::string datum;
        for (uint32_t i = 0; i < rows; ++i) {
            std::vector<TableEntry> row;
            for (size_t j = 0; j < num_cols; ++j) {
                std::cin >> datum;
                row.push_back(create_entry(datum, col_types[j]));

            }
            data.push_back(row);
        }

        if (!(hash_index.empty())) {
            regenerate_hash_index();
        }
        if (!(bst_index.empty())) {
            regenerate_bst_index();
        }
        std::cout << "Added " << rows << " rows to " << name << " from position ";
        std::cout << data.size() - rows << " to " << data.size() - 1 << "\n";

    }

    // DELETES ROWS BASED ON TABLE ENTRY VALUE
    void table_delete() {
        std::string junk;
        std::string table_name;
        std::string col_name;
        char op;
        std::string value;
        std::cin >> junk >> col_name >> op >> value;
        // Get it of column in col_names vector
        auto col_it = col_names.find(col_name);

        // Check if exists
        if (col_it == col_names.end()) {
            std::cout << "Error during DELETE: " << col_name << " does not name a column in " << table_name << "\n";
            getline(std::cin, junk);
            return;
        }

        // Iterator arithmetic to find index of column
        size_t col_index = col_it->second;


        auto it_remove = data.begin();
        size_t removed = 0;
        TableEntry v = create_entry(value, col_types[col_index]);

        switch (op) {
        case '<':
        {
            Less l(create_entry(value, col_types[col_index]), col_index);
            it_remove = remove_if(data.begin(), data.end(), l);
            break;
        }
        case '>':
        {
            Greater g(create_entry(value, col_types[col_index]), col_index);
            it_remove = remove_if(data.begin(), data.end(), g);
            break;
        }
        case '=':
        {
            Equal e(create_entry(value, col_types[col_index]), col_index);
            it_remove = remove_if(data.begin(), data.end(), e);
            break;
        }

        }

        removed = data.end() - it_remove;

        // Erase rows that meet criteria
        data.erase(it_remove, data.end());
        if (!(hash_index.empty())) {
            regenerate_hash_index();
        }
        if (!(bst_index.empty())) {
            regenerate_bst_index();
        }
        std::cout << "Deleted " << removed << " rows from " << name << "\n";


    }

    // PRINTS ALL SPECIFIED COLUMNS FROM TABLE
    void print_all(bool quiet, std::vector<std::string> cols) {

        for (size_t i = 0; i < cols.size(); ++i) {
            auto it = col_names.find(cols[i]);
            std::string junk;
            if (it == col_names.end()) {
                std::cout << "Error during PRINT: " << cols[i] << " does not name a column in " << name << "\n";
                getline(std::cin, junk);
                return;
            }
        }

        if (!quiet) {
            for (size_t i = 0; i < cols.size(); ++i) {
                std::cout << cols[i] << " ";
            }
            std::cout << "\n";
        }
        if (!quiet) {
            for (size_t row = 0; row < data.size(); ++row) {
                for (size_t col = 0; col < cols.size(); ++col) {
                    std::cout << data[row][col_names[cols[col]]] << " ";
                }
                std::cout << "\n";
            }
        }
        std::cout << "Printed " << data.size() << " matching rows from " << name << "\n";
    }

    // PRINTS COLS WHEN SPECIFIC COL DATA MEETS CONDITION
    void print_where(bool quiet, std::vector<std::string>& cols) {
        for (size_t i = 0; i < cols.size(); ++i) {
            auto it = col_names.find(cols[i]);
            std::string junk;
            if (it == col_names.end()) {
                std::cout << "Error during PRINT: " << cols[i] << " does not name a column in " << name << "\n";
                getline(std::cin, junk);
                return;
            }
        }

        if (!quiet) {
            for (size_t i = 0; i < cols.size(); ++i) {
                std::cout << cols[i] << " ";
            }
            std::cout << "\n";
        }


        std::string col_name, val;
        char condition;
        size_t printed = 0;
        // print from cols in vector cols when val in col_name meets condition
        std::cin >> col_name >> condition >> val;

        // gets col index
        auto col_it = col_names.find(col_name);
        // checks if exists
        if (col_it == col_names.end()) {
            std::cout << "Error during PRINT: " << col_name << " does not name a column in " << name << "\n";
            getline(std::cin, col_name);
            return;
        }

        size_t comp_col_idx = col_it->second;
        TableEntry comp_val = create_entry(val, col_types[comp_col_idx]);

        if (bst_index.size() > 0
            && indexed_col == comp_col_idx) {
            auto begin = bst_index.begin();
            auto end = bst_index.end();
            switch (condition) {
            case '<':
            {
                // change back to lower bound
                end = bst_index.lower_bound(comp_val);
                for (auto it = begin; it != end; ++it) {
                    for (size_t i = 0; i < it->second.size(); ++i) {
                        if (!quiet) {
                            for (size_t j = 0; j < cols.size(); ++j) {
                                std::cout << data[it->second[i]][col_names[cols[j]]] << " ";
                            }
                            std::cout << "\n";
                        }

                        ++printed;
                    }
                }
                break;
            }
            case '>':
            {
                begin = bst_index.upper_bound(comp_val);
                for (auto it = begin; it != end; ++it) {
                    for (size_t i = 0; i < it->second.size(); ++i) {
                        if (!quiet) {
                            for (size_t j = 0; j < cols.size(); ++j) {
                                std::cout << data[it->second[i]][col_names[cols[j]]] << " ";
                            }
                            std::cout << "\n";
                        }

                        ++printed;
                    }
                }
                break;
            }
            case '=':
            {
                auto it = bst_index.find(comp_val);
                if (it != bst_index.end()) {
                    for (size_t i = 0; i < it->second.size(); ++i) {
                        if (!quiet) {
                            for (size_t j = 0; j < cols.size(); ++j) {
                                std::cout << data[it->second[i]][col_names[cols[j]]] << " ";
                            }
                            std::cout << "\n";
                        }

                        ++printed;

                    }
                }

                break;
            }

            }

        }
        else if (hash_index.size() > 0
            && indexed_col == comp_col_idx
            && condition == '=') {
            auto val_index = hash_index.find(comp_val);

            // if no value exists in hash, break
            if (val_index != hash_index.end()) {
                std::vector<size_t> row_index = val_index->second;
                printed = row_index.size();
                // loop through and print
                if (!quiet) {
                    for (size_t i = 0; i < row_index.size(); ++i) {
                        for (size_t j = 0; j < cols.size(); ++j) {
                            std::cout << data[row_index[i]][col_names[cols[j]]] << " ";
                        }
                        std::cout << "\n";
                    }
                }
            }

        }
        else {
            std::vector<size_t> rows;
            for (size_t i = 0; i < data.size(); ++i) {
                switch (condition) {
                case '<':
                {
                    Less l(comp_val, comp_col_idx);

                    if (l(data[i])) {
                        rows.push_back(i);
                    }

                    break;
                }
                case '>':
                {
                    Greater g(comp_val, comp_col_idx);
                    if (g(data[i])) {
                        rows.push_back(i);
                    }

                    break;
                }
                case '=':
                {
                    Equal e(comp_val, comp_col_idx);

                    if (e(data[i])) {
                        rows.push_back(i);
                    }


                    break;
                }
                }


            }
            printed = rows.size();
            if (!quiet) {
                for (size_t i = 0; i < rows.size(); ++i) {
                    for (size_t j = 0; j < cols.size(); ++j) {
                        std::cout << data[rows[i]][col_names[cols[j]]] << " ";
                    }
                    std::cout << "\n";

                }
            }
        }
        //std::cout << "Printed " << printed << " matching rows from " << t.name << "\n";
        std::cout << "Printed " << printed << " matching rows from " << name << "\n";
    }


    // GENERATES HASH INDEX
    void generate_hash_index(std::string column) {
        std::string junk;
        if (!(bst_index.empty())) {
            bst_index.clear();
        }

        auto col_it = col_names.find(column);

        // Check if col exists
        if (col_it == col_names.end()) {
            std::cout << "Error during GENERATE: " << column << " does not name a column in " << name << "\n";
            getline(std::cin, junk);
            return;
        }
        size_t col_index = col_it->second;
        indexed_col = col_index;
        indexed_name = column;
        hash_index.clear();
        for (size_t i = 0; i < data.size(); ++i) {
            hash_index[data[i][indexed_col]].push_back(i);
        }

    }
    // GENERATES BST INDEX
    void generate_bst_index(std::string& column) {
        std::string junk;
        if (!(hash_index.empty())) {
            hash_index.clear();
        }

        auto col_it = col_names.find(column);

        // Check if col exists
        if (col_it == col_names.end()) {
            std::cout << "Error during GENERATE: " << column << " does not name a column in " << name << "\n";
            getline(std::cin, junk);
            return;
        }
        size_t col_index = col_it->second;
        indexed_col = col_index;
        indexed_name = column;
        bst_index.clear();
        for (size_t i = 0; i < data.size(); ++i) {
            bst_index[data[i][indexed_col]].push_back(i);
        }

        //std::cout << "Created bst index for table " << name << " on column " << column << "\n";

    }


    void regenerate_hash_index() {
        std::string junk;
        if (!(bst_index.empty())) {
            bst_index.clear();
        }
        hash_index.clear();
        for (size_t i = 0; i < data.size(); ++i) {
            hash_index[data[i][indexed_col]].push_back(i);
        }
        //std::cout << "Created hash index for table " << name << " on column " << indexed_name << "\n";
    }
    // REGENERATES BST INDEX
    void regenerate_bst_index() {

        bst_index.clear();
        for (size_t i = 0; i < data.size(); ++i) {
            bst_index[data[i][indexed_col]].push_back(i);
        }

        //std::cout << "Created bst index for table " << name << " on column " << indexed_name << "\n";

    }
    // JOINS TWO TABLES
    void join(Table& t2, bool quiet) {
        std::string col_first;
        std::string col_second;
        std::string junk;
        size_t n;
        size_t rows = 0;

        std::cin >> junk >> col_first >> junk >> col_second >> junk >> junk >> n;



        auto col_first_it = col_names.find(col_first);
        auto col_second_it = t2.col_names.find(col_second);

        if (col_first_it == col_names.end()) {
            std::cout << "Error during JOIN: " << col_first << " does not name a column in " << name << "\n";
            getline(std::cin, junk);
            return;
        }
        else if (col_second_it == t2.col_names.end()) {
            std::cout << "Error during JOIN: " << col_second << " does not name a column in " << t2.name << "\n";
            getline(std::cin, junk);
            return;
        }

        // Gets column indices
        size_t first_col_idx = col_first_it->second;
        size_t second_col_idx = col_second_it->second;

        // Takes in print columns
        std::vector<std::string> to_print;
        std::vector<size_t> tables;
        for (size_t i = 0; i < n; ++i) {
            std::string col_name;
            size_t table;

            std::cin >> col_name >> table;

            // stores table that contains column to print from later
            tables.push_back(table);
            // checks existence
            if (table == 1) {
                auto print_it1 = col_names.find(col_name);

                if (print_it1 == col_names.end()) {
                    std::cout << "Error during JOIN: " << col_name << " does not name a column in " << name << "\n";
                    getline(std::cin, col_name);
                    return;
                }


            }
            else {
                auto print_it2 = t2.col_names.find(col_name);
                if (print_it2 == t2.col_names.end()) {
                    std::cout << "Error during JOIN: " << col_name << " does not name a column in " << t2.name << "\n";
                    getline(std::cin, col_name);
                    return;
                }


            }

            //push into vector of print indices
            to_print.push_back(col_name);



        }
        if (!quiet) {
            for (size_t i = 0; i < to_print.size(); ++i) {
                if (tables[i] == 1) {
                    std::cout << to_print[i] << " ";
                }
                else {
                    std::cout << to_print[i] << " ";
                }
            }
            std::cout << "\n";
        }

        if (!(t2.bst_index.empty()) && (t2.indexed_col == second_col_idx)) {

        }
        if (!(t2.hash_index.empty()) && (t2.indexed_col == second_col_idx)) {
            // use generated hash

            for (size_t i = 0; i < data.size(); ++i) {
                auto find = t2.hash_index.find(data[i][first_col_idx]);
                if (find != t2.hash_index.end()) {

                    // match -> print out in format:
                    // <value1rowA> <value2rowA> ... <valueNrowA>
                    for (size_t j = 0; j < find->second.size(); ++j) {
                        if (!quiet) {
                            for (size_t k = 0; k < tables.size(); ++k) {
                                if (tables[k] == 1) {

                                    std::cout << data[i][col_names.find(to_print[k])->second] << " ";
                                }
                                else {
                                    std::cout << t2.data[find->second[j]][t2.col_names.find(to_print[k])->second] << " ";
                                }
                            }
                            std::cout << "\n";
                        }

                        ++rows;
                    }
                }
            }
        }
        else {
            t2.generate_hash_index(col_second);
            for (size_t i = 0; i < data.size(); ++i) {
                auto find = t2.hash_index.find(data[i][first_col_idx]);
                if (find != t2.hash_index.end()) {

                    // match -> print out in format:
                    // <value1rowA> <value2rowA> ... <valueNrowA>
                    for (size_t j = 0; j < find->second.size(); ++j) {
                        if (!quiet) {
                            for (size_t k = 0; k < tables.size(); ++k) {
                                if (tables[k] == 1) {

                                    std::cout << data[i][col_names.find(to_print[k])->second] << " ";
                                }
                                else {
                                    std::cout << t2.data[find->second[j]][t2.col_names.find(to_print[k])->second] << " ";
                                }
                            }
                            std::cout << "\n";
                        }

                        ++rows;
                    }
                }

            }
            t2.hash_index.clear();


        }
        // Print out how many rows were matched
        std::cout << "Printed " << rows << " rows from joining " << name << " to " << t2.name << "\n";
    }
};

// CLASS FOR EVERYTHING RAN IN MAIN
class Silly {
private:
    bool quiet;
    std::unordered_map<std::string, Table> silly_map;



public:
    // CONSTRUCTOR

    Silly(bool q_in)
        : quiet(q_in) {}


    void read() {

        // string containing data read from file
        std::string cmd;

        // string to store comments
        std::string junk;

        // Loop to read in data until quit command
        while (std::cin >> cmd) {
            std::cout << "% ";
            char c = cmd[0];
            // Gets rid of comments
            switch (c) {
            case '#':
            {
                getline(std::cin, junk);
                break;
            }
            case 'C':
            {
                std::string name;
                uint32_t num_cols;
                std::cin >> name >> num_cols;

                // Checks to make sure the name of table is not already in there
                if (silly_map.find(name) != silly_map.end()) {
                    std::cout << "Error during CREATE: Cannot create already existing table " << name << "\n";
                    getline(std::cin, name);
                    continue;
                }
                Table t(name, num_cols);
                t.update_types_names(num_cols);
                std::cout << "created\n";

                silly_map.insert({ name, t });
                break;
            }
            case 'I':
            {
                //check if table exists
                std::string name;
                std::string junk;
                std::cin >> junk >> name;
                auto it = silly_map.find(name);

                if (it == silly_map.end()) {
                    std::cout << "Error during INSERT: " << name << " does not name a table in the database\n";
                    getline(std::cin, name);
                    continue;
                }

                Table& t = it->second;
                t.insert();
                break;
            }
            case 'D':
            {
                std::string table_name, junk;
                std::cin >> junk >> table_name;
                // Get table iterator
                auto it = silly_map.find(table_name);
                if (it == silly_map.end()) {
                    std::cout << "Error during DELETE: " << table_name << " does not name a table in the database\n";
                    getline(std::cin, junk);
                    continue;
                }
                Table& t = it->second;

                t.table_delete();
                break;
            }
            case 'P':
            {
                std::string junk;
                std::string table_name;
                std::string col_name;
                //size_t num_cols;
                std::cin >> junk >> table_name;

                // Finds table and checks if its valid
                auto it = silly_map.find(table_name);
                if (it == silly_map.end()) {
                    std::cout << "Error during PRINT: " << table_name << " does not name a table in the database\n";
                    getline(std::cin, junk);
                    continue;
                }
                Table& t = it->second;

                std::string col_print;
                size_t num_cols;
                std::cin >> num_cols;
                // loop through then add to print vector
                std::vector<std::string> cols;
                for (size_t i = 0; i < num_cols; ++i) {
                    std::cin >> col_print;
                    cols.push_back(col_print);
                }

                std::string print_type;
                std::cin >> print_type;
                char ptype_char = print_type[0];
                if (ptype_char == 'W') {
                    t.print_where(quiet, cols);


                }
                else {
                    t.print_all(quiet, cols);
                }
                break;
            }
            case 'R':
            {
                std::string table_name;
                std::cin >> table_name;
                auto it = silly_map.find(table_name);
                if (it == silly_map.end()) {
                    std::cout << "Error during REMOVE: " << table_name << " does not name a table in the database\n";
                    getline(std::cin, table_name);
                    continue;
                }
                silly_map.erase(it);

                std::cout << "Table " << table_name << " deleted\n";
                break;
            }
            case 'G':
            {
                std::string junk;
                std::string table_name;
                std::string type;
                std::string column;

                std::cin >> junk >> table_name >> type >> junk >> junk >> column;
                char tchar = type[0];
                auto it = silly_map.find(table_name);
                if (it == silly_map.end()) {
                    std::cout << "Error during GENERATE: " << table_name << " does not name a table in the database\n";
                    getline(std::cin, junk);
                    continue;
                }
                Table& t = it->second;

                if (tchar == 'h') {
                    t.generate_hash_index(column);

                    std::cout << "Created hash index for table " << t.get_name() << " on column " << column << "\n";
                }
                else {
                    t.generate_bst_index(column);
                    std::cout << "Created bst index for table " << t.get_name() << " on column " << column << "\n";
                }






                break;
            }
            case 'J':
            {
                std::string table1_name, table2_name;
                std::cin >> table1_name >> junk >> table2_name;
                auto it = silly_map.find(table1_name);
                auto it2 = silly_map.find(table2_name);

                if (it == silly_map.end()) {
                    std::cout << "Error during JOIN: " << table1_name << " does not name a table in the database\n";
                    getline(std::cin, junk);
                    continue;
                }
                else if (it2 == silly_map.end()) {
                    std::cout << "Error during JOIN: " << table2_name << " does not name a table in the database\n";
                    getline(std::cin, junk);
                    continue;
                }

                Table& t1 = it->second;
                Table& t2 = it2->second;
                t1.join(t2, quiet);
                break;
            }
            case 'Q':
            {
                break;
            }
            default:
            {
                std::string junk;
                std::cout << "Error: unrecognized command\n";
                getline(std::cin, junk);
                break;
            }

            }
        }
        std::cout << "Thanks for being silly!\n";
    };

    // READS IN DATA FROM FILE AND RUNS APPROPRIATE FUNCTIONS
    // CREATES A NEW TABLE UNDER FORMAT:
    // "CREATE <tablename> <N> <coltype1> <coltype2> ... <coltypeN> <colname1> <colname2> ... <colnameN>"
    // PRINTS "New table <tablename> with column(s) <colname1> <colname2> ... <colnameN> created"
    void create() {

    }

};

// READS COMMAND LINE OPTIONS
Silly getMode(int argc, char* argv[]) {
    int option_index = 0, option = 0; // nececcary for getopt_long()
    opterr = false; // "", doesnt display getopt error messages about options

    //bool h = false;
    bool q = false;

    struct option longOpts[] = { { "help",     no_argument,             nullptr, 'h' },
                                    { "quiet",       no_argument,             nullptr, 'q' },
                                    { nullptr, 0, nullptr, '\0' } };

    while ((option = getopt_long(argc, argv, "hq", longOpts, &option_index)) != -1) {
        switch (option) {
            //case 'h': { h = true; break; }
        case 'q': { q = true; break; }
        default: {
            std::cout << "'" << ((char)option) << "'$";
            std::cerr << "Unknown Option\n"; exit(1);
        }
        }
    }
    return Silly(q);
}

int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    std::cin >> std::boolalpha;  // add these two lines
    std::cout << std::boolalpha; // add these two lines
    Silly s = getMode(argc, argv);

    s.read();
}




