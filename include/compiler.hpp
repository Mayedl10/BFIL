#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "common.hpp"

struct CodeSnippets {

    // requires no additional data cells
    std::string setToZero = "[-]";

    // memory layout: i r r r r r r r (requires 8 adjasent cells)
    std::string numOut = ">>++++++++++<<[->+>-[>+>>]>[+[-<+>]>+>>]<<<<<<]>>[-]>>>++++++++++<[->-[>+>>]>[+[-<+>]>+>>]<<<<<]>[-]>>[>++++++[-<++++++++>]<.<<+>+>[-]]<[<[->-<]++++++[->++++++++<]>.[-]]<<++++++[-<++++++++>]<.[-]<<[-<+>]<";

    // memory layout: io i
    std::string difference = "[-<->]";

    // memory layout: o1 o2 o3 i1 i2
    std::string comparison_magic = "+[>>>>[-<]<<]<[->>+<]>>[<<<->+>>[-]]>[-]";

    // memory layout: i o       pointer lands on i
    std::string logic_not = ">[-]+<[[-]>-<]";

    // memory layout: i o       pointer lands on i
    std::string collapse_int = "[>[-]+<[-]]"; // this sets numbers 1-255 to 1 and sets 0 to 0

};

struct BrainFuckOperations {

    // the only reason this exists is because I just don't like having raw data (strings) dangling around my code

    std::string plus = "+";
    std::string minus = "-";

    std::string left = "<";
    std::string right = ">";

    std::string asciiOut = ".";
    std::string input = ",";

    std::string openBr = "[";
    std::string closeBr = "]";

    std::string allOps = "+-<>.,[]";

};

struct OperationRequiredMemory {

    int add         = 4;
    int sub         = add;
    int vout        = 12;
    int compare     = 12;
    int copy        = 4;
    int cout        = 4;
    int logic_not   = 4;
    int logic_and   = 8;
    int logic_or    = logic_and;
    int zeroed_cell = 1;

};

enum CompilerErrors : int {

    insufficientReservedMemoryError,
    invalidHexadecimalValue,
    invalidMemoryAddress,
    invalidValueToLoad,
    invalidComparisonOperator,
    unmatchedEndLoop,
    unmatchedWNZ,
    invalidLogicOperator,
    outOfMemory

};

enum CompilerWarnings : int {

    accessingReservedAddress,
    reservedInlineCharacter,
    variablesAndDirectAddressing

};

class Compiler {

    int* ptrPosPtr;
    std::string* curTokPtr;
    std::vector<std::string> Tokens;
    int tPtr;
    std::vector<std::array<int, 2>> reserved;
    int errorCount;
    int warnCount;
    std::string curTok;
    bool displayWarnings;

    bool variablesUsed;
    bool manualAddressingUsed;

    std::string out = "";
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
    std::vector<std::pair<std::string, int>> variableInitialValues;
    std::unordered_map<std::string, int> variableAddressLookup;

    ReservedWords RW;
    CodeSnippets CS;
    BrainFuckOperations BFO;
    OperationRequiredMemory ORM;

    std::unordered_map<std::string, void (Compiler::*)()> instructionMap;

public:

    void define_globals(bool displayWarnings);
    void raise_compiler_error(int errorID, std::string message = "", std::string errorContext = "");
    void raise_compiler_warning(int warningID, std::string message = "", std::string warningContext = "");
    bool is_valid_hexadecimal(std::string& str);
    bool is_valid_decimal(std::string str);
    inline std::string move_to(int target);
    std::string add_n_chars(int n, char c = '+');
    std::string multiply_string(std::string targetString, int n);
    void get_cur_tok();
    bool is_reserved(int address, std::vector<std::array<int, 2>> reserved_areas);
    bool is_variable(int address);
    bool reserved_overlap(std::vector<std::array<int, 2>> reserved_segments_vector, std::array<int, 2> addresses);
    std::array<int, 2> find_reserved(std::vector<std::array<int, 2>> reserved_segments_vector, int requiredSize);
    int hex_to_int(std::string hexString);
    std::string slice_string(const std::string& targetString, int idx1, int idx2);
    int address_string_to_int(std::string addressString, std::string prefix = "?");
    void scan_code();
    inline int nearest_power_of_two(int n);
    int generate_variable_address();

    void instr_add ();
    void instr_alias ();
    void instr_aout ();
    void instr_compare ();
    void instr_copy ();
    void instr_cout ();
    void instr_decrement ();
    void instr_empty ();
    void instr_endLoop ();
    void instr_increment ();
    void instr_inline ();
    void instr_load ();
    void instr_loads ();
    void instr_logic ();
    void instr_memsize ();
    void instr_read ();
    void instr_reserve ();
    void instr_sub ();
    void instr_var(); // doesn't actually do anything. mainly handled by scan_code.
    void instr_vout ();
    void instr_wnz ();

    std::string compile(std::vector<std::string> Tokens_, bool displayWarnings);

};


inline std::string Compiler::move_to(int target) {

    int curPos = *ptrPosPtr;

    if (ptrPosPtr == nullptr) {
        raise_compiler_error(-1, "Used internal function 'move_to' with ptrPosPtr = nullptr.", "This should never occur. (Hence the id -1)");
        return "[ERROR]";

    } else {
        *ptrPosPtr = target;
        std::string ret = "";

        while (curPos < target) {
            ret += ">";
            curPos++;
        }

        while (curPos > target) {
            ret += "<";
            curPos--;
        }

        return ret;
    }
}

// unused
inline int Compiler::nearest_power_of_two(int n) {

    if (n <= 0) {
        return 1; // Minimum power of two is 2^0 = 1
    }

    int power = 1;
    while (power < n) {
        power *= 2;
    }

    return power;

}



#endif