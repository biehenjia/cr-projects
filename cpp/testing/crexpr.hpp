#pragma once 
#include "crobj.hpp"

class CRexpr : public CRobj {
    public:
        CRexpr(oc ot, size_t l);
        CRexpr(oc ot, const CRobj& o1);
        CRexpr(oc ot, const CRobj& o1, const CRobj& o2);


        CRobj* add(const CRobj&) const override;
        CRobj* mul(const CRobj&) const override;
        CRobj* pow(const CRobj&) const override;

        CRobj* exp() const override;
        CRobj* ln() const override;
        CRobj* sin() const override; 
        CRobj* cos() const override;

        CRobj* copy() const override;

        double valueof() const override;
        void shift() override;

        oc optype;

};