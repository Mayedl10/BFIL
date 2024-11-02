#include "common.hpp"
#include "compiler.hpp"

/*
memory layout abbreviations: (all of them may be combined)
i   ...     input
o   ...     output
r   ...     "RAM" (just memory a piece of code needs to work)


[-<->] means [0] -= [1]; [1] = 0. memory layout: io i
therefore: this gives the difference of two values
    examples:
        ++>+++[-<->] sets cell [0] to 255 because [0] is smaller than [1]

[->+>+<<] means [1] = [0]; [2] = [0]; [0] = 0. memory layout: i o o
therefore: copies a value to the two cells next to it and deletes the original value

[->+>+<<]>>[-<<+>>]
therefore: like the previous one, but io o r

>+<[[-]>-<]>[<+>-]< invert values. memory layout: [io r]


compare two values: layout: o1 o2 o3 i1 i2
>>>    move to i1
++    put value of i1
>      move to i2
+++    put value of i2
<<<<   move to r1
+[>>>>[-<]<<]<[->>+<]>>[<<<->+>>[-]]>[-] magic

if i1 == i2: o1->1
if i1 > i2: o2->1
if i2 < i2: o3->1

pointer will always end up on i2 at the end

*/
#ifdef COMPILER_DEBUG
// unused
int debugCTR = 0;
void debug_print() {
    std::cout << "[Debug]:" << debugCTR << std::endl;
    debugCTR++;
}
#endif

void Compiler::define_globals(bool displayWarnings) {
    this->errorCount = 0;
    this->ptrPosPtr = nullptr;
    this->warnCount = 0;
    this->displayWarnings = displayWarnings;
    this->insideSubroutine = false;
    this->subroutineTokens = {};

    this->instructionMap = { // DOES NOT CONTAIN instr_OP_EQ

        {RW.RW_add,         &Compiler::instr_add},
        {RW.RW_alias,       &Compiler::instr_alias},
        {RW.RW_aout,        &Compiler::instr_aout},
        {RW.RW_call,        &Compiler::instr_call},
        {RW.RW_compare,     &Compiler::instr_compare},
        {RW.RW_copy,        &Compiler::instr_copy},
        {RW.RW_cout,        &Compiler::instr_cout},
        {RW.RW_decrement,   &Compiler::instr_decrement},
        {RW.RW_def,         &Compiler::instr_def},
        {RW.RW_empty,       &Compiler::instr_empty},
        {RW.RW_endIf,       &Compiler::instr_endIf},
        {RW.RW_endLoop,     &Compiler::instr_endLoop},
        {RW.RW_include,     &Compiler::instr_include},
        {RW.RW_increment,   &Compiler::instr_increment},
        {RW.RW_if,          &Compiler::instr_if},
        {RW.RW_inline,      &Compiler::instr_inline},
        {RW.RW_load,        &Compiler::instr_load},
        {RW.RW_loads,       &Compiler::instr_loads},
        {RW.RW_logic,       &Compiler::instr_logic},
        {RW.RW_memsize,     &Compiler::instr_memsize},
        {RW.RW_read,        &Compiler::instr_read},
        {RW.RW_reserve,     &Compiler::instr_reserve},
        {RW.RW_return,      &Compiler::instr_return},
        {RW.RW_sub,         &Compiler::instr_sub},
        {RW.RW_var,         &Compiler::instr_var},
        {RW.RW_vout,        &Compiler::instr_vout},
        {RW.RW_wnz,         &Compiler::instr_wnz}

    };

    this->reserved = {{0, 0}};
    

}

void Compiler::raise_compiler_error(int errorID, std::string message, std::string errorContext) {
    if (errorCount == 0) {
        std::cout << "[Compiler error] ";
        std::cout << "Error: " << message << std::endl;
        std::cout << "Error code: " << errorID << std::endl;

        if (errorContext.size() > 0){

            std::cout << " Error context: " << errorContext << std::endl;
        }

        std::cout << std::endl;

    } else if (errorCount == 1) {

        std::cout << "[Further errors will be supressed]\n" << std::endl;
    }

    errorCount++;
}

void Compiler::raise_compiler_warning(int warningID, std::string message, std::string warningContext) {

    if (displayWarnings) {

        if (warnCount == 0) {

            std::cout << "[Compiler Warning] ";
            std::cout << "Warning: " << message << std::endl;
            std::cout << "Warning code: " << warningID << std::endl;

            if (warningContext.size() > 0) {

                std::cout << "Warning context: " << warningContext << std::endl;
            }

            std::cout << std::endl;

        } else if (warnCount == 1) {

            std::cout << "[Further warnings will be supressed]\n" << std::endl;
        }
    
    }

    warnCount++;

}

bool Compiler::is_valid_hexadecimal(std::string& str) {

    if (str.size() < 2 || str.substr(0, 2) != "0x") {
        return false;
    }

    for (size_t i = 2; i < str.size(); i++) {

        if (!(std::isxdigit(str[i]))) {

            return false;
        }
    }

    return true;
}

bool Compiler::is_valid_decimal(std::string str) {
    for (char c: str) {
        if ((c < '0') || (c > '9')) {
            return false;
        }
    }
    return true;
}

std::string Compiler::add_n_chars(int n, char c) {
    std::string ret = "";

    for (int i = 0; i < n; i++) {
        ret += c;
    }

    return ret;
}

std::string Compiler::multiply_string(std::string targetString, int n) {
    std::string ret = "";

    for (int i = 0; i < n; i++) {
        ret += targetString;
    }

    return ret;
}



void Compiler::get_cur_tok() {
    *curTokPtr = Tokens[tPtr];

}

bool Compiler::is_reserved(int address, std::vector<std::array<int, 2>> reserved_areas) {

    for (auto y: reserved_areas) {

        if ((address >= y[0]) && (address <= y[1])) {

            return true;
        }
    }

    return false;
}

bool Compiler::is_variable(int address) {

    for (std::pair<const std::string, int>& y: variableAddressLookup) {
        if (y.second == address) {
            return true;
        }
    }
    return false;
}

bool Compiler::reserved_overlap(std::vector<std::array<int, 2>> reserved_segments_vector, std::array<int, 2> addresses) {   
    std::vector<int> addressRange = range(addresses[0], addresses[1]);

    for (auto a: addressRange) {
        if (is_reserved(a, reserved_segments_vector)) {
            return true;
        }
    }

    return false;
}

std::array<int, 2> Compiler::find_reserved(std::vector<std::array<int, 2>> reserved_segments_vector, int requiredSize) {

    std::vector<std::array<int, 2>> candidates;
    std::vector<std::array<int, 3>> candidates_and_distances;
    std::array<int, 3> ret;
    
    // never return [0] bc that's always ?0
    for (int ctr = 1; ctr < reserved_segments_vector.size(); ctr++) {

        if (reserved_segments_vector[ctr][1] - reserved_segments_vector[ctr][0] >= requiredSize) {

            candidates.push_back(reserved_segments_vector[ctr]);
        }
    }

    if (candidates.empty()) {

        raise_compiler_error(CompilerErrors::insufficientReservedMemoryError, "Insufficient memory reserved for this operation. Either use 'reserve' with a big enough range of addresses or avoid using the operation that caused this error.");
        return {-1,-1};
    }
    
    for (auto c: candidates) {

        candidates_and_distances.push_back({c[0], c[1], abs(c[0] - *ptrPosPtr)});
    }

    ret = candidates_and_distances[0];

    for (auto c: candidates_and_distances) {

        if (c[2] < ret[2]) {
            ret = c;
        }
    }

    return {ret[0], ret[1]};

}




int Compiler::hex_to_int(std::string hexString) {

    if (!is_valid_hexadecimal(hexString)) {

        raise_compiler_error(CompilerErrors::invalidHexadecimalValue, "Invalid hexadecimal value.");
        return -1;
    }

    if (hexString.substr(0, 2) == "0x") {
        try {
            return std::stoi(hexString.substr(2), nullptr, 16);

        } catch (const std::invalid_argument& e) {
            raise_compiler_error(CompilerErrors::invalidHexadecimalValue, "Invalid hexadecimal value.");
            return -1;
        }

    } else {
        raise_compiler_error(CompilerErrors::invalidHexadecimalValue, "Invalid hexadecimal value.");
        return -1;
    }
}

std::string Compiler::slice_string(const std::string& targetString, int idx1, int idx2) {

    int length = targetString.length();
    // Handle negative indices
    if (idx1 < 0) {
        idx1 = length + idx1;
        if (idx1 < 0) idx1 = 0;
    }
    if (idx2 < 0) {
        idx2 = length + idx2;
        if (idx2 < 0) idx2 = 0;
    }

    if (idx1 >= length || idx2 < 0 || idx1 > idx2) {
        return "";
    }

    int sliceLength = idx2 - idx1 + 1;
    return targetString.substr(idx1, sliceLength);
}

bool Compiler::is_valid_address_str(std::string addressString) {

    // i am not combining these into one if statement because i don't wanna try accessing std[1] on a size 1 string or smth

    if ((addressString.size() <= 1)) {
        return false;
    }
    return ((addressString[0] == RW.RW_prefix_ADDR[0]) && (is_valid_decimal(addressString.substr(1))));
}

int Compiler::address_string_to_int(std::string addressString, std::string prefix) {

    try
    {
        return std::stoi(slice_string(addressString, prefix.length(), -1));
    }
    catch(const std::exception& e)
    {
        raise_compiler_error(CompilerErrors::invalidMemoryAddress, "Invalid memory address.", addressString);
        return -1;
    }
    
}

int Compiler::generate_variable_address() {

    /*
    
    an address is valid if:
        - not reserved
        - not already used by a variable    
    */

    // address ?0 should not be used!
    for (int i = 1; i < memsize; i++) {
        if ((!is_reserved(i, reserved)) && (!is_variable(i))) {
            return i;
        }
    }

    // if this point is reached, somehow every address is already in use or smth --> error

    raise_compiler_error(CompilerErrors::outOfMemory, "Cannot declare variable; out of memory.", "");
    
    return -1;
}

std::string Compiler::construct_address_str(int addr) {
    return RW.RW_prefix_ADDR + std::to_string(addr);
}

bool Compiler::var_exists(std::string var) {
    return !(variableAddressLookup.find(var) == variableAddressLookup.end());
}

bool Compiler::both_variables_and_addresses_used() {

    manualAddressingUsed = false;
    variablesUsed = false;

    for (int i = 0; i < (int)(Tokens.size()); i++) {

        if (
            (Tokens[i][0] == RW.RW_prefix_ADDR[0]) &&
            ((Tokens[i-1] != RW.RW_reserve) && (Tokens[i-3] != RW.RW_reserve))
            ) {
            manualAddressingUsed = true;
        
        } else if (Tokens[i] == RW.RW_var) {
            variablesUsed = true;
        }
    }

    return (manualAddressingUsed && variablesUsed);

};

void Compiler::scan_code_var(int &tIdx, std::vector<int> &varDeclatationIdxs) {

    // check if next token has already been added as a variable, error if yes

    for (std::pair<const std::string, int> y: variableAddressLookup) {
        if (y.first == Tokens[tIdx+1]) {
            raise_compiler_error(CompilerErrors::multipleVariableDefinitions, "Variable " + Tokens[tIdx+1] + " has already been defined.", "... var " + Tokens[tIdx+1] + " ...");
        }
    }

    // check if next token is a keyword, error if yes
    if (vector_contains_string(RW.RW_ALL, Tokens[tIdx+1])) {
        raise_compiler_error(CompilerErrors::invalidVariableName, "Cannot create a variable with the name \"" + Tokens[tIdx+1] + "\".", "... var " + Tokens[tIdx+1] + " ...");
    }

    // generate_variable_address
    int variableAddress = generate_variable_address();

    std::pair<std::string, int> initialValueEntry = {Tokens[tIdx+1], 0};

    // is t+1 "="? keep default value at 0 if no, otherwise: check if t+2 is a number, set default value to t+2 if true, error if not
    if ((Tokens[tIdx+1] == RW.RW_operator_EQ) && (is_valid_decimal(Tokens[tIdx+2]))) {
        initialValueEntry.second = std::stoi(Tokens[tIdx+2]);
    }

    // add variable to var index
    variableAddressLookup.insert({Tokens[tIdx+1], variableAddress});
    varDeclatationIdxs.push_back(tIdx);

    // in scan_code, for every following token: if token != keyword, but is variable, replace with address string
    

}

void Compiler::scan_code() {
/*
this function scans for various things and manages variable creation

*/


    // check if token like ?*
    // check if token is RW_var
    /*
    variables
    - scan entire code
    - on var: new variable entry
        - if name in keywords: error
    - on var name: replace with address

    syntax:
    var [name]
    var [name] = [value]
    ... [name] ...
    */

    std::string t;
    int memsizeCount = 0;
    std::vector<int> varDeclatationIdxs = {};

    for (int i = 0; i < static_cast<int>(Tokens.size()); i++) {
        t = Tokens[i];

        // determine initial memsize and raise warning if memsize changes
        if (t == RW.RW_memsize) {
            memsize = hex_to_int(Tokens[i+1]);
            memsizeCount++;
        }

        if (t == RW.RW_var) {
            scan_code_var(i, varDeclatationIdxs);

        }


        // if token != keyword, but is variable, replace with address string

/*
    conditions for replacing:
    - is declared var
    - t-1 != "var"
    - (t-3 != "var") && (t-1 == "=")
    - isn't keyword

*/

        if (
            // only true if t has been declared as a variable
            (var_exists(t)) &&
            // check if t is not a keyword
            (!vector_contains_string(RW.RW_ALL, t)) &&
            // check if varDeclatationIdxs contains t-1 or t-3
            !(
                vector_contains_int(varDeclatationIdxs, i-1) ||
                (vector_contains_int(varDeclatationIdxs, i-3) && (Tokens[i-1] == RW.RW_operator_EQ))
            )
            
        ) {
            Tokens[i] = construct_address_str(variableAddressLookup[t]);
        } 

    }

    if (memsizeCount > 1) {
        raise_compiler_warning(CompilerWarnings::multipleMemsizeDeclarations, "Multiple \"memsize\" statements used. This will likely lead to errors.", "");
    }
}

bool Compiler::vector_contains_string(std::vector<std::string> vec, std::string str) {
    for (auto y: vec) {
        if (y == str) {
            return true;
        }
    }
    return false;
}

bool Compiler::vector_contains_int(std::vector<int> vec, int i) {
    for (auto y: vec) {
        if (y == i) {
            return true;
        }
    }
    return false;
}

void Compiler::add_linker_directory(std::string path) {
    this->linkerDirectories.push_back(path);
}

std::string Compiler::compile(std::vector<std::string> Tokens_string_vector, bool displayWarnings) {

    define_globals(displayWarnings);

    out = "";
    Tokens = Tokens_string_vector;  // just doing this so I can use a member variable instead of a local parameter
    tPtr = 0;                       // token "pointer"
    int tPtrLimit = Tokens.size();  // highest possible value for the token pointer
    curTokPtr = &curTok;
    ptrPosPtr = &ptrPosition;

    if (both_variables_and_addresses_used()) {
        raise_compiler_warning(CompilerWarnings::variablesAndDirectAddressing, "Using direct addresses AND variables might lead to unwanted behaviour!");
    }

    scan_code();

    generate_subroutines();
    include_files();
    insert_subroutine_tokens();

    while (tPtr < tPtrLimit) {

        tempInt = 0;
        tempStr = "";
        tempIntVect.clear();
        tempReservedArea = {};

        get_cur_tok(); // use *curTokPtr to access the value

        if (curTokPtr == nullptr) {
            break;

        } else if (vector_contains_string(RW.RW_ALL, curTok)) {
            (this->*instructionMap[curTok])();

        } else {
            
            if (is_valid_address_str(curTok) && (Tokens[tPtr+1] == RW.RW_operator_EQ)) {
                tPtr++;
                get_cur_tok();
                this->instr_op_EQ();

            } else if (curTok != RW.RW_PADDING_TOKEN) {                
                raise_compiler_error(CompilerErrors::unexpectedToken, "Unexpected Token: Token nr. " + std::to_string(tPtr) + ".\nNote, that variables have been replaced with address strings (?n).", "... " + Tokens[tPtr] + " ...");
            }
        }

        tPtr++;
    }

    if (!loopingAddressesStack.empty()) {
        raise_compiler_error(CompilerErrors::unmatchedEndLoop, "Unmatched 'whileNotZero' statement. This means that there are more 'whileNotZero' statements than 'endLoop' statements.");
    }
    

#ifdef COMPILER_DEBUG
    std::cout << "\n\n--------Debug--------\nMemsize: " << memsize << "\nReserved memory areas (inclusive):"  << std::endl;
    for (auto i: reserved) {
        std::cout << "----{" << i[0] << " -> " << i[1] << "}" << std::endl; 
    }
#endif

    if (errorCount) {
        std::cout << "Exited compilation with a total of "+std::to_string(errorCount)+" compilation errors.\nAt max one of them has been displayed." << std::endl;
    }
    
    curTokPtr = nullptr;
    ptrPosPtr = nullptr;

    if (errorCount == 0){
        return out;
    } else {
        return "\n";
    }

}