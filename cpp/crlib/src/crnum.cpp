#include "crnum.hpp"
#include "crsum.hpp"

CRnum::CRnum(double v){
    value = v;
}

CRobj* CRnum::add(const CRobj& target) const { 

}

CRnum* CRnum::add(const CRnum& target) const {
    return new CRnum(this->value + target.value);
}

CRsum* CRnum::add(const CRsum& target) const {
    return target.add(*this);
}

CRnum* CRnum::copy() const{
    return new CRnum(this->value);
}

void CRnum::simplify() {
    //do nothing
}