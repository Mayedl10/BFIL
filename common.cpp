#include "common.hpp"

std::vector<int> range (int a, int b){
    std::vector<int> rangeVect;
    for (int i = a; i <= b; i++) {
        rangeVect.push_back(i);
    }

    return rangeVect;
}