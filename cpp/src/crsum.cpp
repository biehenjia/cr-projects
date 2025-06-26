#include "crsum.hpp"
#include "crnum.hpp"
#include "crexpr.hpp"
#include "crprod.hpp"


CRsum::CRsum(size_t l){
    length = l;
    operands.reserve(l);
}


CRsum::CRsum(double x, double h){ 
    length = 2;
    operands = {new CRnum(x), new CRnum(h)};
}


CRsum* CRsum::copy() const{
    auto result = new CRsum(length);
    for (size_t i = 0; i < length; i++ ){ 
        result->operands[i] = operands[i]->copy(); 
    }

    if (initialized){
        result->fastvalues.resize(length);
        result->isnumbers.resize(length);
    }

    for (size_t i = 0; i < length; i++){ 
        result->fastvalues[i] = fastvalues[i];
        result->isnumbers[i] = isnumbers[i];
    }
    return result;
}


CRsum* CRsum::add(const CRsum& target) const {

    if (length < target.length){
        auto result = new CRsum(target.length);
        for (size_t i = 0; i < length; i++){
            result->operands[i] = new CRnum(this->operands[i]->valueof() + target.operands[i]->valueof());
        }
        for (size_t i = length; i < target.length; i++){
            result->operands[i] = new CRnum(target.operands[i]->valueof());
        }
        result->simplify(); 
        return result;
    } else {
        auto result = new CRsum(length);
        for (size_t i = 0; i < target.length; i++){ 
            result->operands[i] = new CRnum(this->operands[i]->valueof() + target.operands[i]->valueof());
        }
        for (size_t i = target.length; i < length; i++){
            result->operands[i] = new CRnum(this->operands[i]->valueof());
        }
        result->simplify();
        return result;
    }

}


CRsum* CRsum::add(const CRnum& target) const { 
    auto result = this->copy();
    auto temp = new CRnum(result->operands[0]->valueof() + target.valueof());
    delete result->operands[0];
    result->operands[0] = temp;

    result->simplify();
    return result;
}


CRsum* CRsum::mul(const CRnum& target) const { 
    auto result = new CRsum(length);
    for (size_t i = 0; i < length; i++){ 
        result->operands[i] = new CRnum(operands[i]->valueof() * target.valueof());
    }
    result->simplify(); 
    return result;
}


CRsum* CRsum::mul(const CRsum& target) const {
    if (length >= target.length){
        

        double  newlength = length + target.length -1;

        auto result = new CRsum(newlength);
        double rtemp2,r1;
        
        //convolve
        double n = length - 1;
        double m = target.length -1;
        for (double i= 0; i < newlength; i++){
            double r1 = 0;
            double ibound11 = std::max(0.0,i-m);
            double ibound12 = std::min(i,n);
            for (double j = ibound11; j < ibound12+1; j++){
                double r2 = 0;
                double ibound21 = i-j;
                double ibound22 = std::min(i,m);
                for (double k = ibound21; k < ibound22+1; k++){
                    double rtemp1 = choose(j,i-k);
                    double rtemp11 = target.operands[i]->valueof(); 
                    rtemp1 *= rtemp11;
                    r2 += rtemp1;
                }
                double rtemp2 = choose(i,j);
                r2 *= rtemp2;
                double r = this->operands[i]->valueof();
                rtemp2 *= r;
                r1 += rtemp2;
            }
            result->operands[i] = new CRnum(r1);
        }
        size_t length = newlength;
        result->simplify();
        return result;
    } else { 
        auto result = target.mul(*this);
        result->simplify();
        return result;
    }
}


void CRsum::simplify() {
    size_t j = length - 1;
    while (j > 0 && operands[j]->valueof() == 0){
        delete operands[j];
        j--;
    }

    operands.resize(j+1);
}

void CRsum::shift() {
    for (size_t i = 0; i < length-1; i++){
        fastvalues[i] += fastvalues[i+1];
    }
}

CRobj* CRsum::add(const CRobj& target) const {
    return new CRexpr(oc::ADD, *this->copy(), *target.copy());
}


CRobj* CRsum::mul(const CRobj& target) const {
    return new CRexpr(oc::MUL, *this->copy(), *target.copy());
}

CRobj* CRsum::pow(const CRobj& target) const {
    return new CRexpr(oc::POW, *this->copy(), *target.copy());
}

CRobj* CRsum::exp() const {
    auto result = new CRprod(length);
    for (size_t i = 0; i< length; i++){ 
        result->operands[i] = new CRnum(std::exp(operands[i]->valueof()));
    }
    result->simplify();
    return result;
}

CRobj* CRsum::ln() const {
    return new CRexpr(oc::LN, *this->copy());
}

CRobj* CRsum::sin() const { 
    return new CRexpr(oc::SIN, *this->copy());
}

CRobj* CRsum::cos() const { 
    return new CRexpr(oc::COS, *this->copy());
}