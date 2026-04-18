#include "scope_table.h"
#include <iostream>
using namespace std;

class symbol_table
{
private:
    scope_table *current_scope;
    int bucket_count;
    int current_scope_id;

public:
    symbol_table(int bucket_count);
    ~symbol_table();

    void enter_scope(ofstream &outlog);
    void exit_scope(ofstream &outlog);

    bool insert(symbol_info *symbol);
    symbol_info *lookup(symbol_info *symbol);

    void print_current_scope(ofstream &outlog);
    void print_all_scopes(ofstream &outlog);

    // helper for inserting in current scope only
    symbol_info *lookup_in_current_scope(symbol_info *symbol);
};

symbol_table::symbol_table(int _bucket_count)
{
    bucket_count = _bucket_count;
    current_scope_id = 1;
    current_scope = new scope_table(bucket_count, current_scope_id, nullptr);
}

//  Destructor: deletes all scopes
symbol_table::~symbol_table()
{
    while (current_scope != nullptr)
    {
        scope_table *temp = current_scope;
        current_scope = current_scope->get_parent_scope();
        delete temp;
    }
}

// Enter a new nested scope
void symbol_table::enter_scope(ofstream &outlog)
{
    int new_id = current_scope->get_unique_id() + 1;
    scope_table *new_scope = new scope_table(bucket_count, new_id, current_scope);
    current_scope = new_scope;

    outlog << "New ScopeTable with ID " << current_scope->get_unique_id() << " created\n\n";
}

void symbol_table::exit_scope(ofstream &outlog)
{
    if (current_scope == nullptr)
    {
        outlog << "No scope to exit\n\n";
        cout << "No scope to exit....\n";
        return;
    }

    outlog << "ScopeTable with ID " << current_scope->get_unique_id() << " removed\n\n";

    scope_table *temp = current_scope;
    current_scope = current_scope->get_parent_scope();
    delete temp;
}

bool symbol_table::insert(symbol_info *symbol)
{
    symbol_info *returnedResult = lookup_in_current_scope(symbol);
    if (returnedResult == nullptr)
    {
        current_scope->insert_in_scope(symbol);
        return true;
    }
    return false;
}

symbol_info *symbol_table::lookup_in_current_scope(symbol_info *symbol)
{
    return current_scope->lookup_in_only_current_scope(symbol);
}

symbol_info *symbol_table::lookup(symbol_info *symbol)
{
    return current_scope->lookup_in_scope(symbol);
}

void symbol_table::print_current_scope(ofstream &outlog)
{
    current_scope->print_scope_table(outlog);
}

void symbol_table::print_all_scopes(ofstream &outlog)
{
    outlog << "################################" << endl
           << endl;
    scope_table *temp = current_scope;
    while (temp != nullptr)
    {
        temp->print_scope_table(outlog);
        temp = temp->get_parent_scope();
    }
    outlog << "################################" << endl
           << endl;
}
