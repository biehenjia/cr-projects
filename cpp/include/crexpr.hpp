#pragma once 
#include "crobj.hpp"

class CRexpr : public CRobj {
    public:

        CRexpr(oc ot, size_t length);
        CRexpr( oc ot, const CRobj& o1);
        CRexpr(oc ot, const CRobj& o1, const CRobj& o2);


        std::unique_ptr<CRobj> add(const CRobj&) const override;
        std::unique_ptr<CRobj> mul(const CRobj&) const override;
        std::unique_ptr<CRobj> pow(const CRobj&) const override;

        std::unique_ptr<CRobj> exp() const override;
        std::unique_ptr<CRobj> ln() const override;
        std::unique_ptr<CRobj> sin() const override; 
        std::unique_ptr<CRobj> cos() const override;

        std::unique_ptr<CRobj> copy() const override;

        double valueof() const override;
        void shift(size_t index) override;

        oc optype;

};