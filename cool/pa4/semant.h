#ifndef SEMANT_H_
#define SEMANT_H_

#include "cool-tree.h"
#include "list.h"
#include "stringtab.h"
#include "symtab.h"
#include <assert.h>
#include <iostream>
#include <map>

#define TRUE 1
#define FALSE 0

class ClassTable;
typedef ClassTable* ClassTableP;

// This is a structure that may be used to contain the semantic
// information such as the inheritance graph.  You may use it or not as
// you like: it is only here to provide a container for the supplied
// methods.

class ClassTable {
private:
    int semant_errors;
    void install_basic_classes();
    ostream& error_stream;
    std::map<Symbol, Class_> table;

public:
    ClassTable(Classes);
    int errors() const { return semant_errors; }
    ostream& semant_error();
    ostream& semant_error(Class_ c);
    ostream& semant_error(Symbol filename, tree_node* t);

    auto begin() const -> decltype(table)::const_iterator { return table.begin(); }
    auto end() const -> decltype(table)::const_iterator { return table.end(); }

    const Class_ at(Symbol name) const { return table.at(name); }
};

#endif
