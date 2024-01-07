#ifndef COMPILER_HPP
#define COMPILER_HPP


std::string compile(std::vector<std::string> Tokens_);

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

// #define COMPILER_DEBUG

#endif