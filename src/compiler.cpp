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

int* ptrPosPtr = nullptr;
static std::string* curTokPtr;
static std::vector<std::string> Tokens;
static int tPtr;
std::vector<std::array<int, 2>> reserved;


#ifdef COMPILER_DEBUG
// unused
int debugCTR = 0;
void debug_print() {
    std::cout << "[Debug]:" << debugCTR << std::endl;
    debugCTR++;
}
#endif

int errorCount = 0;
int warnCount = 0;

void raise_compiler_error(int errorID, std::string message = "", std::string errorContext = "") {
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

void raise_compiler_warning(int warningID, std::string message = "", std::string warningContext = "") {

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

static inline std::string move_to(int target, int curPos = *ptrPosPtr) {

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

static std::string add_n_chars(int n, char c = '+') {
    std::string ret = "";

    for (int i = 0; i < n; i++) {
        ret += c;
    }

    return ret;
}

static std::string multiply_string(std::string targetString, int n) {
    std::string ret = "";

    for (int i = 0; i < n; i++) {
        ret += targetString;
    }

    return ret;
}



static void get_cur_tok() {
    *curTokPtr = Tokens[tPtr];

}

static bool is_reserved(int address, std::vector<std::array<int, 2>> reserved_areas) {

    for (auto y: reserved_areas) {

        if ((address >= y[0]) && (address <= y[1])) {

            return true;
        }
    }

    return false;
}

static bool reserved_overlap(std::vector<std::array<int, 2>> reserved_segments_vector, std::array<int, 2> addresses) {   
    std::vector<int> addressRange = range(addresses[0], addresses[1]);

    for (auto a: addressRange) {
        if (is_reserved(a, reserved_segments_vector)) {
            return true;
        }
    }

    return false;
}

static std::array<int, 2> find_reserved(std::vector<std::array<int, 2>> reserved_segments_vector, int requiredSize) {

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




static int hex_to_int(std::string hexString) {

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

static std::string slice_string(const std::string& targetString, int idx1, int idx2) {

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

static int address_string_to_int(std::string addressString, std::string prefix = "?") {

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

// unused
static inline int nearest_power_of_two(int n) {

    if (n <= 0) {
        return 1; // Minimum power of two is 2^0 = 1
    }

    int power = 1;
    while (power < n) {
        power *= 2;
    }

    return power;

}

std::string compile(std::vector<std::string> Tokens_string_vector) {

    ReservedWords RW;
    CodeSnippets CS;
    BrainFuckOperations BFO;
    OperationRequiredMemory ORM;

    std::string out = "";

    Tokens = Tokens_string_vector;  // just doing this so I can use a global variable instead of a local parameter

    tPtr = 0;                       // token "pointer"
    int tPtrLimit = Tokens.size();  // highest possible value for the token pointer

    std::string curTok;
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
            tPtr++;
            get_cur_tok();
            memsize = hex_to_int(curTok);

        } else if (curTok == RW.RW_load) {
            tPtr++;
            get_cur_tok();
            tempInt = address_string_to_int(curTok);

            if (reserved_overlap(reserved, {tempInt, tempInt})) {
                
                raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
                "Using a reserved address is not recommended!",              
                "... 'load " + curTok + " <- value' ..." 
                );
            }

            if (tempInt >= memsize || tempInt < 0) {

                raise_compiler_error(CompilerErrors::invalidMemoryAddress,
                "Couldn't load into address because it is bigger than specified memory size ('memsize' instruction) or it is less than zero.", 
                "... 'load ?"+std::to_string(tempInt)+" <- value' ...");
            }

            out += move_to(tempInt, ptrPosition);
            out += CS.setToZero;

            tPtr += 1;
            tPtr += 1;

            get_cur_tok();

            if (std::stoi(curTok) < 0 || std::stoi(curTok) > 255) {

                raise_compiler_error(CompilerErrors::invalidValueToLoad,
                "Couldn't load value because it is either less than zero or greater than 255. Only unsigned 8-bit integers (0-255) may be loaded.",
                "... 'load ?address <- "+curTok+"' ...");
            }

            out += add_n_chars(std::stoi(curTok), '+');

        } else if (curTok == RW.RW_reserve) {

            tPtr++;
            get_cur_tok();
            tempReservedArea[0] = address_string_to_int(curTok);

            tPtr+=2;
            get_cur_tok();
            tempReservedArea[1] = address_string_to_int(curTok);

            if ((tempReservedArea[0] >= memsize) || (tempReservedArea[1] >= memsize) || (tempReservedArea[0] < 0) || (tempReservedArea[1] < 0)) {

                raise_compiler_error(CompilerErrors::invalidMemoryAddress,
                "Couldn't reserve memory area as it is bigger than specified memory size ('memsize' instruction) or it is less than zero.",
                "... 'reserve ?"+std::to_string(tempReservedArea[0])+" ~ ?"+std::to_string(tempReservedArea[1])+"' ...");
            }

            reserved.push_back(tempReservedArea);

        } else if (curTok == RW.RW_add) {

            tempStr = "";
            tempIntVect.clear();    // forgetting this line. caused me to spend 4 HOURS TRYING TO FIGURE OUT WHY IT WASN'T WORKING! I EVEN FUCKING REWROTE THIS ENTIRE PART AAAA
            tempReservedArea = find_reserved(reserved, ORM.add); // reserved area with required size

            tPtr++;
            get_cur_tok();
            tempIntVect.push_back(address_string_to_int(curTok)); // param 1 ... idx[0]
            
            if (reserved_overlap(reserved, {tempIntVect[0], tempIntVect[0]})) {
                
                raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
                "Using a reserved address is not recommended!",              
                "... 'add " + curTok + " <- address' ..." 
                );
            }

            tPtr+=2;
            get_cur_tok();
            tempIntVect.push_back(address_string_to_int(curTok)); // param 2 ... idx[1]

            if (reserved_overlap(reserved, {tempIntVect[1], tempIntVect[1]})) {
                
                raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
                "Using a reserved address is not recommended!",              
                "... 'add address <- " + curTok + "' ..." 
                );
            }

            // code gen

            // "sanitize" reserved area (just in case)
            tempStr += move_to(tempReservedArea[0]);
            tempStr += CS.setToZero;
            while (ptrPosition < tempReservedArea[1]) {
                tempStr += move_to(ptrPosition +1);
                tempStr += CS.setToZero;
            }

            // 0. move param1 to reserved[0]
            // 1. move param2 to reserved[1:2] (paste them twice there)
            // 2. move reserved[2] to param2 (so param2 can keep its value)
            // 3. add reserved[1] to reserved[0]
            // 4. move reserved[0] to param1
            // 5. "sanitize" reserved area again


            // .0 move param1 to reserved[0]
            tempStr += move_to(tempIntVect[0]);          // move_to param1
            tempStr += BFO.openBr;                      // [
            tempStr += BFO.minus;                       //  -   grab param1.value
            tempStr += move_to(tempReservedArea[0]);     //  <<< move_to reserved[0]
            tempStr += BFO.plus;                        //  +   place param1.value
            tempStr += move_to(tempIntVect[0]);          //  >>> move_to param1
            tempStr += BFO.closeBr;                     // ]

            // .1 move param2 to reserved[1:2] (paste them twice there)
            tempStr += move_to(tempIntVect[1]);          // move_to param2
            tempStr += BFO.openBr;                      // [
            tempStr += BFO.minus;                       //  -   grav param2.value
            tempStr += move_to(tempReservedArea[0]+1);   //  <<< move_to reserved[1]
            tempStr += BFO.plus;                        //  +   place param2.value
            tempStr += move_to(tempReservedArea[0]+2);   //  >   move_to reserved[2]
            tempStr += BFO.plus;                        //  +   place param2.value
            tempStr += move_to(tempIntVect[1]);          //  >>> move_to param2
            tempStr += BFO.closeBr;                     // ]

            // .2 move reserved[2] to param2 (so param2 can keep its value)
            tempStr += move_to(tempReservedArea[0]+2);   // move_to reserved[2]
            tempStr += BFO.openBr;                      // [
            tempStr += BFO.minus;                       //  -   grab reserved[2]
            tempStr += move_to(tempIntVect[1]);          //  >>> move_to param2
            tempStr += BFO.plus;                        //  +   place value
            tempStr += move_to(tempReservedArea[0]+2);   //  <<< move_to reserved[2]
            tempStr += BFO.closeBr;                     // ]

            // .3 add r[1] -> r[0]
            tempStr += move_to(tempReservedArea[0]+1);   // move_to reserved[1]
            tempStr += BFO.openBr;                      // [
            tempStr += BFO.minus;                       //  -   grab [1]
            tempStr += move_to(tempReservedArea[0]);     //  <<< move_to [0]
            tempStr += BFO.plus;                        //  +   add [1] -> [0]
            tempStr += move_to(tempReservedArea[0]+1);   //  >>> move_to [1]
            tempStr += BFO.closeBr;                     // ]

            // .4 move reserved[0] to param1
            tempStr += move_to(tempReservedArea[0]);     // move_to reserved[0]
            tempStr += BFO.openBr;                      // [
            tempStr += BFO.minus;                       //  -   grab [0]
            tempStr += move_to(tempIntVect[0]);          //  >>> move_to param1
            tempStr += BFO.plus;                        //  +   place [0]
            tempStr += move_to(tempReservedArea[0]);     //  <<< move_to [0]
            tempStr += BFO.closeBr;                     // ]

            // .5 "sanitize" the reserved area again
            tempStr += move_to(tempReservedArea[0]);
            tempStr += CS.setToZero;
            while (ptrPosition < tempReservedArea[1]) {
                tempStr += move_to(ptrPosition +1);
                tempStr += CS.setToZero;
            }

            out += tempStr;

        } else if (curTok == RW.RW_sub) {

            tempStr = "";
            tempIntVect.clear();    // forgetting this line. caused me to spend 4 HOURS TRYING TO FIGURE OUT WHY IT WASN'T WORKING! I EVEN FUCKING REWROTE THIS ENTIRE PART AAAA
            tempReservedArea = find_reserved(reserved, ORM.add); // reserved area with required size

            tPtr++;
            get_cur_tok();
            tempIntVect.push_back(address_string_to_int(curTok)); // param 1 ... idx[0]

            if (reserved_overlap(reserved, {tempIntVect[0], tempIntVect[0]})) {
                
                raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
                "Using a reserved address is not recommended!",              
                "... 'sub " + curTok + " <- address' ..." 
                );
            }

            tPtr+=2;
            get_cur_tok();
            tempIntVect.push_back(address_string_to_int(curTok)); // param 2 ... idx[1]

            if (reserved_overlap(reserved, {tempIntVect[1], tempIntVect[1]})) {
                
                raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
                "Using a reserved address is not recommended!",              
                "... 'add address <- " + curTok + "' ..." 
                );
            }

            // code gen

            // "sanitize" reserved area (just in case)
            tempStr += move_to(tempReservedArea[0]);
            tempStr += CS.setToZero;
            while (ptrPosition < tempReservedArea[1]) {
                tempStr += move_to(ptrPosition +1);
                tempStr += CS.setToZero;
            }

            // 0. move param1 to reserved[0]
            // 1. move param2 to reserved[1:2] (paste them twice there)
            // 2. move reserved[2] to param2 (so param2 can keep its value)
            // 3. add reserved[1] to reserved[0]
            // 4. move reserved[0] to param1
            // 5. "sanitize" reserved area again


            // .0 move param1 to reserved[0]
            tempStr += move_to(tempIntVect[0]);          // move_to param1
            tempStr += BFO.openBr;                      // [
            tempStr += BFO.minus;                       //  -   grab param1.value
            tempStr += move_to(tempReservedArea[0]);     //  <<< move_to reserved[0]
            tempStr += BFO.plus;                        //  +   place param1.value
            tempStr += move_to(tempIntVect[0]);          //  >>> move_to param1
            tempStr += BFO.closeBr;                     // ]

            // .1 move param2 to reserved[1:2] (paste them twice there)
            tempStr += move_to(tempIntVect[1]);          // move_to param2
            tempStr += BFO.openBr;                      // [
            tempStr += BFO.minus;                       //  -   grav param2.value
            tempStr += move_to(tempReservedArea[0]+1);   //  <<< move_to reserved[1]
            tempStr += BFO.plus;                        //  +   place param2.value
            tempStr += move_to(tempReservedArea[0]+2);   //  >   move_to reserved[2]
            tempStr += BFO.plus;                        //  +   place param2.value
            tempStr += move_to(tempIntVect[1]);          //  >>> move_to param2
            tempStr += BFO.closeBr;                     // ]

            // .2 move reserved[2] to param2 (so param2 can keep its value)
            tempStr += move_to(tempReservedArea[0]+2);   // move_to reserved[2]
            tempStr += BFO.openBr;                      // [
            tempStr += BFO.minus;                       //  -   grab reserved[2]
            tempStr += move_to(tempIntVect[1]);          //  >>> move_to param2
            tempStr += BFO.plus;                        //  +   place value
            tempStr += move_to(tempReservedArea[0]+2);   //  <<< move_to reserved[2]
            tempStr += BFO.closeBr;                     // ]

            // .3 add r[1] -> r[0]
            tempStr += move_to(tempReservedArea[0]+1);   // move_to reserved[1]
            tempStr += BFO.openBr;                      // [
            tempStr += BFO.minus;                       //  -   grab [1]
            tempStr += move_to(tempReservedArea[0]);     //  <<< move_to [0]
            tempStr += BFO.minus;                       //  -   sub [1] -> [0]      only line different from "add"
            tempStr += move_to(tempReservedArea[0]+1);   //  >>> move_to [1]
            tempStr += BFO.closeBr;                     // ]

            // .4 move reserved[0] to param1
            tempStr += move_to(tempReservedArea[0]);     // move_to reserved[0]
            tempStr += BFO.openBr;                      // [
            tempStr += BFO.minus;                       //  -   grab [0]
            tempStr += move_to(tempIntVect[0]);          //  >>> move_to param1
            tempStr += BFO.plus;                        //  +   place [0]
            tempStr += move_to(tempReservedArea[0]);     //  <<< move_to [0]
            tempStr += BFO.closeBr;                     // ]

            // .5 "sanitize" the reserved area again
            tempStr += move_to(tempReservedArea[0]);
            tempStr += CS.setToZero;
            while (ptrPosition < tempReservedArea[1]) {
                tempStr += move_to(ptrPosition +1);
                tempStr += CS.setToZero;
            }

            out += tempStr;

        } else if (curTok == RW.RW_aout) {

            tPtr++;
            get_cur_tok();
            tempInt = std::stoi(curTok);    // amount of parameters
            tempIntVect.clear();
            tempStr = "";

            for (int i = 0; i < tempInt; i++) {
                tPtr++;
                get_cur_tok();
                tempIntVect.push_back(address_string_to_int(curTok));
            }

            // brainfuck code generation
            // how?: iterate through tempIntVect, move to ?i:tempIntVect and use BFO.asciiOut

            for (int i = 0; i < tempIntVect.size(); i++) {
                tempStr += move_to(tempIntVect[i]);
                tempStr += BFO.asciiOut;
            }

            out += tempStr;

        } else if (curTok == RW.RW_vout) {

            tempStr = "";
            tempInt = 0;
            tempIntVect.clear();
            tempReservedArea = {};

            // steps:
             /*
                0. get the correct memory addresses into tempIntVect
                1. find a big enough reserved area and sanitize it
                2. for each value in tempIntVect do:
                    2.0 copy value from addres to r[0:1]
                    2.1 move r[1] back to address
                    2.2 move to r[0]
                    2.3 do the magic
                    2.4 sanitize the reserved area
             */

            // .0
            tPtr++;
            get_cur_tok();
            tempInt = std::stoi(curTok);    // now contains the amount of parameters

            // get the addresses into tempIntVect
            for (int i = 0; i < tempInt; i++) {
                tPtr++;
                get_cur_tok();
                tempIntVect.push_back(address_string_to_int(curTok));
            }

            // .1
            tempReservedArea = find_reserved(reserved, ORM.vout);

            // sanitize
            tempStr += move_to(tempReservedArea[0]);
            tempStr += CS.setToZero;
            while (ptrPosition < tempReservedArea[1]) {
                tempStr += move_to(ptrPosition +1);
                tempStr += CS.setToZero;
            }

            // .2
            //get address of separating char
            tPtr+=2;
            get_cur_tok();
            bool doSeparate;
            if (address_string_to_int(curTok) < 0 || address_string_to_int(curTok) > memsize) {
                doSeparate = false;
            } else {
                doSeparate = true;
            }
            int separatorAddress = address_string_to_int(curTok);

            // do .2::[0:4]
            for (int i = 0; i < tempIntVect.size(); i++) {
                int address = tempIntVect[i];                
                //.2::0 copy value
                tempStr += move_to(address);                 // move_to address
                tempStr += BFO.openBr;                      // [
                tempStr += BFO.minus;                       //  -   grab value
                tempStr += move_to(tempReservedArea[0]);     //  <<< move_to r[0]
                tempStr += BFO.plus;                        //  +   paste @r[0]
                tempStr += move_to(tempReservedArea[0]+1);   //  >   move_to r[1]
                tempStr += BFO.plus;                        //  +   paste @r[1]
                tempStr += move_to(address);                 //  >>> move_to address
                tempStr += BFO.closeBr;                     // ]

                // .2::1 r[1] -> address
                tempStr += move_to(tempReservedArea[0]+1);   // move_to r[1]
                tempStr += BFO.openBr;                      // [
                tempStr += BFO.minus;                       //  -   grab value
                tempStr += move_to(address);                 //  >>> move_to address
                tempStr += BFO.plus;                        //  +   paste @address
                tempStr += move_to(tempReservedArea[0]+1);   //  <<< move_to r[1]
                tempStr += BFO.closeBr;                     // ]

                // .2::2 move_to r[0]
                tempStr += move_to(tempReservedArea[0]);

                // .2::3 do da magic
                tempStr += CS.numOut;

                // .2::4 sanitize and insert spaces between numbers
                tempStr += move_to(tempReservedArea[0]);
                tempStr += CS.setToZero;
                while (ptrPosition < tempReservedArea[1]) {
                    tempStr += move_to(ptrPosition +1);
                    tempStr += CS.setToZero;
                }

                if (doSeparate && (i+1 < tempIntVect.size())) {
                    tempStr += move_to(separatorAddress);
                    tempStr += BFO.asciiOut;
                }

            }

            out += tempStr;

        } else if (curTok == RW.RW_compare) {

            tempStr = "";
            tempInt = 0;
            tempIntVect.clear();
            tempReservedArea = {};
            std::string comparisonMode = "";

            tPtr++;
            get_cur_tok();
            tempIntVect.push_back(address_string_to_int(curTok)); // get first input address into [0]

            tPtr++;
            get_cur_tok();
            comparisonMode = curTok;    // either =, <, or >

            tPtr++;
            get_cur_tok();
            tempIntVect.push_back(address_string_to_int(curTok)); // get second input address into [1]

            tPtr+=2;
            get_cur_tok();
            tempIntVect.push_back(address_string_to_int(curTok)); // get output address into [2]

            // memory layout for "compare": o1  o2  o3  i1  i2  r1
            //                              [0] [1] [2] [3] [4] [5]
            // steps: 
             /*
             0. find reserved area and sanitize
             1. copy [0] to r[3:4]
             2. move r[4] to [1]
             3. copy [1] to r[4:5]
             4. move r[5] to [2]
             5. move_to r[4] (this is where the pointer is after the comparison. I don't want to loose track of that thing)
             6. insert magic.
             7. move to the output address and set it to zero (to not interfere with 8.) 
             8. depending on "comparisonMode", move to either r[0], r[1] or r[2] and move that value to the output parameter.
             9. sanitize r
             */

            // .0
            tempReservedArea = find_reserved(reserved, ORM.compare);

            // sanitize
            tempStr += move_to(tempReservedArea[0]);
            tempStr += CS.setToZero;
            while (ptrPosition < tempReservedArea[1]) {
                tempStr += move_to(ptrPosition +1);
                tempStr += CS.setToZero;
            }

            // .1 copy [0] to r[3:4]
            tempStr += move_to(tempIntVect[0]);              // move_to input 1
            tempStr += BFO.openBr;                          // [
            tempStr += BFO.minus;                           //  -   grab value
            tempStr += move_to(tempReservedArea[0]+3);       //  <<< move_to r[3]
            tempStr += BFO.plus;                            //  +   place @r[3]
            tempStr += move_to(tempReservedArea[0]+4);   	//  >   move_to r[4]
            tempStr += BFO.plus;                            //  +   place @r[4]
            tempStr += move_to(tempIntVect[0]);              //  >>> move_to input 1
            tempStr += BFO.closeBr;                         // ]

            // .2 move r[4] to [1]
            tempStr += move_to(tempReservedArea[0]+4);       // move_to r[4]
            tempStr += BFO.openBr;                          // [
            tempStr += BFO.minus;                           //  -   grab value
            tempStr += move_to(tempIntVect[0]);              //  >>> move_to input 1
            tempStr += BFO.plus;                            //  +   place @input 1
            tempStr += move_to(tempReservedArea[0]+4);       //  <<< move_to r[4]
            tempStr += BFO.closeBr;                         // ]

            // .3 copy [1] to r[4:5]
            tempStr += move_to(tempIntVect[1]);              // move_to input 2
            tempStr += BFO.openBr;                          // [
            tempStr += BFO.minus;                           //  -   grab value
            tempStr += move_to(tempReservedArea[0]+4);       //  <<< move_to r[4]
            tempStr += BFO.plus;                            //  +   place @r[4]
            tempStr += move_to(tempReservedArea[0]+5);   	//  >   move_to r[5]
            tempStr += BFO.plus;                            //  +   place @r[5]
            tempStr += move_to(tempIntVect[1]);              //  >>> move_to input 2
            tempStr += BFO.closeBr;                         // ]

            // .4 move r[5] to [2]
            tempStr += move_to(tempReservedArea[0]+5);       // move_to r[5]
            tempStr += BFO.openBr;                          // [
            tempStr += BFO.minus;                           //  -   grab value
            tempStr += move_to(tempIntVect[1]);              //  >>> move_to input 1
            tempStr += BFO.plus;                            //  +   place @input 1
            tempStr += move_to(tempReservedArea[0]+5);       //  <<< move_to r[5]
            tempStr += BFO.closeBr;  

            // .5
            tempStr += move_to(tempReservedArea[0]);         // this is where the pointer needs to be

            // .6 magic
            tempStr += CS.comparison_magic;                 // dangerous magic.
            move_to(tempReservedArea[0]+4);


            // .7 move to the output address and set it to zero (to not interfere with 8.) 
            tempStr += move_to(tempIntVect[2]);
            tempStr += CS.setToZero;

            // .8  depending on "comparisonMode", move to either r[0], r[1] or r[2] and move that value to the output parameter.
            if (comparisonMode == "=") {

                tempStr += move_to(tempReservedArea[0]);

            } else if (comparisonMode == ">") {

                tempStr += move_to(tempReservedArea[0]+1);

            } else if (comparisonMode == "<") {

                tempStr += move_to(tempReservedArea[0]+2);

            } else {
                
                raise_compiler_error(CompilerErrors::invalidComparisonOperator, "Invalid comparison operator.", "... 'compare ?address " + comparisonMode + " ?address -> ?address' ...");
            }

            tempStr += BFO.openBr;                          // [
            tempStr += BFO.minus;                           //  -
            tempStr += move_to(tempIntVect[2]);              //  >>>
            tempStr += BFO.plus;                            //  +

            if (comparisonMode == "=") {                    //  <<<

                tempStr += move_to(tempReservedArea[0]);

            } else if (comparisonMode == ">") {

                tempStr += move_to(tempReservedArea[0]+1);

            } else if (comparisonMode == "<") {

                tempStr += move_to(tempReservedArea[0]+2);

            } else {

                raise_compiler_error(CompilerErrors::invalidComparisonOperator, "Invalid comparison operator.", "... 'compare ?address " + comparisonMode + " ?address -> ?address' ...");
            }

            tempStr += BFO.closeBr;

            // .9
            tempStr += move_to(tempReservedArea[0]);
            tempStr += CS.setToZero;
            while (ptrPosition < tempReservedArea[1]) {
                tempStr += move_to(ptrPosition +1);
                tempStr += CS.setToZero;
            }


             /*
             So will ich meinen Stab zerbrechen,
             ihn etliche Klafter tief in die Erde vergraben,
             und tiefer als jemals ein Senkbley fiel,
             mein Zauberbuch im Meer versenken.
             -William Shakespeare: Der Sturm, Akt 5, Szene 1

             I'll break my staff,
             Bury it certain fathoms in the earth,
             And deeper than did ever plummet sound
             I'll drown my book.
             -William Shakespeare: The tempest, act 5, scene 1          
             */

            // I added this as a joke because of the incredible fuckery I make my compiler do here. I give over the control over the pointer to the comparison algorithm and have to trust that it always ends up at the same position.


            out += tempStr;
        
        } else if (curTok == RW.RW_copy) {

            tempStr = "";
            tempInt = 0;
            tempIntVect.clear();
            tempReservedArea = {};

            tPtr++;
            get_cur_tok();
            tempIntVect.push_back(address_string_to_int(curTok)); // get target

            if (reserved_overlap(reserved, {tempIntVect[0], tempIntVect[0]})) {
                
                raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
                "Using a reserved address is not recommended!",              
                "... 'copy " + curTok + " <- address' ..." 
                );
            }

            tPtr+=2;
            get_cur_tok();
            tempIntVect.push_back(address_string_to_int(curTok)); // get source

            if (reserved_overlap(reserved, {tempIntVect[1], tempIntVect[1]})) {
                
                raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
                "Using a reserved address is not recommended!",              
                "... 'copy address <- " + curTok + "' ..." 
                );
            }

            // tempIntVect[0]...target ; [1]...source

             /*
            steps:

            0. find and sanitize reserved area
            1. copy source and paste it twice in r[0:1]
            2. move r[0] to source
            3. move r[1] to target
            4. sanitize reserved area

             */

            // .0
            tempReservedArea = find_reserved(reserved, ORM.copy);

            // sanitize
            tempStr += move_to(tempReservedArea[0]);
            tempStr += CS.setToZero;
            while (ptrPosition < tempReservedArea[1]) {
                tempStr += move_to(ptrPosition +1);
                tempStr += CS.setToZero;
            }

            // .1
            tempStr += move_to(tempIntVect[1]);          // move_to source
            tempStr += BFO.openBr;                      // [
            tempStr += BFO.minus;                       //  -   grab value
            tempStr += move_to(tempReservedArea[0]);     //  <<< move_to r[0]
            tempStr += BFO.plus;                        //  +   paste @r[0]
            tempStr += move_to(tempReservedArea[0]+1);   //  >   move_to r[1]
            tempStr += BFO.plus;                        //  +   paste @r[1]
            tempStr += move_to(tempIntVect[1]);          //  >>> move_to source
            tempStr += BFO.closeBr;                     // ]

            // .2

            tempStr += move_to(tempIntVect[1]);
            tempStr += CS.setToZero;

            tempStr += move_to(tempReservedArea[0]);     // move_to r[0]
            tempStr += BFO.openBr;                      // [
            tempStr += BFO.minus;                       //  -   grab value
            tempStr += move_to(tempIntVect[1]);          //  >>> move_to source
            tempStr += BFO.plus;                        //  +   paste value
            tempStr += move_to(tempReservedArea[0]);     //  <<< move_to r[0]
            tempStr += BFO.closeBr;                     // ]

            // .3
            tempStr += move_to(tempIntVect[0]);
            tempStr += CS.setToZero;

            tempStr += move_to(tempReservedArea[0]+1);   // move_to r[1]
            tempStr += BFO.openBr;                      // [
            tempStr += BFO.minus;                       //  -   grab value
            tempStr += move_to(tempIntVect[0]);          //  >>> move_to target
            tempStr += BFO.plus;                        //  +   paste value
            tempStr += move_to(tempReservedArea[0]+1);   //  <<< move_to r[1]
            tempStr += BFO.closeBr;                     // ]

            // .4
            tempStr += move_to(tempReservedArea[0]);
            tempStr += CS.setToZero;
            while (ptrPosition < tempReservedArea[1]) {
                tempStr += move_to(ptrPosition +1);
                tempStr += CS.setToZero;
            }

            out += tempStr;

        } else if (curTok == RW.RW_wnz) {

            tempStr = "";

             /*

            steps
            
            0. grab the target and push it to loopingAddressesStack
            1. move to loopingAddressesStack.top()
            2. [
            
            the rest is done in the "endLoop" part            
             */

            tPtr++;
            get_cur_tok();
            loopingAddressesStack.push(address_string_to_int(curTok)); // .0

            tempStr += move_to(loopingAddressesStack.top());             // .1
            tempStr += BFO.openBr;                                      // .2

            out += tempStr;

        } else if (curTok == RW.RW_endLoop) {

            tempStr = "";
            tempInt = 0;

             /*
            steps (numbering continued from WNZ)

            3. pop loopingAddressesStack into tempInt (raise an error if the stack is empty)
            4. move_to tempInt
            5. ]

             */

            // .3
            if (loopingAddressesStack.empty()) {
                raise_compiler_error(CompilerErrors::unmatchedEndLoop, "Unmatched 'endLoop'. This means there are more 'endLoop' statements than 'whileNotZero' statements.");
            } else {
                tempInt = loopingAddressesStack.top();
                loopingAddressesStack.pop();    
            }

            // .4
            tempStr += move_to(tempInt);
            // .5
            tempStr += BFO.closeBr;

            out += tempStr;

        } else if (curTok == RW.RW_empty) {

            tempStr = "";
            tempIntVect.clear();
            
            tPtr++;
            get_cur_tok();
            tempReservedArea[0] = address_string_to_int(curTok);

            tPtr+=2;
            get_cur_tok();
            tempReservedArea[1] = address_string_to_int(curTok);

            // this is just my "sanitize reserved area" script.
            // that's why I use tempReservedArea even though this 
            // sanitizes something else than a reserved area.
            tempStr += move_to(tempReservedArea[0]);
            tempStr += CS.setToZero;
            while (ptrPosition < tempReservedArea[1]) {
                tempStr += move_to(ptrPosition +1);
                tempStr += CS.setToZero;
            }

            out += tempStr;

        } else if (curTok == RW.RW_increment) {

            tempStr = "";
            tempInt = 0;

            tPtr++;
            get_cur_tok();

            tempInt = address_string_to_int(curTok);

            if (reserved_overlap(reserved, {tempInt, tempInt})) {
                
                raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
                "Using a reserved address is not recommended!",              
                "... 'increment " + curTok + "' ..." 
                );
            }

            tempStr += move_to(tempInt);
            tempStr += BFO.plus;

            out += tempStr;

        } else if (curTok == RW.RW_decrement) {

            tempStr = "";
            tempInt = 0;

            tPtr++;
            get_cur_tok();

            tempInt = address_string_to_int(curTok);

            if (reserved_overlap(reserved, {tempInt, tempInt})) {
                
                raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
                "Using a reserved address is not recommended!",              
                "... 'decrement " + curTok + "' ..." 
                );
            }

            tempStr += move_to(tempInt);
            tempStr += BFO.minus;

            out += tempStr;

        } else if (curTok == RW.RW_read) {

            tempStr = "";
            tempInt = 0;

            tPtr++;
            get_cur_tok();

            tempInt = address_string_to_int(curTok);

            if (reserved_overlap(reserved, {tempInt, tempInt})) {
                
                raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
                "Using a reserved address is not recommended!",              
                "... 'read " + curTok + "' ..." 
                );
            }

            tempStr += move_to(tempInt);
            tempStr += BFO.input;

            out += tempStr;

        } else if (curTok == RW.RW_cout){

            tempStr = "";
            tempReservedArea = {};
            tempIntVect.clear();
            tempInt = 0;

            tPtr++;
            get_cur_tok();

            tempReservedArea = find_reserved(reserved, ORM.cout);

            tempStr += move_to(tempReservedArea[0]);

            // add_n_chars(amount, '+');

            for (char c: curTok) {
                tempInt = c;
                tempStr += CS.setToZero;
                tempStr += add_n_chars(tempInt, '+');
                tempStr += BFO.asciiOut;
            }

            out += tempStr+CS.setToZero;

        } else if (curTok == RW.RW_logic) {

            tempStr = "";
            tempReservedArea = {};
            tempIntVect.clear();
            tempInt = 0; 
            
            tPtr += 2;
            get_cur_tok();
            logicMode = curTok;

            tPtr--;
            get_cur_tok();

            tempIntVect.push_back(address_string_to_int(curTok));  // no matter the mode, [0] always contains the first parameter.

            if (logicMode == RW.RW_LM_and) {
                tempReservedArea = find_reserved(reserved, ORM.logic_and);

                tPtr += 2;
                get_cur_tok();
                tempIntVect.push_back(address_string_to_int(curTok)); // [1] is i2
                tPtr += 2;
                get_cur_tok();
                tempIntVect.push_back(address_string_to_int(curTok)); // [2] is output

                if (reserved_overlap(reserved, {tempIntVect.back(), tempIntVect.back()})) {
                    
                    raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
                    "Using a reserved address is not recommended!",              
                    "... 'logic address and address -> " + curTok + "' ..." 
                    );
                }
                
                // and mode
                // steps:
                 /*
                 0. copy i1 to r[0:1]               // same as "or" //
                 1. move r[1] to i1                 // same as "or" //
                 2. copy i2 to r[1:2]               // same as "or" //
                 3. move r[2] to i2                 // same as "or" //
                 4. collapse r[1] to r[2]           // same as "or" //
                 5. collapse r[0] to r[1]           // same as "or" //
                 6. move r[1] to r[0]               // same as "or" //
                 7. move r[2] to r[1]               // same as "or" //
                 8. add r[1] -> r[0]                // same as "or" //
                 9. do comparison magic with value 2
                 10. move equality output to output
                 11. sanitize
                 */

                
                // .0   // same as "or" //
                tempStr += move_to(tempIntVect[0]);
                tempStr += BFO.openBr;
                tempStr += BFO.minus;
                tempStr += move_to(tempReservedArea[0]);
                tempStr +=BFO.plus;
                tempStr += move_to(tempReservedArea[0]+1);
                tempStr += BFO.plus;
                tempStr += move_to(tempIntVect[0]);
                tempStr += BFO.closeBr;

                // .1   // same as "or" //
                tempStr += move_to(tempReservedArea[0]+1);
                tempStr += BFO.openBr;
                tempStr += BFO.minus;
                tempStr += move_to(tempIntVect[0]);
                tempStr += BFO.plus;
                tempStr += move_to(tempReservedArea[0]+1);
                tempStr += BFO.closeBr;

                // .2   // same as "or" //
                tempStr += move_to(tempIntVect[1]);
                tempStr += BFO.openBr;
                tempStr += BFO.minus;
                tempStr += move_to(tempReservedArea[0]+1);
                tempStr +=BFO.plus;
                tempStr += move_to(tempReservedArea[0]+2);
                tempStr += BFO.plus;
                tempStr += move_to(tempIntVect[1]);
                tempStr += BFO.closeBr;

                // .3   // same as "or" //
                tempStr += move_to(tempReservedArea[0]+2);
                tempStr += BFO.openBr;
                tempStr += BFO.minus;
                tempStr += move_to(tempIntVect[1]);
                tempStr += BFO.plus;
                tempStr += move_to(tempReservedArea[0]+2);
                tempStr += BFO.closeBr;

                // .4   // same as "or" //
                tempStr += move_to(tempReservedArea[0]+1);
                tempStr += CS.collapse_int;

                // .5   // same as "or" //
                tempStr += move_to(tempReservedArea[0]);
                tempStr += CS.collapse_int;

                // .6   // same as "or" //
                tempStr += move_to(tempReservedArea[0]+1);
                tempStr += BFO.openBr;
                tempStr += BFO.minus;
                tempStr += move_to(tempReservedArea[0]);
                tempStr += BFO.plus;
                tempStr += move_to(tempReservedArea[0]+1);
                tempStr += BFO.closeBr;

                // .7   // same as "or" //
                tempStr += move_to(tempReservedArea[0]+2);
                tempStr += BFO.openBr;
                tempStr += BFO.minus;
                tempStr += move_to(tempReservedArea[0]+1);
                tempStr += BFO.plus;
                tempStr += move_to(tempReservedArea[0]+2);
                tempStr += BFO.closeBr;

                // .8   // same as "or" //
                tempStr += move_to(tempReservedArea[0]+1);
                tempStr += BFO.openBr;
                tempStr += BFO.minus;
                tempStr += move_to(tempReservedArea[0]);
                tempStr += BFO.plus;
                tempStr += move_to(tempReservedArea[0]+1);
                tempStr += BFO.closeBr;

                // .9 "Ich wollte meinen Stab zerbrechen..."
                // .9:1 move r[0] to r[3]
                // .9:2 load 2 into r[4]
                // .9:3 move_to r[0] and set ptrPosition to r[4]
                // .9:4 magic

                // .9:1
                tempStr += move_to(tempReservedArea[0]);
                tempStr += BFO.openBr;
                tempStr += BFO.minus;
                tempStr += move_to(tempReservedArea[0]+3);
                tempStr += BFO.plus;
                tempStr += move_to(tempReservedArea[0]);
                tempStr += BFO.closeBr;

                // .9:2
                tempStr += move_to(tempReservedArea[0]+4);
                tempStr += CS.setToZero;
                tempStr += BFO.plus;
                tempStr += BFO.plus;

                // .9:3
                tempStr += move_to(tempReservedArea[0]);
                move_to(tempReservedArea[0]+4);

                // .9:4
                tempStr += CS.comparison_magic;

                // .10
                tempStr += move_to(tempIntVect[2]);
                tempStr += CS.setToZero;

                tempStr += move_to(tempReservedArea[0]);
                tempStr += BFO.openBr;
                tempStr += BFO.minus;
                tempStr += move_to(tempIntVect[2]);
                tempStr += BFO.plus;
                tempStr += move_to(tempReservedArea[0]);
                tempStr += BFO.closeBr;

                // .11
                tempStr += move_to(tempReservedArea[0]);
                tempStr += CS.setToZero;
                while (ptrPosition < tempReservedArea[1]) {
                    tempStr += move_to(ptrPosition +1);
                    tempStr += CS.setToZero;
                }

            } else if (logicMode == RW.RW_LM_or) {

                tempReservedArea = find_reserved(reserved, ORM.logic_and);

                tPtr += 2;
                get_cur_tok();
                tempIntVect.push_back(address_string_to_int(curTok)); // [1] is i2
                tPtr += 2;
                get_cur_tok();
                tempIntVect.push_back(address_string_to_int(curTok)); // [2] is output

                if (reserved_overlap(reserved, {tempIntVect.back(), tempIntVect.back()})) {
                    
                    raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
                    "Using a reserved address is not recommended!",              
                    "... 'logic address or address -> " + curTok + "' ..." 
                    );
                }

                // or mode
                // steps:
                 /*
                 
                 0. copy i1 to r[0:1]               // same as "and" //
                 1. move r[1] to i1                 // same as "and" //
                 2. copy i2 to r[1:2]               // same as "and" //
                 3. move r[2] to i2                 // same as "and" //
                 4. collapse r[1] to r[2]           // same as "and" //
                 5. collapse r[0] to r[1]           // same as "and" //
                 6. move r[1] to r[0]               // same as "and" //
                 7. move r[2] to r[1]               // same as "and" //
                 8. add r[1] -> r[0]                // same as "and" //
                 9. collapse r[0] to r[1]
                 10. move r[1] to the zeroed output address
                 */

                // .0   // same as "and" //
                tempStr += move_to(tempIntVect[0]);
                tempStr += BFO.openBr;
                tempStr += BFO.minus;
                tempStr += move_to(tempReservedArea[0]);
                tempStr +=BFO.plus;
                tempStr += move_to(tempReservedArea[0]+1);
                tempStr += BFO.plus;
                tempStr += move_to(tempIntVect[0]);
                tempStr += BFO.closeBr;

                // .1   // same as "and" //
                tempStr += move_to(tempReservedArea[0]+1);
                tempStr += BFO.openBr;
                tempStr += BFO.minus;
                tempStr += move_to(tempIntVect[0]);
                tempStr += BFO.plus;
                tempStr += move_to(tempReservedArea[0]+1);
                tempStr += BFO.closeBr;

                // .2   // same as "and" //
                tempStr += move_to(tempIntVect[1]);
                tempStr += BFO.openBr;
                tempStr += BFO.minus;
                tempStr += move_to(tempReservedArea[0]+1);
                tempStr +=BFO.plus;
                tempStr += move_to(tempReservedArea[0]+2);
                tempStr += BFO.plus;
                tempStr += move_to(tempIntVect[1]);
                tempStr += BFO.closeBr;

                // .3   // same as "and" //
                tempStr += move_to(tempReservedArea[0]+2);
                tempStr += BFO.openBr;
                tempStr += BFO.minus;
                tempStr += move_to(tempIntVect[1]);
                tempStr += BFO.plus;
                tempStr += move_to(tempReservedArea[0]+2);
                tempStr += BFO.closeBr;

                // .4   // same as "and" //
                tempStr += move_to(tempReservedArea[0]+1);
                tempStr += CS.collapse_int;

                // .5   // same as "and" //
                tempStr += move_to(tempReservedArea[0]);
                tempStr += CS.collapse_int;

                // .6   // same as "and" //
                tempStr += move_to(tempReservedArea[0]+1);
                tempStr += BFO.openBr;
                tempStr += BFO.minus;
                tempStr += move_to(tempReservedArea[0]);
                tempStr += BFO.plus;
                tempStr += move_to(tempReservedArea[0]+1);
                tempStr += BFO.closeBr;

                // .7   // same as "and" //
                tempStr += move_to(tempReservedArea[0]+2);
                tempStr += BFO.openBr;
                tempStr += BFO.minus;
                tempStr += move_to(tempReservedArea[0]+1);
                tempStr += BFO.plus;
                tempStr += move_to(tempReservedArea[0]+2);
                tempStr += BFO.closeBr;

                // .8   // same as "and" //
                tempStr += move_to(tempReservedArea[0]+1);
                tempStr += BFO.openBr;
                tempStr += BFO.minus;
                tempStr += move_to(tempReservedArea[0]);
                tempStr += BFO.plus;
                tempStr += move_to(tempReservedArea[0]+1);
                tempStr += BFO.closeBr;

                // .9
                tempStr += move_to(tempReservedArea[0]);
                tempStr += CS.collapse_int;

                // .10
                tempStr += move_to(tempIntVect[2]);
                tempStr += CS.setToZero;

                tempStr += move_to(tempReservedArea[0]+1);
                tempStr += BFO.openBr;
                tempStr += BFO.minus;
                tempStr += move_to(tempIntVect[2]);
                tempStr += BFO.plus;
                tempStr += move_to(tempReservedArea[0]+1);
                tempStr += BFO.closeBr;

            } else if (logicMode == RW.RW_LM_not) {

                tempReservedArea = find_reserved(reserved, ORM.logic_not);
                // not mode

                tPtr+=3;
                get_cur_tok();
                tempIntVect.push_back(address_string_to_int(curTok));  // [1] = target

                if (reserved_overlap(reserved, {tempIntVect.back(), tempIntVect.back()})) {
                    
                    raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
                    "Using a reserved address is not recommended!",              
                    "... 'logic address not -> " + curTok + "' ..." 
                    );
                }

                // copy source to r[0:1]
                tempStr += move_to(tempIntVect[0]);
                tempStr += BFO.openBr;
                tempStr += BFO.minus;
                tempStr += move_to(tempReservedArea[0]);
                tempStr += BFO.plus;
                tempStr += move_to(tempReservedArea[0]+1);
                tempStr += BFO.plus;
                tempStr += move_to(tempIntVect[0]);
                tempStr += BFO.closeBr;

                // move r[1] back to source
                tempStr += move_to(tempReservedArea[0]+1);
                tempStr += BFO.openBr;
                tempStr += BFO.minus;
                tempStr += move_to(tempIntVect[0]);
                tempStr += BFO.plus;
                tempStr += move_to(tempReservedArea[0]+1);
                tempStr += BFO.closeBr;

                // do the not-magic
                tempStr += move_to(tempReservedArea[0]);
                tempStr += CS.logic_not;
                tempStr += move_to(tempIntVect[1]);
                tempStr += CS.setToZero;

                tempStr += move_to(tempReservedArea[0]+1);
                tempStr += BFO.openBr;
                tempStr += BFO.minus;
                tempStr += move_to(tempIntVect[1]);
                tempStr += BFO.plus;
                tempStr += move_to(tempReservedArea[0]+1);
                tempStr += BFO.closeBr;
                


            } else {

                raise_compiler_error(CompilerErrors::invalidLogicOperator,
                "Invalid operator for 'logic' statement.",
                "... 'logic ?address " + logicMode + "' ...");
            }

            out += tempStr;

        } else if (curTok == RW.RW_alias) {

            tPtr++;
            get_cur_tok();
            tempStr = curTok;

            tPtr+=2;
            get_cur_tok();

            for (int i = 0; i < Tokens.size(); i++) {
                if (Tokens[i] == curTok) {
                    Tokens[i] = tempStr;
                }
            }

        } else if (curTok == RW.RW_loads) {

            tempStr = "";
            tempInt = 0;
            tempIntVect.clear();

            tPtr++;
            get_cur_tok();

            tempInt = address_string_to_int(curTok);
            tempStr += move_to(tempInt);

            if (tempInt < 0 || tempInt >= memsize) {
                raise_compiler_error(CompilerErrors::invalidMemoryAddress,
                "Couldn't load into address because it is bigger than specified memory size ('memsize' instruction) or it is less than zero.", 
                "... 'loads ?"+std::to_string(tempInt)+"...");
            }

            if (reserved_overlap(reserved, {tempInt, tempInt})) {
                
                raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
                "Using a reserved address is not recommended!",              
                "... 'loads " + curTok + " ~ address : values' ..."
                );
            }

            tPtr+=2;
            get_cur_tok();
            tempInt = std::stoi(curTok);
            tPtr++;

            for (int i = 0; i < tempInt; i++) {

                tPtr++;
                get_cur_tok();
                tempIntVect.push_back(std::stoi(curTok));
            }

            for (int y: tempIntVect) {

                tempStr += CS.setToZero;
                tempStr += add_n_chars(y, '+');
                tempStr += move_to(ptrPosition+1);

                if ((y > 255) || (y < 0)) {
                    raise_compiler_error(CompilerErrors::invalidValueToLoad,
                    "Couldn't load value because it is either less than zero or greater than 255. Only unsigned 8-bit integers (0-255) may be loaded.",
                    "... 'loads "+curTok+"' ...");
                }

                if (reserved_overlap(reserved, {y, y})) {
                    
                    raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
                    "Using a reserved address is not recommended!",              
                    "... 'loads address ~ address : values' ...\nA loads statement is trying to write to address ?" + std::to_string(y)
                    );
                }

            }

            out += tempStr;
        
        } else if (curTok == RW.RW_inline) {

            out += " ";
            tPtr++;
            get_cur_tok();

            if (string_contains(curTok, BFO.allOps)) {

                raise_compiler_warning(CompilerWarnings::reservedInlineCharacter, 
                "Your inline statement could lead to errors because it contains one of the following characters: " + BFO.allOps,
                "... 'inline " + curTok + "' ..."
                );

            }

            out += curTok;
            out += " ";

            // raise warning if the thing contains "bad" characters. (+-<>[].;)

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