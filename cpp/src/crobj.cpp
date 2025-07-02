#include "crobj.hpp"
#include <iostream>



CRobj::CRobj(size_t l){
    length = l;
    operands.resize(l,nullptr);
}

//recurse
CRobj::~CRobj(){
    for (CRobj* p : operands){
        delete p;
    }
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
    std::cout << "[initialize() called]\n";

    if (initialized) {
        return valueof();
    }

    initialized = true;
    fastvalues.resize(length,0);
    isnumbers.resize(length);

    for (size_t i = 0; i < length; i++){ 
        isnumbers[i] = operands[i]->isnumber();
        if (!isnumbers[i]){
            fastvalues[i] = operands[i]->initialize();
        }
    }
    
    for (size_t i = 0; i < length; i++){ 
        std::cout<< fastvalues[i] << " ";
    }
    std::cout<<"\n Those were the fastvalues\n";

    return fastvalues[0];
}



void CRobj::simplify(){
    for (size_t i = 0; i < length; i++){ 
        operands[i]->simplify();
    }
}


void CRobj::shift() {
    return;
}


