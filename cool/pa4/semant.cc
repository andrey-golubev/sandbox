#include "semant.h"
#include "utilities.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <utility>

extern int semant_debug;
extern char* curr_filename;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol arg, arg2, Bool, concat, cool_abort, copy, Int, in_int, in_string, IO, length, Main,
    main_meth, No_class, No_type, Object, out_int, out_string, prim_slot, self, SELF_TYPE, Str,
    str_field, substr, type_name, val;

using MTable = SymbolTable<Symbol, method_class>;
using ATable = SymbolTable<Symbol, attr_class>;

static std::map<Symbol, MTable> method_tables;  // table per class entry
static std::map<Symbol, ATable> attrib_tables;  // table per class entry

//
// Initializing the predefined symbols.
//
static void initialize_constants(void) {
    arg = idtable.add_string("arg");
    arg2 = idtable.add_string("arg2");
    Bool = idtable.add_string("Bool");
    concat = idtable.add_string("concat");
    cool_abort = idtable.add_string("abort");
    copy = idtable.add_string("copy");
    Int = idtable.add_string("Int");
    in_int = idtable.add_string("in_int");
    in_string = idtable.add_string("in_string");
    IO = idtable.add_string("IO");
    length = idtable.add_string("length");
    Main = idtable.add_string("Main");
    main_meth = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any
    //   user-defined class.
    No_class = idtable.add_string("_no_class");
    No_type = idtable.add_string("_no_type");
    Object = idtable.add_string("Object");
    out_int = idtable.add_string("out_int");
    out_string = idtable.add_string("out_string");
    prim_slot = idtable.add_string("_prim_slot");
    self = idtable.add_string("self");
    SELF_TYPE = idtable.add_string("SELF_TYPE");
    Str = idtable.add_string("String");
    str_field = idtable.add_string("_str_field");
    substr = idtable.add_string("substr");
    type_name = idtable.add_string("type_name");
    val = idtable.add_string("_val");
}

static std::ostream& log() {
    if (!semant_debug) {
        static std::fstream devnull("/dev/null");
        return devnull;
    }
    return std::cout;
}

ClassTable::ClassTable(Classes classes) : semant_errors(0), error_stream(cerr) {
    log() << "----------------\n"
          << "Checking inheritance graph...\n\n";

    install_basic_classes();

    // iterate over classes to populate class table
    for (const auto& cls : to_range(classes)) {
        const auto name = cls->get_name();

        const std::array<Symbol, 6> basic_classes = {
            SELF_TYPE, Object, IO, Int, Bool, Str,
        };
        const auto it = std::find(basic_classes.begin(), basic_classes.end(), name);
        if (it != basic_classes.end()) {
            semant_error(cls) << "Basic class is redefined: " << *it << std::endl;
        }

        if (table.end() != table.find(name)) {
            semant_error(cls) << "this class is redefined" << std::endl;
            return;  // TODO: how to tell recoverable error from non-recoverable?
        }

        table.insert({name, cls});
    }

    // ensure there's Main class
    if (table.end() == table.find(Main)) {
        semant_error() << "Main class is not defined" << std::endl;
    }

    // check inheritance (for cycles and other errors)
    for (Class_ curr_cls : to_range(classes)) {
        Class_ this_class = curr_cls;  // copy
        Symbol parent = curr_cls->get_parent();

        log() << curr_cls->get_name() << " ";

        while (parent != Object) {  // Object is the root base class of everything
            log() << " <- " << parent;

            if (parent == this_class->get_name()) {
                semant_error(this_class) << "Cycle dependency found in inheritance" << std::endl;
                break;
            }

            if (table.end() == table.find(parent)) {
                semant_error(curr_cls) << "Cannot find parent: " << parent << std::endl;
                break;
            }

            const std::array<Symbol, 4> basic_classes = {SELF_TYPE, Int, Bool, Str};
            const auto it = std::find(basic_classes.begin(), basic_classes.end(), parent);
            if (it != basic_classes.end()) {
                semant_error(curr_cls) << "Inherits basic class: " << *it << std::endl;
                break;
            }

            curr_cls = table.at(parent);
            parent = curr_cls->get_parent();
        }

        if (parent == Object) {
            log() << " <- " << parent;
        }

        log() << "\n";
    }

    log() << "----------------" << std::endl;
}

void ClassTable::install_basic_classes() {

    // The tree package uses these globals to annotate the classes built below.
    // curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");

    // The following demonstrates how to create dummy parse trees to
    // refer to basic Cool classes.  There's no need for method
    // bodies -- these are already built into the runtime system.

    // IMPORTANT: The results of the following expressions are
    // stored in local variables.  You will want to do something
    // with those variables at the end of this method to make this
    // code meaningful.

    //
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.

    Class_ Object_class = class_(
        Object, No_class,
        append_Features(
            append_Features(single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
                            single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
            single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
        filename);

    //
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    Class_ IO_class = class_(
        IO, Object,
        append_Features(
            append_Features(
                append_Features(single_Features(method(out_string, single_Formals(formal(arg, Str)),
                                                       SELF_TYPE, no_expr())),
                                single_Features(method(out_int, single_Formals(formal(arg, Int)),
                                                       SELF_TYPE, no_expr()))),
                single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
            single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
        filename);

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer.
    //
    Class_ Int_class =
        class_(Int, Object, single_Features(attr(val, prim_slot, no_expr())), filename);

    //
    // Bool also has only the "val" slot.
    //
    Class_ Bool_class =
        class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())), filename);

    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //
    Class_ Str_class = class_(
        Str, Object,
        append_Features(
            append_Features(
                append_Features(
                    append_Features(single_Features(attr(val, Int, no_expr())),
                                    single_Features(attr(str_field, prim_slot, no_expr()))),
                    single_Features(method(length, nil_Formals(), Int, no_expr()))),
                single_Features(method(concat, single_Formals(formal(arg, Str)), Str, no_expr()))),
            single_Features(method(
                substr,
                append_Formals(single_Formals(formal(arg, Int)), single_Formals(formal(arg2, Int))),
                Str, no_expr()))),
        filename);

    table.insert({Object, Object_class});
    table.insert({IO, IO_class});
    table.insert({Int, Int_class});
    table.insert({Bool, Bool_class});
    table.insert({Str, Str_class});
}

////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

ostream& ClassTable::semant_error(Class_ c) { return semant_error(c->get_filename(), c); }

ostream& ClassTable::semant_error(Symbol filename, tree_node* t) {
    error_stream << filename << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream& ClassTable::semant_error() {
    semant_errors++;
    return error_stream;
}

// forward declaration
template<typename Tables, typename Filter, typename Cast>
void initialize_tables(ClassTable& classtable, Tables& tables, Filter filter, Cast cast);

void initialize_method_tables(ClassTable& classtable) {
    log() << "----------------\nConstructing method tables...\n\n";
    initialize_tables(
        classtable, method_tables, [](Feature f) { return f->is_method(); },
        [](Feature f) -> method_class* { return dynamic_cast<method_class*>(f); });
    log() << "----------------\n";
}

void initialize_attrib_tables(ClassTable& classtable) {
    log() << "----------------\nConstructing attribute tables...\n\n";
    initialize_tables(
        classtable, attrib_tables, [](Feature f) { return !f->is_method(); },
        [](Feature f) -> attr_class* { return dynamic_cast<attr_class*>(f); });
    log() << "----------------\n";
}

/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */
void program_class::semant() {
    initialize_constants();

    const auto check_errors = [](const ClassTable& ct) {
        if (ct.errors()) {
            cerr << "Compilation halted due to static semantic errors." << endl;
            exit(1);
        }
    };

    ClassTable classtable(classes);
    check_errors(classtable);

    // at this stage, inheritance graph is expected to be correct

    initialize_method_tables(classtable);
    check_errors(classtable);

    initialize_attrib_tables(classtable);
    check_errors(classtable);

    // TODO: remove later?
    if (semant_debug) {
        exit(0);
    }
}

template<typename Table> struct TableScope {
    Table m_table;
    TableScope(Table& t) : m_table(t) { m_table.enterscope(); }
    ~TableScope() { m_table.exitscope(); }
};
using MScope = TableScope<MTable>;
using AScope = TableScope<ATable>;

template<typename Tables, typename Filter, typename Cast>
void initialize_tables(ClassTable& classtable, Tables& tables, Filter filter, Cast cast) {
    for (const auto& p : classtable) {
        Symbol name;
        Class_ cls;
        std::tie(name, cls) = p;
        log() << "Class: " << p.first << std::endl;

        Features features = cls->get_features();
        if (features == nil_Features()) {  // empty class is not an error?
            continue;
        }

        auto& table = tables[name];
        table.enterscope();  // Note: create global scope that exists forever!

        for (const auto& feature : to_range(features)) {
            if (!filter(feature)) {
                continue;
            }
            const auto fname = feature->get_name();
            log() << pad(2) << fname << std::endl;

            // this is unified for any feature - redefinition is considered an error
            if (table.lookup(fname) != nullptr) {
                classtable.semant_error(cls) << "Redefined: " << fname << std::endl;
                continue;
            }

            table.addid(fname, cast(feature));
        }
    }
}
