#include "crobj.hpp"
#include <iostream>

CRobj::CRobj(size_t l){
    length = l;
    operands.resize(l);
}

double CRobj::valueof() const{
    if (initialized){ 
        return fastvalues[0];
    }
    return 0;
}

bool CRobj::isnumber() const{
    return false;
}

double CRobj::initialize() {

    if (initialized) {
        return valueof();
    }
    // std::cout<<"new initialize\n";
    initialized = true;
    fastvalues.resize(length,0);
    isnumbers.resize(length, false);

    for (size_t i = 0; i < length; i++){ 
        isnumbers[i] = operands[i]->isnumber();
        fastvalues[i] = operands[i]->initialize();

    }
    // for (size_t i = 0; i <length; i++ ){ 
    //     std::cout<<fastvalues[i]<<" ";
    // }
    // std::cout<<" initialized values\n";
    return fastvalues[0];
}

void CRobj::simplify(){
    for (size_t i = 0; i < length; i++){ 
        operands[i]->simplify();
    }
}

void CRobj::shift(size_t index) {
    return;
}


