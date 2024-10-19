#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "common.hpp"

extern int* ptrPosPtr;
extern std::string* curTokPtr;
extern std::vector<std::string> Tokens;
extern int tPtr;
extern std::vector<std::array<int, 2>> reserved;
extern int errorCount;
extern int warnCount;
extern std::string curTok;

void raise_compiler_error(int errorID, std::string message = "", std::string errorContext = "");
void raise_compiler_warning(int warningID, std::string message = "", std::string warningContext = "");
bool is_valid_hexadecimal(std::string& str);
inline std::string move_to(int target, int curPos = *ptrPosPtr);
std::string add_n_chars(int n, char c = '+');
std::string multiply_string(std::string targetString, int n);
void get_cur_tok();
bool is_reserved(int address, std::vector<std::array<int, 2>> reserved_areas);
bool reserved_overlap(std::vector<std::array<int, 2>> reserved_segments_vector, std::array<int, 2> addresses);
std::array<int, 2> find_reserved(std::vector<std::array<int, 2>> reserved_segments_vector, int requiredSize);
int hex_to_int(std::string hexString);
std::string slice_string(const std::string& targetString, int idx1, int idx2);
int address_string_to_int(std::string addressString, std::string prefix = "?");
inline int nearest_power_of_two(int n);
std::string compile(std::vector<std::string> Tokens_);

inline std::string move_to(int target, int curPos) {

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
inline int nearest_power_of_two(int n) {

    if (n <= 0) {
        return 1; // Minimum power of two is 2^0 = 1
    }

    int power = 1;
    while (power < n) {
        power *= 2;
    }

    return power;

}

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
    invalidLogicOperator

};

enum CompilerWarnings : int {

    accessingReservedAddress,
    reservedInlineCharacter

};

extern ReservedWords RW;
extern CodeSnippets CS;
extern BrainFuckOperations BFO;
extern OperationRequiredMemory ORM;

#endif