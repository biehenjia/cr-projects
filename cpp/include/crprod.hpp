#pragma once
#include "crobj.hpp"

class CRprod :public CRobj {
    public: 
        CRprod(size_t i, size_t l);
        
        CRobj* add(const CRobj& target) const override;
        CRobj* mul(const CRobj& target) const override;
        CRobj* pow(const CRobj& target) const override;

        //todo;
        CRobj* exp() const override;
        CRobj* ln() const override;

        CRobj* sin() const override;
        CRobj* cos() const override;

        void simplify() override;
        void shift(size_t index) override;

        CRprod* copy() const override;
        CRobj* correctp(size_t nl) const;
};

