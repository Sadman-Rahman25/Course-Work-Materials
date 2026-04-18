#include "symbol_info.h"
#include <string>
#include <sstream>
#include <cstdlib>

class scope_table
{
private:
    int bucket_count;
    int unique_id;
    scope_table *parent_scope = NULL;
    vector<list<symbol_info *>> table;

    int hash_function(string name)
    {
        int hash_value = 0;
        // write your hash function here
        if (name.length() == 0) {
            hash_value = int(name[0]);
        }
        else {
            for (int i = 0; i < name.length(); i++)
            {
                hash_value += int(name[i]);
            }           
        }
        hash_value = hash_value % bucket_count;
        return hash_value;   
    }

public:
    scope_table();
    scope_table(int bucket_count, int unique_id, scope_table *parent_scope);
    scope_table *get_parent_scope();
    int get_unique_id();
    symbol_info *lookup_in_scope(symbol_info* symbol);
    bool insert_in_scope(symbol_info* symbol);
    bool delete_from_scope(symbol_info* symbol);
    void print_scope_table(ofstream& outlog);
    ~scope_table();

    // you can add more methods if you need
    symbol_info *lookup_in_parent_scope(symbol_info* symbol);
    symbol_info *lookup_in_only_current_scope(symbol_info* symbol);
};

scope_table::scope_table() {}
scope_table::scope_table(int _bucket_count, int _unique_id, scope_table *_parent_scope) {
    bucket_count = _bucket_count;
    unique_id = _unique_id;
    parent_scope = _parent_scope;
    table.resize(bucket_count);
}

scope_table *scope_table::get_parent_scope() {
    return parent_scope;
}

int scope_table::get_unique_id() {
    return unique_id;
}

symbol_info *scope_table::lookup_in_scope(symbol_info* symbol) {
    for (int i = 0; i < table.size(); i++) {
        list<symbol_info*> *info = &table[i];
        for (auto it = info->begin(); it != info->end(); it++) {
            if ((*it)->get_name() == symbol->get_name()) {
                return *it;
            }
        }
    }
    return lookup_in_parent_scope(symbol);
}

symbol_info *scope_table::lookup_in_parent_scope(symbol_info* symbol) {
    scope_table *sc = get_parent_scope();
    if (sc == NULL) {
        return NULL;
    }
    return sc->lookup_in_scope(symbol);
}

symbol_info *scope_table::lookup_in_only_current_scope(symbol_info* symbol) {
    int hashed_index = hash_function(symbol->get_name());
    list<symbol_info*> *info = &table[hashed_index];
    for (auto it = info->begin(); it != info->end(); it++) {
        if ((*it)->get_name() == symbol->get_name()) { 
            return *it;
        }
    }
    return NULL;
}

bool scope_table::insert_in_scope(symbol_info *symbol) {
    int hashed_index = hash_function(symbol->get_name());
    list<symbol_info *> *info = &table[hashed_index];
    info->push_back(symbol);
    return 0;
}

bool scope_table::delete_from_scope(symbol_info *symbol) {
    int hashed_index = hash_function(symbol->get_name());
    list<symbol_info *> *info = &table[hashed_index];
    info->pop_back();
}

scope_table::~scope_table() {
    for (int i = 0; i < table.size(); i++) {
        table[i].clear();
    }
}

// complete the methods of scope_table class
void scope_table::print_scope_table(ofstream& outlog)
{
    stringstream ss;
    ss << unique_id;
    outlog << "ScopeTable # " + ss.str() << endl;

    // Iterate through the current scope table and print the symbols and all relevant information
    for (int i = 0; i < table.size(); i++) {
        for (auto it = table[i].begin(); it != table[i].end(); it++) {
            outlog << i << " -->" << endl;
            outlog << "< " << (*it)->get_name() << " : " << (*it)->get_type() << " >\n";
            if ((*it)->get_variable_state()) {
                outlog << "Variable\n";
                outlog << "Type: " << (*it)->get_return_type() << endl;
            } else if ((*it)->get_function_state()) {
                outlog << "Function Definition\n";
                outlog << "Return Type: " << (*it)->get_return_type() << endl;
                outlog << "Number of Parameters: " << (*it)->get_parameter_size() << endl;
                outlog << "Parameter Details: ";
                int numberOfParameters = (*it)->get_parameter_size();
                if (numberOfParameters > 0) {
                    list<symbol_info*> parameter_info = (*it)->get_parameter_info();
                    for (auto it2 = parameter_info.begin(); it2 != parameter_info.end(); it2++) {
                        outlog << (*it2)->get_name() << " " << (*it2)->get_return_type();
                        if (numberOfParameters > 1) {
                            numberOfParameters--;
                            outlog << ", ";
                        }
                    }
                }
                outlog << "\n";
            } else if ((*it)->get_array_state()) {
                outlog << "Array\n"; 
                outlog << "Type: " << (*it)->get_return_type() << endl;
                outlog << "Size: " << (*it)->get_array_size() << endl;
            }
            outlog << endl;
        }
    }
    outlog << endl;
}