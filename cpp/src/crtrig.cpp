#include "crtrig.hpp"
#include "crsum.hpp"
#include "crexpr.hpp"
#include "crnum.hpp"
#include "crprod.hpp"

CRtrig::CRtrig(oc t, size_t l) {
    length = l;
    trigtype = t;
    operands.resize(length,nullptr);
}


CRobj* CRtrig::copy() const {
    auto result = new CRtrig(trigtype, length);
    for (size_t i = 0; i < length; i++){ 
        result->operands[i] = operands[i]->copy();
    }

    // ignore initialized logic for now
    return result;
}

CRobj* CRtrig::add(const CRobj& target) const { 
    return new CRexpr(oc::ADD, *this->copy(), *target.copy() );
}

CRobj* CRtrig::pow(const CRobj& target) const{ 
    return new CRexpr(oc::POW, *this->copy(), *target.copy());
}


CRobj* CRtrig::mul(const CRobj& target) const { 
    CRobj* result;
    if (auto p = dynamic_cast<const CRnum*>(&target)){
        if (trigtype == oc::SIN || trigtype == oc::COS){
            auto result = copy(); 
            delete result->operands[0];
            result->operands[0] = new CRnum(p->valueof() * operands[0]->valueof());
            delete result->operands[length/2];
            result->operands[length/2] = new CRnum(p->valueof() * operands[length/2]->valueof());
        } else { 
            return new CRexpr(oc::MUL, *this->copy(), *target.copy());
        }
        
    } else if (auto p = dynamic_cast<const CRprod*> (&target)){ 
        auto result = new CRtrig(trigtype,length);
        size_t L;
        std::vector<CRobj> o1, o2;
        CRobj* c = nullptr;
        CRobj* t = nullptr;
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
            result->operands[i] = o1[i].mul(o2[i]);
            result->operands[i+L] = o1[i+L].mul(o2[i]);
        }
        if (c) delete c;
        if (t) delete t;
        result->length = 2*L; 

    }
    return result;
}


CRobj* CRtrig::correctt(size_t nl) const { 
    auto result = copy();
    result->operands.resize(nl * 2);
    for (size_t i = 0; i < length/2; i++){
        result->operands[i] = operands[i+length/2]->copy();
    }
    for (size_t i = length/2; i < nl*2; i++){
        result->operands[i] = new CRnum(0.0);
        result->operands[i+nl] = new CRnum(1.0);
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


CRobj* CRtrig::exp() const {
    return new CRexpr(oc::EXP, *this->copy());
}

CRobj* CRtrig::ln() const {
    return new CRexpr(oc::LN, *this->copy());
}

CRobj* CRtrig::sin() const {
    return new CRexpr(oc::SIN, *this->copy());
}

CRobj* CRtrig::cos() const {
    return new CRexpr(oc::COS, *this->copy());
}

//todo
void CRtrig::simplify() {

}

void CRtrig::shift(){
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