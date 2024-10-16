#include "common.hpp"

std::vector<int> range (int a, int b){
    
    std::vector<int> rangeVect;
    for (int i = a; i <= b; i++) {
        rangeVect.push_back(i);
    }

    return rangeVect;
}

bool stringContains (std::string master, std::string chars) {

    for (char mc: master) {

        for (char cc: chars) {

            if (cc == mc) {
                return true;
            }

        }

    }

    return false;

}