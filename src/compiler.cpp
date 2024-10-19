#include "common.hpp"
#include "compiler.hpp"
#include "instr.hpp"

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


//////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// GLOBAL VARIABLES //////////////////////////////////////////
// declared using extern in compiler.hpp
// written down here bc that somehow fixes a bunch of linker errors
int* ptrPosPtr;
std::string* curTokPtr;
std::vector<std::string> Tokens;
int tPtr;
std::vector<std::array<int, 2>> reserved;
int errorCount;
int warnCount;
std::string curTok;

ReservedWords RW;
CodeSnippets CS;
BrainFuckOperations BFO;
OperationRequiredMemory ORM;
////////////////////////////////////////// GLOBAL VARIABLES //////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef COMPILER_DEBUG
// unused
int debugCTR = 0;
void debug_print() {
    std::cout << "[Debug]:" << debugCTR << std::endl;
    debugCTR++;
}
#endif

static void define_globals() {
    errorCount = 0;
    ptrPosPtr = nullptr;
    warnCount = 0;
}

void raise_compiler_error(int errorID, std::string message, std::string errorContext) {
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

void raise_compiler_warning(int warningID, std::string message, std::string warningContext) {

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

    warnCount++;

}

bool is_valid_hexadecimal(std::string& str) {

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

std::string add_n_chars(int n, char c) {
    std::string ret = "";

    for (int i = 0; i < n; i++) {
        ret += c;
    }

    return ret;
}

std::string multiply_string(std::string targetString, int n) {
    std::string ret = "";

    for (int i = 0; i < n; i++) {
        ret += targetString;
    }

    return ret;
}



void get_cur_tok() {
    *curTokPtr = Tokens[tPtr];

}

bool is_reserved(int address, std::vector<std::array<int, 2>> reserved_areas) {

    for (auto y: reserved_areas) {

        if ((address >= y[0]) && (address <= y[1])) {

            return true;
        }
    }

    return false;
}

bool reserved_overlap(std::vector<std::array<int, 2>> reserved_segments_vector, std::array<int, 2> addresses) {   
    std::vector<int> addressRange = range(addresses[0], addresses[1]);

    for (auto a: addressRange) {
        if (is_reserved(a, reserved_segments_vector)) {
            return true;
        }
    }

    return false;
}

std::array<int, 2> find_reserved(std::vector<std::array<int, 2>> reserved_segments_vector, int requiredSize) {

    std::vector<std::array<int, 2>> candidates;
    std::vector<std::array<int, 3>> candidates_and_distances;
    std::array<int, 3> ret;
    
    for (int ctr = 0; ctr < reserved_segments_vector.size(); ctr++) {

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




int hex_to_int(std::string hexString) {

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

std::string slice_string(const std::string& targetString, int idx1, int idx2) {

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

int address_string_to_int(std::string addressString, std::string prefix) {

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

std::string compile(std::vector<std::string> Tokens_string_vector) {

    define_globals();

    std::string out = "";

    Tokens = Tokens_string_vector;  // just doing this so I can use a global variable instead of a local parameter

    tPtr = 0;                       // token "pointer"
    int tPtrLimit = Tokens.size();  // highest possible value for the token pointer

    curTokPtr = &curTok;

    int memsize = -1;
    bool initialized = false;

    int tempInt;
    std::string tempStr;
    std::vector<int> tempIntVect;
    std::array<int, 2> tempReservedArea;

    std::stack<int> loopingAddressesStack;
    std::stack<int> branchingAddressesStack;
    std::string logicMode;
    

    int ptrPosition = 0;
    ptrPosPtr = &ptrPosition;

    while (tPtr < tPtrLimit) {
        tempInt = 0;
        tempStr = "";
        tempIntVect.clear();
        tempReservedArea = {};

        get_cur_tok(); // use *curTokPtr to access the value

        if (curTokPtr == nullptr) {
            break;

        } else if (curTok == RW.RW_memsize) {
            instr_memsize(out, tempStr, tempIntVect, tempReservedArea, tPtr, ptrPosition, memsize);

        } else if (curTok == RW.RW_load) {

            instr_load(out, tempStr, tempIntVect, tempReservedArea, tPtr, ptrPosition, tempInt, memsize);

        } else if (curTok == RW.RW_reserve) {

            instr_reserve(out, tempStr, tempIntVect, tempReservedArea, tPtr, ptrPosition, memsize);

        } else if (curTok == RW.RW_add) {

            instr_add(out, tempStr, tempIntVect, tempReservedArea, tPtr, ptrPosition);

        } else if (curTok == RW.RW_sub) {

            instr_sub(out, tempStr, tempIntVect, tempReservedArea, tPtr, ptrPosition);

        } else if (curTok == RW.RW_aout) {

            instr_aout(out, tempStr, tempIntVect, tempReservedArea, tPtr, ptrPosition, tempInt);

        } else if (curTok == RW.RW_vout) {

            instr_vout(out, tempStr, tempIntVect, tempReservedArea, tPtr, ptrPosition, tempInt, memsize);

        } else if (curTok == RW.RW_compare) {

            instr_compare(out, tempStr, tempIntVect, tempReservedArea, tPtr, ptrPosition, tempInt);
        
        } else if (curTok == RW.RW_copy) {

            instr_copy(out, tempStr, tempIntVect, tempReservedArea, tPtr, ptrPosition, tempInt);

        } else if (curTok == RW.RW_wnz) {

            instr_wnz(out, tempStr, tempIntVect, tempReservedArea, tPtr, ptrPosition, loopingAddressesStack);

        } else if (curTok == RW.RW_endLoop) {

            instr_endLoop(out, tempStr, tempIntVect, tempReservedArea, tPtr, ptrPosition, tempInt, loopingAddressesStack);

        } else if (curTok == RW.RW_empty) {

            instr_empty(out, tempStr, tempIntVect, tempReservedArea, tPtr, ptrPosition);

        } else if (curTok == RW.RW_increment) {

            instr_increment(out, tempStr, tempIntVect, tempReservedArea, tPtr, ptrPosition, tempInt);

        } else if (curTok == RW.RW_decrement) {

            instr_decrement(out, tempStr, tempIntVect, tempReservedArea, tPtr, ptrPosition, tempInt);

        } else if (curTok == RW.RW_read) {

            instr_read(out, tempStr, tempIntVect, tempReservedArea, tPtr, ptrPosition, tempInt);

        } else if (curTok == RW.RW_cout){

            instr_cout(out, tempStr, tempIntVect, tempReservedArea, tPtr, ptrPosition, tempInt);

        } else if (curTok == RW.RW_logic) {

            instr_logic(out, tempStr, tempIntVect, tempReservedArea, tPtr, ptrPosition, tempInt, logicMode);

        } else if (curTok == RW.RW_alias) {

            instr_alias(out, tempStr, tempIntVect, tempReservedArea, tPtr, ptrPosition);

        } else if (curTok == RW.RW_loads) {

            instr_loads(out, tempStr, tempIntVect, tempReservedArea, tPtr, ptrPosition, tempInt, memsize);
        
        } else if (curTok == RW.RW_inline) {

            instr_inline(out, tempStr, tempIntVect, tempReservedArea, tPtr, ptrPosition);

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