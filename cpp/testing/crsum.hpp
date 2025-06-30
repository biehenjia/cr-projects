#pragma once
#include "crobj.hpp"


class CRsum : public CRobj {
    public: 
        CRsum(size_t l); 
        CRsum(double x, double h);
        
        CRobj* add(const CRobj& target) const override;
        CRobj* mul(const CRobj& target) const override;

        // handle negative power in the visitor
        CRobj* pow(const CRobj& target) const  override;
        //covariant
        CRobj* exp() const override;
        CRobj* ln() const override;

        CRobj* sin() const override;
        CRobj* cos() const override;

        void simplify() override;
        CRsum* copy() const override;
        void shift() override;

};