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
    outOfMemory,
    multipleVariableDefinitions,
    invalidVariableName,
    unexpectedToken,
    typeError,
    nestedSubroutineDefinition,
    defWithoutReturn,
    returnWithoutDef,
    invalidSubroutineIdentifier,
    INTERNAL_ERROR_insert_subroutine_tokens_A,
    INTERNAL_ERROR_insert_subroutine_tokens_B,
    INTERNAL_ERROR_insert_subroutine_tokens_C,
    includedFileNotFound

};

enum CompilerWarnings : int {

    accessingReservedAddress,
    reservedInlineCharacter,
    variablesAndDirectAddressing,
    multipleMemsizeDeclarations

};

class Compiler {

    int* ptrPosPtr;
    std::string* curTokPtr;
    std::vector<std::string> Tokens;
    int tPtr;
    std::vector<std::array<int, 2>> reserved;
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
    std::stack<int> ifZeroAddressStack;
    std::string logicMode;
    int ptrPosition = 0;
    std::vector<std::pair<std::string, int>> variableInitialValues;
    std::unordered_map<std::string, int> variableAddressLookup;
    bool insideSubroutine;

    ReservedWords RW;
    CodeSnippets CS;
    BrainFuckOperations BFO;
    OperationRequiredMemory ORM;

    std::unordered_map<std::string, void (Compiler::*)()> instructionMap;
    std::unordered_map<std::string, std::vector<std::string>> subroutineTokens;

    std::vector<std::string> includedFiles;
    std::vector<std::string> linkerDirectories = {"./"};

public:

    int errorCount;
    int warnCount;

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
    bool is_valid_address_str(std::string addressString);
    int address_string_to_int(std::string addressString, std::string prefix = "?");
    bool both_variables_and_addresses_used();
    void scan_code_var(int &tIdx, std::vector<int> &varDeclatationIdxs);
    void scan_code();
    std::string construct_address_str(int addr);
    inline int nearest_power_of_two(int n);
    int generate_variable_address();
    bool vector_contains_string(std::vector<std::string> vec, std::string str);
    bool vector_contains_int(std::vector<int> vec, int i);
    bool var_exists(std::string var);
    void load_const_value(int target, int value); // in instr_LOAD.cpp
    void copy_values(int source, int target, bool allowReserved); // in instr_COPY.cpp
    void find_included_files();
    void include_files();
    void generate_subroutines();
    void generate_subroutines(std::vector<std::string> &Tokens);
    void insert_subroutine_tokens();
    bool subroutine_exists(std::string name);

    void instr_add();
    void instr_alias();
    void instr_aout();
    void instr_call();
    void instr_compare();
    void instr_copy();
    void instr_cout();
    void instr_decrement();
    void instr_def();
    void instr_empty();
    void instr_endIf();
    void instr_endLoop ();
    void instr_if();
    void instr_include();
    void instr_increment();
    void instr_inline();
    void instr_load();
    void instr_loads();
    void instr_logic();
    void instr_memsize();
    void instr_read();
    void instr_reserve();
    void instr_return();
    void instr_sub();
    void instr_var(); // only sets default values
    void instr_vout();
    void instr_wnz();

    void instr_op_EQ();

    void add_linker_directory(std::string path);
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