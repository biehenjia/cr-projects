#include "crobj.hpp"

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
    return fastvalues[0];
}





