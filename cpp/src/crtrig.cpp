#include "crtrig.hpp"
#include "crsum.hpp"
#include "crexpr.hpp"
#include "crnum.hpp"
#include "crprod.hpp"

CRtrig::CRtrig(size_t i, oc t, size_t l) {
    length = l;
    trigtype = t;
    operands.resize(length,nullptr);
    index = i;
}

std::unique_ptr< CRobj>CRtrig::copy() const {
    auto result = std::make_unique< CRtrig>(index, trigtype, length);
    for (size_t i = 0; i < length; i++){ 
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

std::unique_ptr<CRobj> CRtrig::add(const CRobj& target) const { 
    return std::make_unique< CRexpr>(oc::ADD, *this->copy(), *target.copy() );
}

std::unique_ptr<CRobj> CRtrig::pow(const CRobj& target) const{ 
    return std::make_unique< CRexpr>(oc::POW, *this->copy(), *target.copy());
}

std::unique_ptr<CRobj> CRtrig::mul(const CRobj& target) const { 
    if (index != target.index) { 
        if (trigtype == oc::SIN || trigtype == oc::COS){
            auto result = copy(); 

            if (operands[0]->index > target.index){ 
                result->operands[0] = operands[0]->mul(target);
            } else { 
                result->operands[0] = target.mul(*operands[0]);
            }
            if (operands[length/2]->index > target.index){ 
                result->operands[length/2]= operands[length/2]->mul(target);
            } else { 
                result->operands[length/2] = target.mul(*operands[0]);
            }
            result->simplify();
            return result;
        }
    } else if (auto p = dynamic_cast<const CRprod*> (&target)){ 
        auto result = std::make_unique< CRtrig>(index, trigtype,length);
        size_t L;
        std::vector<CRobj> o1, o2;
        std::unique_ptr<CRobj> c = nullptr;
        std::unique_ptr<CRobj> t = nullptr;
        if (length/2 > p->length){
            auto o1 = operands;
            c = p->correctp(length/2);
            auto o2 = c->operands;
            size_t L = length/2   ;

        } else if (length/2 < p->length){
            t = correctt(p->length);
            auto o1 = t->operands;
            auto o2 = p->operands;
            size_t L = p->length;

        } else {
            auto o1 = operands;
            auto o2 = p->operands;
            size_t L = length/2;
        }
        result->operands.resize(L*2);
        for (size_t i = 0; i < L; i++){
            if (o1[i].index > o2[i].index) {
                result->operands[i] = o1[i].mul(o2[i]);
            } else { 
                result->operands[i+L] = o1[i+L].mul(o2[i]);
            }  
        }
        result->length = 2*L; 
        result->simplify();
        return result;
    } else { 
        return std::make_unique< CRexpr>(oc::MUL,*this->copy(), *target.copy());

    }
}

std::unique_ptr<CRobj> CRtrig::correctt(size_t nl) const { 
    auto result = copy();
    result->operands.resize(nl * 2);
    for (size_t i = 0; i < length/2; i++){
        result->operands[i] = operands[i+length/2]->copy();
    }
    for (size_t i = length/2; i < nl*2; i++){
        result->operands[i] = std::make_unique< CRnum>(0.0);
        result->operands[i+nl] = std::make_unique< CRnum>(1.0);
    }
    return result;
}

std::unique_ptr<CRobj> CRtrig::correctt(size_t nl) const { 
    auto result = copy();
    result->operands.resize(nl * 2);
    for (size_t i = 0; i < length/2; i++){
        result->operands[i] = operands[i+length/2]->copy();
    }
    for (size_t i = length/2; i < nl*2; i++){
        result->operands[i] = std::make_unique< CRnum>(0.0);
        result->operands[i+nl] = std::make_unique< CRnum>(1.0);
    }
    return result;
}

double CRtrig::valueof() const {
    double result = 0;
    switch (trigtype) {
        case oc::SIN:
            result =  fastvalues[0];
            break;
        case oc::COS:
            result =  fastvalues[length/2];
            break;
        case oc::TAN:
            result = fastvalues[0]/fastvalues[length/2];
            break;
        case oc::COT:
            result = fastvalues[length/2]/fastvalues[0];
            break;
    }
    return result;
}

std::unique_ptr<CRobj> CRtrig::exp() const {
    return std::make_unique< CRexpr>(oc::EXP, *this->copy());
}

std::unique_ptr<CRobj> CRtrig::ln() const {
    return std::make_unique< CRexpr>(oc::LN, *this->copy());
}

std::unique_ptr<CRobj> CRtrig::sin() const {
    return std::make_unique< CRexpr>(oc::SIN, *this->copy());
}

std::unique_ptr<CRobj> CRtrig::cos() const {
    return std::make_unique< CRexpr>(oc::COS, *this->copy());
}

//todo
void CRtrig::simplify() {

}

void CRtrig::shift(size_t i){
    if (index != i){ 
        for (size_t i = 0; i < length; i++){ 
            if (isnumbers[i]){ 
                operands[i]->shift(i);
                fastvalues[i] = operands[i]->valueof();
            }
        }
    } else { 
        double r1, r2, r3, r4, z;
        size_t t = length/2;
        for (size_t i = 0; i < t-1; i++){ 
            r1 = fastvalues[i] * fastvalues[i+t+1];
            r2 = fastvalues[i+t] * fastvalues[i+1];
            z = r1 + r2;
            r3 = fastvalues[i+t] * fastvalues[i+t+1];
            r4 = fastvalues[i] * fastvalues[i+1];
            fastvalues[i+t] = r3-r4;
            fastvalues[i] = z;
        }
    }
    
}