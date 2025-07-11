#include "crsum.hpp"
#include "crnum.hpp"
#include "crexpr.hpp"
#include "crprod.hpp"
#include "crtrig.hpp"


CRsum:: CRsum(size_t i, size_t l){
    index = i;
    length = l;
    operands.resize(length, nullptr); 
}

CRsum::CRsum(size_t i, double x, double h){ 
    length = 2; 
    index =i;
    operands = {new CRnum(x), new CRnum(h)};
}

CRsum* CRsum::copy() const { 
    auto result = new CRsum(index, length);
    for (size_t i = 0; i < length; i++ ){ 
        result->operands[i] = operands[i]->copy(); 
    }
    if (initialized){
        result->initialized = true;
        result->fastvalues.resize(length);
        result->isnumbers.resize(length);
        for (size_t i = 0; i < length; i++){ 
            result->fastvalues[i] = fastvalues[i];
            result->isnumbers[i] = isnumbers[i];
        }
    }
    return result;
}

// always assume invariant 
CRobj* CRsum::add(const CRobj& target) const { 

    

    if (target.index != index){
        auto result = copy();
        CRobj* temp = nullptr;
        if (operands[0]->index > target.index) {
            auto temp = operands[0]->add(target);
        } else { 
            auto temp = operands[0]->add(target);
        }
        delete result->operands[0];
        result->operands[0] = temp;
        result->simplify();
        return result;
    } else if (auto p = dynamic_cast<const CRsum*>(&target)) {
        size_t maxLen = std::max(length, p->length);
        auto result = new CRsum(index, maxLen);
        for (size_t i = 0; i < maxLen; ++i) {
            double a = (i < length) ? this->operands[i]->valueof(): 0.0;
            double b = (i < target.length) ? p->operands[i]->valueof() : 0.0;
            result->operands[i] = new CRnum(a + b);
        }
        result->simplify();
        return result;
    } else { 
        return new CRexpr(oc::ADD, *this->copy(), *target.copy());
    }
}

CRobj* CRsum::mul(const CRobj& target) const { 

    
    if (target.index != index){
        auto result = copy();
        CRobj* temp = nullptr;
        for (size_t i = 0; i< length; i++){
            if (operands[0]->index > target.index) {
                temp = operands[0]->mul(target);
            } else { 
                temp = operands[0]->mul(target);
            }
            delete result->operands[0];
            result->operands[0] = temp;
        }
        result->simplify();
        return result;
    } else if (auto p = dynamic_cast<const CRsum*>(&target)) {

        if (length >= target.length){
            size_t  newlength = length + target.length -1;
            auto result = new CRsum(index, newlength);
            double rtemp2,r1;
            size_t n = length - 1;
            size_t m = target.length -1;
            for (size_t i= 0; i < newlength; i++){
                double r1 = 0;
                size_t ibound11 = (i > m ? i - m : 0);
                size_t ibound12 = std::min(i, n);
                for (size_t j = ibound11; j < ibound12+1; j++){
                    double r2 = 0;
                    size_t ibound21 = (i > j  ? i - j : 0);
                    size_t ibound22 = std::min(i,m);
                    for (size_t k = ibound21; k < ibound22+1; k++){
                        double rtemp1 = choose(j,i-k);
                        double rtemp11 = target.operands[k]->valueof(); 
                        rtemp1 *= rtemp11;
                        r2 += rtemp1;
                    }
                    double rtemp2 = choose(i, j);  
                    r2 *= rtemp2;
                    r2 *= this->operands[j]->valueof(); 
                    r1 += r2;   
                }
                result->operands[i] = new CRnum(r1);
                
            }
            result->length = newlength;
            result->simplify();
            return result;

        } else { 
            return target.mul(*this);
        }
    } else { 
        return new CRexpr(oc::MUL, *this->copy(), *target.copy());
    }
}

CRobj* CRsum::pow(const CRobj& target) const { 
    
    if (auto p = dynamic_cast<const CRnum*>(&target)) {
        auto result = copy();
        double pv = p->valueof();
        if (pv >= 0 && std::floor(pv) == pv) {
            size_t exp = size_t(pv);
            CRobj* result = new CRnum(1.0); 
            CRobj* base = copy(); 
            while (exp > 0) {
                if (exp & 1) {
                    CRobj* tmp = result->mul(*base);
                    delete result;
                    result = tmp;
                }
                exp >>= 1;
                if (exp) {
                    CRobj* tmp = base->mul(*base);
                    delete base;
                    base = tmp;
                }  
            }
            delete base;
            return result;
        }
    } else {
        return new CRexpr(oc::POW, *this->copy(), *target.copy());
    }
}

CRobj* CRsum::exp() const {
    auto result = new CRprod(index, length);
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
    auto result = new CRtrig(index, oc::SIN, length* 2);
    for (size_t i = 0; i < length; i++){ 
        result->operands[i] = new CRnum(std::sin(operands[i]->valueof()));
        result->operands[i+length] = new CRnum(std::cos(operands[i]->valueof()));
    }
    return result;
}

CRobj* CRsum::cos() const { 
    auto result = new CRtrig(index, oc::COS, length* 2);
    for (size_t i = 0; i < length; i++){ 
        result->operands[i] = new CRnum(std::sin(operands[i]->valueof()));
        result->operands[i+length] = new CRnum(std::cos(operands[i]->valueof()));
    }
    return result;
}

void CRsum::shift(size_t i ) {
    if (index > i){
        for (size_t i = 0; i < length; i++){ 
            if (isnumbers[i]){ 
                operands[i]->shift(i);
                fastvalues[i] = operands[i]->valueof();
            }
        }
    } else {
        for (size_t i = 0; i < length-1; i++){
            fastvalues[i] += fastvalues[i+1];
        }
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

